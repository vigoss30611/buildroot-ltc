
/home/warits/src/coding/irom_dev/release-sa5/out/vmirom:     file format elf32-littlearm


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
      44:	3c00a038 	.word	0x3c00a038

00000048 <_bss_end>:
      48:	3c00aca0 	.word	0x3c00aca0

0000004c <__stack>:
      4c:	3c009fe0 	.word	0x3c009fe0

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
      78:	eb000470 	bl	1240 <chip_test>
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
      9c:	00000758 	.word	0x00000758

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

00000150 <init_data_section>:
     150:	e59f1010 	ldr	r1, [pc, #16]	; 168 <init_data_section+0x18>
     154:	e59f0010 	ldr	r0, [pc, #16]	; 16c <init_data_section+0x1c>
     158:	e1510000 	cmp	r1, r0
     15c:	012fff1e 	bxeq	lr
     160:	e59f2008 	ldr	r2, [pc, #8]	; 170 <init_data_section+0x20>
     164:	ea0004be 	b	1464 <memcpy>
     168:	00019bec 	.word	0x00019bec
     16c:	3c00a000 	.word	0x3c00a000
     170:	00000038 	.word	0x00000038

00000174 <bl_get_binary>:
     174:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     178:	e1a0b000 	mov	fp, r0
     17c:	e24dd014 	sub	sp, sp, #20	; 0x14
     180:	e1a00001 	mov	r0, r1
     184:	e59f1110 	ldr	r1, [pc, #272]	; 29c <bl_get_binary+0x128>
     188:	e1a07002 	mov	r7, r2
     18c:	e1a0a003 	mov	sl, r3
     190:	eb001783 	bl	5fa4 <vs_is_device>
     194:	e2504000 	subs	r4, r0, #0	; 0x0
     198:	1a00000c 	bne	1d0 <bl_get_binary+0x5c>
     19c:	e897000c 	ldm	r7, {r2, r3}
     1a0:	e1a0000b 	mov	r0, fp
     1a4:	e58da000 	str	sl, [sp]
     1a8:	e58d4004 	str	r4, [sp, #4]
     1ac:	eb001795 	bl	6008 <vs_read>
     1b0:	e150000a 	cmp	r0, sl
     1b4:	1a000035 	bne	290 <bl_get_binary+0x11c>
     1b8:	e897000c 	ldm	r7, {r2, r3}
     1bc:	e0922000 	adds	r2, r2, r0
     1c0:	e0a33fc0 	adc	r3, r3, r0, asr #31
     1c4:	e887000c 	stm	r7, {r2, r3}
     1c8:	e1a00004 	mov	r0, r4
     1cc:	ea000030 	b	294 <bl_get_binary+0x120>
     1d0:	e3a00000 	mov	r0, #0	; 0x0
     1d4:	eb002b4b 	bl	af08 <nand_get_config>
     1d8:	e2509000 	subs	r9, r0, #0	; 0x0
     1dc:	03e00033 	mvneq	r0, #51	; 0x33
     1e0:	0a00002b 	beq	294 <bl_get_binary+0x120>
     1e4:	e3a08000 	mov	r8, #0	; 0x0
     1e8:	e1a06008 	mov	r6, r8
     1ec:	e5991014 	ldr	r1, [r9, #20]
     1f0:	e897000c 	ldm	r7, {r2, r3}
     1f4:	e08b0006 	add	r0, fp, r6
     1f8:	e58d1000 	str	r1, [sp]
     1fc:	e3a01000 	mov	r1, #0	; 0x0
     200:	e58d1004 	str	r1, [sp, #4]
     204:	eb00177f 	bl	6008 <vs_read>
     208:	e5993014 	ldr	r3, [r9, #20]
     20c:	e1a01000 	mov	r1, r0
     210:	e1510003 	cmp	r1, r3
     214:	e59f0084 	ldr	r0, [pc, #132]	; 2a0 <bl_get_binary+0x12c>
     218:	e897000c 	ldm	r7, {r2, r3}
     21c:	e0816006 	add	r6, r1, r6
     220:	e1cd20f8 	strd	r2, [sp, #8]
     224:	1a000005 	bne	240 <bl_get_binary+0xcc>
     228:	e1a04002 	mov	r4, r2
     22c:	e0922001 	adds	r2, r2, r1
     230:	e1a05003 	mov	r5, r3
     234:	e2a33000 	adc	r3, r3, #0	; 0x0
     238:	e887000c 	stm	r7, {r2, r3}
     23c:	ea00000e 	b	27c <bl_get_binary+0x108>
     240:	e3a04702 	mov	r4, #524288	; 0x80000
     244:	e1cd20d8 	ldrd	r2, [sp, #8]
     248:	e3a05000 	mov	r5, #0	; 0x0
     24c:	e0922004 	adds	r2, r2, r4
     250:	e59f404c 	ldr	r4, [pc, #76]	; 2a4 <bl_get_binary+0x130>
     254:	e0a33005 	adc	r3, r3, r5
     258:	e3e05000 	mvn	r5, #0	; 0x0
     25c:	e0033005 	and	r3, r3, r5
     260:	e0022004 	and	r2, r2, r4
     264:	e887000c 	stm	r7, {r2, r3}
     268:	e59f5038 	ldr	r5, [pc, #56]	; 2a8 <bl_get_binary+0x134>
     26c:	e1a0e00f 	mov	lr, pc
     270:	e595f000 	ldr	pc, [r5]
     274:	e2888001 	add	r8, r8, #1	; 0x1
     278:	e3a06000 	mov	r6, #0	; 0x0
     27c:	e156000a 	cmp	r6, sl
     280:	a3a00000 	movge	r0, #0	; 0x0
     284:	aa000002 	bge	294 <bl_get_binary+0x120>
     288:	e3580003 	cmp	r8, #3	; 0x3
     28c:	daffffd6 	ble	1ec <bl_get_binary+0x78>
     290:	e3e00001 	mvn	r0, #1	; 0x1
     294:	e28dd014 	add	sp, sp, #20	; 0x14
     298:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     29c:	000166d1 	.word	0x000166d1
     2a0:	000166d5 	.word	0x000166d5
     2a4:	fff80000 	.word	0xfff80000
     2a8:	3c00a5cc 	.word	0x3c00a5cc

000002ac <bl_check_data>:
     2ac:	e92d4030 	push	{r4, r5, lr}
     2b0:	e5903004 	ldr	r3, [r0, #4]
     2b4:	e24dd004 	sub	sp, sp, #4	; 0x4
     2b8:	e1b03e23 	lsrs	r3, r3, #28
     2bc:	e1a04000 	mov	r4, r0
     2c0:	e1a05001 	mov	r5, r1
     2c4:	0a000002 	beq	2d4 <bl_check_data+0x28>
     2c8:	e1a00001 	mov	r0, r1
     2cc:	e594200c 	ldr	r2, [r4, #12]
     2d0:	eb00186f 	bl	6494 <ss_aes_data>
     2d4:	e1a00004 	mov	r0, r4
     2d8:	e1a01005 	mov	r1, r5
     2dc:	e28dd004 	add	sp, sp, #4	; 0x4
     2e0:	e8bd4030 	pop	{r4, r5, lr}
     2e4:	ea0005f0 	b	1aac <isi_check_data_separate>

000002e8 <bl_check_header>:
     2e8:	e92d4010 	push	{r4, lr}
     2ec:	e1a04000 	mov	r4, r0
     2f0:	eb0005c3 	bl	1a04 <isi_check_header>
     2f4:	e3500000 	cmp	r0, #0	; 0x0
     2f8:	08bd8010 	popeq	{r4, pc}
     2fc:	e59f3034 	ldr	r3, [pc, #52]	; 338 <bl_check_header+0x50>
     300:	e59f0034 	ldr	r0, [pc, #52]	; 33c <bl_check_header+0x54>
     304:	e1a0e00f 	mov	lr, pc
     308:	e593f000 	ldr	pc, [r3]
     30c:	e3a00000 	mov	r0, #0	; 0x0
     310:	e59f2028 	ldr	r2, [pc, #40]	; 340 <bl_check_header+0x58>
     314:	e1a01000 	mov	r1, r0
     318:	eb00186e 	bl	64d8 <ss_aes_init>
     31c:	e1a00004 	mov	r0, r4
     320:	e1a01004 	mov	r1, r4
     324:	e3a02c02 	mov	r2, #512	; 0x200
     328:	eb001859 	bl	6494 <ss_aes_data>
     32c:	e1a00004 	mov	r0, r4
     330:	e8bd4010 	pop	{r4, lr}
     334:	ea0005b2 	b	1a04 <isi_check_header>
     338:	3c00a5cc 	.word	0x3c00a5cc
     33c:	000166f1 	.word	0x000166f1
     340:	3c00a004 	.word	0x3c00a004

00000344 <bl_load_isi>:
     344:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     348:	e1a07001 	mov	r7, r1
     34c:	e24ddc02 	sub	sp, sp, #512	; 0x200
     350:	e1a04000 	mov	r4, r0
     354:	e3a01001 	mov	r1, #1	; 0x1
     358:	e1a00007 	mov	r0, r7
     35c:	e1a08002 	mov	r8, r2
     360:	e1a06003 	mov	r6, r3
     364:	eb0017fa 	bl	6354 <vs_assign_by_id>
     368:	e2505000 	subs	r5, r0, #0	; 0x0
     36c:	0a000008 	beq	394 <bl_load_isi+0x50>
     370:	e59f315c 	ldr	r3, [pc, #348]	; 4d4 <bl_load_isi+0x190>
     374:	e1a00007 	mov	r0, r7
     378:	e5934000 	ldr	r4, [r3]
     37c:	eb0016e1 	bl	5f08 <vs_device_name>
     380:	e1a02005 	mov	r2, r5
     384:	e1a01000 	mov	r1, r0
     388:	e59f0148 	ldr	r0, [pc, #328]	; 4d8 <bl_load_isi+0x194>
     38c:	e12fff34 	blx	r4
     390:	ea00004c 	b	4c8 <bl_load_isi+0x184>
     394:	e1a00004 	mov	r0, r4
     398:	e1a01007 	mov	r1, r7
     39c:	e1a02008 	mov	r2, r8
     3a0:	e3a03b01 	mov	r3, #1024	; 0x400
     3a4:	ebffff72 	bl	174 <bl_get_binary>
     3a8:	e2505000 	subs	r5, r0, #0	; 0x0
     3ac:	1a00003c 	bne	4a4 <bl_load_isi+0x160>
     3b0:	e1a00004 	mov	r0, r4
     3b4:	ebffffcb 	bl	2e8 <bl_check_header>
     3b8:	e2505000 	subs	r5, r0, #0	; 0x0
     3bc:	0a000005 	beq	3d8 <bl_load_isi+0x94>
     3c0:	e59f0114 	ldr	r0, [pc, #276]	; 4dc <bl_load_isi+0x198>
     3c4:	e1a01005 	mov	r1, r5
     3c8:	e59f3104 	ldr	r3, [pc, #260]	; 4d4 <bl_load_isi+0x190>
     3cc:	e1a0e00f 	mov	lr, pc
     3d0:	e593f000 	ldr	pc, [r3]
     3d4:	ea00003b 	b	4c8 <bl_load_isi+0x184>
     3d8:	e5943004 	ldr	r3, [r4, #4]
     3dc:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
     3e0:	e2433002 	sub	r3, r3, #2	; 0x2
     3e4:	e3530001 	cmp	r3, #1	; 0x1
     3e8:	9a000006 	bls	408 <bl_load_isi+0xc4>
     3ec:	e1a01005 	mov	r1, r5
     3f0:	e59f00e8 	ldr	r0, [pc, #232]	; 4e0 <bl_load_isi+0x19c>
     3f4:	e59f30d8 	ldr	r3, [pc, #216]	; 4d4 <bl_load_isi+0x190>
     3f8:	e1a0e00f 	mov	lr, pc
     3fc:	e593f000 	ldr	pc, [r3]
     400:	e3e0500b 	mvn	r5, #11	; 0xb
     404:	ea00002f 	b	4c8 <bl_load_isi+0x184>
     408:	e594300c 	ldr	r3, [r4, #12]
     40c:	e3560000 	cmp	r6, #0	; 0x0
     410:	e2435c02 	sub	r5, r3, #512	; 0x200
     414:	0a000005 	beq	430 <bl_load_isi+0xec>
     418:	e3550000 	cmp	r5, #0	; 0x0
     41c:	d1a00004 	movle	r0, r4
     420:	c3e0500c 	mvngt	r5, #12	; 0xc
     424:	d2801c02 	addle	r1, r0, #512	; 0x200
     428:	ca000026 	bgt	4c8 <bl_load_isi+0x184>
     42c:	ea000023 	b	4c0 <bl_load_isi+0x17c>
     430:	e5946010 	ldr	r6, [r4, #16]
     434:	e3560000 	cmp	r6, #0	; 0x0
     438:	aa000008 	bge	460 <bl_load_isi+0x11c>
     43c:	eb001b81 	bl	7248 <cdbg_dram_enabled>
     440:	e3500000 	cmp	r0, #0	; 0x0
     444:	1a000005 	bne	460 <bl_load_isi+0x11c>
     448:	e59f0094 	ldr	r0, [pc, #148]	; 4e4 <bl_load_isi+0x1a0>
     44c:	e59f3080 	ldr	r3, [pc, #128]	; 4d4 <bl_load_isi+0x190>
     450:	e1a0e00f 	mov	lr, pc
     454:	e593f000 	ldr	pc, [r3]
     458:	e3e05014 	mvn	r5, #20	; 0x14
     45c:	ea000019 	b	4c8 <bl_load_isi+0x184>
     460:	e1a01004 	mov	r1, r4
     464:	e1a0000d 	mov	r0, sp
     468:	e3a02c02 	mov	r2, #512	; 0x200
     46c:	eb0003fc 	bl	1464 <memcpy>
     470:	e3550000 	cmp	r5, #0	; 0x0
     474:	e2841c02 	add	r1, r4, #512	; 0x200
     478:	da00000b 	ble	4ac <bl_load_isi+0x168>
     47c:	e3a02c02 	mov	r2, #512	; 0x200
     480:	e1a00006 	mov	r0, r6
     484:	eb0003ff 	bl	1488 <memmove>
     488:	e1a03005 	mov	r3, r5
     48c:	e1a01007 	mov	r1, r7
     490:	e1a02008 	mov	r2, r8
     494:	e2860c02 	add	r0, r6, #512	; 0x200
     498:	ebffff35 	bl	174 <bl_get_binary>
     49c:	e2505000 	subs	r5, r0, #0	; 0x0
     4a0:	0a000004 	beq	4b8 <bl_load_isi+0x174>
     4a4:	e59f003c 	ldr	r0, [pc, #60]	; 4e8 <bl_load_isi+0x1a4>
     4a8:	eaffffc5 	b	3c4 <bl_load_isi+0x80>
     4ac:	e594200c 	ldr	r2, [r4, #12]
     4b0:	e1a00006 	mov	r0, r6
     4b4:	eb0003f3 	bl	1488 <memmove>
     4b8:	e1a01006 	mov	r1, r6
     4bc:	e1a0000d 	mov	r0, sp
     4c0:	ebffff79 	bl	2ac <bl_check_data>
     4c4:	e1a05000 	mov	r5, r0
     4c8:	e1a00005 	mov	r0, r5
     4cc:	e28ddc02 	add	sp, sp, #512	; 0x200
     4d0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
     4d4:	3c00a5cc 	.word	0x3c00a5cc
     4d8:	000166fe 	.word	0x000166fe
     4dc:	00016742 	.word	0x00016742
     4e0:	0001675f 	.word	0x0001675f
     4e4:	0001677a 	.word	0x0001677a
     4e8:	00016722 	.word	0x00016722

000004ec <try_get_bl>:
     4ec:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     4f0:	e3a02602 	mov	r2, #2097152	; 0x200000
     4f4:	e24dd00c 	sub	sp, sp, #12	; 0xc
     4f8:	e3a03000 	mov	r3, #0	; 0x0
     4fc:	e88d000c 	stm	sp, {r2, r3}
     500:	e1a07001 	mov	r7, r1
     504:	e1a06000 	mov	r6, r0
     508:	eb0000c2 	bl	818 <boot_device>
     50c:	e1a04000 	mov	r4, r0
     510:	eb001655 	bl	5e6c <vs_device_bootable>
     514:	e2505000 	subs	r5, r0, #0	; 0x0
     518:	1a000008 	bne	540 <try_get_bl+0x54>
     51c:	e59f308c 	ldr	r3, [pc, #140]	; 5b0 <try_get_bl+0xc4>
     520:	e1a00004 	mov	r0, r4
     524:	e5934000 	ldr	r4, [r3]
     528:	eb001676 	bl	5f08 <vs_device_name>
     52c:	e1a01000 	mov	r1, r0
     530:	e59f007c 	ldr	r0, [pc, #124]	; 5b4 <try_get_bl+0xc8>
     534:	e12fff34 	blx	r4
     538:	e1a00005 	mov	r0, r5
     53c:	ea000019 	b	5a8 <try_get_bl+0xbc>
     540:	e1a00004 	mov	r0, r4
     544:	eb001665 	bl	5ee0 <vs_device_boot0>
     548:	e3500000 	cmp	r0, #0	; 0x0
     54c:	13a02000 	movne	r2, #0	; 0x0
     550:	13a03000 	movne	r3, #0	; 0x0
     554:	188d000c 	stmne	sp, {r2, r3}
     558:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
     55c:	e1a01004 	mov	r1, r4
     560:	e1a0200d 	mov	r2, sp
     564:	e3a03000 	mov	r3, #0	; 0x0
     568:	ebffff75 	bl	344 <bl_load_isi>
     56c:	e3500000 	cmp	r0, #0	; 0x0
     570:	13a00000 	movne	r0, #0	; 0x0
     574:	1a00000b 	bne	5a8 <try_get_bl+0xbc>
     578:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     57c:	e5932008 	ldr	r2, [r3, #8]
     580:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     584:	e5862000 	str	r2, [r6]
     588:	e5874000 	str	r4, [r7]
     58c:	e5933008 	ldr	r3, [r3, #8]
     590:	e3530000 	cmp	r3, #0	; 0x0
     594:	aa000002 	bge	5a4 <try_get_bl+0xb8>
     598:	e3a00001 	mov	r0, #1	; 0x1
     59c:	e59f1014 	ldr	r1, [pc, #20]	; 5b8 <try_get_bl+0xcc>
     5a0:	eb001b3e 	bl	72a0 <cdbg_verify_burn_enable>
     5a4:	e3a00001 	mov	r0, #1	; 0x1
     5a8:	e28dd00c 	add	sp, sp, #12	; 0xc
     5ac:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     5b0:	3c00a5cc 	.word	0x3c00a5cc
     5b4:	0001679d 	.word	0x0001679d
     5b8:	3c000100 	.word	0x3c000100

000005bc <try_boot>:
     5bc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     5c0:	e3a03000 	mov	r3, #0	; 0x0
     5c4:	e24dd010 	sub	sp, sp, #16	; 0x10
     5c8:	e1a07000 	mov	r7, r0
     5cc:	e1a00003 	mov	r0, r3
     5d0:	e58d300c 	str	r3, [sp, #12]
     5d4:	e58d3008 	str	r3, [sp, #8]
     5d8:	e1a04003 	mov	r4, r3
     5dc:	eb000074 	bl	7b4 <set_xom>
     5e0:	eb000079 	bl	7cc <boot_info>
     5e4:	e28d800c 	add	r8, sp, #12	; 0xc
     5e8:	e28d6008 	add	r6, sp, #8	; 0x8
     5ec:	e1a05000 	mov	r5, r0
     5f0:	ea000008 	b	618 <try_boot+0x5c>
     5f4:	eb000281 	bl	1000 <gtc_time>
     5f8:	e1a00004 	mov	r0, r4
     5fc:	eb00006c 	bl	7b4 <set_xom>
     600:	e1a00008 	mov	r0, r8
     604:	e1a01006 	mov	r1, r6
     608:	ebffffb7 	bl	4ec <try_get_bl>
     60c:	e3500000 	cmp	r0, #0	; 0x0
     610:	13a04001 	movne	r4, #1	; 0x1
     614:	1a000003 	bne	628 <try_boot+0x6c>
     618:	e1540005 	cmp	r4, r5
     61c:	e2844001 	add	r4, r4, #1	; 0x1
     620:	bafffff3 	blt	5f4 <try_boot+0x38>
     624:	e3a04000 	mov	r4, #0	; 0x0
     628:	eb000274 	bl	1000 <gtc_time>
     62c:	e3a00001 	mov	r0, #1	; 0x1
     630:	eb000288 	bl	1058 <ecfg_check_flag>
     634:	e3500000 	cmp	r0, #0	; 0x0
     638:	1a00000c 	bne	670 <try_boot+0xb4>
     63c:	e3570000 	cmp	r7, #0	; 0x0
     640:	1a00000a 	bne	670 <try_boot+0xb4>
     644:	eb00023f 	bl	f48 <get_reset_stats>
     648:	e3500000 	cmp	r0, #0	; 0x0
     64c:	13500002 	cmpne	r0, #2	; 0x2
     650:	1a000006 	bne	670 <try_boot+0xb4>
     654:	e3500002 	cmp	r0, #2	; 0x2
     658:	1a000001 	bne	664 <try_boot+0xa8>
     65c:	e28d000c 	add	r0, sp, #12	; 0xc
     660:	eb000245 	bl	f7c <update_mask_on_reset>
     664:	e59d000c 	ldr	r0, [sp, #12]
     668:	eb001dee 	bl	7e28 <burn>
     66c:	eb000263 	bl	1000 <gtc_time>
     670:	e3540000 	cmp	r4, #0	; 0x0
     674:	0a000021 	beq	700 <try_boot+0x144>
     678:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     67c:	e5933004 	ldr	r3, [r3, #4]
     680:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
     684:	e3530002 	cmp	r3, #2	; 0x2
     688:	1a000016 	bne	6e8 <try_boot+0x12c>
     68c:	e3a01001 	mov	r1, #1	; 0x1
     690:	e59f0070 	ldr	r0, [pc, #112]	; 708 <try_boot+0x14c>
     694:	eb001b69 	bl	7440 <cdbg_do_config>
     698:	eb00022a 	bl	f48 <get_reset_stats>
     69c:	e3500001 	cmp	r0, #1	; 0x1
     6a0:	1a000000 	bne	6a8 <try_boot+0xec>
     6a4:	eb001b26 	bl	7344 <cdbg_restore_os>
     6a8:	e28d0008 	add	r0, sp, #8	; 0x8
     6ac:	e1a0100d 	mov	r1, sp
     6b0:	eb001aef 	bl	7274 <cdbg_boot_redirect>
     6b4:	e1a0200d 	mov	r2, sp
     6b8:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
     6bc:	e59d1008 	ldr	r1, [sp, #8]
     6c0:	e3a03000 	mov	r3, #0	; 0x0
     6c4:	ebffff1e 	bl	344 <bl_load_isi>
     6c8:	e3500000 	cmp	r0, #0	; 0x0
     6cc:	0a000005 	beq	6e8 <try_boot+0x12c>
     6d0:	e1a01000 	mov	r1, r0
     6d4:	e59f3030 	ldr	r3, [pc, #48]	; 70c <try_boot+0x150>
     6d8:	e59f0030 	ldr	r0, [pc, #48]	; 710 <try_boot+0x154>
     6dc:	e1a0e00f 	mov	lr, pc
     6e0:	e593f000 	ldr	pc, [r3]
     6e4:	ea000005 	b	700 <try_boot+0x144>
     6e8:	eb000244 	bl	1000 <gtc_time>
     6ec:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     6f0:	e5930014 	ldr	r0, [r3, #20]
     6f4:	e59d1008 	ldr	r1, [sp, #8]
     6f8:	eb001af3 	bl	72cc <cdbg_jump>
     6fc:	eafffffe 	b	6fc <try_boot+0x140>
     700:	eb001b31 	bl	73cc <cdbg_shell>
     704:	eafffffe 	b	704 <try_boot+0x148>
     708:	3c000200 	.word	0x3c000200
     70c:	3c00a5cc 	.word	0x3c00a5cc
     710:	000167b7 	.word	0x000167b7

00000714 <initialization>:
     714:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     718:	e24dd004 	sub	sp, sp, #4	; 0x4
     71c:	ebfffe8b 	bl	150 <init_data_section>
     720:	eb0002c1 	bl	122c <init_efuse>
     724:	eb000027 	bl	7c8 <init_xom>
     728:	eb0000e8 	bl	ad0 <init_lowlevel>
     72c:	eb0001ff 	bl	f30 <init_igps_ram>
     730:	eb000475 	bl	190c <init_malloc>
     734:	eb003819 	bl	e7a0 <init_timer>
     738:	eb00023d 	bl	1034 <init_gtc>
     73c:	eb00379f 	bl	e5c0 <init_serial>
     740:	eb0022c8 	bl	9268 <init_mmc>
     744:	eb002a44 	bl	b05c <init_nand>
     748:	eb0001fd 	bl	f44 <init_crypto>
     74c:	e3a00000 	mov	r0, #0	; 0x0
     750:	e28dd004 	add	sp, sp, #4	; 0x4
     754:	e8bd8000 	pop	{pc}

00000758 <main>:
     758:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     75c:	e3a00000 	mov	r0, #0	; 0x0
     760:	e24dd004 	sub	sp, sp, #4	; 0x4
     764:	eb001aa2 	bl	71f4 <cdbg_log_toggle>
     768:	ebffffe9 	bl	714 <initialization>
     76c:	eb000223 	bl	1000 <gtc_time>
     770:	e3a00001 	mov	r0, #1	; 0x1
     774:	eb001a9e 	bl	71f4 <cdbg_log_toggle>
     778:	e59f001c 	ldr	r0, [pc, #28]	; 79c <main+0x44>
     77c:	e59f301c 	ldr	r3, [pc, #28]	; 7a0 <main+0x48>
     780:	e1a0e00f 	mov	lr, pc
     784:	e593f000 	ldr	pc, [r3]
     788:	e3a00000 	mov	r0, #0	; 0x0
     78c:	eb001a98 	bl	71f4 <cdbg_log_toggle>
     790:	e3a00000 	mov	r0, #0	; 0x0
     794:	ebffff88 	bl	5bc <try_boot>
     798:	eafffffe 	b	798 <main+0x40>
     79c:	000167da 	.word	0x000167da
     7a0:	3c00a5cc 	.word	0x3c00a5cc

000007a4 <get_xom>:
     7a4:	e59f3004 	ldr	r3, [pc, #4]	; 7b0 <get_xom+0xc>
     7a8:	e1d300b0 	ldrh	r0, [r3]
     7ac:	e12fff1e 	bx	lr
     7b0:	3c00a038 	.word	0x3c00a038

000007b4 <set_xom>:
     7b4:	e59f3008 	ldr	r3, [pc, #8]	; 7c4 <set_xom+0x10>
     7b8:	e200001f 	and	r0, r0, #31	; 0x1f
     7bc:	e1c300b0 	strh	r0, [r3]
     7c0:	e12fff1e 	bx	lr
     7c4:	3c00a038 	.word	0x3c00a038

000007c8 <init_xom>:
     7c8:	e12fff1e 	bx	lr

000007cc <boot_info>:
     7cc:	e92d4010 	push	{r4, lr}
     7d0:	eb000227 	bl	1074 <ecfg_paras>
     7d4:	e1a04000 	mov	r4, r0
     7d8:	e3a00b01 	mov	r0, #1024	; 0x400
     7dc:	eb00021d 	bl	1058 <ecfg_check_flag>
     7e0:	e59f3028 	ldr	r3, [pc, #40]	; 810 <boot_info+0x44>
     7e4:	e3500000 	cmp	r0, #0	; 0x0
     7e8:	11d330b0 	ldrhne	r3, [r3]
     7ec:	01d330b0 	ldrheq	r3, [r3]
     7f0:	1203301f 	andne	r3, r3, #31	; 0x1f
     7f4:	0203301f 	andeq	r3, r3, #31	; 0x1f
     7f8:	159f2014 	ldrne	r2, [pc, #20]	; 814 <boot_info+0x48>
     7fc:	11a03083 	lslne	r3, r3, #1
     800:	00843083 	addeq	r3, r4, r3, lsl #1
     804:	119200b3 	ldrhne	r0, [r2, r3]
     808:	01d302b0 	ldrheq	r0, [r3, #32]
     80c:	e8bd8010 	pop	{r4, pc}
     810:	3c00a038 	.word	0x3c00a038
     814:	00016350 	.word	0x00016350

00000818 <boot_device>:
     818:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     81c:	e24dd004 	sub	sp, sp, #4	; 0x4
     820:	ebffffe9 	bl	7cc <boot_info>
     824:	e1a00620 	lsr	r0, r0, #12
     828:	e6ff0070 	uxth	r0, r0
     82c:	e28dd004 	add	sp, sp, #4	; 0x4
     830:	e8bd8000 	pop	{pc}

00000834 <set_pll>:
     834:	e3500003 	cmp	r0, #3	; 0x3
     838:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     83c:	e1a02001 	mov	r2, r1
     840:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     844:	e3a03018 	mov	r3, #24	; 0x18
     848:	e0030093 	mul	r3, r3, r0
     84c:	e59f808c 	ldr	r8, [pc, #140]	; 8e0 <set_pll+0xac>
     850:	e59f108c 	ldr	r1, [pc, #140]	; 8e4 <set_pll+0xb0>
     854:	e59f008c 	ldr	r0, [pc, #140]	; 8e8 <set_pll+0xb4>
     858:	e59fc08c 	ldr	ip, [pc, #140]	; 8ec <set_pll+0xb8>
     85c:	e59f408c 	ldr	r4, [pc, #140]	; 8f0 <set_pll+0xbc>
     860:	e59fa08c 	ldr	sl, [pc, #140]	; 8f4 <set_pll+0xc0>
     864:	e0838008 	add	r8, r3, r8
     868:	e083c00c 	add	ip, r3, ip
     86c:	e0831001 	add	r1, r3, r1
     870:	e3a07001 	mov	r7, #1	; 0x1
     874:	e0830000 	add	r0, r3, r0
     878:	e1a06422 	lsr	r6, r2, #8
     87c:	e0834004 	add	r4, r3, r4
     880:	e3a05000 	mov	r5, #0	; 0x0
     884:	e6ef2072 	uxtb	r2, r2
     888:	e083a00a 	add	sl, r3, sl
     88c:	e3a03002 	mov	r3, #2	; 0x2
     890:	e5c87000 	strb	r7, [r8]
     894:	e5c12000 	strb	r2, [r1]
     898:	e5c06000 	strb	r6, [r0]
     89c:	e5cc5000 	strb	r5, [ip]
     8a0:	e5c43000 	strb	r3, [r4]
     8a4:	e0833007 	add	r3, r3, r7
     8a8:	e5c83000 	strb	r3, [r8]
     8ac:	e1a02005 	mov	r2, r5
     8b0:	e5cc7000 	strb	r7, [ip]
     8b4:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     8b8:	ea000001 	b	8c4 <set_pll+0x90>
     8bc:	e1520001 	cmp	r2, r1
     8c0:	0a000003 	beq	8d4 <set_pll+0xa0>
     8c4:	e5da3000 	ldrb	r3, [sl]
     8c8:	e2822001 	add	r2, r2, #1	; 0x1
     8cc:	e3130001 	tst	r3, #1	; 0x1
     8d0:	0afffff9 	beq	8bc <set_pll+0x88>
     8d4:	e3a03002 	mov	r3, #2	; 0x2
     8d8:	e5883000 	str	r3, [r8]
     8dc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     8e0:	21e0a00c 	.word	0x21e0a00c
     8e4:	21e0a000 	.word	0x21e0a000
     8e8:	21e0a004 	.word	0x21e0a004
     8ec:	21e0a010 	.word	0x21e0a010
     8f0:	21e0a008 	.word	0x21e0a008
     8f4:	21e0a014 	.word	0x21e0a014

000008f8 <set_clk_divider>:
     8f8:	e3500005 	cmp	r0, #5	; 0x5
     8fc:	812fff1e 	bxhi	lr
     900:	e59fc020 	ldr	ip, [pc, #32]	; 928 <set_clk_divider+0x30>
     904:	e1a03280 	lsl	r3, r0, #5
     908:	e083c00c 	add	ip, r3, ip
     90c:	e3a02000 	mov	r2, #0	; 0x0
     910:	e7d13002 	ldrb	r3, [r1, r2]
     914:	e2822001 	add	r2, r2, #1	; 0x1
     918:	e3520008 	cmp	r2, #8	; 0x8
     91c:	e4cc3004 	strb	r3, [ip], #4
     920:	1afffffa 	bne	910 <set_clk_divider+0x18>
     924:	e12fff1e 	bx	lr
     928:	21e0a070 	.word	0x21e0a070

0000092c <module_set_clock>:
     92c:	e3a03001 	mov	r3, #1	; 0x1
     930:	e580300c 	str	r3, [r0, #12]
     934:	e2011007 	and	r1, r1, #7	; 0x7
     938:	e3a03000 	mov	r3, #0	; 0x0
     93c:	e5801000 	str	r1, [r0]
     940:	e5802004 	str	r2, [r0, #4]
     944:	e580300c 	str	r3, [r0, #12]
     948:	e12fff1e 	bx	lr

0000094c <set_clk>:
     94c:	e1d010b2 	ldrh	r1, [r0, #2]
     950:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
     954:	e3510000 	cmp	r1, #0	; 0x0
     958:	e1a04000 	mov	r4, r0
     95c:	0a000001 	beq	968 <set_clk+0x1c>
     960:	e3a00001 	mov	r0, #1	; 0x1
     964:	ebffffb2 	bl	834 <set_pll>
     968:	e1d410b4 	ldrh	r1, [r4, #4]
     96c:	e3510000 	cmp	r1, #0	; 0x0
     970:	0a000001 	beq	97c <set_clk+0x30>
     974:	e3a00002 	mov	r0, #2	; 0x2
     978:	ebffffad 	bl	834 <set_pll>
     97c:	e1d410b6 	ldrh	r1, [r4, #6]
     980:	e3510000 	cmp	r1, #0	; 0x0
     984:	0a000001 	beq	990 <set_clk+0x44>
     988:	e3a00003 	mov	r0, #3	; 0x3
     98c:	ebffffa8 	bl	834 <set_pll>
     990:	e59f30f4 	ldr	r3, [pc, #244]	; a8c <set_clk+0x140>
     994:	e3a01001 	mov	r1, #1	; 0x1
     998:	e3a00000 	mov	r0, #0	; 0x0
     99c:	e3a02003 	mov	r2, #3	; 0x3
     9a0:	e583119c 	str	r1, [r3, #412]
     9a4:	e3a0500b 	mov	r5, #11	; 0xb
     9a8:	e5830190 	str	r0, [r3, #400]
     9ac:	e3a04005 	mov	r4, #5	; 0x5
     9b0:	e5832194 	str	r2, [r3, #404]
     9b4:	e1a07002 	mov	r7, r2
     9b8:	e2822010 	add	r2, r2, #16	; 0x10
     9bc:	e583019c 	str	r0, [r3, #412]
     9c0:	e3a0c002 	mov	ip, #2	; 0x2
     9c4:	e583125c 	str	r1, [r3, #604]
     9c8:	e5831250 	str	r1, [r3, #592]
     9cc:	e5835254 	str	r5, [r3, #596]
     9d0:	e583025c 	str	r0, [r3, #604]
     9d4:	e583127c 	str	r1, [r3, #636]
     9d8:	e5831270 	str	r1, [r3, #624]
     9dc:	e5834274 	str	r4, [r3, #628]
     9e0:	e583027c 	str	r0, [r3, #636]
     9e4:	e58312cc 	str	r1, [r3, #716]
     9e8:	e58312c0 	str	r1, [r3, #704]
     9ec:	e58322c4 	str	r2, [r3, #708]
     9f0:	e3a0200f 	mov	r2, #15	; 0xf
     9f4:	e58302cc 	str	r0, [r3, #716]
     9f8:	e59f6090 	ldr	r6, [pc, #144]	; a90 <set_clk+0x144>
     9fc:	e583124c 	str	r1, [r3, #588]
     a00:	e59f908c 	ldr	r9, [pc, #140]	; a94 <set_clk+0x148>
     a04:	e583c240 	str	ip, [r3, #576]
     a08:	e5832244 	str	r2, [r3, #580]
     a0c:	e282200a 	add	r2, r2, #10	; 0xa
     a10:	e583024c 	str	r0, [r3, #588]
     a14:	e1a0a001 	mov	sl, r1
     a18:	e58312fc 	str	r1, [r3, #764]
     a1c:	e1a08000 	mov	r8, r0
     a20:	e583c2f0 	str	ip, [r3, #752]
     a24:	e58342f4 	str	r4, [r3, #756]
     a28:	e58302fc 	str	r0, [r3, #764]
     a2c:	e58312dc 	str	r1, [r3, #732]
     a30:	e583c2d0 	str	ip, [r3, #720]
     a34:	e58352d4 	str	r5, [r3, #724]
     a38:	e58302dc 	str	r0, [r3, #732]
     a3c:	e583129c 	str	r1, [r3, #668]
     a40:	e583c290 	str	ip, [r3, #656]
     a44:	e5832294 	str	r2, [r3, #660]
     a48:	e583029c 	str	r0, [r3, #668]
     a4c:	e586a000 	str	sl, [r6]
     a50:	e506700c 	str	r7, [r6, #-12]
     a54:	e5067008 	str	r7, [r6, #-8]
     a58:	e4868010 	str	r8, [r6], #16
     a5c:	e1560009 	cmp	r6, r9
     a60:	1afffff9 	bne	a4c <set_clk+0x100>
     a64:	e59f2020 	ldr	r2, [pc, #32]	; a8c <set_clk+0x140>
     a68:	e3a03001 	mov	r3, #1	; 0x1
     a6c:	e582326c 	str	r3, [r2, #620]
     a70:	e2833002 	add	r3, r3, #2	; 0x2
     a74:	e5823260 	str	r3, [r2, #608]
     a78:	e2833010 	add	r3, r3, #16	; 0x10
     a7c:	e5823264 	str	r3, [r2, #612]
     a80:	e3a03000 	mov	r3, #0	; 0x0
     a84:	e582326c 	str	r3, [r2, #620]
     a88:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
     a8c:	21e0a000 	.word	0x21e0a000
     a90:	21e0a13c 	.word	0x21e0a13c
     a94:	21e0a19c 	.word	0x21e0a19c

00000a98 <module_enable>:
     a98:	e3a020ff 	mov	r2, #255	; 0xff
     a9c:	e3a01000 	mov	r1, #0	; 0x0
     aa0:	e3a03011 	mov	r3, #17	; 0x11
     aa4:	e580100c 	str	r1, [r0, #12]
     aa8:	e5802000 	str	r2, [r0]
     aac:	e5802004 	str	r2, [r0, #4]
     ab0:	e5803008 	str	r3, [r0, #8]
     ab4:	e5903008 	ldr	r3, [r0, #8]
     ab8:	e3c33010 	bic	r3, r3, #16	; 0x10
     abc:	e5803008 	str	r3, [r0, #8]
     ac0:	e5802018 	str	r2, [r0, #24]
     ac4:	e5801000 	str	r1, [r0]
     ac8:	e580200c 	str	r2, [r0, #12]
     acc:	e12fff1e 	bx	lr

00000ad0 <init_lowlevel>:
     ad0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     ad4:	e24dd044 	sub	sp, sp, #68	; 0x44
     ad8:	e3a01000 	mov	r1, #0	; 0x0
     adc:	e3a02040 	mov	r2, #64	; 0x40
     ae0:	e1a0000d 	mov	r0, sp
     ae4:	eb000256 	bl	1444 <memset>
     ae8:	e3a03063 	mov	r3, #99	; 0x63
     aec:	e5cd3010 	strb	r3, [sp, #16]
     af0:	e3e03073 	mvn	r3, #115	; 0x73
     af4:	e5cd3011 	strb	r3, [sp, #17]
     af8:	e28330a5 	add	r3, r3, #165	; 0xa5
     afc:	e5cd3012 	strb	r3, [sp, #18]
     b00:	e3a0301f 	mov	r3, #31	; 0x1f
     b04:	e5cd3018 	strb	r3, [sp, #24]
     b08:	e283305d 	add	r3, r3, #93	; 0x5d
     b0c:	e5cd3019 	strb	r3, [sp, #25]
     b10:	e3e0300f 	mvn	r3, #15	; 0xf
     b14:	e5cd301a 	strb	r3, [sp, #26]
     b18:	e2833020 	add	r3, r3, #32	; 0x20
     b1c:	e5cd3020 	strb	r3, [sp, #32]
     b20:	e2833032 	add	r3, r3, #50	; 0x32
     b24:	e5cd3021 	strb	r3, [sp, #33]
     b28:	e3a03002 	mov	r3, #2	; 0x2
     b2c:	e5cd3029 	strb	r3, [sp, #41]
     b30:	e3e0301f 	mvn	r3, #31	; 0x1f
     b34:	e5cd3030 	strb	r3, [sp, #48]
     b38:	e3e0307c 	mvn	r3, #124	; 0x7c
     b3c:	e5cd3031 	strb	r3, [sp, #49]
     b40:	e283308c 	add	r3, r3, #140	; 0x8c
     b44:	e3a02008 	mov	r2, #8	; 0x8
     b48:	e5cd3032 	strb	r3, [sp, #50]
     b4c:	e3a03001 	mov	r3, #1	; 0x1
     b50:	e5cd202a 	strb	r2, [sp, #42]
     b54:	e58d3038 	str	r3, [sp, #56]
     b58:	e5cd2022 	strb	r2, [sp, #34]
     b5c:	e3a01000 	mov	r1, #0	; 0x0
     b60:	e3e00000 	mvn	r0, #0	; 0x0
     b64:	e59f3044 	ldr	r3, [pc, #68]	; bb0 <init_lowlevel+0xe0>
     b68:	e1a02101 	lsl	r2, r1, #2
     b6c:	e2811001 	add	r1, r1, #1	; 0x1
     b70:	e0823003 	add	r3, r2, r3
     b74:	e3510008 	cmp	r1, #8	; 0x8
     b78:	e5830000 	str	r0, [r3]
     b7c:	1afffff8 	bne	b64 <init_lowlevel+0x94>
     b80:	e3012042 	movw	r2, #4162	; 0x1042
     b84:	e28d0040 	add	r0, sp, #64	; 0x40
     b88:	e3013031 	movw	r3, #4145	; 0x1031
     b8c:	e16024b0 	strh	r2, [r0, #-64]!
     b90:	e1cd30b4 	strh	r3, [sp, #4]
     b94:	e1a0000d 	mov	r0, sp
     b98:	e302304f 	movw	r3, #8271	; 0x204f
     b9c:	e1cd30b2 	strh	r3, [sp, #2]
     ba0:	e1cd20b6 	strh	r2, [sp, #6]
     ba4:	ebffff68 	bl	94c <set_clk>
     ba8:	e28dd044 	add	sp, sp, #68	; 0x44
     bac:	e8bd8000 	pop	{pc}
     bb0:	21b01180 	.word	0x21b01180

00000bb4 <pads_states>:
     bb4:	e2002007 	and	r2, r0, #7	; 0x7
     bb8:	e3a03001 	mov	r3, #1	; 0x1
     bbc:	e1a02213 	lsl	r2, r3, r2
     bc0:	e3500000 	cmp	r0, #0	; 0x0
     bc4:	e2803007 	add	r3, r0, #7	; 0x7
     bc8:	b1a00003 	movlt	r0, r3
     bcc:	e1a001c0 	asr	r0, r0, #3
     bd0:	e59f302c 	ldr	r3, [pc, #44]	; c04 <pads_states+0x50>
     bd4:	e1a01100 	lsl	r1, r0, #2
     bd8:	e0813003 	add	r3, r1, r3
     bdc:	e5933000 	ldr	r3, [r3]
     be0:	e0130002 	ands	r0, r3, r2
     be4:	012fff1e 	bxeq	lr
     be8:	e59f3018 	ldr	r3, [pc, #24]	; c08 <pads_states+0x54>
     bec:	e0813003 	add	r3, r1, r3
     bf0:	e5933000 	ldr	r3, [r3]
     bf4:	e1130002 	tst	r3, r2
     bf8:	13a00001 	movne	r0, #1	; 0x1
     bfc:	03a00002 	moveq	r0, #2	; 0x2
     c00:	e12fff1e 	bx	lr
     c04:	21e09064 	.word	0x21e09064
     c08:	21e090b4 	.word	0x21e090b4

00000c0c <pads_chmod>:
     c0c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     c10:	e1b07840 	asrs	r7, r0, #16
     c14:	e1a00800 	lsl	r0, r0, #16
     c18:	03a03000 	moveq	r3, #0	; 0x0
     c1c:	13a03001 	movne	r3, #1	; 0x1
     c20:	e1a00820 	lsr	r0, r0, #16
     c24:	e1570000 	cmp	r7, r0
     c28:	a3a03000 	movge	r3, #0	; 0x0
     c2c:	b2033001 	andlt	r3, r3, #1	; 0x1
     c30:	e3530000 	cmp	r3, #0	; 0x0
     c34:	e24dd004 	sub	sp, sp, #4	; 0x4
     c38:	e1a06001 	mov	r6, r1
     c3c:	e1a09002 	mov	r9, r2
     c40:	1a000048 	bne	d68 <pads_chmod+0x15c>
     c44:	e3570000 	cmp	r7, #0	; 0x0
     c48:	01a07000 	moveq	r7, r0
     c4c:	e1a05000 	mov	r5, r0
     c50:	e240801d 	sub	r8, r0, #29	; 0x1d
     c54:	e59fb114 	ldr	fp, [pc, #276]	; d70 <pads_chmod+0x164>
     c58:	e3a0a001 	mov	sl, #1	; 0x1
     c5c:	ea00003f 	b	d60 <pads_chmod+0x154>
     c60:	e3550000 	cmp	r5, #0	; 0x0
     c64:	e2853007 	add	r3, r5, #7	; 0x7
     c68:	a1a03005 	movge	r3, r5
     c6c:	e3580006 	cmp	r8, #6	; 0x6
     c70:	e1a001c3 	asr	r0, r3, #3
     c74:	959b3008 	ldrls	r3, [fp, #8]
     c78:	e2051007 	and	r1, r5, #7	; 0x7
     c7c:	9183381a 	orrls	r3, r3, sl, lsl r8
     c80:	958b3008 	strls	r3, [fp, #8]
     c84:	e3560001 	cmp	r6, #1	; 0x1
     c88:	0a00000a 	beq	cb8 <pads_chmod+0xac>
     c8c:	e3560002 	cmp	r6, #2	; 0x2
     c90:	0a000015 	beq	cec <pads_chmod+0xe0>
     c94:	e3560000 	cmp	r6, #0	; 0x0
     c98:	1a000029 	bne	d44 <pads_chmod+0x138>
     c9c:	e59f30d0 	ldr	r3, [pc, #208]	; d74 <pads_chmod+0x168>
     ca0:	e1a02100 	lsl	r2, r0, #2
     ca4:	e0823003 	add	r3, r2, r3
     ca8:	e5932000 	ldr	r2, [r3]
     cac:	e1c2211a 	bic	r2, r2, sl, lsl r1
     cb0:	e5832000 	str	r2, [r3]
     cb4:	ea000027 	b	d58 <pads_chmod+0x14c>
     cb8:	e1a0c116 	lsl	ip, r6, r1
     cbc:	e59f20b0 	ldr	r2, [pc, #176]	; d74 <pads_chmod+0x168>
     cc0:	e1a00100 	lsl	r0, r0, #2
     cc4:	e0802002 	add	r2, r0, r2
     cc8:	e5923000 	ldr	r3, [r2]
     ccc:	e59f10a4 	ldr	r1, [pc, #164]	; d78 <pads_chmod+0x16c>
     cd0:	e183300c 	orr	r3, r3, ip
     cd4:	e5823000 	str	r3, [r2]
     cd8:	e0801001 	add	r1, r0, r1
     cdc:	e5913000 	ldr	r3, [r1]
     ce0:	e183300c 	orr	r3, r3, ip
     ce4:	e5813000 	str	r3, [r1]
     ce8:	ea00001a 	b	d58 <pads_chmod+0x14c>
     cec:	e1a0c11a 	lsl	ip, sl, r1
     cf0:	e59f207c 	ldr	r2, [pc, #124]	; d74 <pads_chmod+0x168>
     cf4:	e1a00100 	lsl	r0, r0, #2
     cf8:	e0802002 	add	r2, r0, r2
     cfc:	e5923000 	ldr	r3, [r2]
     d00:	e59f1070 	ldr	r1, [pc, #112]	; d78 <pads_chmod+0x16c>
     d04:	e183300c 	orr	r3, r3, ip
     d08:	e5823000 	str	r3, [r2]
     d0c:	e0801001 	add	r1, r0, r1
     d10:	e5913000 	ldr	r3, [r1]
     d14:	e1e0400c 	mvn	r4, ip
     d18:	e59f205c 	ldr	r2, [pc, #92]	; d7c <pads_chmod+0x170>
     d1c:	e0033004 	and	r3, r3, r4
     d20:	e5813000 	str	r3, [r1]
     d24:	e3590000 	cmp	r9, #0	; 0x0
     d28:	e0802002 	add	r2, r0, r2
     d2c:	15923000 	ldrne	r3, [r2]
     d30:	05923000 	ldreq	r3, [r2]
     d34:	1183300c 	orrne	r3, r3, ip
     d38:	00033004 	andeq	r3, r3, r4
     d3c:	e5823000 	str	r3, [r2]
     d40:	ea000004 	b	d58 <pads_chmod+0x14c>
     d44:	e59f0034 	ldr	r0, [pc, #52]	; d80 <pads_chmod+0x174>
     d48:	e1a01006 	mov	r1, r6
     d4c:	e59f3030 	ldr	r3, [pc, #48]	; d84 <pads_chmod+0x178>
     d50:	e1a0e00f 	mov	lr, pc
     d54:	e593f000 	ldr	pc, [r3]
     d58:	e2855001 	add	r5, r5, #1	; 0x1
     d5c:	e2888001 	add	r8, r8, #1	; 0x1
     d60:	e1550007 	cmp	r5, r7
     d64:	daffffbd 	ble	c60 <pads_chmod+0x54>
     d68:	e28dd004 	add	sp, sp, #4	; 0x4
     d6c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     d70:	21e09000 	.word	0x21e09000
     d74:	21e09064 	.word	0x21e09064
     d78:	21e090b4 	.word	0x21e090b4
     d7c:	21e09104 	.word	0x21e09104
     d80:	000167df 	.word	0x000167df
     d84:	3c00a5cc 	.word	0x3c00a5cc

00000d88 <pads_pull>:
     d88:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     d8c:	e1b06840 	asrs	r6, r0, #16
     d90:	e1a00800 	lsl	r0, r0, #16
     d94:	03a03000 	moveq	r3, #0	; 0x0
     d98:	13a03001 	movne	r3, #1	; 0x1
     d9c:	e1a00820 	lsr	r0, r0, #16
     da0:	e1560000 	cmp	r6, r0
     da4:	a3a03000 	movge	r3, #0	; 0x0
     da8:	b2033001 	andlt	r3, r3, #1	; 0x1
     dac:	e3530000 	cmp	r3, #0	; 0x0
     db0:	e1a0a001 	mov	sl, r1
     db4:	e1a08002 	mov	r8, r2
     db8:	18bd85f0 	popne	{r4, r5, r6, r7, r8, sl, pc}
     dbc:	e3560000 	cmp	r6, #0	; 0x0
     dc0:	01a06000 	moveq	r6, r0
     dc4:	e1a01000 	mov	r1, r0
     dc8:	e240501d 	sub	r5, r0, #29	; 0x1d
     dcc:	e3a07001 	mov	r7, #1	; 0x1
     dd0:	ea00001e 	b	e50 <pads_pull+0xc8>
     dd4:	e3510000 	cmp	r1, #0	; 0x0
     dd8:	e2813007 	add	r3, r1, #7	; 0x7
     ddc:	a1a03001 	movge	r3, r1
     de0:	e1a031c3 	asr	r3, r3, #3
     de4:	e35a0000 	cmp	sl, #0	; 0x0
     de8:	e1a00103 	lsl	r0, r3, #2
     dec:	e2014007 	and	r4, r1, #7	; 0x7
     df0:	059f2064 	ldreq	r2, [pc, #100]	; e5c <pads_pull+0xd4>
     df4:	00802002 	addeq	r2, r0, r2
     df8:	05923000 	ldreq	r3, [r2]
     dfc:	01c33417 	biceq	r3, r3, r7, lsl r4
     e00:	0a00000f 	beq	e44 <pads_pull+0xbc>
     e04:	e3550006 	cmp	r5, #6	; 0x6
     e08:	859f2050 	ldrhi	r2, [pc, #80]	; e60 <pads_pull+0xd8>
     e0c:	959f2050 	ldrls	r2, [pc, #80]	; e64 <pads_pull+0xdc>
     e10:	80802002 	addhi	r2, r0, r2
     e14:	91a0c005 	movls	ip, r5
     e18:	81a0c004 	movhi	ip, r4
     e1c:	e3580000 	cmp	r8, #0	; 0x0
     e20:	15923000 	ldrne	r3, [r2]
     e24:	05923000 	ldreq	r3, [r2]
     e28:	11833c17 	orrne	r3, r3, r7, lsl ip
     e2c:	01c33c17 	biceq	r3, r3, r7, lsl ip
     e30:	e5823000 	str	r3, [r2]
     e34:	e59f2020 	ldr	r2, [pc, #32]	; e5c <pads_pull+0xd4>
     e38:	e0802002 	add	r2, r0, r2
     e3c:	e5923000 	ldr	r3, [r2]
     e40:	e1833417 	orr	r3, r3, r7, lsl r4
     e44:	e5823000 	str	r3, [r2]
     e48:	e2811001 	add	r1, r1, #1	; 0x1
     e4c:	e2855001 	add	r5, r5, #1	; 0x1
     e50:	e1510006 	cmp	r1, r6
     e54:	daffffde 	ble	dd4 <pads_pull+0x4c>
     e58:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     e5c:	21e09014 	.word	0x21e09014
     e60:	21e091a4 	.word	0x21e091a4
     e64:	21e0900c 	.word	0x21e0900c

00000e68 <pads_setpin>:
     e68:	e92d4070 	push	{r4, r5, r6, lr}
     e6c:	e1b04840 	asrs	r4, r0, #16
     e70:	e1a00800 	lsl	r0, r0, #16
     e74:	03a03000 	moveq	r3, #0	; 0x0
     e78:	13a03001 	movne	r3, #1	; 0x1
     e7c:	e1a00820 	lsr	r0, r0, #16
     e80:	e1540000 	cmp	r4, r0
     e84:	a3a03000 	movge	r3, #0	; 0x0
     e88:	b2033001 	andlt	r3, r3, #1	; 0x1
     e8c:	e3530000 	cmp	r3, #0	; 0x0
     e90:	e1a05001 	mov	r5, r1
     e94:	18bd8070 	popne	{r4, r5, r6, pc}
     e98:	e3540000 	cmp	r4, #0	; 0x0
     e9c:	01a04000 	moveq	r4, r0
     ea0:	e3a06001 	mov	r6, #1	; 0x1
     ea4:	ea00000e 	b	ee4 <pads_setpin+0x7c>
     ea8:	e3500000 	cmp	r0, #0	; 0x0
     eac:	a1a03000 	movge	r3, r0
     eb0:	e1a031c3 	asr	r3, r3, #3
     eb4:	e1a03103 	lsl	r3, r3, #2
     eb8:	e3550000 	cmp	r5, #0	; 0x0
     ebc:	e083c00c 	add	ip, r3, ip
     ec0:	e0832002 	add	r2, r3, r2
     ec4:	15923000 	ldrne	r3, [r2]
     ec8:	059c3000 	ldreq	r3, [ip]
     ecc:	e2001007 	and	r1, r0, #7	; 0x7
     ed0:	11833116 	orrne	r3, r3, r6, lsl r1
     ed4:	01c33116 	biceq	r3, r3, r6, lsl r1
     ed8:	15823000 	strne	r3, [r2]
     edc:	058c3000 	streq	r3, [ip]
     ee0:	e2800001 	add	r0, r0, #1	; 0x1
     ee4:	e59fc010 	ldr	ip, [pc, #16]	; efc <pads_setpin+0x94>
     ee8:	e1500004 	cmp	r0, r4
     eec:	e1a0200c 	mov	r2, ip
     ef0:	e2803007 	add	r3, r0, #7	; 0x7
     ef4:	daffffeb 	ble	ea8 <pads_setpin+0x40>
     ef8:	e8bd8070 	pop	{r4, r5, r6, pc}
     efc:	21e09104 	.word	0x21e09104

00000f00 <pads_getpin>:
     f00:	e6ff3070 	uxth	r3, r0
     f04:	e59f2020 	ldr	r2, [pc, #32]	; f2c <pads_getpin+0x2c>
     f08:	e1a031c3 	asr	r3, r3, #3
     f0c:	e0822103 	add	r2, r2, r3, lsl #2
     f10:	e2000007 	and	r0, r0, #7	; 0x7
     f14:	e3a03001 	mov	r3, #1	; 0x1
     f18:	e0022013 	and	r2, r2, r3, lsl r0
     f1c:	e5920000 	ldr	r0, [r2]
     f20:	e2500000 	subs	r0, r0, #0	; 0x0
     f24:	13a00001 	movne	r0, #1	; 0x1
     f28:	e12fff1e 	bx	lr
     f2c:	21e09154 	.word	0x21e09154

00000f30 <init_igps_ram>:
     f30:	e59f3008 	ldr	r3, [pc, #8]	; f40 <init_igps_ram+0x10>
     f34:	e3a02001 	mov	r2, #1	; 0x1
     f38:	e5832c10 	str	r2, [r3, #3088]
     f3c:	e12fff1e 	bx	lr
     f40:	21e0a000 	.word	0x21e0a000

00000f44 <init_crypto>:
     f44:	e12fff1e 	bx	lr

00000f48 <get_reset_stats>:
     f48:	e59f2028 	ldr	r2, [pc, #40]	; f78 <get_reset_stats+0x30>
     f4c:	e5923c10 	ldr	r3, [r2, #3088]
     f50:	e2130002 	ands	r0, r3, #2	; 0x2
     f54:	012fff1e 	bxeq	lr
     f58:	e5920c2c 	ldr	r0, [r2, #3116]
     f5c:	e3100001 	tst	r0, #1	; 0x1
     f60:	03a00001 	moveq	r0, #1	; 0x1
     f64:	012fff1e 	bxeq	lr
     f68:	e3500037 	cmp	r0, #55	; 0x37
     f6c:	03a00002 	moveq	r0, #2	; 0x2
     f70:	13a00003 	movne	r0, #3	; 0x3
     f74:	e12fff1e 	bx	lr
     f78:	21e09000 	.word	0x21e09000

00000f7c <update_mask_on_reset>:
     f7c:	e59f3018 	ldr	r3, [pc, #24]	; f9c <update_mask_on_reset+0x20>
     f80:	e5932c30 	ldr	r2, [r3, #3120]
     f84:	e3120080 	tst	r2, #128	; 0x80
     f88:	13a03001 	movne	r3, #1	; 0x1
     f8c:	1202200f 	andne	r2, r2, #15	; 0xf
     f90:	11e03213 	mvnne	r3, r3, lsl r2
     f94:	15803000 	strne	r3, [r0]
     f98:	e12fff1e 	bx	lr
     f9c:	21e09000 	.word	0x21e09000

00000fa0 <get_wakeup_entry>:
     fa0:	e59f301c 	ldr	r3, [pc, #28]	; fc4 <get_wakeup_entry+0x24>
     fa4:	e5931c38 	ldr	r1, [r3, #3128]
     fa8:	e5932c34 	ldr	r2, [r3, #3124]
     fac:	e5930c30 	ldr	r0, [r3, #3120]
     fb0:	e1822401 	orr	r2, r2, r1, lsl #8
     fb4:	e5933c2c 	ldr	r3, [r3, #3116]
     fb8:	e1800402 	orr	r0, r0, r2, lsl #8
     fbc:	e1830400 	orr	r0, r3, r0, lsl #8
     fc0:	e12fff1e 	bx	lr
     fc4:	21e09000 	.word	0x21e09000

00000fc8 <system_reset>:
     fc8:	e59f3008 	ldr	r3, [pc, #8]	; fd8 <system_reset+0x10>
     fcc:	e3a02003 	mov	r2, #3	; 0x3
     fd0:	e5832c00 	str	r2, [r3, #3072]
     fd4:	e12fff1e 	bx	lr
     fd8:	21e09000 	.word	0x21e09000

00000fdc <module_reset>:
     fdc:	e3a030ff 	mov	r3, #255	; 0xff
     fe0:	e5803000 	str	r3, [r0]
     fe4:	e92d4010 	push	{r4, lr}
     fe8:	e1a04000 	mov	r4, r0
     fec:	e3a0000a 	mov	r0, #10	; 0xa
     ff0:	eb0035a6 	bl	e690 <udelay>
     ff4:	e3a03000 	mov	r3, #0	; 0x0
     ff8:	e5843000 	str	r3, [r4]
     ffc:	e8bd8010 	pop	{r4, pc}

00001000 <gtc_time>:
    1000:	e59f3020 	ldr	r3, [pc, #32]	; 1028 <gtc_time+0x28>
    1004:	e92d4010 	push	{r4, lr}
    1008:	e5930000 	ldr	r0, [r3]
    100c:	e59f3018 	ldr	r3, [pc, #24]	; 102c <gtc_time+0x2c>
    1010:	e5934000 	ldr	r4, [r3]
    1014:	eb0035c1 	bl	e720 <get_timer>
    1018:	e1a01000 	mov	r1, r0
    101c:	e59f000c 	ldr	r0, [pc, #12]	; 1030 <gtc_time+0x30>
    1020:	e12fff34 	blx	r4
    1024:	e8bd8010 	pop	{r4, pc}
    1028:	3c00a03c 	.word	0x3c00a03c
    102c:	3c00a5cc 	.word	0x3c00a5cc
    1030:	000167f6 	.word	0x000167f6

00001034 <init_gtc>:
    1034:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1038:	e3a00000 	mov	r0, #0	; 0x0
    103c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1040:	eb0035b6 	bl	e720 <get_timer>
    1044:	e59f3008 	ldr	r3, [pc, #8]	; 1054 <init_gtc+0x20>
    1048:	e5830000 	str	r0, [r3]
    104c:	e28dd004 	add	sp, sp, #4	; 0x4
    1050:	e8bd8000 	pop	{pc}
    1054:	3c00a03c 	.word	0x3c00a03c

00001058 <ecfg_check_flag>:
    1058:	e3500001 	cmp	r0, #1	; 0x1
    105c:	0a000002 	beq	106c <ecfg_check_flag+0x14>
    1060:	e3500801 	cmp	r0, #65536	; 0x10000
    1064:	13a00001 	movne	r0, #1	; 0x1
    1068:	112fff1e 	bxne	lr
    106c:	e3a00000 	mov	r0, #0	; 0x0
    1070:	e12fff1e 	bx	lr

00001074 <ecfg_paras>:
    1074:	e59f0000 	ldr	r0, [pc, #0]	; 107c <ecfg_paras+0x8>
    1078:	e12fff1e 	bx	lr
    107c:	3c00a5d0 	.word	0x3c00a5d0

00001080 <efuse_mac>:
    1080:	e3a00000 	mov	r0, #0	; 0x0
    1084:	e3a01000 	mov	r1, #0	; 0x0
    1088:	e12fff1e 	bx	lr

0000108c <efuse_info>:
    108c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1090:	e1a04000 	mov	r4, r0
    1094:	e24dd004 	sub	sp, sp, #4	; 0x4
    1098:	e59f6144 	ldr	r6, [pc, #324]	; 11e4 <efuse_info+0x158>
    109c:	e59f0144 	ldr	r0, [pc, #324]	; 11e8 <efuse_info+0x15c>
    10a0:	e1a0e00f 	mov	lr, pc
    10a4:	e596f000 	ldr	pc, [r6]
    10a8:	e59f013c 	ldr	r0, [pc, #316]	; 11ec <efuse_info+0x160>
    10ac:	e1a0e00f 	mov	lr, pc
    10b0:	e596f000 	ldr	pc, [r6]
    10b4:	e3540000 	cmp	r4, #0	; 0x0
    10b8:	0a00003c 	beq	11b0 <efuse_info+0x124>
    10bc:	e59f412c 	ldr	r4, [pc, #300]	; 11f0 <efuse_info+0x164>
    10c0:	e59f012c 	ldr	r0, [pc, #300]	; 11f4 <efuse_info+0x168>
    10c4:	e5941000 	ldr	r1, [r4]
    10c8:	e1a0e00f 	mov	lr, pc
    10cc:	e596f000 	ldr	pc, [r6]
    10d0:	e5d41004 	ldrb	r1, [r4, #4]
    10d4:	e59f011c 	ldr	r0, [pc, #284]	; 11f8 <efuse_info+0x16c>
    10d8:	e1a0e00f 	mov	lr, pc
    10dc:	e596f000 	ldr	pc, [r6]
    10e0:	e5d41005 	ldrb	r1, [r4, #5]
    10e4:	e59f0110 	ldr	r0, [pc, #272]	; 11fc <efuse_info+0x170>
    10e8:	e1a0e00f 	mov	lr, pc
    10ec:	e596f000 	ldr	pc, [r6]
    10f0:	e5d41006 	ldrb	r1, [r4, #6]
    10f4:	e59f0104 	ldr	r0, [pc, #260]	; 1200 <efuse_info+0x174>
    10f8:	e1a0e00f 	mov	lr, pc
    10fc:	e596f000 	ldr	pc, [r6]
    1100:	e5d41007 	ldrb	r1, [r4, #7]
    1104:	e59f00f8 	ldr	r0, [pc, #248]	; 1204 <efuse_info+0x178>
    1108:	e1a0e00f 	mov	lr, pc
    110c:	e596f000 	ldr	pc, [r6]
    1110:	e1d410b8 	ldrh	r1, [r4, #8]
    1114:	e59f00ec 	ldr	r0, [pc, #236]	; 1208 <efuse_info+0x17c>
    1118:	e1a0e00f 	mov	lr, pc
    111c:	e596f000 	ldr	pc, [r6]
    1120:	e1d410ba 	ldrh	r1, [r4, #10]
    1124:	e59f00e0 	ldr	r0, [pc, #224]	; 120c <efuse_info+0x180>
    1128:	e1a0e00f 	mov	lr, pc
    112c:	e596f000 	ldr	pc, [r6]
    1130:	e1d410bc 	ldrh	r1, [r4, #12]
    1134:	e59f00d4 	ldr	r0, [pc, #212]	; 1210 <efuse_info+0x184>
    1138:	e1a0e00f 	mov	lr, pc
    113c:	e596f000 	ldr	pc, [r6]
    1140:	e1d410be 	ldrh	r1, [r4, #14]
    1144:	e59f00c8 	ldr	r0, [pc, #200]	; 1214 <efuse_info+0x188>
    1148:	e1a0e00f 	mov	lr, pc
    114c:	e596f000 	ldr	pc, [r6]
    1150:	e1a05004 	mov	r5, r4
    1154:	e3a04000 	mov	r4, #0	; 0x0
    1158:	e1a01004 	mov	r1, r4
    115c:	e5952010 	ldr	r2, [r5, #16]
    1160:	e2844001 	add	r4, r4, #1	; 0x1
    1164:	e59f00ac 	ldr	r0, [pc, #172]	; 1218 <efuse_info+0x18c>
    1168:	e1a0e00f 	mov	lr, pc
    116c:	e596f000 	ldr	pc, [r6]
    1170:	e3540004 	cmp	r4, #4	; 0x4
    1174:	e2855004 	add	r5, r5, #4	; 0x4
    1178:	1afffff6 	bne	1158 <efuse_info+0xcc>
    117c:	e59f6060 	ldr	r6, [pc, #96]	; 11e4 <efuse_info+0x158>
    1180:	e59f5068 	ldr	r5, [pc, #104]	; 11f0 <efuse_info+0x164>
    1184:	e284400c 	add	r4, r4, #12	; 0xc
    1188:	e1a03084 	lsl	r3, r4, #1
    118c:	e1a01004 	mov	r1, r4
    1190:	e19520b3 	ldrh	r2, [r5, r3]
    1194:	e2844001 	add	r4, r4, #1	; 0x1
    1198:	e59f007c 	ldr	r0, [pc, #124]	; 121c <efuse_info+0x190>
    119c:	e1a0e00f 	mov	lr, pc
    11a0:	e596f000 	ldr	pc, [r6]
    11a4:	e354001c 	cmp	r4, #28	; 0x1c
    11a8:	1afffff6 	bne	1188 <efuse_info+0xfc>
    11ac:	ea00000a 	b	11dc <efuse_info+0x150>
    11b0:	e59f4068 	ldr	r4, [pc, #104]	; 1220 <efuse_info+0x194>
    11b4:	e59f7068 	ldr	r7, [pc, #104]	; 1224 <efuse_info+0x198>
    11b8:	e1a05006 	mov	r5, r6
    11bc:	e5942000 	ldr	r2, [r4]
    11c0:	e1a01004 	mov	r1, r4
    11c4:	e59f005c 	ldr	r0, [pc, #92]	; 1228 <efuse_info+0x19c>
    11c8:	e2844004 	add	r4, r4, #4	; 0x4
    11cc:	e1a0e00f 	mov	lr, pc
    11d0:	e595f000 	ldr	pc, [r5]
    11d4:	e1540007 	cmp	r4, r7
    11d8:	1afffff7 	bne	11bc <efuse_info+0x130>
    11dc:	e28dd004 	add	sp, sp, #4	; 0x4
    11e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    11e4:	3c00a5cc 	.word	0x3c00a5cc
    11e8:	000167fe 	.word	0x000167fe
    11ec:	00018f36 	.word	0x00018f36
    11f0:	3c00a5d0 	.word	0x3c00a5d0
    11f4:	00016813 	.word	0x00016813
    11f8:	00016822 	.word	0x00016822
    11fc:	00016831 	.word	0x00016831
    1200:	00016844 	.word	0x00016844
    1204:	00016853 	.word	0x00016853
    1208:	00016868 	.word	0x00016868
    120c:	00016875 	.word	0x00016875
    1210:	00016885 	.word	0x00016885
    1214:	00016896 	.word	0x00016896
    1218:	000168a8 	.word	0x000168a8
    121c:	000168b8 	.word	0x000168b8
    1220:	21e0d9b4 	.word	0x21e0d9b4
    1224:	21e0da94 	.word	0x21e0da94
    1228:	000168c9 	.word	0x000168c9

0000122c <init_efuse>:
    122c:	e59f0008 	ldr	r0, [pc, #8]	; 123c <init_efuse+0x10>
    1230:	e3a01000 	mov	r1, #0	; 0x0
    1234:	e3a02038 	mov	r2, #56	; 0x38
    1238:	ea000081 	b	1444 <memset>
    123c:	3c00a5d0 	.word	0x3c00a5d0

00001240 <chip_test>:
    1240:	e59f0044 	ldr	r0, [pc, #68]	; 128c <end+0x4>
    1244:	e5901000 	ldr	r1, [r0]
    1248:	e59f2040 	ldr	r2, [pc, #64]	; 1290 <end+0x8>
    124c:	e1520001 	cmp	r2, r1
    1250:	1a00000c 	bne	1288 <end>
    1254:	e5901004 	ldr	r1, [r0, #4]
    1258:	e59f2034 	ldr	r2, [pc, #52]	; 1294 <end+0xc>
    125c:	e1520001 	cmp	r2, r1
    1260:	1a000008 	bne	1288 <end>
    1264:	e5901008 	ldr	r1, [r0, #8]
    1268:	e59f2028 	ldr	r2, [pc, #40]	; 1298 <end+0x10>
    126c:	e1520001 	cmp	r2, r1
    1270:	1a000004 	bne	1288 <end>
    1274:	e590100c 	ldr	r1, [r0, #12]
    1278:	e59f201c 	ldr	r2, [pc, #28]	; 129c <end+0x14>
    127c:	e1520001 	cmp	r2, r1
    1280:	1a000000 	bne	1288 <end>
    1284:	e3a0f30f 	mov	pc, #1006632960	; 0x3c000000

00001288 <end>:
    1288:	e1a0f00e 	.word	0xe1a0f00e
    128c:	3c007ff0 	.word	0x3c007ff0
    1290:	6f666e69 	.word	0x6f666e69
    1294:	612d6d74 	.word	0x612d6d74
    1298:	2d6e696c 	.word	0x2d6e696c
    129c:	796c6e6f 	.word	0x796c6e6f

000012a0 <strcpy>:
    12a0:	e3a02000 	mov	r2, #0	; 0x0
    12a4:	e7d13002 	ldrb	r3, [r1, r2]
    12a8:	e3530000 	cmp	r3, #0	; 0x0
    12ac:	e7c03002 	strb	r3, [r0, r2]
    12b0:	e2822001 	add	r2, r2, #1	; 0x1
    12b4:	1afffffa 	bne	12a4 <strcpy+0x4>
    12b8:	e12fff1e 	bx	lr

000012bc <strncpy>:
    12bc:	e3a0c000 	mov	ip, #0	; 0x0
    12c0:	e3520000 	cmp	r2, #0	; 0x0
    12c4:	e2422001 	sub	r2, r2, #1	; 0x1
    12c8:	012fff1e 	bxeq	lr
    12cc:	e7d1300c 	ldrb	r3, [r1, ip]
    12d0:	e3530000 	cmp	r3, #0	; 0x0
    12d4:	e7c0300c 	strb	r3, [r0, ip]
    12d8:	e28cc001 	add	ip, ip, #1	; 0x1
    12dc:	1afffff7 	bne	12c0 <strncpy+0x4>
    12e0:	e12fff1e 	bx	lr

000012e4 <strncat>:
    12e4:	e92d4030 	push	{r4, r5, lr}
    12e8:	e2525000 	subs	r5, r2, #0	; 0x0
    12ec:	e1a04001 	mov	r4, r1
    12f0:	11a0c000 	movne	ip, r0
    12f4:	1a000001 	bne	1300 <strncat+0x1c>
    12f8:	e8bd8030 	pop	{r4, r5, pc}
    12fc:	e28cc001 	add	ip, ip, #1	; 0x1
    1300:	e5dc3000 	ldrb	r3, [ip]
    1304:	e3530000 	cmp	r3, #0	; 0x0
    1308:	1afffffb 	bne	12fc <strncat+0x18>
    130c:	ea000004 	b	1324 <strncat+0x40>
    1310:	e1550002 	cmp	r5, r2
    1314:	1a000004 	bne	132c <strncat+0x48>
    1318:	e3a03000 	mov	r3, #0	; 0x0
    131c:	e5cc3000 	strb	r3, [ip]
    1320:	e8bd8030 	pop	{r4, r5, pc}
    1324:	e1a02003 	mov	r2, r3
    1328:	e1a0100c 	mov	r1, ip
    132c:	e7d43002 	ldrb	r3, [r4, r2]
    1330:	e28cc001 	add	ip, ip, #1	; 0x1
    1334:	e3530000 	cmp	r3, #0	; 0x0
    1338:	e7c13002 	strb	r3, [r1, r2]
    133c:	e2822001 	add	r2, r2, #1	; 0x1
    1340:	1afffff2 	bne	1310 <strncat+0x2c>
    1344:	e8bd8030 	pop	{r4, r5, pc}

00001348 <strcmp>:
    1348:	e92d4010 	push	{r4, lr}
    134c:	e3a02000 	mov	r2, #0	; 0x0
    1350:	e1a04000 	mov	r4, r0
    1354:	e7d4c002 	ldrb	ip, [r4, r2]
    1358:	e7d13002 	ldrb	r3, [r1, r2]
    135c:	e2822001 	add	r2, r2, #1	; 0x1
    1360:	e063300c 	rsb	r3, r3, ip
    1364:	e6ef0073 	uxtb	r0, r3
    1368:	e3500000 	cmp	r0, #0	; 0x0
    136c:	1a000001 	bne	1378 <strcmp+0x30>
    1370:	e35c0000 	cmp	ip, #0	; 0x0
    1374:	1afffff6 	bne	1354 <strcmp+0xc>
    1378:	e6af0070 	sxtb	r0, r0
    137c:	e8bd8010 	pop	{r4, pc}

00001380 <strncmp>:
    1380:	e92d4030 	push	{r4, r5, lr}
    1384:	e3a0c000 	mov	ip, #0	; 0x0
    1388:	e1a05000 	mov	r5, r0
    138c:	e1a04001 	mov	r4, r1
    1390:	ea000009 	b	13bc <strncmp+0x3c>
    1394:	e7d5100c 	ldrb	r1, [r5, ip]
    1398:	e7d4300c 	ldrb	r3, [r4, ip]
    139c:	e2422001 	sub	r2, r2, #1	; 0x1
    13a0:	e0633001 	rsb	r3, r3, r1
    13a4:	e6ef0073 	uxtb	r0, r3
    13a8:	e3500000 	cmp	r0, #0	; 0x0
    13ac:	e28cc001 	add	ip, ip, #1	; 0x1
    13b0:	1a000004 	bne	13c8 <strncmp+0x48>
    13b4:	e3510000 	cmp	r1, #0	; 0x0
    13b8:	0a000002 	beq	13c8 <strncmp+0x48>
    13bc:	e3520000 	cmp	r2, #0	; 0x0
    13c0:	1afffff3 	bne	1394 <strncmp+0x14>
    13c4:	e1a00002 	mov	r0, r2
    13c8:	e6af0070 	sxtb	r0, r0
    13cc:	e8bd8030 	pop	{r4, r5, pc}

000013d0 <strchr>:
    13d0:	e1a03000 	mov	r3, r0
    13d4:	e6ef1071 	uxtb	r1, r1
    13d8:	ea000002 	b	13e8 <strchr+0x18>
    13dc:	e3500000 	cmp	r0, #0	; 0x0
    13e0:	012fff1e 	bxeq	lr
    13e4:	e2833001 	add	r3, r3, #1	; 0x1
    13e8:	e5d30000 	ldrb	r0, [r3]
    13ec:	e1500001 	cmp	r0, r1
    13f0:	1afffff9 	bne	13dc <strchr+0xc>
    13f4:	e1a00003 	mov	r0, r3
    13f8:	e12fff1e 	bx	lr

000013fc <strlen>:
    13fc:	e1a02000 	mov	r2, r0
    1400:	ea000000 	b	1408 <strlen+0xc>
    1404:	e2800001 	add	r0, r0, #1	; 0x1
    1408:	e5d03000 	ldrb	r3, [r0]
    140c:	e3530000 	cmp	r3, #0	; 0x0
    1410:	1afffffb 	bne	1404 <strlen+0x8>
    1414:	e0620000 	rsb	r0, r2, r0
    1418:	e12fff1e 	bx	lr

0000141c <strnlen>:
    141c:	e3a02000 	mov	r2, #0	; 0x0
    1420:	e1510002 	cmp	r1, r2
    1424:	e080c002 	add	ip, r0, r2
    1428:	0a000003 	beq	143c <strnlen+0x20>
    142c:	e7d03002 	ldrb	r3, [r0, r2]
    1430:	e2822001 	add	r2, r2, #1	; 0x1
    1434:	e3530000 	cmp	r3, #0	; 0x0
    1438:	1afffff8 	bne	1420 <strnlen+0x4>
    143c:	e060000c 	rsb	r0, r0, ip
    1440:	e12fff1e 	bx	lr

00001444 <memset>:
    1444:	e3a03000 	mov	r3, #0	; 0x0
    1448:	ea000001 	b	1454 <memset+0x10>
    144c:	e7c01003 	strb	r1, [r0, r3]
    1450:	e2833001 	add	r3, r3, #1	; 0x1
    1454:	e3520000 	cmp	r2, #0	; 0x0
    1458:	e2422001 	sub	r2, r2, #1	; 0x1
    145c:	1afffffa 	bne	144c <memset+0x8>
    1460:	e12fff1e 	bx	lr

00001464 <memcpy>:
    1464:	e3a0c000 	mov	ip, #0	; 0x0
    1468:	ea000002 	b	1478 <memcpy+0x14>
    146c:	e7d1300c 	ldrb	r3, [r1, ip]
    1470:	e7c0300c 	strb	r3, [r0, ip]
    1474:	e28cc001 	add	ip, ip, #1	; 0x1
    1478:	e3520000 	cmp	r2, #0	; 0x0
    147c:	e2422001 	sub	r2, r2, #1	; 0x1
    1480:	1afffff9 	bne	146c <memcpy+0x8>
    1484:	e12fff1e 	bx	lr

00001488 <memmove>:
    1488:	e1500001 	cmp	r0, r1
    148c:	e92d4010 	push	{r4, lr}
    1490:	08bd8010 	popeq	{r4, pc}
    1494:	93a0c000 	movls	ip, #0	; 0x0
    1498:	9a000003 	bls	14ac <memmove+0x24>
    149c:	ea000006 	b	14bc <memmove+0x34>
    14a0:	e7d1300c 	ldrb	r3, [r1, ip]
    14a4:	e7c0300c 	strb	r3, [r0, ip]
    14a8:	e28cc001 	add	ip, ip, #1	; 0x1
    14ac:	e3520000 	cmp	r2, #0	; 0x0
    14b0:	e2422001 	sub	r2, r2, #1	; 0x1
    14b4:	1afffff9 	bne	14a0 <memmove+0x18>
    14b8:	e8bd8010 	pop	{r4, pc}
    14bc:	e0814002 	add	r4, r1, r2
    14c0:	e3a0c000 	mov	ip, #0	; 0x0
    14c4:	e0801002 	add	r1, r0, r2
    14c8:	ea000001 	b	14d4 <memmove+0x4c>
    14cc:	e7d4300c 	ldrb	r3, [r4, ip]
    14d0:	e7c1300c 	strb	r3, [r1, ip]
    14d4:	e3520000 	cmp	r2, #0	; 0x0
    14d8:	e24cc001 	sub	ip, ip, #1	; 0x1
    14dc:	e2422001 	sub	r2, r2, #1	; 0x1
    14e0:	1afffff9 	bne	14cc <memmove+0x44>
    14e4:	e8bd8010 	pop	{r4, pc}

000014e8 <memcmp>:
    14e8:	e92d4030 	push	{r4, r5, lr}
    14ec:	e3a0c000 	mov	ip, #0	; 0x0
    14f0:	e1a04001 	mov	r4, r1
    14f4:	e1a05000 	mov	r5, r0
    14f8:	e1a01002 	mov	r1, r2
    14fc:	ea000005 	b	1518 <memcmp+0x30>
    1500:	e7d5200c 	ldrb	r2, [r5, ip]
    1504:	e7d4300c 	ldrb	r3, [r4, ip]
    1508:	e28cc001 	add	ip, ip, #1	; 0x1
    150c:	e0520003 	subs	r0, r2, r3
    1510:	18bd8030 	popne	{r4, r5, pc}
    1514:	e2411001 	sub	r1, r1, #1	; 0x1
    1518:	e3510000 	cmp	r1, #0	; 0x0
    151c:	1afffff7 	bne	1500 <memcmp+0x18>
    1520:	e1a00001 	mov	r0, r1
    1524:	e8bd8030 	pop	{r4, r5, pc}

00001528 <so_m_isused>:
    1528:	e5903004 	ldr	r3, [r0, #4]
    152c:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1530:	03a00000 	moveq	r0, #0	; 0x0
    1534:	13a00001 	movne	r0, #1	; 0x1
    1538:	e12fff1e 	bx	lr

0000153c <so_m_set_used>:
    153c:	e5903004 	ldr	r3, [r0, #4]
    1540:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1544:	e5803004 	str	r3, [r0, #4]
    1548:	e12fff1e 	bx	lr

0000154c <so_m_unset_used>:
    154c:	e5903004 	ldr	r3, [r0, #4]
    1550:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1554:	e5803004 	str	r3, [r0, #4]
    1558:	e12fff1e 	bx	lr

0000155c <so_m_get_size>:
    155c:	e5900004 	ldr	r0, [r0, #4]
    1560:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    1564:	e12fff1e 	bx	lr

00001568 <so_m_set_size>:
    1568:	e5903004 	ldr	r3, [r0, #4]
    156c:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    1570:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    1574:	e1811003 	orr	r1, r1, r3
    1578:	e5801004 	str	r1, [r0, #4]
    157c:	e12fff1e 	bx	lr

00001580 <so_m_alloc>:
    1580:	e59f3048 	ldr	r3, [pc, #72]	; 15d0 <so_m_alloc+0x50>
    1584:	e3a02000 	mov	r2, #0	; 0x0
    1588:	e593c000 	ldr	ip, [r3]
    158c:	e5931008 	ldr	r1, [r3, #8]
    1590:	ea000007 	b	15b4 <so_m_alloc+0x34>
    1594:	e5903004 	ldr	r3, [r0, #4]
    1598:	e3530000 	cmp	r3, #0	; 0x0
    159c:	1a000004 	bne	15b4 <so_m_alloc+0x34>
    15a0:	e59f2028 	ldr	r2, [pc, #40]	; 15d0 <so_m_alloc+0x50>
    15a4:	e5923004 	ldr	r3, [r2, #4]
    15a8:	e2433001 	sub	r3, r3, #1	; 0x1
    15ac:	e5823004 	str	r3, [r2, #4]
    15b0:	e12fff1e 	bx	lr
    15b4:	e152000c 	cmp	r2, ip
    15b8:	e1a00001 	mov	r0, r1
    15bc:	e2822001 	add	r2, r2, #1	; 0x1
    15c0:	e2811010 	add	r1, r1, #16	; 0x10
    15c4:	3afffff2 	bcc	1594 <so_m_alloc+0x14>
    15c8:	e3a00000 	mov	r0, #0	; 0x0
    15cc:	e12fff1e 	bx	lr
    15d0:	3c00a60c 	.word	0x3c00a60c

000015d4 <so_m_remove>:
    15d4:	e3a03000 	mov	r3, #0	; 0x0
    15d8:	e59f2014 	ldr	r2, [pc, #20]	; 15f4 <so_m_remove+0x20>
    15dc:	e5803000 	str	r3, [r0]
    15e0:	e5803004 	str	r3, [r0, #4]
    15e4:	e5923004 	ldr	r3, [r2, #4]
    15e8:	e2833001 	add	r3, r3, #1	; 0x1
    15ec:	e5823004 	str	r3, [r2, #4]
    15f0:	e12fff1e 	bx	lr
    15f4:	3c00a60c 	.word	0x3c00a60c

000015f8 <malloc>:
    15f8:	e3500000 	cmp	r0, #0	; 0x0
    15fc:	e92d4070 	push	{r4, r5, r6, lr}
    1600:	0a000047 	beq	1724 <malloc+0x12c>
    1604:	e59f1120 	ldr	r1, [pc, #288]	; 172c <malloc+0x134>
    1608:	e280301f 	add	r3, r0, #31	; 0x1f
    160c:	e5b12014 	ldr	r2, [r1, #20]!
    1610:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    1614:	e2425008 	sub	r5, r2, #8	; 0x8
    1618:	ea000009 	b	1644 <malloc+0x4c>
    161c:	e5953004 	ldr	r3, [r5, #4]
    1620:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    1624:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    1628:	1a000003 	bne	163c <malloc+0x44>
    162c:	e1520004 	cmp	r2, r4
    1630:	3a000001 	bcc	163c <malloc+0x44>
    1634:	9a000030 	bls	16fc <malloc+0x104>
    1638:	ea000005 	b	1654 <malloc+0x5c>
    163c:	e5953008 	ldr	r3, [r5, #8]
    1640:	e2435008 	sub	r5, r3, #8	; 0x8
    1644:	e2853008 	add	r3, r5, #8	; 0x8
    1648:	e1530001 	cmp	r3, r1
    164c:	1afffff2 	bne	161c <malloc+0x24>
    1650:	ea000033 	b	1724 <malloc+0x12c>
    1654:	e59f30d0 	ldr	r3, [pc, #208]	; 172c <malloc+0x134>
    1658:	e1a01000 	mov	r1, r0
    165c:	e5932008 	ldr	r2, [r3, #8]
    1660:	e5930000 	ldr	r0, [r3]
    1664:	ea00000a 	b	1694 <malloc+0x9c>
    1668:	e5923004 	ldr	r3, [r2, #4]
    166c:	e2822010 	add	r2, r2, #16	; 0x10
    1670:	e3530000 	cmp	r3, #0	; 0x0
    1674:	1a000006 	bne	1694 <malloc+0x9c>
    1678:	e59f20ac 	ldr	r2, [pc, #172]	; 172c <malloc+0x134>
    167c:	e3560000 	cmp	r6, #0	; 0x0
    1680:	e5923004 	ldr	r3, [r2, #4]
    1684:	e2433001 	sub	r3, r3, #1	; 0x1
    1688:	e5823004 	str	r3, [r2, #4]
    168c:	1a000005 	bne	16a8 <malloc+0xb0>
    1690:	ea000023 	b	1724 <malloc+0x12c>
    1694:	e1510000 	cmp	r1, r0
    1698:	e1a06002 	mov	r6, r2
    169c:	e2811001 	add	r1, r1, #1	; 0x1
    16a0:	3afffff0 	bcc	1668 <malloc+0x70>
    16a4:	ea00001e 	b	1724 <malloc+0x12c>
    16a8:	e5953004 	ldr	r3, [r5, #4]
    16ac:	e5962004 	ldr	r2, [r6, #4]
    16b0:	e0643003 	rsb	r3, r4, r3
    16b4:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    16b8:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16bc:	e1833002 	orr	r3, r3, r2
    16c0:	e5863004 	str	r3, [r6, #4]
    16c4:	e5951000 	ldr	r1, [r5]
    16c8:	e5950004 	ldr	r0, [r5, #4]
    16cc:	e1a0c005 	mov	ip, r5
    16d0:	e1a03006 	mov	r3, r6
    16d4:	e0841001 	add	r1, r4, r1
    16d8:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    16dc:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    16e0:	e5bc4008 	ldr	r4, [ip, #8]!
    16e4:	e4831008 	str	r1, [r3], #8
    16e8:	e1822000 	orr	r2, r2, r0
    16ec:	e5864008 	str	r4, [r6, #8]
    16f0:	e5843004 	str	r3, [r4, #4]
    16f4:	e985000c 	stmib	r5, {r2, r3}
    16f8:	e586c00c 	str	ip, [r6, #12]
    16fc:	e5953004 	ldr	r3, [r5, #4]
    1700:	e59f1024 	ldr	r1, [pc, #36]	; 172c <malloc+0x134>
    1704:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1708:	e5853004 	str	r3, [r5, #4]
    170c:	e5912010 	ldr	r2, [r1, #16]
    1710:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1714:	e0833002 	add	r3, r3, r2
    1718:	e5813010 	str	r3, [r1, #16]
    171c:	e5950000 	ldr	r0, [r5]
    1720:	e8bd8070 	pop	{r4, r5, r6, pc}
    1724:	e3a00000 	mov	r0, #0	; 0x0
    1728:	e8bd8070 	pop	{r4, r5, r6, pc}
    172c:	3c00a60c 	.word	0x3c00a60c

00001730 <__sOMERGe>:
    1730:	e5902004 	ldr	r2, [r0, #4]
    1734:	e92d4030 	push	{r4, r5, lr}
    1738:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    173c:	e1a05000 	mov	r5, r0
    1740:	e1a0c001 	mov	ip, r1
    1744:	18bd8030 	popne	{r4, r5, pc}
    1748:	e5913004 	ldr	r3, [r1, #4]
    174c:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    1750:	18bd8030 	popne	{r4, r5, pc}
    1754:	e0823003 	add	r3, r2, r3
    1758:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    175c:	e59f0030 	ldr	r0, [pc, #48]	; 1794 <__sOMERGe+0x64>
    1760:	e5853004 	str	r3, [r5, #4]
    1764:	e5814000 	str	r4, [r1]
    1768:	e5814004 	str	r4, [r1, #4]
    176c:	e5903004 	ldr	r3, [r0, #4]
    1770:	e591100c 	ldr	r1, [r1, #12]
    1774:	e59c2008 	ldr	r2, [ip, #8]
    1778:	e2833001 	add	r3, r3, #1	; 0x1
    177c:	e5812000 	str	r2, [r1]
    1780:	e5821004 	str	r1, [r2, #4]
    1784:	e5803004 	str	r3, [r0, #4]
    1788:	e58c4008 	str	r4, [ip, #8]
    178c:	e58c400c 	str	r4, [ip, #12]
    1790:	e8bd8030 	pop	{r4, r5, pc}
    1794:	3c00a60c 	.word	0x3c00a60c

00001798 <free>:
    1798:	e3500000 	cmp	r0, #0	; 0x0
    179c:	159f2090 	ldrne	r2, [pc, #144]	; 1834 <free+0x9c>
    17a0:	e92d4010 	push	{r4, lr}
    17a4:	15b23014 	ldrne	r3, [r2, #20]!
    17a8:	1a000004 	bne	17c0 <free+0x28>
    17ac:	e8bd8010 	pop	{r4, pc}
    17b0:	e5943000 	ldr	r3, [r4]
    17b4:	e1500003 	cmp	r0, r3
    17b8:	0a000005 	beq	17d4 <free+0x3c>
    17bc:	e5943008 	ldr	r3, [r4, #8]
    17c0:	e2434008 	sub	r4, r3, #8	; 0x8
    17c4:	e2843008 	add	r3, r4, #8	; 0x8
    17c8:	e1530002 	cmp	r3, r2
    17cc:	1afffff7 	bne	17b0 <free+0x18>
    17d0:	e8bd8010 	pop	{r4, pc}
    17d4:	e5943004 	ldr	r3, [r4, #4]
    17d8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    17dc:	08bd8010 	popeq	{r4, pc}
    17e0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    17e4:	e59f1048 	ldr	r1, [pc, #72]	; 1834 <free+0x9c>
    17e8:	e5843004 	str	r3, [r4, #4]
    17ec:	e5912010 	ldr	r2, [r1, #16]
    17f0:	e5940008 	ldr	r0, [r4, #8]
    17f4:	e0632002 	rsb	r2, r3, r2
    17f8:	e59f3038 	ldr	r3, [pc, #56]	; 1838 <free+0xa0>
    17fc:	e5812010 	str	r2, [r1, #16]
    1800:	e1500003 	cmp	r0, r3
    1804:	0a000002 	beq	1814 <free+0x7c>
    1808:	e2401008 	sub	r1, r0, #8	; 0x8
    180c:	e1a00004 	mov	r0, r4
    1810:	ebffffc6 	bl	1730 <__sOMERGe>
    1814:	e594000c 	ldr	r0, [r4, #12]
    1818:	e59f3018 	ldr	r3, [pc, #24]	; 1838 <free+0xa0>
    181c:	e1500003 	cmp	r0, r3
    1820:	08bd8010 	popeq	{r4, pc}
    1824:	e2400008 	sub	r0, r0, #8	; 0x8
    1828:	e1a01004 	mov	r1, r4
    182c:	e8bd4010 	pop	{r4, lr}
    1830:	eaffffbe 	b	1730 <__sOMERGe>
    1834:	3c00a60c 	.word	0x3c00a60c
    1838:	3c00a620 	.word	0x3c00a620

0000183c <__sOINFo>:
    183c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1840:	e59f50a8 	ldr	r5, [pc, #168]	; 18f0 <__sOINFo+0xb4>
    1844:	e24dd004 	sub	sp, sp, #4	; 0x4
    1848:	e59f60a4 	ldr	r6, [pc, #164]	; 18f4 <__sOINFo+0xb8>
    184c:	e8950006 	ldm	r5, {r1, r2}
    1850:	e59f00a0 	ldr	r0, [pc, #160]	; 18f8 <__sOINFo+0xbc>
    1854:	e1a0e00f 	mov	lr, pc
    1858:	e596f000 	ldr	pc, [r6]
    185c:	e2853008 	add	r3, r5, #8	; 0x8
    1860:	e8930098 	ldm	r3, {r3, r4, r7}
    1864:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1868:	e0634004 	rsb	r4, r3, r4
    186c:	e3a00064 	mov	r0, #100	; 0x64
    1870:	e1a01004 	mov	r1, r4
    1874:	e0000790 	mul	r0, r0, r7
    1878:	eb004d41 	bl	14d84 <__aeabi_uidiv>
    187c:	e1a01004 	mov	r1, r4
    1880:	e1a03000 	mov	r3, r0
    1884:	e1a02007 	mov	r2, r7
    1888:	e59f006c 	ldr	r0, [pc, #108]	; 18fc <__sOINFo+0xc0>
    188c:	e1a0e00f 	mov	lr, pc
    1890:	e596f000 	ldr	pc, [r6]
    1894:	e59f0064 	ldr	r0, [pc, #100]	; 1900 <__sOINFo+0xc4>
    1898:	e1a0e00f 	mov	lr, pc
    189c:	e596f000 	ldr	pc, [r6]
    18a0:	e59f005c 	ldr	r0, [pc, #92]	; 1904 <__sOINFo+0xc8>
    18a4:	e1a0e00f 	mov	lr, pc
    18a8:	e596f000 	ldr	pc, [r6]
    18ac:	e5b53014 	ldr	r3, [r5, #20]!
    18b0:	ea000007 	b	18d4 <__sOINFo+0x98>
    18b4:	e8940006 	ldm	r4, {r1, r2}
    18b8:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    18bc:	03a03000 	moveq	r3, #0	; 0x0
    18c0:	13a03001 	movne	r3, #1	; 0x1
    18c4:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    18c8:	e1a0e00f 	mov	lr, pc
    18cc:	e596f000 	ldr	pc, [r6]
    18d0:	e5943008 	ldr	r3, [r4, #8]
    18d4:	e2434008 	sub	r4, r3, #8	; 0x8
    18d8:	e2843008 	add	r3, r4, #8	; 0x8
    18dc:	e1530005 	cmp	r3, r5
    18e0:	e59f0020 	ldr	r0, [pc, #32]	; 1908 <__sOINFo+0xcc>
    18e4:	1afffff2 	bne	18b4 <__sOINFo+0x78>
    18e8:	e28dd004 	add	sp, sp, #4	; 0x4
    18ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    18f0:	3c00a60c 	.word	0x3c00a60c
    18f4:	3c00a5cc 	.word	0x3c00a5cc
    18f8:	000168d9 	.word	0x000168d9
    18fc:	000168f3 	.word	0x000168f3
    1900:	00016918 	.word	0x00016918
    1904:	00018f38 	.word	0x00018f38
    1908:	00016929 	.word	0x00016929

0000190c <init_malloc>:
    190c:	e59f20e4 	ldr	r2, [pc, #228]	; 19f8 <init_malloc+0xec>
    1910:	e59f30e4 	ldr	r3, [pc, #228]	; 19fc <init_malloc+0xf0>
    1914:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    1918:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    191c:	e0602003 	rsb	r2, r0, r3
    1920:	e92d4010 	push	{r4, lr}
    1924:	e59f40d4 	ldr	r4, [pc, #212]	; 1a00 <init_malloc+0xf4>
    1928:	e3520b01 	cmp	r2, #1024	; 0x400
    192c:	e5840008 	str	r0, [r4, #8]
    1930:	e584300c 	str	r3, [r4, #12]
    1934:	93e00000 	mvnls	r0, #0	; 0x0
    1938:	98bd8010 	popls	{r4, pc}
    193c:	e284c014 	add	ip, r4, #20	; 0x14
    1940:	e3a03040 	mov	r3, #64	; 0x40
    1944:	e3a01000 	mov	r1, #0	; 0x0
    1948:	e3a02b01 	mov	r2, #1024	; 0x400
    194c:	e584c018 	str	ip, [r4, #24]
    1950:	e584c014 	str	ip, [r4, #20]
    1954:	e5843004 	str	r3, [r4, #4]
    1958:	e5843000 	str	r3, [r4]
    195c:	ebfffeb8 	bl	1444 <memset>
    1960:	e5940008 	ldr	r0, [r4, #8]
    1964:	e594c000 	ldr	ip, [r4]
    1968:	e1a01000 	mov	r1, r0
    196c:	e3a02000 	mov	r2, #0	; 0x0
    1970:	ea000006 	b	1990 <init_malloc+0x84>
    1974:	e59e3004 	ldr	r3, [lr, #4]
    1978:	e3530000 	cmp	r3, #0	; 0x0
    197c:	059f207c 	ldreq	r2, [pc, #124]	; 1a00 <init_malloc+0xf4>
    1980:	05923004 	ldreq	r3, [r2, #4]
    1984:	02433001 	subeq	r3, r3, #1	; 0x1
    1988:	05823004 	streq	r3, [r2, #4]
    198c:	0a000005 	beq	19a8 <init_malloc+0x9c>
    1990:	e152000c 	cmp	r2, ip
    1994:	e1a0e001 	mov	lr, r1
    1998:	e2822001 	add	r2, r2, #1	; 0x1
    199c:	e2811010 	add	r1, r1, #16	; 0x10
    19a0:	3afffff3 	bcc	1974 <init_malloc+0x68>
    19a4:	e3a0e000 	mov	lr, #0	; 0x0
    19a8:	e59fc050 	ldr	ip, [pc, #80]	; 1a00 <init_malloc+0xf4>
    19ac:	e59e2004 	ldr	r2, [lr, #4]
    19b0:	e59c300c 	ldr	r3, [ip, #12]
    19b4:	e1a0100e 	mov	r1, lr
    19b8:	e2433b01 	sub	r3, r3, #1024	; 0x400
    19bc:	e0603003 	rsb	r3, r0, r3
    19c0:	e59c4018 	ldr	r4, [ip, #24]
    19c4:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    19c8:	e2800b01 	add	r0, r0, #1024	; 0x400
    19cc:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    19d0:	e4810008 	str	r0, [r1], #8
    19d4:	e1833002 	orr	r3, r3, r2
    19d8:	e28c2014 	add	r2, ip, #20	; 0x14
    19dc:	e58e2008 	str	r2, [lr, #8]
    19e0:	e58c1018 	str	r1, [ip, #24]
    19e4:	e5841000 	str	r1, [r4]
    19e8:	e58e3004 	str	r3, [lr, #4]
    19ec:	e58e400c 	str	r4, [lr, #12]
    19f0:	e3a00000 	mov	r0, #0	; 0x0
    19f4:	e8bd8010 	pop	{r4, pc}
    19f8:	3c00c01f 	.word	0x3c00c01f
    19fc:	3c018000 	.word	0x3c018000
    1a00:	3c00a60c 	.word	0x3c00a60c

00001a04 <isi_check_header>:
    1a04:	e92d4070 	push	{r4, r5, r6, lr}
    1a08:	e5902000 	ldr	r2, [r0]
    1a0c:	e59f3088 	ldr	r3, [pc, #136]	; 1a9c <isi_check_header+0x98>
    1a10:	e1a05000 	mov	r5, r0
    1a14:	e1520003 	cmp	r2, r3
    1a18:	e24dd008 	sub	sp, sp, #8	; 0x8
    1a1c:	13e0001e 	mvnne	r0, #30	; 0x1e
    1a20:	1a00001b 	bne	1a94 <isi_check_header+0x90>
    1a24:	e3a06000 	mov	r6, #0	; 0x0
    1a28:	e595401c 	ldr	r4, [r5, #28]
    1a2c:	e3a01c02 	mov	r1, #512	; 0x200
    1a30:	e585601c 	str	r6, [r5, #28]
    1a34:	e1a00006 	mov	r0, r6
    1a38:	eb0013bf 	bl	693c <hash_init>
    1a3c:	e3a01c02 	mov	r1, #512	; 0x200
    1a40:	e1a00005 	mov	r0, r5
    1a44:	eb00139c 	bl	68bc <hash_data>
    1a48:	e28d0004 	add	r0, sp, #4	; 0x4
    1a4c:	eb00137e 	bl	684c <hash_value>
    1a50:	e585401c 	str	r4, [r5, #28]
    1a54:	e59d3004 	ldr	r3, [sp, #4]
    1a58:	e1540003 	cmp	r4, r3
    1a5c:	13e0001f 	mvnne	r0, #31	; 0x1f
    1a60:	1a00000b 	bne	1a94 <isi_check_header+0x90>
    1a64:	e5951004 	ldr	r1, [r5, #4]
    1a68:	e59f4030 	ldr	r4, [pc, #48]	; 1aa0 <isi_check_header+0x9c>
    1a6c:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1a70:	e59f002c 	ldr	r0, [pc, #44]	; 1aa4 <isi_check_header+0xa0>
    1a74:	e1a0e00f 	mov	lr, pc
    1a78:	e594f000 	ldr	pc, [r4]
    1a7c:	e5d51007 	ldrb	r1, [r5, #7]
    1a80:	e59f0020 	ldr	r0, [pc, #32]	; 1aa8 <isi_check_header+0xa4>
    1a84:	e201100f 	and	r1, r1, #15	; 0xf
    1a88:	e1a0e00f 	mov	lr, pc
    1a8c:	e594f000 	ldr	pc, [r4]
    1a90:	e1a00006 	mov	r0, r6
    1a94:	e28dd008 	add	sp, sp, #8	; 0x8
    1a98:	e8bd8070 	pop	{r4, r5, r6, pc}
    1a9c:	6973692b 	.word	0x6973692b
    1aa0:	3c00a5cc 	.word	0x3c00a5cc
    1aa4:	00016951 	.word	0x00016951
    1aa8:	0001695f 	.word	0x0001695f

00001aac <isi_check_data_separate>:
    1aac:	e92d4070 	push	{r4, r5, r6, lr}
    1ab0:	e1a05000 	mov	r5, r0
    1ab4:	e5d00007 	ldrb	r0, [r0, #7]
    1ab8:	e24dd028 	sub	sp, sp, #40	; 0x28
    1abc:	e1a04001 	mov	r4, r1
    1ac0:	e200000f 	and	r0, r0, #15	; 0xf
    1ac4:	e595100c 	ldr	r1, [r5, #12]
    1ac8:	eb00139b 	bl	693c <hash_init>
    1acc:	e28d6008 	add	r6, sp, #8	; 0x8
    1ad0:	e595100c 	ldr	r1, [r5, #12]
    1ad4:	e1a00004 	mov	r0, r4
    1ad8:	eb001377 	bl	68bc <hash_data>
    1adc:	e1a00006 	mov	r0, r6
    1ae0:	eb001359 	bl	684c <hash_value>
    1ae4:	eb001351 	bl	6830 <hash_len>
    1ae8:	e2854020 	add	r4, r5, #32	; 0x20
    1aec:	e1a02000 	mov	r2, r0
    1af0:	e1a01004 	mov	r1, r4
    1af4:	e1a00006 	mov	r0, r6
    1af8:	ebfffe7a 	bl	14e8 <memcmp>
    1afc:	e3500000 	cmp	r0, #0	; 0x0
    1b00:	0a000014 	beq	1b58 <isi_check_data_separate+0xac>
    1b04:	e595100c 	ldr	r1, [r5, #12]
    1b08:	e59f3050 	ldr	r3, [pc, #80]	; 1b60 <isi_check_data_separate+0xb4>
    1b0c:	e59f0050 	ldr	r0, [pc, #80]	; 1b64 <isi_check_data_separate+0xb8>
    1b10:	e1a0e00f 	mov	lr, pc
    1b14:	e593f000 	ldr	pc, [r3]
    1b18:	eb001344 	bl	6830 <hash_len>
    1b1c:	e1a01004 	mov	r1, r4
    1b20:	e1a03000 	mov	r3, r0
    1b24:	e3a02001 	mov	r2, #1	; 0x1
    1b28:	e1a00004 	mov	r0, r4
    1b2c:	e3a04020 	mov	r4, #32	; 0x20
    1b30:	e58d4000 	str	r4, [sp]
    1b34:	eb003ff6 	bl	11b14 <print_buffer>
    1b38:	eb00133c 	bl	6830 <hash_len>
    1b3c:	e1a01006 	mov	r1, r6
    1b40:	e1a03000 	mov	r3, r0
    1b44:	e3a02001 	mov	r2, #1	; 0x1
    1b48:	e1a00006 	mov	r0, r6
    1b4c:	e58d4000 	str	r4, [sp]
    1b50:	eb003fef 	bl	11b14 <print_buffer>
    1b54:	e3e00020 	mvn	r0, #32	; 0x20
    1b58:	e28dd028 	add	sp, sp, #40	; 0x28
    1b5c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1b60:	3c00a5cc 	.word	0x3c00a5cc
    1b64:	00016972 	.word	0x00016972

00001b68 <isi_check_data>:
    1b68:	e2801c02 	add	r1, r0, #512	; 0x200
    1b6c:	eaffffce 	b	1aac <isi_check_data_separate>

00001b70 <isi_check>:
    1b70:	e92d4010 	push	{r4, lr}
    1b74:	e1a04000 	mov	r4, r0
    1b78:	ebffffa1 	bl	1a04 <isi_check_header>
    1b7c:	e3500000 	cmp	r0, #0	; 0x0
    1b80:	18bd8010 	popne	{r4, pc}
    1b84:	e1a00004 	mov	r0, r4
    1b88:	e8bd4010 	pop	{r4, lr}
    1b8c:	eafffff5 	b	1b68 <isi_check_data>

00001b90 <load_serial_ymodem>:
    1b90:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1b94:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1b98:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1b9c:	e284400c 	add	r4, r4, #12	; 0xc
    1ba0:	e3a03002 	mov	r3, #2	; 0x2
    1ba4:	e1a07000 	mov	r7, r0
    1ba8:	e1a01004 	mov	r1, r4
    1bac:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1bb0:	e58d3404 	str	r3, [sp, #1028]
    1bb4:	eb004bf8 	bl	14b9c <xyzModem_stream_open>
    1bb8:	e3500000 	cmp	r0, #0	; 0x0
    1bbc:	1a000012 	bne	1c0c <load_serial_ymodem+0x7c>
    1bc0:	e1a08004 	mov	r8, r4
    1bc4:	e1a05000 	mov	r5, r0
    1bc8:	e1a04000 	mov	r4, r0
    1bcc:	e1a0600d 	mov	r6, sp
    1bd0:	ea000001 	b	1bdc <load_serial_ymodem+0x4c>
    1bd4:	e085500c 	add	r5, r5, ip
    1bd8:	ebfffe21 	bl	1464 <memcpy>
    1bdc:	e3a01b01 	mov	r1, #1024	; 0x400
    1be0:	e1a02008 	mov	r2, r8
    1be4:	e1a0000d 	mov	r0, sp
    1be8:	eb004b57 	bl	1494c <xyzModem_stream_read>
    1bec:	e0843007 	add	r3, r4, r7
    1bf0:	e250c000 	subs	ip, r0, #0	; 0x0
    1bf4:	e1a0100d 	mov	r1, sp
    1bf8:	e1a0200c 	mov	r2, ip
    1bfc:	e1a00003 	mov	r0, r3
    1c00:	e084400c 	add	r4, r4, ip
    1c04:	cafffff2 	bgt	1bd4 <load_serial_ymodem+0x44>
    1c08:	ea000007 	b	1c2c <load_serial_ymodem+0x9c>
    1c0c:	e59f3054 	ldr	r3, [pc, #84]	; 1c68 <load_serial_ymodem+0xd8>
    1c10:	e59d040c 	ldr	r0, [sp, #1036]
    1c14:	e5934000 	ldr	r4, [r3]
    1c18:	eb0047d2 	bl	13b68 <xyzModem_error>
    1c1c:	e1a01000 	mov	r1, r0
    1c20:	e59f0044 	ldr	r0, [pc, #68]	; 1c6c <load_serial_ymodem+0xdc>
    1c24:	e12fff34 	blx	r4
    1c28:	e3a05000 	mov	r5, #0	; 0x0
    1c2c:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1c30:	e280000c 	add	r0, r0, #12	; 0xc
    1c34:	eb0047b3 	bl	13b08 <xyzModem_stream_close>
    1c38:	e3a00000 	mov	r0, #0	; 0x0
    1c3c:	e59f102c 	ldr	r1, [pc, #44]	; 1c70 <load_serial_ymodem+0xe0>
    1c40:	eb004a61 	bl	145cc <xyzModem_stream_terminate>
    1c44:	e1a01005 	mov	r1, r5
    1c48:	e59f0024 	ldr	r0, [pc, #36]	; 1c74 <load_serial_ymodem+0xe4>
    1c4c:	e1a02005 	mov	r2, r5
    1c50:	e59f3010 	ldr	r3, [pc, #16]	; 1c68 <load_serial_ymodem+0xd8>
    1c54:	e1a0e00f 	mov	lr, pc
    1c58:	e593f000 	ldr	pc, [r3]
    1c5c:	e1a00007 	mov	r0, r7
    1c60:	e28dde41 	add	sp, sp, #1040	; 0x410
    1c64:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1c68:	3c00a5cc 	.word	0x3c00a5cc
    1c6c:	00016a6e 	.word	0x00016a6e
    1c70:	00001c78 	.word	0x00001c78
    1c74:	00016996 	.word	0x00016996

00001c78 <getcxmodem>:
    1c78:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1c7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1c80:	eb004452 	bl	12dd0 <tstc>
    1c84:	e3500000 	cmp	r0, #0	; 0x0
    1c88:	1a000002 	bne	1c98 <getcxmodem+0x20>
    1c8c:	e3e00000 	mvn	r0, #0	; 0x0
    1c90:	e28dd004 	add	sp, sp, #4	; 0x4
    1c94:	e8bd8000 	pop	{pc}
    1c98:	e28dd004 	add	sp, sp, #4	; 0x4
    1c9c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1ca0:	ea00444b 	b	12dd4 <getc>

00001ca4 <do_load_serial_bin>:
    1ca4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1ca8:	e3520001 	cmp	r2, #1	; 0x1
    1cac:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1cb0:	e1a06002 	mov	r6, r2
    1cb4:	e1a07000 	mov	r7, r0
    1cb8:	e1a05003 	mov	r5, r3
    1cbc:	ca000006 	bgt	1cdc <do_load_serial_bin+0x38>
    1cc0:	e59f0120 	ldr	r0, [pc, #288]	; 1de8 <do_load_serial_bin+0x144>
    1cc4:	e3a06000 	mov	r6, #0	; 0x0
    1cc8:	e59f311c 	ldr	r3, [pc, #284]	; 1dec <do_load_serial_bin+0x148>
    1ccc:	e1a0e00f 	mov	lr, pc
    1cd0:	e593f000 	ldr	pc, [r3]
    1cd4:	e1a04006 	mov	r4, r6
    1cd8:	ea00000c 	b	1d10 <do_load_serial_bin+0x6c>
    1cdc:	e5930004 	ldr	r0, [r3, #4]
    1ce0:	e3a01000 	mov	r1, #0	; 0x0
    1ce4:	e3a02010 	mov	r2, #16	; 0x10
    1ce8:	eb004021 	bl	11d74 <simple_strtoul>
    1cec:	e3560003 	cmp	r6, #3	; 0x3
    1cf0:	e1a04000 	mov	r4, r0
    1cf4:	13a06000 	movne	r6, #0	; 0x0
    1cf8:	1a000004 	bne	1d10 <do_load_serial_bin+0x6c>
    1cfc:	e5950008 	ldr	r0, [r5, #8]
    1d00:	e3a01000 	mov	r1, #0	; 0x0
    1d04:	e3a02010 	mov	r2, #16	; 0x10
    1d08:	eb004019 	bl	11d74 <simple_strtoul>
    1d0c:	e1a06000 	mov	r6, r0
    1d10:	e5950000 	ldr	r0, [r5]
    1d14:	e59f10d4 	ldr	r1, [pc, #212]	; 1df0 <do_load_serial_bin+0x14c>
    1d18:	ebfffd8a 	bl	1348 <strcmp>
    1d1c:	e3500000 	cmp	r0, #0	; 0x0
    1d20:	1a000007 	bne	1d44 <do_load_serial_bin+0xa0>
    1d24:	e1a01004 	mov	r1, r4
    1d28:	e59f00c4 	ldr	r0, [pc, #196]	; 1df4 <do_load_serial_bin+0x150>
    1d2c:	e59f30b8 	ldr	r3, [pc, #184]	; 1dec <do_load_serial_bin+0x148>
    1d30:	e1a0e00f 	mov	lr, pc
    1d34:	e593f000 	ldr	pc, [r3]
    1d38:	e1a00004 	mov	r0, r4
    1d3c:	ebffff93 	bl	1b90 <load_serial_ymodem>
    1d40:	ea000025 	b	1ddc <do_load_serial_bin+0x138>
    1d44:	e5950000 	ldr	r0, [r5]
    1d48:	e59f10a8 	ldr	r1, [pc, #168]	; 1df8 <do_load_serial_bin+0x154>
    1d4c:	ebfffd7d 	bl	1348 <strcmp>
    1d50:	e3500000 	cmp	r0, #0	; 0x0
    1d54:	e59f5090 	ldr	r5, [pc, #144]	; 1dec <do_load_serial_bin+0x148>
    1d58:	1a00001b 	bne	1dcc <do_load_serial_bin+0x128>
    1d5c:	e1a02006 	mov	r2, r6
    1d60:	e59f0094 	ldr	r0, [pc, #148]	; 1dfc <do_load_serial_bin+0x158>
    1d64:	e1a01004 	mov	r1, r4
    1d68:	e1a0e00f 	mov	lr, pc
    1d6c:	e595f000 	ldr	pc, [r5]
    1d70:	e1a00004 	mov	r0, r4
    1d74:	e1a01006 	mov	r1, r6
    1d78:	eb004976 	bl	14358 <control_send>
    1d7c:	e3500000 	cmp	r0, #0	; 0x0
    1d80:	aa000005 	bge	1d9c <do_load_serial_bin+0xf8>
    1d84:	e59d0004 	ldr	r0, [sp, #4]
    1d88:	e5954000 	ldr	r4, [r5]
    1d8c:	eb004775 	bl	13b68 <xyzModem_error>
    1d90:	e1a01000 	mov	r1, r0
    1d94:	e59f0064 	ldr	r0, [pc, #100]	; 1e00 <do_load_serial_bin+0x15c>
    1d98:	e12fff34 	blx	r4
    1d9c:	e28d0004 	add	r0, sp, #4	; 0x4
    1da0:	eb004758 	bl	13b08 <xyzModem_stream_close>
    1da4:	e3a00000 	mov	r0, #0	; 0x0
    1da8:	e59f1054 	ldr	r1, [pc, #84]	; 1e04 <do_load_serial_bin+0x160>
    1dac:	eb004a06 	bl	145cc <xyzModem_stream_terminate>
    1db0:	e1a01006 	mov	r1, r6
    1db4:	e59f004c 	ldr	r0, [pc, #76]	; 1e08 <do_load_serial_bin+0x164>
    1db8:	e1a02006 	mov	r2, r6
    1dbc:	e59f3028 	ldr	r3, [pc, #40]	; 1dec <do_load_serial_bin+0x148>
    1dc0:	e1a0e00f 	mov	lr, pc
    1dc4:	e593f000 	ldr	pc, [r3]
    1dc8:	ea000003 	b	1ddc <do_load_serial_bin+0x138>
    1dcc:	e5971014 	ldr	r1, [r7, #20]
    1dd0:	e59f0034 	ldr	r0, [pc, #52]	; 1e0c <do_load_serial_bin+0x168>
    1dd4:	e1a0e00f 	mov	lr, pc
    1dd8:	e595f000 	ldr	pc, [r5]
    1ddc:	e3a00000 	mov	r0, #0	; 0x0
    1de0:	e28dd00c 	add	sp, sp, #12	; 0xc
    1de4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1de8:	000169be 	.word	0x000169be
    1dec:	3c00a5cc 	.word	0x3c00a5cc
    1df0:	000169d8 	.word	0x000169d8
    1df4:	000169db 	.word	0x000169db
    1df8:	00016a1e 	.word	0x00016a1e
    1dfc:	00016a21 	.word	0x00016a21
    1e00:	00016a6e 	.word	0x00016a6e
    1e04:	00001c78 	.word	0x00001c78
    1e08:	00016996 	.word	0x00016996
    1e0c:	00016a6d 	.word	0x00016a6d

00001e10 <do_mem_cp>:
    1e10:	e3520004 	cmp	r2, #4	; 0x4
    1e14:	e92d4070 	push	{r4, r5, r6, lr}
    1e18:	e1a04003 	mov	r4, r3
    1e1c:	0a000002 	beq	1e2c <do_mem_cp+0x1c>
    1e20:	eb0003bb 	bl	2d14 <cmd_usage>
    1e24:	e3a00001 	mov	r0, #1	; 0x1
    1e28:	e8bd8070 	pop	{r4, r5, r6, pc}
    1e2c:	e5930004 	ldr	r0, [r3, #4]
    1e30:	e3a01000 	mov	r1, #0	; 0x0
    1e34:	e3a02010 	mov	r2, #16	; 0x10
    1e38:	eb003fcd 	bl	11d74 <simple_strtoul>
    1e3c:	e3a01000 	mov	r1, #0	; 0x0
    1e40:	e3a02010 	mov	r2, #16	; 0x10
    1e44:	e1a06000 	mov	r6, r0
    1e48:	e5940008 	ldr	r0, [r4, #8]
    1e4c:	eb003fc8 	bl	11d74 <simple_strtoul>
    1e50:	e3a01000 	mov	r1, #0	; 0x0
    1e54:	e1a05000 	mov	r5, r0
    1e58:	e3a02010 	mov	r2, #16	; 0x10
    1e5c:	e594000c 	ldr	r0, [r4, #12]
    1e60:	eb003fc3 	bl	11d74 <simple_strtoul>
    1e64:	e2800003 	add	r0, r0, #3	; 0x3
    1e68:	e1b00120 	lsrs	r0, r0, #2
    1e6c:	1a000002 	bne	1e7c <do_mem_cp+0x6c>
    1e70:	ea000006 	b	1e90 <do_mem_cp+0x80>
    1e74:	e5163004 	ldr	r3, [r6, #-4]
    1e78:	e5053004 	str	r3, [r5, #-4]
    1e7c:	e3500000 	cmp	r0, #0	; 0x0
    1e80:	e2866004 	add	r6, r6, #4	; 0x4
    1e84:	e2400001 	sub	r0, r0, #1	; 0x1
    1e88:	e2855004 	add	r5, r5, #4	; 0x4
    1e8c:	1afffff8 	bne	1e74 <do_mem_cp+0x64>
    1e90:	e3a00000 	mov	r0, #0	; 0x0
    1e94:	e8bd8070 	pop	{r4, r5, r6, pc}

00001e98 <do_mem_cmp>:
    1e98:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e9c:	e3520004 	cmp	r2, #4	; 0x4
    1ea0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1ea4:	e1a04003 	mov	r4, r3
    1ea8:	0a000002 	beq	1eb8 <do_mem_cmp+0x20>
    1eac:	eb000398 	bl	2d14 <cmd_usage>
    1eb0:	e3a04001 	mov	r4, #1	; 0x1
    1eb4:	ea000029 	b	1f60 <do_mem_cmp+0xc8>
    1eb8:	e5930004 	ldr	r0, [r3, #4]
    1ebc:	e3a01000 	mov	r1, #0	; 0x0
    1ec0:	e3a02010 	mov	r2, #16	; 0x10
    1ec4:	eb003faa 	bl	11d74 <simple_strtoul>
    1ec8:	e3a01000 	mov	r1, #0	; 0x0
    1ecc:	e3a02010 	mov	r2, #16	; 0x10
    1ed0:	e1a07000 	mov	r7, r0
    1ed4:	e5940008 	ldr	r0, [r4, #8]
    1ed8:	eb003fa5 	bl	11d74 <simple_strtoul>
    1edc:	e3a01000 	mov	r1, #0	; 0x0
    1ee0:	e1a06000 	mov	r6, r0
    1ee4:	e3a02010 	mov	r2, #16	; 0x10
    1ee8:	e594000c 	ldr	r0, [r4, #12]
    1eec:	eb003fa0 	bl	11d74 <simple_strtoul>
    1ef0:	e2800003 	add	r0, r0, #3	; 0x3
    1ef4:	e1a00120 	lsr	r0, r0, #2
    1ef8:	e3a05000 	mov	r5, #0	; 0x0
    1efc:	ea00000f 	b	1f40 <do_mem_cmp+0xa8>
    1f00:	e5972000 	ldr	r2, [r7]
    1f04:	e5963000 	ldr	r3, [r6]
    1f08:	e1520003 	cmp	r2, r3
    1f0c:	0a000008 	beq	1f34 <do_mem_cmp+0x9c>
    1f10:	e58d3000 	str	r3, [sp]
    1f14:	e1a01007 	mov	r1, r7
    1f18:	e1a03006 	mov	r3, r6
    1f1c:	e59f0048 	ldr	r0, [pc, #72]	; 1f6c <do_mem_cmp+0xd4>
    1f20:	e59fc048 	ldr	ip, [pc, #72]	; 1f70 <do_mem_cmp+0xd8>
    1f24:	e1a0e00f 	mov	lr, pc
    1f28:	e59cf000 	ldr	pc, [ip]
    1f2c:	e3a04001 	mov	r4, #1	; 0x1
    1f30:	ea000005 	b	1f4c <do_mem_cmp+0xb4>
    1f34:	e2855001 	add	r5, r5, #1	; 0x1
    1f38:	e2877004 	add	r7, r7, #4	; 0x4
    1f3c:	e2866004 	add	r6, r6, #4	; 0x4
    1f40:	e1500005 	cmp	r0, r5
    1f44:	1affffed 	bne	1f00 <do_mem_cmp+0x68>
    1f48:	e3a04000 	mov	r4, #0	; 0x0
    1f4c:	e1a01005 	mov	r1, r5
    1f50:	e59f001c 	ldr	r0, [pc, #28]	; 1f74 <do_mem_cmp+0xdc>
    1f54:	e59f3014 	ldr	r3, [pc, #20]	; 1f70 <do_mem_cmp+0xd8>
    1f58:	e1a0e00f 	mov	lr, pc
    1f5c:	e593f000 	ldr	pc, [r3]
    1f60:	e1a00004 	mov	r0, r4
    1f64:	e28dd00c 	add	sp, sp, #12	; 0xc
    1f68:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1f6c:	00016b73 	.word	0x00016b73
    1f70:	3c00a5cc 	.word	0x3c00a5cc
    1f74:	00016bab 	.word	0x00016bab

00001f78 <do_mem_mw>:
    1f78:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f7c:	e1a06002 	mov	r6, r2
    1f80:	e2422003 	sub	r2, r2, #3	; 0x3
    1f84:	e3520001 	cmp	r2, #1	; 0x1
    1f88:	e24dd004 	sub	sp, sp, #4	; 0x4
    1f8c:	e1a04003 	mov	r4, r3
    1f90:	9a000002 	bls	1fa0 <do_mem_mw+0x28>
    1f94:	eb00035e 	bl	2d14 <cmd_usage>
    1f98:	e3a00001 	mov	r0, #1	; 0x1
    1f9c:	ea000016 	b	1ffc <do_mem_mw+0x84>
    1fa0:	e3a01000 	mov	r1, #0	; 0x0
    1fa4:	e3a02010 	mov	r2, #16	; 0x10
    1fa8:	e5930004 	ldr	r0, [r3, #4]
    1fac:	eb003f70 	bl	11d74 <simple_strtoul>
    1fb0:	e3a01000 	mov	r1, #0	; 0x0
    1fb4:	e1a05000 	mov	r5, r0
    1fb8:	e3a02010 	mov	r2, #16	; 0x10
    1fbc:	e5940008 	ldr	r0, [r4, #8]
    1fc0:	eb003f6b 	bl	11d74 <simple_strtoul>
    1fc4:	e3560004 	cmp	r6, #4	; 0x4
    1fc8:	e1a07000 	mov	r7, r0
    1fcc:	13a00001 	movne	r0, #1	; 0x1
    1fd0:	1a000006 	bne	1ff0 <do_mem_mw+0x78>
    1fd4:	e594000c 	ldr	r0, [r4, #12]
    1fd8:	e3a01000 	mov	r1, #0	; 0x0
    1fdc:	e3a02010 	mov	r2, #16	; 0x10
    1fe0:	eb003f63 	bl	11d74 <simple_strtoul>
    1fe4:	ea000001 	b	1ff0 <do_mem_mw+0x78>
    1fe8:	e5057004 	str	r7, [r5, #-4]
    1fec:	e2400001 	sub	r0, r0, #1	; 0x1
    1ff0:	e3500000 	cmp	r0, #0	; 0x0
    1ff4:	e2855004 	add	r5, r5, #4	; 0x4
    1ff8:	1afffffa 	bne	1fe8 <do_mem_mw+0x70>
    1ffc:	e28dd004 	add	sp, sp, #4	; 0x4
    2000:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00002004 <do_mem_md>:
    2004:	e92d4070 	push	{r4, r5, r6, lr}
    2008:	e3520001 	cmp	r2, #1	; 0x1
    200c:	e24dd008 	sub	sp, sp, #8	; 0x8
    2010:	e1a04002 	mov	r4, r2
    2014:	e1a05003 	mov	r5, r3
    2018:	ca000002 	bgt	2028 <do_mem_md+0x24>
    201c:	eb00033c 	bl	2d14 <cmd_usage>
    2020:	e3a00001 	mov	r0, #1	; 0x1
    2024:	ea000014 	b	207c <do_mem_md+0x78>
    2028:	e5930004 	ldr	r0, [r3, #4]
    202c:	e3a01000 	mov	r1, #0	; 0x0
    2030:	e3a02010 	mov	r2, #16	; 0x10
    2034:	eb003f4e 	bl	11d74 <simple_strtoul>
    2038:	e3540002 	cmp	r4, #2	; 0x2
    203c:	e1a06000 	mov	r6, r0
    2040:	03a030a0 	moveq	r3, #160	; 0xa0
    2044:	0a000004 	beq	205c <do_mem_md+0x58>
    2048:	e5950008 	ldr	r0, [r5, #8]
    204c:	e3a01000 	mov	r1, #0	; 0x0
    2050:	e3a02010 	mov	r2, #16	; 0x10
    2054:	eb003f46 	bl	11d74 <simple_strtoul>
    2058:	e1a03000 	mov	r3, r0
    205c:	e3a0c004 	mov	ip, #4	; 0x4
    2060:	e1a00006 	mov	r0, r6
    2064:	e1a0200c 	mov	r2, ip
    2068:	e1a03123 	lsr	r3, r3, #2
    206c:	e1a01006 	mov	r1, r6
    2070:	e58dc000 	str	ip, [sp]
    2074:	eb003ea6 	bl	11b14 <print_buffer>
    2078:	e3a00000 	mov	r0, #0	; 0x0
    207c:	e28dd008 	add	sp, sp, #8	; 0x8
    2080:	e8bd8070 	pop	{r4, r5, r6, pc}

00002084 <do_nand>:
    2084:	e3520001 	cmp	r2, #1	; 0x1
    2088:	e92d4070 	push	{r4, r5, r6, lr}
    208c:	e1a06002 	mov	r6, r2
    2090:	e1a05003 	mov	r5, r3
    2094:	da000020 	ble	211c <do_nand+0x98>
    2098:	e5930004 	ldr	r0, [r3, #4]
    209c:	e59f10a0 	ldr	r1, [pc, #160]	; 2144 <do_nand+0xc0>
    20a0:	e3a02007 	mov	r2, #7	; 0x7
    20a4:	ebfffcb5 	bl	1380 <strncmp>
    20a8:	e2504000 	subs	r4, r0, #0	; 0x0
    20ac:	1a000001 	bne	20b8 <do_nand+0x34>
    20b0:	eb002a2e 	bl	c970 <ecc_print_stat>
    20b4:	ea000009 	b	20e0 <do_nand+0x5c>
    20b8:	e5950004 	ldr	r0, [r5, #4]
    20bc:	e59f1084 	ldr	r1, [pc, #132]	; 2148 <do_nand+0xc4>
    20c0:	e3a02003 	mov	r2, #3	; 0x3
    20c4:	ebfffcad 	bl	1380 <strncmp>
    20c8:	e2504000 	subs	r4, r0, #0	; 0x0
    20cc:	1a000005 	bne	20e8 <do_nand+0x64>
    20d0:	e3560002 	cmp	r6, #2	; 0x2
    20d4:	d3a00000 	movle	r0, #0	; 0x0
    20d8:	c3a00001 	movgt	r0, #1	; 0x1
    20dc:	eb00271c 	bl	bd54 <base_scan_badblocks>
    20e0:	e1a00004 	mov	r0, r4
    20e4:	e8bd8070 	pop	{r4, r5, r6, pc}
    20e8:	e5950004 	ldr	r0, [r5, #4]
    20ec:	e59f1058 	ldr	r1, [pc, #88]	; 214c <do_nand+0xc8>
    20f0:	e3a02007 	mov	r2, #7	; 0x7
    20f4:	ebfffca1 	bl	1380 <strncmp>
    20f8:	e2504000 	subs	r4, r0, #0	; 0x0
    20fc:	13a00000 	movne	r0, #0	; 0x0
    2100:	18bd8070 	popne	{r4, r5, r6, pc}
    2104:	e3560002 	cmp	r6, #2	; 0x2
    2108:	1a000005 	bne	2124 <do_nand+0xa0>
    210c:	e59f003c 	ldr	r0, [pc, #60]	; 2150 <do_nand+0xcc>
    2110:	e59f303c 	ldr	r3, [pc, #60]	; 2154 <do_nand+0xd0>
    2114:	e1a0e00f 	mov	lr, pc
    2118:	e593f000 	ldr	pc, [r3]
    211c:	e3e00000 	mvn	r0, #0	; 0x0
    2120:	e8bd8070 	pop	{r4, r5, r6, pc}
    2124:	e3a02010 	mov	r2, #16	; 0x10
    2128:	e1a01004 	mov	r1, r4
    212c:	e5950008 	ldr	r0, [r5, #8]
    2130:	eb003f0f 	bl	11d74 <simple_strtoul>
    2134:	e1a01004 	mov	r1, r4
    2138:	eb0027e1 	bl	c0c4 <base_block_markbad>
    213c:	e1a00004 	mov	r0, r4
    2140:	e8bd8070 	pop	{r4, r5, r6, pc}
    2144:	00016c75 	.word	0x00016c75
    2148:	00016c81 	.word	0x00016c81
    214c:	00016c7d 	.word	0x00016c7d
    2150:	00016c85 	.word	0x00016c85
    2154:	3c00a5cc 	.word	0x3c00a5cc

00002158 <do_sc>:
    2158:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    215c:	e3520002 	cmp	r2, #2	; 0x2
    2160:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    2164:	0a000002 	beq	2174 <do_sc+0x1c>
    2168:	eb0002e9 	bl	2d14 <cmd_usage>
    216c:	e3a00001 	mov	r0, #1	; 0x1
    2170:	ea000060 	b	22f8 <do_sc+0x1a0>
    2174:	e5930004 	ldr	r0, [r3, #4]
    2178:	e3a01000 	mov	r1, #0	; 0x0
    217c:	e3a02010 	mov	r2, #16	; 0x10
    2180:	eb003efb 	bl	11d74 <simple_strtoul>
    2184:	e28d4f41 	add	r4, sp, #260	; 0x104
    2188:	e2844002 	add	r4, r4, #2	; 0x2
    218c:	e59f116c 	ldr	r1, [pc, #364]	; 2300 <do_sc+0x1a8>
    2190:	e1a06000 	mov	r6, r0
    2194:	e3a02012 	mov	r2, #18	; 0x12
    2198:	e1a00004 	mov	r0, r4
    219c:	ebfffcb0 	bl	1464 <memcpy>
    21a0:	e3a01000 	mov	r1, #0	; 0x0
    21a4:	ea000004 	b	21bc <do_sc+0x64>
    21a8:	e7d12006 	ldrb	r2, [r1, r6]
    21ac:	e7d43001 	ldrb	r3, [r4, r1]
    21b0:	e2811001 	add	r1, r1, #1	; 0x1
    21b4:	e1520003 	cmp	r2, r3
    21b8:	1a000014 	bne	2210 <do_sc+0xb8>
    21bc:	e3510012 	cmp	r1, #18	; 0x12
    21c0:	e0815006 	add	r5, r1, r6
    21c4:	1afffff7 	bne	21a8 <do_sc+0x50>
    21c8:	e2851eff 	add	r1, r5, #4080	; 0xff0
    21cc:	e281100f 	add	r1, r1, #15	; 0xf
    21d0:	e1a02005 	mov	r2, r5
    21d4:	e5d23000 	ldrb	r3, [r2]
    21d8:	e353000a 	cmp	r3, #10	; 0xa
    21dc:	1a000005 	bne	21f8 <do_sc+0xa0>
    21e0:	e5d23001 	ldrb	r3, [r2, #1]
    21e4:	e353005d 	cmp	r3, #93	; 0x5d
    21e8:	1a000002 	bne	21f8 <do_sc+0xa0>
    21ec:	e5d23002 	ldrb	r3, [r2, #2]
    21f0:	e353005d 	cmp	r3, #93	; 0x5d
    21f4:	0a000003 	beq	2208 <do_sc+0xb0>
    21f8:	e1520001 	cmp	r2, r1
    21fc:	e2822001 	add	r2, r2, #1	; 0x1
    2200:	1afffff3 	bne	21d4 <do_sc+0x7c>
    2204:	ea000001 	b	2210 <do_sc+0xb8>
    2208:	e3550000 	cmp	r5, #0	; 0x0
    220c:	1a000006 	bne	222c <do_sc+0xd4>
    2210:	e59f00ec 	ldr	r0, [pc, #236]	; 2304 <do_sc+0x1ac>
    2214:	e1a01006 	mov	r1, r6
    2218:	e59f30e8 	ldr	r3, [pc, #232]	; 2308 <do_sc+0x1b0>
    221c:	e1a0e00f 	mov	lr, pc
    2220:	e593f000 	ldr	pc, [r3]
    2224:	e3e00000 	mvn	r0, #0	; 0x0
    2228:	ea000032 	b	22f8 <do_sc+0x1a0>
    222c:	e59f40d4 	ldr	r4, [pc, #212]	; 2308 <do_sc+0x1b0>
    2230:	e1a01006 	mov	r1, r6
    2234:	e59f00d0 	ldr	r0, [pc, #208]	; 230c <do_sc+0x1b4>
    2238:	e28d6008 	add	r6, sp, #8	; 0x8
    223c:	e1a0e00f 	mov	lr, pc
    2240:	e594f000 	ldr	pc, [r4]
    2244:	e2466002 	sub	r6, r6, #2	; 0x2
    2248:	e1a07004 	mov	r7, r4
    224c:	ea000000 	b	2254 <do_sc+0xfc>
    2250:	e2855001 	add	r5, r5, #1	; 0x1
    2254:	e5d53000 	ldrb	r3, [r5]
    2258:	e353002d 	cmp	r3, #45	; 0x2d
    225c:	1353000a 	cmpne	r3, #10	; 0xa
    2260:	0afffffa 	beq	2250 <do_sc+0xf8>
    2264:	e353005b 	cmp	r3, #91	; 0x5b
    2268:	0afffff8 	beq	2250 <do_sc+0xf8>
    226c:	e353005d 	cmp	r3, #93	; 0x5d
    2270:	0a000001 	beq	227c <do_sc+0x124>
    2274:	e1a02006 	mov	r2, r6
    2278:	ea000005 	b	2294 <do_sc+0x13c>
    227c:	e5d53001 	ldrb	r3, [r5, #1]
    2280:	e353005d 	cmp	r3, #93	; 0x5d
    2284:	1afffffa 	bne	2274 <do_sc+0x11c>
    2288:	ea00000c 	b	22c0 <do_sc+0x168>
    228c:	e5423001 	strb	r3, [r2, #-1]
    2290:	e2855001 	add	r5, r5, #1	; 0x1
    2294:	e5d53000 	ldrb	r3, [r5]
    2298:	e1a01002 	mov	r1, r2
    229c:	e353000a 	cmp	r3, #10	; 0xa
    22a0:	1353005d 	cmpne	r3, #93	; 0x5d
    22a4:	03a04000 	moveq	r4, #0	; 0x0
    22a8:	13a04001 	movne	r4, #1	; 0x1
    22ac:	e2822001 	add	r2, r2, #1	; 0x1
    22b0:	1afffff5 	bne	228c <do_sc+0x134>
    22b4:	e3550000 	cmp	r5, #0	; 0x0
    22b8:	e5c14000 	strb	r4, [r1]
    22bc:	1a000005 	bne	22d8 <do_sc+0x180>
    22c0:	e59f0048 	ldr	r0, [pc, #72]	; 2310 <do_sc+0x1b8>
    22c4:	e59f303c 	ldr	r3, [pc, #60]	; 2308 <do_sc+0x1b0>
    22c8:	e1a0e00f 	mov	lr, pc
    22cc:	e593f000 	ldr	pc, [r3]
    22d0:	e3a00000 	mov	r0, #0	; 0x0
    22d4:	ea000007 	b	22f8 <do_sc+0x1a0>
    22d8:	e1a01006 	mov	r1, r6
    22dc:	e59f0030 	ldr	r0, [pc, #48]	; 2314 <do_sc+0x1bc>
    22e0:	e1a0e00f 	mov	lr, pc
    22e4:	e597f000 	ldr	pc, [r7]
    22e8:	e1a00006 	mov	r0, r6
    22ec:	e1a01004 	mov	r1, r4
    22f0:	eb0042f3 	bl	12ec4 <run_command>
    22f4:	eaffffd6 	b	2254 <do_sc+0xfc>
    22f8:	e28ddf47 	add	sp, sp, #284	; 0x11c
    22fc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    2300:	0001655c 	.word	0x0001655c
    2304:	00016cc4 	.word	0x00016cc4
    2308:	3c00a5cc 	.word	0x3c00a5cc
    230c:	00016ce1 	.word	0x00016ce1
    2310:	00016cfa 	.word	0x00016cfa
    2314:	00016d07 	.word	0x00016d07

00002318 <do_ls>:
    2318:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    231c:	e59f001c 	ldr	r0, [pc, #28]	; 2340 <do_ls+0x28>
    2320:	e24dd004 	sub	sp, sp, #4	; 0x4
    2324:	e59f1018 	ldr	r1, [pc, #24]	; 2344 <do_ls+0x2c>
    2328:	e59f3018 	ldr	r3, [pc, #24]	; 2348 <do_ls+0x30>
    232c:	e1a0e00f 	mov	lr, pc
    2330:	e593f000 	ldr	pc, [r3]
    2334:	e3a00000 	mov	r0, #0	; 0x0
    2338:	e28dd004 	add	sp, sp, #4	; 0x4
    233c:	e8bd8000 	pop	{pc}
    2340:	00016a6e 	.word	0x00016a6e
    2344:	3c00a004 	.word	0x3c00a004
    2348:	3c00a5cc 	.word	0x3c00a5cc

0000234c <do_pads>:
    234c:	e92d4030 	push	{r4, r5, lr}
    2350:	e3520002 	cmp	r2, #2	; 0x2
    2354:	e24dd004 	sub	sp, sp, #4	; 0x4
    2358:	e1a04003 	mov	r4, r3
    235c:	d59f0114 	ldrle	r0, [pc, #276]	; 2478 <do_pads+0x12c>
    2360:	da000007 	ble	2384 <do_pads+0x38>
    2364:	e5930008 	ldr	r0, [r3, #8]
    2368:	e3a01000 	mov	r1, #0	; 0x0
    236c:	e3a0200a 	mov	r2, #10	; 0xa
    2370:	eb003e7f 	bl	11d74 <simple_strtoul>
    2374:	e350008a 	cmp	r0, #138	; 0x8a
    2378:	e1a05000 	mov	r5, r0
    237c:	9a000004 	bls	2394 <do_pads+0x48>
    2380:	e59f00f4 	ldr	r0, [pc, #244]	; 247c <do_pads+0x130>
    2384:	e59f30f4 	ldr	r3, [pc, #244]	; 2480 <do_pads+0x134>
    2388:	e1a0e00f 	mov	lr, pc
    238c:	e593f000 	ldr	pc, [r3]
    2390:	ea000035 	b	246c <do_pads+0x120>
    2394:	e59f10e8 	ldr	r1, [pc, #232]	; 2484 <do_pads+0x138>
    2398:	e3a02002 	mov	r2, #2	; 0x2
    239c:	e5940004 	ldr	r0, [r4, #4]
    23a0:	ebfffbf6 	bl	1380 <strncmp>
    23a4:	e2502000 	subs	r2, r0, #0	; 0x0
    23a8:	01a00005 	moveq	r0, r5
    23ac:	03a01001 	moveq	r1, #1	; 0x1
    23b0:	0a000019 	beq	241c <do_pads+0xd0>
    23b4:	e59f10cc 	ldr	r1, [pc, #204]	; 2488 <do_pads+0x13c>
    23b8:	e3a02004 	mov	r2, #4	; 0x4
    23bc:	e5940004 	ldr	r0, [r4, #4]
    23c0:	ebfffbee 	bl	1380 <strncmp>
    23c4:	e3500000 	cmp	r0, #0	; 0x0
    23c8:	01a00005 	moveq	r0, r5
    23cc:	03a01002 	moveq	r1, #2	; 0x2
    23d0:	03a02001 	moveq	r2, #1	; 0x1
    23d4:	0a000010 	beq	241c <do_pads+0xd0>
    23d8:	e59f10ac 	ldr	r1, [pc, #172]	; 248c <do_pads+0x140>
    23dc:	e3a02003 	mov	r2, #3	; 0x3
    23e0:	e5940004 	ldr	r0, [r4, #4]
    23e4:	ebfffbe5 	bl	1380 <strncmp>
    23e8:	e2502000 	subs	r2, r0, #0	; 0x0
    23ec:	01a00005 	moveq	r0, r5
    23f0:	03a01002 	moveq	r1, #2	; 0x2
    23f4:	0a000008 	beq	241c <do_pads+0xd0>
    23f8:	e5940004 	ldr	r0, [r4, #4]
    23fc:	e59f108c 	ldr	r1, [pc, #140]	; 2490 <do_pads+0x144>
    2400:	e3a02004 	mov	r2, #4	; 0x4
    2404:	ebfffbdd 	bl	1380 <strncmp>
    2408:	e3500000 	cmp	r0, #0	; 0x0
    240c:	1a000004 	bne	2424 <do_pads+0xd8>
    2410:	e1a01000 	mov	r1, r0
    2414:	e1a02001 	mov	r2, r1
    2418:	e1a00005 	mov	r0, r5
    241c:	ebfff9fa 	bl	c0c <pads_chmod>
    2420:	ea000011 	b	246c <do_pads+0x120>
    2424:	e5940004 	ldr	r0, [r4, #4]
    2428:	e59f1064 	ldr	r1, [pc, #100]	; 2494 <do_pads+0x148>
    242c:	e3a02002 	mov	r2, #2	; 0x2
    2430:	ebfffbd2 	bl	1380 <strncmp>
    2434:	e3500000 	cmp	r0, #0	; 0x0
    2438:	e59f4040 	ldr	r4, [pc, #64]	; 2480 <do_pads+0x134>
    243c:	1a000007 	bne	2460 <do_pads+0x114>
    2440:	e1a00005 	mov	r0, r5
    2444:	e5944000 	ldr	r4, [r4]
    2448:	ebfff9d9 	bl	bb4 <pads_states>
    244c:	e1a01005 	mov	r1, r5
    2450:	e1a02000 	mov	r2, r0
    2454:	e59f003c 	ldr	r0, [pc, #60]	; 2498 <do_pads+0x14c>
    2458:	e12fff34 	blx	r4
    245c:	ea000002 	b	246c <do_pads+0x120>
    2460:	e59f0034 	ldr	r0, [pc, #52]	; 249c <do_pads+0x150>
    2464:	e1a0e00f 	mov	lr, pc
    2468:	e594f000 	ldr	pc, [r4]
    246c:	e3a00000 	mov	r0, #0	; 0x0
    2470:	e28dd004 	add	sp, sp, #4	; 0x4
    2474:	e8bd8030 	pop	{r4, r5, pc}
    2478:	00016d2e 	.word	0x00016d2e
    247c:	00016d46 	.word	0x00016d46
    2480:	3c00a5cc 	.word	0x3c00a5cc
    2484:	00016d6d 	.word	0x00016d6d
    2488:	00016d70 	.word	0x00016d70
    248c:	00016d75 	.word	0x00016d75
    2490:	00016d79 	.word	0x00016d79
    2494:	00016d7e 	.word	0x00016d7e
    2498:	00016d81 	.word	0x00016d81
    249c:	00016d96 	.word	0x00016d96

000024a0 <do_mux>:
    24a0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    24a4:	e3520001 	cmp	r2, #1	; 0x1
    24a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    24ac:	da000005 	ble	24c8 <do_mux+0x28>
    24b0:	e5930004 	ldr	r0, [r3, #4]
    24b4:	e3a01000 	mov	r1, #0	; 0x0
    24b8:	e3a0200a 	mov	r2, #10	; 0xa
    24bc:	eb003e2c 	bl	11d74 <simple_strtoul>
    24c0:	e6ff0070 	uxth	r0, r0
    24c4:	eb003d3f 	bl	119c8 <set_mux>
    24c8:	e59f301c 	ldr	r3, [pc, #28]	; 24ec <do_mux+0x4c>
    24cc:	e59f001c 	ldr	r0, [pc, #28]	; 24f0 <do_mux+0x50>
    24d0:	e5931000 	ldr	r1, [r3]
    24d4:	e59f3018 	ldr	r3, [pc, #24]	; 24f4 <do_mux+0x54>
    24d8:	e1a0e00f 	mov	lr, pc
    24dc:	e593f000 	ldr	pc, [r3]
    24e0:	e3a00000 	mov	r0, #0	; 0x0
    24e4:	e28dd004 	add	sp, sp, #4	; 0x4
    24e8:	e8bd8000 	pop	{pc}
    24ec:	21e09000 	.word	0x21e09000
    24f0:	00016daf 	.word	0x00016daf
    24f4:	3c00a5cc 	.word	0x3c00a5cc

000024f8 <do_mac>:
    24f8:	e59f3020 	ldr	r3, [pc, #32]	; 2520 <do_mac+0x28>
    24fc:	e92d4010 	push	{r4, lr}
    2500:	e5934000 	ldr	r4, [r3]
    2504:	ebfffadd 	bl	1080 <efuse_mac>
    2508:	e1a02000 	mov	r2, r0
    250c:	e1a03001 	mov	r3, r1
    2510:	e59f000c 	ldr	r0, [pc, #12]	; 2524 <do_mac+0x2c>
    2514:	e12fff34 	blx	r4
    2518:	e3a00000 	mov	r0, #0	; 0x0
    251c:	e8bd8010 	pop	{r4, pc}
    2520:	3c00a5cc 	.word	0x3c00a5cc
    2524:	00016dc1 	.word	0x00016dc1

00002528 <do_boot>:
    2528:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    252c:	e3a00000 	mov	r0, #0	; 0x0
    2530:	e24dd004 	sub	sp, sp, #4	; 0x4
    2534:	ebfff820 	bl	5bc <try_boot>
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
    2560:	ebfffb86 	bl	1380 <strncmp>
    2564:	e3500000 	cmp	r0, #0	; 0x0
    2568:	02800001 	addeq	r0, r0, #1	; 0x1
    256c:	0a000000 	beq	2574 <do_efuse+0x30>
    2570:	e3a00000 	mov	r0, #0	; 0x0
    2574:	ebfffac4 	bl	108c <efuse_info>
    2578:	e3a00000 	mov	r0, #0	; 0x0
    257c:	e28dd004 	add	sp, sp, #4	; 0x4
    2580:	e8bd8000 	pop	{pc}
    2584:	00016dd9 	.word	0x00016dd9

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
    25ac:	ebfffb73 	bl	1380 <strncmp>
    25b0:	e2501000 	subs	r1, r0, #0	; 0x0
    25b4:	1a00000d 	bne	25f0 <do_pi+0x68>
    25b8:	e3540002 	cmp	r4, #2	; 0x2
    25bc:	01a04001 	moveq	r4, r1
    25c0:	0a000003 	beq	25d4 <do_pi+0x4c>
    25c4:	e5950008 	ldr	r0, [r5, #8]
    25c8:	e3a02010 	mov	r2, #16	; 0x10
    25cc:	eb003de8 	bl	11d74 <simple_strtoul>
    25d0:	e1a04000 	mov	r4, r0
    25d4:	e59f007c 	ldr	r0, [pc, #124]	; 2658 <do_pi+0xd0>
    25d8:	e1a01004 	mov	r1, r4
    25dc:	e59f3078 	ldr	r3, [pc, #120]	; 265c <do_pi+0xd4>
    25e0:	e1a0e00f 	mov	lr, pc
    25e4:	e593f000 	ldr	pc, [r3]
    25e8:	e1a00004 	mov	r0, r4
    25ec:	eb00160d 	bl	7e28 <burn>
    25f0:	e3a04000 	mov	r4, #0	; 0x0
    25f4:	ea000013 	b	2648 <do_pi+0xc0>
    25f8:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    25fc:	eb004170 	bl	12bc4 <ius_check_header>
    2600:	e2504000 	subs	r4, r0, #0	; 0x0
    2604:	159f0054 	ldrne	r0, [pc, #84]	; 2660 <do_pi+0xd8>
    2608:	1a000005 	bne	2624 <do_pi+0x9c>
    260c:	e3a0000d 	mov	r0, #13	; 0xd
    2610:	e3a01001 	mov	r1, #1	; 0x1
    2614:	eb000f4e 	bl	6354 <vs_assign_by_id>
    2618:	e2504000 	subs	r4, r0, #0	; 0x0
    261c:	0a000005 	beq	2638 <do_pi+0xb0>
    2620:	e59f003c 	ldr	r0, [pc, #60]	; 2664 <do_pi+0xdc>
    2624:	e1a01004 	mov	r1, r4
    2628:	e59f302c 	ldr	r3, [pc, #44]	; 265c <do_pi+0xd4>
    262c:	e1a0e00f 	mov	lr, pc
    2630:	e593f000 	ldr	pc, [r3]
    2634:	ea000003 	b	2648 <do_pi+0xc0>
    2638:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    263c:	e3a0100d 	mov	r1, #13	; 0xd
    2640:	e1a02004 	mov	r2, r4
    2644:	eb001563 	bl	7bd8 <burn_images>
    2648:	e1a00004 	mov	r0, r4
    264c:	e28dd004 	add	sp, sp, #4	; 0x4
    2650:	e8bd8030 	pop	{r4, r5, pc}
    2654:	00016ddc 	.word	0x00016ddc
    2658:	00016de1 	.word	0x00016de1
    265c:	3c00a5cc 	.word	0x3c00a5cc
    2660:	00016e0b 	.word	0x00016e0b
    2664:	00016e2b 	.word	0x00016e2b

00002668 <do_spbyte>:
    2668:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    266c:	e5930004 	ldr	r0, [r3, #4]
    2670:	e24dd004 	sub	sp, sp, #4	; 0x4
    2674:	e3a01000 	mov	r1, #0	; 0x0
    2678:	e3a0200a 	mov	r2, #10	; 0xa
    267c:	eb003dbc 	bl	11d74 <simple_strtoul>
    2680:	eb00327c 	bl	f078 <spi_set_bytes>
    2684:	e3a00000 	mov	r0, #0	; 0x0
    2688:	e28dd004 	add	sp, sp, #4	; 0x4
    268c:	e8bd8000 	pop	{pc}

00002690 <do_cfg>:
    2690:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2694:	e5930004 	ldr	r0, [r3, #4]
    2698:	e24dd004 	sub	sp, sp, #4	; 0x4
    269c:	e3a01000 	mov	r1, #0	; 0x0
    26a0:	e3a02010 	mov	r2, #16	; 0x10
    26a4:	eb003db2 	bl	11d74 <simple_strtoul>
    26a8:	eb0013e1 	bl	7634 <cdbg_config_isi>
    26ac:	e3a00000 	mov	r0, #0	; 0x0
    26b0:	e28dd004 	add	sp, sp, #4	; 0x4
    26b4:	e8bd8000 	pop	{pc}

000026b8 <do_sleep>:
    26b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26bc:	e3a01000 	mov	r1, #0	; 0x0
    26c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    26c4:	e3a0200a 	mov	r2, #10	; 0xa
    26c8:	e5930004 	ldr	r0, [r3, #4]
    26cc:	eb003da8 	bl	11d74 <simple_strtoul>
    26d0:	e59f3020 	ldr	r3, [pc, #32]	; 26f8 <do_sleep+0x40>
    26d4:	e0000093 	mul	r0, r3, r0
    26d8:	eb002fec 	bl	e690 <udelay>
    26dc:	e59f0018 	ldr	r0, [pc, #24]	; 26fc <do_sleep+0x44>
    26e0:	e59f3018 	ldr	r3, [pc, #24]	; 2700 <do_sleep+0x48>
    26e4:	e1a0e00f 	mov	lr, pc
    26e8:	e593f000 	ldr	pc, [r3]
    26ec:	e3a00000 	mov	r0, #0	; 0x0
    26f0:	e28dd004 	add	sp, sp, #4	; 0x4
    26f4:	e8bd8000 	pop	{pc}
    26f8:	000f4240 	.word	0x000f4240
    26fc:	00016e46 	.word	0x00016e46
    2700:	3c00a5cc 	.word	0x3c00a5cc

00002704 <do_go>:
    2704:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2708:	e5930004 	ldr	r0, [r3, #4]
    270c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2710:	e3a02010 	mov	r2, #16	; 0x10
    2714:	e3a01000 	mov	r1, #0	; 0x0
    2718:	eb003d95 	bl	11d74 <simple_strtoul>
    271c:	e3a01000 	mov	r1, #0	; 0x0
    2720:	eb0012e9 	bl	72cc <cdbg_jump>
    2724:	e3a00000 	mov	r0, #0	; 0x0
    2728:	e28dd004 	add	sp, sp, #4	; 0x4
    272c:	e8bd8000 	pop	{pc}

00002730 <do_ml>:
    2730:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2734:	e24dd004 	sub	sp, sp, #4	; 0x4
    2738:	ebfffc3f 	bl	183c <__sOINFo>
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
    2764:	eb003d82 	bl	11d74 <simple_strtoul>
    2768:	e6ff0070 	uxth	r0, r0
    276c:	ebfff810 	bl	7b4 <set_xom>
    2770:	e59f5034 	ldr	r5, [pc, #52]	; 27ac <do_bp+0x64>
    2774:	e3a00001 	mov	r0, #1	; 0x1
    2778:	e5954000 	ldr	r4, [r5]
    277c:	ebfff808 	bl	7a4 <get_xom>
    2780:	e1a01000 	mov	r1, r0
    2784:	e59f0024 	ldr	r0, [pc, #36]	; 27b0 <do_bp+0x68>
    2788:	e12fff34 	blx	r4
    278c:	e5954000 	ldr	r4, [r5]
    2790:	ebfff80d 	bl	7cc <boot_info>
    2794:	e1a01000 	mov	r1, r0
    2798:	e59f0014 	ldr	r0, [pc, #20]	; 27b4 <do_bp+0x6c>
    279c:	e12fff34 	blx	r4
    27a0:	e3a00000 	mov	r0, #0	; 0x0
    27a4:	e28dd004 	add	sp, sp, #4	; 0x4
    27a8:	e8bd8030 	pop	{r4, r5, pc}
    27ac:	3c00a5cc 	.word	0x3c00a5cc
    27b0:	00016e51 	.word	0x00016e51
    27b4:	00016e67 	.word	0x00016e67

000027b8 <do_reset>:
    27b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    27bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    27c0:	ebfffa00 	bl	fc8 <system_reset>
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
    28ac:	3c00a040 	.word	0x3c00a040
    28b0:	00016fd8 	.word	0x00016fd8
    28b4:	3c00a5cc 	.word	0x3c00a5cc
    28b8:	00017000 	.word	0x00017000
    28bc:	0001700f 	.word	0x0001700f
    28c0:	00017029 	.word	0x00017029
    28c4:	00017034 	.word	0x00017034
    28c8:	00017042 	.word	0x00017042

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
    28f0:	eb003d1f 	bl	11d74 <simple_strtoul>
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
    292c:	ebfffa85 	bl	1348 <strcmp>
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
    2964:	eb000d78 	bl	5f4c <vs_device_count>
    2968:	e1550000 	cmp	r5, r0
    296c:	e1a01005 	mov	r1, r5
    2970:	e59f02d8 	ldr	r0, [pc, #728]	; 2c50 <do_vs+0x384>
    2974:	e59f62c8 	ldr	r6, [pc, #712]	; 2c44 <do_vs+0x378>
    2978:	bafffff5 	blt	2954 <do_vs+0x88>
    297c:	e59f02d0 	ldr	r0, [pc, #720]	; 2c54 <do_vs+0x388>
    2980:	e1a0e00f 	mov	lr, pc
    2984:	e596f000 	ldr	pc, [r6]
    2988:	eb000ca3 	bl	5c1c <vs_is_assigned>
    298c:	e2504000 	subs	r4, r0, #0	; 0x0
    2990:	0a000009 	beq	29bc <do_vs+0xf0>
    2994:	e59f32bc 	ldr	r3, [pc, #700]	; 2c58 <do_vs+0x38c>
    2998:	e5965000 	ldr	r5, [r6]
    299c:	e5934000 	ldr	r4, [r3]
    29a0:	e1a00004 	mov	r0, r4
    29a4:	eb000d17 	bl	5e08 <vs_align>
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
    29d8:	ebfffa5a 	bl	1348 <strcmp>
    29dc:	e2505000 	subs	r5, r0, #0	; 0x0
    29e0:	1a000087 	bne	2c04 <do_vs+0x338>
    29e4:	eb000e4a 	bl	6314 <vs_reset>
    29e8:	e28d4020 	add	r4, sp, #32	; 0x20
    29ec:	e5240018 	str	r0, [r4, #-24]!
    29f0:	e59f126c 	ldr	r1, [pc, #620]	; 2c64 <do_vs+0x398>
    29f4:	e2840008 	add	r0, r4, #8	; 0x8
    29f8:	ebfffa28 	bl	12a0 <strcpy>
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
    2a24:	ebfffa47 	bl	1348 <strcmp>
    2a28:	e2505000 	subs	r5, r0, #0	; 0x0
    2a2c:	1a000074 	bne	2c04 <do_vs+0x338>
    2a30:	e5940008 	ldr	r0, [r4, #8]
    2a34:	e3a01001 	mov	r1, #1	; 0x1
    2a38:	eb000e63 	bl	63cc <vs_assign_by_name>
    2a3c:	e2507000 	subs	r7, r0, #0	; 0x0
    2a40:	e59f61fc 	ldr	r6, [pc, #508]	; 2c44 <do_vs+0x378>
    2a44:	1a00000b 	bne	2a78 <do_vs+0x1ac>
    2a48:	e5943008 	ldr	r3, [r4, #8]
    2a4c:	e59f2218 	ldr	r2, [pc, #536]	; 2c6c <do_vs+0x3a0>
    2a50:	e1a00003 	mov	r0, r3
    2a54:	e5823000 	str	r3, [r2]
    2a58:	eb000d3d 	bl	5f54 <vs_device_id>
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
    2a98:	eb003d0d 	bl	11ed4 <simple_strtoull>
    2a9c:	e3a02010 	mov	r2, #16	; 0x10
    2aa0:	e1a07000 	mov	r7, r0
    2aa4:	e1a08001 	mov	r8, r1
    2aa8:	e594000c 	ldr	r0, [r4, #12]
    2aac:	e3a01000 	mov	r1, #0	; 0x0
    2ab0:	eb003d07 	bl	11ed4 <simple_strtoull>
    2ab4:	e59f11bc 	ldr	r1, [pc, #444]	; 2c78 <do_vs+0x3ac>
    2ab8:	e1a05000 	mov	r5, r0
    2abc:	e28d0010 	add	r0, sp, #16	; 0x10
    2ac0:	ebfff9f6 	bl	12a0 <strcpy>
    2ac4:	e3a03002 	mov	r3, #2	; 0x2
    2ac8:	e5940004 	ldr	r0, [r4, #4]
    2acc:	e59f11a4 	ldr	r1, [pc, #420]	; 2c78 <do_vs+0x3ac>
    2ad0:	e58d300c 	str	r3, [sp, #12]
    2ad4:	ebfffa1b 	bl	1348 <strcmp>
    2ad8:	e3500000 	cmp	r0, #0	; 0x0
    2adc:	1a000004 	bne	2af4 <do_vs+0x228>
    2ae0:	e1a00007 	mov	r0, r7
    2ae4:	e1a01008 	mov	r1, r8
    2ae8:	e1a02005 	mov	r2, r5
    2aec:	eb000c89 	bl	5d18 <vs_erase>
    2af0:	ea000008 	b	2b18 <do_vs+0x24c>
    2af4:	e5940004 	ldr	r0, [r4, #4]
    2af8:	e59f117c 	ldr	r1, [pc, #380]	; 2c7c <do_vs+0x3b0>
    2afc:	ebfffa11 	bl	1348 <strcmp>
    2b00:	e3500000 	cmp	r0, #0	; 0x0
    2b04:	1a00003e 	bne	2c04 <do_vs+0x338>
    2b08:	e1a00007 	mov	r0, r7
    2b0c:	e1a01008 	mov	r1, r8
    2b10:	e1a02005 	mov	r2, r5
    2b14:	eb000c9d 	bl	5d90 <vs_scrub>
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
    2b48:	eb003c89 	bl	11d74 <simple_strtoul>
    2b4c:	e3a02010 	mov	r2, #16	; 0x10
    2b50:	e1a09000 	mov	r9, r0
    2b54:	e3a01000 	mov	r1, #0	; 0x0
    2b58:	e594000c 	ldr	r0, [r4, #12]
    2b5c:	eb003cdc 	bl	11ed4 <simple_strtoull>
    2b60:	e3a02010 	mov	r2, #16	; 0x10
    2b64:	e1a08000 	mov	r8, r0
    2b68:	e1a0a001 	mov	sl, r1
    2b6c:	e5940010 	ldr	r0, [r4, #16]
    2b70:	e3a01000 	mov	r1, #0	; 0x0
    2b74:	e08db002 	add	fp, sp, r2
    2b78:	eb003cd5 	bl	11ed4 <simple_strtoull>
    2b7c:	e3a03000 	mov	r3, #0	; 0x0
    2b80:	e59f10f8 	ldr	r1, [pc, #248]	; 2c80 <do_vs+0x3b4>
    2b84:	e1a07000 	mov	r7, r0
    2b88:	e1a0000b 	mov	r0, fp
    2b8c:	e58d300c 	str	r3, [sp, #12]
    2b90:	ebfff9c2 	bl	12a0 <strcpy>
    2b94:	e5940004 	ldr	r0, [r4, #4]
    2b98:	e59f10e4 	ldr	r1, [pc, #228]	; 2c84 <do_vs+0x3b8>
    2b9c:	ebfff9e9 	bl	1348 <strcmp>
    2ba0:	e3500000 	cmp	r0, #0	; 0x0
    2ba4:	1a000006 	bne	2bc4 <do_vs+0x2f8>
    2ba8:	e1a00009 	mov	r0, r9
    2bac:	e1a02008 	mov	r2, r8
    2bb0:	e1a0300a 	mov	r3, sl
    2bb4:	e58d7000 	str	r7, [sp]
    2bb8:	e58d5004 	str	r5, [sp, #4]
    2bbc:	eb000c3f 	bl	5cc0 <vs_write>
    2bc0:	eaffffd4 	b	2b18 <do_vs+0x24c>
    2bc4:	e5940004 	ldr	r0, [r4, #4]
    2bc8:	e59f10b8 	ldr	r1, [pc, #184]	; 2c88 <do_vs+0x3bc>
    2bcc:	ebfff9dd 	bl	1348 <strcmp>
    2bd0:	e3500000 	cmp	r0, #0	; 0x0
    2bd4:	1a00000a 	bne	2c04 <do_vs+0x338>
    2bd8:	e1a00009 	mov	r0, r9
    2bdc:	e1a02008 	mov	r2, r8
    2be0:	e1a0300a 	mov	r3, sl
    2be4:	e58d7000 	str	r7, [sp]
    2be8:	e58d5004 	str	r5, [sp, #4]
    2bec:	eb000d05 	bl	6008 <vs_read>
    2bf0:	e59f1094 	ldr	r1, [pc, #148]	; 2c8c <do_vs+0x3c0>
    2bf4:	e58d0008 	str	r0, [sp, #8]
    2bf8:	e1a0000b 	mov	r0, fp
    2bfc:	ebfff9a7 	bl	12a0 <strcpy>
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
    2c40:	00016e96 	.word	0x00016e96
    2c44:	3c00a5cc 	.word	0x3c00a5cc
    2c48:	00017059 	.word	0x00017059
    2c4c:	00015c14 	.word	0x00015c14
    2c50:	00017067 	.word	0x00017067
    2c54:	0001706f 	.word	0x0001706f
    2c58:	3c00a040 	.word	0x3c00a040
    2c5c:	00017081 	.word	0x00017081
    2c60:	0001708b 	.word	0x0001708b
    2c64:	00016e84 	.word	0x00016e84
    2c68:	0001709e 	.word	0x0001709e
    2c6c:	3c00a044 	.word	0x3c00a044
    2c70:	000170a5 	.word	0x000170a5
    2c74:	000170b1 	.word	0x000170b1
    2c78:	000170de 	.word	0x000170de
    2c7c:	000170e4 	.word	0x000170e4
    2c80:	000170ea 	.word	0x000170ea
    2c84:	000170f3 	.word	0x000170f3
    2c88:	000170f9 	.word	0x000170f9
    2c8c:	000170fe 	.word	0x000170fe
    2c90:	00016a6d 	.word	0x00016a6d

00002c94 <cmd_get_data_size>:
    2c94:	e92d4030 	push	{r4, r5, lr}
    2c98:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c9c:	e1a04001 	mov	r4, r1
    2ca0:	e1a05000 	mov	r5, r0
    2ca4:	ebfff9d4 	bl	13fc <strlen>
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
    2d58:	eb003ff3 	bl	12d2c <puts>
    2d5c:	e3a00001 	mov	r0, #1	; 0x1
    2d60:	ea000003 	b	2d74 <cmd_usage+0x60>
    2d64:	eb003ff0 	bl	12d2c <puts>
    2d68:	e3a0000a 	mov	r0, #10	; 0xa
    2d6c:	eb004016 	bl	12dcc <putc>
    2d70:	e3a00000 	mov	r0, #0	; 0x0
    2d74:	e28dd004 	add	sp, sp, #4	; 0x4
    2d78:	e8bd8030 	pop	{r4, r5, pc}
    2d7c:	3c00a5cc 	.word	0x3c00a5cc
    2d80:	000171f3 	.word	0x000171f3
    2d84:	000171fd 	.word	0x000171fd
    2d88:	00017208 	.word	0x00017208

00002d8c <find_cmd_tbl>:
    2d8c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2d90:	e1a05001 	mov	r5, r1
    2d94:	e24dd004 	sub	sp, sp, #4	; 0x4
    2d98:	e3a0102e 	mov	r1, #46	; 0x2e
    2d9c:	e1a07000 	mov	r7, r0
    2da0:	e1a08002 	mov	r8, r2
    2da4:	ebfff989 	bl	13d0 <strchr>
    2da8:	e3500000 	cmp	r0, #0	; 0x0
    2dac:	10676000 	rsbne	r6, r7, r0
    2db0:	1a000002 	bne	2dc0 <find_cmd_tbl+0x34>
    2db4:	e1a00007 	mov	r0, r7
    2db8:	ebfff98f 	bl	13fc <strlen>
    2dbc:	e1a06000 	mov	r6, r0
    2dc0:	e3a03018 	mov	r3, #24	; 0x18
    2dc4:	e02a5893 	mla	sl, r3, r8, r5
    2dc8:	e1a04005 	mov	r4, r5
    2dcc:	e1a08005 	mov	r8, r5
    2dd0:	e3a05000 	mov	r5, #0	; 0x0
    2dd4:	ea00000a 	b	2e04 <find_cmd_tbl+0x78>
    2dd8:	e5941000 	ldr	r1, [r4]
    2ddc:	ebfff967 	bl	1380 <strncmp>
    2de0:	e3500000 	cmp	r0, #0	; 0x0
    2de4:	1a000005 	bne	2e00 <find_cmd_tbl+0x74>
    2de8:	e5940000 	ldr	r0, [r4]
    2dec:	ebfff982 	bl	13fc <strlen>
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
    2e48:	0001988c 	.word	0x0001988c
    2e4c:	00019bec 	.word	0x00019bec
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
    2ed4:	ebfff91b 	bl	1348 <strcmp>
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
    2f3c:	eb003fa5 	bl	12dd8 <ctrlc>
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
    2fe0:	3c00a5cc 	.word	0x3c00a5cc
    2fe4:	00017229 	.word	0x00017229
    2fe8:	00017233 	.word	0x00017233

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
    3030:	00019bec 	.word	0x00019bec
    3034:	0001988c 	.word	0x0001988c
    3038:	aaaaaaab 	.word	0xaaaaaaab

0000303c <tt_blk_trolling>:
    303c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3040:	e3a01001 	mov	r1, #1	; 0x1
    3044:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    3048:	e1a04002 	mov	r4, r2
    304c:	e1a05003 	mov	r5, r3
    3050:	e58d0010 	str	r0, [sp, #16]
    3054:	e59da050 	ldr	sl, [sp, #80]
    3058:	eb000cdb 	bl	63cc <vs_assign_by_name>
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
    30a4:	eb000b57 	bl	5e08 <vs_align>
    30a8:	e1a06004 	mov	r6, r4
    30ac:	e1a07005 	mov	r7, r5
    30b0:	ea000002 	b	30c0 <tt_blk_trolling+0x84>
    30b4:	e1a06004 	mov	r6, r4
    30b8:	e1a07005 	mov	r7, r5
    30bc:	e3a04502 	mov	r4, #8388608	; 0x800000
    30c0:	e3a0000e 	mov	r0, #14	; 0xe
    30c4:	eb000b4f 	bl	5e08 <vs_align>
    30c8:	e1a01000 	mov	r1, r0
    30cc:	e1a00004 	mov	r0, r4
    30d0:	eb00472b 	bl	14d84 <__aeabi_uidiv>
    30d4:	e1a01000 	mov	r1, r0
    30d8:	e3a00000 	mov	r0, #0	; 0x0
    30dc:	eb00054e 	bl	461c <tt_rnd_get_int>
    30e0:	e1a04000 	mov	r4, r0
    30e4:	e3a0000e 	mov	r0, #14	; 0xe
    30e8:	eb000b46 	bl	5e08 <vs_align>
    30ec:	e0040094 	mul	r4, r4, r0
    30f0:	e28d0024 	add	r0, sp, #36	; 0x24
    30f4:	e1a01004 	mov	r1, r4
    30f8:	e1a02004 	mov	r2, r4
    30fc:	eb00056f 	bl	46c0 <tt_rnd_get_data>
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
    3130:	eb000b34 	bl	5e08 <vs_align>
    3134:	e3550000 	cmp	r5, #0	; 0x0
    3138:	e1cd41f8 	strd	r4, [sp, #24]
    313c:	1a000005 	bne	3158 <tt_blk_trolling+0x11c>
    3140:	e1a01000 	mov	r1, r0
    3144:	e1a00004 	mov	r0, r4
    3148:	eb00470d 	bl	14d84 <__aeabi_uidiv>
    314c:	e58d501c 	str	r5, [sp, #28]
    3150:	e58d0018 	str	r0, [sp, #24]
    3154:	ea000002 	b	3164 <tt_blk_trolling+0x128>
    3158:	e1a01000 	mov	r1, r0
    315c:	e28d0018 	add	r0, sp, #24	; 0x18
    3160:	eb003ac6 	bl	11c80 <__div64_32>
    3164:	e59d1018 	ldr	r1, [sp, #24]
    3168:	e3a00000 	mov	r0, #0	; 0x0
    316c:	eb00052a 	bl	461c <tt_rnd_get_int>
    3170:	e1a04000 	mov	r4, r0
    3174:	e3a0000e 	mov	r0, #14	; 0xe
    3178:	eb000b22 	bl	5e08 <vs_align>
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
    31a4:	eb000b6a 	bl	5f54 <vs_device_id>
    31a8:	eb000b5d 	bl	5f24 <vs_device_erasable>
    31ac:	e3500000 	cmp	r0, #0	; 0x0
    31b0:	0a000006 	beq	31d0 <tt_blk_trolling+0x194>
    31b4:	e59f0298 	ldr	r0, [pc, #664]	; 3454 <tt_blk_trolling+0x418>
    31b8:	e1a0e00f 	mov	lr, pc
    31bc:	e594f000 	ldr	pc, [r4]
    31c0:	e1a00008 	mov	r0, r8
    31c4:	e1a01009 	mov	r1, r9
    31c8:	e59d2024 	ldr	r2, [sp, #36]
    31cc:	eb000ad1 	bl	5d18 <vs_erase>
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
    3214:	ebfff892 	bl	1464 <memcpy>
    3218:	e3a00000 	mov	r0, #0	; 0x0
    321c:	eb002d3f 	bl	e720 <get_timer>
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
    3250:	eb000a9a 	bl	5cc0 <vs_write>
    3254:	e59d0008 	ldr	r0, [sp, #8]
    3258:	eb002d30 	bl	e720 <get_timer>
    325c:	e0656006 	rsb	r6, r5, r6
    3260:	e0877000 	add	r7, r7, r0
    3264:	e3560000 	cmp	r6, #0	; 0x0
    3268:	e1a0000a 	mov	r0, sl
    326c:	1affffe0 	bne	31f4 <tt_blk_trolling+0x1b8>
    3270:	ea00000b 	b	32a4 <tt_blk_trolling+0x268>
    3274:	e1a0000a 	mov	r0, sl
    3278:	eb002d28 	bl	e720 <get_timer>
    327c:	e59d1024 	ldr	r1, [sp, #36]
    3280:	e1a04000 	mov	r4, r0
    3284:	e1a02008 	mov	r2, r8
    3288:	e1a03009 	mov	r3, r9
    328c:	e1a0000b 	mov	r0, fp
    3290:	e88d0402 	stm	sp, {r1, sl}
    3294:	eb000a89 	bl	5cc0 <vs_write>
    3298:	e1a00004 	mov	r0, r4
    329c:	eb002d1f 	bl	e720 <get_timer>
    32a0:	e1a07000 	mov	r7, r0
    32a4:	e59d0024 	ldr	r0, [sp, #36]
    32a8:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    32ac:	e1a00540 	asr	r0, r0, #10
    32b0:	e1a01007 	mov	r1, r7
    32b4:	e0000093 	mul	r0, r3, r0
    32b8:	eb0046b1 	bl	14d84 <__aeabi_uidiv>
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
    32e4:	eb002d0d 	bl	e720 <get_timer>
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
    3320:	eb000b38 	bl	6008 <vs_read>
    3324:	e1a00005 	mov	r0, r5
    3328:	eb002cfc 	bl	e720 <get_timer>
    332c:	e1a05000 	mov	r5, r0
    3330:	e59d0024 	ldr	r0, [sp, #36]
    3334:	e1a0100a 	mov	r1, sl
    3338:	e0660000 	rsb	r0, r6, r0
    333c:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3340:	e1a02004 	mov	r2, r4
    3344:	ebfff846 	bl	1464 <memcpy>
    3348:	e0877005 	add	r7, r7, r5
    334c:	e0646006 	rsb	r6, r4, r6
    3350:	e3a00000 	mov	r0, #0	; 0x0
    3354:	e1560000 	cmp	r6, r0
    3358:	1affffe1 	bne	32e4 <tt_blk_trolling+0x2a8>
    335c:	ea00000b 	b	3390 <tt_blk_trolling+0x354>
    3360:	e1a0000a 	mov	r0, sl
    3364:	eb002ced 	bl	e720 <get_timer>
    3368:	e59d1024 	ldr	r1, [sp, #36]
    336c:	e1a04000 	mov	r4, r0
    3370:	e1a02008 	mov	r2, r8
    3374:	e1a03009 	mov	r3, r9
    3378:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    337c:	e88d0402 	stm	sp, {r1, sl}
    3380:	eb000b20 	bl	6008 <vs_read>
    3384:	e1a00004 	mov	r0, r4
    3388:	eb002ce4 	bl	e720 <get_timer>
    338c:	e1a07000 	mov	r7, r0
    3390:	e59d0024 	ldr	r0, [sp, #36]
    3394:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3398:	e1a00540 	asr	r0, r0, #10
    339c:	e0000093 	mul	r0, r3, r0
    33a0:	e1a01007 	mov	r1, r7
    33a4:	eb004676 	bl	14d84 <__aeabi_uidiv>
    33a8:	e59f4098 	ldr	r4, [pc, #152]	; 3448 <tt_blk_trolling+0x40c>
    33ac:	e1a05000 	mov	r5, r0
    33b0:	e59f00a8 	ldr	r0, [pc, #168]	; 3460 <tt_blk_trolling+0x424>
    33b4:	e1a0e00f 	mov	lr, pc
    33b8:	e594f000 	ldr	pc, [r4]
    33bc:	e1a0000b 	mov	r0, fp
    33c0:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    33c4:	e59d2024 	ldr	r2, [sp, #36]
    33c8:	e3a0300c 	mov	r3, #12	; 0xc
    33cc:	eb00043e 	bl	44cc <tt_cmp_buffer>
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
    3444:	000173da 	.word	0x000173da
    3448:	3c00a5cc 	.word	0x3c00a5cc
    344c:	000173f8 	.word	0x000173f8
    3450:	00017418 	.word	0x00017418
    3454:	00016ef4 	.word	0x00016ef4
    3458:	00016bf3 	.word	0x00016bf3
    345c:	00017f32 	.word	0x00017f32
    3460:	0001744c 	.word	0x0001744c
    3464:	0001744f 	.word	0x0001744f
    3468:	00017489 	.word	0x00017489
    346c:	3c00a048 	.word	0x3c00a048
    3470:	000171fb 	.word	0x000171fb

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
    34c0:	eb003a2b 	bl	11d74 <simple_strtoul>
    34c4:	e59f118c 	ldr	r1, [pc, #396]	; 3658 <do_tt_blk+0x1e4>
    34c8:	e1a04000 	mov	r4, r0
    34cc:	e3a02005 	mov	r2, #5	; 0x5
    34d0:	e5970004 	ldr	r0, [r7, #4]
    34d4:	ebfff7a9 	bl	1380 <strncmp>
    34d8:	e3500000 	cmp	r0, #0	; 0x0
    34dc:	059f0178 	ldreq	r0, [pc, #376]	; 365c <do_tt_blk+0x1e8>
    34e0:	01a01004 	moveq	r1, r4
    34e4:	059f3168 	ldreq	r3, [pc, #360]	; 3654 <do_tt_blk+0x1e0>
    34e8:	0a000008 	beq	3510 <do_tt_blk+0x9c>
    34ec:	e59f116c 	ldr	r1, [pc, #364]	; 3660 <do_tt_blk+0x1ec>
    34f0:	e5970004 	ldr	r0, [r7, #4]
    34f4:	e3a02003 	mov	r2, #3	; 0x3
    34f8:	ebfff7a0 	bl	1380 <strncmp>
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
    3530:	eb003a0f 	bl	11d74 <simple_strtoul>
    3534:	e1a06000 	mov	r6, r0
    3538:	e1a01006 	mov	r1, r6
    353c:	e59f0124 	ldr	r0, [pc, #292]	; 3668 <do_tt_blk+0x1f4>
    3540:	e59f310c 	ldr	r3, [pc, #268]	; 3654 <do_tt_blk+0x1e0>
    3544:	e1a0e00f 	mov	lr, pc
    3548:	e593f000 	ldr	pc, [r3]
    354c:	e3a00000 	mov	r0, #0	; 0x0
    3550:	eb002c72 	bl	e720 <get_timer>
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
    35a0:	ebfff776 	bl	1380 <strncmp>
    35a4:	e3500000 	cmp	r0, #0	; 0x0
    35a8:	e3a02003 	mov	r2, #3	; 0x3
    35ac:	e59f10ac 	ldr	r1, [pc, #172]	; 3660 <do_tt_blk+0x1ec>
    35b0:	e1a03008 	mov	r3, r8
    35b4:	05950000 	ldreq	r0, [r5]
    35b8:	01a02007 	moveq	r2, r7
    35bc:	0a000008 	beq	35e4 <do_tt_blk+0x170>
    35c0:	e5950000 	ldr	r0, [r5]
    35c4:	ebfff76d 	bl	1380 <strncmp>
    35c8:	e3500000 	cmp	r0, #0	; 0x0
    35cc:	05950000 	ldreq	r0, [r5]
    35d0:	15950000 	ldrne	r0, [r5]
    35d4:	e1a02009 	mov	r2, r9
    35d8:	e1a0300a 	mov	r3, sl
    35dc:	01a02007 	moveq	r2, r7
    35e0:	01a03008 	moveq	r3, r8
    35e4:	e58d6000 	str	r6, [sp]
    35e8:	ebfffe93 	bl	303c <tt_blk_trolling>
    35ec:	eb003df7 	bl	12dd0 <tstc>
    35f0:	e3500000 	cmp	r0, #0	; 0x0
    35f4:	e2844001 	add	r4, r4, #1	; 0x1
    35f8:	0affffe0 	beq	3580 <do_tt_blk+0x10c>
    35fc:	eb003df4 	bl	12dd4 <getc>
    3600:	e3500003 	cmp	r0, #3	; 0x3
    3604:	1affffdd 	bne	3580 <do_tt_blk+0x10c>
    3608:	e1a0000b 	mov	r0, fp
    360c:	eb002c43 	bl	e720 <get_timer>
    3610:	e59f3054 	ldr	r3, [pc, #84]	; 366c <do_tt_blk+0x1f8>
    3614:	e30e1a60 	movw	r1, #60000	; 0xea60
    3618:	e5934000 	ldr	r4, [r3]
    361c:	e5933004 	ldr	r3, [r3, #4]
    3620:	e1a04a24 	lsr	r4, r4, #20
    3624:	e1844603 	orr	r4, r4, r3, lsl #12
    3628:	eb0045d5 	bl	14d84 <__aeabi_uidiv>
    362c:	e1a01004 	mov	r1, r4
    3630:	e1a02000 	mov	r2, r0
    3634:	e59f3018 	ldr	r3, [pc, #24]	; 3654 <do_tt_blk+0x1e0>
    3638:	e59f0034 	ldr	r0, [pc, #52]	; 3674 <do_tt_blk+0x200>
    363c:	e1a0e00f 	mov	lr, pc
    3640:	e593f000 	ldr	pc, [r3]
    3644:	e3a00000 	mov	r0, #0	; 0x0
    3648:	e28dd00c 	add	sp, sp, #12	; 0xc
    364c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3650:	000174b6 	.word	0x000174b6
    3654:	3c00a5cc 	.word	0x3c00a5cc
    3658:	000174fd 	.word	0x000174fd
    365c:	00017503 	.word	0x00017503
    3660:	00016ecb 	.word	0x00016ecb
    3664:	00017527 	.word	0x00017527
    3668:	0001754b 	.word	0x0001754b
    366c:	3c00a048 	.word	0x3c00a048
    3670:	0001756e 	.word	0x0001756e
    3674:	00017578 	.word	0x00017578

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
    36dc:	3c00a5cc 	.word	0x3c00a5cc
    36e0:	000175c6 	.word	0x000175c6
    36e4:	00019625 	.word	0x00019625
    36e8:	000171fb 	.word	0x000171fb

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
    3718:	ebfff751 	bl	1464 <memcpy>
    371c:	e1a02009 	mov	r2, r9
    3720:	e59f11dc 	ldr	r1, [pc, #476]	; 3904 <do_crypto_quick+0x218>
    3724:	e1a00007 	mov	r0, r7
    3728:	ebfff74d 	bl	1464 <memcpy>
    372c:	e08d8005 	add	r8, sp, r5
    3730:	e1a02005 	mov	r2, r5
    3734:	e59f11cc 	ldr	r1, [pc, #460]	; 3908 <do_crypto_quick+0x21c>
    3738:	e1a00006 	mov	r0, r6
    373c:	ebfff748 	bl	1464 <memcpy>
    3740:	e1a02005 	mov	r2, r5
    3744:	e59f11c0 	ldr	r1, [pc, #448]	; 390c <do_crypto_quick+0x220>
    3748:	e1a00008 	mov	r0, r8
    374c:	ebfff744 	bl	1464 <memcpy>
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
    379c:	eb000b4d 	bl	64d8 <ss_aes_init>
    37a0:	e1a0100d 	mov	r1, sp
    37a4:	e1a02005 	mov	r2, r5
    37a8:	e1a00006 	mov	r0, r6
    37ac:	eb000b38 	bl	6494 <ss_aes_data>
    37b0:	e1a0100d 	mov	r1, sp
    37b4:	e1a02005 	mov	r2, r5
    37b8:	e59f0160 	ldr	r0, [pc, #352]	; 3920 <do_crypto_quick+0x234>
    37bc:	ebffffad 	bl	3678 <__show_strbuf>
    37c0:	e1a01007 	mov	r1, r7
    37c4:	e1a0200a 	mov	r2, sl
    37c8:	e3a00000 	mov	r0, #0	; 0x0
    37cc:	eb000b41 	bl	64d8 <ss_aes_init>
    37d0:	e59f714c 	ldr	r7, [pc, #332]	; 3924 <do_crypto_quick+0x238>
    37d4:	e1a0100d 	mov	r1, sp
    37d8:	e1a02005 	mov	r2, r5
    37dc:	e1a00008 	mov	r0, r8
    37e0:	eb000b2b 	bl	6494 <ss_aes_data>
    37e4:	e1a0100d 	mov	r1, sp
    37e8:	e1a02005 	mov	r2, r5
    37ec:	e59f0134 	ldr	r0, [pc, #308]	; 3928 <do_crypto_quick+0x23c>
    37f0:	ebffffa0 	bl	3678 <__show_strbuf>
    37f4:	e1a02007 	mov	r2, r7
    37f8:	e3a01000 	mov	r1, #0	; 0x0
    37fc:	e3a00001 	mov	r0, #1	; 0x1
    3800:	eb000b34 	bl	64d8 <ss_aes_init>
    3804:	e1a0100d 	mov	r1, sp
    3808:	e1a02005 	mov	r2, r5
    380c:	e1a00006 	mov	r0, r6
    3810:	eb000b1f 	bl	6494 <ss_aes_data>
    3814:	e1a0100d 	mov	r1, sp
    3818:	e1a02005 	mov	r2, r5
    381c:	e59f0108 	ldr	r0, [pc, #264]	; 392c <do_crypto_quick+0x240>
    3820:	ebffff94 	bl	3678 <__show_strbuf>
    3824:	e3a00000 	mov	r0, #0	; 0x0
    3828:	e1a02007 	mov	r2, r7
    382c:	e1a01000 	mov	r1, r0
    3830:	eb000b28 	bl	64d8 <ss_aes_init>
    3834:	e1a0100d 	mov	r1, sp
    3838:	e1a02005 	mov	r2, r5
    383c:	e1a0000d 	mov	r0, sp
    3840:	eb000b13 	bl	6494 <ss_aes_data>
    3844:	e1a02005 	mov	r2, r5
    3848:	e1a0100d 	mov	r1, sp
    384c:	e59f00dc 	ldr	r0, [pc, #220]	; 3930 <do_crypto_quick+0x244>
    3850:	ebffff88 	bl	3678 <__show_strbuf>
    3854:	e1a01005 	mov	r1, r5
    3858:	e3a00001 	mov	r0, #1	; 0x1
    385c:	eb000c36 	bl	693c <hash_init>
    3860:	e1a01005 	mov	r1, r5
    3864:	e1a00006 	mov	r0, r6
    3868:	eb000c13 	bl	68bc <hash_data>
    386c:	e1a0000d 	mov	r0, sp
    3870:	eb000bf5 	bl	684c <hash_value>
    3874:	eb000bed 	bl	6830 <hash_len>
    3878:	e1a0100d 	mov	r1, sp
    387c:	e1a02000 	mov	r2, r0
    3880:	e59f00ac 	ldr	r0, [pc, #172]	; 3934 <do_crypto_quick+0x248>
    3884:	ebffff7b 	bl	3678 <__show_strbuf>
    3888:	e1a01005 	mov	r1, r5
    388c:	e3a00002 	mov	r0, #2	; 0x2
    3890:	eb000c29 	bl	693c <hash_init>
    3894:	e1a01005 	mov	r1, r5
    3898:	e1a00006 	mov	r0, r6
    389c:	eb000c06 	bl	68bc <hash_data>
    38a0:	e1a0000d 	mov	r0, sp
    38a4:	eb000be8 	bl	684c <hash_value>
    38a8:	eb000be0 	bl	6830 <hash_len>
    38ac:	e1a0100d 	mov	r1, sp
    38b0:	e1a02000 	mov	r2, r0
    38b4:	e59f007c 	ldr	r0, [pc, #124]	; 3938 <do_crypto_quick+0x24c>
    38b8:	ebffff6e 	bl	3678 <__show_strbuf>
    38bc:	e1a01005 	mov	r1, r5
    38c0:	e3a00003 	mov	r0, #3	; 0x3
    38c4:	eb000c1c 	bl	693c <hash_init>
    38c8:	e1a01005 	mov	r1, r5
    38cc:	e1a00006 	mov	r0, r6
    38d0:	eb000bf9 	bl	68bc <hash_data>
    38d4:	e1a0000d 	mov	r0, sp
    38d8:	eb000bdb 	bl	684c <hash_value>
    38dc:	eb000bd3 	bl	6830 <hash_len>
    38e0:	e1a0100d 	mov	r1, sp
    38e4:	e1a02000 	mov	r2, r0
    38e8:	e59f004c 	ldr	r0, [pc, #76]	; 393c <do_crypto_quick+0x250>
    38ec:	ebffff61 	bl	3678 <__show_strbuf>
    38f0:	e1a0400d 	mov	r4, sp
    38f4:	e3a00000 	mov	r0, #0	; 0x0
    38f8:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    38fc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3900:	00015b4c 	.word	0x00015b4c
    3904:	00015b2c 	.word	0x00015b2c
    3908:	00015aec 	.word	0x00015aec
    390c:	00015aac 	.word	0x00015aac
    3910:	000175cb 	.word	0x000175cb
    3914:	000175ce 	.word	0x000175ce
    3918:	000175d2 	.word	0x000175d2
    391c:	000175dd 	.word	0x000175dd
    3920:	000175ee 	.word	0x000175ee
    3924:	3c00a004 	.word	0x3c00a004
    3928:	00017601 	.word	0x00017601
    392c:	000175ea 	.word	0x000175ea
    3930:	000175fd 	.word	0x000175fd
    3934:	00017610 	.word	0x00017610
    3938:	00017614 	.word	0x00017614
    393c:	00017619 	.word	0x00017619

00003940 <tt_crypto_hash>:
    3940:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3944:	e3a0548e 	mov	r5, #-1912602624	; 0x8e000000
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
    3984:	eb002b41 	bl	e690 <udelay>
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
    39d8:	eb000bd7 	bl	693c <hash_init>
    39dc:	eb000b93 	bl	6830 <hash_len>
    39e0:	e1a03000 	mov	r3, r0
    39e4:	e59f01a4 	ldr	r0, [pc, #420]	; 3b90 <tt_crypto_hash+0x250>
    39e8:	e3a0c020 	mov	ip, #32	; 0x20
    39ec:	e1a01000 	mov	r1, r0
    39f0:	e3a02001 	mov	r2, #1	; 0x1
    39f4:	e58dc000 	str	ip, [sp]
    39f8:	eb003845 	bl	11b14 <print_buffer>
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
    3a2c:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3a30:	e2811030 	add	r1, r1, #48	; 0x30
    3a34:	e1a02004 	mov	r2, r4
    3a38:	ebfff689 	bl	1464 <memcpy>
    3a3c:	e3a00000 	mov	r0, #0	; 0x0
    3a40:	eb002b36 	bl	e720 <get_timer>
    3a44:	e3550901 	cmp	r5, #16384	; 0x4000
    3a48:	b1a01005 	movlt	r1, r5
    3a4c:	a3a01901 	movge	r1, #16384	; 0x4000
    3a50:	e1a07000 	mov	r7, r0
    3a54:	e1a00006 	mov	r0, r6
    3a58:	eb000b97 	bl	68bc <hash_data>
    3a5c:	e3540902 	cmp	r4, #32768	; 0x8000
    3a60:	e0645005 	rsb	r5, r4, r5
    3a64:	1a000002 	bne	3a74 <tt_crypto_hash+0x134>
    3a68:	e3a01901 	mov	r1, #16384	; 0x4000
    3a6c:	e1a00008 	mov	r0, r8
    3a70:	eb000b91 	bl	68bc <hash_data>
    3a74:	e1a00007 	mov	r0, r7
    3a78:	eb002b28 	bl	e720 <get_timer>
    3a7c:	e08aa000 	add	sl, sl, r0
    3a80:	e3550000 	cmp	r5, #0	; 0x0
    3a84:	e1a00006 	mov	r0, r6
    3a88:	1affffe2 	bne	3a18 <tt_crypto_hash+0xd8>
    3a8c:	ea000008 	b	3ab4 <tt_crypto_hash+0x174>
    3a90:	e1a00006 	mov	r0, r6
    3a94:	eb002b21 	bl	e720 <get_timer>
    3a98:	e5951008 	ldr	r1, [r5, #8]
    3a9c:	e1a04000 	mov	r4, r0
    3aa0:	e59f00ec 	ldr	r0, [pc, #236]	; 3b94 <tt_crypto_hash+0x254>
    3aa4:	eb000b84 	bl	68bc <hash_data>
    3aa8:	e1a00004 	mov	r0, r4
    3aac:	eb002b1b 	bl	e720 <get_timer>
    3ab0:	e1a0a000 	mov	sl, r0
    3ab4:	e28d4008 	add	r4, sp, #8	; 0x8
    3ab8:	e3a0648e 	mov	r6, #-1912602624	; 0x8e000000
    3abc:	e1a00004 	mov	r0, r4
    3ac0:	eb000b61 	bl	684c <hash_value>
    3ac4:	e5965008 	ldr	r5, [r6, #8]
    3ac8:	eb000b58 	bl	6830 <hash_len>
    3acc:	e3a0c020 	mov	ip, #32	; 0x20
    3ad0:	e1a03000 	mov	r3, r0
    3ad4:	e1a01004 	mov	r1, r4
    3ad8:	e3a02001 	mov	r2, #1	; 0x1
    3adc:	e1a00004 	mov	r0, r4
    3ae0:	e58dc000 	str	ip, [sp]
    3ae4:	eb00380a 	bl	11b14 <print_buffer>
    3ae8:	e5963004 	ldr	r3, [r6, #4]
    3aec:	e28d1038 	add	r1, sp, #56	; 0x38
    3af0:	e0812103 	add	r2, r1, r3, lsl #2
    3af4:	e59f3084 	ldr	r3, [pc, #132]	; 3b80 <tt_crypto_hash+0x240>
    3af8:	e5127010 	ldr	r7, [r2, #-16]
    3afc:	e5938000 	ldr	r8, [r3]
    3b00:	eb000b4a 	bl	6830 <hash_len>
    3b04:	e1a01004 	mov	r1, r4
    3b08:	e1a02000 	mov	r2, r0
    3b0c:	e59f007c 	ldr	r0, [pc, #124]	; 3b90 <tt_crypto_hash+0x250>
    3b10:	ebfff674 	bl	14e8 <memcmp>
    3b14:	e1a05525 	lsr	r5, r5, #10
    3b18:	e3500000 	cmp	r0, #0	; 0x0
    3b1c:	e59f3074 	ldr	r3, [pc, #116]	; 3b98 <tt_crypto_hash+0x258>
    3b20:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3b24:	e1a0100a 	mov	r1, sl
    3b28:	e0000590 	mul	r0, r0, r5
    3b2c:	e59f4068 	ldr	r4, [pc, #104]	; 3b9c <tt_crypto_hash+0x25c>
    3b30:	01a04003 	moveq	r4, r3
    3b34:	eb004492 	bl	14d84 <__aeabi_uidiv>
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
    3b78:	00015b5c 	.word	0x00015b5c
    3b7c:	00017620 	.word	0x00017620
    3b80:	3c00a5cc 	.word	0x3c00a5cc
    3b84:	004c4b40 	.word	0x004c4b40
    3b88:	0001763f 	.word	0x0001763f
    3b8c:	0001765a 	.word	0x0001765a
    3b90:	8e000010 	.word	0x8e000010
    3b94:	8e000030 	.word	0x8e000030
    3b98:	00017675 	.word	0x00017675
    3b9c:	0001767a 	.word	0x0001767a
    3ba0:	00017681 	.word	0x00017681
    3ba4:	3c00a058 	.word	0x3c00a058

00003ba8 <tt_crypto_aes>:
    3ba8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3bac:	e24dd014 	sub	sp, sp, #20	; 0x14
    3bb0:	e1a06000 	mov	r6, r0
    3bb4:	e3a01000 	mov	r1, #0	; 0x0
    3bb8:	e28d000c 	add	r0, sp, #12	; 0xc
    3bbc:	e3a02502 	mov	r2, #8388608	; 0x800000
    3bc0:	eb0002be 	bl	46c0 <tt_rnd_get_data>
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
    3c0c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3c10:	ebfff613 	bl	1464 <memcpy>
    3c14:	e59f0304 	ldr	r0, [pc, #772]	; 3f20 <tt_crypto_aes+0x378>
    3c18:	e1a0e00f 	mov	lr, pc
    3c1c:	e594f000 	ldr	pc, [r4]
    3c20:	e3a01000 	mov	r1, #0	; 0x0
    3c24:	e59f22f8 	ldr	r2, [pc, #760]	; 3f24 <tt_crypto_aes+0x37c>
    3c28:	e3a00001 	mov	r0, #1	; 0x1
    3c2c:	eb000a29 	bl	64d8 <ss_aes_init>
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
    3c68:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3c6c:	e1a02007 	mov	r2, r7
    3c70:	ebfff5fb 	bl	1464 <memcpy>
    3c74:	e3a00000 	mov	r0, #0	; 0x0
    3c78:	eb002aa8 	bl	e720 <get_timer>
    3c7c:	e3550901 	cmp	r5, #16384	; 0x4000
    3c80:	b1a02005 	movlt	r2, r5
    3c84:	a3a02901 	movge	r2, #16384	; 0x4000
    3c88:	e1a04000 	mov	r4, r0
    3c8c:	e1a01006 	mov	r1, r6
    3c90:	e1a00006 	mov	r0, r6
    3c94:	eb0009fe 	bl	6494 <ss_aes_data>
    3c98:	e3570902 	cmp	r7, #32768	; 0x8000
    3c9c:	1a000003 	bne	3cb0 <tt_crypto_aes+0x108>
    3ca0:	e1a01008 	mov	r1, r8
    3ca4:	e3a02901 	mov	r2, #16384	; 0x4000
    3ca8:	e1a00008 	mov	r0, r8
    3cac:	eb0009f8 	bl	6494 <ss_aes_data>
    3cb0:	e1a00004 	mov	r0, r4
    3cb4:	eb002a99 	bl	e720 <get_timer>
    3cb8:	e1a04000 	mov	r4, r0
    3cbc:	e59d000c 	ldr	r0, [sp, #12]
    3cc0:	e1a01006 	mov	r1, r6
    3cc4:	e0650000 	rsb	r0, r5, r0
    3cc8:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3ccc:	e1a02007 	mov	r2, r7
    3cd0:	ebfff5e3 	bl	1464 <memcpy>
    3cd4:	e08aa004 	add	sl, sl, r4
    3cd8:	e0675005 	rsb	r5, r7, r5
    3cdc:	e3550000 	cmp	r5, #0	; 0x0
    3ce0:	e1a00006 	mov	r0, r6
    3ce4:	1affffda 	bne	3c54 <tt_crypto_aes+0xac>
    3ce8:	ea000009 	b	3d14 <tt_crypto_aes+0x16c>
    3cec:	e1a00006 	mov	r0, r6
    3cf0:	eb002a8a 	bl	e720 <get_timer>
    3cf4:	e1a04000 	mov	r4, r0
    3cf8:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3cfc:	e1a01000 	mov	r1, r0
    3d00:	e59d200c 	ldr	r2, [sp, #12]
    3d04:	eb0009e2 	bl	6494 <ss_aes_data>
    3d08:	e1a00004 	mov	r0, r4
    3d0c:	eb002a83 	bl	e720 <get_timer>
    3d10:	e1a0a000 	mov	sl, r0
    3d14:	e59d300c 	ldr	r3, [sp, #12]
    3d18:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3d1c:	e1a0100b 	mov	r1, fp
    3d20:	e3a02010 	mov	r2, #16	; 0x10
    3d24:	e58d3004 	str	r3, [sp, #4]
    3d28:	ebfff5ee 	bl	14e8 <memcmp>
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
    3d64:	eb0009db 	bl	64d8 <ss_aes_init>
    3d68:	e59f01c4 	ldr	r0, [pc, #452]	; 3f34 <tt_crypto_aes+0x38c>
    3d6c:	e1a0e00f 	mov	lr, pc
    3d70:	e594f000 	ldr	pc, [r4]
    3d74:	e3560000 	cmp	r6, #0	; 0x0
    3d78:	159d500c 	ldrne	r5, [sp, #12]
    3d7c:	13a08000 	movne	r8, #0	; 0x0
    3d80:	12869901 	addne	r9, r6, #16384	; 0x4000
    3d84:	1a000022 	bne	3e14 <tt_crypto_aes+0x26c>
    3d88:	ea000025 	b	3e24 <tt_crypto_aes+0x27c>
    3d8c:	eb002a63 	bl	e720 <get_timer>
    3d90:	e59d100c 	ldr	r1, [sp, #12]
    3d94:	e3550902 	cmp	r5, #32768	; 0x8000
    3d98:	b1a07005 	movlt	r7, r5
    3d9c:	a3a07902 	movge	r7, #32768	; 0x8000
    3da0:	e0651001 	rsb	r1, r5, r1
    3da4:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3da8:	e1a02007 	mov	r2, r7
    3dac:	e1a04000 	mov	r4, r0
    3db0:	e1a00006 	mov	r0, r6
    3db4:	ebfff5aa 	bl	1464 <memcpy>
    3db8:	e3550901 	cmp	r5, #16384	; 0x4000
    3dbc:	b1a02005 	movlt	r2, r5
    3dc0:	a3a02901 	movge	r2, #16384	; 0x4000
    3dc4:	e1a00006 	mov	r0, r6
    3dc8:	e1a01006 	mov	r1, r6
    3dcc:	eb0009b0 	bl	6494 <ss_aes_data>
    3dd0:	e3570902 	cmp	r7, #32768	; 0x8000
    3dd4:	1a000003 	bne	3de8 <tt_crypto_aes+0x240>
    3dd8:	e1a01009 	mov	r1, r9
    3ddc:	e3a02901 	mov	r2, #16384	; 0x4000
    3de0:	e1a00009 	mov	r0, r9
    3de4:	eb0009aa 	bl	6494 <ss_aes_data>
    3de8:	e1a00004 	mov	r0, r4
    3dec:	eb002a4b 	bl	e720 <get_timer>
    3df0:	e1a04000 	mov	r4, r0
    3df4:	e59d000c 	ldr	r0, [sp, #12]
    3df8:	e1a01006 	mov	r1, r6
    3dfc:	e0650000 	rsb	r0, r5, r0
    3e00:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3e04:	e1a02007 	mov	r2, r7
    3e08:	ebfff595 	bl	1464 <memcpy>
    3e0c:	e0888004 	add	r8, r8, r4
    3e10:	e0675005 	rsb	r5, r7, r5
    3e14:	e3a00000 	mov	r0, #0	; 0x0
    3e18:	e1550000 	cmp	r5, r0
    3e1c:	1affffda 	bne	3d8c <tt_crypto_aes+0x1e4>
    3e20:	ea000009 	b	3e4c <tt_crypto_aes+0x2a4>
    3e24:	e1a00006 	mov	r0, r6
    3e28:	eb002a3c 	bl	e720 <get_timer>
    3e2c:	e1a04000 	mov	r4, r0
    3e30:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3e34:	e1a01000 	mov	r1, r0
    3e38:	e59d200c 	ldr	r2, [sp, #12]
    3e3c:	eb000994 	bl	6494 <ss_aes_data>
    3e40:	e1a00004 	mov	r0, r4
    3e44:	eb002a35 	bl	e720 <get_timer>
    3e48:	e1a08000 	mov	r8, r0
    3e4c:	e59d3004 	ldr	r3, [sp, #4]
    3e50:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3e54:	e1a00543 	asr	r0, r3, #10
    3e58:	e1a0100a 	mov	r1, sl
    3e5c:	e0000094 	mul	r0, r4, r0
    3e60:	eb004449 	bl	14f8c <__aeabi_idiv>
    3e64:	e1a06000 	mov	r6, r0
    3e68:	e59d000c 	ldr	r0, [sp, #12]
    3e6c:	e1a01008 	mov	r1, r8
    3e70:	e1a00540 	asr	r0, r0, #10
    3e74:	e0000094 	mul	r0, r4, r0
    3e78:	eb004443 	bl	14f8c <__aeabi_idiv>
    3e7c:	e59f408c 	ldr	r4, [pc, #140]	; 3f10 <tt_crypto_aes+0x368>
    3e80:	e1a05000 	mov	r5, r0
    3e84:	e59f00ac 	ldr	r0, [pc, #172]	; 3f38 <tt_crypto_aes+0x390>
    3e88:	e1a0e00f 	mov	lr, pc
    3e8c:	e594f000 	ldr	pc, [r4]
    3e90:	e1a0000b 	mov	r0, fp
    3e94:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    3e98:	e59d200c 	ldr	r2, [sp, #12]
    3e9c:	e3a0300c 	mov	r3, #12	; 0xc
    3ea0:	eb000189 	bl	44cc <tt_cmp_buffer>
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
    3f10:	3c00a5cc 	.word	0x3c00a5cc
    3f14:	000176a1 	.word	0x000176a1
    3f18:	000176c4 	.word	0x000176c4
    3f1c:	00017106 	.word	0x00017106
    3f20:	00017f46 	.word	0x00017f46
    3f24:	3c00a004 	.word	0x3c00a004
    3f28:	00016ef4 	.word	0x00016ef4
    3f2c:	000171fb 	.word	0x000171fb
    3f30:	000176f0 	.word	0x000176f0
    3f34:	00018979 	.word	0x00018979
    3f38:	0001744c 	.word	0x0001744c
    3f3c:	00017711 	.word	0x00017711
    3f40:	0001774f 	.word	0x0001774f
    3f44:	3c00a050 	.word	0x3c00a050

00003f48 <do_tt_crypto>:
    3f48:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3f4c:	e3520001 	cmp	r2, #1	; 0x1
    3f50:	e24dd004 	sub	sp, sp, #4	; 0x4
    3f54:	d3a07000 	movle	r7, #0	; 0x0
    3f58:	da000009 	ble	3f84 <do_tt_crypto+0x3c>
    3f5c:	e5930004 	ldr	r0, [r3, #4]
    3f60:	e3a01000 	mov	r1, #0	; 0x0
    3f64:	e3a02010 	mov	r2, #16	; 0x10
    3f68:	eb003781 	bl	11d74 <simple_strtoul>
    3f6c:	e1a07000 	mov	r7, r0
    3f70:	e1a01007 	mov	r1, r7
    3f74:	e59f0114 	ldr	r0, [pc, #276]	; 4090 <do_tt_crypto+0x148>
    3f78:	e59f3114 	ldr	r3, [pc, #276]	; 4094 <do_tt_crypto+0x14c>
    3f7c:	e1a0e00f 	mov	lr, pc
    3f80:	e593f000 	ldr	pc, [r3]
    3f84:	e3a00000 	mov	r0, #0	; 0x0
    3f88:	eb0029e4 	bl	e720 <get_timer>
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
    3fb8:	eb000197 	bl	461c <tt_rnd_get_int>
    3fbc:	e3a01002 	mov	r1, #2	; 0x2
    3fc0:	e1a04000 	mov	r4, r0
    3fc4:	e3a00000 	mov	r0, #0	; 0x0
    3fc8:	eb000193 	bl	461c <tt_rnd_get_int>
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
    4014:	eb003b6d 	bl	12dd0 <tstc>
    4018:	e3500000 	cmp	r0, #0	; 0x0
    401c:	e2866001 	add	r6, r6, #1	; 0x1
    4020:	0affffe2 	beq	3fb0 <do_tt_crypto+0x68>
    4024:	eb003b6a 	bl	12dd4 <getc>
    4028:	e3500003 	cmp	r0, #3	; 0x3
    402c:	1affffdf 	bne	3fb0 <do_tt_crypto+0x68>
    4030:	e1a00008 	mov	r0, r8
    4034:	eb0029b9 	bl	e720 <get_timer>
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
    4064:	eb004346 	bl	14d84 <__aeabi_uidiv>
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
    4090:	00017780 	.word	0x00017780
    4094:	3c00a5cc 	.word	0x3c00a5cc
    4098:	3c00a058 	.word	0x3c00a058
    409c:	3c00a050 	.word	0x3c00a050
    40a0:	000177a6 	.word	0x000177a6
    40a4:	000177c5 	.word	0x000177c5

000040a8 <tt_ff_trolling>:
    40a8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    40ac:	e3a01001 	mov	r1, #1	; 0x1
    40b0:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    40b4:	e58d200c 	str	r2, [sp, #12]
    40b8:	e58d3008 	str	r3, [sp, #8]
    40bc:	e1a05000 	mov	r5, r0
    40c0:	eb0008c1 	bl	63cc <vs_assign_by_name>
    40c4:	e2506000 	subs	r6, r0, #0	; 0x0
    40c8:	0a000005 	beq	40e4 <tt_ff_trolling+0x3c>
    40cc:	e1a01005 	mov	r1, r5
    40d0:	e59f0330 	ldr	r0, [pc, #816]	; 4408 <tt_ff_trolling+0x360>
    40d4:	e59f3330 	ldr	r3, [pc, #816]	; 440c <tt_ff_trolling+0x364>
    40d8:	e1a0e00f 	mov	lr, pc
    40dc:	e593f000 	ldr	pc, [r3]
    40e0:	ea00000c 	b	4118 <tt_ff_trolling+0x70>
    40e4:	e3a0000e 	mov	r0, #14	; 0xe
    40e8:	eb000746 	bl	5e08 <vs_align>
    40ec:	e1a01000 	mov	r1, r0
    40f0:	e28d0018 	add	r0, sp, #24	; 0x18
    40f4:	e5201004 	str	r1, [r0, #-4]!
    40f8:	e1a02001 	mov	r2, r1
    40fc:	eb00016f 	bl	46c0 <tt_rnd_get_data>
    4100:	e250a000 	subs	sl, r0, #0	; 0x0
    4104:	e59f4300 	ldr	r4, [pc, #768]	; 440c <tt_ff_trolling+0x364>
    4108:	1a000004 	bne	4120 <tt_ff_trolling+0x78>
    410c:	e59f02fc 	ldr	r0, [pc, #764]	; 4410 <tt_ff_trolling+0x368>
    4110:	e1a0e00f 	mov	lr, pc
    4114:	e594f000 	ldr	pc, [r4]
    4118:	e3e00000 	mvn	r0, #0	; 0x0
    411c:	ea0000a9 	b	43c8 <tt_ff_trolling+0x320>
    4120:	e59d2014 	ldr	r2, [sp, #20]
    4124:	e1a0100a 	mov	r1, sl
    4128:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    412c:	ebfff4cc 	bl	1464 <memcpy>
    4130:	e3a01501 	mov	r1, #4194304	; 0x400000
    4134:	e59f02d8 	ldr	r0, [pc, #728]	; 4414 <tt_ff_trolling+0x36c>
    4138:	e1a0e00f 	mov	lr, pc
    413c:	e594f000 	ldr	pc, [r4]
    4140:	e1a00005 	mov	r0, r5
    4144:	eb000782 	bl	5f54 <vs_device_id>
    4148:	e1a0b004 	mov	fp, r4
    414c:	e3a05000 	mov	r5, #0	; 0x0
    4150:	e3a04000 	mov	r4, #0	; 0x0
    4154:	e3a08501 	mov	r8, #4194304	; 0x400000
    4158:	e3a09000 	mov	r9, #0	; 0x0
    415c:	e1a07000 	mov	r7, r0
    4160:	ea00003d 	b	425c <tt_ff_trolling+0x1b4>
    4164:	e1a00007 	mov	r0, r7
    4168:	eb00076d 	bl	5f24 <vs_device_erasable>
    416c:	e3500000 	cmp	r0, #0	; 0x0
    4170:	0a000003 	beq	4184 <tt_ff_trolling+0xdc>
    4174:	e1a00004 	mov	r0, r4
    4178:	e1a01005 	mov	r1, r5
    417c:	e3a02501 	mov	r2, #4194304	; 0x400000
    4180:	eb0006e4 	bl	5d18 <vs_erase>
    4184:	e1a00007 	mov	r0, r7
    4188:	e59f1288 	ldr	r1, [pc, #648]	; 4418 <tt_ff_trolling+0x370>
    418c:	eb000784 	bl	5fa4 <vs_is_device>
    4190:	e3500000 	cmp	r0, #0	; 0x0
    4194:	1a000009 	bne	41c0 <tt_ff_trolling+0x118>
    4198:	e1a00007 	mov	r0, r7
    419c:	e59f1278 	ldr	r1, [pc, #632]	; 441c <tt_ff_trolling+0x374>
    41a0:	eb00077f 	bl	5fa4 <vs_is_device>
    41a4:	e3500000 	cmp	r0, #0	; 0x0
    41a8:	1a000004 	bne	41c0 <tt_ff_trolling+0x118>
    41ac:	e1a00007 	mov	r0, r7
    41b0:	e59f1268 	ldr	r1, [pc, #616]	; 4420 <tt_ff_trolling+0x378>
    41b4:	eb00077a 	bl	5fa4 <vs_is_device>
    41b8:	e3500000 	cmp	r0, #0	; 0x0
    41bc:	0a00000a 	beq	41ec <tt_ff_trolling+0x144>
    41c0:	e1a00004 	mov	r0, r4
    41c4:	e1a01005 	mov	r1, r5
    41c8:	eb001eb2 	bl	bc98 <nnd_vs_isbad>
    41cc:	e3500000 	cmp	r0, #0	; 0x0
    41d0:	0a000005 	beq	41ec <tt_ff_trolling+0x144>
    41d4:	e59f0248 	ldr	r0, [pc, #584]	; 4424 <tt_ff_trolling+0x37c>
    41d8:	e1a02004 	mov	r2, r4
    41dc:	e1a03005 	mov	r3, r5
    41e0:	e1a0e00f 	mov	lr, pc
    41e4:	e59bf000 	ldr	pc, [fp]
    41e8:	ea000018 	b	4250 <tt_ff_trolling+0x1a8>
    41ec:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    41f0:	e5816000 	str	r6, [r1]
    41f4:	e1a00001 	mov	r0, r1
    41f8:	e59d1014 	ldr	r1, [sp, #20]
    41fc:	e1a02004 	mov	r2, r4
    4200:	e58d1000 	str	r1, [sp]
    4204:	e1a03005 	mov	r3, r5
    4208:	e3a01000 	mov	r1, #0	; 0x0
    420c:	e58d1004 	str	r1, [sp, #4]
    4210:	eb0006aa 	bl	5cc0 <vs_write>
    4214:	e3500000 	cmp	r0, #0	; 0x0
    4218:	aa000006 	bge	4238 <tt_ff_trolling+0x190>
    421c:	e1a02004 	mov	r2, r4
    4220:	e1a03005 	mov	r3, r5
    4224:	e59f01fc 	ldr	r0, [pc, #508]	; 4428 <tt_ff_trolling+0x380>
    4228:	e59f11dc 	ldr	r1, [pc, #476]	; 440c <tt_ff_trolling+0x364>
    422c:	e1a0e00f 	mov	lr, pc
    4230:	e591f000 	ldr	pc, [r1]
    4234:	ea000010 	b	427c <tt_ff_trolling+0x1d4>
    4238:	e316001f 	tst	r6, #31	; 0x1f
    423c:	1a000003 	bne	4250 <tt_ff_trolling+0x1a8>
    4240:	e59f01e4 	ldr	r0, [pc, #484]	; 442c <tt_ff_trolling+0x384>
    4244:	e1a01106 	lsl	r1, r6, #2
    4248:	e1a0e00f 	mov	lr, pc
    424c:	e59bf000 	ldr	pc, [fp]
    4250:	e0944008 	adds	r4, r4, r8
    4254:	e0a55009 	adc	r5, r5, r9
    4258:	e2866001 	add	r6, r6, #1	; 0x1
    425c:	e59d3008 	ldr	r3, [sp, #8]
    4260:	e1530005 	cmp	r3, r5
    4264:	8affffbe 	bhi	4164 <tt_ff_trolling+0xbc>
    4268:	1a000058 	bne	43d0 <tt_ff_trolling+0x328>
    426c:	e59d100c 	ldr	r1, [sp, #12]
    4270:	e1510004 	cmp	r1, r4
    4274:	8affffba 	bhi	4164 <tt_ff_trolling+0xbc>
    4278:	ea000054 	b	43d0 <tt_ff_trolling+0x328>
    427c:	e59f4188 	ldr	r4, [pc, #392]	; 440c <tt_ff_trolling+0x364>
    4280:	e59f01a8 	ldr	r0, [pc, #424]	; 4430 <tt_ff_trolling+0x388>
    4284:	e3a01501 	mov	r1, #4194304	; 0x400000
    4288:	e1a0e00f 	mov	lr, pc
    428c:	e594f000 	ldr	pc, [r4]
    4290:	e1a0b004 	mov	fp, r4
    4294:	e3a06000 	mov	r6, #0	; 0x0
    4298:	e28aa004 	add	sl, sl, #4	; 0x4
    429c:	e3a04000 	mov	r4, #0	; 0x0
    42a0:	e3a05000 	mov	r5, #0	; 0x0
    42a4:	e3a08501 	mov	r8, #4194304	; 0x400000
    42a8:	e3a09000 	mov	r9, #0	; 0x0
    42ac:	ea00003d 	b	43a8 <tt_ff_trolling+0x300>
    42b0:	e1a00007 	mov	r0, r7
    42b4:	e59f115c 	ldr	r1, [pc, #348]	; 4418 <tt_ff_trolling+0x370>
    42b8:	eb000739 	bl	5fa4 <vs_is_device>
    42bc:	e3500000 	cmp	r0, #0	; 0x0
    42c0:	1a000009 	bne	42ec <tt_ff_trolling+0x244>
    42c4:	e1a00007 	mov	r0, r7
    42c8:	e59f114c 	ldr	r1, [pc, #332]	; 441c <tt_ff_trolling+0x374>
    42cc:	eb000734 	bl	5fa4 <vs_is_device>
    42d0:	e3500000 	cmp	r0, #0	; 0x0
    42d4:	1a000004 	bne	42ec <tt_ff_trolling+0x244>
    42d8:	e1a00007 	mov	r0, r7
    42dc:	e59f113c 	ldr	r1, [pc, #316]	; 4420 <tt_ff_trolling+0x378>
    42e0:	eb00072f 	bl	5fa4 <vs_is_device>
    42e4:	e3500000 	cmp	r0, #0	; 0x0
    42e8:	0a00000a 	beq	4318 <tt_ff_trolling+0x270>
    42ec:	e1a00004 	mov	r0, r4
    42f0:	e1a01005 	mov	r1, r5
    42f4:	eb001e67 	bl	bc98 <nnd_vs_isbad>
    42f8:	e3500000 	cmp	r0, #0	; 0x0
    42fc:	0a000005 	beq	4318 <tt_ff_trolling+0x270>
    4300:	e59f011c 	ldr	r0, [pc, #284]	; 4424 <tt_ff_trolling+0x37c>
    4304:	e1a02004 	mov	r2, r4
    4308:	e1a03005 	mov	r3, r5
    430c:	e1a0e00f 	mov	lr, pc
    4310:	e59bf000 	ldr	pc, [fp]
    4314:	ea000020 	b	439c <tt_ff_trolling+0x2f4>
    4318:	e59d1014 	ldr	r1, [sp, #20]
    431c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    4320:	e58d1000 	str	r1, [sp]
    4324:	e1a02004 	mov	r2, r4
    4328:	e3a01000 	mov	r1, #0	; 0x0
    432c:	e1a03005 	mov	r3, r5
    4330:	e58d1004 	str	r1, [sp, #4]
    4334:	eb000733 	bl	6008 <vs_read>
    4338:	e316001f 	tst	r6, #31	; 0x1f
    433c:	1a000003 	bne	4350 <tt_ff_trolling+0x2a8>
    4340:	e59f00e4 	ldr	r0, [pc, #228]	; 442c <tt_ff_trolling+0x384>
    4344:	e1a01106 	lsl	r1, r6, #2
    4348:	e1a0e00f 	mov	lr, pc
    434c:	e59bf000 	ldr	pc, [fp]
    4350:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    4354:	e5933000 	ldr	r3, [r3]
    4358:	e1530006 	cmp	r3, r6
    435c:	1a000007 	bne	4380 <tt_ff_trolling+0x2d8>
    4360:	e59d2014 	ldr	r2, [sp, #20]
    4364:	e1a0000a 	mov	r0, sl
    4368:	e2422004 	sub	r2, r2, #4	; 0x4
    436c:	e59f10c0 	ldr	r1, [pc, #192]	; 4434 <tt_ff_trolling+0x38c>
    4370:	e3a0300a 	mov	r3, #10	; 0xa
    4374:	eb000054 	bl	44cc <tt_cmp_buffer>
    4378:	e3500000 	cmp	r0, #0	; 0x0
    437c:	0a000006 	beq	439c <tt_ff_trolling+0x2f4>
    4380:	e1a02004 	mov	r2, r4
    4384:	e1a03005 	mov	r3, r5
    4388:	e59f00a8 	ldr	r0, [pc, #168]	; 4438 <tt_ff_trolling+0x390>
    438c:	e59f1078 	ldr	r1, [pc, #120]	; 440c <tt_ff_trolling+0x364>
    4390:	e1a0e00f 	mov	lr, pc
    4394:	e591f000 	ldr	pc, [r1]
    4398:	ea000018 	b	4400 <tt_ff_trolling+0x358>
    439c:	e0944008 	adds	r4, r4, r8
    43a0:	e0a55009 	adc	r5, r5, r9
    43a4:	e2866001 	add	r6, r6, #1	; 0x1
    43a8:	e59d3008 	ldr	r3, [sp, #8]
    43ac:	e1530005 	cmp	r3, r5
    43b0:	8affffbe 	bhi	42b0 <tt_ff_trolling+0x208>
    43b4:	1a00000a 	bne	43e4 <tt_ff_trolling+0x33c>
    43b8:	e59d100c 	ldr	r1, [sp, #12]
    43bc:	e1510004 	cmp	r1, r4
    43c0:	8affffba 	bhi	42b0 <tt_ff_trolling+0x208>
    43c4:	ea000006 	b	43e4 <tt_ff_trolling+0x33c>
    43c8:	e28dd01c 	add	sp, sp, #28	; 0x1c
    43cc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    43d0:	e59f0064 	ldr	r0, [pc, #100]	; 443c <tt_ff_trolling+0x394>
    43d4:	e59f3030 	ldr	r3, [pc, #48]	; 440c <tt_ff_trolling+0x364>
    43d8:	e1a0e00f 	mov	lr, pc
    43dc:	e593f000 	ldr	pc, [r3]
    43e0:	eaffffa5 	b	427c <tt_ff_trolling+0x1d4>
    43e4:	e59f0050 	ldr	r0, [pc, #80]	; 443c <tt_ff_trolling+0x394>
    43e8:	e59f401c 	ldr	r4, [pc, #28]	; 440c <tt_ff_trolling+0x364>
    43ec:	e1a0e00f 	mov	lr, pc
    43f0:	e594f000 	ldr	pc, [r4]
    43f4:	e59f0044 	ldr	r0, [pc, #68]	; 4440 <tt_ff_trolling+0x398>
    43f8:	e1a0e00f 	mov	lr, pc
    43fc:	e594f000 	ldr	pc, [r4]
    4400:	e3a00000 	mov	r0, #0	; 0x0
    4404:	eaffffef 	b	43c8 <tt_ff_trolling+0x320>
    4408:	00017849 	.word	0x00017849
    440c:	3c00a5cc 	.word	0x3c00a5cc
    4410:	00017866 	.word	0x00017866
    4414:	00017885 	.word	0x00017885
    4418:	000166d1 	.word	0x000166d1
    441c:	000178a3 	.word	0x000178a3
    4420:	000178a7 	.word	0x000178a7
    4424:	000178ab 	.word	0x000178ab
    4428:	000178ca 	.word	0x000178ca
    442c:	000178e2 	.word	0x000178e2
    4430:	000178e9 	.word	0x000178e9
    4434:	8e000004 	.word	0x8e000004
    4438:	00017909 	.word	0x00017909
    443c:	00017919 	.word	0x00017919
    4440:	00017920 	.word	0x00017920

00004444 <do_tt_ff>:
    4444:	e92d4030 	push	{r4, r5, lr}
    4448:	e3520001 	cmp	r2, #1	; 0x1
    444c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4450:	e1a05003 	mov	r5, r3
    4454:	ca000005 	bgt	4470 <do_tt_ff+0x2c>
    4458:	e59f0060 	ldr	r0, [pc, #96]	; 44c0 <do_tt_ff+0x7c>
    445c:	e59f3060 	ldr	r3, [pc, #96]	; 44c4 <do_tt_ff+0x80>
    4460:	e1a0e00f 	mov	lr, pc
    4464:	e593f000 	ldr	pc, [r3]
    4468:	e3e00000 	mvn	r0, #0	; 0x0
    446c:	ea000011 	b	44b8 <do_tt_ff+0x74>
    4470:	e3520002 	cmp	r2, #2	; 0x2
    4474:	03a04000 	moveq	r4, #0	; 0x0
    4478:	0a000009 	beq	44a4 <do_tt_ff+0x60>
    447c:	e3a01000 	mov	r1, #0	; 0x0
    4480:	e5930008 	ldr	r0, [r3, #8]
    4484:	e3a0200a 	mov	r2, #10	; 0xa
    4488:	eb003639 	bl	11d74 <simple_strtoul>
    448c:	e1a04000 	mov	r4, r0
    4490:	e1a01004 	mov	r1, r4
    4494:	e59f002c 	ldr	r0, [pc, #44]	; 44c8 <do_tt_ff+0x84>
    4498:	e59f3024 	ldr	r3, [pc, #36]	; 44c4 <do_tt_ff+0x80>
    449c:	e1a0e00f 	mov	lr, pc
    44a0:	e593f000 	ldr	pc, [r3]
    44a4:	e5950004 	ldr	r0, [r5, #4]
    44a8:	e1a03624 	lsr	r3, r4, #12
    44ac:	e1a02a04 	lsl	r2, r4, #20
    44b0:	ebfffefc 	bl	40a8 <tt_ff_trolling>
    44b4:	e3a00000 	mov	r0, #0	; 0x0
    44b8:	e28dd004 	add	sp, sp, #4	; 0x4
    44bc:	e8bd8030 	pop	{r4, r5, pc}
    44c0:	00017938 	.word	0x00017938
    44c4:	3c00a5cc 	.word	0x3c00a5cc
    44c8:	0001797e 	.word	0x0001797e

000044cc <tt_cmp_buffer>:
    44cc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    44d0:	e3a04000 	mov	r4, #0	; 0x0
    44d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    44d8:	e1a0a002 	mov	sl, r2
    44dc:	e1a07000 	mov	r7, r0
    44e0:	e1a06001 	mov	r6, r1
    44e4:	e1a08003 	mov	r8, r3
    44e8:	e1a09142 	asr	r9, r2, #2
    44ec:	e1a05004 	mov	r5, r4
    44f0:	e59fb0e4 	ldr	fp, [pc, #228]	; 45dc <tt_cmp_buffer+0x110>
    44f4:	ea00001f 	b	4578 <tt_cmp_buffer+0xac>
    44f8:	e5970000 	ldr	r0, [r7]
    44fc:	e5961000 	ldr	r1, [r6]
    4500:	e1500001 	cmp	r0, r1
    4504:	0a000018 	beq	456c <tt_cmp_buffer+0xa0>
    4508:	e5d72000 	ldrb	r2, [r7]
    450c:	e5d63000 	ldrb	r3, [r6]
    4510:	e1520003 	cmp	r2, r3
    4514:	e5d72001 	ldrb	r2, [r7, #1]
    4518:	e5d63001 	ldrb	r3, [r6, #1]
    451c:	12855001 	addne	r5, r5, #1	; 0x1
    4520:	e1520003 	cmp	r2, r3
    4524:	e5d72002 	ldrb	r2, [r7, #2]
    4528:	e5d63002 	ldrb	r3, [r6, #2]
    452c:	12855001 	addne	r5, r5, #1	; 0x1
    4530:	e1520003 	cmp	r2, r3
    4534:	e5d72003 	ldrb	r2, [r7, #3]
    4538:	e5d63003 	ldrb	r3, [r6, #3]
    453c:	12855001 	addne	r5, r5, #1	; 0x1
    4540:	e1520003 	cmp	r2, r3
    4544:	12855001 	addne	r5, r5, #1	; 0x1
    4548:	e3580000 	cmp	r8, #0	; 0x0
    454c:	0a000006 	beq	456c <tt_cmp_buffer+0xa0>
    4550:	e1a02000 	mov	r2, r0
    4554:	e1a03001 	mov	r3, r1
    4558:	e59f0080 	ldr	r0, [pc, #128]	; 45e0 <tt_cmp_buffer+0x114>
    455c:	e1a01104 	lsl	r1, r4, #2
    4560:	e1a0e00f 	mov	lr, pc
    4564:	e59bf000 	ldr	pc, [fp]
    4568:	e2488001 	sub	r8, r8, #1	; 0x1
    456c:	e2844001 	add	r4, r4, #1	; 0x1
    4570:	e2877004 	add	r7, r7, #4	; 0x4
    4574:	e2866004 	add	r6, r6, #4	; 0x4
    4578:	e1540009 	cmp	r4, r9
    457c:	baffffdd 	blt	44f8 <tt_cmp_buffer+0x2c>
    4580:	e3ca9003 	bic	r9, sl, #3	; 0x3
    4584:	e3a04000 	mov	r4, #0	; 0x0
    4588:	e20aa003 	and	sl, sl, #3	; 0x3
    458c:	e59fb048 	ldr	fp, [pc, #72]	; 45dc <tt_cmp_buffer+0x110>
    4590:	ea00000c 	b	45c8 <tt_cmp_buffer+0xfc>
    4594:	e7d72004 	ldrb	r2, [r7, r4]
    4598:	e7d63004 	ldrb	r3, [r6, r4]
    459c:	e1520003 	cmp	r2, r3
    45a0:	0a000007 	beq	45c4 <tt_cmp_buffer+0xf8>
    45a4:	e3580000 	cmp	r8, #0	; 0x0
    45a8:	e2855001 	add	r5, r5, #1	; 0x1
    45ac:	0a000004 	beq	45c4 <tt_cmp_buffer+0xf8>
    45b0:	e0841009 	add	r1, r4, r9
    45b4:	e59f0028 	ldr	r0, [pc, #40]	; 45e4 <tt_cmp_buffer+0x118>
    45b8:	e1a0e00f 	mov	lr, pc
    45bc:	e59bf000 	ldr	pc, [fp]
    45c0:	e2488001 	sub	r8, r8, #1	; 0x1
    45c4:	e2844001 	add	r4, r4, #1	; 0x1
    45c8:	e154000a 	cmp	r4, sl
    45cc:	bafffff0 	blt	4594 <tt_cmp_buffer+0xc8>
    45d0:	e1a00005 	mov	r0, r5
    45d4:	e28dd004 	add	sp, sp, #4	; 0x4
    45d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    45dc:	3c00a5cc 	.word	0x3c00a5cc
    45e0:	000179a5 	.word	0x000179a5
    45e4:	000179d0 	.word	0x000179d0

000045e8 <tt_rnd_seed>:
    45e8:	e92d4030 	push	{r4, r5, lr}
    45ec:	e59f5024 	ldr	r5, [pc, #36]	; 4618 <tt_rnd_seed+0x30>
    45f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    45f4:	e3a00000 	mov	r0, #0	; 0x0
    45f8:	e5954000 	ldr	r4, [r5]
    45fc:	eb002847 	bl	e720 <get_timer>
    4600:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    4604:	e6e40070 	uxtab	r0, r4, r0
    4608:	e0003003 	and	r3, r0, r3
    460c:	e5853000 	str	r3, [r5]
    4610:	e28dd004 	add	sp, sp, #4	; 0x4
    4614:	e8bd8030 	pop	{r4, r5, pc}
    4618:	3c00a060 	.word	0x3c00a060

0000461c <tt_rnd_get_int>:
    461c:	e59fc094 	ldr	ip, [pc, #148]	; 46b8 <tt_rnd_get_int+0x9c>
    4620:	e3a02102 	mov	r2, #-2147483648	; 0x80000000
    4624:	e59c3000 	ldr	r3, [ip]
    4628:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    462c:	e19330b2 	ldrh	r3, [r3, r2]
    4630:	e24dd004 	sub	sp, sp, #4	; 0x4
    4634:	e3c33003 	bic	r3, r3, #3	; 0x3
    4638:	e6ff3073 	uxth	r3, r3
    463c:	e19340b2 	ldrh	r4, [r3, r2]
    4640:	e58c3000 	str	r3, [ip]
    4644:	e1a07000 	mov	r7, r0
    4648:	e1a06001 	mov	r6, r1
    464c:	ebffffe5 	bl	45e8 <tt_rnd_seed>
    4650:	e1a00004 	mov	r0, r4
    4654:	eb00439d 	bl	154d0 <__aeabi_ui2d>
    4658:	e3a02000 	mov	r2, #0	; 0x0
    465c:	e59f3058 	ldr	r3, [pc, #88]	; 46bc <tt_rnd_get_int+0xa0>
    4660:	eb004475 	bl	1583c <__aeabi_ddiv>
    4664:	e1a04000 	mov	r4, r0
    4668:	e0670006 	rsb	r0, r7, r6
    466c:	e1a05001 	mov	r5, r1
    4670:	eb00439f 	bl	154f4 <__aeabi_i2d>
    4674:	e1a02000 	mov	r2, r0
    4678:	e1a03001 	mov	r3, r1
    467c:	e1a00004 	mov	r0, r4
    4680:	e1a01005 	mov	r1, r5
    4684:	eb0043d1 	bl	155d0 <__aeabi_dmul>
    4688:	e1a04000 	mov	r4, r0
    468c:	e1a00007 	mov	r0, r7
    4690:	e1a05001 	mov	r5, r1
    4694:	eb004396 	bl	154f4 <__aeabi_i2d>
    4698:	e1a02000 	mov	r2, r0
    469c:	e1a03001 	mov	r3, r1
    46a0:	e1a00004 	mov	r0, r4
    46a4:	e1a01005 	mov	r1, r5
    46a8:	eb0042dd 	bl	15224 <__adddf3>
    46ac:	eb0044e3 	bl	15a40 <__aeabi_d2iz>
    46b0:	e28dd004 	add	sp, sp, #4	; 0x4
    46b4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    46b8:	3c00a060 	.word	0x3c00a060
    46bc:	40efffe0 	.word	0x40efffe0

000046c0 <tt_rnd_get_data>:
    46c0:	e92d4010 	push	{r4, lr}
    46c4:	e1a04000 	mov	r4, r0
    46c8:	e1a00001 	mov	r0, r1
    46cc:	e1a01002 	mov	r1, r2
    46d0:	ebffffd1 	bl	461c <tt_rnd_get_int>
    46d4:	e59f3010 	ldr	r3, [pc, #16]	; 46ec <tt_rnd_get_data+0x2c>
    46d8:	e5840000 	str	r0, [r4]
    46dc:	e5930000 	ldr	r0, [r3]
    46e0:	e3c00003 	bic	r0, r0, #3	; 0x3
    46e4:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    46e8:	e8bd8010 	pop	{r4, pc}
    46ec:	3c00a060 	.word	0x3c00a060

000046f0 <do_tt_nbi>:
    46f0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    46f4:	e3a00001 	mov	r0, #1	; 0x1
    46f8:	e24dd054 	sub	sp, sp, #84	; 0x54
    46fc:	ebfff028 	bl	7a4 <get_xom>
    4700:	e59f30d0 	ldr	r3, [pc, #208]	; 47d8 <do_tt_nbi+0xe8>
    4704:	e28d4010 	add	r4, sp, #16	; 0x10
    4708:	e1a0c003 	mov	ip, r3
    470c:	e1a06000 	mov	r6, r0
    4710:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    4714:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4718:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    471c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4720:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    4724:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4728:	e89c000f 	ldm	ip, {r0, r1, r2, r3}
    472c:	e59fa0a8 	ldr	sl, [pc, #168]	; 47dc <do_tt_nbi+0xec>
    4730:	e59f80a8 	ldr	r8, [pc, #168]	; 47e0 <do_tt_nbi+0xf0>
    4734:	e59f70a8 	ldr	r7, [pc, #168]	; 47e4 <do_tt_nbi+0xf4>
    4738:	e884000f 	stm	r4, {r0, r1, r2, r3}
    473c:	e3a0400a 	mov	r4, #10	; 0xa
    4740:	e1a00004 	mov	r0, r4
    4744:	ebfff01a 	bl	7b4 <set_xom>
    4748:	eb001eaa 	bl	c1f8 <nand_rescan_bootcfg>
    474c:	e3a00000 	mov	r0, #0	; 0x0
    4750:	eb0019ec 	bl	af08 <nand_get_config>
    4754:	e5d03005 	ldrb	r3, [r0, #5]
    4758:	e5d02006 	ldrb	r2, [r0, #6]
    475c:	e3530000 	cmp	r3, #0	; 0x0
    4760:	15d03003 	ldrbne	r3, [r0, #3]
    4764:	128d1050 	addne	r1, sp, #80	; 0x50
    4768:	10813103 	addne	r3, r1, r3, lsl #2
    476c:	e5d01000 	ldrb	r1, [r0]
    4770:	15133040 	ldrne	r3, [r3, #-64]
    4774:	e3510000 	cmp	r1, #0	; 0x0
    4778:	e590e014 	ldr	lr, [r0, #20]
    477c:	e59a5000 	ldr	r5, [sl]
    4780:	e5d0c002 	ldrb	ip, [r0, #2]
    4784:	059f105c 	ldreq	r1, [pc, #92]	; 47e8 <do_tt_nbi+0xf8>
    4788:	0a000002 	beq	4798 <do_tt_nbi+0xa8>
    478c:	e3510001 	cmp	r1, #1	; 0x1
    4790:	01a01008 	moveq	r1, r8
    4794:	11a01007 	movne	r1, r7
    4798:	e58d1008 	str	r1, [sp, #8]
    479c:	e88d1008 	stm	sp, {r3, ip}
    47a0:	e5d03001 	ldrb	r3, [r0, #1]
    47a4:	e1a01004 	mov	r1, r4
    47a8:	e58d300c 	str	r3, [sp, #12]
    47ac:	e2844001 	add	r4, r4, #1	; 0x1
    47b0:	e1a0300e 	mov	r3, lr
    47b4:	e59f0030 	ldr	r0, [pc, #48]	; 47ec <do_tt_nbi+0xfc>
    47b8:	e12fff35 	blx	r5
    47bc:	e354001c 	cmp	r4, #28	; 0x1c
    47c0:	1affffde 	bne	4740 <do_tt_nbi+0x50>
    47c4:	e1a00006 	mov	r0, r6
    47c8:	ebffeff9 	bl	7b4 <set_xom>
    47cc:	e3a00000 	mov	r0, #0	; 0x0
    47d0:	e28dd054 	add	sp, sp, #84	; 0x54
    47d4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    47d8:	00015b6c 	.word	0x00015b6c
    47dc:	3c00a5cc 	.word	0x3c00a5cc
    47e0:	00017a09 	.word	0x00017a09
    47e4:	00017a02 	.word	0x00017a02
    47e8:	000179fb 	.word	0x000179fb
    47ec:	00017a0e 	.word	0x00017a0e

000047f0 <tt_simple_test>:
    47f0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    47f4:	e1a0a001 	mov	sl, r1
    47f8:	e24dd044 	sub	sp, sp, #68	; 0x44
    47fc:	e3a01001 	mov	r1, #1	; 0x1
    4800:	e1a05000 	mov	r5, r0
    4804:	eb0006f0 	bl	63cc <vs_assign_by_name>
    4808:	e3500000 	cmp	r0, #0	; 0x0
    480c:	13e00000 	mvnne	r0, #0	; 0x0
    4810:	1a000048 	bne	4938 <tt_simple_test+0x148>
    4814:	e1a00005 	mov	r0, r5
    4818:	e59f1120 	ldr	r1, [pc, #288]	; 4940 <tt_simple_test+0x150>
    481c:	e3a02003 	mov	r2, #3	; 0x3
    4820:	ebfff2d6 	bl	1380 <strncmp>
    4824:	e3500000 	cmp	r0, #0	; 0x0
    4828:	1a000001 	bne	4834 <tt_simple_test+0x44>
    482c:	eb0019e3 	bl	afc0 <fnd_align>
    4830:	ea000001 	b	483c <tt_simple_test+0x4c>
    4834:	e3a0000e 	mov	r0, #14	; 0xe
    4838:	eb000572 	bl	5e08 <vs_align>
    483c:	e1a06400 	lsl	r6, r0, #8
    4840:	e1a00005 	mov	r0, r5
    4844:	eb0005c2 	bl	5f54 <vs_device_id>
    4848:	e59f80f4 	ldr	r8, [pc, #244]	; 4944 <tt_simple_test+0x154>
    484c:	e1a07000 	mov	r7, r0
    4850:	e5984000 	ldr	r4, [r8]
    4854:	eb00056b 	bl	5e08 <vs_align>
    4858:	e1a01005 	mov	r1, r5
    485c:	e1a02000 	mov	r2, r0
    4860:	e59f00e0 	ldr	r0, [pc, #224]	; 4948 <tt_simple_test+0x158>
    4864:	e12fff34 	blx	r4
    4868:	e35a0000 	cmp	sl, #0	; 0x0
    486c:	1a00001a 	bne	48dc <tt_simple_test+0xec>
    4870:	e1a00007 	mov	r0, r7
    4874:	eb0005aa 	bl	5f24 <vs_device_erasable>
    4878:	e3500000 	cmp	r0, #0	; 0x0
    487c:	0a000009 	beq	48a8 <tt_simple_test+0xb8>
    4880:	e59f00c4 	ldr	r0, [pc, #196]	; 494c <tt_simple_test+0x15c>
    4884:	e1a0e00f 	mov	lr, pc
    4888:	e598f000 	ldr	pc, [r8]
    488c:	e59f10bc 	ldr	r1, [pc, #188]	; 4950 <tt_simple_test+0x160>
    4890:	e1a0000d 	mov	r0, sp
    4894:	e1a02006 	mov	r2, r6
    4898:	eb0038c1 	bl	12ba4 <sprintf>
    489c:	e1a0000d 	mov	r0, sp
    48a0:	e1a0100a 	mov	r1, sl
    48a4:	eb003986 	bl	12ec4 <run_command>
    48a8:	e59f3094 	ldr	r3, [pc, #148]	; 4944 <tt_simple_test+0x154>
    48ac:	e59f00a0 	ldr	r0, [pc, #160]	; 4954 <tt_simple_test+0x164>
    48b0:	e1a0e00f 	mov	lr, pc
    48b4:	e593f000 	ldr	pc, [r3]
    48b8:	e1a02006 	mov	r2, r6
    48bc:	e59f1094 	ldr	r1, [pc, #148]	; 4958 <tt_simple_test+0x168>
    48c0:	e1a0000d 	mov	r0, sp
    48c4:	eb0038b6 	bl	12ba4 <sprintf>
    48c8:	e1a0000d 	mov	r0, sp
    48cc:	e3a01000 	mov	r1, #0	; 0x0
    48d0:	e1a0400d 	mov	r4, sp
    48d4:	eb00397a 	bl	12ec4 <run_command>
    48d8:	ea000015 	b	4934 <tt_simple_test+0x144>
    48dc:	e59f0078 	ldr	r0, [pc, #120]	; 495c <tt_simple_test+0x16c>
    48e0:	e1a0e00f 	mov	lr, pc
    48e4:	e598f000 	ldr	pc, [r8]
    48e8:	e1a02006 	mov	r2, r6
    48ec:	e59f106c 	ldr	r1, [pc, #108]	; 4960 <tt_simple_test+0x170>
    48f0:	e1a0000d 	mov	r0, sp
    48f4:	eb0038aa 	bl	12ba4 <sprintf>
    48f8:	e3a01000 	mov	r1, #0	; 0x0
    48fc:	e1a0000d 	mov	r0, sp
    4900:	eb00396f 	bl	12ec4 <run_command>
    4904:	e59f0058 	ldr	r0, [pc, #88]	; 4964 <tt_simple_test+0x174>
    4908:	e1a0e00f 	mov	lr, pc
    490c:	e598f000 	ldr	pc, [r8]
    4910:	e1a02006 	mov	r2, r6
    4914:	e59f104c 	ldr	r1, [pc, #76]	; 4968 <tt_simple_test+0x178>
    4918:	e3a03003 	mov	r3, #3	; 0x3
    491c:	e59f0048 	ldr	r0, [pc, #72]	; 496c <tt_simple_test+0x17c>
    4920:	ebfffee9 	bl	44cc <tt_cmp_buffer>
    4924:	e59f0044 	ldr	r0, [pc, #68]	; 4970 <tt_simple_test+0x180>
    4928:	e1a0400d 	mov	r4, sp
    492c:	e1a0e00f 	mov	lr, pc
    4930:	e598f000 	ldr	pc, [r8]
    4934:	e3a00000 	mov	r0, #0	; 0x0
    4938:	e28dd044 	add	sp, sp, #68	; 0x44
    493c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4940:	000178a3 	.word	0x000178a3
    4944:	3c00a5cc 	.word	0x3c00a5cc
    4948:	00017d0a 	.word	0x00017d0a
    494c:	00017a2a 	.word	0x00017a2a
    4950:	00017a37 	.word	0x00017a37
    4954:	00017a4c 	.word	0x00017a4c
    4958:	00017a59 	.word	0x00017a59
    495c:	00017a77 	.word	0x00017a77
    4960:	00017a84 	.word	0x00017a84
    4964:	00017aa1 	.word	0x00017aa1
    4968:	50008000 	.word	0x50008000
    496c:	40008000 	.word	0x40008000
    4970:	000171fb 	.word	0x000171fb

00004974 <do_tt_randomizer>:
    4974:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4978:	e24dd064 	sub	sp, sp, #100	; 0x64
    497c:	e1a06002 	mov	r6, r2
    4980:	e28d0010 	add	r0, sp, #16	; 0x10
    4984:	e59f12e0 	ldr	r1, [pc, #736]	; 4c6c <do_tt_randomizer+0x2f8>
    4988:	e3a02050 	mov	r2, #80	; 0x50
    498c:	e1a05003 	mov	r5, r3
    4990:	ebfff2b3 	bl	1464 <memcpy>
    4994:	e3560001 	cmp	r6, #1	; 0x1
    4998:	d59f02d0 	ldrle	r0, [pc, #720]	; 4c70 <do_tt_randomizer+0x2fc>
    499c:	da000009 	ble	49c8 <do_tt_randomizer+0x54>
    49a0:	e5950004 	ldr	r0, [r5, #4]
    49a4:	e59f12c8 	ldr	r1, [pc, #712]	; 4c74 <do_tt_randomizer+0x300>
    49a8:	e3a02003 	mov	r2, #3	; 0x3
    49ac:	ebfff273 	bl	1380 <strncmp>
    49b0:	e2504000 	subs	r4, r0, #0	; 0x0
    49b4:	e2858004 	add	r8, r5, #4	; 0x4
    49b8:	1a000011 	bne	4a04 <do_tt_randomizer+0x90>
    49bc:	e3560002 	cmp	r6, #2	; 0x2
    49c0:	1a000005 	bne	49dc <do_tt_randomizer+0x68>
    49c4:	e59f02ac 	ldr	r0, [pc, #684]	; 4c78 <do_tt_randomizer+0x304>
    49c8:	e59f32ac 	ldr	r3, [pc, #684]	; 4c7c <do_tt_randomizer+0x308>
    49cc:	e1a0e00f 	mov	lr, pc
    49d0:	e593f000 	ldr	pc, [r3]
    49d4:	e3e00000 	mvn	r0, #0	; 0x0
    49d8:	ea0000a1 	b	4c64 <do_tt_randomizer+0x2f0>
    49dc:	e3a02010 	mov	r2, #16	; 0x10
    49e0:	e1a01004 	mov	r1, r4
    49e4:	e5950008 	ldr	r0, [r5, #8]
    49e8:	eb0034e1 	bl	11d74 <simple_strtoul>
    49ec:	ebffef70 	bl	7b4 <set_xom>
    49f0:	e1a00004 	mov	r0, r4
    49f4:	e1a01004 	mov	r1, r4
    49f8:	eb001eb5 	bl	c4d4 <nand_rescan_config>
    49fc:	e1a00004 	mov	r0, r4
    4a00:	ea000000 	b	4a08 <do_tt_randomizer+0x94>
    4a04:	e3a00001 	mov	r0, #1	; 0x1
    4a08:	eb00193e 	bl	af08 <nand_get_config>
    4a0c:	e3500000 	cmp	r0, #0	; 0x0
    4a10:	e1a05000 	mov	r5, r0
    4a14:	059f0264 	ldreq	r0, [pc, #612]	; 4c80 <do_tt_randomizer+0x30c>
    4a18:	0affffea 	beq	49c8 <do_tt_randomizer+0x54>
    4a1c:	e3a00001 	mov	r0, #1	; 0x1
    4a20:	e3a01000 	mov	r1, #0	; 0x0
    4a24:	e59fb250 	ldr	fp, [pc, #592]	; 4c7c <do_tt_randomizer+0x308>
    4a28:	e58d0008 	str	r0, [sp, #8]
    4a2c:	e58d100c 	str	r1, [sp, #12]
    4a30:	e59d000c 	ldr	r0, [sp, #12]
    4a34:	e59f2248 	ldr	r2, [pc, #584]	; 4c84 <do_tt_randomizer+0x310>
    4a38:	e3a09000 	mov	r9, #0	; 0x0
    4a3c:	e7923000 	ldr	r3, [r2, r0]
    4a40:	e59f0240 	ldr	r0, [pc, #576]	; 4c88 <do_tt_randomizer+0x314>
    4a44:	e1a01003 	mov	r1, r3
    4a48:	e5853048 	str	r3, [r5, #72]
    4a4c:	e59f2228 	ldr	r2, [pc, #552]	; 4c7c <do_tt_randomizer+0x308>
    4a50:	e1a0e00f 	mov	lr, pc
    4a54:	e592f000 	ldr	pc, [r2]
    4a58:	e59f1224 	ldr	r1, [pc, #548]	; 4c84 <do_tt_randomizer+0x310>
    4a5c:	e59d200c 	ldr	r2, [sp, #12]
    4a60:	e59d0008 	ldr	r0, [sp, #8]
    4a64:	e0813002 	add	r3, r1, r2
    4a68:	e3a0100a 	mov	r1, #10	; 0xa
    4a6c:	e58d3000 	str	r3, [sp]
    4a70:	eb0041cd 	bl	151ac <__aeabi_idivmod>
    4a74:	e28da010 	add	sl, sp, #16	; 0x10
    4a78:	e58d1004 	str	r1, [sp, #4]
    4a7c:	ea00006a 	b	4c2c <do_tt_randomizer+0x2b8>
    4a80:	e4972004 	ldr	r2, [r7], #4
    4a84:	e1a01004 	mov	r1, r4
    4a88:	e5862038 	str	r2, [r6, #56]
    4a8c:	e2844001 	add	r4, r4, #1	; 0x1
    4a90:	e59f01f4 	ldr	r0, [pc, #500]	; 4c8c <do_tt_randomizer+0x318>
    4a94:	e59f31e0 	ldr	r3, [pc, #480]	; 4c7c <do_tt_randomizer+0x308>
    4a98:	e1a0e00f 	mov	lr, pc
    4a9c:	e593f000 	ldr	pc, [r3]
    4aa0:	e3540004 	cmp	r4, #4	; 0x4
    4aa4:	e2866004 	add	r6, r6, #4	; 0x4
    4aa8:	1afffff4 	bne	4a80 <do_tt_randomizer+0x10c>
    4aac:	e3a00001 	mov	r0, #1	; 0x1
    4ab0:	e5c50001 	strb	r0, [r5, #1]
    4ab4:	e3a00000 	mov	r0, #0	; 0x0
    4ab8:	eb002718 	bl	e720 <get_timer>
    4abc:	e1a04000 	mov	r4, r0
    4ac0:	e1a00005 	mov	r0, r5
    4ac4:	eb001e3f 	bl	c3c8 <nand_init_randomizer>
    4ac8:	e1a00004 	mov	r0, r4
    4acc:	eb002713 	bl	e720 <get_timer>
    4ad0:	e1a01000 	mov	r1, r0
    4ad4:	e59f01b4 	ldr	r0, [pc, #436]	; 4c90 <do_tt_randomizer+0x31c>
    4ad8:	e1a0e00f 	mov	lr, pc
    4adc:	e59bf000 	ldr	pc, [fp]
    4ae0:	e3a01000 	mov	r1, #0	; 0x0
    4ae4:	e5980000 	ldr	r0, [r8]
    4ae8:	ebffff40 	bl	47f0 <tt_simple_test>
    4aec:	e3a01000 	mov	r1, #0	; 0x0
    4af0:	e5c51001 	strb	r1, [r5, #1]
    4af4:	e59f0198 	ldr	r0, [pc, #408]	; 4c94 <do_tt_randomizer+0x320>
    4af8:	e1a0e00f 	mov	lr, pc
    4afc:	e59bf000 	ldr	pc, [fp]
    4b00:	e3a01001 	mov	r1, #1	; 0x1
    4b04:	e5980000 	ldr	r0, [r8]
    4b08:	ebffff38 	bl	47f0 <tt_simple_test>
    4b0c:	e3a02001 	mov	r2, #1	; 0x1
    4b10:	e5c52001 	strb	r2, [r5, #1]
    4b14:	e59f017c 	ldr	r0, [pc, #380]	; 4c98 <do_tt_randomizer+0x324>
    4b18:	e1a0e00f 	mov	lr, pc
    4b1c:	e59bf000 	ldr	pc, [fp]
    4b20:	e3a01001 	mov	r1, #1	; 0x1
    4b24:	e5980000 	ldr	r0, [r8]
    4b28:	ebffff30 	bl	47f0 <tt_simple_test>
    4b2c:	e2899001 	add	r9, r9, #1	; 0x1
    4b30:	e59f0164 	ldr	r0, [pc, #356]	; 4c9c <do_tt_randomizer+0x328>
    4b34:	e1a0e00f 	mov	lr, pc
    4b38:	e59bf000 	ldr	pc, [fp]
    4b3c:	e1a00009 	mov	r0, r9
    4b40:	e3a01005 	mov	r1, #5	; 0x5
    4b44:	eb004198 	bl	151ac <__aeabi_idivmod>
    4b48:	e28d3060 	add	r3, sp, #96	; 0x60
    4b4c:	e083c201 	add	ip, r3, r1, lsl #4
    4b50:	e51c3050 	ldr	r3, [ip, #-80]
    4b54:	e1a00005 	mov	r0, r5
    4b58:	e5853038 	str	r3, [r5, #56]
    4b5c:	e51c304c 	ldr	r3, [ip, #-76]
    4b60:	e585303c 	str	r3, [r5, #60]
    4b64:	e51c3048 	ldr	r3, [ip, #-72]
    4b68:	e5853040 	str	r3, [r5, #64]
    4b6c:	e51c3044 	ldr	r3, [ip, #-68]
    4b70:	e5853044 	str	r3, [r5, #68]
    4b74:	eb001e13 	bl	c3c8 <nand_init_randomizer>
    4b78:	e3a01001 	mov	r1, #1	; 0x1
    4b7c:	e5980000 	ldr	r0, [r8]
    4b80:	ebffff1a 	bl	47f0 <tt_simple_test>
    4b84:	e59f0114 	ldr	r0, [pc, #276]	; 4ca0 <do_tt_randomizer+0x32c>
    4b88:	e1a0e00f 	mov	lr, pc
    4b8c:	e59bf000 	ldr	pc, [fp]
    4b90:	e59a3000 	ldr	r3, [sl]
    4b94:	e1a00005 	mov	r0, r5
    4b98:	e5853038 	str	r3, [r5, #56]
    4b9c:	e59a3004 	ldr	r3, [sl, #4]
    4ba0:	e585303c 	str	r3, [r5, #60]
    4ba4:	e59a3008 	ldr	r3, [sl, #8]
    4ba8:	e5853040 	str	r3, [r5, #64]
    4bac:	e59a300c 	ldr	r3, [sl, #12]
    4bb0:	e28aa010 	add	sl, sl, #16	; 0x10
    4bb4:	e5853044 	str	r3, [r5, #68]
    4bb8:	eb001e02 	bl	c3c8 <nand_init_randomizer>
    4bbc:	e3a01001 	mov	r1, #1	; 0x1
    4bc0:	e5980000 	ldr	r0, [r8]
    4bc4:	ebffff09 	bl	47f0 <tt_simple_test>
    4bc8:	e59f00d4 	ldr	r0, [pc, #212]	; 4ca4 <do_tt_randomizer+0x330>
    4bcc:	e1a0e00f 	mov	lr, pc
    4bd0:	e59bf000 	ldr	pc, [fp]
    4bd4:	e59f20a8 	ldr	r2, [pc, #168]	; 4c84 <do_tt_randomizer+0x310>
    4bd8:	e59d1004 	ldr	r1, [sp, #4]
    4bdc:	e1a00005 	mov	r0, r5
    4be0:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    4be4:	e5853048 	str	r3, [r5, #72]
    4be8:	eb001df6 	bl	c3c8 <nand_init_randomizer>
    4bec:	e3a01001 	mov	r1, #1	; 0x1
    4bf0:	e5980000 	ldr	r0, [r8]
    4bf4:	ebfffefd 	bl	47f0 <tt_simple_test>
    4bf8:	e59f00a8 	ldr	r0, [pc, #168]	; 4ca8 <do_tt_randomizer+0x334>
    4bfc:	e1a0e00f 	mov	lr, pc
    4c00:	e59bf000 	ldr	pc, [fp]
    4c04:	e59d0000 	ldr	r0, [sp]
    4c08:	e5903000 	ldr	r3, [r0]
    4c0c:	e1a00005 	mov	r0, r5
    4c10:	e5853048 	str	r3, [r5, #72]
    4c14:	eb001deb 	bl	c3c8 <nand_init_randomizer>
    4c18:	e5980000 	ldr	r0, [r8]
    4c1c:	e3a01001 	mov	r1, #1	; 0x1
    4c20:	ebfffef2 	bl	47f0 <tt_simple_test>
    4c24:	e3590004 	cmp	r9, #4	; 0x4
    4c28:	0a000004 	beq	4c40 <do_tt_randomizer+0x2cc>
    4c2c:	e28d1010 	add	r1, sp, #16	; 0x10
    4c30:	e0817209 	add	r7, r1, r9, lsl #4
    4c34:	e1a06005 	mov	r6, r5
    4c38:	e3a04000 	mov	r4, #0	; 0x0
    4c3c:	eaffff8f 	b	4a80 <do_tt_randomizer+0x10c>
    4c40:	e59d2008 	ldr	r2, [sp, #8]
    4c44:	e59d300c 	ldr	r3, [sp, #12]
    4c48:	e2822001 	add	r2, r2, #1	; 0x1
    4c4c:	e2833004 	add	r3, r3, #4	; 0x4
    4c50:	e3520006 	cmp	r2, #6	; 0x6
    4c54:	e58d2008 	str	r2, [sp, #8]
    4c58:	e58d300c 	str	r3, [sp, #12]
    4c5c:	1affff73 	bne	4a30 <do_tt_randomizer+0xbc>
    4c60:	e3a00000 	mov	r0, #0	; 0x0
    4c64:	e28dd064 	add	sp, sp, #100	; 0x64
    4c68:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4c6c:	00015bac 	.word	0x00015bac
    4c70:	00017ab0 	.word	0x00017ab0
    4c74:	000166d1 	.word	0x000166d1
    4c78:	00017ad0 	.word	0x00017ad0
    4c7c:	3c00a5cc 	.word	0x3c00a5cc
    4c80:	00017ae7 	.word	0x00017ae7
    4c84:	000119d8 	.word	0x000119d8
    4c88:	00017b0c 	.word	0x00017b0c
    4c8c:	00017b24 	.word	0x00017b24
    4c90:	00017b3e 	.word	0x00017b3e
    4c94:	00017b61 	.word	0x00017b61
    4c98:	00017b8a 	.word	0x00017b8a
    4c9c:	00017bb2 	.word	0x00017bb2
    4ca0:	00017bda 	.word	0x00017bda
    4ca4:	00017c07 	.word	0x00017c07
    4ca8:	00017c2f 	.word	0x00017c2f

00004cac <tt_nand_cross_cmp>:
    4cac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4cb0:	e24dd004 	sub	sp, sp, #4	; 0x4
    4cb4:	e59da028 	ldr	sl, [sp, #40]
    4cb8:	e3530000 	cmp	r3, #0	; 0x0
    4cbc:	e1a05002 	mov	r5, r2
    4cc0:	13a02000 	movne	r2, #0	; 0x0
    4cc4:	059a2010 	ldreq	r2, [sl, #16]
    4cc8:	e3530000 	cmp	r3, #0	; 0x0
    4ccc:	e1a06001 	mov	r6, r1
    4cd0:	159a3010 	ldrne	r3, [sl, #16]
    4cd4:	e59a1014 	ldr	r1, [sl, #20]
    4cd8:	e1a07000 	mov	r7, r0
    4cdc:	e0838001 	add	r8, r3, r1
    4ce0:	e082b001 	add	fp, r2, r1
    4ce4:	e3a04000 	mov	r4, #0	; 0x0
    4ce8:	e2689000 	rsb	r9, r8, #0	; 0x0
    4cec:	e3a03000 	mov	r3, #0	; 0x0
    4cf0:	e1550003 	cmp	r5, r3
    4cf4:	e1a00007 	mov	r0, r7
    4cf8:	e1a01006 	mov	r1, r6
    4cfc:	e087700b 	add	r7, r7, fp
    4d00:	e0866008 	add	r6, r6, r8
    4d04:	e0855009 	add	r5, r5, r9
    4d08:	da000003 	ble	4d1c <tt_nand_cross_cmp+0x70>
    4d0c:	e59a2014 	ldr	r2, [sl, #20]
    4d10:	ebfffded 	bl	44cc <tt_cmp_buffer>
    4d14:	e0844000 	add	r4, r4, r0
    4d18:	eafffff3 	b	4cec <tt_nand_cross_cmp+0x40>
    4d1c:	e1a00004 	mov	r0, r4
    4d20:	e28dd004 	add	sp, sp, #4	; 0x4
    4d24:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00004d28 <do_tt_sysinfo>:
    4d28:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4d2c:	e3520001 	cmp	r2, #1	; 0x1
    4d30:	e24dd044 	sub	sp, sp, #68	; 0x44
    4d34:	e1a04003 	mov	r4, r3
    4d38:	ca000003 	bgt	4d4c <do_tt_sysinfo+0x24>
    4d3c:	e59f01ac 	ldr	r0, [pc, #428]	; 4ef0 <do_tt_sysinfo+0x1c8>
    4d40:	e59f31ac 	ldr	r3, [pc, #428]	; 4ef4 <do_tt_sysinfo+0x1cc>
    4d44:	e1a0e00f 	mov	lr, pc
    4d48:	e593f000 	ldr	pc, [r3]
    4d4c:	e3a00001 	mov	r0, #1	; 0x1
    4d50:	eb00186c 	bl	af08 <nand_get_config>
    4d54:	e2505000 	subs	r5, r0, #0	; 0x0
    4d58:	1a000004 	bne	4d70 <do_tt_sysinfo+0x48>
    4d5c:	e59f0194 	ldr	r0, [pc, #404]	; 4ef8 <do_tt_sysinfo+0x1d0>
    4d60:	e59f318c 	ldr	r3, [pc, #396]	; 4ef4 <do_tt_sysinfo+0x1cc>
    4d64:	e1a0e00f 	mov	lr, pc
    4d68:	e593f000 	ldr	pc, [r3]
    4d6c:	ea000004 	b	4d84 <do_tt_sysinfo+0x5c>
    4d70:	e59f0184 	ldr	r0, [pc, #388]	; 4efc <do_tt_sysinfo+0x1d4>
    4d74:	e3a01001 	mov	r1, #1	; 0x1
    4d78:	eb000593 	bl	63cc <vs_assign_by_name>
    4d7c:	e2507000 	subs	r7, r0, #0	; 0x0
    4d80:	0a000001 	beq	4d8c <do_tt_sysinfo+0x64>
    4d84:	e3e00000 	mvn	r0, #0	; 0x0
    4d88:	ea000056 	b	4ee8 <do_tt_sysinfo+0x1c0>
    4d8c:	e1a01007 	mov	r1, r7
    4d90:	e3a0200a 	mov	r2, #10	; 0xa
    4d94:	e5940004 	ldr	r0, [r4, #4]
    4d98:	eb0033f5 	bl	11d74 <simple_strtoul>
    4d9c:	e5850010 	str	r0, [r5, #16]
    4da0:	e1a04000 	mov	r4, r0
    4da4:	eb001885 	bl	afc0 <fnd_align>
    4da8:	e1a06480 	lsl	r6, r0, #9
    4dac:	e1a01004 	mov	r1, r4
    4db0:	e59f513c 	ldr	r5, [pc, #316]	; 4ef4 <do_tt_sysinfo+0x1cc>
    4db4:	e59f0144 	ldr	r0, [pc, #324]	; 4f00 <do_tt_sysinfo+0x1d8>
    4db8:	e1a02006 	mov	r2, r6
    4dbc:	e1a0e00f 	mov	lr, pc
    4dc0:	e595f000 	ldr	pc, [r5]
    4dc4:	e1a0400d 	mov	r4, sp
    4dc8:	e1a01007 	mov	r1, r7
    4dcc:	e59f0130 	ldr	r0, [pc, #304]	; 4f04 <do_tt_sysinfo+0x1dc>
    4dd0:	e1a0e00f 	mov	lr, pc
    4dd4:	e595f000 	ldr	pc, [r5]
    4dd8:	e59f0128 	ldr	r0, [pc, #296]	; 4f08 <do_tt_sysinfo+0x1e0>
    4ddc:	e1a0e00f 	mov	lr, pc
    4de0:	e595f000 	ldr	pc, [r5]
    4de4:	e1a02006 	mov	r2, r6
    4de8:	e59f111c 	ldr	r1, [pc, #284]	; 4f0c <do_tt_sysinfo+0x1e4>
    4dec:	e1a0000d 	mov	r0, sp
    4df0:	eb00376b 	bl	12ba4 <sprintf>
    4df4:	e3a01000 	mov	r1, #0	; 0x0
    4df8:	e1a0000d 	mov	r0, sp
    4dfc:	eb003830 	bl	12ec4 <run_command>
    4e00:	eb0037f2 	bl	12dd0 <tstc>
    4e04:	e2873001 	add	r3, r7, #1	; 0x1
    4e08:	e3500000 	cmp	r0, #0	; 0x0
    4e0c:	e1a07003 	mov	r7, r3
    4e10:	0a000002 	beq	4e20 <do_tt_sysinfo+0xf8>
    4e14:	eb0037ee 	bl	12dd4 <getc>
    4e18:	e3500003 	cmp	r0, #3	; 0x3
    4e1c:	0a000030 	beq	4ee4 <do_tt_sysinfo+0x1bc>
    4e20:	e59f00e8 	ldr	r0, [pc, #232]	; 4f10 <do_tt_sysinfo+0x1e8>
    4e24:	e1a0e00f 	mov	lr, pc
    4e28:	e595f000 	ldr	pc, [r5]
    4e2c:	e1a02006 	mov	r2, r6
    4e30:	e59f10dc 	ldr	r1, [pc, #220]	; 4f14 <do_tt_sysinfo+0x1ec>
    4e34:	e1a0000d 	mov	r0, sp
    4e38:	eb003759 	bl	12ba4 <sprintf>
    4e3c:	e3a01000 	mov	r1, #0	; 0x0
    4e40:	e1a0000d 	mov	r0, sp
    4e44:	eb00381e 	bl	12ec4 <run_command>
    4e48:	eb0037e0 	bl	12dd0 <tstc>
    4e4c:	e3500000 	cmp	r0, #0	; 0x0
    4e50:	0a000002 	beq	4e60 <do_tt_sysinfo+0x138>
    4e54:	eb0037de 	bl	12dd4 <getc>
    4e58:	e3500003 	cmp	r0, #3	; 0x3
    4e5c:	0a000020 	beq	4ee4 <do_tt_sysinfo+0x1bc>
    4e60:	e59f00b0 	ldr	r0, [pc, #176]	; 4f18 <do_tt_sysinfo+0x1f0>
    4e64:	e1a0e00f 	mov	lr, pc
    4e68:	e595f000 	ldr	pc, [r5]
    4e6c:	e1a02006 	mov	r2, r6
    4e70:	e59f10a4 	ldr	r1, [pc, #164]	; 4f1c <do_tt_sysinfo+0x1f4>
    4e74:	e1a0000d 	mov	r0, sp
    4e78:	eb003749 	bl	12ba4 <sprintf>
    4e7c:	e3a01000 	mov	r1, #0	; 0x0
    4e80:	e1a0000d 	mov	r0, sp
    4e84:	eb00380e 	bl	12ec4 <run_command>
    4e88:	eb0037d0 	bl	12dd0 <tstc>
    4e8c:	e3500000 	cmp	r0, #0	; 0x0
    4e90:	0a000002 	beq	4ea0 <do_tt_sysinfo+0x178>
    4e94:	eb0037ce 	bl	12dd4 <getc>
    4e98:	e3500003 	cmp	r0, #3	; 0x3
    4e9c:	0a000010 	beq	4ee4 <do_tt_sysinfo+0x1bc>
    4ea0:	e59f0078 	ldr	r0, [pc, #120]	; 4f20 <do_tt_sysinfo+0x1f8>
    4ea4:	e1a0e00f 	mov	lr, pc
    4ea8:	e595f000 	ldr	pc, [r5]
    4eac:	e59f1070 	ldr	r1, [pc, #112]	; 4f24 <do_tt_sysinfo+0x1fc>
    4eb0:	e1a02006 	mov	r2, r6
    4eb4:	e3a0300c 	mov	r3, #12	; 0xc
    4eb8:	e59f0068 	ldr	r0, [pc, #104]	; 4f28 <do_tt_sysinfo+0x200>
    4ebc:	ebfffd82 	bl	44cc <tt_cmp_buffer>
    4ec0:	e59f0064 	ldr	r0, [pc, #100]	; 4f2c <do_tt_sysinfo+0x204>
    4ec4:	e1a0e00f 	mov	lr, pc
    4ec8:	e595f000 	ldr	pc, [r5]
    4ecc:	eb0037bf 	bl	12dd0 <tstc>
    4ed0:	e3500000 	cmp	r0, #0	; 0x0
    4ed4:	0affffbb 	beq	4dc8 <do_tt_sysinfo+0xa0>
    4ed8:	eb0037bd 	bl	12dd4 <getc>
    4edc:	e3500003 	cmp	r0, #3	; 0x3
    4ee0:	1affffb8 	bne	4dc8 <do_tt_sysinfo+0xa0>
    4ee4:	e3a00000 	mov	r0, #0	; 0x0
    4ee8:	e28dd044 	add	sp, sp, #68	; 0x44
    4eec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4ef0:	00017c5c 	.word	0x00017c5c
    4ef4:	3c00a5cc 	.word	0x3c00a5cc
    4ef8:	00017c73 	.word	0x00017c73
    4efc:	000178a3 	.word	0x000178a3
    4f00:	00017c8e 	.word	0x00017c8e
    4f04:	0001756e 	.word	0x0001756e
    4f08:	00017a2a 	.word	0x00017a2a
    4f0c:	00017caf 	.word	0x00017caf
    4f10:	00017a4c 	.word	0x00017a4c
    4f14:	00017cc3 	.word	0x00017cc3
    4f18:	00017a77 	.word	0x00017a77
    4f1c:	00017ce0 	.word	0x00017ce0
    4f20:	00017aa1 	.word	0x00017aa1
    4f24:	50008000 	.word	0x50008000
    4f28:	40008000 	.word	0x40008000
    4f2c:	000171fb 	.word	0x000171fb

00004f30 <do_tt_sn>:
    4f30:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4f34:	e3520001 	cmp	r2, #1	; 0x1
    4f38:	e24dd044 	sub	sp, sp, #68	; 0x44
    4f3c:	e1a04002 	mov	r4, r2
    4f40:	e1a05003 	mov	r5, r3
    4f44:	ca000003 	bgt	4f58 <do_tt_sn+0x28>
    4f48:	e59f01d8 	ldr	r0, [pc, #472]	; 5128 <do_tt_sn+0x1f8>
    4f4c:	e59f31d8 	ldr	r3, [pc, #472]	; 512c <do_tt_sn+0x1fc>
    4f50:	e1a0e00f 	mov	lr, pc
    4f54:	e593f000 	ldr	pc, [r3]
    4f58:	e5950004 	ldr	r0, [r5, #4]
    4f5c:	e3a01001 	mov	r1, #1	; 0x1
    4f60:	eb000519 	bl	63cc <vs_assign_by_name>
    4f64:	e250a000 	subs	sl, r0, #0	; 0x0
    4f68:	13e00000 	mvnne	r0, #0	; 0x0
    4f6c:	1a00006b 	bne	5120 <do_tt_sn+0x1f0>
    4f70:	e5950004 	ldr	r0, [r5, #4]
    4f74:	e59f11b4 	ldr	r1, [pc, #436]	; 5130 <do_tt_sn+0x200>
    4f78:	e3a02003 	mov	r2, #3	; 0x3
    4f7c:	ebfff0ff 	bl	1380 <strncmp>
    4f80:	e3500000 	cmp	r0, #0	; 0x0
    4f84:	13a07501 	movne	r7, #4194304	; 0x400000
    4f88:	1a000001 	bne	4f94 <do_tt_sn+0x64>
    4f8c:	eb00180b 	bl	afc0 <fnd_align>
    4f90:	e1a07500 	lsl	r7, r0, #10
    4f94:	e5950004 	ldr	r0, [r5, #4]
    4f98:	eb0003ed 	bl	5f54 <vs_device_id>
    4f9c:	e3540002 	cmp	r4, #2	; 0x2
    4fa0:	e1a08000 	mov	r8, r0
    4fa4:	da000007 	ble	4fc8 <do_tt_sn+0x98>
    4fa8:	e3a01000 	mov	r1, #0	; 0x0
    4fac:	e3a0200a 	mov	r2, #10	; 0xa
    4fb0:	e5950008 	ldr	r0, [r5, #8]
    4fb4:	eb00336e 	bl	11d74 <simple_strtoul>
    4fb8:	e1a04000 	mov	r4, r0
    4fbc:	e1a00008 	mov	r0, r8
    4fc0:	eb000390 	bl	5e08 <vs_align>
    4fc4:	e0070094 	mul	r7, r4, r0
    4fc8:	e59f615c 	ldr	r6, [pc, #348]	; 512c <do_tt_sn+0x1fc>
    4fcc:	e1a00008 	mov	r0, r8
    4fd0:	e5954004 	ldr	r4, [r5, #4]
    4fd4:	e5965000 	ldr	r5, [r6]
    4fd8:	eb00038a 	bl	5e08 <vs_align>
    4fdc:	e1a01004 	mov	r1, r4
    4fe0:	e1a02000 	mov	r2, r0
    4fe4:	e59f0148 	ldr	r0, [pc, #328]	; 5134 <do_tt_sn+0x204>
    4fe8:	e12fff35 	blx	r5
    4fec:	e1a0400d 	mov	r4, sp
    4ff0:	e1a0100a 	mov	r1, sl
    4ff4:	e59f013c 	ldr	r0, [pc, #316]	; 5138 <do_tt_sn+0x208>
    4ff8:	e1a0e00f 	mov	lr, pc
    4ffc:	e596f000 	ldr	pc, [r6]
    5000:	e1a00008 	mov	r0, r8
    5004:	eb0003c6 	bl	5f24 <vs_device_erasable>
    5008:	e28a3001 	add	r3, sl, #1	; 0x1
    500c:	e3500000 	cmp	r0, #0	; 0x0
    5010:	e1a0a003 	mov	sl, r3
    5014:	e59f0120 	ldr	r0, [pc, #288]	; 513c <do_tt_sn+0x20c>
    5018:	0a00000e 	beq	5058 <do_tt_sn+0x128>
    501c:	e1a0e00f 	mov	lr, pc
    5020:	e596f000 	ldr	pc, [r6]
    5024:	e1a02007 	mov	r2, r7
    5028:	e59f1110 	ldr	r1, [pc, #272]	; 5140 <do_tt_sn+0x210>
    502c:	e1a0000d 	mov	r0, sp
    5030:	eb0036db 	bl	12ba4 <sprintf>
    5034:	e3a01000 	mov	r1, #0	; 0x0
    5038:	e1a0000d 	mov	r0, sp
    503c:	eb0037a0 	bl	12ec4 <run_command>
    5040:	eb003762 	bl	12dd0 <tstc>
    5044:	e3500000 	cmp	r0, #0	; 0x0
    5048:	0a000002 	beq	5058 <do_tt_sn+0x128>
    504c:	eb003760 	bl	12dd4 <getc>
    5050:	e3500003 	cmp	r0, #3	; 0x3
    5054:	0a000030 	beq	511c <do_tt_sn+0x1ec>
    5058:	e59f00e4 	ldr	r0, [pc, #228]	; 5144 <do_tt_sn+0x214>
    505c:	e1a0e00f 	mov	lr, pc
    5060:	e596f000 	ldr	pc, [r6]
    5064:	e1a02007 	mov	r2, r7
    5068:	e59f10d8 	ldr	r1, [pc, #216]	; 5148 <do_tt_sn+0x218>
    506c:	e1a0000d 	mov	r0, sp
    5070:	eb0036cb 	bl	12ba4 <sprintf>
    5074:	e3a01000 	mov	r1, #0	; 0x0
    5078:	e1a0000d 	mov	r0, sp
    507c:	eb003790 	bl	12ec4 <run_command>
    5080:	eb003752 	bl	12dd0 <tstc>
    5084:	e3500000 	cmp	r0, #0	; 0x0
    5088:	0a000002 	beq	5098 <do_tt_sn+0x168>
    508c:	eb003750 	bl	12dd4 <getc>
    5090:	e3500003 	cmp	r0, #3	; 0x3
    5094:	0a000020 	beq	511c <do_tt_sn+0x1ec>
    5098:	e59f00ac 	ldr	r0, [pc, #172]	; 514c <do_tt_sn+0x21c>
    509c:	e1a0e00f 	mov	lr, pc
    50a0:	e596f000 	ldr	pc, [r6]
    50a4:	e1a02007 	mov	r2, r7
    50a8:	e59f10a0 	ldr	r1, [pc, #160]	; 5150 <do_tt_sn+0x220>
    50ac:	e1a0000d 	mov	r0, sp
    50b0:	eb0036bb 	bl	12ba4 <sprintf>
    50b4:	e3a01000 	mov	r1, #0	; 0x0
    50b8:	e1a0000d 	mov	r0, sp
    50bc:	eb003780 	bl	12ec4 <run_command>
    50c0:	eb003742 	bl	12dd0 <tstc>
    50c4:	e3500000 	cmp	r0, #0	; 0x0
    50c8:	0a000002 	beq	50d8 <do_tt_sn+0x1a8>
    50cc:	eb003740 	bl	12dd4 <getc>
    50d0:	e3500003 	cmp	r0, #3	; 0x3
    50d4:	0a000010 	beq	511c <do_tt_sn+0x1ec>
    50d8:	e59f0074 	ldr	r0, [pc, #116]	; 5154 <do_tt_sn+0x224>
    50dc:	e1a0e00f 	mov	lr, pc
    50e0:	e596f000 	ldr	pc, [r6]
    50e4:	e59f106c 	ldr	r1, [pc, #108]	; 5158 <do_tt_sn+0x228>
    50e8:	e1a02007 	mov	r2, r7
    50ec:	e3a0300c 	mov	r3, #12	; 0xc
    50f0:	e59f0064 	ldr	r0, [pc, #100]	; 515c <do_tt_sn+0x22c>
    50f4:	ebfffcf4 	bl	44cc <tt_cmp_buffer>
    50f8:	e59f0060 	ldr	r0, [pc, #96]	; 5160 <do_tt_sn+0x230>
    50fc:	e1a0e00f 	mov	lr, pc
    5100:	e596f000 	ldr	pc, [r6]
    5104:	eb003731 	bl	12dd0 <tstc>
    5108:	e3500000 	cmp	r0, #0	; 0x0
    510c:	0affffb7 	beq	4ff0 <do_tt_sn+0xc0>
    5110:	eb00372f 	bl	12dd4 <getc>
    5114:	e3500003 	cmp	r0, #3	; 0x3
    5118:	1affffb4 	bne	4ff0 <do_tt_sn+0xc0>
    511c:	e3a00000 	mov	r0, #0	; 0x0
    5120:	e28dd044 	add	sp, sp, #68	; 0x44
    5124:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    5128:	00017cfc 	.word	0x00017cfc
    512c:	3c00a5cc 	.word	0x3c00a5cc
    5130:	000178a3 	.word	0x000178a3
    5134:	00017d09 	.word	0x00017d09
    5138:	0001756e 	.word	0x0001756e
    513c:	00017a2a 	.word	0x00017a2a
    5140:	00017caf 	.word	0x00017caf
    5144:	00017a4c 	.word	0x00017a4c
    5148:	00017cc3 	.word	0x00017cc3
    514c:	00017a77 	.word	0x00017a77
    5150:	00017ce0 	.word	0x00017ce0
    5154:	00017aa1 	.word	0x00017aa1
    5158:	50008000 	.word	0x50008000
    515c:	40008000 	.word	0x40008000
    5160:	000171fb 	.word	0x000171fb

00005164 <tt_nand_trolling>:
    5164:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5168:	e24dd034 	sub	sp, sp, #52	; 0x34
    516c:	e58d000c 	str	r0, [sp, #12]
    5170:	e1a06001 	mov	r6, r1
    5174:	e3a00001 	mov	r0, #1	; 0x1
    5178:	e3a01064 	mov	r1, #100	; 0x64
    517c:	e1a09002 	mov	r9, r2
    5180:	ebfffd25 	bl	461c <tt_rnd_get_int>
    5184:	e3500032 	cmp	r0, #50	; 0x32
    5188:	c5991020 	ldrgt	r1, [r9, #32]
    518c:	c58d1010 	strgt	r1, [sp, #16]
    5190:	ca000003 	bgt	51a4 <tt_nand_trolling+0x40>
    5194:	e3a00001 	mov	r0, #1	; 0x1
    5198:	e5991020 	ldr	r1, [r9, #32]
    519c:	ebfffd1e 	bl	461c <tt_rnd_get_int>
    51a0:	e58d0010 	str	r0, [sp, #16]
    51a4:	e59d200c 	ldr	r2, [sp, #12]
    51a8:	e5991014 	ldr	r1, [r9, #20]
    51ac:	e3520002 	cmp	r2, #2	; 0x2
    51b0:	05993010 	ldreq	r3, [r9, #16]
    51b4:	059d2010 	ldreq	r2, [sp, #16]
    51b8:	159d3010 	ldrne	r3, [sp, #16]
    51bc:	00833001 	addeq	r3, r3, r1
    51c0:	00010293 	muleq	r1, r3, r2
    51c4:	10010391 	mulne	r1, r1, r3
    51c8:	e28d002c 	add	r0, sp, #44	; 0x2c
    51cc:	e1a02001 	mov	r2, r1
    51d0:	ebfffd3a 	bl	46c0 <tt_rnd_get_data>
    51d4:	e3500000 	cmp	r0, #0	; 0x0
    51d8:	e58d0014 	str	r0, [sp, #20]
    51dc:	1a000004 	bne	51f4 <tt_nand_trolling+0x90>
    51e0:	e59f0330 	ldr	r0, [pc, #816]	; 5518 <tt_nand_trolling+0x3b4>
    51e4:	e59f3330 	ldr	r3, [pc, #816]	; 551c <tt_nand_trolling+0x3b8>
    51e8:	e1a0e00f 	mov	lr, pc
    51ec:	e593f000 	ldr	pc, [r3]
    51f0:	ea000028 	b	5298 <tt_nand_trolling+0x134>
    51f4:	e5991024 	ldr	r1, [r9, #36]
    51f8:	e3a00000 	mov	r0, #0	; 0x0
    51fc:	ebfffd06 	bl	461c <tt_rnd_get_int>
    5200:	e5995020 	ldr	r5, [r9, #32]
    5204:	e59d2010 	ldr	r2, [sp, #16]
    5208:	e1a04000 	mov	r4, r0
    520c:	e0621005 	rsb	r1, r2, r5
    5210:	e3a00000 	mov	r0, #0	; 0x0
    5214:	ebfffd00 	bl	461c <tt_rnd_get_int>
    5218:	e59d300c 	ldr	r3, [sp, #12]
    521c:	e0240495 	mla	r4, r5, r4, r0
    5220:	e3530000 	cmp	r3, #0	; 0x0
    5224:	05963014 	ldreq	r3, [r6, #20]
    5228:	15993014 	ldrne	r3, [r9, #20]
    522c:	e1a05fc4 	asr	r5, r4, #31
    5230:	e0876394 	umull	r6, r7, r4, r3
    5234:	e3a01000 	mov	r1, #0	; 0x0
    5238:	e3a02010 	mov	r2, #16	; 0x10
    523c:	e28d001c 	add	r0, sp, #28	; 0x1c
    5240:	e0277593 	mla	r7, r3, r5, r7
    5244:	ebfff07e 	bl	1444 <memset>
    5248:	e59f32d0 	ldr	r3, [pc, #720]	; 5520 <tt_nand_trolling+0x3bc>
    524c:	e59d200c 	ldr	r2, [sp, #12]
    5250:	e58d301c 	str	r3, [sp, #28]
    5254:	e59f32c8 	ldr	r3, [pc, #712]	; 5524 <tt_nand_trolling+0x3c0>
    5258:	e28d1030 	add	r1, sp, #48	; 0x30
    525c:	e58d3020 	str	r3, [sp, #32]
    5260:	e59f32c0 	ldr	r3, [pc, #704]	; 5528 <tt_nand_trolling+0x3c4>
    5264:	e59f82b0 	ldr	r8, [pc, #688]	; 551c <tt_nand_trolling+0x3b8>
    5268:	e58d3024 	str	r3, [sp, #36]
    526c:	e0813102 	add	r3, r1, r2, lsl #2
    5270:	e513b014 	ldr	fp, [r3, #-20]
    5274:	e3a01001 	mov	r1, #1	; 0x1
    5278:	e1a0000b 	mov	r0, fp
    527c:	eb000452 	bl	63cc <vs_assign_by_name>
    5280:	e3500000 	cmp	r0, #0	; 0x0
    5284:	0a000005 	beq	52a0 <tt_nand_trolling+0x13c>
    5288:	e1a0100b 	mov	r1, fp
    528c:	e59f0298 	ldr	r0, [pc, #664]	; 552c <tt_nand_trolling+0x3c8>
    5290:	e1a0e00f 	mov	lr, pc
    5294:	e598f000 	ldr	pc, [r8]
    5298:	e3e00000 	mvn	r0, #0	; 0x0
    529c:	ea00009b 	b	5510 <tt_nand_trolling+0x3ac>
    52a0:	e59d2014 	ldr	r2, [sp, #20]
    52a4:	e59d302c 	ldr	r3, [sp, #44]
    52a8:	e59f0280 	ldr	r0, [pc, #640]	; 5530 <tt_nand_trolling+0x3cc>
    52ac:	e1a0100b 	mov	r1, fp
    52b0:	e88d00c0 	stm	sp, {r6, r7}
    52b4:	e1a0e00f 	mov	lr, pc
    52b8:	e598f000 	ldr	pc, [r8]
    52bc:	e1a00006 	mov	r0, r6
    52c0:	e1a01007 	mov	r1, r7
    52c4:	eb0002a2 	bl	5d54 <vs_isbad>
    52c8:	e250a000 	subs	sl, r0, #0	; 0x0
    52cc:	0a000004 	beq	52e4 <tt_nand_trolling+0x180>
    52d0:	e59f025c 	ldr	r0, [pc, #604]	; 5534 <tt_nand_trolling+0x3d0>
    52d4:	e1a0e00f 	mov	lr, pc
    52d8:	e598f000 	ldr	pc, [r8]
    52dc:	e3e0000a 	mvn	r0, #10	; 0xa
    52e0:	ea00008a 	b	5510 <tt_nand_trolling+0x3ac>
    52e4:	e1a01007 	mov	r1, r7
    52e8:	e59d202c 	ldr	r2, [sp, #44]
    52ec:	e1a00006 	mov	r0, r6
    52f0:	eb000288 	bl	5d18 <vs_erase>
    52f4:	e1a0000a 	mov	r0, sl
    52f8:	eb002508 	bl	e720 <get_timer>
    52fc:	e59d102c 	ldr	r1, [sp, #44]
    5300:	e1a02006 	mov	r2, r6
    5304:	e1a03007 	mov	r3, r7
    5308:	e1a04000 	mov	r4, r0
    530c:	e59d0014 	ldr	r0, [sp, #20]
    5310:	e88d0402 	stm	sp, {r1, sl}
    5314:	eb000269 	bl	5cc0 <vs_write>
    5318:	e1a00004 	mov	r0, r4
    531c:	eb0024ff 	bl	e720 <get_timer>
    5320:	e1a01000 	mov	r1, r0
    5324:	e59d002c 	ldr	r0, [sp, #44]
    5328:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    532c:	e1a00540 	asr	r0, r0, #10
    5330:	e0000095 	mul	r0, r5, r0
    5334:	eb003e92 	bl	14d84 <__aeabi_uidiv>
    5338:	e58d0008 	str	r0, [sp, #8]
    533c:	e1a0000a 	mov	r0, sl
    5340:	eb0024f6 	bl	e720 <get_timer>
    5344:	e59d102c 	ldr	r1, [sp, #44]
    5348:	e1a02006 	mov	r2, r6
    534c:	e1a03007 	mov	r3, r7
    5350:	e1a04000 	mov	r4, r0
    5354:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5358:	e88d0402 	stm	sp, {r1, sl}
    535c:	eb000329 	bl	6008 <vs_read>
    5360:	e1a00004 	mov	r0, r4
    5364:	eb0024ed 	bl	e720 <get_timer>
    5368:	e59d402c 	ldr	r4, [sp, #44]
    536c:	e1a01000 	mov	r1, r0
    5370:	e1a00544 	asr	r0, r4, #10
    5374:	e0000095 	mul	r0, r5, r0
    5378:	eb003e81 	bl	14d84 <__aeabi_uidiv>
    537c:	e1a02004 	mov	r2, r4
    5380:	e1a05000 	mov	r5, r0
    5384:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    5388:	e59d0014 	ldr	r0, [sp, #20]
    538c:	e3a0300c 	mov	r3, #12	; 0xc
    5390:	ebfffc4d 	bl	44cc <tt_cmp_buffer>
    5394:	e3500000 	cmp	r0, #0	; 0x0
    5398:	0a000007 	beq	53bc <tt_nand_trolling+0x258>
    539c:	e58d0000 	str	r0, [sp]
    53a0:	e1a0100b 	mov	r1, fp
    53a4:	e59d2008 	ldr	r2, [sp, #8]
    53a8:	e1a03005 	mov	r3, r5
    53ac:	e59f0184 	ldr	r0, [pc, #388]	; 5538 <tt_nand_trolling+0x3d4>
    53b0:	e1a0e00f 	mov	lr, pc
    53b4:	e598f000 	ldr	pc, [r8]
    53b8:	ea000005 	b	53d4 <tt_nand_trolling+0x270>
    53bc:	e1a0100b 	mov	r1, fp
    53c0:	e59d2008 	ldr	r2, [sp, #8]
    53c4:	e1a03005 	mov	r3, r5
    53c8:	e59f016c 	ldr	r0, [pc, #364]	; 553c <tt_nand_trolling+0x3d8>
    53cc:	e1a0e00f 	mov	lr, pc
    53d0:	e598f000 	ldr	pc, [r8]
    53d4:	e59d102c 	ldr	r1, [sp, #44]
    53d8:	e59f0160 	ldr	r0, [pc, #352]	; 5540 <tt_nand_trolling+0x3dc>
    53dc:	e890000c 	ldm	r0, {r2, r3}
    53e0:	e0922001 	adds	r2, r2, r1
    53e4:	e0a33fc1 	adc	r3, r3, r1, asr #31
    53e8:	e59d100c 	ldr	r1, [sp, #12]
    53ec:	e880000c 	stm	r0, {r2, r3}
    53f0:	e3510000 	cmp	r1, #0	; 0x0
    53f4:	0a000040 	beq	54fc <tt_nand_trolling+0x398>
    53f8:	e2618003 	rsb	r8, r1, #3	; 0x3
    53fc:	e28d2030 	add	r2, sp, #48	; 0x30
    5400:	e0823108 	add	r3, r2, r8, lsl #2
    5404:	e513a014 	ldr	sl, [r3, #-20]
    5408:	e3a01001 	mov	r1, #1	; 0x1
    540c:	e1a0000a 	mov	r0, sl
    5410:	eb0003ed 	bl	63cc <vs_assign_by_name>
    5414:	e3500000 	cmp	r0, #0	; 0x0
    5418:	0a000005 	beq	5434 <tt_nand_trolling+0x2d0>
    541c:	e1a0100a 	mov	r1, sl
    5420:	e59f0104 	ldr	r0, [pc, #260]	; 552c <tt_nand_trolling+0x3c8>
    5424:	e59f30f0 	ldr	r3, [pc, #240]	; 551c <tt_nand_trolling+0x3b8>
    5428:	e1a0e00f 	mov	lr, pc
    542c:	e593f000 	ldr	pc, [r3]
    5430:	eaffff98 	b	5298 <tt_nand_trolling+0x134>
    5434:	e3580001 	cmp	r8, #1	; 0x1
    5438:	15990010 	ldrne	r0, [r9, #16]
    543c:	e5993014 	ldr	r3, [r9, #20]
    5440:	e59d1010 	ldr	r1, [sp, #16]
    5444:	e0803003 	add	r3, r0, r3
    5448:	e0030391 	mul	r3, r1, r3
    544c:	e3a00000 	mov	r0, #0	; 0x0
    5450:	e58d302c 	str	r3, [sp, #44]
    5454:	eb0024b1 	bl	e720 <get_timer>
    5458:	e59d102c 	ldr	r1, [sp, #44]
    545c:	e1a02006 	mov	r2, r6
    5460:	e1a04000 	mov	r4, r0
    5464:	e1a03007 	mov	r3, r7
    5468:	e58d1000 	str	r1, [sp]
    546c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5470:	e3a01000 	mov	r1, #0	; 0x0
    5474:	e58d1004 	str	r1, [sp, #4]
    5478:	eb0002e2 	bl	6008 <vs_read>
    547c:	e1a00004 	mov	r0, r4
    5480:	eb0024a6 	bl	e720 <get_timer>
    5484:	e59d402c 	ldr	r4, [sp, #44]
    5488:	e1a01000 	mov	r1, r0
    548c:	e1a03544 	asr	r3, r4, #10
    5490:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    5494:	e0000390 	mul	r0, r0, r3
    5498:	eb003e39 	bl	14d84 <__aeabi_uidiv>
    549c:	e3580002 	cmp	r8, #2	; 0x2
    54a0:	13a03000 	movne	r3, #0	; 0x0
    54a4:	03a03001 	moveq	r3, #1	; 0x1
    54a8:	e1a05000 	mov	r5, r0
    54ac:	e1a02004 	mov	r2, r4
    54b0:	e59d0014 	ldr	r0, [sp, #20]
    54b4:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    54b8:	e58d9000 	str	r9, [sp]
    54bc:	ebfffdfa 	bl	4cac <tt_nand_cross_cmp>
    54c0:	e3500000 	cmp	r0, #0	; 0x0
    54c4:	e59f8050 	ldr	r8, [pc, #80]	; 551c <tt_nand_trolling+0x3b8>
    54c8:	0a000006 	beq	54e8 <tt_nand_trolling+0x384>
    54cc:	e1a03000 	mov	r3, r0
    54d0:	e1a0100a 	mov	r1, sl
    54d4:	e1a02005 	mov	r2, r5
    54d8:	e59f0064 	ldr	r0, [pc, #100]	; 5544 <tt_nand_trolling+0x3e0>
    54dc:	e1a0e00f 	mov	lr, pc
    54e0:	e598f000 	ldr	pc, [r8]
    54e4:	ea000004 	b	54fc <tt_nand_trolling+0x398>
    54e8:	e1a0100a 	mov	r1, sl
    54ec:	e1a02005 	mov	r2, r5
    54f0:	e59f0050 	ldr	r0, [pc, #80]	; 5548 <tt_nand_trolling+0x3e4>
    54f4:	e1a0e00f 	mov	lr, pc
    54f8:	e598f000 	ldr	pc, [r8]
    54fc:	e59f0048 	ldr	r0, [pc, #72]	; 554c <tt_nand_trolling+0x3e8>
    5500:	e59f3014 	ldr	r3, [pc, #20]	; 551c <tt_nand_trolling+0x3b8>
    5504:	e1a0e00f 	mov	lr, pc
    5508:	e593f000 	ldr	pc, [r3]
    550c:	e3a00000 	mov	r0, #0	; 0x0
    5510:	e28dd034 	add	sp, sp, #52	; 0x34
    5514:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5518:	00017d27 	.word	0x00017d27
    551c:	3c00a5cc 	.word	0x3c00a5cc
    5520:	000166d1 	.word	0x000166d1
    5524:	000178a7 	.word	0x000178a7
    5528:	000178a3 	.word	0x000178a3
    552c:	00017d46 	.word	0x00017d46
    5530:	00017d63 	.word	0x00017d63
    5534:	00017d93 	.word	0x00017d93
    5538:	00017dbb 	.word	0x00017dbb
    553c:	00017df4 	.word	0x00017df4
    5540:	3c00a068 	.word	0x3c00a068
    5544:	00017e20 	.word	0x00017e20
    5548:	00017e4d 	.word	0x00017e4d
    554c:	000171fb 	.word	0x000171fb

00005550 <do_tt_ndloop>:
    5550:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5554:	e3a00001 	mov	r0, #1	; 0x1
    5558:	e24dd004 	sub	sp, sp, #4	; 0x4
    555c:	eb001669 	bl	af08 <nand_get_config>
    5560:	e2505000 	subs	r5, r0, #0	; 0x0
    5564:	1a000005 	bne	5580 <do_tt_ndloop+0x30>
    5568:	e59f00d0 	ldr	r0, [pc, #208]	; 5640 <do_tt_ndloop+0xf0>
    556c:	e59f30d0 	ldr	r3, [pc, #208]	; 5644 <do_tt_ndloop+0xf4>
    5570:	e1a0e00f 	mov	lr, pc
    5574:	e593f000 	ldr	pc, [r3]
    5578:	e3e00001 	mvn	r0, #1	; 0x1
    557c:	ea00002d 	b	5638 <do_tt_ndloop+0xe8>
    5580:	e3a00000 	mov	r0, #0	; 0x0
    5584:	eb002465 	bl	e720 <get_timer>
    5588:	e59f10b8 	ldr	r1, [pc, #184]	; 5648 <do_tt_ndloop+0xf8>
    558c:	e3a02000 	mov	r2, #0	; 0x0
    5590:	e3a03000 	mov	r3, #0	; 0x0
    5594:	e881000c 	stm	r1, {r2, r3}
    5598:	e59f70a4 	ldr	r7, [pc, #164]	; 5644 <do_tt_ndloop+0xf4>
    559c:	e1a06000 	mov	r6, r0
    55a0:	e3a04000 	mov	r4, #0	; 0x0
    55a4:	e1a01004 	mov	r1, r4
    55a8:	e59f009c 	ldr	r0, [pc, #156]	; 564c <do_tt_ndloop+0xfc>
    55ac:	e1a0e00f 	mov	lr, pc
    55b0:	e597f000 	ldr	pc, [r7]
    55b4:	e3a01000 	mov	r1, #0	; 0x0
    55b8:	e1a02005 	mov	r2, r5
    55bc:	e3a00001 	mov	r0, #1	; 0x1
    55c0:	ebfffee7 	bl	5164 <tt_nand_trolling>
    55c4:	e3500000 	cmp	r0, #0	; 0x0
    55c8:	e2844001 	add	r4, r4, #1	; 0x1
    55cc:	1a000003 	bne	55e0 <do_tt_ndloop+0x90>
    55d0:	e1a01000 	mov	r1, r0
    55d4:	e1a02005 	mov	r2, r5
    55d8:	e3a00002 	mov	r0, #2	; 0x2
    55dc:	ebfffee0 	bl	5164 <tt_nand_trolling>
    55e0:	eb0035fa 	bl	12dd0 <tstc>
    55e4:	e3500000 	cmp	r0, #0	; 0x0
    55e8:	0affffed 	beq	55a4 <do_tt_ndloop+0x54>
    55ec:	eb0035f8 	bl	12dd4 <getc>
    55f0:	e3500003 	cmp	r0, #3	; 0x3
    55f4:	1affffea 	bne	55a4 <do_tt_ndloop+0x54>
    55f8:	e1a00006 	mov	r0, r6
    55fc:	eb002447 	bl	e720 <get_timer>
    5600:	e59f3040 	ldr	r3, [pc, #64]	; 5648 <do_tt_ndloop+0xf8>
    5604:	e30e1a60 	movw	r1, #60000	; 0xea60
    5608:	e5934000 	ldr	r4, [r3]
    560c:	e5933004 	ldr	r3, [r3, #4]
    5610:	e1a04a24 	lsr	r4, r4, #20
    5614:	e1844603 	orr	r4, r4, r3, lsl #12
    5618:	eb003e5b 	bl	14f8c <__aeabi_idiv>
    561c:	e1a01004 	mov	r1, r4
    5620:	e1a02000 	mov	r2, r0
    5624:	e59f3018 	ldr	r3, [pc, #24]	; 5644 <do_tt_ndloop+0xf4>
    5628:	e59f0020 	ldr	r0, [pc, #32]	; 5650 <do_tt_ndloop+0x100>
    562c:	e1a0e00f 	mov	lr, pc
    5630:	e593f000 	ldr	pc, [r3]
    5634:	e3a00000 	mov	r0, #0	; 0x0
    5638:	e28dd004 	add	sp, sp, #4	; 0x4
    563c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5640:	00017e6d 	.word	0x00017e6d
    5644:	3c00a5cc 	.word	0x3c00a5cc
    5648:	3c00a068 	.word	0x3c00a068
    564c:	0001756e 	.word	0x0001756e
    5650:	00017578 	.word	0x00017578

00005654 <do_tt_nand>:
    5654:	e3520001 	cmp	r2, #1	; 0x1
    5658:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    565c:	e1a08002 	mov	r8, r2
    5660:	e1a0a003 	mov	sl, r3
    5664:	c59f90e0 	ldrgt	r9, [pc, #224]	; 574c <do_tt_nand+0xf8>
    5668:	c3a06001 	movgt	r6, #1	; 0x1
    566c:	ca000005 	bgt	5688 <do_tt_nand+0x34>
    5670:	e59f00d8 	ldr	r0, [pc, #216]	; 5750 <do_tt_nand+0xfc>
    5674:	e59f30d0 	ldr	r3, [pc, #208]	; 574c <do_tt_nand+0xf8>
    5678:	e1a0e00f 	mov	lr, pc
    567c:	e593f000 	ldr	pc, [r3]
    5680:	e3e00000 	mvn	r0, #0	; 0x0
    5684:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    5688:	e3a0200a 	mov	r2, #10	; 0xa
    568c:	e3a01000 	mov	r1, #0	; 0x0
    5690:	e79a0106 	ldr	r0, [sl, r6, lsl #2]
    5694:	eb0031b6 	bl	11d74 <simple_strtoul>
    5698:	e1a07000 	mov	r7, r0
    569c:	ebffec44 	bl	7b4 <set_xom>
    56a0:	e3a00000 	mov	r0, #0	; 0x0
    56a4:	e1a01000 	mov	r1, r0
    56a8:	eb001b89 	bl	c4d4 <nand_rescan_config>
    56ac:	e3a00000 	mov	r0, #0	; 0x0
    56b0:	eb001614 	bl	af08 <nand_get_config>
    56b4:	e1a05000 	mov	r5, r0
    56b8:	e3a00001 	mov	r0, #1	; 0x1
    56bc:	eb001611 	bl	af08 <nand_get_config>
    56c0:	e3500000 	cmp	r0, #0	; 0x0
    56c4:	13550000 	cmpne	r5, #0	; 0x0
    56c8:	e1a04000 	mov	r4, r0
    56cc:	1a000005 	bne	56e8 <do_tt_nand+0x94>
    56d0:	e59f007c 	ldr	r0, [pc, #124]	; 5754 <do_tt_nand+0x100>
    56d4:	e59f3070 	ldr	r3, [pc, #112]	; 574c <do_tt_nand+0xf8>
    56d8:	e1a0e00f 	mov	lr, pc
    56dc:	e593f000 	ldr	pc, [r3]
    56e0:	e3e00001 	mvn	r0, #1	; 0x1
    56e4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    56e8:	e1a01007 	mov	r1, r7
    56ec:	e59f0064 	ldr	r0, [pc, #100]	; 5758 <do_tt_nand+0x104>
    56f0:	e1a0e00f 	mov	lr, pc
    56f4:	e599f000 	ldr	pc, [r9]
    56f8:	e3a00000 	mov	r0, #0	; 0x0
    56fc:	e1a01005 	mov	r1, r5
    5700:	e1a02004 	mov	r2, r4
    5704:	ebfffe96 	bl	5164 <tt_nand_trolling>
    5708:	e370000b 	cmn	r0, #11	; 0xb
    570c:	0afffff9 	beq	56f8 <do_tt_nand+0xa4>
    5710:	e3a00001 	mov	r0, #1	; 0x1
    5714:	e1a01005 	mov	r1, r5
    5718:	e1a02004 	mov	r2, r4
    571c:	ebfffe90 	bl	5164 <tt_nand_trolling>
    5720:	e370000b 	cmn	r0, #11	; 0xb
    5724:	0afffff9 	beq	5710 <do_tt_nand+0xbc>
    5728:	e1a01005 	mov	r1, r5
    572c:	e1a02004 	mov	r2, r4
    5730:	e2866001 	add	r6, r6, #1	; 0x1
    5734:	e3a00002 	mov	r0, #2	; 0x2
    5738:	ebfffe89 	bl	5164 <tt_nand_trolling>
    573c:	e1560008 	cmp	r6, r8
    5740:	baffffd0 	blt	5688 <do_tt_nand+0x34>
    5744:	e3a00000 	mov	r0, #0	; 0x0
    5748:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    574c:	3c00a5cc 	.word	0x3c00a5cc
    5750:	00017e9f 	.word	0x00017e9f
    5754:	00017e6d 	.word	0x00017e6d
    5758:	00017ed5 	.word	0x00017ed5

0000575c <tt_wire_get_opt>:
    575c:	e5900008 	ldr	r0, [r0, #8]
    5760:	e12fff1e 	bx	lr

00005764 <tt_wire_get_len>:
    5764:	e590000c 	ldr	r0, [r0, #12]
    5768:	e12fff1e 	bx	lr

0000576c <tt_wire_make_data>:
    576c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5770:	e24dd004 	sub	sp, sp, #4	; 0x4
    5774:	e1a04000 	mov	r4, r0
    5778:	e1a07002 	mov	r7, r2
    577c:	e1a06003 	mov	r6, r3
    5780:	ebffef37 	bl	1464 <memcpy>
    5784:	e59f3040 	ldr	r3, [pc, #64]	; 57cc <tt_wire_make_data+0x60>
    5788:	e5846008 	str	r6, [r4, #8]
    578c:	e5843000 	str	r3, [r4]
    5790:	e59d3018 	ldr	r3, [sp, #24]
    5794:	e3a05000 	mov	r5, #0	; 0x0
    5798:	e584300c 	str	r3, [r4, #12]
    579c:	e1a01007 	mov	r1, r7
    57a0:	e5845004 	str	r5, [r4, #4]
    57a4:	e1a00005 	mov	r0, r5
    57a8:	eb000463 	bl	693c <hash_init>
    57ac:	e1a01007 	mov	r1, r7
    57b0:	e1a00004 	mov	r0, r4
    57b4:	eb000440 	bl	68bc <hash_data>
    57b8:	e2840004 	add	r0, r4, #4	; 0x4
    57bc:	eb000422 	bl	684c <hash_value>
    57c0:	e1a00005 	mov	r0, r5
    57c4:	e28dd004 	add	sp, sp, #4	; 0x4
    57c8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    57cc:	7766dd80 	.word	0x7766dd80

000057d0 <tt_wire_check_valid>:
    57d0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    57d4:	e5903000 	ldr	r3, [r0]
    57d8:	e59f2084 	ldr	r2, [pc, #132]	; 5864 <tt_wire_check_valid+0x94>
    57dc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    57e0:	e1530002 	cmp	r3, r2
    57e4:	e1a04000 	mov	r4, r0
    57e8:	e1a06001 	mov	r6, r1
    57ec:	e5907004 	ldr	r7, [r0, #4]
    57f0:	0a000006 	beq	5810 <tt_wire_check_valid+0x40>
    57f4:	e1a01003 	mov	r1, r3
    57f8:	e59f0068 	ldr	r0, [pc, #104]	; 5868 <tt_wire_check_valid+0x98>
    57fc:	e59f3068 	ldr	r3, [pc, #104]	; 586c <tt_wire_check_valid+0x9c>
    5800:	e1a0e00f 	mov	lr, pc
    5804:	e593f000 	ldr	pc, [r3]
    5808:	e3a00000 	mov	r0, #0	; 0x0
    580c:	ea000012 	b	585c <tt_wire_check_valid+0x8c>
    5810:	e3a05000 	mov	r5, #0	; 0x0
    5814:	e5805004 	str	r5, [r0, #4]
    5818:	e1a00005 	mov	r0, r5
    581c:	eb000446 	bl	693c <hash_init>
    5820:	e1a01006 	mov	r1, r6
    5824:	e1a00004 	mov	r0, r4
    5828:	eb000423 	bl	68bc <hash_data>
    582c:	e28d0004 	add	r0, sp, #4	; 0x4
    5830:	eb000405 	bl	684c <hash_value>
    5834:	e59d1004 	ldr	r1, [sp, #4]
    5838:	e1510007 	cmp	r1, r7
    583c:	03a00001 	moveq	r0, #1	; 0x1
    5840:	0a000005 	beq	585c <tt_wire_check_valid+0x8c>
    5844:	e59f0024 	ldr	r0, [pc, #36]	; 5870 <tt_wire_check_valid+0xa0>
    5848:	e1a02007 	mov	r2, r7
    584c:	e59f3018 	ldr	r3, [pc, #24]	; 586c <tt_wire_check_valid+0x9c>
    5850:	e1a0e00f 	mov	lr, pc
    5854:	e593f000 	ldr	pc, [r3]
    5858:	e1a00005 	mov	r0, r5
    585c:	e28dd00c 	add	sp, sp, #12	; 0xc
    5860:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5864:	7766dd80 	.word	0x7766dd80
    5868:	00017f4f 	.word	0x00017f4f
    586c:	3c00a5cc 	.word	0x3c00a5cc
    5870:	00017f78 	.word	0x00017f78

00005874 <do_tt_wire>:
    5874:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5878:	e3520001 	cmp	r2, #1	; 0x1
    587c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    5880:	e1a04003 	mov	r4, r3
    5884:	d59f032c 	ldrle	r0, [pc, #812]	; 5bb8 <do_tt_wire+0x344>
    5888:	da00002b 	ble	593c <do_tt_wire+0xc8>
    588c:	e3a02003 	mov	r2, #3	; 0x3
    5890:	e5930004 	ldr	r0, [r3, #4]
    5894:	e59f1320 	ldr	r1, [pc, #800]	; 5bbc <do_tt_wire+0x348>
    5898:	ebffeeb8 	bl	1380 <strncmp>
    589c:	e3500000 	cmp	r0, #0	; 0x0
    58a0:	059f2318 	ldreq	r2, [pc, #792]	; 5bc0 <do_tt_wire+0x34c>
    58a4:	0a000006 	beq	58c4 <do_tt_wire+0x50>
    58a8:	e5940004 	ldr	r0, [r4, #4]
    58ac:	e59f1310 	ldr	r1, [pc, #784]	; 5bc4 <do_tt_wire+0x350>
    58b0:	e3a02003 	mov	r2, #3	; 0x3
    58b4:	ebffeeb1 	bl	1380 <strncmp>
    58b8:	e3500000 	cmp	r0, #0	; 0x0
    58bc:	1a000009 	bne	58e8 <do_tt_wire+0x74>
    58c0:	e59f2300 	ldr	r2, [pc, #768]	; 5bc8 <do_tt_wire+0x354>
    58c4:	e59f3300 	ldr	r3, [pc, #768]	; 5bcc <do_tt_wire+0x358>
    58c8:	e3a00000 	mov	r0, #0	; 0x0
    58cc:	e5832000 	str	r2, [r3]
    58d0:	eb002392 	bl	e720 <get_timer>
    58d4:	e59f82f4 	ldr	r8, [pc, #756]	; 5bd0 <do_tt_wire+0x35c>
    58d8:	e59f92ec 	ldr	r9, [pc, #748]	; 5bcc <do_tt_wire+0x358>
    58dc:	e1a0a008 	mov	sl, r8
    58e0:	e58d000c 	str	r0, [sp, #12]
    58e4:	ea000006 	b	5904 <do_tt_wire+0x90>
    58e8:	e5941004 	ldr	r1, [r4, #4]
    58ec:	e59f02e0 	ldr	r0, [pc, #736]	; 5bd4 <do_tt_wire+0x360>
    58f0:	e59f32d8 	ldr	r3, [pc, #728]	; 5bd0 <do_tt_wire+0x35c>
    58f4:	e1a0e00f 	mov	lr, pc
    58f8:	e593f000 	ldr	pc, [r3]
    58fc:	e3e00000 	mvn	r0, #0	; 0x0
    5900:	ea0000aa 	b	5bb0 <do_tt_wire+0x33c>
    5904:	e59f02cc 	ldr	r0, [pc, #716]	; 5bd8 <do_tt_wire+0x364>
    5908:	eb002360 	bl	e690 <udelay>
    590c:	e59f02c8 	ldr	r0, [pc, #712]	; 5bdc <do_tt_wire+0x368>
    5910:	e1a0e00f 	mov	lr, pc
    5914:	e598f000 	ldr	pc, [r8]
    5918:	e5993000 	ldr	r3, [r9]
    591c:	e1a0e00f 	mov	lr, pc
    5920:	e593f000 	ldr	pc, [r3]
    5924:	e3500000 	cmp	r0, #0	; 0x0
    5928:	a3a07000 	movge	r7, #0	; 0x0
    592c:	a3a06020 	movge	r6, #32	; 0x20
    5930:	a59fb2a8 	ldrge	fp, [pc, #680]	; 5be0 <do_tt_wire+0x36c>
    5934:	aa000004 	bge	594c <do_tt_wire+0xd8>
    5938:	e59f02a4 	ldr	r0, [pc, #676]	; 5be4 <do_tt_wire+0x370>
    593c:	e59f328c 	ldr	r3, [pc, #652]	; 5bd0 <do_tt_wire+0x35c>
    5940:	e1a0e00f 	mov	lr, pc
    5944:	e593f000 	ldr	pc, [r3]
    5948:	eaffffeb 	b	58fc <do_tt_wire+0x88>
    594c:	e3570000 	cmp	r7, #0	; 0x0
    5950:	e1a01006 	mov	r1, r6
    5954:	e59f028c 	ldr	r0, [pc, #652]	; 5be8 <do_tt_wire+0x374>
    5958:	1a00002f 	bne	5a1c <do_tt_wire+0x1a8>
    595c:	e59f0288 	ldr	r0, [pc, #648]	; 5bec <do_tt_wire+0x378>
    5960:	e1a0e00f 	mov	lr, pc
    5964:	e598f000 	ldr	pc, [r8]
    5968:	e1a00007 	mov	r0, r7
    596c:	eb00236b 	bl	e720 <get_timer>
    5970:	e1a01006 	mov	r1, r6
    5974:	e1a04000 	mov	r4, r0
    5978:	e5993000 	ldr	r3, [r9]
    597c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5980:	e1a0e00f 	mov	lr, pc
    5984:	e593f004 	ldr	pc, [r3, #4]
    5988:	e1a05000 	mov	r5, r0
    598c:	e1a00004 	mov	r0, r4
    5990:	eb002362 	bl	e720 <get_timer>
    5994:	e3550000 	cmp	r5, #0	; 0x0
    5998:	e1a04000 	mov	r4, r0
    599c:	e1a01006 	mov	r1, r6
    59a0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    59a4:	ba00003d 	blt	5aa0 <do_tt_wire+0x22c>
    59a8:	ebffff88 	bl	57d0 <tt_wire_check_valid>
    59ac:	e3500000 	cmp	r0, #0	; 0x0
    59b0:	e1a01004 	mov	r1, r4
    59b4:	e1a03546 	asr	r3, r6, #10
    59b8:	1a000007 	bne	59dc <do_tt_wire+0x168>
    59bc:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    59c0:	e0000390 	mul	r0, r0, r3
    59c4:	eb003d70 	bl	14f8c <__aeabi_idiv>
    59c8:	e1a01000 	mov	r1, r0
    59cc:	e59f021c 	ldr	r0, [pc, #540]	; 5bf0 <do_tt_wire+0x37c>
    59d0:	e1a0e00f 	mov	lr, pc
    59d4:	e59af000 	ldr	pc, [sl]
    59d8:	eaffffc9 	b	5904 <do_tt_wire+0x90>
    59dc:	e59fc210 	ldr	ip, [pc, #528]	; 5bf4 <do_tt_wire+0x380>
    59e0:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    59e4:	e0000392 	mul	r0, r2, r3
    59e8:	e89c000c 	ldm	ip, {r2, r3}
    59ec:	e0922006 	adds	r2, r2, r6
    59f0:	e0a33fc6 	adc	r3, r3, r6, asr #31
    59f4:	e88c000c 	stm	ip, {r2, r3}
    59f8:	eb003d63 	bl	14f8c <__aeabi_idiv>
    59fc:	e1a01000 	mov	r1, r0
    5a00:	e59f01f0 	ldr	r0, [pc, #496]	; 5bf8 <do_tt_wire+0x384>
    5a04:	e1a0e00f 	mov	lr, pc
    5a08:	e598f000 	ldr	pc, [r8]
    5a0c:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    5a10:	e5937008 	ldr	r7, [r3, #8]
    5a14:	e593600c 	ldr	r6, [r3, #12]
    5a18:	ea000044 	b	5b30 <do_tt_wire+0x2bc>
    5a1c:	e3570001 	cmp	r7, #1	; 0x1
    5a20:	1a000030 	bne	5ae8 <do_tt_wire+0x274>
    5a24:	e1a0e00f 	mov	lr, pc
    5a28:	e598f000 	ldr	pc, [r8]
    5a2c:	e1a01007 	mov	r1, r7
    5a30:	e3a00000 	mov	r0, #0	; 0x0
    5a34:	ebfffaf8 	bl	461c <tt_rnd_get_int>
    5a38:	e3a01020 	mov	r1, #32	; 0x20
    5a3c:	e1a07000 	mov	r7, r0
    5a40:	e3a02502 	mov	r2, #8388608	; 0x800000
    5a44:	e28d0014 	add	r0, sp, #20	; 0x14
    5a48:	ebfffb1c 	bl	46c0 <tt_rnd_get_data>
    5a4c:	e59dc014 	ldr	ip, [sp, #20]
    5a50:	e1a01000 	mov	r1, r0
    5a54:	e1a02006 	mov	r2, r6
    5a58:	e1a03007 	mov	r3, r7
    5a5c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5a60:	e58dc000 	str	ip, [sp]
    5a64:	ebffff40 	bl	576c <tt_wire_make_data>
    5a68:	e3a00000 	mov	r0, #0	; 0x0
    5a6c:	eb00232b 	bl	e720 <get_timer>
    5a70:	e1a01006 	mov	r1, r6
    5a74:	e1a05000 	mov	r5, r0
    5a78:	e5993000 	ldr	r3, [r9]
    5a7c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5a80:	e1a0e00f 	mov	lr, pc
    5a84:	e593f008 	ldr	pc, [r3, #8]
    5a88:	e1a04000 	mov	r4, r0
    5a8c:	e1a00005 	mov	r0, r5
    5a90:	eb002322 	bl	e720 <get_timer>
    5a94:	e3540000 	cmp	r4, #0	; 0x0
    5a98:	e1a01000 	mov	r1, r0
    5a9c:	aa000003 	bge	5ab0 <do_tt_wire+0x23c>
    5aa0:	e59f0154 	ldr	r0, [pc, #340]	; 5bfc <do_tt_wire+0x388>
    5aa4:	e1a0e00f 	mov	lr, pc
    5aa8:	e59af000 	ldr	pc, [sl]
    5aac:	eaffff94 	b	5904 <do_tt_wire+0x90>
    5ab0:	e89b000c 	ldm	fp, {r2, r3}
    5ab4:	e0922006 	adds	r2, r2, r6
    5ab8:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5abc:	e59d6014 	ldr	r6, [sp, #20]
    5ac0:	e88b000c 	stm	fp, {r2, r3}
    5ac4:	e1a00546 	asr	r0, r6, #10
    5ac8:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5acc:	e0000092 	mul	r0, r2, r0
    5ad0:	eb003d2d 	bl	14f8c <__aeabi_idiv>
    5ad4:	e1a01000 	mov	r1, r0
    5ad8:	e59f0120 	ldr	r0, [pc, #288]	; 5c00 <do_tt_wire+0x38c>
    5adc:	e1a0e00f 	mov	lr, pc
    5ae0:	e598f000 	ldr	pc, [r8]
    5ae4:	ea000011 	b	5b30 <do_tt_wire+0x2bc>
    5ae8:	e3570003 	cmp	r7, #3	; 0x3
    5aec:	1a000004 	bne	5b04 <do_tt_wire+0x290>
    5af0:	e59f010c 	ldr	r0, [pc, #268]	; 5c04 <do_tt_wire+0x390>
    5af4:	e59f30d4 	ldr	r3, [pc, #212]	; 5bd0 <do_tt_wire+0x35c>
    5af8:	e1a0e00f 	mov	lr, pc
    5afc:	e593f000 	ldr	pc, [r3]
    5b00:	ea000010 	b	5b48 <do_tt_wire+0x2d4>
    5b04:	e3570002 	cmp	r7, #2	; 0x2
    5b08:	1a000003 	bne	5b1c <do_tt_wire+0x2a8>
    5b0c:	e59f00f4 	ldr	r0, [pc, #244]	; 5c08 <do_tt_wire+0x394>
    5b10:	e1a0e00f 	mov	lr, pc
    5b14:	e598f000 	ldr	pc, [r8]
    5b18:	eaffff79 	b	5904 <do_tt_wire+0x90>
    5b1c:	e59f00e8 	ldr	r0, [pc, #232]	; 5c0c <do_tt_wire+0x398>
    5b20:	e59f30a8 	ldr	r3, [pc, #168]	; 5bd0 <do_tt_wire+0x35c>
    5b24:	e1a0e00f 	mov	lr, pc
    5b28:	e593f000 	ldr	pc, [r3]
    5b2c:	eafffffe 	b	5b2c <do_tt_wire+0x2b8>
    5b30:	eb0034a6 	bl	12dd0 <tstc>
    5b34:	e3500000 	cmp	r0, #0	; 0x0
    5b38:	0affff83 	beq	594c <do_tt_wire+0xd8>
    5b3c:	eb0034a4 	bl	12dd4 <getc>
    5b40:	e3500003 	cmp	r0, #3	; 0x3
    5b44:	1affff80 	bne	594c <do_tt_wire+0xd8>
    5b48:	e59d000c 	ldr	r0, [sp, #12]
    5b4c:	eb0022f3 	bl	e720 <get_timer>
    5b50:	e59f3088 	ldr	r3, [pc, #136]	; 5be0 <do_tt_wire+0x36c>
    5b54:	e59f4074 	ldr	r4, [pc, #116]	; 5bd0 <do_tt_wire+0x35c>
    5b58:	e893000a 	ldm	r3, {r1, r3}
    5b5c:	e1a01a21 	lsr	r1, r1, #20
    5b60:	e1811603 	orr	r1, r1, r3, lsl #12
    5b64:	e1a05000 	mov	r5, r0
    5b68:	e59f00a0 	ldr	r0, [pc, #160]	; 5c10 <do_tt_wire+0x39c>
    5b6c:	e1a0e00f 	mov	lr, pc
    5b70:	e594f000 	ldr	pc, [r4]
    5b74:	e59f3078 	ldr	r3, [pc, #120]	; 5bf4 <do_tt_wire+0x380>
    5b78:	e59f0094 	ldr	r0, [pc, #148]	; 5c14 <do_tt_wire+0x3a0>
    5b7c:	e893000a 	ldm	r3, {r1, r3}
    5b80:	e1a01a21 	lsr	r1, r1, #20
    5b84:	e1811603 	orr	r1, r1, r3, lsl #12
    5b88:	e1a0e00f 	mov	lr, pc
    5b8c:	e594f000 	ldr	pc, [r4]
    5b90:	e30e1a60 	movw	r1, #60000	; 0xea60
    5b94:	e1a00005 	mov	r0, r5
    5b98:	eb003cfb 	bl	14f8c <__aeabi_idiv>
    5b9c:	e1a01000 	mov	r1, r0
    5ba0:	e59f0070 	ldr	r0, [pc, #112]	; 5c18 <do_tt_wire+0x3a4>
    5ba4:	e1a0e00f 	mov	lr, pc
    5ba8:	e594f000 	ldr	pc, [r4]
    5bac:	e3a00000 	mov	r0, #0	; 0x0
    5bb0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    5bb4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5bb8:	00017fa1 	.word	0x00017fa1
    5bbc:	00017fea 	.word	0x00017fea
    5bc0:	00015bfc 	.word	0x00015bfc
    5bc4:	00017fee 	.word	0x00017fee
    5bc8:	00015c14 	.word	0x00015c14
    5bcc:	3c00a080 	.word	0x3c00a080
    5bd0:	3c00a5cc 	.word	0x3c00a5cc
    5bd4:	00017ff2 	.word	0x00017ff2
    5bd8:	001e8480 	.word	0x001e8480
    5bdc:	00018012 	.word	0x00018012
    5be0:	3c00a078 	.word	0x3c00a078
    5be4:	00018034 	.word	0x00018034
    5be8:	000180a1 	.word	0x000180a1
    5bec:	00018057 	.word	0x00018057
    5bf0:	00018083 	.word	0x00018083
    5bf4:	3c00a070 	.word	0x3c00a070
    5bf8:	00018092 	.word	0x00018092
    5bfc:	00018073 	.word	0x00018073
    5c00:	000180bd 	.word	0x000180bd
    5c04:	000180cc 	.word	0x000180cc
    5c08:	000180de 	.word	0x000180de
    5c0c:	000180f7 	.word	0x000180f7
    5c10:	00018124 	.word	0x00018124
    5c14:	0001813e 	.word	0x0001813e
    5c18:	0001815c 	.word	0x0001815c

00005c1c <vs_is_assigned>:
    5c1c:	e59f300c 	ldr	r3, [pc, #12]	; 5c30 <vs_is_assigned+0x14>
    5c20:	e5930000 	ldr	r0, [r3]
    5c24:	e2500000 	subs	r0, r0, #0	; 0x0
    5c28:	13a00001 	movne	r0, #1	; 0x1
    5c2c:	e12fff1e 	bx	lr
    5c30:	3c00a088 	.word	0x3c00a088

00005c34 <vs_is_opt_available>:
    5c34:	e350000d 	cmp	r0, #13	; 0xd
    5c38:	959f2078 	ldrls	r2, [pc, #120]	; 5cb8 <vs_is_opt_available+0x84>
    5c3c:	93a0302c 	movls	r3, #44	; 0x2c
    5c40:	859f3074 	ldrhi	r3, [pc, #116]	; 5cbc <vs_is_opt_available+0x88>
    5c44:	90202093 	mlals	r0, r3, r0, r2
    5c48:	85930000 	ldrhi	r0, [r3]
    5c4c:	e3500000 	cmp	r0, #0	; 0x0
    5c50:	0a000016 	beq	5cb0 <vs_is_opt_available+0x7c>
    5c54:	e3510005 	cmp	r1, #5	; 0x5
    5c58:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    5c5c:	ea000013 	b	5cb0 <vs_is_opt_available+0x7c>
    5c60:	00005c78 	.word	0x00005c78
    5c64:	00005c80 	.word	0x00005c80
    5c68:	00005c88 	.word	0x00005c88
    5c6c:	00005c90 	.word	0x00005c90
    5c70:	00005c98 	.word	0x00005c98
    5c74:	00005ca0 	.word	0x00005ca0
    5c78:	e590300c 	ldr	r3, [r0, #12]
    5c7c:	ea000008 	b	5ca4 <vs_is_opt_available+0x70>
    5c80:	e5903010 	ldr	r3, [r0, #16]
    5c84:	ea000006 	b	5ca4 <vs_is_opt_available+0x70>
    5c88:	e5903014 	ldr	r3, [r0, #20]
    5c8c:	ea000004 	b	5ca4 <vs_is_opt_available+0x70>
    5c90:	e5903018 	ldr	r3, [r0, #24]
    5c94:	ea000002 	b	5ca4 <vs_is_opt_available+0x70>
    5c98:	e590301c 	ldr	r3, [r0, #28]
    5c9c:	ea000000 	b	5ca4 <vs_is_opt_available+0x70>
    5ca0:	e5903020 	ldr	r3, [r0, #32]
    5ca4:	e2530000 	subs	r0, r3, #0	; 0x0
    5ca8:	13a00001 	movne	r0, #1	; 0x1
    5cac:	e12fff1e 	bx	lr
    5cb0:	e3a00000 	mov	r0, #0	; 0x0
    5cb4:	e12fff1e 	bx	lr
    5cb8:	00015c14 	.word	0x00015c14
    5cbc:	3c00a088 	.word	0x3c00a088

00005cc0 <vs_write>:
    5cc0:	e92d4010 	push	{r4, lr}
    5cc4:	e24dd008 	sub	sp, sp, #8	; 0x8
    5cc8:	e59d4010 	ldr	r4, [sp, #16]
    5ccc:	e3540000 	cmp	r4, #0	; 0x0
    5cd0:	01a00004 	moveq	r0, r4
    5cd4:	0a00000c 	beq	5d0c <vs_write+0x4c>
    5cd8:	e59f1034 	ldr	r1, [pc, #52]	; 5d14 <vs_write+0x54>
    5cdc:	e5911000 	ldr	r1, [r1]
    5ce0:	e3510000 	cmp	r1, #0	; 0x0
    5ce4:	03e00001 	mvneq	r0, #1	; 0x1
    5ce8:	0a000007 	beq	5d0c <vs_write+0x4c>
    5cec:	e591c014 	ldr	ip, [r1, #20]
    5cf0:	e35c0000 	cmp	ip, #0	; 0x0
    5cf4:	03e00002 	mvneq	r0, #2	; 0x2
    5cf8:	0a000003 	beq	5d0c <vs_write+0x4c>
    5cfc:	e59d1014 	ldr	r1, [sp, #20]
    5d00:	e58d4000 	str	r4, [sp]
    5d04:	e58d1004 	str	r1, [sp, #4]
    5d08:	e12fff3c 	blx	ip
    5d0c:	e28dd008 	add	sp, sp, #8	; 0x8
    5d10:	e8bd8010 	pop	{r4, pc}
    5d14:	3c00a088 	.word	0x3c00a088

00005d18 <vs_erase>:
    5d18:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5d1c:	e59f302c 	ldr	r3, [pc, #44]	; 5d50 <vs_erase+0x38>
    5d20:	e24dd004 	sub	sp, sp, #4	; 0x4
    5d24:	e5933000 	ldr	r3, [r3]
    5d28:	e3530000 	cmp	r3, #0	; 0x0
    5d2c:	03e00001 	mvneq	r0, #1	; 0x1
    5d30:	0a000004 	beq	5d48 <vs_erase+0x30>
    5d34:	e593301c 	ldr	r3, [r3, #28]
    5d38:	e3530000 	cmp	r3, #0	; 0x0
    5d3c:	03e00002 	mvneq	r0, #2	; 0x2
    5d40:	0a000000 	beq	5d48 <vs_erase+0x30>
    5d44:	e12fff33 	blx	r3
    5d48:	e28dd004 	add	sp, sp, #4	; 0x4
    5d4c:	e8bd8000 	pop	{pc}
    5d50:	3c00a088 	.word	0x3c00a088

00005d54 <vs_isbad>:
    5d54:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5d58:	e59f302c 	ldr	r3, [pc, #44]	; 5d8c <vs_isbad+0x38>
    5d5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5d60:	e5933000 	ldr	r3, [r3]
    5d64:	e3530000 	cmp	r3, #0	; 0x0
    5d68:	03e00001 	mvneq	r0, #1	; 0x1
    5d6c:	0a000004 	beq	5d84 <vs_isbad+0x30>
    5d70:	e5933024 	ldr	r3, [r3, #36]
    5d74:	e3530000 	cmp	r3, #0	; 0x0
    5d78:	03e00002 	mvneq	r0, #2	; 0x2
    5d7c:	0a000000 	beq	5d84 <vs_isbad+0x30>
    5d80:	e12fff33 	blx	r3
    5d84:	e28dd004 	add	sp, sp, #4	; 0x4
    5d88:	e8bd8000 	pop	{pc}
    5d8c:	3c00a088 	.word	0x3c00a088

00005d90 <vs_scrub>:
    5d90:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5d94:	e59f302c 	ldr	r3, [pc, #44]	; 5dc8 <vs_scrub+0x38>
    5d98:	e24dd004 	sub	sp, sp, #4	; 0x4
    5d9c:	e5933000 	ldr	r3, [r3]
    5da0:	e3530000 	cmp	r3, #0	; 0x0
    5da4:	03e00001 	mvneq	r0, #1	; 0x1
    5da8:	0a000004 	beq	5dc0 <vs_scrub+0x30>
    5dac:	e5933020 	ldr	r3, [r3, #32]
    5db0:	e3530000 	cmp	r3, #0	; 0x0
    5db4:	03e00002 	mvneq	r0, #2	; 0x2
    5db8:	0a000000 	beq	5dc0 <vs_scrub+0x30>
    5dbc:	e12fff33 	blx	r3
    5dc0:	e28dd004 	add	sp, sp, #4	; 0x4
    5dc4:	e8bd8000 	pop	{pc}
    5dc8:	3c00a088 	.word	0x3c00a088

00005dcc <vs_special>:
    5dcc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5dd0:	e59f302c 	ldr	r3, [pc, #44]	; 5e04 <vs_special+0x38>
    5dd4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5dd8:	e5933000 	ldr	r3, [r3]
    5ddc:	e3530000 	cmp	r3, #0	; 0x0
    5de0:	03e00001 	mvneq	r0, #1	; 0x1
    5de4:	0a000004 	beq	5dfc <vs_special+0x30>
    5de8:	e5933028 	ldr	r3, [r3, #40]
    5dec:	e3530000 	cmp	r3, #0	; 0x0
    5df0:	03e00002 	mvneq	r0, #2	; 0x2
    5df4:	0a000000 	beq	5dfc <vs_special+0x30>
    5df8:	e12fff33 	blx	r3
    5dfc:	e28dd004 	add	sp, sp, #4	; 0x4
    5e00:	e8bd8000 	pop	{pc}
    5e04:	3c00a088 	.word	0x3c00a088

00005e08 <vs_align>:
    5e08:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5e0c:	e350000d 	cmp	r0, #13	; 0xd
    5e10:	e24dd004 	sub	sp, sp, #4	; 0x4
    5e14:	8a00000f 	bhi	5e58 <vs_align+0x50>
    5e18:	e59f2044 	ldr	r2, [pc, #68]	; 5e64 <vs_align+0x5c>
    5e1c:	e3a0302c 	mov	r3, #44	; 0x2c
    5e20:	e0202093 	mla	r0, r3, r0, r2
    5e24:	ea000001 	b	5e30 <vs_align+0x28>
    5e28:	e59f3038 	ldr	r3, [pc, #56]	; 5e68 <vs_align+0x60>
    5e2c:	e5930000 	ldr	r0, [r3]
    5e30:	e3500000 	cmp	r0, #0	; 0x0
    5e34:	0a000004 	beq	5e4c <vs_align+0x44>
    5e38:	e590000c 	ldr	r0, [r0, #12]
    5e3c:	e3500000 	cmp	r0, #0	; 0x0
    5e40:	0a000001 	beq	5e4c <vs_align+0x44>
    5e44:	e12fff30 	blx	r0
    5e48:	ea000000 	b	5e50 <vs_align+0x48>
    5e4c:	e3a00000 	mov	r0, #0	; 0x0
    5e50:	e28dd004 	add	sp, sp, #4	; 0x4
    5e54:	e8bd8000 	pop	{pc}
    5e58:	e350000e 	cmp	r0, #14	; 0xe
    5e5c:	1afffffa 	bne	5e4c <vs_align+0x44>
    5e60:	eafffff0 	b	5e28 <vs_align+0x20>
    5e64:	00015c14 	.word	0x00015c14
    5e68:	3c00a088 	.word	0x3c00a088

00005e6c <vs_device_bootable>:
    5e6c:	e350000d 	cmp	r0, #13	; 0xd
    5e70:	959f2014 	ldrls	r2, [pc, #20]	; 5e8c <vs_device_bootable+0x20>
    5e74:	93a0302c 	movls	r3, #44	; 0x2c
    5e78:	90232093 	mlals	r3, r3, r0, r2
    5e7c:	95933008 	ldrls	r3, [r3, #8]
    5e80:	83a00000 	movhi	r0, #0	; 0x0
    5e84:	92030001 	andls	r0, r3, #1	; 0x1
    5e88:	e12fff1e 	bx	lr
    5e8c:	00015c14 	.word	0x00015c14

00005e90 <vs_device_burnsrc>:
    5e90:	e350000d 	cmp	r0, #13	; 0xd
    5e94:	959f2018 	ldrls	r2, [pc, #24]	; 5eb4 <vs_device_burnsrc+0x24>
    5e98:	93a0302c 	movls	r3, #44	; 0x2c
    5e9c:	90232093 	mlals	r3, r3, r0, r2
    5ea0:	95933008 	ldrls	r3, [r3, #8]
    5ea4:	83a00000 	movhi	r0, #0	; 0x0
    5ea8:	91a030a3 	lsrls	r3, r3, #1
    5eac:	92030001 	andls	r0, r3, #1	; 0x1
    5eb0:	e12fff1e 	bx	lr
    5eb4:	00015c14 	.word	0x00015c14

00005eb8 <vs_device_burndst>:
    5eb8:	e350000d 	cmp	r0, #13	; 0xd
    5ebc:	959f2018 	ldrls	r2, [pc, #24]	; 5edc <vs_device_burndst+0x24>
    5ec0:	93a0302c 	movls	r3, #44	; 0x2c
    5ec4:	90232093 	mlals	r3, r3, r0, r2
    5ec8:	95933008 	ldrls	r3, [r3, #8]
    5ecc:	83a00000 	movhi	r0, #0	; 0x0
    5ed0:	91a03123 	lsrls	r3, r3, #2
    5ed4:	92030001 	andls	r0, r3, #1	; 0x1
    5ed8:	e12fff1e 	bx	lr
    5edc:	00015c14 	.word	0x00015c14

00005ee0 <vs_device_boot0>:
    5ee0:	e350000d 	cmp	r0, #13	; 0xd
    5ee4:	959f2018 	ldrls	r2, [pc, #24]	; 5f04 <vs_device_boot0+0x24>
    5ee8:	93a0302c 	movls	r3, #44	; 0x2c
    5eec:	90232093 	mlals	r3, r3, r0, r2
    5ef0:	95933008 	ldrls	r3, [r3, #8]
    5ef4:	83a00000 	movhi	r0, #0	; 0x0
    5ef8:	91a031a3 	lsrls	r3, r3, #3
    5efc:	92030001 	andls	r0, r3, #1	; 0x1
    5f00:	e12fff1e 	bx	lr
    5f04:	00015c14 	.word	0x00015c14

00005f08 <vs_device_name>:
    5f08:	e350000d 	cmp	r0, #13	; 0xd
    5f0c:	959f200c 	ldrls	r2, [pc, #12]	; 5f20 <vs_device_name+0x18>
    5f10:	93a0302c 	movls	r3, #44	; 0x2c
    5f14:	83a00000 	movhi	r0, #0	; 0x0
    5f18:	90202093 	mlals	r0, r3, r0, r2
    5f1c:	e12fff1e 	bx	lr
    5f20:	00015c14 	.word	0x00015c14

00005f24 <vs_device_erasable>:
    5f24:	e350000d 	cmp	r0, #13	; 0xd
    5f28:	959f2018 	ldrls	r2, [pc, #24]	; 5f48 <vs_device_erasable+0x24>
    5f2c:	93a0302c 	movls	r3, #44	; 0x2c
    5f30:	90232093 	mlals	r3, r3, r0, r2
    5f34:	95933008 	ldrls	r3, [r3, #8]
    5f38:	83a00000 	movhi	r0, #0	; 0x0
    5f3c:	91a03223 	lsrls	r3, r3, #4
    5f40:	92030001 	andls	r0, r3, #1	; 0x1
    5f44:	e12fff1e 	bx	lr
    5f48:	00015c14 	.word	0x00015c14

00005f4c <vs_device_count>:
    5f4c:	e3a0000e 	mov	r0, #14	; 0xe
    5f50:	e12fff1e 	bx	lr

00005f54 <vs_device_id>:
    5f54:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5f58:	e59f7040 	ldr	r7, [pc, #64]	; 5fa0 <vs_device_id+0x4c>
    5f5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f60:	e1a05000 	mov	r5, r0
    5f64:	e3a04000 	mov	r4, #0	; 0x0
    5f68:	e3a0602c 	mov	r6, #44	; 0x2c
    5f6c:	e0217496 	mla	r1, r6, r4, r7
    5f70:	e1a00005 	mov	r0, r5
    5f74:	e3a02010 	mov	r2, #16	; 0x10
    5f78:	ebffed00 	bl	1380 <strncmp>
    5f7c:	e3500000 	cmp	r0, #0	; 0x0
    5f80:	0a000003 	beq	5f94 <vs_device_id+0x40>
    5f84:	e2844001 	add	r4, r4, #1	; 0x1
    5f88:	e354000e 	cmp	r4, #14	; 0xe
    5f8c:	1afffff6 	bne	5f6c <vs_device_id+0x18>
    5f90:	e2844001 	add	r4, r4, #1	; 0x1
    5f94:	e1a00004 	mov	r0, r4
    5f98:	e28dd004 	add	sp, sp, #4	; 0x4
    5f9c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5fa0:	00015c14 	.word	0x00015c14

00005fa4 <vs_is_device>:
    5fa4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5fa8:	e350000e 	cmp	r0, #14	; 0xe
    5fac:	059f304c 	ldreq	r3, [pc, #76]	; 6000 <vs_is_device+0x5c>
    5fb0:	e1a0c001 	mov	ip, r1
    5fb4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5fb8:	05931000 	ldreq	r1, [r3]
    5fbc:	0a000004 	beq	5fd4 <vs_is_device+0x30>
    5fc0:	e350000d 	cmp	r0, #13	; 0xd
    5fc4:	8a00000a 	bhi	5ff4 <vs_is_device+0x50>
    5fc8:	e59f2034 	ldr	r2, [pc, #52]	; 6004 <vs_is_device+0x60>
    5fcc:	e3a0302c 	mov	r3, #44	; 0x2c
    5fd0:	e0212093 	mla	r1, r3, r0, r2
    5fd4:	e3510000 	cmp	r1, #0	; 0x0
    5fd8:	0a000005 	beq	5ff4 <vs_is_device+0x50>
    5fdc:	e1a0000c 	mov	r0, ip
    5fe0:	e3a02010 	mov	r2, #16	; 0x10
    5fe4:	ebffece5 	bl	1380 <strncmp>
    5fe8:	e2700001 	rsbs	r0, r0, #1	; 0x1
    5fec:	33a00000 	movcc	r0, #0	; 0x0
    5ff0:	ea000000 	b	5ff8 <vs_is_device+0x54>
    5ff4:	e3a00000 	mov	r0, #0	; 0x0
    5ff8:	e28dd004 	add	sp, sp, #4	; 0x4
    5ffc:	e8bd8000 	pop	{pc}
    6000:	3c00a088 	.word	0x3c00a088
    6004:	00015c14 	.word	0x00015c14

00006008 <vs_read>:
    6008:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    600c:	e59f4234 	ldr	r4, [pc, #564]	; 6248 <vs_read+0x240>
    6010:	e1a09000 	mov	r9, r0
    6014:	e5940000 	ldr	r0, [r4]
    6018:	e24dd024 	sub	sp, sp, #36	; 0x24
    601c:	e3500000 	cmp	r0, #0	; 0x0
    6020:	e1a06002 	mov	r6, r2
    6024:	e1a07003 	mov	r7, r3
    6028:	e59d5048 	ldr	r5, [sp, #72]
    602c:	03e04001 	mvneq	r4, #1	; 0x1
    6030:	0a000081 	beq	623c <vs_read+0x234>
    6034:	e5903010 	ldr	r3, [r0, #16]
    6038:	e3530000 	cmp	r3, #0	; 0x0
    603c:	03e04002 	mvneq	r4, #2	; 0x2
    6040:	0a00007d 	beq	623c <vs_read+0x234>
    6044:	e3550000 	cmp	r5, #0	; 0x0
    6048:	01a04005 	moveq	r4, r5
    604c:	0a00007a 	beq	623c <vs_read+0x234>
    6050:	e59f11f4 	ldr	r1, [pc, #500]	; 624c <vs_read+0x244>
    6054:	e3a02003 	mov	r2, #3	; 0x3
    6058:	ebffecc8 	bl	1380 <strncmp>
    605c:	e3500000 	cmp	r0, #0	; 0x0
    6060:	1a00000a 	bne	6090 <vs_read+0x88>
    6064:	e59d204c 	ldr	r2, [sp, #76]
    6068:	e58d5000 	str	r5, [sp]
    606c:	e58d2004 	str	r2, [sp, #4]
    6070:	e5941000 	ldr	r1, [r4]
    6074:	e1a00009 	mov	r0, r9
    6078:	e1a02006 	mov	r2, r6
    607c:	e1a03007 	mov	r3, r7
    6080:	e1a0e00f 	mov	lr, pc
    6084:	e591f010 	ldr	pc, [r1, #16]
    6088:	e1a04000 	mov	r4, r0
    608c:	ea00006a 	b	623c <vs_read+0x234>
    6090:	e3a0000e 	mov	r0, #14	; 0xe
    6094:	ebffff5b 	bl	5e08 <vs_align>
    6098:	e2402001 	sub	r2, r0, #1	; 0x1
    609c:	e0124006 	ands	r4, r2, r6
    60a0:	10644000 	rsbne	r4, r4, r0
    60a4:	e0643005 	rsb	r3, r4, r5
    60a8:	e0032002 	and	r2, r3, r2
    60ac:	e192a004 	orrs	sl, r2, r4
    60b0:	e1a08000 	mov	r8, r0
    60b4:	e58d3014 	str	r3, [sp, #20]
    60b8:	e58d200c 	str	r2, [sp, #12]
    60bc:	e58da018 	str	sl, [sp, #24]
    60c0:	01a0400a 	moveq	r4, sl
    60c4:	058da01c 	streq	sl, [sp, #28]
    60c8:	0a00002c 	beq	6180 <vs_read+0x178>
    60cc:	ebffed49 	bl	15f8 <malloc>
    60d0:	e250b000 	subs	fp, r0, #0	; 0x0
    60d4:	e58db01c 	str	fp, [sp, #28]
    60d8:	1a00000c 	bne	6110 <vs_read+0x108>
    60dc:	e59f416c 	ldr	r4, [pc, #364]	; 6250 <vs_read+0x248>
    60e0:	e59f016c 	ldr	r0, [pc, #364]	; 6254 <vs_read+0x24c>
    60e4:	e3a01f4a 	mov	r1, #296	; 0x128
    60e8:	e1a0e00f 	mov	lr, pc
    60ec:	e594f000 	ldr	pc, [r4]
    60f0:	e59f0160 	ldr	r0, [pc, #352]	; 6258 <vs_read+0x250>
    60f4:	e59f1160 	ldr	r1, [pc, #352]	; 625c <vs_read+0x254>
    60f8:	e1a0e00f 	mov	lr, pc
    60fc:	e594f000 	ldr	pc, [r4]
    6100:	e59f0158 	ldr	r0, [pc, #344]	; 6260 <vs_read+0x258>
    6104:	e1a0e00f 	mov	lr, pc
    6108:	e594f000 	ldr	pc, [r4]
    610c:	eafffffe 	b	610c <vs_read+0x104>
    6110:	e3540000 	cmp	r4, #0	; 0x0
    6114:	0a000019 	beq	6180 <vs_read+0x178>
    6118:	e59f2128 	ldr	r2, [pc, #296]	; 6248 <vs_read+0x240>
    611c:	e2683000 	rsb	r3, r8, #0	; 0x0
    6120:	e5921000 	ldr	r1, [r2]
    6124:	e59d204c 	ldr	r2, [sp, #76]
    6128:	e006a003 	and	sl, r6, r3
    612c:	e3a0b000 	mov	fp, #0	; 0x0
    6130:	e58d2004 	str	r2, [sp, #4]
    6134:	e1a0300b 	mov	r3, fp
    6138:	e58d8000 	str	r8, [sp]
    613c:	e1a0200a 	mov	r2, sl
    6140:	e1a0e00f 	mov	lr, pc
    6144:	e591f010 	ldr	pc, [r1, #16]
    6148:	e250b000 	subs	fp, r0, #0	; 0x0
    614c:	b1a0400b 	movlt	r4, fp
    6150:	ba000037 	blt	6234 <vs_read+0x22c>
    6154:	e59d201c 	ldr	r2, [sp, #28]
    6158:	e0641008 	rsb	r1, r4, r8
    615c:	e0821001 	add	r1, r2, r1
    6160:	e1a00009 	mov	r0, r9
    6164:	e1a02004 	mov	r2, r4
    6168:	ebffecbd 	bl	1464 <memcpy>
    616c:	e0966004 	adds	r6, r6, r4
    6170:	e59d5014 	ldr	r5, [sp, #20]
    6174:	e0899004 	add	r9, r9, r4
    6178:	e2a77000 	adc	r7, r7, #0	; 0x0
    617c:	e1a0400b 	mov	r4, fp
    6180:	e59d300c 	ldr	r3, [sp, #12]
    6184:	e0555003 	subs	r5, r5, r3
    6188:	0a00000f 	beq	61cc <vs_read+0x1c4>
    618c:	e59f30b4 	ldr	r3, [pc, #180]	; 6248 <vs_read+0x240>
    6190:	e59da04c 	ldr	sl, [sp, #76]
    6194:	e58d5000 	str	r5, [sp]
    6198:	e5931000 	ldr	r1, [r3]
    619c:	e1a00009 	mov	r0, r9
    61a0:	e58da004 	str	sl, [sp, #4]
    61a4:	e1a02006 	mov	r2, r6
    61a8:	e1a03007 	mov	r3, r7
    61ac:	e1a0e00f 	mov	lr, pc
    61b0:	e591f010 	ldr	pc, [r1, #16]
    61b4:	e3500000 	cmp	r0, #0	; 0x0
    61b8:	ba000012 	blt	6208 <vs_read+0x200>
    61bc:	e0966005 	adds	r6, r6, r5
    61c0:	e0a77fc5 	adc	r7, r7, r5, asr #31
    61c4:	e0844000 	add	r4, r4, r0
    61c8:	e0899005 	add	r9, r9, r5
    61cc:	e59db00c 	ldr	fp, [sp, #12]
    61d0:	e35b0000 	cmp	fp, #0	; 0x0
    61d4:	0a000013 	beq	6228 <vs_read+0x220>
    61d8:	e59d204c 	ldr	r2, [sp, #76]
    61dc:	e59f3064 	ldr	r3, [pc, #100]	; 6248 <vs_read+0x240>
    61e0:	e58d2004 	str	r2, [sp, #4]
    61e4:	e58d8000 	str	r8, [sp]
    61e8:	e5931000 	ldr	r1, [r3]
    61ec:	e1a02006 	mov	r2, r6
    61f0:	e1a03007 	mov	r3, r7
    61f4:	e59d001c 	ldr	r0, [sp, #28]
    61f8:	e1a0e00f 	mov	lr, pc
    61fc:	e591f010 	ldr	pc, [r1, #16]
    6200:	e3500000 	cmp	r0, #0	; 0x0
    6204:	aa000001 	bge	6210 <vs_read+0x208>
    6208:	e1a04000 	mov	r4, r0
    620c:	ea000005 	b	6228 <vs_read+0x220>
    6210:	e1a00009 	mov	r0, r9
    6214:	e59d101c 	ldr	r1, [sp, #28]
    6218:	e59d200c 	ldr	r2, [sp, #12]
    621c:	ebffec90 	bl	1464 <memcpy>
    6220:	e59d300c 	ldr	r3, [sp, #12]
    6224:	e0844003 	add	r4, r4, r3
    6228:	e59da018 	ldr	sl, [sp, #24]
    622c:	e35a0000 	cmp	sl, #0	; 0x0
    6230:	0a000001 	beq	623c <vs_read+0x234>
    6234:	e59d001c 	ldr	r0, [sp, #28]
    6238:	ebffed56 	bl	1798 <free>
    623c:	e1a00004 	mov	r0, r4
    6240:	e28dd024 	add	sp, sp, #36	; 0x24
    6244:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6248:	3c00a088 	.word	0x3c00a088
    624c:	000178a3 	.word	0x000178a3
    6250:	3c00a5cc 	.word	0x3c00a5cc
    6254:	00018185 	.word	0x00018185
    6258:	00016a6e 	.word	0x00016a6e
    625c:	00018197 	.word	0x00018197
    6260:	000181a2 	.word	0x000181a2

00006264 <__vs_device_init_case>:
    6264:	e92d4010 	push	{r4, lr}
    6268:	e3a0000e 	mov	r0, #14	; 0xe
    626c:	e59f1084 	ldr	r1, [pc, #132]	; 62f8 <__vs_device_init_case+0x94>
    6270:	ebffff4b 	bl	5fa4 <vs_is_device>
    6274:	e3500000 	cmp	r0, #0	; 0x0
    6278:	1a000010 	bne	62c0 <__vs_device_init_case+0x5c>
    627c:	e280000e 	add	r0, r0, #14	; 0xe
    6280:	e59f1074 	ldr	r1, [pc, #116]	; 62fc <__vs_device_init_case+0x98>
    6284:	ebffff46 	bl	5fa4 <vs_is_device>
    6288:	e3500000 	cmp	r0, #0	; 0x0
    628c:	1a00000b 	bne	62c0 <__vs_device_init_case+0x5c>
    6290:	e280000e 	add	r0, r0, #14	; 0xe
    6294:	e59f1064 	ldr	r1, [pc, #100]	; 6300 <__vs_device_init_case+0x9c>
    6298:	ebffff41 	bl	5fa4 <vs_is_device>
    629c:	e3500000 	cmp	r0, #0	; 0x0
    62a0:	1a000006 	bne	62c0 <__vs_device_init_case+0x5c>
    62a4:	e280000e 	add	r0, r0, #14	; 0xe
    62a8:	e59f1054 	ldr	r1, [pc, #84]	; 6304 <__vs_device_init_case+0xa0>
    62ac:	ebffff3c 	bl	5fa4 <vs_is_device>
    62b0:	e3500000 	cmp	r0, #0	; 0x0
    62b4:	13a0400c 	movne	r4, #12	; 0xc
    62b8:	1a000001 	bne	62c4 <__vs_device_init_case+0x60>
    62bc:	e8bd8010 	pop	{r4, pc}
    62c0:	e3a0400b 	mov	r4, #11	; 0xb
    62c4:	e59f203c 	ldr	r2, [pc, #60]	; 6308 <__vs_device_init_case+0xa4>
    62c8:	e5923000 	ldr	r3, [r2]
    62cc:	e1540003 	cmp	r4, r3
    62d0:	08bd8010 	popeq	{r4, pc}
    62d4:	e1a00004 	mov	r0, r4
    62d8:	e5824000 	str	r4, [r2]
    62dc:	eb002db9 	bl	119c8 <set_mux>
    62e0:	e1a01004 	mov	r1, r4
    62e4:	e59f0020 	ldr	r0, [pc, #32]	; 630c <__vs_device_init_case+0xa8>
    62e8:	e59f3020 	ldr	r3, [pc, #32]	; 6310 <__vs_device_init_case+0xac>
    62ec:	e1a0e00f 	mov	lr, pc
    62f0:	e593f000 	ldr	pc, [r3]
    62f4:	e8bd8010 	pop	{r4, pc}
    62f8:	000166d1 	.word	0x000166d1
    62fc:	000178a7 	.word	0x000178a7
    6300:	000178a3 	.word	0x000178a3
    6304:	000181d3 	.word	0x000181d3
    6308:	3c00a000 	.word	0x3c00a000
    630c:	000181d8 	.word	0x000181d8
    6310:	3c00a5cc 	.word	0x3c00a5cc

00006314 <vs_reset>:
    6314:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6318:	e24dd004 	sub	sp, sp, #4	; 0x4
    631c:	ebffffd0 	bl	6264 <__vs_device_init_case>
    6320:	e59f3028 	ldr	r3, [pc, #40]	; 6350 <vs_reset+0x3c>
    6324:	e5933000 	ldr	r3, [r3]
    6328:	e3530000 	cmp	r3, #0	; 0x0
    632c:	03e00001 	mvneq	r0, #1	; 0x1
    6330:	0a000004 	beq	6348 <vs_reset+0x34>
    6334:	e5933018 	ldr	r3, [r3, #24]
    6338:	e3530000 	cmp	r3, #0	; 0x0
    633c:	03e00002 	mvneq	r0, #2	; 0x2
    6340:	0a000000 	beq	6348 <vs_reset+0x34>
    6344:	e12fff33 	blx	r3
    6348:	e28dd004 	add	sp, sp, #4	; 0x4
    634c:	e8bd8000 	pop	{pc}
    6350:	3c00a088 	.word	0x3c00a088

00006354 <vs_assign_by_id>:
    6354:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6358:	e59fc05c 	ldr	ip, [pc, #92]	; 63bc <vs_assign_by_id+0x68>
    635c:	e3a03000 	mov	r3, #0	; 0x0
    6360:	e350000d 	cmp	r0, #13	; 0xd
    6364:	e24dd004 	sub	sp, sp, #4	; 0x4
    6368:	e58c3000 	str	r3, [ip]
    636c:	8a00000b 	bhi	63a0 <vs_assign_by_id+0x4c>
    6370:	e59f2048 	ldr	r2, [pc, #72]	; 63c0 <vs_assign_by_id+0x6c>
    6374:	e283302c 	add	r3, r3, #44	; 0x2c
    6378:	e0232093 	mla	r3, r3, r0, r2
    637c:	e3510000 	cmp	r1, #0	; 0x0
    6380:	e58c3000 	str	r3, [ip]
    6384:	01a00001 	moveq	r0, r1
    6388:	0a000002 	beq	6398 <vs_assign_by_id+0x44>
    638c:	e28dd004 	add	sp, sp, #4	; 0x4
    6390:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    6394:	eaffffde 	b	6314 <vs_reset>
    6398:	e28dd004 	add	sp, sp, #4	; 0x4
    639c:	e8bd8000 	pop	{pc}
    63a0:	e1a01000 	mov	r1, r0
    63a4:	e59f3018 	ldr	r3, [pc, #24]	; 63c4 <vs_assign_by_id+0x70>
    63a8:	e59f0018 	ldr	r0, [pc, #24]	; 63c8 <vs_assign_by_id+0x74>
    63ac:	e1a0e00f 	mov	lr, pc
    63b0:	e593f000 	ldr	pc, [r3]
    63b4:	e3e00000 	mvn	r0, #0	; 0x0
    63b8:	eafffff6 	b	6398 <vs_assign_by_id+0x44>
    63bc:	3c00a088 	.word	0x3c00a088
    63c0:	00015c14 	.word	0x00015c14
    63c4:	3c00a5cc 	.word	0x3c00a5cc
    63c8:	000181ee 	.word	0x000181ee

000063cc <vs_assign_by_name>:
    63cc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    63d0:	e59f3084 	ldr	r3, [pc, #132]	; 645c <vs_assign_by_name+0x90>
    63d4:	e3a02000 	mov	r2, #0	; 0x0
    63d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    63dc:	e5832000 	str	r2, [r3]
    63e0:	e1a06000 	mov	r6, r0
    63e4:	e1a07001 	mov	r7, r1
    63e8:	e1a04002 	mov	r4, r2
    63ec:	e3a0a02c 	mov	sl, #44	; 0x2c
    63f0:	e59f8068 	ldr	r8, [pc, #104]	; 6460 <vs_assign_by_name+0x94>
    63f4:	ea00000d 	b	6430 <vs_assign_by_name+0x64>
    63f8:	e025849a 	mla	r5, sl, r4, r8
    63fc:	e1a00005 	mov	r0, r5
    6400:	ebffebde 	bl	1380 <strncmp>
    6404:	e3500000 	cmp	r0, #0	; 0x0
    6408:	1a000007 	bne	642c <vs_assign_by_name+0x60>
    640c:	e59f3048 	ldr	r3, [pc, #72]	; 645c <vs_assign_by_name+0x90>
    6410:	e3570000 	cmp	r7, #0	; 0x0
    6414:	e5835000 	str	r5, [r3]
    6418:	01a00007 	moveq	r0, r7
    641c:	0a00000c 	beq	6454 <vs_assign_by_name+0x88>
    6420:	e28dd004 	add	sp, sp, #4	; 0x4
    6424:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    6428:	eaffffb9 	b	6314 <vs_reset>
    642c:	e2844001 	add	r4, r4, #1	; 0x1
    6430:	e354000e 	cmp	r4, #14	; 0xe
    6434:	e1a01006 	mov	r1, r6
    6438:	e3a02010 	mov	r2, #16	; 0x10
    643c:	1affffed 	bne	63f8 <vs_assign_by_name+0x2c>
    6440:	e59f001c 	ldr	r0, [pc, #28]	; 6464 <vs_assign_by_name+0x98>
    6444:	e59f301c 	ldr	r3, [pc, #28]	; 6468 <vs_assign_by_name+0x9c>
    6448:	e1a0e00f 	mov	lr, pc
    644c:	e593f000 	ldr	pc, [r3]
    6450:	e3e00000 	mvn	r0, #0	; 0x0
    6454:	e28dd004 	add	sp, sp, #4	; 0x4
    6458:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    645c:	3c00a088 	.word	0x3c00a088
    6460:	00015c14 	.word	0x00015c14
    6464:	00018207 	.word	0x00018207
    6468:	3c00a5cc 	.word	0x3c00a5cc

0000646c <ss_lock_isk>:
    646c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6470:	e24dd004 	sub	sp, sp, #4	; 0x4
    6474:	eb0001d0 	bl	6bbc <security_enabled>
    6478:	e3500000 	cmp	r0, #0	; 0x0
    647c:	0a000002 	beq	648c <ss_lock_isk+0x20>
    6480:	e28dd004 	add	sp, sp, #4	; 0x4
    6484:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    6488:	ea00002e 	b	6548 <block_cipher_lock_isk>
    648c:	e28dd004 	add	sp, sp, #4	; 0x4
    6490:	e8bd8000 	pop	{pc}

00006494 <ss_aes_data>:
    6494:	e92d4070 	push	{r4, r5, r6, lr}
    6498:	e1a05000 	mov	r5, r0
    649c:	e1a06001 	mov	r6, r1
    64a0:	e1a04002 	mov	r4, r2
    64a4:	eb0001c4 	bl	6bbc <security_enabled>
    64a8:	e3500000 	cmp	r0, #0	; 0x0
    64ac:	0a000007 	beq	64d0 <ss_aes_data+0x3c>
    64b0:	e284200f 	add	r2, r4, #15	; 0xf
    64b4:	e1a00005 	mov	r0, r5
    64b8:	e1a01006 	mov	r1, r6
    64bc:	e3c2200f 	bic	r2, r2, #15	; 0xf
    64c0:	eb0000a0 	bl	6748 <block_cipher_operation>
    64c4:	e3500000 	cmp	r0, #0	; 0x0
    64c8:	13e00000 	mvnne	r0, #0	; 0x0
    64cc:	18bd8070 	popne	{r4, r5, r6, pc}
    64d0:	e3a00000 	mov	r0, #0	; 0x0
    64d4:	e8bd8070 	pop	{r4, r5, r6, pc}

000064d8 <ss_aes_init>:
    64d8:	e92d4070 	push	{r4, r5, r6, lr}
    64dc:	e1a05000 	mov	r5, r0
    64e0:	e1a04001 	mov	r4, r1
    64e4:	e1a06002 	mov	r6, r2
    64e8:	eb0001b3 	bl	6bbc <security_enabled>
    64ec:	e3500000 	cmp	r0, #0	; 0x0
    64f0:	0a00000b 	beq	6524 <ss_aes_init+0x4c>
    64f4:	e3540000 	cmp	r4, #0	; 0x0
    64f8:	01a00005 	moveq	r0, r5
    64fc:	01a02004 	moveq	r2, r4
    6500:	01a03006 	moveq	r3, r6
    6504:	03a01001 	moveq	r1, #1	; 0x1
    6508:	11a00005 	movne	r0, r5
    650c:	11a02004 	movne	r2, r4
    6510:	11a03006 	movne	r3, r6
    6514:	13a01000 	movne	r1, #0	; 0x0
    6518:	eb00002c 	bl	65d0 <block_cipher_aes_init>
    651c:	e3500000 	cmp	r0, #0	; 0x0
    6520:	1a000001 	bne	652c <ss_aes_init+0x54>
    6524:	e3a00000 	mov	r0, #0	; 0x0
    6528:	e8bd8070 	pop	{r4, r5, r6, pc}
    652c:	e3e00000 	mvn	r0, #0	; 0x0
    6530:	e8bd8070 	pop	{r4, r5, r6, pc}

00006534 <check_register_bit>:
    6534:	e3a03001 	mov	r3, #1	; 0x1
    6538:	e0103113 	ands	r3, r0, r3, lsl r1
    653c:	03a00000 	moveq	r0, #0	; 0x0
    6540:	13a00001 	movne	r0, #1	; 0x1
    6544:	e12fff1e 	bx	lr

00006548 <block_cipher_lock_isk>:
    6548:	e59f200c 	ldr	r2, [pc, #12]	; 655c <block_cipher_lock_isk+0x14>
    654c:	e5923400 	ldr	r3, [r2, #1024]
    6550:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    6554:	e5823400 	str	r3, [r2, #1024]
    6558:	e12fff1e 	bx	lr
    655c:	23200000 	.word	0x23200000

00006560 <block_cipher_reset>:
    6560:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6564:	e59f4060 	ldr	r4, [pc, #96]	; 65cc <block_cipher_reset+0x6c>
    6568:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    656c:	e5843400 	str	r3, [r4, #1024]
    6570:	e24dd004 	sub	sp, sp, #4	; 0x4
    6574:	eb002034 	bl	e64c <get_ticks>
    6578:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    657c:	e3a07000 	mov	r7, #0	; 0x0
    6580:	e0966000 	adds	r6, r6, r0
    6584:	e1a05004 	mov	r5, r4
    6588:	e0a77001 	adc	r7, r7, r1
    658c:	e5944400 	ldr	r4, [r4, #1024]
    6590:	ea000008 	b	65b8 <block_cipher_reset+0x58>
    6594:	e5954400 	ldr	r4, [r5, #1024]
    6598:	eb00202b 	bl	e64c <get_ticks>
    659c:	e1510007 	cmp	r1, r7
    65a0:	8a000002 	bhi	65b0 <block_cipher_reset+0x50>
    65a4:	1a000003 	bne	65b8 <block_cipher_reset+0x58>
    65a8:	e1500006 	cmp	r0, r6
    65ac:	9a000001 	bls	65b8 <block_cipher_reset+0x58>
    65b0:	e3a00001 	mov	r0, #1	; 0x1
    65b4:	ea000002 	b	65c4 <block_cipher_reset+0x64>
    65b8:	e3540000 	cmp	r4, #0	; 0x0
    65bc:	bafffff4 	blt	6594 <block_cipher_reset+0x34>
    65c0:	e3a00000 	mov	r0, #0	; 0x0
    65c4:	e28dd004 	add	sp, sp, #4	; 0x4
    65c8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    65cc:	23200000 	.word	0x23200000

000065d0 <block_cipher_aes_init>:
    65d0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    65d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    65d8:	e1a05000 	mov	r5, r0
    65dc:	e1a04001 	mov	r4, r1
    65e0:	e1a06002 	mov	r6, r2
    65e4:	e1a07003 	mov	r7, r3
    65e8:	ebffffdc 	bl	6560 <block_cipher_reset>
    65ec:	e3500000 	cmp	r0, #0	; 0x0
    65f0:	1a00003d 	bne	66ec <block_cipher_aes_init+0x11c>
    65f4:	e59f3134 	ldr	r3, [pc, #308]	; 6730 <block_cipher_aes_init+0x160>
    65f8:	e3a02001 	mov	r2, #1	; 0x1
    65fc:	e3550001 	cmp	r5, #1	; 0x1
    6600:	e5832404 	str	r2, [r3, #1028]
    6604:	8a000038 	bhi	66ec <block_cipher_aes_init+0x11c>
    6608:	e3540003 	cmp	r4, #3	; 0x3
    660c:	13540000 	cmpne	r4, #0	; 0x0
    6610:	0a000001 	beq	661c <block_cipher_aes_init+0x4c>
    6614:	e1540002 	cmp	r4, r2
    6618:	1a000033 	bne	66ec <block_cipher_aes_init+0x11c>
    661c:	e1a03305 	lsl	r3, r5, #6
    6620:	e3833901 	orr	r3, r3, #16384	; 0x4000
    6624:	e3833004 	orr	r3, r3, #4	; 0x4
    6628:	e59f2100 	ldr	r2, [pc, #256]	; 6730 <block_cipher_aes_init+0x160>
    662c:	e1833984 	orr	r3, r3, r4, lsl #19
    6630:	e3540000 	cmp	r4, #0	; 0x0
    6634:	e5823400 	str	r3, [r2, #1024]
    6638:	1a000008 	bne	6660 <block_cipher_aes_init+0x90>
    663c:	e59f10f0 	ldr	r1, [pc, #240]	; 6734 <block_cipher_aes_init+0x164>
    6640:	e59f00f0 	ldr	r0, [pc, #240]	; 6738 <block_cipher_aes_init+0x168>
    6644:	e59f30f0 	ldr	r3, [pc, #240]	; 673c <block_cipher_aes_init+0x16c>
    6648:	e0862001 	add	r2, r6, r1
    664c:	e0823003 	add	r3, r2, r3
    6650:	e5933000 	ldr	r3, [r3]
    6654:	e4813004 	str	r3, [r1], #4
    6658:	e1510000 	cmp	r1, r0
    665c:	1afffff8 	bne	6644 <block_cipher_aes_init+0x74>
    6660:	e59f10d0 	ldr	r1, [pc, #208]	; 6738 <block_cipher_aes_init+0x168>
    6664:	e59f00d4 	ldr	r0, [pc, #212]	; 6740 <block_cipher_aes_init+0x170>
    6668:	e59f30d4 	ldr	r3, [pc, #212]	; 6744 <block_cipher_aes_init+0x174>
    666c:	e0872001 	add	r2, r7, r1
    6670:	e0823003 	add	r3, r2, r3
    6674:	e5933000 	ldr	r3, [r3]
    6678:	e4813004 	str	r3, [r1], #4
    667c:	e1510000 	cmp	r1, r0
    6680:	1afffff8 	bne	6668 <block_cipher_aes_init+0x98>
    6684:	e3540000 	cmp	r4, #0	; 0x0
    6688:	1a000008 	bne	66b0 <block_cipher_aes_init+0xe0>
    668c:	eb001fee 	bl	e64c <get_ticks>
    6690:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    6694:	e3a05000 	mov	r5, #0	; 0x0
    6698:	e0944000 	adds	r4, r4, r0
    669c:	e59f608c 	ldr	r6, [pc, #140]	; 6730 <block_cipher_aes_init+0x160>
    66a0:	e0a55001 	adc	r5, r5, r1
    66a4:	e596340c 	ldr	r3, [r6, #1036]
    66a8:	e3130008 	tst	r3, #8	; 0x8
    66ac:	0a000011 	beq	66f8 <block_cipher_aes_init+0x128>
    66b0:	eb001fe5 	bl	e64c <get_ticks>
    66b4:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    66b8:	e3a05000 	mov	r5, #0	; 0x0
    66bc:	e0944000 	adds	r4, r4, r0
    66c0:	e59f6068 	ldr	r6, [pc, #104]	; 6730 <block_cipher_aes_init+0x160>
    66c4:	e0a55001 	adc	r5, r5, r1
    66c8:	e596340c 	ldr	r3, [r6, #1036]
    66cc:	e59f205c 	ldr	r2, [pc, #92]	; 6730 <block_cipher_aes_init+0x160>
    66d0:	e3130010 	tst	r3, #16	; 0x10
    66d4:	0a00000e 	beq	6714 <block_cipher_aes_init+0x144>
    66d8:	e5923400 	ldr	r3, [r2, #1024]
    66dc:	e3a00000 	mov	r0, #0	; 0x0
    66e0:	e3833001 	orr	r3, r3, #1	; 0x1
    66e4:	e5823400 	str	r3, [r2, #1024]
    66e8:	ea000000 	b	66f0 <block_cipher_aes_init+0x120>
    66ec:	e3a00001 	mov	r0, #1	; 0x1
    66f0:	e28dd004 	add	sp, sp, #4	; 0x4
    66f4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    66f8:	eb001fd3 	bl	e64c <get_ticks>
    66fc:	e1510005 	cmp	r1, r5
    6700:	8afffff9 	bhi	66ec <block_cipher_aes_init+0x11c>
    6704:	1affffe6 	bne	66a4 <block_cipher_aes_init+0xd4>
    6708:	e1500004 	cmp	r0, r4
    670c:	8afffff6 	bhi	66ec <block_cipher_aes_init+0x11c>
    6710:	eaffffe3 	b	66a4 <block_cipher_aes_init+0xd4>
    6714:	eb001fcc 	bl	e64c <get_ticks>
    6718:	e1510005 	cmp	r1, r5
    671c:	8afffff2 	bhi	66ec <block_cipher_aes_init+0x11c>
    6720:	1affffe8 	bne	66c8 <block_cipher_aes_init+0xf8>
    6724:	e1500004 	cmp	r0, r4
    6728:	8affffef 	bhi	66ec <block_cipher_aes_init+0x11c>
    672c:	eaffffe5 	b	66c8 <block_cipher_aes_init+0xf8>
    6730:	23200000 	.word	0x23200000
    6734:	23200420 	.word	0x23200420
    6738:	23200440 	.word	0x23200440
    673c:	dcdffbe0 	.word	0xdcdffbe0
    6740:	23200450 	.word	0x23200450
    6744:	dcdffbc0 	.word	0xdcdffbc0

00006748 <block_cipher_operation>:
    6748:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    674c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    6750:	e1a08002 	mov	r8, r2
    6754:	e58d0004 	str	r0, [sp, #4]
    6758:	e1a09001 	mov	r9, r1
    675c:	e1a06002 	mov	r6, r2
    6760:	e59fa0c4 	ldr	sl, [pc, #196]	; 682c <block_cipher_operation+0xe4>
    6764:	e3a0b000 	mov	fp, #0	; 0x0
    6768:	ea000028 	b	6810 <block_cipher_operation+0xc8>
    676c:	e59d3004 	ldr	r3, [sp, #4]
    6770:	e3560902 	cmp	r6, #32768	; 0x8000
    6774:	b1a07006 	movlt	r7, r6
    6778:	a3a07902 	movge	r7, #32768	; 0x8000
    677c:	e0664008 	rsb	r4, r6, r8
    6780:	e3a00000 	mov	r0, #0	; 0x0
    6784:	e0831004 	add	r1, r3, r4
    6788:	e1a02007 	mov	r2, r7
    678c:	eb0000e8 	bl	6b34 <dma_sdma_init>
    6790:	e3500000 	cmp	r0, #0	; 0x0
    6794:	1a000021 	bne	6820 <block_cipher_operation+0xd8>
    6798:	e0891004 	add	r1, r9, r4
    679c:	e2800001 	add	r0, r0, #1	; 0x1
    67a0:	e1a02007 	mov	r2, r7
    67a4:	eb0000e2 	bl	6b34 <dma_sdma_init>
    67a8:	e3500000 	cmp	r0, #0	; 0x0
    67ac:	1a00001b 	bne	6820 <block_cipher_operation+0xd8>
    67b0:	eb0000cf 	bl	6af4 <dma_start>
    67b4:	e3a00001 	mov	r0, #1	; 0x1
    67b8:	eb0000cd 	bl	6af4 <dma_start>
    67bc:	eb001fa2 	bl	e64c <get_ticks>
    67c0:	e090400a 	adds	r4, r0, sl
    67c4:	e0a1500b 	adc	r5, r1, fp
    67c8:	ea000005 	b	67e4 <block_cipher_operation+0x9c>
    67cc:	eb001f9e 	bl	e64c <get_ticks>
    67d0:	e1510005 	cmp	r1, r5
    67d4:	8a000011 	bhi	6820 <block_cipher_operation+0xd8>
    67d8:	1a000001 	bne	67e4 <block_cipher_operation+0x9c>
    67dc:	e1500004 	cmp	r0, r4
    67e0:	8a00000e 	bhi	6820 <block_cipher_operation+0xd8>
    67e4:	e3a01003 	mov	r1, #3	; 0x3
    67e8:	e3a00001 	mov	r0, #1	; 0x1
    67ec:	eb0000c6 	bl	6b0c <dma_get_status>
    67f0:	e3500000 	cmp	r0, #0	; 0x0
    67f4:	0afffff4 	beq	67cc <block_cipher_operation+0x84>
    67f8:	e3a00000 	mov	r0, #0	; 0x0
    67fc:	e3a01003 	mov	r1, #3	; 0x3
    6800:	eb0000c1 	bl	6b0c <dma_get_status>
    6804:	e3500000 	cmp	r0, #0	; 0x0
    6808:	0a000004 	beq	6820 <block_cipher_operation+0xd8>
    680c:	e0676006 	rsb	r6, r7, r6
    6810:	e3560000 	cmp	r6, #0	; 0x0
    6814:	1affffd4 	bne	676c <block_cipher_operation+0x24>
    6818:	e1a00006 	mov	r0, r6
    681c:	ea000000 	b	6824 <block_cipher_operation+0xdc>
    6820:	e3a00001 	mov	r0, #1	; 0x1
    6824:	e28dd00c 	add	sp, sp, #12	; 0xc
    6828:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    682c:	000f4240 	.word	0x000f4240

00006830 <hash_len>:
    6830:	e59f300c 	ldr	r3, [pc, #12]	; 6844 <hash_len+0x14>
    6834:	e5932000 	ldr	r2, [r3]
    6838:	e59f3008 	ldr	r3, [pc, #8]	; 6848 <hash_len+0x18>
    683c:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    6840:	e12fff1e 	bx	lr
    6844:	3c00a08c 	.word	0x3c00a08c
    6848:	00015e7c 	.word	0x00015e7c

0000684c <hash_value>:
    684c:	e92d4070 	push	{r4, r5, r6, lr}
    6850:	e59f6054 	ldr	r6, [pc, #84]	; 68ac <hash_value+0x60>
    6854:	e1a05000 	mov	r5, r0
    6858:	e5963000 	ldr	r3, [r6]
    685c:	e3530000 	cmp	r3, #0	; 0x0
    6860:	0a000009 	beq	688c <hash_value+0x40>
    6864:	eb0000d4 	bl	6bbc <security_enabled>
    6868:	e2504000 	subs	r4, r0, #0	; 0x0
    686c:	1a000006 	bne	688c <hash_value+0x40>
    6870:	e59f0038 	ldr	r0, [pc, #56]	; 68b0 <hash_value+0x64>
    6874:	e5961000 	ldr	r1, [r6]
    6878:	e59f3034 	ldr	r3, [pc, #52]	; 68b4 <hash_value+0x68>
    687c:	e1a0e00f 	mov	lr, pc
    6880:	e593f000 	ldr	pc, [r3]
    6884:	e1a00004 	mov	r0, r4
    6888:	e8bd8070 	pop	{r4, r5, r6, pc}
    688c:	e59f3018 	ldr	r3, [pc, #24]	; 68ac <hash_value+0x60>
    6890:	e1a00005 	mov	r0, r5
    6894:	e5932000 	ldr	r2, [r3]
    6898:	e59f3018 	ldr	r3, [pc, #24]	; 68b8 <hash_value+0x6c>
    689c:	e0833202 	add	r3, r3, r2, lsl #4
    68a0:	e1a0e00f 	mov	lr, pc
    68a4:	e593f00c 	ldr	pc, [r3, #12]
    68a8:	e8bd8070 	pop	{r4, r5, r6, pc}
    68ac:	3c00a08c 	.word	0x3c00a08c
    68b0:	00018222 	.word	0x00018222
    68b4:	3c00a5cc 	.word	0x3c00a5cc
    68b8:	00015e7c 	.word	0x00015e7c

000068bc <hash_data>:
    68bc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    68c0:	e59f7064 	ldr	r7, [pc, #100]	; 692c <hash_data+0x70>
    68c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    68c8:	e5973000 	ldr	r3, [r7]
    68cc:	e1a05000 	mov	r5, r0
    68d0:	e3530000 	cmp	r3, #0	; 0x0
    68d4:	e1a06001 	mov	r6, r1
    68d8:	0a000009 	beq	6904 <hash_data+0x48>
    68dc:	eb0000b6 	bl	6bbc <security_enabled>
    68e0:	e2504000 	subs	r4, r0, #0	; 0x0
    68e4:	1a000006 	bne	6904 <hash_data+0x48>
    68e8:	e59f0040 	ldr	r0, [pc, #64]	; 6930 <hash_data+0x74>
    68ec:	e5971000 	ldr	r1, [r7]
    68f0:	e59f303c 	ldr	r3, [pc, #60]	; 6934 <hash_data+0x78>
    68f4:	e1a0e00f 	mov	lr, pc
    68f8:	e593f000 	ldr	pc, [r3]
    68fc:	e1a00004 	mov	r0, r4
    6900:	ea000007 	b	6924 <hash_data+0x68>
    6904:	e59f3020 	ldr	r3, [pc, #32]	; 692c <hash_data+0x70>
    6908:	e1a00005 	mov	r0, r5
    690c:	e5932000 	ldr	r2, [r3]
    6910:	e59f3020 	ldr	r3, [pc, #32]	; 6938 <hash_data+0x7c>
    6914:	e1a01006 	mov	r1, r6
    6918:	e0833202 	add	r3, r3, r2, lsl #4
    691c:	e1a0e00f 	mov	lr, pc
    6920:	e593f008 	ldr	pc, [r3, #8]
    6924:	e28dd004 	add	sp, sp, #4	; 0x4
    6928:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    692c:	3c00a08c 	.word	0x3c00a08c
    6930:	00018222 	.word	0x00018222
    6934:	3c00a5cc 	.word	0x3c00a5cc
    6938:	00015e7c 	.word	0x00015e7c

0000693c <hash_init>:
    693c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6940:	e3500003 	cmp	r0, #3	; 0x3
    6944:	e24dd004 	sub	sp, sp, #4	; 0x4
    6948:	e1a04000 	mov	r4, r0
    694c:	e1a07001 	mov	r7, r1
    6950:	9a000005 	bls	696c <hash_init+0x30>
    6954:	e59f0064 	ldr	r0, [pc, #100]	; 69c0 <hash_init+0x84>
    6958:	e59f3064 	ldr	r3, [pc, #100]	; 69c4 <hash_init+0x88>
    695c:	e1a0e00f 	mov	lr, pc
    6960:	e593f000 	ldr	pc, [r3]
    6964:	e3e00000 	mvn	r0, #0	; 0x0
    6968:	ea000012 	b	69b8 <hash_init+0x7c>
    696c:	e59f6054 	ldr	r6, [pc, #84]	; 69c8 <hash_init+0x8c>
    6970:	e3500000 	cmp	r0, #0	; 0x0
    6974:	e5860000 	str	r0, [r6]
    6978:	0a000009 	beq	69a4 <hash_init+0x68>
    697c:	eb00008e 	bl	6bbc <security_enabled>
    6980:	e2505000 	subs	r5, r0, #0	; 0x0
    6984:	1a000006 	bne	69a4 <hash_init+0x68>
    6988:	e59f003c 	ldr	r0, [pc, #60]	; 69cc <hash_init+0x90>
    698c:	e5961000 	ldr	r1, [r6]
    6990:	e59f302c 	ldr	r3, [pc, #44]	; 69c4 <hash_init+0x88>
    6994:	e1a0e00f 	mov	lr, pc
    6998:	e593f000 	ldr	pc, [r3]
    699c:	e1a00005 	mov	r0, r5
    69a0:	ea000004 	b	69b8 <hash_init+0x7c>
    69a4:	e59f3024 	ldr	r3, [pc, #36]	; 69d0 <hash_init+0x94>
    69a8:	e1a00007 	mov	r0, r7
    69ac:	e0833204 	add	r3, r3, r4, lsl #4
    69b0:	e1a0e00f 	mov	lr, pc
    69b4:	e593f004 	ldr	pc, [r3, #4]
    69b8:	e28dd004 	add	sp, sp, #4	; 0x4
    69bc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    69c0:	00018243 	.word	0x00018243
    69c4:	3c00a5cc 	.word	0x3c00a5cc
    69c8:	3c00a08c 	.word	0x3c00a08c
    69cc:	00018222 	.word	0x00018222
    69d0:	00015e7c 	.word	0x00015e7c

000069d4 <md5_value>:
    69d4:	e92d4030 	push	{r4, r5, lr}
    69d8:	e59f3054 	ldr	r3, [pc, #84]	; 6a34 <md5_value+0x60>
    69dc:	e1a05000 	mov	r5, r0
    69e0:	e893000c 	ldm	r3, {r2, r3}
    69e4:	e1823003 	orr	r3, r2, r3
    69e8:	e24dd024 	sub	sp, sp, #36	; 0x24
    69ec:	e3530000 	cmp	r3, #0	; 0x0
    69f0:	13e00000 	mvnne	r0, #0	; 0x0
    69f4:	1a00000c 	bne	6a2c <md5_value+0x58>
    69f8:	e1a0000d 	mov	r0, sp
    69fc:	eb00016d 	bl	6fb8 <hash_module_get_result>
    6a00:	e3500000 	cmp	r0, #0	; 0x0
    6a04:	e1a0400d 	mov	r4, sp
    6a08:	13e00001 	mvnne	r0, #1	; 0x1
    6a0c:	1a000006 	bne	6a2c <md5_value+0x58>
    6a10:	e1a0200d 	mov	r2, sp
    6a14:	e7d23000 	ldrb	r3, [r2, r0]
    6a18:	e7c53000 	strb	r3, [r5, r0]
    6a1c:	e2800001 	add	r0, r0, #1	; 0x1
    6a20:	e3500010 	cmp	r0, #16	; 0x10
    6a24:	1afffffa 	bne	6a14 <md5_value+0x40>
    6a28:	e3a00000 	mov	r0, #0	; 0x0
    6a2c:	e28dd024 	add	sp, sp, #36	; 0x24
    6a30:	e8bd8030 	pop	{r4, r5, pc}
    6a34:	3c00a090 	.word	0x3c00a090

00006a38 <md5_data>:
    6a38:	e59f305c 	ldr	r3, [pc, #92]	; 6a9c <md5_data+0x64>
    6a3c:	e92d4070 	push	{r4, r5, r6, lr}
    6a40:	e1a06001 	mov	r6, r1
    6a44:	e5931004 	ldr	r1, [r3, #4]
    6a48:	e1a04006 	mov	r4, r6
    6a4c:	e3a05000 	mov	r5, #0	; 0x0
    6a50:	e1550001 	cmp	r5, r1
    6a54:	e593c000 	ldr	ip, [r3]
    6a58:	8a000002 	bhi	6a68 <md5_data+0x30>
    6a5c:	1a000003 	bne	6a70 <md5_data+0x38>
    6a60:	e154000c 	cmp	r4, ip
    6a64:	9a000001 	bls	6a70 <md5_data+0x38>
    6a68:	e3e00000 	mvn	r0, #0	; 0x0
    6a6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a70:	e1a0200c 	mov	r2, ip
    6a74:	e1a03001 	mov	r3, r1
    6a78:	e59fc01c 	ldr	ip, [pc, #28]	; 6a9c <md5_data+0x64>
    6a7c:	e0522004 	subs	r2, r2, r4
    6a80:	e0c33005 	sbc	r3, r3, r5
    6a84:	e1a01006 	mov	r1, r6
    6a88:	e88c000c 	stm	ip, {r2, r3}
    6a8c:	eb0001ac 	bl	7144 <hash_dma_transfer>
    6a90:	e3500000 	cmp	r0, #0	; 0x0
    6a94:	13e00001 	mvnne	r0, #1	; 0x1
    6a98:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a9c:	3c00a090 	.word	0x3c00a090

00006aa0 <md5_init>:
    6aa0:	e1a03ea0 	lsr	r3, r0, #29
    6aa4:	e1a0c180 	lsl	ip, r0, #3
    6aa8:	e92d4010 	push	{r4, lr}
    6aac:	e59f403c 	ldr	r4, [pc, #60]	; 6af0 <md5_init+0x50>
    6ab0:	e1a0200c 	mov	r2, ip
    6ab4:	e1a01003 	mov	r1, r3
    6ab8:	e3a00000 	mov	r0, #0	; 0x0
    6abc:	e5843004 	str	r3, [r4, #4]
    6ac0:	e584c000 	str	ip, [r4]
    6ac4:	eb000182 	bl	70d4 <hash_module_init>
    6ac8:	e3500000 	cmp	r0, #0	; 0x0
    6acc:	05942000 	ldreq	r2, [r4]
    6ad0:	05943004 	ldreq	r3, [r4, #4]
    6ad4:	01a021a2 	lsreq	r2, r2, #3
    6ad8:	01822e83 	orreq	r2, r2, r3, lsl #29
    6adc:	01a031a3 	lsreq	r3, r3, #3
    6ae0:	13e00000 	mvnne	r0, #0	; 0x0
    6ae4:	05843004 	streq	r3, [r4, #4]
    6ae8:	05842000 	streq	r2, [r4]
    6aec:	e8bd8010 	pop	{r4, pc}
    6af0:	3c00a090 	.word	0x3c00a090

00006af4 <dma_start>:
    6af4:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6af8:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6afc:	e1a00400 	lsl	r0, r0, #8
    6b00:	e3a03001 	mov	r3, #1	; 0x1
    6b04:	e5803000 	str	r3, [r0]
    6b08:	e12fff1e 	bx	lr

00006b0c <dma_get_status>:
    6b0c:	e3a0c001 	mov	ip, #1	; 0x1
    6b10:	e1a0211c 	lsl	r2, ip, r1
    6b14:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6b18:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6b1c:	e1a01400 	lsl	r1, r0, #8
    6b20:	e5913024 	ldr	r3, [r1, #36]
    6b24:	e0130002 	ands	r0, r3, r2
    6b28:	15812024 	strne	r2, [r1, #36]
    6b2c:	11a0000c 	movne	r0, ip
    6b30:	e12fff1e 	bx	lr

00006b34 <dma_sdma_init>:
    6b34:	e280c823 	add	ip, r0, #2293760	; 0x230000
    6b38:	e28cca02 	add	ip, ip, #8192	; 0x2000
    6b3c:	e1a0040c 	lsl	r0, ip, #8
    6b40:	e2800004 	add	r0, r0, #4	; 0x4
    6b44:	e5903000 	ldr	r3, [r0]
    6b48:	e3520801 	cmp	r2, #65536	; 0x10000
    6b4c:	e3833001 	orr	r3, r3, #1	; 0x1
    6b50:	e5803000 	str	r3, [r0]
    6b54:	e92d4010 	push	{r4, lr}
    6b58:	e3a03341 	mov	r3, #67108865	; 0x4000001
    6b5c:	e1a0440c 	lsl	r4, ip, #8
    6b60:	e584300c 	str	r3, [r4, #12]
    6b64:	31a02802 	lslcc	r2, r2, #16
    6b68:	31a0340c 	lslcc	r3, ip, #8
    6b6c:	31a02822 	lsrcc	r2, r2, #16
    6b70:	32833010 	addcc	r3, r3, #16	; 0x10
    6b74:	35832000 	strcc	r2, [r3]
    6b78:	23a00001 	movcs	r0, #1	; 0x1
    6b7c:	35841014 	strcc	r1, [r4, #20]
    6b80:	33a00000 	movcc	r0, #0	; 0x0
    6b84:	e8bd8010 	pop	{r4, pc}

00006b88 <security_cke_control>:
    6b88:	e59fc014 	ldr	ip, [pc, #20]	; 6ba4 <security_cke_control+0x1c>
    6b8c:	e3a02001 	mov	r2, #1	; 0x1
    6b90:	e59c3000 	ldr	r3, [ip]
    6b94:	e1c33012 	bic	r3, r3, r2, lsl r0
    6b98:	e1833011 	orr	r3, r3, r1, lsl r0
    6b9c:	e58c3000 	str	r3, [ip]
    6ba0:	e12fff1e 	bx	lr
    6ba4:	23202000 	.word	0x23202000

00006ba8 <security_enable>:
    6ba8:	e59f3008 	ldr	r3, [pc, #8]	; 6bb8 <security_enable+0x10>
    6bac:	e3a02001 	mov	r2, #1	; 0x1
    6bb0:	e5832000 	str	r2, [r3]
    6bb4:	e12fff1e 	bx	lr
    6bb8:	3c00a098 	.word	0x3c00a098

00006bbc <security_enabled>:
    6bbc:	e59f300c 	ldr	r3, [pc, #12]	; 6bd0 <security_enabled+0x14>
    6bc0:	e5930000 	ldr	r0, [r3]
    6bc4:	e2500000 	subs	r0, r0, #0	; 0x0
    6bc8:	13a00001 	movne	r0, #1	; 0x1
    6bcc:	e12fff1e 	bx	lr
    6bd0:	3c00a098 	.word	0x3c00a098

00006bd4 <dma_reset>:
    6bd4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6bd8:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6bdc:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6be0:	e1a07400 	lsl	r7, r0, #8
    6be4:	e3a03002 	mov	r3, #2	; 0x2
    6be8:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bec:	e5873000 	str	r3, [r7]
    6bf0:	eb001e95 	bl	e64c <get_ticks>
    6bf4:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    6bf8:	e3a05000 	mov	r5, #0	; 0x0
    6bfc:	e0944000 	adds	r4, r4, r0
    6c00:	e0a55001 	adc	r5, r5, r1
    6c04:	e5976000 	ldr	r6, [r7]
    6c08:	ea000008 	b	6c30 <dma_reset+0x5c>
    6c0c:	e5976000 	ldr	r6, [r7]
    6c10:	eb001e8d 	bl	e64c <get_ticks>
    6c14:	e1510005 	cmp	r1, r5
    6c18:	8a000002 	bhi	6c28 <dma_reset+0x54>
    6c1c:	1a000003 	bne	6c30 <dma_reset+0x5c>
    6c20:	e1500004 	cmp	r0, r4
    6c24:	9a000001 	bls	6c30 <dma_reset+0x5c>
    6c28:	e3a00001 	mov	r0, #1	; 0x1
    6c2c:	ea000002 	b	6c3c <dma_reset+0x68>
    6c30:	e3560000 	cmp	r6, #0	; 0x0
    6c34:	1afffff4 	bne	6c0c <dma_reset+0x38>
    6c38:	e1a00006 	mov	r0, r6
    6c3c:	e28dd004 	add	sp, sp, #4	; 0x4
    6c40:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00006c44 <sha1_value>:
    6c44:	e92d4030 	push	{r4, r5, lr}
    6c48:	e59f3054 	ldr	r3, [pc, #84]	; 6ca4 <sha1_value+0x60>
    6c4c:	e1a05000 	mov	r5, r0
    6c50:	e893000c 	ldm	r3, {r2, r3}
    6c54:	e1823003 	orr	r3, r2, r3
    6c58:	e24dd024 	sub	sp, sp, #36	; 0x24
    6c5c:	e3530000 	cmp	r3, #0	; 0x0
    6c60:	13e00000 	mvnne	r0, #0	; 0x0
    6c64:	1a00000c 	bne	6c9c <sha1_value+0x58>
    6c68:	e1a0000d 	mov	r0, sp
    6c6c:	eb0000d1 	bl	6fb8 <hash_module_get_result>
    6c70:	e3500000 	cmp	r0, #0	; 0x0
    6c74:	e1a0400d 	mov	r4, sp
    6c78:	13e00001 	mvnne	r0, #1	; 0x1
    6c7c:	1a000006 	bne	6c9c <sha1_value+0x58>
    6c80:	e1a0200d 	mov	r2, sp
    6c84:	e7d23000 	ldrb	r3, [r2, r0]
    6c88:	e7c53000 	strb	r3, [r5, r0]
    6c8c:	e2800001 	add	r0, r0, #1	; 0x1
    6c90:	e3500014 	cmp	r0, #20	; 0x14
    6c94:	1afffffa 	bne	6c84 <sha1_value+0x40>
    6c98:	e3a00000 	mov	r0, #0	; 0x0
    6c9c:	e28dd024 	add	sp, sp, #36	; 0x24
    6ca0:	e8bd8030 	pop	{r4, r5, pc}
    6ca4:	3c00a0a0 	.word	0x3c00a0a0

00006ca8 <sha1_data>:
    6ca8:	e59f305c 	ldr	r3, [pc, #92]	; 6d0c <sha1_data+0x64>
    6cac:	e92d4070 	push	{r4, r5, r6, lr}
    6cb0:	e1a06001 	mov	r6, r1
    6cb4:	e5931004 	ldr	r1, [r3, #4]
    6cb8:	e1a04006 	mov	r4, r6
    6cbc:	e3a05000 	mov	r5, #0	; 0x0
    6cc0:	e1550001 	cmp	r5, r1
    6cc4:	e593c000 	ldr	ip, [r3]
    6cc8:	8a000002 	bhi	6cd8 <sha1_data+0x30>
    6ccc:	1a000003 	bne	6ce0 <sha1_data+0x38>
    6cd0:	e154000c 	cmp	r4, ip
    6cd4:	9a000001 	bls	6ce0 <sha1_data+0x38>
    6cd8:	e3e00000 	mvn	r0, #0	; 0x0
    6cdc:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ce0:	e1a0200c 	mov	r2, ip
    6ce4:	e1a03001 	mov	r3, r1
    6ce8:	e59fc01c 	ldr	ip, [pc, #28]	; 6d0c <sha1_data+0x64>
    6cec:	e0522004 	subs	r2, r2, r4
    6cf0:	e0c33005 	sbc	r3, r3, r5
    6cf4:	e1a01006 	mov	r1, r6
    6cf8:	e88c000c 	stm	ip, {r2, r3}
    6cfc:	eb000110 	bl	7144 <hash_dma_transfer>
    6d00:	e3500000 	cmp	r0, #0	; 0x0
    6d04:	13e00001 	mvnne	r0, #1	; 0x1
    6d08:	e8bd8070 	pop	{r4, r5, r6, pc}
    6d0c:	3c00a0a0 	.word	0x3c00a0a0

00006d10 <sha1_init>:
    6d10:	e1a03ea0 	lsr	r3, r0, #29
    6d14:	e1a0c180 	lsl	ip, r0, #3
    6d18:	e92d4010 	push	{r4, lr}
    6d1c:	e59f403c 	ldr	r4, [pc, #60]	; 6d60 <sha1_init+0x50>
    6d20:	e1a0200c 	mov	r2, ip
    6d24:	e1a01003 	mov	r1, r3
    6d28:	e3a00001 	mov	r0, #1	; 0x1
    6d2c:	e5843004 	str	r3, [r4, #4]
    6d30:	e584c000 	str	ip, [r4]
    6d34:	eb0000e6 	bl	70d4 <hash_module_init>
    6d38:	e3500000 	cmp	r0, #0	; 0x0
    6d3c:	05942000 	ldreq	r2, [r4]
    6d40:	05943004 	ldreq	r3, [r4, #4]
    6d44:	01a021a2 	lsreq	r2, r2, #3
    6d48:	01822e83 	orreq	r2, r2, r3, lsl #29
    6d4c:	01a031a3 	lsreq	r3, r3, #3
    6d50:	13e00000 	mvnne	r0, #0	; 0x0
    6d54:	05843004 	streq	r3, [r4, #4]
    6d58:	05842000 	streq	r2, [r4]
    6d5c:	e8bd8010 	pop	{r4, pc}
    6d60:	3c00a0a0 	.word	0x3c00a0a0

00006d64 <sha256_value>:
    6d64:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6d68:	e59f3028 	ldr	r3, [pc, #40]	; 6d98 <sha256_value+0x34>
    6d6c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6d70:	e893000c 	ldm	r3, {r2, r3}
    6d74:	e1823003 	orr	r3, r2, r3
    6d78:	e3530000 	cmp	r3, #0	; 0x0
    6d7c:	13e00000 	mvnne	r0, #0	; 0x0
    6d80:	1a000002 	bne	6d90 <sha256_value+0x2c>
    6d84:	eb00008b 	bl	6fb8 <hash_module_get_result>
    6d88:	e3500000 	cmp	r0, #0	; 0x0
    6d8c:	13e00001 	mvnne	r0, #1	; 0x1
    6d90:	e28dd004 	add	sp, sp, #4	; 0x4
    6d94:	e8bd8000 	pop	{pc}
    6d98:	3c00a0a8 	.word	0x3c00a0a8

00006d9c <sha256_data>:
    6d9c:	e59f305c 	ldr	r3, [pc, #92]	; 6e00 <sha256_data+0x64>
    6da0:	e92d4070 	push	{r4, r5, r6, lr}
    6da4:	e1a06001 	mov	r6, r1
    6da8:	e5931004 	ldr	r1, [r3, #4]
    6dac:	e1a04006 	mov	r4, r6
    6db0:	e3a05000 	mov	r5, #0	; 0x0
    6db4:	e1550001 	cmp	r5, r1
    6db8:	e593c000 	ldr	ip, [r3]
    6dbc:	8a000002 	bhi	6dcc <sha256_data+0x30>
    6dc0:	1a000003 	bne	6dd4 <sha256_data+0x38>
    6dc4:	e154000c 	cmp	r4, ip
    6dc8:	9a000001 	bls	6dd4 <sha256_data+0x38>
    6dcc:	e3e00000 	mvn	r0, #0	; 0x0
    6dd0:	e8bd8070 	pop	{r4, r5, r6, pc}
    6dd4:	e1a0200c 	mov	r2, ip
    6dd8:	e1a03001 	mov	r3, r1
    6ddc:	e59fc01c 	ldr	ip, [pc, #28]	; 6e00 <sha256_data+0x64>
    6de0:	e0522004 	subs	r2, r2, r4
    6de4:	e0c33005 	sbc	r3, r3, r5
    6de8:	e1a01006 	mov	r1, r6
    6dec:	e88c000c 	stm	ip, {r2, r3}
    6df0:	eb0000d3 	bl	7144 <hash_dma_transfer>
    6df4:	e3500000 	cmp	r0, #0	; 0x0
    6df8:	13e00001 	mvnne	r0, #1	; 0x1
    6dfc:	e8bd8070 	pop	{r4, r5, r6, pc}
    6e00:	3c00a0a8 	.word	0x3c00a0a8

00006e04 <sha256_init>:
    6e04:	e1a03ea0 	lsr	r3, r0, #29
    6e08:	e1a0c180 	lsl	ip, r0, #3
    6e0c:	e92d4010 	push	{r4, lr}
    6e10:	e59f403c 	ldr	r4, [pc, #60]	; 6e54 <sha256_init+0x50>
    6e14:	e1a0200c 	mov	r2, ip
    6e18:	e1a01003 	mov	r1, r3
    6e1c:	e3a00003 	mov	r0, #3	; 0x3
    6e20:	e5843004 	str	r3, [r4, #4]
    6e24:	e584c000 	str	ip, [r4]
    6e28:	eb0000a9 	bl	70d4 <hash_module_init>
    6e2c:	e3500000 	cmp	r0, #0	; 0x0
    6e30:	05942000 	ldreq	r2, [r4]
    6e34:	05943004 	ldreq	r3, [r4, #4]
    6e38:	01a021a2 	lsreq	r2, r2, #3
    6e3c:	01822e83 	orreq	r2, r2, r3, lsl #29
    6e40:	01a031a3 	lsreq	r3, r3, #3
    6e44:	13e00000 	mvnne	r0, #0	; 0x0
    6e48:	05843004 	streq	r3, [r4, #4]
    6e4c:	05842000 	streq	r2, [r4]
    6e50:	e8bd8010 	pop	{r4, pc}
    6e54:	3c00a0a8 	.word	0x3c00a0a8

00006e58 <crc32_init>:
    6e58:	e59f300c 	ldr	r3, [pc, #12]	; 6e6c <crc32_init+0x14>
    6e5c:	e3e02000 	mvn	r2, #0	; 0x0
    6e60:	e5832000 	str	r2, [r3]
    6e64:	e3a00000 	mov	r0, #0	; 0x0
    6e68:	e12fff1e 	bx	lr
    6e6c:	3c00a0b0 	.word	0x3c00a0b0

00006e70 <crc32_data>:
    6e70:	e59f311c 	ldr	r3, [pc, #284]	; 6f94 <crc32_data+0x124>
    6e74:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    6e78:	e593c000 	ldr	ip, [r3]
    6e7c:	e1a08000 	mov	r8, r0
    6e80:	e1a06001 	mov	r6, r1
    6e84:	e1a04000 	mov	r4, r0
    6e88:	e1a07001 	mov	r7, r1
    6e8c:	e59f5104 	ldr	r5, [pc, #260]	; 6f98 <crc32_data+0x128>
    6e90:	ea000027 	b	6f34 <crc32_data+0xc4>
    6e94:	e5543008 	ldrb	r3, [r4, #-8]
    6e98:	e5542007 	ldrb	r2, [r4, #-7]
    6e9c:	e02c3003 	eor	r3, ip, r3
    6ea0:	e20330ff 	and	r3, r3, #255	; 0xff
    6ea4:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    6ea8:	e5541006 	ldrb	r1, [r4, #-6]
    6eac:	e023342c 	eor	r3, r3, ip, lsr #8
    6eb0:	e0232002 	eor	r2, r3, r2
    6eb4:	e20220ff 	and	r2, r2, #255	; 0xff
    6eb8:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    6ebc:	e5540005 	ldrb	r0, [r4, #-5]
    6ec0:	e0222423 	eor	r2, r2, r3, lsr #8
    6ec4:	e0221001 	eor	r1, r2, r1
    6ec8:	e20110ff 	and	r1, r1, #255	; 0xff
    6ecc:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    6ed0:	e5541004 	ldrb	r1, [r4, #-4]
    6ed4:	e0233422 	eor	r3, r3, r2, lsr #8
    6ed8:	e0230000 	eor	r0, r3, r0
    6edc:	e20000ff 	and	r0, r0, #255	; 0xff
    6ee0:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    6ee4:	e5540003 	ldrb	r0, [r4, #-3]
    6ee8:	e0222423 	eor	r2, r2, r3, lsr #8
    6eec:	e0221001 	eor	r1, r2, r1
    6ef0:	e20110ff 	and	r1, r1, #255	; 0xff
    6ef4:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    6ef8:	e554c002 	ldrb	ip, [r4, #-2]
    6efc:	e0233422 	eor	r3, r3, r2, lsr #8
    6f00:	e0230000 	eor	r0, r3, r0
    6f04:	e20000ff 	and	r0, r0, #255	; 0xff
    6f08:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    6f0c:	e5541001 	ldrb	r1, [r4, #-1]
    6f10:	e0222423 	eor	r2, r2, r3, lsr #8
    6f14:	e022c00c 	eor	ip, r2, ip
    6f18:	e20cc0ff 	and	ip, ip, #255	; 0xff
    6f1c:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    6f20:	e0233422 	eor	r3, r3, r2, lsr #8
    6f24:	e0231001 	eor	r1, r3, r1
    6f28:	e20110ff 	and	r1, r1, #255	; 0xff
    6f2c:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    6f30:	e022c423 	eor	ip, r2, r3, lsr #8
    6f34:	e3570007 	cmp	r7, #7	; 0x7
    6f38:	e2844008 	add	r4, r4, #8	; 0x8
    6f3c:	e2477008 	sub	r7, r7, #8	; 0x8
    6f40:	8affffd3 	bhi	6e94 <crc32_data+0x24>
    6f44:	e3c63007 	bic	r3, r6, #7	; 0x7
    6f48:	e0880003 	add	r0, r8, r3
    6f4c:	e59f3040 	ldr	r3, [pc, #64]	; 6f94 <crc32_data+0x124>
    6f50:	e2161007 	ands	r1, r6, #7	; 0x7
    6f54:	e583c000 	str	ip, [r3]
    6f58:	0a00000b 	beq	6f8c <crc32_data+0x11c>
    6f5c:	e59f4034 	ldr	r4, [pc, #52]	; 6f98 <crc32_data+0x128>
    6f60:	e3a02000 	mov	r2, #0	; 0x0
    6f64:	e7d03002 	ldrb	r3, [r0, r2]
    6f68:	e2822001 	add	r2, r2, #1	; 0x1
    6f6c:	e02c3003 	eor	r3, ip, r3
    6f70:	e20330ff 	and	r3, r3, #255	; 0xff
    6f74:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    6f78:	e1510002 	cmp	r1, r2
    6f7c:	e023c42c 	eor	ip, r3, ip, lsr #8
    6f80:	1afffff7 	bne	6f64 <crc32_data+0xf4>
    6f84:	e59f3008 	ldr	r3, [pc, #8]	; 6f94 <crc32_data+0x124>
    6f88:	e583c000 	str	ip, [r3]
    6f8c:	e3a00000 	mov	r0, #0	; 0x0
    6f90:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    6f94:	3c00a0b0 	.word	0x3c00a0b0
    6f98:	00015ebc 	.word	0x00015ebc

00006f9c <crc32_value>:
    6f9c:	e59f3010 	ldr	r3, [pc, #16]	; 6fb4 <crc32_value+0x18>
    6fa0:	e5933000 	ldr	r3, [r3]
    6fa4:	e1e03003 	mvn	r3, r3
    6fa8:	e5803000 	str	r3, [r0]
    6fac:	e3a00000 	mov	r0, #0	; 0x0
    6fb0:	e12fff1e 	bx	lr
    6fb4:	3c00a0b0 	.word	0x3c00a0b0

00006fb8 <hash_module_get_result>:
    6fb8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    6fbc:	e1a08000 	mov	r8, r0
    6fc0:	eb001da1 	bl	e64c <get_ticks>
    6fc4:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    6fc8:	e59f3088 	ldr	r3, [pc, #136]	; 7058 <hash_module_get_result+0xa0>
    6fcc:	e3a05000 	mov	r5, #0	; 0x0
    6fd0:	e0944000 	adds	r4, r4, r0
    6fd4:	e0a55001 	adc	r5, r5, r1
    6fd8:	e1a07003 	mov	r7, r3
    6fdc:	e593680c 	ldr	r6, [r3, #2060]
    6fe0:	ea000008 	b	7008 <hash_module_get_result+0x50>
    6fe4:	e597680c 	ldr	r6, [r7, #2060]
    6fe8:	eb001d97 	bl	e64c <get_ticks>
    6fec:	e1510005 	cmp	r1, r5
    6ff0:	8a000002 	bhi	7000 <hash_module_get_result+0x48>
    6ff4:	1a000003 	bne	7008 <hash_module_get_result+0x50>
    6ff8:	e1500004 	cmp	r0, r4
    6ffc:	9a000001 	bls	7008 <hash_module_get_result+0x50>
    7000:	e3a00001 	mov	r0, #1	; 0x1
    7004:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7008:	e3160001 	tst	r6, #1	; 0x1
    700c:	0afffff4 	beq	6fe4 <hash_module_get_result+0x2c>
    7010:	e59f0044 	ldr	r0, [pc, #68]	; 705c <hash_module_get_result+0xa4>
    7014:	e59fc044 	ldr	ip, [pc, #68]	; 7060 <hash_module_get_result+0xa8>
    7018:	e1a03000 	mov	r3, r0
    701c:	e6931008 	ldr	r1, [r3], r8
    7020:	e59f203c 	ldr	r2, [pc, #60]	; 7064 <hash_module_get_result+0xac>
    7024:	e2800004 	add	r0, r0, #4	; 0x4
    7028:	e0832002 	add	r2, r3, r2
    702c:	e150000c 	cmp	r0, ip
    7030:	e5821000 	str	r1, [r2]
    7034:	1afffff7 	bne	7018 <hash_module_get_result+0x60>
    7038:	e59f2018 	ldr	r2, [pc, #24]	; 7058 <hash_module_get_result+0xa0>
    703c:	e3a03001 	mov	r3, #1	; 0x1
    7040:	e582380c 	str	r3, [r2, #2060]
    7044:	e5923800 	ldr	r3, [r2, #2048]
    7048:	e3a00000 	mov	r0, #0	; 0x0
    704c:	e3c33080 	bic	r3, r3, #128	; 0x80
    7050:	e5823800 	str	r3, [r2, #2048]
    7054:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7058:	23200000 	.word	0x23200000
    705c:	23200858 	.word	0x23200858
    7060:	23200878 	.word	0x23200878
    7064:	dcdff7a8 	.word	0xdcdff7a8

00007068 <hash_module_clear>:
    7068:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    706c:	e59f405c 	ldr	r4, [pc, #92]	; 70d0 <hash_module_clear+0x68>
    7070:	e3a03002 	mov	r3, #2	; 0x2
    7074:	e5843800 	str	r3, [r4, #2048]
    7078:	e24dd004 	sub	sp, sp, #4	; 0x4
    707c:	eb001d72 	bl	e64c <get_ticks>
    7080:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    7084:	e3a07000 	mov	r7, #0	; 0x0
    7088:	e0966000 	adds	r6, r6, r0
    708c:	e1a05004 	mov	r5, r4
    7090:	e0a77001 	adc	r7, r7, r1
    7094:	e5944800 	ldr	r4, [r4, #2048]
    7098:	ea000008 	b	70c0 <hash_module_clear+0x58>
    709c:	e5954800 	ldr	r4, [r5, #2048]
    70a0:	eb001d69 	bl	e64c <get_ticks>
    70a4:	e1510007 	cmp	r1, r7
    70a8:	8a000002 	bhi	70b8 <hash_module_clear+0x50>
    70ac:	1a000003 	bne	70c0 <hash_module_clear+0x58>
    70b0:	e1500006 	cmp	r0, r6
    70b4:	9a000001 	bls	70c0 <hash_module_clear+0x58>
    70b8:	e3a00001 	mov	r0, #1	; 0x1
    70bc:	ea000001 	b	70c8 <hash_module_clear+0x60>
    70c0:	e2140002 	ands	r0, r4, #2	; 0x2
    70c4:	1afffff4 	bne	709c <hash_module_clear+0x34>
    70c8:	e28dd004 	add	sp, sp, #4	; 0x4
    70cc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    70d0:	23200000 	.word	0x23200000

000070d4 <hash_module_init>:
    70d4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    70d8:	e59f4060 	ldr	r4, [pc, #96]	; 7140 <hash_module_init+0x6c>
    70dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    70e0:	e5943800 	ldr	r3, [r4, #2048]
    70e4:	e1a05000 	mov	r5, r0
    70e8:	e3c33080 	bic	r3, r3, #128	; 0x80
    70ec:	e5843800 	str	r3, [r4, #2048]
    70f0:	e1a07001 	mov	r7, r1
    70f4:	e1a06002 	mov	r6, r2
    70f8:	ebffffda 	bl	7068 <hash_module_clear>
    70fc:	e3500000 	cmp	r0, #0	; 0x0
    7100:	13a00001 	movne	r0, #1	; 0x1
    7104:	1a00000b 	bne	7138 <hash_module_init+0x64>
    7108:	e3a03001 	mov	r3, #1	; 0x1
    710c:	e1a02185 	lsl	r2, r5, #3
    7110:	e5843804 	str	r3, [r4, #2052]
    7114:	e5842800 	str	r2, [r4, #2048]
    7118:	e5847814 	str	r7, [r4, #2068]
    711c:	e5846810 	str	r6, [r4, #2064]
    7120:	e5943800 	ldr	r3, [r4, #2048]
    7124:	e3833080 	orr	r3, r3, #128	; 0x80
    7128:	e5843800 	str	r3, [r4, #2048]
    712c:	e5943800 	ldr	r3, [r4, #2048]
    7130:	e3833004 	orr	r3, r3, #4	; 0x4
    7134:	e5843800 	str	r3, [r4, #2048]
    7138:	e28dd004 	add	sp, sp, #4	; 0x4
    713c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7140:	23200000 	.word	0x23200000

00007144 <hash_dma_transfer>:
    7144:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7148:	e1a08001 	mov	r8, r1
    714c:	e24dd004 	sub	sp, sp, #4	; 0x4
    7150:	e1a09000 	mov	r9, r0
    7154:	e1a06001 	mov	r6, r1
    7158:	e59fa090 	ldr	sl, [pc, #144]	; 71f0 <hash_dma_transfer+0xac>
    715c:	e3a0b000 	mov	fp, #0	; 0x0
    7160:	ea00001b 	b	71d4 <hash_dma_transfer+0x90>
    7164:	e3560902 	cmp	r6, #32768	; 0x8000
    7168:	b1a07006 	movlt	r7, r6
    716c:	a3a07902 	movge	r7, #32768	; 0x8000
    7170:	e0661008 	rsb	r1, r6, r8
    7174:	e0891001 	add	r1, r9, r1
    7178:	e3a00002 	mov	r0, #2	; 0x2
    717c:	e1a02007 	mov	r2, r7
    7180:	ebfffe6b 	bl	6b34 <dma_sdma_init>
    7184:	e3500000 	cmp	r0, #0	; 0x0
    7188:	1a000015 	bne	71e4 <hash_dma_transfer+0xa0>
    718c:	e2800002 	add	r0, r0, #2	; 0x2
    7190:	ebfffe57 	bl	6af4 <dma_start>
    7194:	eb001d2c 	bl	e64c <get_ticks>
    7198:	e090400a 	adds	r4, r0, sl
    719c:	e0a1500b 	adc	r5, r1, fp
    71a0:	ea000005 	b	71bc <hash_dma_transfer+0x78>
    71a4:	eb001d28 	bl	e64c <get_ticks>
    71a8:	e1510005 	cmp	r1, r5
    71ac:	8a00000c 	bhi	71e4 <hash_dma_transfer+0xa0>
    71b0:	1a000001 	bne	71bc <hash_dma_transfer+0x78>
    71b4:	e1500004 	cmp	r0, r4
    71b8:	8a000009 	bhi	71e4 <hash_dma_transfer+0xa0>
    71bc:	e3a01003 	mov	r1, #3	; 0x3
    71c0:	e3a00002 	mov	r0, #2	; 0x2
    71c4:	ebfffe50 	bl	6b0c <dma_get_status>
    71c8:	e3500000 	cmp	r0, #0	; 0x0
    71cc:	0afffff4 	beq	71a4 <hash_dma_transfer+0x60>
    71d0:	e0676006 	rsb	r6, r7, r6
    71d4:	e3560000 	cmp	r6, #0	; 0x0
    71d8:	1affffe1 	bne	7164 <hash_dma_transfer+0x20>
    71dc:	e1a00006 	mov	r0, r6
    71e0:	ea000000 	b	71e8 <hash_dma_transfer+0xa4>
    71e4:	e3a00001 	mov	r0, #1	; 0x1
    71e8:	e28dd004 	add	sp, sp, #4	; 0x4
    71ec:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    71f0:	000f4240 	.word	0x000f4240

000071f4 <cdbg_log_toggle>:
    71f4:	e59f3014 	ldr	r3, [pc, #20]	; 7210 <cdbg_log_toggle+0x1c>
    71f8:	e3500000 	cmp	r0, #0	; 0x0
    71fc:	e59f2010 	ldr	r2, [pc, #16]	; 7214 <cdbg_log_toggle+0x20>
    7200:	01a02003 	moveq	r2, r3
    7204:	e59f300c 	ldr	r3, [pc, #12]	; 7218 <cdbg_log_toggle+0x24>
    7208:	e5832000 	str	r2, [r3]
    720c:	e12fff1e 	bx	lr
    7210:	00012cd8 	.word	0x00012cd8
    7214:	00012d5c 	.word	0x00012d5c
    7218:	3c00a5cc 	.word	0x3c00a5cc

0000721c <cdbg_wipe_bootarea>:
    721c:	e59f3018 	ldr	r3, [pc, #24]	; 723c <cdbg_wipe_bootarea+0x20>
    7220:	e59f1018 	ldr	r1, [pc, #24]	; 7240 <cdbg_wipe_bootarea+0x24>
    7224:	e59f2018 	ldr	r2, [pc, #24]	; 7244 <cdbg_wipe_bootarea+0x28>
    7228:	e5031004 	str	r1, [r3, #-4]
    722c:	e2833004 	add	r3, r3, #4	; 0x4
    7230:	e1530002 	cmp	r3, r2
    7234:	1afffffb 	bne	7228 <cdbg_wipe_bootarea+0xc>
    7238:	e12fff1e 	bx	lr
    723c:	3c000004 	.word	0x3c000004
    7240:	eafffffe 	.word	0xeafffffe
    7244:	3c008004 	.word	0x3c008004

00007248 <cdbg_dram_enabled>:
    7248:	e59f3004 	ldr	r3, [pc, #4]	; 7254 <cdbg_dram_enabled+0xc>
    724c:	e5930000 	ldr	r0, [r3]
    7250:	e12fff1e 	bx	lr
    7254:	3c00a164 	.word	0x3c00a164

00007258 <cdbg_verify_burn>:
    7258:	e59f3004 	ldr	r3, [pc, #4]	; 7264 <cdbg_verify_burn+0xc>
    725c:	e5930000 	ldr	r0, [r3]
    7260:	e12fff1e 	bx	lr
    7264:	3c00a0b8 	.word	0x3c00a0b8

00007268 <cdbg_verify_burn_key>:
    7268:	e59f0000 	ldr	r0, [pc, #0]	; 7270 <cdbg_verify_burn_key+0x8>
    726c:	e12fff1e 	bx	lr
    7270:	3c00a0bc 	.word	0x3c00a0bc

00007274 <cdbg_boot_redirect>:
    7274:	e59f301c 	ldr	r3, [pc, #28]	; 7298 <cdbg_boot_redirect+0x24>
    7278:	e5933000 	ldr	r3, [r3]
    727c:	e3730001 	cmn	r3, #1	; 0x1
    7280:	15803000 	strne	r3, [r0]
    7284:	159f3010 	ldrne	r3, [pc, #16]	; 729c <cdbg_boot_redirect+0x28>
    7288:	15932000 	ldrne	r2, [r3]
    728c:	11a03fc2 	asrne	r3, r2, #31
    7290:	1881000c 	stmne	r1, {r2, r3}
    7294:	e12fff1e 	bx	lr
    7298:	3c00a024 	.word	0x3c00a024
    729c:	3c00a0b4 	.word	0x3c00a0b4

000072a0 <cdbg_verify_burn_enable>:
    72a0:	e3500000 	cmp	r0, #0	; 0x0
    72a4:	e59f3018 	ldr	r3, [pc, #24]	; 72c4 <cdbg_verify_burn_enable+0x24>
    72a8:	03a02000 	moveq	r2, #0	; 0x0
    72ac:	13a02001 	movne	r2, #1	; 0x1
    72b0:	e5832000 	str	r2, [r3]
    72b4:	012fff1e 	bxeq	lr
    72b8:	e59f0008 	ldr	r0, [pc, #8]	; 72c8 <cdbg_verify_burn_enable+0x28>
    72bc:	e3a02020 	mov	r2, #32	; 0x20
    72c0:	eaffe867 	b	1464 <memcpy>
    72c4:	3c00a0b8 	.word	0x3c00a0b8
    72c8:	3c00a0bc 	.word	0x3c00a0bc

000072cc <cdbg_jump>:
    72cc:	e92d4030 	push	{r4, r5, lr}
    72d0:	e2504000 	subs	r4, r0, #0	; 0x0
    72d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    72d8:	e1a05001 	mov	r5, r1
    72dc:	aa00000a 	bge	730c <cdbg_jump+0x40>
    72e0:	e59f304c 	ldr	r3, [pc, #76]	; 7334 <cdbg_jump+0x68>
    72e4:	e5933000 	ldr	r3, [r3]
    72e8:	e3530000 	cmp	r3, #0	; 0x0
    72ec:	1a000006 	bne	730c <cdbg_jump+0x40>
    72f0:	e59f0040 	ldr	r0, [pc, #64]	; 7338 <cdbg_jump+0x6c>
    72f4:	e59f3040 	ldr	r3, [pc, #64]	; 733c <cdbg_jump+0x70>
    72f8:	e1a0e00f 	mov	lr, pc
    72fc:	e593f000 	ldr	pc, [r3]
    7300:	e3e00000 	mvn	r0, #0	; 0x0
    7304:	e28dd004 	add	sp, sp, #4	; 0x4
    7308:	e8bd8030 	pop	{r4, r5, pc}
    730c:	ebfffc56 	bl	646c <ss_lock_isk>
    7310:	e59f0028 	ldr	r0, [pc, #40]	; 7340 <cdbg_jump+0x74>
    7314:	e1a01004 	mov	r1, r4
    7318:	e59f301c 	ldr	r3, [pc, #28]	; 733c <cdbg_jump+0x70>
    731c:	e1a0e00f 	mov	lr, pc
    7320:	e593f000 	ldr	pc, [r3]
    7324:	e3000bb8 	movw	r0, #3000	; 0xbb8
    7328:	eb001cd8 	bl	e690 <udelay>
    732c:	e1a00005 	mov	r0, r5
    7330:	e12fff34 	blx	r4
    7334:	3c00a164 	.word	0x3c00a164
    7338:	00018284 	.word	0x00018284
    733c:	3c00a5cc 	.word	0x3c00a5cc
    7340:	0001829e 	.word	0x0001829e

00007344 <cdbg_restore_os>:
    7344:	e92d4010 	push	{r4, lr}
    7348:	ebffe714 	bl	fa0 <get_wakeup_entry>
    734c:	e59f301c 	ldr	r3, [pc, #28]	; 7370 <cdbg_restore_os+0x2c>
    7350:	e1a04000 	mov	r4, r0
    7354:	e59f0018 	ldr	r0, [pc, #24]	; 7374 <cdbg_restore_os+0x30>
    7358:	e1a0e00f 	mov	lr, pc
    735c:	e593f000 	ldr	pc, [r3]
    7360:	e1a00004 	mov	r0, r4
    7364:	e3a01000 	mov	r1, #0	; 0x0
    7368:	e8bd4010 	pop	{r4, lr}
    736c:	eaffffd6 	b	72cc <cdbg_jump>
    7370:	3c00a5cc 	.word	0x3c00a5cc
    7374:	000182ae 	.word	0x000182ae

00007378 <cdbg_get_pp>:
    7378:	e92d4070 	push	{r4, r5, r6, lr}
    737c:	e24dd008 	sub	sp, sp, #8	; 0x8
    7380:	e1a0000d 	mov	r0, sp
    7384:	e59f103c 	ldr	r1, [pc, #60]	; 73c8 <cdbg_get_pp+0x50>
    7388:	e3a02008 	mov	r2, #8	; 0x8
    738c:	ebffe834 	bl	1464 <memcpy>
    7390:	e1a0400d 	mov	r4, sp
    7394:	e1a0600d 	mov	r6, sp
    7398:	ea000004 	b	73b0 <cdbg_get_pp+0x38>
    739c:	eb002e8c 	bl	12dd4 <getc>
    73a0:	e1500004 	cmp	r0, r4
    73a4:	e2854001 	add	r4, r5, #1	; 0x1
    73a8:	e1a05006 	mov	r5, r6
    73ac:	1a000000 	bne	73b4 <cdbg_get_pp+0x3c>
    73b0:	e1a05004 	mov	r5, r4
    73b4:	e5d54000 	ldrb	r4, [r5]
    73b8:	e3540000 	cmp	r4, #0	; 0x0
    73bc:	1afffff6 	bne	739c <cdbg_get_pp+0x24>
    73c0:	e28dd008 	add	sp, sp, #8	; 0x8
    73c4:	e8bd8070 	pop	{r4, r5, r6, pc}
    73c8:	0001656e 	.word	0x0001656e

000073cc <cdbg_shell>:
    73cc:	e92d4010 	push	{r4, lr}
    73d0:	ebfffc25 	bl	646c <ss_lock_isk>
    73d4:	e59f3048 	ldr	r3, [pc, #72]	; 7424 <cdbg_shell+0x58>
    73d8:	e59f1048 	ldr	r1, [pc, #72]	; 7428 <cdbg_shell+0x5c>
    73dc:	e59f2048 	ldr	r2, [pc, #72]	; 742c <cdbg_shell+0x60>
    73e0:	e5031004 	str	r1, [r3, #-4]
    73e4:	e2833004 	add	r3, r3, #4	; 0x4
    73e8:	e1530002 	cmp	r3, r2
    73ec:	1afffffb 	bne	73e0 <cdbg_shell+0x14>
    73f0:	e3a00001 	mov	r0, #1	; 0x1
    73f4:	eb003650 	bl	14d3c <ss_jtag_en>
    73f8:	e59f4030 	ldr	r4, [pc, #48]	; 7430 <cdbg_shell+0x64>
    73fc:	e59f3030 	ldr	r3, [pc, #48]	; 7434 <cdbg_shell+0x68>
    7400:	e59f0030 	ldr	r0, [pc, #48]	; 7438 <cdbg_shell+0x6c>
    7404:	e5843000 	str	r3, [r4]
    7408:	eb002e53 	bl	12d5c <printf_on>
    740c:	ebffffd9 	bl	7378 <cdbg_get_pp>
    7410:	e59f0024 	ldr	r0, [pc, #36]	; 743c <cdbg_shell+0x70>
    7414:	e1a0e00f 	mov	lr, pc
    7418:	e594f000 	ldr	pc, [r4]
    741c:	eb003157 	bl	13980 <main_loop>
    7420:	eafffffd 	b	741c <cdbg_shell+0x50>
    7424:	3c000004 	.word	0x3c000004
    7428:	eafffffe 	.word	0xeafffffe
    742c:	3c008004 	.word	0x3c008004
    7430:	3c00a5cc 	.word	0x3c00a5cc
    7434:	00012d5c 	.word	0x00012d5c
    7438:	000182bd 	.word	0x000182bd
    743c:	000182c2 	.word	0x000182c2

00007440 <cdbg_do_config>:
    7440:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7444:	e3510000 	cmp	r1, #0	; 0x0
    7448:	e24dd008 	sub	sp, sp, #8	; 0x8
    744c:	e1a04000 	mov	r4, r0
    7450:	0a000034 	beq	7528 <cdbg_do_config+0xe8>
    7454:	e5901000 	ldr	r1, [r0]
    7458:	e59f31a4 	ldr	r3, [pc, #420]	; 7604 <cdbg_do_config+0x1c4>
    745c:	e1510003 	cmp	r1, r3
    7460:	0a000005 	beq	747c <cdbg_do_config+0x3c>
    7464:	e59f019c 	ldr	r0, [pc, #412]	; 7608 <cdbg_do_config+0x1c8>
    7468:	e59f319c 	ldr	r3, [pc, #412]	; 760c <cdbg_do_config+0x1cc>
    746c:	e1a0e00f 	mov	lr, pc
    7470:	e593f000 	ldr	pc, [r3]
    7474:	e3e00000 	mvn	r0, #0	; 0x0
    7478:	ea00005f 	b	75fc <cdbg_do_config+0x1bc>
    747c:	e3a06000 	mov	r6, #0	; 0x0
    7480:	e590500c 	ldr	r5, [r0, #12]
    7484:	e3a01050 	mov	r1, #80	; 0x50
    7488:	e580600c 	str	r6, [r0, #12]
    748c:	e1a00006 	mov	r0, r6
    7490:	ebfffd29 	bl	693c <hash_init>
    7494:	e28d8004 	add	r8, sp, #4	; 0x4
    7498:	e3a01050 	mov	r1, #80	; 0x50
    749c:	e1a00004 	mov	r0, r4
    74a0:	ebfffd05 	bl	68bc <hash_data>
    74a4:	e1a00008 	mov	r0, r8
    74a8:	ebfffce7 	bl	684c <hash_value>
    74ac:	e59d2004 	ldr	r2, [sp, #4]
    74b0:	e59f7154 	ldr	r7, [pc, #340]	; 760c <cdbg_do_config+0x1cc>
    74b4:	e1550002 	cmp	r5, r2
    74b8:	e584500c 	str	r5, [r4, #12]
    74bc:	0a000005 	beq	74d8 <cdbg_do_config+0x98>
    74c0:	e59f0148 	ldr	r0, [pc, #328]	; 7610 <cdbg_do_config+0x1d0>
    74c4:	e1a01005 	mov	r1, r5
    74c8:	e1a0e00f 	mov	lr, pc
    74cc:	e597f000 	ldr	pc, [r7]
    74d0:	e3e00001 	mvn	r0, #1	; 0x1
    74d4:	ea000048 	b	75fc <cdbg_do_config+0x1bc>
    74d8:	e59f0134 	ldr	r0, [pc, #308]	; 7614 <cdbg_do_config+0x1d4>
    74dc:	e1a0e00f 	mov	lr, pc
    74e0:	e597f000 	ldr	pc, [r7]
    74e4:	e5941004 	ldr	r1, [r4, #4]
    74e8:	e1a00006 	mov	r0, r6
    74ec:	ebfffd12 	bl	693c <hash_init>
    74f0:	e5941004 	ldr	r1, [r4, #4]
    74f4:	e2840050 	add	r0, r4, #80	; 0x50
    74f8:	ebfffcef 	bl	68bc <hash_data>
    74fc:	e1a00008 	mov	r0, r8
    7500:	ebfffcd1 	bl	684c <hash_value>
    7504:	e5941008 	ldr	r1, [r4, #8]
    7508:	e59d2004 	ldr	r2, [sp, #4]
    750c:	e1510002 	cmp	r1, r2
    7510:	0a000004 	beq	7528 <cdbg_do_config+0xe8>
    7514:	e59f00fc 	ldr	r0, [pc, #252]	; 7618 <cdbg_do_config+0x1d8>
    7518:	e1a0e00f 	mov	lr, pc
    751c:	e597f000 	ldr	pc, [r7]
    7520:	e3e00002 	mvn	r0, #2	; 0x2
    7524:	ea000034 	b	75fc <cdbg_do_config+0x1bc>
    7528:	e5940020 	ldr	r0, [r4, #32]
    752c:	e3500000 	cmp	r0, #0	; 0x0
    7530:	0a000000 	beq	7538 <cdbg_do_config+0xf8>
    7534:	eb001ecf 	bl	f078 <spi_set_bytes>
    7538:	e5940028 	ldr	r0, [r4, #40]
    753c:	e3700001 	cmn	r0, #1	; 0x1
    7540:	0a000000 	beq	7548 <cdbg_do_config+0x108>
    7544:	ebffe49a 	bl	7b4 <set_xom>
    7548:	e5940024 	ldr	r0, [r4, #36]
    754c:	e3700001 	cmn	r0, #1	; 0x1
    7550:	0a000000 	beq	7558 <cdbg_do_config+0x118>
    7554:	eb00291b 	bl	119c8 <set_mux>
    7558:	e594102c 	ldr	r1, [r4, #44]
    755c:	e3710001 	cmn	r1, #1	; 0x1
    7560:	159f20b4 	ldrne	r2, [pc, #180]	; 761c <cdbg_do_config+0x1dc>
    7564:	15943040 	ldrne	r3, [r4, #64]
    7568:	15821000 	strne	r1, [r2]
    756c:	159f20ac 	ldrne	r2, [pc, #172]	; 7620 <cdbg_do_config+0x1e0>
    7570:	15823000 	strne	r3, [r2]
    7574:	e5943010 	ldr	r3, [r4, #16]
    7578:	e3530000 	cmp	r3, #0	; 0x0
    757c:	0a000006 	beq	759c <cdbg_do_config+0x15c>
    7580:	e5940044 	ldr	r0, [r4, #68]
    7584:	e0840000 	add	r0, r4, r0
    7588:	ebffe4ef 	bl	94c <set_clk>
    758c:	e59f0090 	ldr	r0, [pc, #144]	; 7624 <cdbg_do_config+0x1e4>
    7590:	e59f3074 	ldr	r3, [pc, #116]	; 760c <cdbg_do_config+0x1cc>
    7594:	e1a0e00f 	mov	lr, pc
    7598:	e593f000 	ldr	pc, [r3]
    759c:	e5943014 	ldr	r3, [r4, #20]
    75a0:	e3530000 	cmp	r3, #0	; 0x0
    75a4:	0a000006 	beq	75c4 <cdbg_do_config+0x184>
    75a8:	e59f2078 	ldr	r2, [pc, #120]	; 7628 <cdbg_do_config+0x1e8>
    75ac:	e3a03001 	mov	r3, #1	; 0x1
    75b0:	e5823000 	str	r3, [r2]
    75b4:	e59f0070 	ldr	r0, [pc, #112]	; 762c <cdbg_do_config+0x1ec>
    75b8:	e59f304c 	ldr	r3, [pc, #76]	; 760c <cdbg_do_config+0x1cc>
    75bc:	e1a0e00f 	mov	lr, pc
    75c0:	e593f000 	ldr	pc, [r3]
    75c4:	e5943018 	ldr	r3, [r4, #24]
    75c8:	e3530000 	cmp	r3, #0	; 0x0
    75cc:	0a000009 	beq	75f8 <cdbg_do_config+0x1b8>
    75d0:	e594004c 	ldr	r0, [r4, #76]
    75d4:	e594101c 	ldr	r1, [r4, #28]
    75d8:	e0840000 	add	r0, r4, r0
    75dc:	eb0013bc 	bl	c4d4 <nand_rescan_config>
    75e0:	e3500000 	cmp	r0, #0	; 0x0
    75e4:	ba000003 	blt	75f8 <cdbg_do_config+0x1b8>
    75e8:	e59f0040 	ldr	r0, [pc, #64]	; 7630 <cdbg_do_config+0x1f0>
    75ec:	e59f3018 	ldr	r3, [pc, #24]	; 760c <cdbg_do_config+0x1cc>
    75f0:	e1a0e00f 	mov	lr, pc
    75f4:	e593f000 	ldr	pc, [r3]
    75f8:	e3a00000 	mov	r0, #0	; 0x0
    75fc:	e28dd008 	add	sp, sp, #8	; 0x8
    7600:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7604:	69780800 	.word	0x69780800
    7608:	000182d3 	.word	0x000182d3
    760c:	3c00a5cc 	.word	0x3c00a5cc
    7610:	000182ed 	.word	0x000182ed
    7614:	00018315 	.word	0x00018315
    7618:	00018332 	.word	0x00018332
    761c:	3c00a024 	.word	0x3c00a024
    7620:	3c00a0b4 	.word	0x3c00a0b4
    7624:	0001835a 	.word	0x0001835a
    7628:	3c00a164 	.word	0x3c00a164
    762c:	0001836c 	.word	0x0001836c
    7630:	0001837e 	.word	0x0001837e

00007634 <cdbg_config_isi>:
    7634:	e92d4030 	push	{r4, r5, lr}
    7638:	e24dd004 	sub	sp, sp, #4	; 0x4
    763c:	e1a05000 	mov	r5, r0
    7640:	ebffe94a 	bl	1b70 <isi_check>
    7644:	e2504000 	subs	r4, r0, #0	; 0x0
    7648:	0a000005 	beq	7664 <cdbg_config_isi+0x30>
    764c:	e59f0058 	ldr	r0, [pc, #88]	; 76ac <cdbg_config_isi+0x78>
    7650:	e1a01004 	mov	r1, r4
    7654:	e59f3054 	ldr	r3, [pc, #84]	; 76b0 <cdbg_config_isi+0x7c>
    7658:	e1a0e00f 	mov	lr, pc
    765c:	e593f000 	ldr	pc, [r3]
    7660:	ea00000e 	b	76a0 <cdbg_config_isi+0x6c>
    7664:	e5953004 	ldr	r3, [r5, #4]
    7668:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    766c:	e3530002 	cmp	r3, #2	; 0x2
    7670:	0a000005 	beq	768c <cdbg_config_isi+0x58>
    7674:	e59f0038 	ldr	r0, [pc, #56]	; 76b4 <cdbg_config_isi+0x80>
    7678:	e59f3030 	ldr	r3, [pc, #48]	; 76b0 <cdbg_config_isi+0x7c>
    767c:	e1a0e00f 	mov	lr, pc
    7680:	e593f000 	ldr	pc, [r3]
    7684:	e3e04022 	mvn	r4, #34	; 0x22
    7688:	ea000004 	b	76a0 <cdbg_config_isi+0x6c>
    768c:	e2850c02 	add	r0, r5, #512	; 0x200
    7690:	e3a01001 	mov	r1, #1	; 0x1
    7694:	e28dd004 	add	sp, sp, #4	; 0x4
    7698:	e8bd4030 	pop	{r4, r5, lr}
    769c:	eaffff67 	b	7440 <cdbg_do_config>
    76a0:	e1a00004 	mov	r0, r4
    76a4:	e28dd004 	add	sp, sp, #4	; 0x4
    76a8:	e8bd8030 	pop	{r4, r5, pc}
    76ac:	00018390 	.word	0x00018390
    76b0:	3c00a5cc 	.word	0x3c00a5cc
    76b4:	000183ab 	.word	0x000183ab

000076b8 <burn_image_loop>:
    76b8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    76bc:	e24dd04c 	sub	sp, sp, #76	; 0x4c
    76c0:	e59da074 	ldr	sl, [sp, #116]
    76c4:	e1a06000 	mov	r6, r0
    76c8:	e35a0000 	cmp	sl, #0	; 0x0
    76cc:	e58d102c 	str	r1, [sp, #44]
    76d0:	e58d2028 	str	r2, [sp, #40]
    76d4:	e1a0b003 	mov	fp, r3
    76d8:	1a00000c 	bne	7710 <burn_image_loop+0x58>
    76dc:	e59f44b0 	ldr	r4, [pc, #1200]	; 7b94 <burn_image_loop+0x4dc>
    76e0:	e59f04b0 	ldr	r0, [pc, #1200]	; 7b98 <burn_image_loop+0x4e0>
    76e4:	e3a0102d 	mov	r1, #45	; 0x2d
    76e8:	e1a0e00f 	mov	lr, pc
    76ec:	e594f000 	ldr	pc, [r4]
    76f0:	e59f04a4 	ldr	r0, [pc, #1188]	; 7b9c <burn_image_loop+0x4e4>
    76f4:	e59f14a4 	ldr	r1, [pc, #1188]	; 7ba0 <burn_image_loop+0x4e8>
    76f8:	e1a0e00f 	mov	lr, pc
    76fc:	e594f000 	ldr	pc, [r4]
    7700:	e59f049c 	ldr	r0, [pc, #1180]	; 7ba4 <burn_image_loop+0x4ec>
    7704:	e1a0e00f 	mov	lr, pc
    7708:	e594f000 	ldr	pc, [r4]
    770c:	eafffffe 	b	770c <burn_image_loop+0x54>
    7710:	e59d002c 	ldr	r0, [sp, #44]
    7714:	e1500003 	cmp	r0, r3
    7718:	1a000005 	bne	7734 <burn_image_loop+0x7c>
    771c:	e59f0484 	ldr	r0, [pc, #1156]	; 7ba8 <burn_image_loop+0x4f0>
    7720:	e59f346c 	ldr	r3, [pc, #1132]	; 7b94 <burn_image_loop+0x4dc>
    7724:	e1a0e00f 	mov	lr, pc
    7728:	e593f000 	ldr	pc, [r3]
    772c:	e3e00001 	mvn	r0, #1	; 0x1
    7730:	ea000115 	b	7b8c <burn_image_loop+0x4d4>
    7734:	e596c000 	ldr	ip, [r6]
    7738:	e3a03000 	mov	r3, #0	; 0x0
    773c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    7740:	e002200c 	and	r2, r2, ip
    7744:	e1a01483 	lsl	r1, r3, #9
    7748:	e1811ba2 	orr	r1, r1, r2, lsr #23
    774c:	e1a0e482 	lsl	lr, r2, #9
    7750:	e1a0200e 	mov	r2, lr
    7754:	e1a03001 	mov	r3, r1
    7758:	e1cd23f0 	strd	r2, [sp, #48]
    775c:	e1cd83d0 	ldrd	r8, [sp, #48]
    7760:	e1d600b8 	ldrh	r0, [r6, #8]
    7764:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7768:	e0988002 	adds	r8, r8, r2
    776c:	e5961004 	ldr	r1, [r6, #4]
    7770:	e3a03000 	mov	r3, #0	; 0x0
    7774:	e1a0cc2c 	lsr	ip, ip, #24
    7778:	e0a99003 	adc	r9, r9, r3
    777c:	e1a05000 	mov	r5, r0
    7780:	e3a04000 	mov	r4, #0	; 0x0
    7784:	e20cc03f 	and	ip, ip, #63	; 0x3f
    7788:	e1a0000b 	mov	r0, fp
    778c:	e1cd83f0 	strd	r8, [sp, #48]
    7790:	e58dc03c 	str	ip, [sp, #60]
    7794:	e1848001 	orr	r8, r4, r1
    7798:	e596700c 	ldr	r7, [r6, #12]
    779c:	ebfff9e0 	bl	5f24 <vs_device_erasable>
    77a0:	e1500004 	cmp	r0, r4
    77a4:	e1a09005 	mov	r9, r5
    77a8:	0a000006 	beq	77c8 <burn_image_loop+0x110>
    77ac:	e1a01004 	mov	r1, r4
    77b0:	e1a0000b 	mov	r0, fp
    77b4:	ebfffae6 	bl	6354 <vs_assign_by_id>
    77b8:	e1a00008 	mov	r0, r8
    77bc:	e1a01005 	mov	r1, r5
    77c0:	e59d2070 	ldr	r2, [sp, #112]
    77c4:	ebfff953 	bl	5d18 <vs_erase>
    77c8:	e59d002c 	ldr	r0, [sp, #44]
    77cc:	e3a01001 	mov	r1, #1	; 0x1
    77d0:	ebfffadf 	bl	6354 <vs_assign_by_id>
    77d4:	e59d003c 	ldr	r0, [sp, #60]
    77d8:	e2403002 	sub	r3, r0, #2	; 0x2
    77dc:	e3530001 	cmp	r3, #1	; 0x1
    77e0:	8a000018 	bhi	7848 <burn_image_loop+0x190>
    77e4:	e3a01c02 	mov	r1, #512	; 0x200
    77e8:	e58d1000 	str	r1, [sp]
    77ec:	e59d1028 	ldr	r1, [sp, #40]
    77f0:	e1cd23d0 	ldrd	r2, [sp, #48]
    77f4:	e1a0000a 	mov	r0, sl
    77f8:	e58d1004 	str	r1, [sp, #4]
    77fc:	ebfffa01 	bl	6008 <vs_read>
    7800:	e1a0000a 	mov	r0, sl
    7804:	ebffe87e 	bl	1a04 <isi_check_header>
    7808:	e2504000 	subs	r4, r0, #0	; 0x0
    780c:	1a00000d 	bne	7848 <burn_image_loop+0x190>
    7810:	e59a3004 	ldr	r3, [sl, #4]
    7814:	e1b03e23 	lsrs	r3, r3, #28
    7818:	0a00000a 	beq	7848 <burn_image_loop+0x190>
    781c:	e59f0388 	ldr	r0, [pc, #904]	; 7bac <burn_image_loop+0x4f4>
    7820:	e59f336c 	ldr	r3, [pc, #876]	; 7b94 <burn_image_loop+0x4dc>
    7824:	e1a0e00f 	mov	lr, pc
    7828:	e593f000 	ldr	pc, [r3]
    782c:	e59f237c 	ldr	r2, [pc, #892]	; 7bb0 <burn_image_loop+0x4f8>
    7830:	e1a01004 	mov	r1, r4
    7834:	e3a00001 	mov	r0, #1	; 0x1
    7838:	ebfffb26 	bl	64d8 <ss_aes_init>
    783c:	e3a02001 	mov	r2, #1	; 0x1
    7840:	e58d2044 	str	r2, [sp, #68]
    7844:	ea000001 	b	7850 <burn_image_loop+0x198>
    7848:	e3a03000 	mov	r3, #0	; 0x0
    784c:	e58d3044 	str	r3, [sp, #68]
    7850:	e59d003c 	ldr	r0, [sp, #60]
    7854:	e3500005 	cmp	r0, #5	; 0x5
    7858:	11a06007 	movne	r6, r7
    785c:	13a05901 	movne	r5, #16384	; 0x4000
    7860:	1a000010 	bne	78a8 <burn_image_loop+0x1f0>
    7864:	e1a0000b 	mov	r0, fp
    7868:	ebfff966 	bl	5e08 <vs_align>
    786c:	e1a04000 	mov	r4, r0
    7870:	e1a00007 	mov	r0, r7
    7874:	eb000de9 	bl	b020 <fnd_size_shrink>
    7878:	e1a06000 	mov	r6, r0
    787c:	e1a00007 	mov	r0, r7
    7880:	eb000dd8 	bl	afe8 <fnd_size_match>
    7884:	e3500000 	cmp	r0, #0	; 0x0
    7888:	11a05004 	movne	r5, r4
    788c:	1a000005 	bne	78a8 <burn_image_loop+0x1f0>
    7890:	e59f031c 	ldr	r0, [pc, #796]	; 7bb4 <burn_image_loop+0x4fc>
    7894:	e59f32f8 	ldr	r3, [pc, #760]	; 7b94 <burn_image_loop+0x4dc>
    7898:	e1a0e00f 	mov	lr, pc
    789c:	e593f000 	ldr	pc, [r3]
    78a0:	e3e00000 	mvn	r0, #0	; 0x0
    78a4:	ea0000b8 	b	7b8c <burn_image_loop+0x4d4>
    78a8:	e1a0000b 	mov	r0, fp
    78ac:	e59f1304 	ldr	r1, [pc, #772]	; 7bb8 <burn_image_loop+0x500>
    78b0:	ebfff9bb 	bl	5fa4 <vs_is_device>
    78b4:	e3500000 	cmp	r0, #0	; 0x0
    78b8:	1a00000a 	bne	78e8 <burn_image_loop+0x230>
    78bc:	e1a0000b 	mov	r0, fp
    78c0:	e59f12f4 	ldr	r1, [pc, #756]	; 7bbc <burn_image_loop+0x504>
    78c4:	ebfff9b6 	bl	5fa4 <vs_is_device>
    78c8:	e3500000 	cmp	r0, #0	; 0x0
    78cc:	1a000005 	bne	78e8 <burn_image_loop+0x230>
    78d0:	e1a0000b 	mov	r0, fp
    78d4:	e59f12e4 	ldr	r1, [pc, #740]	; 7bc0 <burn_image_loop+0x508>
    78d8:	ebfff9b1 	bl	5fa4 <vs_is_device>
    78dc:	e3500000 	cmp	r0, #0	; 0x0
    78e0:	01a07000 	moveq	r7, r0
    78e4:	0a00001a 	beq	7954 <burn_image_loop+0x29c>
    78e8:	e1a0000b 	mov	r0, fp
    78ec:	e59f12cc 	ldr	r1, [pc, #716]	; 7bc0 <burn_image_loop+0x508>
    78f0:	ebfff9ab 	bl	5fa4 <vs_is_device>
    78f4:	e3500000 	cmp	r0, #0	; 0x0
    78f8:	0a00000a 	beq	7928 <burn_image_loop+0x270>
    78fc:	e3a00000 	mov	r0, #0	; 0x0
    7900:	eb000d80 	bl	af08 <nand_get_config>
    7904:	e3500000 	cmp	r0, #0	; 0x0
    7908:	0a00000b 	beq	793c <burn_image_loop+0x284>
    790c:	e590301c 	ldr	r3, [r0, #28]
    7910:	e3570902 	cmp	r7, #32768	; 0x8000
    7914:	33a06902 	movcc	r6, #32768	; 0x8000
    7918:	e3530702 	cmp	r3, #524288	; 0x80000
    791c:	21a07003 	movcs	r7, r3
    7920:	33a07702 	movcc	r7, #524288	; 0x80000
    7924:	ea00000a 	b	7954 <burn_image_loop+0x29c>
    7928:	e3a00001 	mov	r0, #1	; 0x1
    792c:	eb000d75 	bl	af08 <nand_get_config>
    7930:	e3500000 	cmp	r0, #0	; 0x0
    7934:	1590701c 	ldrne	r7, [r0, #28]
    7938:	1a000005 	bne	7954 <burn_image_loop+0x29c>
    793c:	e59f0280 	ldr	r0, [pc, #640]	; 7bc4 <burn_image_loop+0x50c>
    7940:	e59f324c 	ldr	r3, [pc, #588]	; 7b94 <burn_image_loop+0x4dc>
    7944:	e1a0e00f 	mov	lr, pc
    7948:	e593f000 	ldr	pc, [r3]
    794c:	e3e00033 	mvn	r0, #51	; 0x33
    7950:	ea00008d 	b	7b8c <burn_image_loop+0x4d4>
    7954:	e59d1070 	ldr	r1, [sp, #112]
    7958:	e59f4234 	ldr	r4, [pc, #564]	; 7b94 <burn_image_loop+0x4dc>
    795c:	e59f0264 	ldr	r0, [pc, #612]	; 7bc8 <burn_image_loop+0x510>
    7960:	e1a02005 	mov	r2, r5
    7964:	e1a0e00f 	mov	lr, pc
    7968:	e594f000 	ldr	pc, [r4]
    796c:	e2473001 	sub	r3, r7, #1	; 0x1
    7970:	e1a00003 	mov	r0, r3
    7974:	e1a01fc0 	asr	r1, r0, #31
    7978:	e1cd02f0 	strd	r0, [sp, #32]
    797c:	e59d1070 	ldr	r1, [sp, #112]
    7980:	e1a02007 	mov	r2, r7
    7984:	e1a03fc2 	asr	r3, r2, #31
    7988:	e0661001 	rsb	r1, r6, r1
    798c:	e58d1040 	str	r1, [sp, #64]
    7990:	e1a00005 	mov	r0, r5
    7994:	e3a01000 	mov	r1, #0	; 0x0
    7998:	e58d6038 	str	r6, [sp, #56]
    799c:	e3a04000 	mov	r4, #0	; 0x0
    79a0:	e1cd21f8 	strd	r2, [sp, #24]
    79a4:	e1cd01f0 	strd	r0, [sp, #16]
    79a8:	ea000074 	b	7b80 <burn_image_loop+0x4c8>
    79ac:	e59d1038 	ldr	r1, [sp, #56]
    79b0:	e0663001 	rsb	r3, r6, r1
    79b4:	e1b03a23 	lsrs	r3, r3, #20
    79b8:	0a000005 	beq	79d4 <burn_image_loop+0x31c>
    79bc:	e59f0208 	ldr	r0, [pc, #520]	; 7bcc <burn_image_loop+0x514>
    79c0:	e1a01a26 	lsr	r1, r6, #20
    79c4:	e59f21c8 	ldr	r2, [pc, #456]	; 7b94 <burn_image_loop+0x4dc>
    79c8:	e1a0e00f 	mov	lr, pc
    79cc:	e592f000 	ldr	pc, [r2]
    79d0:	e58d6038 	str	r6, [sp, #56]
    79d4:	e3540000 	cmp	r4, #0	; 0x0
    79d8:	d3a00000 	movle	r0, #0	; 0x0
    79dc:	c3a00001 	movgt	r0, #1	; 0x1
    79e0:	e2844001 	add	r4, r4, #1	; 0x1
    79e4:	eb002501 	bl	10df0 <led_ctrl>
    79e8:	e3540032 	cmp	r4, #50	; 0x32
    79ec:	e3a01000 	mov	r1, #0	; 0x0
    79f0:	e59d002c 	ldr	r0, [sp, #44]
    79f4:	c2444064 	subgt	r4, r4, #100	; 0x64
    79f8:	ebfffa55 	bl	6354 <vs_assign_by_id>
    79fc:	e59d303c 	ldr	r3, [sp, #60]
    7a00:	e3530005 	cmp	r3, #5	; 0x5
    7a04:	11a01005 	movne	r1, r5
    7a08:	1a000001 	bne	7a14 <burn_image_loop+0x35c>
    7a0c:	eb000d6b 	bl	afc0 <fnd_align>
    7a10:	e1a01000 	mov	r1, r0
    7a14:	e58d1000 	str	r1, [sp]
    7a18:	e59d1028 	ldr	r1, [sp, #40]
    7a1c:	e1cd23d0 	ldrd	r2, [sp, #48]
    7a20:	e1a0000a 	mov	r0, sl
    7a24:	e58d1004 	str	r1, [sp, #4]
    7a28:	ebfff976 	bl	6008 <vs_read>
    7a2c:	e59d2044 	ldr	r2, [sp, #68]
    7a30:	e3520000 	cmp	r2, #0	; 0x0
    7a34:	0a000003 	beq	7a48 <burn_image_loop+0x390>
    7a38:	e1a0000a 	mov	r0, sl
    7a3c:	e1a0100a 	mov	r1, sl
    7a40:	e1a02005 	mov	r2, r5
    7a44:	ebfffa92 	bl	6494 <ss_aes_data>
    7a48:	e1a0000b 	mov	r0, fp
    7a4c:	e3a01000 	mov	r1, #0	; 0x0
    7a50:	ebfffa3f 	bl	6354 <vs_assign_by_id>
    7a54:	e3500000 	cmp	r0, #0	; 0x0
    7a58:	1a00002e 	bne	7b18 <burn_image_loop+0x460>
    7a5c:	e3570000 	cmp	r7, #0	; 0x0
    7a60:	0a00001f 	beq	7ae4 <burn_image_loop+0x42c>
    7a64:	e1cd02d0 	ldrd	r0, [sp, #32]
    7a68:	e0082000 	and	r2, r8, r0
    7a6c:	e0093001 	and	r3, r9, r1
    7a70:	e1921003 	orrs	r1, r2, r3
    7a74:	0a000012 	beq	7ac4 <burn_image_loop+0x40c>
    7a78:	ea000019 	b	7ae4 <burn_image_loop+0x42c>
    7a7c:	e59d1040 	ldr	r1, [sp, #64]
    7a80:	e1510007 	cmp	r1, r7
    7a84:	e0671001 	rsb	r1, r7, r1
    7a88:	e58d1040 	str	r1, [sp, #64]
    7a8c:	da000006 	ble	7aac <burn_image_loop+0x3f4>
    7a90:	e59f10fc 	ldr	r1, [pc, #252]	; 7b94 <burn_image_loop+0x4dc>
    7a94:	e1a0e00f 	mov	lr, pc
    7a98:	e591f000 	ldr	pc, [r1]
    7a9c:	e1cd21d8 	ldrd	r2, [sp, #24]
    7aa0:	e0988002 	adds	r8, r8, r2
    7aa4:	e0a99003 	adc	r9, r9, r3
    7aa8:	ea000005 	b	7ac4 <burn_image_loop+0x40c>
    7aac:	e59f011c 	ldr	r0, [pc, #284]	; 7bd0 <burn_image_loop+0x518>
    7ab0:	e59f30dc 	ldr	r3, [pc, #220]	; 7b94 <burn_image_loop+0x4dc>
    7ab4:	e1a0e00f 	mov	lr, pc
    7ab8:	e593f000 	ldr	pc, [r3]
    7abc:	e3e00034 	mvn	r0, #52	; 0x34
    7ac0:	ea000031 	b	7b8c <burn_image_loop+0x4d4>
    7ac4:	e1a00008 	mov	r0, r8
    7ac8:	e1a01009 	mov	r1, r9
    7acc:	ebfff8a0 	bl	5d54 <vs_isbad>
    7ad0:	e3500000 	cmp	r0, #0	; 0x0
    7ad4:	e1a02008 	mov	r2, r8
    7ad8:	e1a03009 	mov	r3, r9
    7adc:	e59f00f0 	ldr	r0, [pc, #240]	; 7bd4 <burn_image_loop+0x51c>
    7ae0:	1affffe5 	bne	7a7c <burn_image_loop+0x3c4>
    7ae4:	e59d303c 	ldr	r3, [sp, #60]
    7ae8:	e3530005 	cmp	r3, #5	; 0x5
    7aec:	11a01005 	movne	r1, r5
    7af0:	1a000001 	bne	7afc <burn_image_loop+0x444>
    7af4:	eb000d31 	bl	afc0 <fnd_align>
    7af8:	e1a01000 	mov	r1, r0
    7afc:	e58d1000 	str	r1, [sp]
    7b00:	e1a0000a 	mov	r0, sl
    7b04:	e3a01000 	mov	r1, #0	; 0x0
    7b08:	e1a02008 	mov	r2, r8
    7b0c:	e1a03009 	mov	r3, r9
    7b10:	e58d1004 	str	r1, [sp, #4]
    7b14:	ebfff869 	bl	5cc0 <vs_write>
    7b18:	e59d003c 	ldr	r0, [sp, #60]
    7b1c:	e3500001 	cmp	r0, #1	; 0x1
    7b20:	01a0e005 	moveq	lr, r5
    7b24:	01a0c005 	moveq	ip, r5
    7b28:	0a000006 	beq	7b48 <burn_image_loop+0x490>
    7b2c:	e59d103c 	ldr	r1, [sp, #60]
    7b30:	e3510005 	cmp	r1, #5	; 0x5
    7b34:	11a0e005 	movne	lr, r5
    7b38:	1a000001 	bne	7b44 <burn_image_loop+0x48c>
    7b3c:	eb000d1f 	bl	afc0 <fnd_align>
    7b40:	e1a0e000 	mov	lr, r0
    7b44:	e3a0c000 	mov	ip, #0	; 0x0
    7b48:	e1560005 	cmp	r6, r5
    7b4c:	31a02006 	movcc	r2, r6
    7b50:	21a02005 	movcs	r2, r5
    7b54:	e58d200c 	str	r2, [sp, #12]
    7b58:	e1cd23d0 	ldrd	r2, [sp, #48]
    7b5c:	e092000e 	adds	r0, r2, lr
    7b60:	e2a31000 	adc	r1, r3, #0	; 0x0
    7b64:	e1cd03f0 	strd	r0, [sp, #48]
    7b68:	e1cd01d0 	ldrd	r0, [sp, #16]
    7b6c:	e0988000 	adds	r8, r8, r0
    7b70:	e0a99001 	adc	r9, r9, r1
    7b74:	e59d100c 	ldr	r1, [sp, #12]
    7b78:	e08aa00c 	add	sl, sl, ip
    7b7c:	e0616006 	rsb	r6, r1, r6
    7b80:	e3560000 	cmp	r6, #0	; 0x0
    7b84:	1affff88 	bne	79ac <burn_image_loop+0x2f4>
    7b88:	e1a00006 	mov	r0, r6
    7b8c:	e28dd04c 	add	sp, sp, #76	; 0x4c
    7b90:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7b94:	3c00a5cc 	.word	0x3c00a5cc
    7b98:	00018185 	.word	0x00018185
    7b9c:	00016a6e 	.word	0x00016a6e
    7ba0:	000183be 	.word	0x000183be
    7ba4:	000183c5 	.word	0x000183c5
    7ba8:	000183db 	.word	0x000183db
    7bac:	000188ef 	.word	0x000188ef
    7bb0:	3c00a004 	.word	0x3c00a004
    7bb4:	00018408 	.word	0x00018408
    7bb8:	000178a7 	.word	0x000178a7
    7bbc:	000178a3 	.word	0x000178a3
    7bc0:	000166d1 	.word	0x000166d1
    7bc4:	00018425 	.word	0x00018425
    7bc8:	0001843d 	.word	0x0001843d
    7bcc:	0001844c 	.word	0x0001844c
    7bd0:	00018478 	.word	0x00018478
    7bd4:	00018459 	.word	0x00018459

00007bd8 <burn_images>:
    7bd8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7bdc:	e1a05000 	mov	r5, r0
    7be0:	e595301c 	ldr	r3, [r5, #28]
    7be4:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    7be8:	e3a00901 	mov	r0, #16384	; 0x4000
    7bec:	e58d1010 	str	r1, [sp, #16]
    7bf0:	e58d200c 	str	r2, [sp, #12]
    7bf4:	e58d3014 	str	r3, [sp, #20]
    7bf8:	ebffe67e 	bl	15f8 <malloc>
    7bfc:	e2509000 	subs	r9, r0, #0	; 0x0
    7c00:	e59f41ec 	ldr	r4, [pc, #492]	; 7df4 <burn_images+0x21c>
    7c04:	1a000004 	bne	7c1c <burn_images+0x44>
    7c08:	e59f01e8 	ldr	r0, [pc, #488]	; 7df8 <burn_images+0x220>
    7c0c:	e1a0e00f 	mov	lr, pc
    7c10:	e594f000 	ldr	pc, [r4]
    7c14:	e3e04000 	mvn	r4, #0	; 0x0
    7c18:	ea000070 	b	7de0 <burn_images+0x208>
    7c1c:	e59f01d8 	ldr	r0, [pc, #472]	; 7dfc <burn_images+0x224>
    7c20:	e59d1014 	ldr	r1, [sp, #20]
    7c24:	e3a07000 	mov	r7, #0	; 0x0
    7c28:	e1a0e00f 	mov	lr, pc
    7c2c:	e594f000 	ldr	pc, [r4]
    7c30:	e2856020 	add	r6, r5, #32	; 0x20
    7c34:	e1a0b004 	mov	fp, r4
    7c38:	e1a0a007 	mov	sl, r7
    7c3c:	ea000061 	b	7dc8 <burn_images+0x1f0>
    7c40:	e59f01b8 	ldr	r0, [pc, #440]	; 7e00 <burn_images+0x228>
    7c44:	e1a01007 	mov	r1, r7
    7c48:	e1a0e00f 	mov	lr, pc
    7c4c:	e59bf000 	ldr	pc, [fp]
    7c50:	e5d63003 	ldrb	r3, [r6, #3]
    7c54:	e5968008 	ldr	r8, [r6, #8]
    7c58:	e203503f 	and	r5, r3, #63	; 0x3f
    7c5c:	e3550003 	cmp	r5, #3	; 0x3
    7c60:	0a000010 	beq	7ca8 <burn_images+0xd0>
    7c64:	ca000006 	bgt	7c84 <burn_images+0xac>
    7c68:	e3550001 	cmp	r5, #1	; 0x1
    7c6c:	0a000013 	beq	7cc0 <burn_images+0xe8>
    7c70:	c1a04828 	lsrgt	r4, r8, #16
    7c74:	ca000026 	bgt	7d14 <burn_images+0x13c>
    7c78:	e3550000 	cmp	r5, #0	; 0x0
    7c7c:	0a00005a 	beq	7dec <burn_images+0x214>
    7c80:	ea00001f 	b	7d04 <burn_images+0x12c>
    7c84:	e3550005 	cmp	r5, #5	; 0x5
    7c88:	0a000008 	beq	7cb0 <burn_images+0xd8>
    7c8c:	b59f0170 	ldrlt	r0, [pc, #368]	; 7e04 <burn_images+0x22c>
    7c90:	ba000007 	blt	7cb4 <burn_images+0xdc>
    7c94:	e3550030 	cmp	r5, #48	; 0x30
    7c98:	0a00000e 	beq	7cd8 <burn_images+0x100>
    7c9c:	e3550031 	cmp	r5, #49	; 0x31
    7ca0:	1a000017 	bne	7d04 <burn_images+0x12c>
    7ca4:	ea000010 	b	7cec <burn_images+0x114>
    7ca8:	e59f0158 	ldr	r0, [pc, #344]	; 7e08 <burn_images+0x230>
    7cac:	ea000000 	b	7cb4 <burn_images+0xdc>
    7cb0:	e59f0154 	ldr	r0, [pc, #340]	; 7e0c <burn_images+0x234>
    7cb4:	ebfff8a6 	bl	5f54 <vs_device_id>
    7cb8:	e1a04000 	mov	r4, r0
    7cbc:	ea000014 	b	7d14 <burn_images+0x13c>
    7cc0:	e59f0148 	ldr	r0, [pc, #328]	; 7e10 <burn_images+0x238>
    7cc4:	e1a01008 	mov	r1, r8
    7cc8:	e1a0a008 	mov	sl, r8
    7ccc:	e1a0e00f 	mov	lr, pc
    7cd0:	e59bf000 	ldr	pc, [fp]
    7cd4:	ea000044 	b	7dec <burn_images+0x214>
    7cd8:	e59f0134 	ldr	r0, [pc, #308]	; 7e14 <burn_images+0x23c>
    7cdc:	e1a0e00f 	mov	lr, pc
    7ce0:	e59bf000 	ldr	pc, [fp]
    7ce4:	ebffe4b7 	bl	fc8 <system_reset>
    7ce8:	ea00003f 	b	7dec <burn_images+0x214>
    7cec:	e59f0124 	ldr	r0, [pc, #292]	; 7e18 <burn_images+0x240>
    7cf0:	e1a0e00f 	mov	lr, pc
    7cf4:	e59bf000 	ldr	pc, [fp]
    7cf8:	e3a00001 	mov	r0, #1	; 0x1
    7cfc:	ebffe22e 	bl	5bc <try_boot>
    7d00:	ea000039 	b	7dec <burn_images+0x214>
    7d04:	e59f0110 	ldr	r0, [pc, #272]	; 7e1c <burn_images+0x244>
    7d08:	e1a0e00f 	mov	lr, pc
    7d0c:	e59bf000 	ldr	pc, [fp]
    7d10:	ea00002a 	b	7dc0 <burn_images+0x1e8>
    7d14:	e354000f 	cmp	r4, #15	; 0xf
    7d18:	0a00000a 	beq	7d48 <burn_images+0x170>
    7d1c:	e1a00004 	mov	r0, r4
    7d20:	e3a01001 	mov	r1, #1	; 0x1
    7d24:	ebfff98a 	bl	6354 <vs_assign_by_id>
    7d28:	e3500000 	cmp	r0, #0	; 0x0
    7d2c:	0a000005 	beq	7d48 <burn_images+0x170>
    7d30:	e59f00e8 	ldr	r0, [pc, #232]	; 7e20 <burn_images+0x248>
    7d34:	e59f30b8 	ldr	r3, [pc, #184]	; 7df4 <burn_images+0x21c>
    7d38:	e1a0e00f 	mov	lr, pc
    7d3c:	e593f000 	ldr	pc, [r3]
    7d40:	e3e04002 	mvn	r4, #2	; 0x2
    7d44:	ea000023 	b	7dd8 <burn_images+0x200>
    7d48:	e3550001 	cmp	r5, #1	; 0x1
    7d4c:	e1a0c828 	lsr	ip, r8, #16
    7d50:	01a0e00a 	moveq	lr, sl
    7d54:	11a0e009 	movne	lr, r9
    7d58:	e1a0c80c 	lsl	ip, ip, #16
    7d5c:	e1a03004 	mov	r3, r4
    7d60:	e1a00006 	mov	r0, r6
    7d64:	e59d1010 	ldr	r1, [sp, #16]
    7d68:	e59d200c 	ldr	r2, [sp, #12]
    7d6c:	e88d5000 	stm	sp, {ip, lr}
    7d70:	ebfffe50 	bl	76b8 <burn_image_loop>
    7d74:	e3500000 	cmp	r0, #0	; 0x0
    7d78:	aa000006 	bge	7d98 <burn_images+0x1c0>
    7d7c:	e1a01007 	mov	r1, r7
    7d80:	e59f009c 	ldr	r0, [pc, #156]	; 7e24 <burn_images+0x24c>
    7d84:	e59f3068 	ldr	r3, [pc, #104]	; 7df4 <burn_images+0x21c>
    7d88:	e1a0e00f 	mov	lr, pc
    7d8c:	e593f000 	ldr	pc, [r3]
    7d90:	e3e04000 	mvn	r4, #0	; 0x0
    7d94:	ea00000f 	b	7dd8 <burn_images+0x200>
    7d98:	e3550001 	cmp	r5, #1	; 0x1
    7d9c:	1a000003 	bne	7db0 <burn_images+0x1d8>
    7da0:	e5960004 	ldr	r0, [r6, #4]
    7da4:	e3a01000 	mov	r1, #0	; 0x0
    7da8:	ebfffd47 	bl	72cc <cdbg_jump>
    7dac:	ea000003 	b	7dc0 <burn_images+0x1e8>
    7db0:	e3550000 	cmp	r5, #0	; 0x0
    7db4:	1a000001 	bne	7dc0 <burn_images+0x1e8>
    7db8:	e1a00009 	mov	r0, r9
    7dbc:	ebfffe1c 	bl	7634 <cdbg_config_isi>
    7dc0:	e2877001 	add	r7, r7, #1	; 0x1
    7dc4:	e2866010 	add	r6, r6, #16	; 0x10
    7dc8:	e59d3014 	ldr	r3, [sp, #20]
    7dcc:	e1570003 	cmp	r7, r3
    7dd0:	baffff9a 	blt	7c40 <burn_images+0x68>
    7dd4:	e3a04000 	mov	r4, #0	; 0x0
    7dd8:	e1a00009 	mov	r0, r9
    7ddc:	ebffe66d 	bl	1798 <free>
    7de0:	e1a00004 	mov	r0, r4
    7de4:	e28dd01c 	add	sp, sp, #28	; 0x1c
    7de8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7dec:	e3a0400f 	mov	r4, #15	; 0xf
    7df0:	eaffffd4 	b	7d48 <burn_images+0x170>
    7df4:	3c00a5cc 	.word	0x3c00a5cc
    7df8:	00018496 	.word	0x00018496
    7dfc:	000184b8 	.word	0x000184b8
    7e00:	000184d5 	.word	0x000184d5
    7e04:	000178a7 	.word	0x000178a7
    7e08:	000166d1 	.word	0x000166d1
    7e0c:	000178a3 	.word	0x000178a3
    7e10:	000184eb 	.word	0x000184eb
    7e14:	00018508 	.word	0x00018508
    7e18:	0001851d 	.word	0x0001851d
    7e1c:	00018530 	.word	0x00018530
    7e20:	00018544 	.word	0x00018544
    7e24:	00018561 	.word	0x00018561

00007e28 <burn>:
    7e28:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7e2c:	e1a09000 	mov	r9, r0
    7e30:	e24dd00c 	sub	sp, sp, #12	; 0xc
    7e34:	e3a00c02 	mov	r0, #512	; 0x200
    7e38:	ebffe5ee 	bl	15f8 <malloc>
    7e3c:	e2507000 	subs	r7, r0, #0	; 0x0
    7e40:	1a000004 	bne	7e58 <burn+0x30>
    7e44:	e59f01a0 	ldr	r0, [pc, #416]	; 7fec <burn+0x1c4>
    7e48:	e59f31a0 	ldr	r3, [pc, #416]	; 7ff0 <burn+0x1c8>
    7e4c:	e1a0e00f 	mov	lr, pc
    7e50:	e593f000 	ldr	pc, [r3]
    7e54:	ea000061 	b	7fe0 <burn+0x1b8>
    7e58:	ebffe26e 	bl	818 <boot_device>
    7e5c:	e3a05000 	mov	r5, #0	; 0x0
    7e60:	e3a0b001 	mov	fp, #1	; 0x1
    7e64:	e59f8184 	ldr	r8, [pc, #388]	; 7ff0 <burn+0x1c8>
    7e68:	e1a0a000 	mov	sl, r0
    7e6c:	ea00004d 	b	7fa8 <burn+0x180>
    7e70:	e1a00005 	mov	r0, r5
    7e74:	ebfff805 	bl	5e90 <vs_device_burnsrc>
    7e78:	e3500000 	cmp	r0, #0	; 0x0
    7e7c:	0a000048 	beq	7fa4 <burn+0x17c>
    7e80:	e155000a 	cmp	r5, sl
    7e84:	0a000046 	beq	7fa4 <burn+0x17c>
    7e88:	e019351b 	ands	r3, r9, fp, lsl r5
    7e8c:	1a000044 	bne	7fa4 <burn+0x17c>
    7e90:	ebffe45a 	bl	1000 <gtc_time>
    7e94:	e1a00005 	mov	r0, r5
    7e98:	e59f1154 	ldr	r1, [pc, #340]	; 7ff4 <burn+0x1cc>
    7e9c:	ebfff840 	bl	5fa4 <vs_is_device>
    7ea0:	e3500000 	cmp	r0, #0	; 0x0
    7ea4:	1a000004 	bne	7ebc <burn+0x94>
    7ea8:	e1a00005 	mov	r0, r5
    7eac:	e59f1144 	ldr	r1, [pc, #324]	; 7ff8 <burn+0x1d0>
    7eb0:	ebfff83b 	bl	5fa4 <vs_is_device>
    7eb4:	e3500000 	cmp	r0, #0	; 0x0
    7eb8:	0a00000e 	beq	7ef8 <burn+0xd0>
    7ebc:	e1a00005 	mov	r0, r5
    7ec0:	e3a01000 	mov	r1, #0	; 0x0
    7ec4:	ebfff922 	bl	6354 <vs_assign_by_id>
    7ec8:	e2506000 	subs	r6, r0, #0	; 0x0
    7ecc:	1a00000e 	bne	7f0c <burn+0xe4>
    7ed0:	ebfff7bd 	bl	5dcc <vs_special>
    7ed4:	e2506000 	subs	r6, r0, #0	; 0x0
    7ed8:	0a000038 	beq	7fc0 <burn+0x198>
    7edc:	e1a00005 	mov	r0, r5
    7ee0:	e5984000 	ldr	r4, [r8]
    7ee4:	ebfff807 	bl	5f08 <vs_device_name>
    7ee8:	e1a02006 	mov	r2, r6
    7eec:	e1a01000 	mov	r1, r0
    7ef0:	e59f0104 	ldr	r0, [pc, #260]	; 7ffc <burn+0x1d4>
    7ef4:	ea00000a 	b	7f24 <burn+0xfc>
    7ef8:	e1a00005 	mov	r0, r5
    7efc:	e3a01001 	mov	r1, #1	; 0x1
    7f00:	ebfff913 	bl	6354 <vs_assign_by_id>
    7f04:	e2506000 	subs	r6, r0, #0	; 0x0
    7f08:	0a000007 	beq	7f2c <burn+0x104>
    7f0c:	e1a00005 	mov	r0, r5
    7f10:	e5984000 	ldr	r4, [r8]
    7f14:	ebfff7fb 	bl	5f08 <vs_device_name>
    7f18:	e1a01000 	mov	r1, r0
    7f1c:	e59f00dc 	ldr	r0, [pc, #220]	; 8000 <burn+0x1d8>
    7f20:	e1a02006 	mov	r2, r6
    7f24:	e12fff34 	blx	r4
    7f28:	ea00001d 	b	7fa4 <burn+0x17c>
    7f2c:	e1a01007 	mov	r1, r7
    7f30:	e59f00cc 	ldr	r0, [pc, #204]	; 8004 <burn+0x1dc>
    7f34:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f38:	e3a03c02 	mov	r3, #512	; 0x200
    7f3c:	e58d6000 	str	r6, [sp]
    7f40:	e1a0e00f 	mov	lr, pc
    7f44:	e598f000 	ldr	pc, [r8]
    7f48:	e3a01c02 	mov	r1, #512	; 0x200
    7f4c:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f50:	e3a03000 	mov	r3, #0	; 0x0
    7f54:	e1a00007 	mov	r0, r7
    7f58:	e88d0042 	stm	sp, {r1, r6}
    7f5c:	ebfff829 	bl	6008 <vs_read>
    7f60:	e1a00007 	mov	r0, r7
    7f64:	eb002b16 	bl	12bc4 <ius_check_header>
    7f68:	e2504000 	subs	r4, r0, #0	; 0x0
    7f6c:	1a00000c 	bne	7fa4 <burn+0x17c>
    7f70:	e1a01005 	mov	r1, r5
    7f74:	e59f3074 	ldr	r3, [pc, #116]	; 7ff0 <burn+0x1c8>
    7f78:	e59f0088 	ldr	r0, [pc, #136]	; 8008 <burn+0x1e0>
    7f7c:	e1a0e00f 	mov	lr, pc
    7f80:	e593f000 	ldr	pc, [r3]
    7f84:	e1a00004 	mov	r0, r4
    7f88:	e1a01004 	mov	r1, r4
    7f8c:	ebfffcc3 	bl	72a0 <cdbg_verify_burn_enable>
    7f90:	e1a00007 	mov	r0, r7
    7f94:	e1a01005 	mov	r1, r5
    7f98:	e1a02004 	mov	r2, r4
    7f9c:	ebffff0d 	bl	7bd8 <burn_images>
    7fa0:	ea000006 	b	7fc0 <burn+0x198>
    7fa4:	e2855001 	add	r5, r5, #1	; 0x1
    7fa8:	ebfff7e7 	bl	5f4c <vs_device_count>
    7fac:	e1550000 	cmp	r5, r0
    7fb0:	baffffae 	blt	7e70 <burn+0x48>
    7fb4:	e1a00007 	mov	r0, r7
    7fb8:	ebffe5f6 	bl	1798 <free>
    7fbc:	ea000007 	b	7fe0 <burn+0x1b8>
    7fc0:	e59f0044 	ldr	r0, [pc, #68]	; 800c <burn+0x1e4>
    7fc4:	e59f3024 	ldr	r3, [pc, #36]	; 7ff0 <burn+0x1c8>
    7fc8:	e1a0e00f 	mov	lr, pc
    7fcc:	e593f000 	ldr	pc, [r3]
    7fd0:	e1a00007 	mov	r0, r7
    7fd4:	ebffe5ef 	bl	1798 <free>
    7fd8:	ebfffcfb 	bl	73cc <cdbg_shell>
    7fdc:	eafffffe 	b	7fdc <burn+0x1b4>
    7fe0:	e3e00000 	mvn	r0, #0	; 0x0
    7fe4:	e28dd00c 	add	sp, sp, #12	; 0xc
    7fe8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7fec:	00018579 	.word	0x00018579
    7ff0:	3c00a5cc 	.word	0x3c00a5cc
    7ff4:	00017fee 	.word	0x00017fee
    7ff8:	00017fea 	.word	0x00017fea
    7ffc:	00018599 	.word	0x00018599
    8000:	000185b6 	.word	0x000185b6
    8004:	000185d5 	.word	0x000185d5
    8008:	00018605 	.word	0x00018605
    800c:	0001861d 	.word	0x0001861d

00008010 <eth_vs_align>:
    8010:	e3a00b01 	mov	r0, #1024	; 0x400
    8014:	e12fff1e 	bx	lr

00008018 <eth_vs_reset>:
    8018:	e92d4010 	push	{r4, lr}
    801c:	e3a00001 	mov	r0, #1	; 0x1
    8020:	eb000065 	bl	81bc <iuw_set_dev>
    8024:	eb000380 	bl	8e2c <iuw_connect>
    8028:	e2504000 	subs	r4, r0, #0	; 0x0
    802c:	0a000004 	beq	8044 <eth_vs_reset+0x2c>
    8030:	e59f0014 	ldr	r0, [pc, #20]	; 804c <eth_vs_reset+0x34>
    8034:	e1a01004 	mov	r1, r4
    8038:	e59f3010 	ldr	r3, [pc, #16]	; 8050 <eth_vs_reset+0x38>
    803c:	e1a0e00f 	mov	lr, pc
    8040:	e593f000 	ldr	pc, [r3]
    8044:	e1a00004 	mov	r0, r4
    8048:	e8bd8010 	pop	{r4, pc}
    804c:	00018640 	.word	0x00018640
    8050:	3c00a5cc 	.word	0x3c00a5cc

00008054 <eth_vs_special>:
    8054:	e92d4010 	push	{r4, lr}
    8058:	e3a00001 	mov	r0, #1	; 0x1
    805c:	eb000056 	bl	81bc <iuw_set_dev>
    8060:	eb000371 	bl	8e2c <iuw_connect>
    8064:	e2504000 	subs	r4, r0, #0	; 0x0
    8068:	e59f3034 	ldr	r3, [pc, #52]	; 80a4 <eth_vs_special+0x50>
    806c:	0a000004 	beq	8084 <eth_vs_special+0x30>
    8070:	e59f0030 	ldr	r0, [pc, #48]	; 80a8 <eth_vs_special+0x54>
    8074:	e1a01004 	mov	r1, r4
    8078:	e1a0e00f 	mov	lr, pc
    807c:	e593f000 	ldr	pc, [r3]
    8080:	ea000005 	b	809c <eth_vs_special+0x48>
    8084:	e59f0020 	ldr	r0, [pc, #32]	; 80ac <eth_vs_special+0x58>
    8088:	e1a0e00f 	mov	lr, pc
    808c:	e593f000 	ldr	pc, [r3]
    8090:	eb0000fb 	bl	8484 <iuw_server>
    8094:	e1a04000 	mov	r4, r0
    8098:	eb00035b 	bl	8e0c <iuw_close>
    809c:	e1a00004 	mov	r0, r4
    80a0:	e8bd8010 	pop	{r4, pc}
    80a4:	3c00a5cc 	.word	0x3c00a5cc
    80a8:	00018640 	.word	0x00018640
    80ac:	00018666 	.word	0x00018666

000080b0 <udc_vs_align>:
    80b0:	e3a00b01 	mov	r0, #1024	; 0x400
    80b4:	e12fff1e 	bx	lr

000080b8 <udc_vs_reset>:
    80b8:	e92d4010 	push	{r4, lr}
    80bc:	e3a00000 	mov	r0, #0	; 0x0
    80c0:	eb00003d 	bl	81bc <iuw_set_dev>
    80c4:	eb000358 	bl	8e2c <iuw_connect>
    80c8:	e2504000 	subs	r4, r0, #0	; 0x0
    80cc:	0a000004 	beq	80e4 <udc_vs_reset+0x2c>
    80d0:	e59f0014 	ldr	r0, [pc, #20]	; 80ec <udc_vs_reset+0x34>
    80d4:	e1a01004 	mov	r1, r4
    80d8:	e59f3010 	ldr	r3, [pc, #16]	; 80f0 <udc_vs_reset+0x38>
    80dc:	e1a0e00f 	mov	lr, pc
    80e0:	e593f000 	ldr	pc, [r3]
    80e4:	e1a00004 	mov	r0, r4
    80e8:	e8bd8010 	pop	{r4, pc}
    80ec:	00018685 	.word	0x00018685
    80f0:	3c00a5cc 	.word	0x3c00a5cc

000080f4 <udc_vs_special>:
    80f4:	e92d4010 	push	{r4, lr}
    80f8:	e3a00000 	mov	r0, #0	; 0x0
    80fc:	eb00002e 	bl	81bc <iuw_set_dev>
    8100:	eb000349 	bl	8e2c <iuw_connect>
    8104:	e2504000 	subs	r4, r0, #0	; 0x0
    8108:	e59f3034 	ldr	r3, [pc, #52]	; 8144 <udc_vs_special+0x50>
    810c:	0a000004 	beq	8124 <udc_vs_special+0x30>
    8110:	e59f0030 	ldr	r0, [pc, #48]	; 8148 <udc_vs_special+0x54>
    8114:	e1a01004 	mov	r1, r4
    8118:	e1a0e00f 	mov	lr, pc
    811c:	e593f000 	ldr	pc, [r3]
    8120:	ea000005 	b	813c <udc_vs_special+0x48>
    8124:	e59f0020 	ldr	r0, [pc, #32]	; 814c <udc_vs_special+0x58>
    8128:	e1a0e00f 	mov	lr, pc
    812c:	e593f000 	ldr	pc, [r3]
    8130:	eb0000d3 	bl	8484 <iuw_server>
    8134:	e1a04000 	mov	r4, r0
    8138:	eb000333 	bl	8e0c <iuw_close>
    813c:	e1a00004 	mov	r0, r4
    8140:	e8bd8010 	pop	{r4, pc}
    8144:	3c00a5cc 	.word	0x3c00a5cc
    8148:	00018685 	.word	0x00018685
    814c:	00018666 	.word	0x00018666

00008150 <iuw_is_cmd>:
    8150:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8154:	e5d03003 	ldrb	r3, [r0, #3]
    8158:	e24dd004 	sub	sp, sp, #4	; 0x4
    815c:	e35300aa 	cmp	r3, #170	; 0xaa
    8160:	159f0048 	ldrne	r0, [pc, #72]	; 81b0 <iuw_is_cmd+0x60>
    8164:	1a00000b 	bne	8198 <iuw_is_cmd+0x48>
    8168:	e3a02000 	mov	r2, #0	; 0x0
    816c:	e1a01002 	mov	r1, r2
    8170:	e7d03002 	ldrb	r3, [r0, r2]
    8174:	e2822001 	add	r2, r2, #1	; 0x1
    8178:	e352001c 	cmp	r2, #28	; 0x1c
    817c:	e0811003 	add	r1, r1, r3
    8180:	1afffffa 	bne	8170 <iuw_is_cmd+0x20>
    8184:	e590301c 	ldr	r3, [r0, #28]
    8188:	e1510003 	cmp	r1, r3
    818c:	03a00001 	moveq	r0, #1	; 0x1
    8190:	0a000004 	beq	81a8 <iuw_is_cmd+0x58>
    8194:	e59f0018 	ldr	r0, [pc, #24]	; 81b4 <iuw_is_cmd+0x64>
    8198:	e59f3018 	ldr	r3, [pc, #24]	; 81b8 <iuw_is_cmd+0x68>
    819c:	e1a0e00f 	mov	lr, pc
    81a0:	e593f000 	ldr	pc, [r3]
    81a4:	e3a00000 	mov	r0, #0	; 0x0
    81a8:	e28dd004 	add	sp, sp, #4	; 0x4
    81ac:	e8bd8000 	pop	{pc}
    81b0:	000186a6 	.word	0x000186a6
    81b4:	000186c3 	.word	0x000186c3
    81b8:	3c00a5cc 	.word	0x3c00a5cc

000081bc <iuw_set_dev>:
    81bc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    81c0:	e3500001 	cmp	r0, #1	; 0x1
    81c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    81c8:	e59f2048 	ldr	r2, [pc, #72]	; 8218 <iuw_set_dev+0x5c>
    81cc:	8a00000b 	bhi	8200 <iuw_set_dev+0x44>
    81d0:	e3a03003 	mov	r3, #3	; 0x3
    81d4:	e0030093 	mul	r3, r3, r0
    81d8:	e59f103c 	ldr	r1, [pc, #60]	; 821c <iuw_set_dev+0x60>
    81dc:	e0811183 	add	r1, r1, r3, lsl #3
    81e0:	e59f3038 	ldr	r3, [pc, #56]	; 8220 <iuw_set_dev+0x64>
    81e4:	e281100c 	add	r1, r1, #12	; 0xc
    81e8:	e5830000 	str	r0, [r3]
    81ec:	e59f0030 	ldr	r0, [pc, #48]	; 8224 <iuw_set_dev+0x68>
    81f0:	e1a0e00f 	mov	lr, pc
    81f4:	e592f000 	ldr	pc, [r2]
    81f8:	e3a00000 	mov	r0, #0	; 0x0
    81fc:	ea000003 	b	8210 <iuw_set_dev+0x54>
    8200:	e59f0020 	ldr	r0, [pc, #32]	; 8228 <iuw_set_dev+0x6c>
    8204:	e1a0e00f 	mov	lr, pc
    8208:	e592f000 	ldr	pc, [r2]
    820c:	e3e00000 	mvn	r0, #0	; 0x0
    8210:	e28dd004 	add	sp, sp, #4	; 0x4
    8214:	e8bd8000 	pop	{pc}
    8218:	3c00a5cc 	.word	0x3c00a5cc
    821c:	000162bc 	.word	0x000162bc
    8220:	3c00a0e0 	.word	0x3c00a0e0
    8224:	000186e3 	.word	0x000186e3
    8228:	00018703 	.word	0x00018703

0000822c <init_iuw>:
    822c:	e59f300c 	ldr	r3, [pc, #12]	; 8240 <init_iuw+0x14>
    8230:	e3a02001 	mov	r2, #1	; 0x1
    8234:	e5832000 	str	r2, [r3]
    8238:	e3a00000 	mov	r0, #0	; 0x0
    823c:	e12fff1e 	bx	lr
    8240:	3c00a0e0 	.word	0x3c00a0e0

00008244 <iuw_local_rw>:
    8244:	e92d4010 	push	{r4, lr}
    8248:	e59d100c 	ldr	r1, [sp, #12]
    824c:	e1a04003 	mov	r4, r3
    8250:	e3510015 	cmp	r1, #21	; 0x15
    8254:	e1a03000 	mov	r3, r0
    8258:	e59dc008 	ldr	ip, [sp, #8]
    825c:	0a00001c 	beq	82d4 <iuw_local_rw+0x90>
    8260:	ca00000c 	bgt	8298 <iuw_local_rw+0x54>
    8264:	e3510012 	cmp	r1, #18	; 0x12
    8268:	0a000020 	beq	82f0 <iuw_local_rw+0xac>
    826c:	ca000004 	bgt	8284 <iuw_local_rw+0x40>
    8270:	e3510010 	cmp	r1, #16	; 0x10
    8274:	0a00001d 	beq	82f0 <iuw_local_rw+0xac>
    8278:	e3510011 	cmp	r1, #17	; 0x11
    827c:	1a000031 	bne	8348 <iuw_local_rw+0x104>
    8280:	ea000013 	b	82d4 <iuw_local_rw+0x90>
    8284:	e3510013 	cmp	r1, #19	; 0x13
    8288:	0a000011 	beq	82d4 <iuw_local_rw+0x90>
    828c:	e3510014 	cmp	r1, #20	; 0x14
    8290:	1a00002c 	bne	8348 <iuw_local_rw+0x104>
    8294:	ea000015 	b	82f0 <iuw_local_rw+0xac>
    8298:	e3510021 	cmp	r1, #33	; 0x21
    829c:	0a00001a 	beq	830c <iuw_local_rw+0xc8>
    82a0:	ca000004 	bgt	82b8 <iuw_local_rw+0x74>
    82a4:	e3510016 	cmp	r1, #22	; 0x16
    82a8:	0a000010 	beq	82f0 <iuw_local_rw+0xac>
    82ac:	e3510020 	cmp	r1, #32	; 0x20
    82b0:	1a000024 	bne	8348 <iuw_local_rw+0x104>
    82b4:	ea000016 	b	8314 <iuw_local_rw+0xd0>
    82b8:	e3510044 	cmp	r1, #68	; 0x44
    82bc:	0a00001c 	beq	8334 <iuw_local_rw+0xf0>
    82c0:	e3510046 	cmp	r1, #70	; 0x46
    82c4:	0a000017 	beq	8328 <iuw_local_rw+0xe4>
    82c8:	e3510042 	cmp	r1, #66	; 0x42
    82cc:	1a00001d 	bne	8348 <iuw_local_rw+0x104>
    82d0:	ea000017 	b	8334 <iuw_local_rw+0xf0>
    82d4:	e1a00003 	mov	r0, r3
    82d8:	e3a01000 	mov	r1, #0	; 0x0
    82dc:	e1a03004 	mov	r3, r4
    82e0:	e58dc008 	str	ip, [sp, #8]
    82e4:	e58d100c 	str	r1, [sp, #12]
    82e8:	e8bd4010 	pop	{r4, lr}
    82ec:	eafff745 	b	6008 <vs_read>
    82f0:	e1a00003 	mov	r0, r3
    82f4:	e3a01000 	mov	r1, #0	; 0x0
    82f8:	e1a03004 	mov	r3, r4
    82fc:	e58dc008 	str	ip, [sp, #8]
    8300:	e58d100c 	str	r1, [sp, #12]
    8304:	e8bd4010 	pop	{r4, lr}
    8308:	eafff66c 	b	5cc0 <vs_write>
    830c:	e1a01002 	mov	r1, r2
    8310:	ea000001 	b	831c <iuw_local_rw+0xd8>
    8314:	e1a00002 	mov	r0, r2
    8318:	e1a01003 	mov	r1, r3
    831c:	e1a0200c 	mov	r2, ip
    8320:	ebffe44f 	bl	1464 <memcpy>
    8324:	ea00000b 	b	8358 <iuw_local_rw+0x114>
    8328:	e1a0100c 	mov	r1, ip
    832c:	ebfff962 	bl	68bc <hash_data>
    8330:	ea000008 	b	8358 <iuw_local_rw+0x114>
    8334:	e1a00003 	mov	r0, r3
    8338:	e1a0200c 	mov	r2, ip
    833c:	e2831a02 	add	r1, r3, #8192	; 0x2000
    8340:	ebfff853 	bl	6494 <ss_aes_data>
    8344:	ea000003 	b	8358 <iuw_local_rw+0x114>
    8348:	e59f0010 	ldr	r0, [pc, #16]	; 8360 <iuw_local_rw+0x11c>
    834c:	e59f3010 	ldr	r3, [pc, #16]	; 8364 <iuw_local_rw+0x120>
    8350:	e1a0e00f 	mov	lr, pc
    8354:	e593f000 	ldr	pc, [r3]
    8358:	e3a00000 	mov	r0, #0	; 0x0
    835c:	e8bd8010 	pop	{r4, pc}
    8360:	00018719 	.word	0x00018719
    8364:	3c00a5cc 	.word	0x3c00a5cc

00008368 <iuw_send_cmd>:
    8368:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    836c:	e59f3084 	ldr	r3, [pc, #132]	; 83f8 <iuw_send_cmd+0x90>
    8370:	e1a04000 	mov	r4, r0
    8374:	e5937000 	ldr	r7, [r3]
    8378:	e1a05001 	mov	r5, r1
    837c:	e1a06002 	mov	r6, r2
    8380:	e3a01000 	mov	r1, #0	; 0x0
    8384:	e3a02020 	mov	r2, #32	; 0x20
    8388:	e24dd004 	sub	sp, sp, #4	; 0x4
    838c:	e1a00007 	mov	r0, r7
    8390:	ebffe42b 	bl	1444 <memset>
    8394:	e38444aa 	orr	r4, r4, #-1442840576	; 0xaa000000
    8398:	e3a02000 	mov	r2, #0	; 0x0
    839c:	e8870070 	stm	r7, {r4, r5, r6}
    83a0:	e1a01002 	mov	r1, r2
    83a4:	e7d73002 	ldrb	r3, [r7, r2]
    83a8:	e2822001 	add	r2, r2, #1	; 0x1
    83ac:	e352001c 	cmp	r2, #28	; 0x1c
    83b0:	e0811003 	add	r1, r1, r3
    83b4:	1afffffa 	bne	83a4 <iuw_send_cmd+0x3c>
    83b8:	e59f303c 	ldr	r3, [pc, #60]	; 83fc <iuw_send_cmd+0x94>
    83bc:	e587101c 	str	r1, [r7, #28]
    83c0:	e5932000 	ldr	r2, [r3]
    83c4:	e59f1034 	ldr	r1, [pc, #52]	; 8400 <iuw_send_cmd+0x98>
    83c8:	e3a03018 	mov	r3, #24	; 0x18
    83cc:	e0221293 	mla	r2, r3, r2, r1
    83d0:	e59f3020 	ldr	r3, [pc, #32]	; 83f8 <iuw_send_cmd+0x90>
    83d4:	e3a01020 	mov	r1, #32	; 0x20
    83d8:	e5930000 	ldr	r0, [r3]
    83dc:	e1a0e00f 	mov	lr, pc
    83e0:	e592f008 	ldr	pc, [r2, #8]
    83e4:	e3500000 	cmp	r0, #0	; 0x0
    83e8:	b3e00000 	mvnlt	r0, #0	; 0x0
    83ec:	a3a00000 	movge	r0, #0	; 0x0
    83f0:	e28dd004 	add	sp, sp, #4	; 0x4
    83f4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    83f8:	3c00a0dc 	.word	0x3c00a0dc
    83fc:	3c00a0e0 	.word	0x3c00a0e0
    8400:	000162bc 	.word	0x000162bc

00008404 <iuw_shake_hand>:
    8404:	e92d4010 	push	{r4, lr}
    8408:	e1a02001 	mov	r2, r1
    840c:	e1a01000 	mov	r1, r0
    8410:	e3a00002 	mov	r0, #2	; 0x2
    8414:	ebffffd3 	bl	8368 <iuw_send_cmd>
    8418:	e59f3058 	ldr	r3, [pc, #88]	; 8478 <iuw_shake_hand+0x74>
    841c:	e59f1058 	ldr	r1, [pc, #88]	; 847c <iuw_shake_hand+0x78>
    8420:	e5932000 	ldr	r2, [r3]
    8424:	e59f4054 	ldr	r4, [pc, #84]	; 8480 <iuw_shake_hand+0x7c>
    8428:	e3a03018 	mov	r3, #24	; 0x18
    842c:	e0221293 	mla	r2, r3, r2, r1
    8430:	e5940000 	ldr	r0, [r4]
    8434:	e3a01020 	mov	r1, #32	; 0x20
    8438:	e1a0e00f 	mov	lr, pc
    843c:	e592f004 	ldr	pc, [r2, #4]
    8440:	e3500000 	cmp	r0, #0	; 0x0
    8444:	b8bd8010 	poplt	{r4, pc}
    8448:	e5940000 	ldr	r0, [r4]
    844c:	ebffff3f 	bl	8150 <iuw_is_cmd>
    8450:	e3500000 	cmp	r0, #0	; 0x0
    8454:	0a000005 	beq	8470 <iuw_shake_hand+0x6c>
    8458:	e5943000 	ldr	r3, [r4]
    845c:	e5933000 	ldr	r3, [r3]
    8460:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    8464:	e3530002 	cmp	r3, #2	; 0x2
    8468:	03a00000 	moveq	r0, #0	; 0x0
    846c:	08bd8010 	popeq	{r4, pc}
    8470:	e3e00000 	mvn	r0, #0	; 0x0
    8474:	e8bd8010 	pop	{r4, pc}
    8478:	3c00a0e0 	.word	0x3c00a0e0
    847c:	000162bc 	.word	0x000162bc
    8480:	3c00a0dc 	.word	0x3c00a0dc

00008484 <iuw_server>:
    8484:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8488:	e3a00000 	mov	r0, #0	; 0x0
    848c:	e24dd044 	sub	sp, sp, #68	; 0x44
    8490:	e58d0018 	str	r0, [sp, #24]
    8494:	ea000002 	b	84a4 <iuw_server+0x20>
    8498:	e59d1018 	ldr	r1, [sp, #24]
    849c:	e2811001 	add	r1, r1, #1	; 0x1
    84a0:	e58d1018 	str	r1, [sp, #24]
    84a4:	e59f38e0 	ldr	r3, [pc, #2272]	; 8d8c <iuw_server+0x908>
    84a8:	e59f08e0 	ldr	r0, [pc, #2272]	; 8d90 <iuw_server+0x90c>
    84ac:	e5932000 	ldr	r2, [r3]
    84b0:	e59f18dc 	ldr	r1, [pc, #2268]	; 8d94 <iuw_server+0x910>
    84b4:	e3a03018 	mov	r3, #24	; 0x18
    84b8:	e0220293 	mla	r2, r3, r2, r0
    84bc:	e5910000 	ldr	r0, [r1]
    84c0:	e3a01020 	mov	r1, #32	; 0x20
    84c4:	e1a0e00f 	mov	lr, pc
    84c8:	e592f004 	ldr	pc, [r2, #4]
    84cc:	e3500000 	cmp	r0, #0	; 0x0
    84d0:	aa000009 	bge	84fc <iuw_server+0x78>
    84d4:	e1a01000 	mov	r1, r0
    84d8:	e59f28b8 	ldr	r2, [pc, #2232]	; 8d98 <iuw_server+0x914>
    84dc:	e59f08b8 	ldr	r0, [pc, #2232]	; 8d9c <iuw_server+0x918>
    84e0:	e1a0e00f 	mov	lr, pc
    84e4:	e592f000 	ldr	pc, [r2]
    84e8:	e59d3018 	ldr	r3, [sp, #24]
    84ec:	e3530063 	cmp	r3, #99	; 0x63
    84f0:	daffffe8 	ble	8498 <iuw_server+0x14>
    84f4:	e59f08a4 	ldr	r0, [pc, #2212]	; 8da0 <iuw_server+0x91c>
    84f8:	ea000045 	b	8614 <iuw_server+0x190>
    84fc:	e59f1890 	ldr	r1, [pc, #2192]	; 8d94 <iuw_server+0x910>
    8500:	e5910000 	ldr	r0, [r1]
    8504:	ebffff11 	bl	8150 <iuw_is_cmd>
    8508:	e3500000 	cmp	r0, #0	; 0x0
    850c:	1a000003 	bne	8520 <iuw_server+0x9c>
    8510:	e59f088c 	ldr	r0, [pc, #2188]	; 8da4 <iuw_server+0x920>
    8514:	e59f287c 	ldr	r2, [pc, #2172]	; 8d98 <iuw_server+0x914>
    8518:	e1a0e00f 	mov	lr, pc
    851c:	e592f000 	ldr	pc, [r2]
    8520:	e59f086c 	ldr	r0, [pc, #2156]	; 8d94 <iuw_server+0x910>
    8524:	e5903000 	ldr	r3, [r0]
    8528:	e5933000 	ldr	r3, [r3]
    852c:	e3c344ff 	bic	r4, r3, #-16777216	; 0xff000000
    8530:	e3540033 	cmp	r4, #51	; 0x33
    8534:	0a00000c 	beq	856c <iuw_server+0xe8>
    8538:	ca000002 	bgt	8548 <iuw_server+0xc4>
    853c:	e3540002 	cmp	r4, #2	; 0x2
    8540:	1a00003b 	bne	8634 <iuw_server+0x1b0>
    8544:	ea000004 	b	855c <iuw_server+0xd8>
    8548:	e35400e0 	cmp	r4, #224	; 0xe0
    854c:	0a00002f 	beq	8610 <iuw_server+0x18c>
    8550:	e35400f0 	cmp	r4, #240	; 0xf0
    8554:	1a000036 	bne	8634 <iuw_server+0x1b0>
    8558:	ea000001 	b	8564 <iuw_server+0xe0>
    855c:	e59f0844 	ldr	r0, [pc, #2116]	; 8da8 <iuw_server+0x924>
    8560:	ea000111 	b	89ac <iuw_server+0x528>
    8564:	e59f0840 	ldr	r0, [pc, #2112]	; 8dac <iuw_server+0x928>
    8568:	ea0001d7 	b	8ccc <iuw_server+0x848>
    856c:	e59f083c 	ldr	r0, [pc, #2108]	; 8db0 <iuw_server+0x92c>
    8570:	e59f3820 	ldr	r3, [pc, #2080]	; 8d98 <iuw_server+0x914>
    8574:	e1a0e00f 	mov	lr, pc
    8578:	e593f000 	ldr	pc, [r3]
    857c:	e59f2810 	ldr	r2, [pc, #2064]	; 8d94 <iuw_server+0x910>
    8580:	e3a00040 	mov	r0, #64	; 0x40
    8584:	e3a01c02 	mov	r1, #512	; 0x200
    8588:	e5924000 	ldr	r4, [r2]
    858c:	ebffff9c 	bl	8404 <iuw_shake_hand>
    8590:	e2505000 	subs	r5, r0, #0	; 0x0
    8594:	159f0818 	ldrne	r0, [pc, #2072]	; 8db4 <iuw_server+0x930>
    8598:	1a0000b8 	bne	8880 <iuw_server+0x3fc>
    859c:	e59f07e8 	ldr	r0, [pc, #2024]	; 8d8c <iuw_server+0x908>
    85a0:	e59f17e8 	ldr	r1, [pc, #2024]	; 8d90 <iuw_server+0x90c>
    85a4:	e5902000 	ldr	r2, [r0]
    85a8:	e3a03018 	mov	r3, #24	; 0x18
    85ac:	e0221293 	mla	r2, r3, r2, r1
    85b0:	e59f37dc 	ldr	r3, [pc, #2012]	; 8d94 <iuw_server+0x910>
    85b4:	e3a01c02 	mov	r1, #512	; 0x200
    85b8:	e5930000 	ldr	r0, [r3]
    85bc:	e1a0e00f 	mov	lr, pc
    85c0:	e592f004 	ldr	pc, [r2, #4]
    85c4:	e1a00004 	mov	r0, r4
    85c8:	eb00297d 	bl	12bc4 <ius_check_header>
    85cc:	e2504000 	subs	r4, r0, #0	; 0x0
    85d0:	0a000006 	beq	85f0 <iuw_server+0x16c>
    85d4:	e59f17bc 	ldr	r1, [pc, #1980]	; 8d98 <iuw_server+0x914>
    85d8:	e59f07d8 	ldr	r0, [pc, #2008]	; 8db8 <iuw_server+0x934>
    85dc:	e1a0e00f 	mov	lr, pc
    85e0:	e591f000 	ldr	pc, [r1]
    85e4:	e1a02005 	mov	r2, r5
    85e8:	e3a01002 	mov	r1, #2	; 0x2
    85ec:	ea0001d9 	b	8d58 <iuw_server+0x8d4>
    85f0:	e1a01004 	mov	r1, r4
    85f4:	ebfffb29 	bl	72a0 <cdbg_verify_burn_enable>
    85f8:	e1a00004 	mov	r0, r4
    85fc:	e1a01004 	mov	r1, r4
    8600:	ebffff7f 	bl	8404 <iuw_shake_hand>
    8604:	e3500000 	cmp	r0, #0	; 0x0
    8608:	0affffa5 	beq	84a4 <iuw_server+0x20>
    860c:	ea0000ef 	b	89d0 <iuw_server+0x54c>
    8610:	e59f07a4 	ldr	r0, [pc, #1956]	; 8dbc <iuw_server+0x938>
    8614:	e59f377c 	ldr	r3, [pc, #1916]	; 8d98 <iuw_server+0x914>
    8618:	e1a0e00f 	mov	lr, pc
    861c:	e593f000 	ldr	pc, [r3]
    8620:	e59f376c 	ldr	r3, [pc, #1900]	; 8d94 <iuw_server+0x910>
    8624:	e5933000 	ldr	r3, [r3]
    8628:	e5930004 	ldr	r0, [r3, #4]
    862c:	e28dd044 	add	sp, sp, #68	; 0x44
    8630:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8634:	ebfffb07 	bl	7258 <cdbg_verify_burn>
    8638:	e2505000 	subs	r5, r0, #0	; 0x0
    863c:	0a000005 	beq	8658 <iuw_server+0x1d4>
    8640:	e1a01004 	mov	r1, r4
    8644:	e59f0774 	ldr	r0, [pc, #1908]	; 8dc0 <iuw_server+0x93c>
    8648:	e59f3748 	ldr	r3, [pc, #1864]	; 8d98 <iuw_server+0x914>
    864c:	e1a0e00f 	mov	lr, pc
    8650:	e593f000 	ldr	pc, [r3]
    8654:	ea0001bd 	b	8d50 <iuw_server+0x8cc>
    8658:	e3540031 	cmp	r4, #49	; 0x31
    865c:	ca000009 	bgt	8688 <iuw_server+0x204>
    8660:	e3540030 	cmp	r4, #48	; 0x30
    8664:	aa0001ae 	bge	8d24 <iuw_server+0x8a0>
    8668:	e3540010 	cmp	r4, #16	; 0x10
    866c:	ba0001b2 	blt	8d3c <iuw_server+0x8b8>
    8670:	e3540016 	cmp	r4, #22	; 0x16
    8674:	da000009 	ble	86a0 <iuw_server+0x21c>
    8678:	e2443020 	sub	r3, r4, #32	; 0x20
    867c:	e3530001 	cmp	r3, #1	; 0x1
    8680:	8a0001ad 	bhi	8d3c <iuw_server+0x8b8>
    8684:	ea000005 	b	86a0 <iuw_server+0x21c>
    8688:	e3540044 	cmp	r4, #68	; 0x44
    868c:	0a000003 	beq	86a0 <iuw_server+0x21c>
    8690:	e3540046 	cmp	r4, #70	; 0x46
    8694:	0a000001 	beq	86a0 <iuw_server+0x21c>
    8698:	e3540042 	cmp	r4, #66	; 0x42
    869c:	1a0001a6 	bne	8d3c <iuw_server+0x8b8>
    86a0:	e59f06ec 	ldr	r0, [pc, #1772]	; 8d94 <iuw_server+0x910>
    86a4:	e3a04000 	mov	r4, #0	; 0x0
    86a8:	e5906000 	ldr	r6, [r0]
    86ac:	e59f0710 	ldr	r0, [pc, #1808]	; 8dc4 <iuw_server+0x940>
    86b0:	e5963000 	ldr	r3, [r6]
    86b4:	e596200c 	ldr	r2, [r6, #12]
    86b8:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    86bc:	e58d3020 	str	r3, [sp, #32]
    86c0:	e5963004 	ldr	r3, [r6, #4]
    86c4:	e59d1020 	ldr	r1, [sp, #32]
    86c8:	e1848003 	orr	r8, r4, r3
    86cc:	e58d2028 	str	r2, [sp, #40]
    86d0:	e59f36c0 	ldr	r3, [pc, #1728]	; 8d98 <iuw_server+0x914>
    86d4:	e1d650b8 	ldrh	r5, [r6, #8]
    86d8:	e5967008 	ldr	r7, [r6, #8]
    86dc:	e1a0e00f 	mov	lr, pc
    86e0:	e593f000 	ldr	pc, [r3]
    86e4:	e59d0020 	ldr	r0, [sp, #32]
    86e8:	e1a09005 	mov	r9, r5
    86ec:	e2403010 	sub	r3, r0, #16	; 0x10
    86f0:	e3530001 	cmp	r3, #1	; 0x1
    86f4:	8a000003 	bhi	8708 <iuw_server+0x284>
    86f8:	e1d650ba 	ldrh	r5, [r6, #10]
    86fc:	e355000f 	cmp	r5, #15	; 0xf
    8700:	0a000078 	beq	88e8 <iuw_server+0x464>
    8704:	ea00002f 	b	87c8 <iuw_server+0x344>
    8708:	e59d1020 	ldr	r1, [sp, #32]
    870c:	e2413012 	sub	r3, r1, #18	; 0x12
    8710:	e3530001 	cmp	r3, #1	; 0x1
    8714:	91a05004 	movls	r5, r4
    8718:	9a00002a 	bls	87c8 <iuw_server+0x344>
    871c:	e59d2020 	ldr	r2, [sp, #32]
    8720:	e2423014 	sub	r3, r2, #20	; 0x14
    8724:	e3530001 	cmp	r3, #1	; 0x1
    8728:	93a05001 	movls	r5, #1	; 0x1
    872c:	9a000025 	bls	87c8 <iuw_server+0x344>
    8730:	e59d0020 	ldr	r0, [sp, #32]
    8734:	e2403016 	sub	r3, r0, #22	; 0x16
    8738:	e3530001 	cmp	r3, #1	; 0x1
    873c:	9a000190 	bls	8d84 <iuw_server+0x900>
    8740:	e2403020 	sub	r3, r0, #32	; 0x20
    8744:	e3530001 	cmp	r3, #1	; 0x1
    8748:	8a000006 	bhi	8768 <iuw_server+0x2e4>
    874c:	e5963004 	ldr	r3, [r6, #4]
    8750:	e5961008 	ldr	r1, [r6, #8]
    8754:	e3e0b000 	mvn	fp, #0	; 0x0
    8758:	e1a08003 	mov	r8, r3
    875c:	e3a09000 	mov	r9, #0	; 0x0
    8760:	e58d1028 	str	r1, [sp, #40]
    8764:	ea00017e 	b	8d64 <iuw_server+0x8e0>
    8768:	e59d2020 	ldr	r2, [sp, #32]
    876c:	e3520044 	cmp	r2, #68	; 0x44
    8770:	13520042 	cmpne	r2, #66	; 0x42
    8774:	1a000006 	bne	8794 <iuw_server+0x310>
    8778:	e5963008 	ldr	r3, [r6, #8]
    877c:	e3e00000 	mvn	r0, #0	; 0x0
    8780:	e58d3028 	str	r3, [sp, #40]
    8784:	e3a0b001 	mov	fp, #1	; 0x1
    8788:	e58d001c 	str	r0, [sp, #28]
    878c:	e3a0aa02 	mov	sl, #8192	; 0x2000
    8790:	ea000175 	b	8d6c <iuw_server+0x8e8>
    8794:	e59d1020 	ldr	r1, [sp, #32]
    8798:	e3510046 	cmp	r1, #70	; 0x46
    879c:	13e0b000 	mvnne	fp, #0	; 0x0
    87a0:	1a00016f 	bne	8d64 <iuw_server+0x8e0>
    87a4:	e5962004 	ldr	r2, [r6, #4]
    87a8:	e5961008 	ldr	r1, [r6, #8]
    87ac:	e1a00002 	mov	r0, r2
    87b0:	e58d201c 	str	r2, [sp, #28]
    87b4:	ebfff860 	bl	693c <hash_init>
    87b8:	e5963008 	ldr	r3, [r6, #8]
    87bc:	e3e0b000 	mvn	fp, #0	; 0x0
    87c0:	e58d3028 	str	r3, [sp, #40]
    87c4:	ea000167 	b	8d68 <iuw_server+0x8e4>
    87c8:	e59f15c8 	ldr	r1, [pc, #1480]	; 8d98 <iuw_server+0x914>
    87cc:	e1a00005 	mov	r0, r5
    87d0:	e5914000 	ldr	r4, [r1]
    87d4:	ebfff5cb 	bl	5f08 <vs_device_name>
    87d8:	e1a01000 	mov	r1, r0
    87dc:	e59f05e4 	ldr	r0, [pc, #1508]	; 8dc8 <iuw_server+0x944>
    87e0:	e12fff34 	blx	r4
    87e4:	e1a00005 	mov	r0, r5
    87e8:	e3a01001 	mov	r1, #1	; 0x1
    87ec:	ebfff6d8 	bl	6354 <vs_assign_by_id>
    87f0:	e3500000 	cmp	r0, #0	; 0x0
    87f4:	aa000007 	bge	8818 <iuw_server+0x394>
    87f8:	e59f2598 	ldr	r2, [pc, #1432]	; 8d98 <iuw_server+0x914>
    87fc:	e1a00005 	mov	r0, r5
    8800:	e5924000 	ldr	r4, [r2]
    8804:	ebfff5bf 	bl	5f08 <vs_device_name>
    8808:	e1a01000 	mov	r1, r0
    880c:	e59f05b8 	ldr	r0, [pc, #1464]	; 8dcc <iuw_server+0x948>
    8810:	e12fff34 	blx	r4
    8814:	ea00014d 	b	8d50 <iuw_server+0x8cc>
    8818:	e1a00005 	mov	r0, r5
    881c:	e59f15ac 	ldr	r1, [pc, #1452]	; 8dd0 <iuw_server+0x94c>
    8820:	ebfff5df 	bl	5fa4 <vs_is_device>
    8824:	e3500000 	cmp	r0, #0	; 0x0
    8828:	1a000009 	bne	8854 <iuw_server+0x3d0>
    882c:	e1a00005 	mov	r0, r5
    8830:	e59f159c 	ldr	r1, [pc, #1436]	; 8dd4 <iuw_server+0x950>
    8834:	ebfff5da 	bl	5fa4 <vs_is_device>
    8838:	e3500000 	cmp	r0, #0	; 0x0
    883c:	1a000004 	bne	8854 <iuw_server+0x3d0>
    8840:	e1a00005 	mov	r0, r5
    8844:	e59f158c 	ldr	r1, [pc, #1420]	; 8dd8 <iuw_server+0x954>
    8848:	ebfff5d5 	bl	5fa4 <vs_is_device>
    884c:	e3500000 	cmp	r0, #0	; 0x0
    8850:	0a000024 	beq	88e8 <iuw_server+0x464>
    8854:	e59f1574 	ldr	r1, [pc, #1396]	; 8dd0 <iuw_server+0x94c>
    8858:	e1a00005 	mov	r0, r5
    885c:	ebfff5d0 	bl	5fa4 <vs_is_device>
    8860:	e3500000 	cmp	r0, #0	; 0x0
    8864:	13a00000 	movne	r0, #0	; 0x0
    8868:	03a00001 	moveq	r0, #1	; 0x1
    886c:	eb0009a5 	bl	af08 <nand_get_config>
    8870:	e3500000 	cmp	r0, #0	; 0x0
    8874:	e1a03000 	mov	r3, r0
    8878:	1a000004 	bne	8890 <iuw_server+0x40c>
    887c:	e59f0558 	ldr	r0, [pc, #1368]	; 8ddc <iuw_server+0x958>
    8880:	e59f3510 	ldr	r3, [pc, #1296]	; 8d98 <iuw_server+0x914>
    8884:	e1a0e00f 	mov	lr, pc
    8888:	e593f000 	ldr	pc, [r3]
    888c:	eaffff04 	b	84a4 <iuw_server+0x20>
    8890:	e593301c 	ldr	r3, [r3, #28]
    8894:	e1a00005 	mov	r0, r5
    8898:	e59f1538 	ldr	r1, [pc, #1336]	; 8dd8 <iuw_server+0x954>
    889c:	e58d3030 	str	r3, [sp, #48]
    88a0:	ebfff5bf 	bl	5fa4 <vs_is_device>
    88a4:	e3500000 	cmp	r0, #0	; 0x0
    88a8:	03a0a901 	moveq	sl, #16384	; 0x4000
    88ac:	03e0b000 	mvneq	fp, #0	; 0x0
    88b0:	0a000010 	beq	88f8 <iuw_server+0x474>
    88b4:	e59d0028 	ldr	r0, [sp, #40]
    88b8:	eb0009ca 	bl	afe8 <fnd_size_match>
    88bc:	e3500000 	cmp	r0, #0	; 0x0
    88c0:	059f0518 	ldreq	r0, [pc, #1304]	; 8de0 <iuw_server+0x95c>
    88c4:	0a000038 	beq	89ac <iuw_server+0x528>
    88c8:	eb0009bc 	bl	afc0 <fnd_align>
    88cc:	e3e0b000 	mvn	fp, #0	; 0x0
    88d0:	e1a0a000 	mov	sl, r0
    88d4:	e1a00005 	mov	r0, r5
    88d8:	e58db01c 	str	fp, [sp, #28]
    88dc:	ebfff549 	bl	5e08 <vs_align>
    88e0:	e58d0034 	str	r0, [sp, #52]
    88e4:	ea000005 	b	8900 <iuw_server+0x47c>
    88e8:	e3a02000 	mov	r2, #0	; 0x0
    88ec:	e3a0a901 	mov	sl, #16384	; 0x4000
    88f0:	e3e0b000 	mvn	fp, #0	; 0x0
    88f4:	e58d2030 	str	r2, [sp, #48]
    88f8:	e58da034 	str	sl, [sp, #52]
    88fc:	e58db01c 	str	fp, [sp, #28]
    8900:	e59d3020 	ldr	r3, [sp, #32]
    8904:	e1a07827 	lsr	r7, r7, #16
    8908:	e2133001 	ands	r3, r3, #1	; 0x1
    890c:	e58d3038 	str	r3, [sp, #56]
    8910:	e1a07807 	lsl	r7, r7, #16
    8914:	1a000007 	bne	8938 <iuw_server+0x4b4>
    8918:	e1a00005 	mov	r0, r5
    891c:	ebfff580 	bl	5f24 <vs_device_erasable>
    8920:	e3500000 	cmp	r0, #0	; 0x0
    8924:	0a000003 	beq	8938 <iuw_server+0x4b4>
    8928:	e1a00008 	mov	r0, r8
    892c:	e1a01009 	mov	r1, r9
    8930:	e1a02007 	mov	r2, r7
    8934:	ebfff4f7 	bl	5d18 <vs_erase>
    8938:	e35b0001 	cmp	fp, #1	; 0x1
    893c:	1a00001e 	bne	89bc <iuw_server+0x538>
    8940:	e5961004 	ldr	r1, [r6, #4]
    8944:	e3510000 	cmp	r1, #0	; 0x0
    8948:	0a00000f 	beq	898c <iuw_server+0x508>
    894c:	e59f0490 	ldr	r0, [pc, #1168]	; 8de4 <iuw_server+0x960>
    8950:	e59f1440 	ldr	r1, [pc, #1088]	; 8d98 <iuw_server+0x914>
    8954:	e1a0e00f 	mov	lr, pc
    8958:	e591f000 	ldr	pc, [r1]
    895c:	e59f3428 	ldr	r3, [pc, #1064]	; 8d8c <iuw_server+0x908>
    8960:	e59f0428 	ldr	r0, [pc, #1064]	; 8d90 <iuw_server+0x90c>
    8964:	e5932000 	ldr	r2, [r3]
    8968:	e59f1424 	ldr	r1, [pc, #1060]	; 8d94 <iuw_server+0x910>
    896c:	e3a03018 	mov	r3, #24	; 0x18
    8970:	e0220293 	mla	r2, r3, r2, r0
    8974:	e5910000 	ldr	r0, [r1]
    8978:	e3a01020 	mov	r1, #32	; 0x20
    897c:	e1a0e00f 	mov	lr, pc
    8980:	e592f004 	ldr	pc, [r2, #4]
    8984:	e59f2408 	ldr	r2, [pc, #1032]	; 8d94 <iuw_server+0x910>
    8988:	e5921000 	ldr	r1, [r2]
    898c:	e59d3020 	ldr	r3, [sp, #32]
    8990:	e59f2450 	ldr	r2, [pc, #1104]	; 8de8 <iuw_server+0x964>
    8994:	e2530042 	subs	r0, r3, #66	; 0x42
    8998:	13a00001 	movne	r0, #1	; 0x1
    899c:	ebfff6cd 	bl	64d8 <ss_aes_init>
    89a0:	e3500000 	cmp	r0, #0	; 0x0
    89a4:	aa000004 	bge	89bc <iuw_server+0x538>
    89a8:	e59f043c 	ldr	r0, [pc, #1084]	; 8dec <iuw_server+0x968>
    89ac:	e59f13e4 	ldr	r1, [pc, #996]	; 8d98 <iuw_server+0x914>
    89b0:	e1a0e00f 	mov	lr, pc
    89b4:	e591f000 	ldr	pc, [r1]
    89b8:	ea0000e4 	b	8d50 <iuw_server+0x8cc>
    89bc:	e3a00040 	mov	r0, #64	; 0x40
    89c0:	e1a0100a 	mov	r1, sl
    89c4:	ebfffe8e 	bl	8404 <iuw_shake_hand>
    89c8:	e2504000 	subs	r4, r0, #0	; 0x0
    89cc:	0a000004 	beq	89e4 <iuw_server+0x560>
    89d0:	e59f03dc 	ldr	r0, [pc, #988]	; 8db4 <iuw_server+0x930>
    89d4:	e59f23bc 	ldr	r2, [pc, #956]	; 8d98 <iuw_server+0x914>
    89d8:	e1a0e00f 	mov	lr, pc
    89dc:	e592f000 	ldr	pc, [r2]
    89e0:	eafffeaf 	b	84a4 <iuw_server+0x20>
    89e4:	e59d3028 	ldr	r3, [sp, #40]
    89e8:	e59f0400 	ldr	r0, [pc, #1024]	; 8df0 <iuw_server+0x96c>
    89ec:	e58d3000 	str	r3, [sp]
    89f0:	e1a02008 	mov	r2, r8
    89f4:	e1a03009 	mov	r3, r9
    89f8:	e59f1398 	ldr	r1, [pc, #920]	; 8d98 <iuw_server+0x914>
    89fc:	e58da004 	str	sl, [sp, #4]
    8a00:	e1a0e00f 	mov	lr, pc
    8a04:	e591f000 	ldr	pc, [r1]
    8a08:	e59d2030 	ldr	r2, [sp, #48]
    8a0c:	e59d0028 	ldr	r0, [sp, #40]
    8a10:	e2423001 	sub	r3, r2, #1	; 0x1
    8a14:	e59d5028 	ldr	r5, [sp, #40]
    8a18:	e0607007 	rsb	r7, r0, r7
    8a1c:	e1a00003 	mov	r0, r3
    8a20:	e1a01fc0 	asr	r1, r0, #31
    8a24:	e1a03fc2 	asr	r3, r2, #31
    8a28:	e58d702c 	str	r7, [sp, #44]
    8a2c:	e58d4024 	str	r4, [sp, #36]
    8a30:	e1cd01f0 	strd	r0, [sp, #16]
    8a34:	e58d503c 	str	r5, [sp, #60]
    8a38:	e1a07004 	mov	r7, r4
    8a3c:	e1cd20f8 	strd	r2, [sp, #8]
    8a40:	ea000092 	b	8c90 <iuw_server+0x80c>
    8a44:	e59d003c 	ldr	r0, [sp, #60]
    8a48:	e0653000 	rsb	r3, r5, r0
    8a4c:	e1b03a23 	lsrs	r3, r3, #20
    8a50:	0a000005 	beq	8a6c <iuw_server+0x5e8>
    8a54:	e59f0398 	ldr	r0, [pc, #920]	; 8df4 <iuw_server+0x970>
    8a58:	e1a01a45 	asr	r1, r5, #20
    8a5c:	e59f2334 	ldr	r2, [pc, #820]	; 8d98 <iuw_server+0x914>
    8a60:	e1a0e00f 	mov	lr, pc
    8a64:	e592f000 	ldr	pc, [r2]
    8a68:	e58d503c 	str	r5, [sp, #60]
    8a6c:	e3570000 	cmp	r7, #0	; 0x0
    8a70:	d3a00000 	movle	r0, #0	; 0x0
    8a74:	c3a00001 	movgt	r0, #1	; 0x1
    8a78:	eb0020dc 	bl	10df0 <led_ctrl>
    8a7c:	e2877001 	add	r7, r7, #1	; 0x1
    8a80:	e59d3038 	ldr	r3, [sp, #56]
    8a84:	e3570032 	cmp	r7, #50	; 0x32
    8a88:	c2477064 	subgt	r7, r7, #100	; 0x64
    8a8c:	e155000a 	cmp	r5, sl
    8a90:	b1a06005 	movlt	r6, r5
    8a94:	a1a0600a 	movge	r6, sl
    8a98:	e3530000 	cmp	r3, #0	; 0x0
    8a9c:	1a00001e 	bne	8b1c <iuw_server+0x698>
    8aa0:	e59f02e4 	ldr	r0, [pc, #740]	; 8d8c <iuw_server+0x908>
    8aa4:	e59f12e4 	ldr	r1, [pc, #740]	; 8d90 <iuw_server+0x90c>
    8aa8:	e5902000 	ldr	r2, [r0]
    8aac:	e2833018 	add	r3, r3, #24	; 0x18
    8ab0:	e0221293 	mla	r2, r3, r2, r1
    8ab4:	e59f32d8 	ldr	r3, [pc, #728]	; 8d94 <iuw_server+0x910>
    8ab8:	e1a01006 	mov	r1, r6
    8abc:	e5930000 	ldr	r0, [r3]
    8ac0:	e1a0e00f 	mov	lr, pc
    8ac4:	e592f004 	ldr	pc, [r2, #4]
    8ac8:	e59d0028 	ldr	r0, [sp, #40]
    8acc:	e1500005 	cmp	r0, r5
    8ad0:	1a000011 	bne	8b1c <iuw_server+0x698>
    8ad4:	e59f12b8 	ldr	r1, [pc, #696]	; 8d94 <iuw_server+0x910>
    8ad8:	e5914000 	ldr	r4, [r1]
    8adc:	e1a00004 	mov	r0, r4
    8ae0:	ebffe3c7 	bl	1a04 <isi_check_header>
    8ae4:	e3500000 	cmp	r0, #0	; 0x0
    8ae8:	1a00000b 	bne	8b1c <iuw_server+0x698>
    8aec:	e5943004 	ldr	r3, [r4, #4]
    8af0:	e1b03e23 	lsrs	r3, r3, #28
    8af4:	0a000008 	beq	8b1c <iuw_server+0x698>
    8af8:	e59f02f8 	ldr	r0, [pc, #760]	; 8df8 <iuw_server+0x974>
    8afc:	e59f2294 	ldr	r2, [pc, #660]	; 8d98 <iuw_server+0x914>
    8b00:	e1a0e00f 	mov	lr, pc
    8b04:	e592f000 	ldr	pc, [r2]
    8b08:	e59d1038 	ldr	r1, [sp, #56]
    8b0c:	e59f22d4 	ldr	r2, [pc, #724]	; 8de8 <iuw_server+0x964>
    8b10:	e3a00001 	mov	r0, #1	; 0x1
    8b14:	ebfff66f 	bl	64d8 <ss_aes_init>
    8b18:	ea000002 	b	8b28 <iuw_server+0x6a4>
    8b1c:	e59d3024 	ldr	r3, [sp, #36]
    8b20:	e3530000 	cmp	r3, #0	; 0x0
    8b24:	0a000006 	beq	8b44 <iuw_server+0x6c0>
    8b28:	e59f1264 	ldr	r1, [pc, #612]	; 8d94 <iuw_server+0x910>
    8b2c:	e1a02006 	mov	r2, r6
    8b30:	e5910000 	ldr	r0, [r1]
    8b34:	e1a01000 	mov	r1, r0
    8b38:	ebfff655 	bl	6494 <ss_aes_data>
    8b3c:	e3a02001 	mov	r2, #1	; 0x1
    8b40:	e58d2024 	str	r2, [sp, #36]
    8b44:	e59d3030 	ldr	r3, [sp, #48]
    8b48:	e3530000 	cmp	r3, #0	; 0x0
    8b4c:	0a00001c 	beq	8bc4 <iuw_server+0x740>
    8b50:	e1cd01d0 	ldrd	r0, [sp, #16]
    8b54:	e0082000 	and	r2, r8, r0
    8b58:	e0093001 	and	r3, r9, r1
    8b5c:	e1921003 	orrs	r1, r2, r3
    8b60:	0a000011 	beq	8bac <iuw_server+0x728>
    8b64:	ea000016 	b	8bc4 <iuw_server+0x740>
    8b68:	e59d202c 	ldr	r2, [sp, #44]
    8b6c:	e59d3030 	ldr	r3, [sp, #48]
    8b70:	e1520003 	cmp	r2, r3
    8b74:	e0632002 	rsb	r2, r3, r2
    8b78:	e58d202c 	str	r2, [sp, #44]
    8b7c:	da000008 	ble	8ba4 <iuw_server+0x720>
    8b80:	e1cd20d8 	ldrd	r2, [sp, #8]
    8b84:	e0988002 	adds	r8, r8, r2
    8b88:	e0a99003 	adc	r9, r9, r3
    8b8c:	e1a02008 	mov	r2, r8
    8b90:	e1a03009 	mov	r3, r9
    8b94:	e59f11fc 	ldr	r1, [pc, #508]	; 8d98 <iuw_server+0x914>
    8b98:	e1a0e00f 	mov	lr, pc
    8b9c:	e591f000 	ldr	pc, [r1]
    8ba0:	ea000001 	b	8bac <iuw_server+0x728>
    8ba4:	e59f0250 	ldr	r0, [pc, #592]	; 8dfc <iuw_server+0x978>
    8ba8:	ea000047 	b	8ccc <iuw_server+0x848>
    8bac:	e1a00008 	mov	r0, r8
    8bb0:	e1a01009 	mov	r1, r9
    8bb4:	ebfff466 	bl	5d54 <vs_isbad>
    8bb8:	e3500000 	cmp	r0, #0	; 0x0
    8bbc:	e59f023c 	ldr	r0, [pc, #572]	; 8e00 <iuw_server+0x97c>
    8bc0:	1affffe8 	bne	8b68 <iuw_server+0x6e4>
    8bc4:	e59d301c 	ldr	r3, [sp, #28]
    8bc8:	e3530000 	cmp	r3, #0	; 0x0
    8bcc:	a59f11c0 	ldrge	r1, [pc, #448]	; 8d94 <iuw_server+0x910>
    8bd0:	b59f21bc 	ldrlt	r2, [pc, #444]	; 8d94 <iuw_server+0x910>
    8bd4:	a5910000 	ldrge	r0, [r1]
    8bd8:	e59d1020 	ldr	r1, [sp, #32]
    8bdc:	b5920000 	ldrlt	r0, [r2]
    8be0:	a1a02008 	movge	r2, r8
    8be4:	b1a02008 	movlt	r2, r8
    8be8:	a1a03009 	movge	r3, r9
    8bec:	b1a03009 	movlt	r3, r9
    8bf0:	a58d6000 	strge	r6, [sp]
    8bf4:	b58da000 	strlt	sl, [sp]
    8bf8:	e58d1004 	str	r1, [sp, #4]
    8bfc:	ebfffd90 	bl	8244 <iuw_local_rw>
    8c00:	e59d2038 	ldr	r2, [sp, #56]
    8c04:	e3520000 	cmp	r2, #0	; 0x0
    8c08:	0a000009 	beq	8c34 <iuw_server+0x7b0>
    8c0c:	e59f3178 	ldr	r3, [pc, #376]	; 8d8c <iuw_server+0x908>
    8c10:	e59f0178 	ldr	r0, [pc, #376]	; 8d90 <iuw_server+0x90c>
    8c14:	e5932000 	ldr	r2, [r3]
    8c18:	e59f1174 	ldr	r1, [pc, #372]	; 8d94 <iuw_server+0x910>
    8c1c:	e3a03018 	mov	r3, #24	; 0x18
    8c20:	e0220293 	mla	r2, r3, r2, r0
    8c24:	e5910000 	ldr	r0, [r1]
    8c28:	e1a01006 	mov	r1, r6
    8c2c:	e1a0e00f 	mov	lr, pc
    8c30:	e592f008 	ldr	pc, [r2, #8]
    8c34:	e35b0001 	cmp	fp, #1	; 0x1
    8c38:	1a000010 	bne	8c80 <iuw_server+0x7fc>
    8c3c:	e3a00000 	mov	r0, #0	; 0x0
    8c40:	e1a01000 	mov	r1, r0
    8c44:	ebfffdee 	bl	8404 <iuw_shake_hand>
    8c48:	e59f213c 	ldr	r2, [pc, #316]	; 8d8c <iuw_server+0x908>
    8c4c:	e59f1140 	ldr	r1, [pc, #320]	; 8d94 <iuw_server+0x910>
    8c50:	e5923000 	ldr	r3, [r2]
    8c54:	e5910000 	ldr	r0, [r1]
    8c58:	e59f1130 	ldr	r1, [pc, #304]	; 8d90 <iuw_server+0x90c>
    8c5c:	e3a02018 	mov	r2, #24	; 0x18
    8c60:	e0231392 	mla	r3, r2, r3, r1
    8c64:	e2800a02 	add	r0, r0, #8192	; 0x2000
    8c68:	e1a01006 	mov	r1, r6
    8c6c:	e1a0e00f 	mov	lr, pc
    8c70:	e593f008 	ldr	pc, [r3, #8]
    8c74:	e3a00000 	mov	r0, #0	; 0x0
    8c78:	e1a01000 	mov	r1, r0
    8c7c:	ebfffde0 	bl	8404 <iuw_shake_hand>
    8c80:	e59d2034 	ldr	r2, [sp, #52]
    8c84:	e0665005 	rsb	r5, r6, r5
    8c88:	e0988002 	adds	r8, r8, r2
    8c8c:	e0a99fc2 	adc	r9, r9, r2, asr #31
    8c90:	e3550000 	cmp	r5, #0	; 0x0
    8c94:	caffff6a 	bgt	8a44 <iuw_server+0x5c0>
    8c98:	e59d301c 	ldr	r3, [sp, #28]
    8c9c:	e3530000 	cmp	r3, #0	; 0x0
    8ca0:	ba000019 	blt	8d0c <iuw_server+0x888>
    8ca4:	ebfff6e1 	bl	6830 <hash_len>
    8ca8:	e1a01000 	mov	r1, r0
    8cac:	e3a00040 	mov	r0, #64	; 0x40
    8cb0:	ebfffdd3 	bl	8404 <iuw_shake_hand>
    8cb4:	e59f10d8 	ldr	r1, [pc, #216]	; 8d94 <iuw_server+0x910>
    8cb8:	e5910000 	ldr	r0, [r1]
    8cbc:	ebfff6e2 	bl	684c <hash_value>
    8cc0:	e3500000 	cmp	r0, #0	; 0x0
    8cc4:	aa000004 	bge	8cdc <iuw_server+0x858>
    8cc8:	e59f0134 	ldr	r0, [pc, #308]	; 8e04 <iuw_server+0x980>
    8ccc:	e59f20c4 	ldr	r2, [pc, #196]	; 8d98 <iuw_server+0x914>
    8cd0:	e1a0e00f 	mov	lr, pc
    8cd4:	e592f000 	ldr	pc, [r2]
    8cd8:	ea00001c 	b	8d50 <iuw_server+0x8cc>
    8cdc:	e59f00a8 	ldr	r0, [pc, #168]	; 8d8c <iuw_server+0x908>
    8ce0:	e59f10a8 	ldr	r1, [pc, #168]	; 8d90 <iuw_server+0x90c>
    8ce4:	e5903000 	ldr	r3, [r0]
    8ce8:	e3a02018 	mov	r2, #24	; 0x18
    8cec:	e0231392 	mla	r3, r2, r3, r1
    8cf0:	e59f209c 	ldr	r2, [pc, #156]	; 8d94 <iuw_server+0x910>
    8cf4:	e5935008 	ldr	r5, [r3, #8]
    8cf8:	e5924000 	ldr	r4, [r2]
    8cfc:	ebfff6cb 	bl	6830 <hash_len>
    8d00:	e1a01000 	mov	r1, r0
    8d04:	e1a00004 	mov	r0, r4
    8d08:	e12fff35 	blx	r5
    8d0c:	e37b0001 	cmn	fp, #1	; 0x1
    8d10:	1afffde3 	bne	84a4 <iuw_server+0x20>
    8d14:	e3a00000 	mov	r0, #0	; 0x0
    8d18:	e1a01000 	mov	r1, r0
    8d1c:	ebfffdb8 	bl	8404 <iuw_shake_hand>
    8d20:	eafffddf 	b	84a4 <iuw_server+0x20>
    8d24:	e59f0080 	ldr	r0, [pc, #128]	; 8dac <iuw_server+0x928>
    8d28:	e59f3068 	ldr	r3, [pc, #104]	; 8d98 <iuw_server+0x914>
    8d2c:	e1a0e00f 	mov	lr, pc
    8d30:	e593f000 	ldr	pc, [r3]
    8d34:	e1a01005 	mov	r1, r5
    8d38:	ea000005 	b	8d54 <iuw_server+0x8d0>
    8d3c:	e1a01004 	mov	r1, r4
    8d40:	e59f00c0 	ldr	r0, [pc, #192]	; 8e08 <iuw_server+0x984>
    8d44:	e59f204c 	ldr	r2, [pc, #76]	; 8d98 <iuw_server+0x914>
    8d48:	e1a0e00f 	mov	lr, pc
    8d4c:	e592f000 	ldr	pc, [r2]
    8d50:	e3a01000 	mov	r1, #0	; 0x0
    8d54:	e1a02001 	mov	r2, r1
    8d58:	e3a000f0 	mov	r0, #240	; 0xf0
    8d5c:	ebfffd81 	bl	8368 <iuw_send_cmd>
    8d60:	eafffdcf 	b	84a4 <iuw_server+0x20>
    8d64:	e58db01c 	str	fp, [sp, #28]
    8d68:	e3a0a901 	mov	sl, #16384	; 0x4000
    8d6c:	e3a03000 	mov	r3, #0	; 0x0
    8d70:	e3a00901 	mov	r0, #16384	; 0x4000
    8d74:	e58d3030 	str	r3, [sp, #48]
    8d78:	e58d0034 	str	r0, [sp, #52]
    8d7c:	e3a0500f 	mov	r5, #15	; 0xf
    8d80:	eafffede 	b	8900 <iuw_server+0x47c>
    8d84:	e3a05002 	mov	r5, #2	; 0x2
    8d88:	eafffe8e 	b	87c8 <iuw_server+0x344>
    8d8c:	3c00a0e0 	.word	0x3c00a0e0
    8d90:	000162bc 	.word	0x000162bc
    8d94:	3c00a0dc 	.word	0x3c00a0dc
    8d98:	3c00a5cc 	.word	0x3c00a5cc
    8d9c:	00018733 	.word	0x00018733
    8da0:	0001874e 	.word	0x0001874e
    8da4:	00018765 	.word	0x00018765
    8da8:	00018782 	.word	0x00018782
    8dac:	0001879c 	.word	0x0001879c
    8db0:	000187b3 	.word	0x000187b3
    8db4:	000187c7 	.word	0x000187c7
    8db8:	000187f6 	.word	0x000187f6
    8dbc:	0001880e 	.word	0x0001880e
    8dc0:	0001881d 	.word	0x0001881d
    8dc4:	00018832 	.word	0x00018832
    8dc8:	0001884c 	.word	0x0001884c
    8dcc:	00018864 	.word	0x00018864
    8dd0:	000166d1 	.word	0x000166d1
    8dd4:	000178a7 	.word	0x000178a7
    8dd8:	000178a3 	.word	0x000178a3
    8ddc:	00018425 	.word	0x00018425
    8de0:	00018408 	.word	0x00018408
    8de4:	0001887f 	.word	0x0001887f
    8de8:	3c00a004 	.word	0x3c00a004
    8dec:	00018894 	.word	0x00018894
    8df0:	000188b3 	.word	0x000188b3
    8df4:	000188dd 	.word	0x000188dd
    8df8:	000188ea 	.word	0x000188ea
    8dfc:	00018478 	.word	0x00018478
    8e00:	00018459 	.word	0x00018459
    8e04:	00018908 	.word	0x00018908
    8e08:	00018920 	.word	0x00018920

00008e0c <iuw_close>:
    8e0c:	e92d4010 	push	{r4, lr}
    8e10:	e59f4010 	ldr	r4, [pc, #16]	; 8e28 <iuw_close+0x1c>
    8e14:	e5940000 	ldr	r0, [r4]
    8e18:	ebffe25e 	bl	1798 <free>
    8e1c:	e3a00000 	mov	r0, #0	; 0x0
    8e20:	e5840000 	str	r0, [r4]
    8e24:	e8bd8010 	pop	{r4, pc}
    8e28:	3c00a0dc 	.word	0x3c00a0dc

00008e2c <iuw_connect>:
    8e2c:	e92d4030 	push	{r4, r5, lr}
    8e30:	e59f3094 	ldr	r3, [pc, #148]	; 8ecc <iuw_connect+0xa0>
    8e34:	e24dd004 	sub	sp, sp, #4	; 0x4
    8e38:	e5932000 	ldr	r2, [r3]
    8e3c:	e3a03018 	mov	r3, #24	; 0x18
    8e40:	e0030293 	mul	r3, r3, r2
    8e44:	e59f2084 	ldr	r2, [pc, #132]	; 8ed0 <iuw_connect+0xa4>
    8e48:	e1a0e00f 	mov	lr, pc
    8e4c:	e792f003 	ldr	pc, [r2, r3]
    8e50:	e2505000 	subs	r5, r0, #0	; 0x0
    8e54:	ba000019 	blt	8ec0 <iuw_connect+0x94>
    8e58:	e59f4074 	ldr	r4, [pc, #116]	; 8ed4 <iuw_connect+0xa8>
    8e5c:	e5943000 	ldr	r3, [r4]
    8e60:	e3530000 	cmp	r3, #0	; 0x0
    8e64:	1a000002 	bne	8e74 <iuw_connect+0x48>
    8e68:	e3a00901 	mov	r0, #16384	; 0x4000
    8e6c:	ebffe1e1 	bl	15f8 <malloc>
    8e70:	e5840000 	str	r0, [r4]
    8e74:	e59f3058 	ldr	r3, [pc, #88]	; 8ed4 <iuw_connect+0xa8>
    8e78:	e5933000 	ldr	r3, [r3]
    8e7c:	e3530000 	cmp	r3, #0	; 0x0
    8e80:	1a000004 	bne	8e98 <iuw_connect+0x6c>
    8e84:	e59f004c 	ldr	r0, [pc, #76]	; 8ed8 <iuw_connect+0xac>
    8e88:	e59f304c 	ldr	r3, [pc, #76]	; 8edc <iuw_connect+0xb0>
    8e8c:	e1a0e00f 	mov	lr, pc
    8e90:	e593f000 	ldr	pc, [r3]
    8e94:	ea000004 	b	8eac <iuw_connect+0x80>
    8e98:	e3a00000 	mov	r0, #0	; 0x0
    8e9c:	e1a01000 	mov	r1, r0
    8ea0:	ebfffd57 	bl	8404 <iuw_shake_hand>
    8ea4:	e2505000 	subs	r5, r0, #0	; 0x0
    8ea8:	0a000004 	beq	8ec0 <iuw_connect+0x94>
    8eac:	e59f4020 	ldr	r4, [pc, #32]	; 8ed4 <iuw_connect+0xa8>
    8eb0:	e5940000 	ldr	r0, [r4]
    8eb4:	ebffe237 	bl	1798 <free>
    8eb8:	e3a03000 	mov	r3, #0	; 0x0
    8ebc:	e5843000 	str	r3, [r4]
    8ec0:	e1a00005 	mov	r0, r5
    8ec4:	e28dd004 	add	sp, sp, #4	; 0x4
    8ec8:	e8bd8030 	pop	{r4, r5, pc}
    8ecc:	3c00a0e0 	.word	0x3c00a0e0
    8ed0:	000162bc 	.word	0x000162bc
    8ed4:	3c00a0dc 	.word	0x3c00a0dc
    8ed8:	0001893a 	.word	0x0001893a
    8edc:	3c00a5cc 	.word	0x3c00a5cc

00008ee0 <mmc_send_cmd>:
    8ee0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8ee4:	e24dd004 	sub	sp, sp, #4	; 0x4
    8ee8:	e1a0e00f 	mov	lr, pc
    8eec:	e590f104 	ldr	pc, [r0, #260]
    8ef0:	e28dd004 	add	sp, sp, #4	; 0x4
    8ef4:	e8bd8000 	pop	{pc}

00008ef8 <mmc_set_blocklen>:
    8ef8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8efc:	e3a03015 	mov	r3, #21	; 0x15
    8f00:	e24dd024 	sub	sp, sp, #36	; 0x24
    8f04:	e3a02000 	mov	r2, #0	; 0x0
    8f08:	e58d3004 	str	r3, [sp, #4]
    8f0c:	e3a03010 	mov	r3, #16	; 0x10
    8f10:	e58d1008 	str	r1, [sp, #8]
    8f14:	e1cd30b0 	strh	r3, [sp]
    8f18:	e58d201c 	str	r2, [sp, #28]
    8f1c:	e1a0100d 	mov	r1, sp
    8f20:	e1a0e00f 	mov	lr, pc
    8f24:	e590f104 	ldr	pc, [r0, #260]
    8f28:	e28dd024 	add	sp, sp, #36	; 0x24
    8f2c:	e8bd8000 	pop	{pc}

00008f30 <find_mmc_device>:
    8f30:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8f34:	e59f3048 	ldr	r3, [pc, #72]	; 8f84 <find_mmc_device+0x54>
    8f38:	e24dd004 	sub	sp, sp, #4	; 0x4
    8f3c:	e1a01000 	mov	r1, r0
    8f40:	e1a0c003 	mov	ip, r3
    8f44:	e5932000 	ldr	r2, [r3]
    8f48:	ea000004 	b	8f60 <find_mmc_device+0x30>
    8f4c:	e592309c 	ldr	r3, [r2, #156]
    8f50:	e1a00002 	mov	r0, r2
    8f54:	e1530001 	cmp	r3, r1
    8f58:	0a000007 	beq	8f7c <find_mmc_device+0x4c>
    8f5c:	e5922000 	ldr	r2, [r2]
    8f60:	e152000c 	cmp	r2, ip
    8f64:	1afffff8 	bne	8f4c <find_mmc_device+0x1c>
    8f68:	e59f0018 	ldr	r0, [pc, #24]	; 8f88 <find_mmc_device+0x58>
    8f6c:	e59f3018 	ldr	r3, [pc, #24]	; 8f8c <find_mmc_device+0x5c>
    8f70:	e1a0e00f 	mov	lr, pc
    8f74:	e593f000 	ldr	pc, [r3]
    8f78:	e3a00000 	mov	r0, #0	; 0x0
    8f7c:	e28dd004 	add	sp, sp, #4	; 0x4
    8f80:	e8bd8000 	pop	{pc}
    8f84:	3c00a0e4 	.word	0x3c00a0e4
    8f88:	0001895b 	.word	0x0001895b
    8f8c:	3c00a5cc 	.word	0x3c00a5cc

00008f90 <mmc_send_ext_csd>:
    8f90:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8f94:	e3a03015 	mov	r3, #21	; 0x15
    8f98:	e24dd034 	sub	sp, sp, #52	; 0x34
    8f9c:	e58d3004 	str	r3, [sp, #4]
    8fa0:	e3a03c02 	mov	r3, #512	; 0x200
    8fa4:	e3a02000 	mov	r2, #0	; 0x0
    8fa8:	e3a0c001 	mov	ip, #1	; 0x1
    8fac:	e58d302c 	str	r3, [sp, #44]
    8fb0:	e3a03008 	mov	r3, #8	; 0x8
    8fb4:	e58d1020 	str	r1, [sp, #32]
    8fb8:	e58d201c 	str	r2, [sp, #28]
    8fbc:	e58d2008 	str	r2, [sp, #8]
    8fc0:	e58dc024 	str	ip, [sp, #36]
    8fc4:	e1cd30b0 	strh	r3, [sp]
    8fc8:	e58dc028 	str	ip, [sp, #40]
    8fcc:	e1a0100d 	mov	r1, sp
    8fd0:	e28d2020 	add	r2, sp, #32	; 0x20
    8fd4:	e1a0e00f 	mov	lr, pc
    8fd8:	e590f104 	ldr	pc, [r0, #260]
    8fdc:	e28dd034 	add	sp, sp, #52	; 0x34
    8fe0:	e8bd8000 	pop	{pc}

00008fe4 <sd_switch>:
    8fe4:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    8fe8:	e1a02102 	lsl	r2, r2, #2
    8fec:	e18ccf81 	orr	ip, ip, r1, lsl #31
    8ff0:	e3a0100f 	mov	r1, #15	; 0xf
    8ff4:	e1ccc211 	bic	ip, ip, r1, lsl r2
    8ff8:	e18cc213 	orr	ip, ip, r3, lsl r2
    8ffc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9000:	e3a03015 	mov	r3, #21	; 0x15
    9004:	e24dd034 	sub	sp, sp, #52	; 0x34
    9008:	e58d3004 	str	r3, [sp, #4]
    900c:	e3a03000 	mov	r3, #0	; 0x0
    9010:	e58d301c 	str	r3, [sp, #28]
    9014:	e59d3038 	ldr	r3, [sp, #56]
    9018:	e3a02001 	mov	r2, #1	; 0x1
    901c:	e58d3020 	str	r3, [sp, #32]
    9020:	e3a03040 	mov	r3, #64	; 0x40
    9024:	e58d302c 	str	r3, [sp, #44]
    9028:	e3a03006 	mov	r3, #6	; 0x6
    902c:	e58d2024 	str	r2, [sp, #36]
    9030:	e58d2028 	str	r2, [sp, #40]
    9034:	e58dc008 	str	ip, [sp, #8]
    9038:	e1cd30b0 	strh	r3, [sp]
    903c:	e1a0100d 	mov	r1, sp
    9040:	e28d2020 	add	r2, sp, #32	; 0x20
    9044:	e1a0e00f 	mov	lr, pc
    9048:	e590f104 	ldr	pc, [r0, #260]
    904c:	e28dd034 	add	sp, sp, #52	; 0x34
    9050:	e8bd8000 	pop	{pc}

00009054 <mmc_set_ios>:
    9054:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9058:	e24dd004 	sub	sp, sp, #4	; 0x4
    905c:	e1a0e00f 	mov	lr, pc
    9060:	e590f108 	ldr	pc, [r0, #264]
    9064:	e28dd004 	add	sp, sp, #4	; 0x4
    9068:	e8bd8000 	pop	{pc}

0000906c <mmc_set_clock>:
    906c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9070:	e590203c 	ldr	r2, [r0, #60]
    9074:	e590c038 	ldr	ip, [r0, #56]
    9078:	e1510002 	cmp	r1, r2
    907c:	21a01002 	movcs	r1, r2
    9080:	e24dd004 	sub	sp, sp, #4	; 0x4
    9084:	e151000c 	cmp	r1, ip
    9088:	25801048 	strcs	r1, [r0, #72]
    908c:	3580c048 	strcc	ip, [r0, #72]
    9090:	e1a0e00f 	mov	lr, pc
    9094:	e590f108 	ldr	pc, [r0, #264]
    9098:	e28dd004 	add	sp, sp, #4	; 0x4
    909c:	e8bd8000 	pop	{pc}

000090a0 <mmc_set_bus_width>:
    90a0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    90a4:	e5801044 	str	r1, [r0, #68]
    90a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    90ac:	e1a0e00f 	mov	lr, pc
    90b0:	e590f108 	ldr	pc, [r0, #264]
    90b4:	e28dd004 	add	sp, sp, #4	; 0x4
    90b8:	e8bd8000 	pop	{pc}

000090bc <mmc_send_if_cond>:
    90bc:	e92d4010 	push	{r4, lr}
    90c0:	e590202c 	ldr	r2, [r0, #44]
    90c4:	e59f3060 	ldr	r3, [pc, #96]	; 912c <mmc_send_if_cond+0x70>
    90c8:	e24dd020 	sub	sp, sp, #32	; 0x20
    90cc:	e0023003 	and	r3, r2, r3
    90d0:	e3530000 	cmp	r3, #0	; 0x0
    90d4:	e28d1020 	add	r1, sp, #32	; 0x20
    90d8:	e3a03008 	mov	r3, #8	; 0x8
    90dc:	e300c1aa 	movw	ip, #426	; 0x1aa
    90e0:	e16132b0 	strh	r3, [r1, #-32]!
    90e4:	03a0c0aa 	moveq	ip, #170	; 0xaa
    90e8:	e3a02000 	mov	r2, #0	; 0x0
    90ec:	e3a03015 	mov	r3, #21	; 0x15
    90f0:	e98d1008 	stmib	sp, {r3, ip}
    90f4:	e58d201c 	str	r2, [sp, #28]
    90f8:	e1a0100d 	mov	r1, sp
    90fc:	e5903104 	ldr	r3, [r0, #260]
    9100:	e1a04000 	mov	r4, r0
    9104:	e12fff33 	blx	r3
    9108:	e3500000 	cmp	r0, #0	; 0x0
    910c:	1a000004 	bne	9124 <mmc_send_if_cond+0x68>
    9110:	e5dd300c 	ldrb	r3, [sp, #12]
    9114:	e35300aa 	cmp	r3, #170	; 0xaa
    9118:	059f3010 	ldreq	r3, [pc, #16]	; 9130 <mmc_send_if_cond+0x74>
    911c:	13e00010 	mvnne	r0, #16	; 0x10
    9120:	05843030 	streq	r3, [r4, #48]
    9124:	e28dd020 	add	sp, sp, #32	; 0x20
    9128:	e8bd8010 	pop	{r4, pc}
    912c:	00ff8000 	.word	0x00ff8000
    9130:	00020020 	.word	0x00020020

00009134 <mmc_register>:
    9134:	e5903110 	ldr	r3, [r0, #272]
    9138:	e59fc05c 	ldr	ip, [pc, #92]	; 919c <mmc_register+0x68>
    913c:	e3530000 	cmp	r3, #0	; 0x0
    9140:	e3a03006 	mov	r3, #6	; 0x6
    9144:	e59c2000 	ldr	r2, [ip]
    9148:	e5803098 	str	r3, [r0, #152]
    914c:	e59f304c 	ldr	r3, [pc, #76]	; 91a0 <mmc_register+0x6c>
    9150:	e2821001 	add	r1, r2, #1	; 0x1
    9154:	e58030f8 	str	r3, [r0, #248]
    9158:	e59f3044 	ldr	r3, [pc, #68]	; 91a4 <mmc_register+0x70>
    915c:	e580209c 	str	r2, [r0, #156]
    9160:	e58030fc 	str	r3, [r0, #252]
    9164:	e3a03001 	mov	r3, #1	; 0x1
    9168:	e58c1000 	str	r1, [ip]
    916c:	e5c030a4 	strb	r3, [r0, #164]
    9170:	03a03c02 	moveq	r3, #512	; 0x200
    9174:	05803110 	streq	r3, [r0, #272]
    9178:	e59f3028 	ldr	r3, [pc, #40]	; 91a8 <mmc_register+0x74>
    917c:	e5800004 	str	r0, [r0, #4]
    9180:	e5932004 	ldr	r2, [r3, #4]
    9184:	e5803000 	str	r3, [r0]
    9188:	e5830004 	str	r0, [r3, #4]
    918c:	e5820000 	str	r0, [r2]
    9190:	e5802004 	str	r2, [r0, #4]
    9194:	e3a00000 	mov	r0, #0	; 0x0
    9198:	e12fff1e 	bx	lr
    919c:	3c00a028 	.word	0x3c00a028
    91a0:	0000a370 	.word	0x0000a370
    91a4:	0000988c 	.word	0x0000988c
    91a8:	3c00a0e4 	.word	0x3c00a0e4

000091ac <mmc_get_dev>:
    91ac:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    91b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    91b4:	ebffff5d 	bl	8f30 <find_mmc_device>
    91b8:	e3500000 	cmp	r0, #0	; 0x0
    91bc:	12800098 	addne	r0, r0, #152	; 0x98
    91c0:	e28dd004 	add	sp, sp, #4	; 0x4
    91c4:	e8bd8000 	pop	{pc}

000091c8 <print_mmc_devices>:
    91c8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    91cc:	e59f3068 	ldr	r3, [pc, #104]	; 923c <print_mmc_devices+0x74>
    91d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    91d4:	e1a05000 	mov	r5, r0
    91d8:	e1a07003 	mov	r7, r3
    91dc:	e5934000 	ldr	r4, [r3]
    91e0:	e59f6058 	ldr	r6, [pc, #88]	; 9240 <print_mmc_devices+0x78>
    91e4:	ea00000a 	b	9214 <print_mmc_devices+0x4c>
    91e8:	e594209c 	ldr	r2, [r4, #156]
    91ec:	e1a0e00f 	mov	lr, pc
    91f0:	e596f000 	ldr	pc, [r6]
    91f4:	e5943000 	ldr	r3, [r4]
    91f8:	e59f0044 	ldr	r0, [pc, #68]	; 9244 <print_mmc_devices+0x7c>
    91fc:	e1530007 	cmp	r3, r7
    9200:	e1a01005 	mov	r1, r5
    9204:	0a000001 	beq	9210 <print_mmc_devices+0x48>
    9208:	e1a0e00f 	mov	lr, pc
    920c:	e596f000 	ldr	pc, [r6]
    9210:	e5944000 	ldr	r4, [r4]
    9214:	e1540007 	cmp	r4, r7
    9218:	e2841008 	add	r1, r4, #8	; 0x8
    921c:	e59f0024 	ldr	r0, [pc, #36]	; 9248 <print_mmc_devices+0x80>
    9220:	1afffff0 	bne	91e8 <print_mmc_devices+0x20>
    9224:	e59f0020 	ldr	r0, [pc, #32]	; 924c <print_mmc_devices+0x84>
    9228:	e59f3010 	ldr	r3, [pc, #16]	; 9240 <print_mmc_devices+0x78>
    922c:	e1a0e00f 	mov	lr, pc
    9230:	e593f000 	ldr	pc, [r3]
    9234:	e28dd004 	add	sp, sp, #4	; 0x4
    9238:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    923c:	3c00a0e4 	.word	0x3c00a0e4
    9240:	3c00a5cc 	.word	0x3c00a5cc
    9244:	0001897b 	.word	0x0001897b
    9248:	00018974 	.word	0x00018974
    924c:	000171fb 	.word	0x000171fb

00009250 <get_mmc_num>:
    9250:	e59f3004 	ldr	r3, [pc, #4]	; 925c <get_mmc_num+0xc>
    9254:	e5930000 	ldr	r0, [r3]
    9258:	e12fff1e 	bx	lr
    925c:	3c00a028 	.word	0x3c00a028

00009260 <mmc_vs_align>:
    9260:	e3a00c02 	mov	r0, #512	; 0x200
    9264:	e12fff1e 	bx	lr

00009268 <init_mmc>:
    9268:	e59f3060 	ldr	r3, [pc, #96]	; 92d0 <init_mmc+0x68>
    926c:	e59f2060 	ldr	r2, [pc, #96]	; 92d4 <init_mmc+0x6c>
    9270:	e92d4070 	push	{r4, r5, r6, lr}
    9274:	e3a04000 	mov	r4, #0	; 0x0
    9278:	e3a00b02 	mov	r0, #2048	; 0x800
    927c:	e5833004 	str	r3, [r3, #4]
    9280:	e5824000 	str	r4, [r2]
    9284:	e5833000 	str	r3, [r3]
    9288:	ebffe0da 	bl	15f8 <malloc>
    928c:	e1a05000 	mov	r5, r0
    9290:	e2806b01 	add	r6, r0, #1024	; 0x400
    9294:	e1a00005 	mov	r0, r5
    9298:	e3a01000 	mov	r1, #0	; 0x0
    929c:	e3a02f4e 	mov	r2, #312	; 0x138
    92a0:	ebffe067 	bl	1444 <memset>
    92a4:	e5856130 	str	r6, [r5, #304]
    92a8:	e1a00004 	mov	r0, r4
    92ac:	e1a01005 	mov	r1, r5
    92b0:	eb000490 	bl	a4f8 <imap_sdhc_initialize>
    92b4:	e3500000 	cmp	r0, #0	; 0x0
    92b8:	e2844001 	add	r4, r4, #1	; 0x1
    92bc:	e2855f4e 	add	r5, r5, #312	; 0x138
    92c0:	18bd8070 	popne	{r4, r5, r6, pc}
    92c4:	e3540003 	cmp	r4, #3	; 0x3
    92c8:	1afffff1 	bne	9294 <init_mmc+0x2c>
    92cc:	e8bd8070 	pop	{r4, r5, r6, pc}
    92d0:	3c00a0e4 	.word	0x3c00a0e4
    92d4:	3c00a028 	.word	0x3c00a028

000092d8 <sd_change_freq>:
    92d8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    92dc:	e5903050 	ldr	r3, [r0, #80]
    92e0:	e3a01000 	mov	r1, #0	; 0x0
    92e4:	e2132b01 	ands	r2, r3, #1024	; 0x400
    92e8:	e1a04000 	mov	r4, r0
    92ec:	e580104c 	str	r1, [r0, #76]
    92f0:	e24dd084 	sub	sp, sp, #132	; 0x84
    92f4:	11a00001 	movne	r0, r1
    92f8:	1a000070 	bne	94c0 <sd_change_freq+0x1e8>
    92fc:	e1d438b0 	ldrh	r3, [r4, #128]
    9300:	e3a05015 	mov	r5, #21	; 0x15
    9304:	e1a03803 	lsl	r3, r3, #16
    9308:	e58d3050 	str	r3, [sp, #80]
    930c:	e28d6048 	add	r6, sp, #72	; 0x48
    9310:	e3a03037 	mov	r3, #55	; 0x37
    9314:	e1cd34b8 	strh	r3, [sp, #72]
    9318:	e58d504c 	str	r5, [sp, #76]
    931c:	e58d2064 	str	r2, [sp, #100]
    9320:	e1a01006 	mov	r1, r6
    9324:	e1a0e00f 	mov	lr, pc
    9328:	e594f104 	ldr	pc, [r4, #260]
    932c:	e3500000 	cmp	r0, #0	; 0x0
    9330:	1a000062 	bne	94c0 <sd_change_freq+0x1e8>
    9334:	e3a03033 	mov	r3, #51	; 0x33
    9338:	e58d504c 	str	r5, [sp, #76]
    933c:	e1a09006 	mov	r9, r6
    9340:	e58d0064 	str	r0, [sp, #100]
    9344:	e1cd34b8 	strh	r3, [sp, #72]
    9348:	e58d0050 	str	r0, [sp, #80]
    934c:	e3a05003 	mov	r5, #3	; 0x3
    9350:	e28da078 	add	sl, sp, #120	; 0x78
    9354:	e3a08008 	mov	r8, #8	; 0x8
    9358:	e3a06001 	mov	r6, #1	; 0x1
    935c:	e28d7068 	add	r7, sp, #104	; 0x68
    9360:	e58da068 	str	sl, [sp, #104]
    9364:	e58d8074 	str	r8, [sp, #116]
    9368:	e58d6070 	str	r6, [sp, #112]
    936c:	e58d606c 	str	r6, [sp, #108]
    9370:	e1a00004 	mov	r0, r4
    9374:	e1a01009 	mov	r1, r9
    9378:	e1a02007 	mov	r2, r7
    937c:	e1a0e00f 	mov	lr, pc
    9380:	e594f104 	ldr	pc, [r4, #260]
    9384:	e250b000 	subs	fp, r0, #0	; 0x0
    9388:	0a000003 	beq	939c <sd_change_freq+0xc4>
    938c:	e3550000 	cmp	r5, #0	; 0x0
    9390:	e2455001 	sub	r5, r5, #1	; 0x1
    9394:	1afffff1 	bne	9360 <sd_change_freq+0x88>
    9398:	ea000047 	b	94bc <sd_change_freq+0x1e4>
    939c:	e59d3078 	ldr	r3, [sp, #120]
    93a0:	e59d007c 	ldr	r0, [sp, #124]
    93a4:	e1a01c03 	lsl	r1, r3, #24
    93a8:	e2032cff 	and	r2, r3, #65280	; 0xff00
    93ac:	e1811c23 	orr	r1, r1, r3, lsr #24
    93b0:	e1811402 	orr	r1, r1, r2, lsl #8
    93b4:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    93b8:	e1811423 	orr	r1, r1, r3, lsr #8
    93bc:	e1a03c00 	lsl	r3, r0, #24
    93c0:	e1833c20 	orr	r3, r3, r0, lsr #24
    93c4:	e2002cff 	and	r2, r0, #65280	; 0xff00
    93c8:	e1a0cc21 	lsr	ip, r1, #24
    93cc:	e1833402 	orr	r3, r3, r2, lsl #8
    93d0:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    93d4:	e20cc00f 	and	ip, ip, #15	; 0xf
    93d8:	e1833420 	orr	r3, r3, r0, lsr #8
    93dc:	e35c0001 	cmp	ip, #1	; 0x1
    93e0:	e584305c 	str	r3, [r4, #92]
    93e4:	e5841058 	str	r1, [r4, #88]
    93e8:	059f30d8 	ldreq	r3, [pc, #216]	; 94c8 <sd_change_freq+0x1f0>
    93ec:	0a000004 	beq	9404 <sd_change_freq+0x12c>
    93f0:	3a000002 	bcc	9400 <sd_change_freq+0x128>
    93f4:	e35c0002 	cmp	ip, #2	; 0x2
    93f8:	059f30cc 	ldreq	r3, [pc, #204]	; 94cc <sd_change_freq+0x1f4>
    93fc:	0a000000 	beq	9404 <sd_change_freq+0x12c>
    9400:	e59f30c8 	ldr	r3, [pc, #200]	; 94d0 <sd_change_freq+0x1f8>
    9404:	e5843030 	str	r3, [r4, #48]
    9408:	e5943058 	ldr	r3, [r4, #88]
    940c:	e5942030 	ldr	r2, [r4, #48]
    9410:	e3130701 	tst	r3, #262144	; 0x40000
    9414:	1594304c 	ldrne	r3, [r4, #76]
    9418:	13833c01 	orrne	r3, r3, #256	; 0x100
    941c:	1584304c 	strne	r3, [r4, #76]
    9420:	e59f30a8 	ldr	r3, [pc, #168]	; 94d0 <sd_change_freq+0x1f8>
    9424:	e1520003 	cmp	r2, r3
    9428:	13a05004 	movne	r5, #4	; 0x4
    942c:	128d6008 	addne	r6, sp, #8	; 0x8
    9430:	1a000007 	bne	9454 <sd_change_freq+0x17c>
    9434:	ea000020 	b	94bc <sd_change_freq+0x1e4>
    9438:	e58d6000 	str	r6, [sp]
    943c:	ebfffee8 	bl	8fe4 <sd_switch>
    9440:	e3500000 	cmp	r0, #0	; 0x0
    9444:	1a00001d 	bne	94c0 <sd_change_freq+0x1e8>
    9448:	e59d3024 	ldr	r3, [sp, #36]
    944c:	e3130c02 	tst	r3, #512	; 0x200
    9450:	0a000005 	beq	946c <sd_change_freq+0x194>
    9454:	e3a01000 	mov	r1, #0	; 0x0
    9458:	e2555001 	subs	r5, r5, #1	; 0x1
    945c:	e1a00004 	mov	r0, r4
    9460:	e1a02001 	mov	r2, r1
    9464:	e3a03001 	mov	r3, #1	; 0x1
    9468:	2afffff2 	bcs	9438 <sd_change_freq+0x160>
    946c:	e59d3014 	ldr	r3, [sp, #20]
    9470:	e3130c02 	tst	r3, #512	; 0x200
    9474:	0a000010 	beq	94bc <sd_change_freq+0x1e4>
    9478:	e3a01001 	mov	r1, #1	; 0x1
    947c:	e28dc008 	add	ip, sp, #8	; 0x8
    9480:	e1a00004 	mov	r0, r4
    9484:	e3a02000 	mov	r2, #0	; 0x0
    9488:	e1a03001 	mov	r3, r1
    948c:	e58dc000 	str	ip, [sp]
    9490:	ebfffed3 	bl	8fe4 <sd_switch>
    9494:	e3500000 	cmp	r0, #0	; 0x0
    9498:	1a000008 	bne	94c0 <sd_change_freq+0x1e8>
    949c:	e59d3018 	ldr	r3, [sp, #24]
    94a0:	e1a03c03 	lsl	r3, r3, #24
    94a4:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    94a8:	e3530401 	cmp	r3, #16777216	; 0x1000000
    94ac:	0594304c 	ldreq	r3, [r4, #76]
    94b0:	03833001 	orreq	r3, r3, #1	; 0x1
    94b4:	0584304c 	streq	r3, [r4, #76]
    94b8:	ea000000 	b	94c0 <sd_change_freq+0x1e8>
    94bc:	e1a0000b 	mov	r0, fp
    94c0:	e28dd084 	add	sp, sp, #132	; 0x84
    94c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    94c8:	0002001a 	.word	0x0002001a
    94cc:	00020020 	.word	0x00020020
    94d0:	00020010 	.word	0x00020010

000094d4 <sd_send_op_cond>:
    94d4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    94d8:	e59f9140 	ldr	r9, [pc, #320]	; 9620 <sd_send_op_cond+0x14c>
    94dc:	e24dd020 	sub	sp, sp, #32	; 0x20
    94e0:	e1a04000 	mov	r4, r0
    94e4:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    94e8:	e3a0a015 	mov	sl, #21	; 0x15
    94ec:	e3a06000 	mov	r6, #0	; 0x0
    94f0:	e1a0700d 	mov	r7, sp
    94f4:	e3a08001 	mov	r8, #1	; 0x1
    94f8:	e3a03037 	mov	r3, #55	; 0x37
    94fc:	e1cd30b0 	strh	r3, [sp]
    9500:	e1a0100d 	mov	r1, sp
    9504:	e1a02006 	mov	r2, r6
    9508:	e58da004 	str	sl, [sp, #4]
    950c:	e58d6008 	str	r6, [sp, #8]
    9510:	e58d601c 	str	r6, [sp, #28]
    9514:	e1a00004 	mov	r0, r4
    9518:	e1a0e00f 	mov	lr, pc
    951c:	e594f104 	ldr	pc, [r4, #260]
    9520:	e2503000 	subs	r3, r0, #0	; 0x0
    9524:	e1a0100d 	mov	r1, sp
    9528:	e3a02000 	mov	r2, #0	; 0x0
    952c:	e59fe0f0 	ldr	lr, [pc, #240]	; 9624 <sd_send_op_cond+0x150>
    9530:	e1a00004 	mov	r0, r4
    9534:	1a000036 	bne	9614 <sd_send_op_cond+0x140>
    9538:	e594302c 	ldr	r3, [r4, #44]
    953c:	e594c030 	ldr	ip, [r4, #48]
    9540:	e003e00e 	and	lr, r3, lr
    9544:	e15c0009 	cmp	ip, r9
    9548:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    954c:	e58de008 	str	lr, [sp, #8]
    9550:	058d3008 	streq	r3, [sp, #8]
    9554:	e3a03029 	mov	r3, #41	; 0x29
    9558:	e1cd30b0 	strh	r3, [sp]
    955c:	e58d8004 	str	r8, [sp, #4]
    9560:	e5943104 	ldr	r3, [r4, #260]
    9564:	e12fff33 	blx	r3
    9568:	e2503000 	subs	r3, r0, #0	; 0x0
    956c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    9570:	1a000027 	bne	9614 <sd_send_op_cond+0x140>
    9574:	eb001445 	bl	e690 <udelay>
    9578:	e59d300c 	ldr	r3, [sp, #12]
    957c:	e3530000 	cmp	r3, #0	; 0x0
    9580:	ba000002 	blt	9590 <sd_send_op_cond+0xbc>
    9584:	e2555001 	subs	r5, r5, #1	; 0x1
    9588:	2affffda 	bcs	94f8 <sd_send_op_cond+0x24>
    958c:	ea00001f 	b	9610 <sd_send_op_cond+0x13c>
    9590:	e3550000 	cmp	r5, #0	; 0x0
    9594:	da00001d 	ble	9610 <sd_send_op_cond+0x13c>
    9598:	e59f3080 	ldr	r3, [pc, #128]	; 9620 <sd_send_op_cond+0x14c>
    959c:	e5942030 	ldr	r2, [r4, #48]
    95a0:	e1520003 	cmp	r2, r3
    95a4:	12433010 	subne	r3, r3, #16	; 0x10
    95a8:	15843030 	strne	r3, [r4, #48]
    95ac:	e5943050 	ldr	r3, [r4, #80]
    95b0:	e3130b01 	tst	r3, #1024	; 0x400
    95b4:	0a00000c 	beq	95ec <sd_send_op_cond+0x118>
    95b8:	e3a03001 	mov	r3, #1	; 0x1
    95bc:	e3a02000 	mov	r2, #0	; 0x0
    95c0:	e58d3004 	str	r3, [sp, #4]
    95c4:	e3a0303a 	mov	r3, #58	; 0x3a
    95c8:	e1cd30b0 	strh	r3, [sp]
    95cc:	e58d2008 	str	r2, [sp, #8]
    95d0:	e58d201c 	str	r2, [sp, #28]
    95d4:	e1a00004 	mov	r0, r4
    95d8:	e1a0100d 	mov	r1, sp
    95dc:	e1a0e00f 	mov	lr, pc
    95e0:	e594f104 	ldr	pc, [r4, #260]
    95e4:	e2503000 	subs	r3, r0, #0	; 0x0
    95e8:	1a000009 	bne	9614 <sd_send_op_cond+0x140>
    95ec:	e59d200c 	ldr	r2, [sp, #12]
    95f0:	e3a03000 	mov	r3, #0	; 0x0
    95f4:	e1c438b0 	strh	r3, [r4, #128]
    95f8:	e1a03f22 	lsr	r3, r2, #30
    95fc:	e2033001 	and	r3, r3, #1	; 0x1
    9600:	e5843040 	str	r3, [r4, #64]
    9604:	e5842054 	str	r2, [r4, #84]
    9608:	e3a03000 	mov	r3, #0	; 0x0
    960c:	ea000000 	b	9614 <sd_send_op_cond+0x140>
    9610:	e3e03010 	mvn	r3, #16	; 0x10
    9614:	e1a00003 	mov	r0, r3
    9618:	e28dd020 	add	sp, sp, #32	; 0x20
    961c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9620:	00020020 	.word	0x00020020
    9624:	00ff8000 	.word	0x00ff8000

00009628 <mmc_go_idle>:
    9628:	e92d4010 	push	{r4, lr}
    962c:	e1a04000 	mov	r4, r0
    9630:	e24dd020 	sub	sp, sp, #32	; 0x20
    9634:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    9638:	eb001414 	bl	e690 <udelay>
    963c:	e3a02000 	mov	r2, #0	; 0x0
    9640:	e1a00004 	mov	r0, r4
    9644:	e1cd20b0 	strh	r2, [sp]
    9648:	e58d2008 	str	r2, [sp, #8]
    964c:	e58d2004 	str	r2, [sp, #4]
    9650:	e58d201c 	str	r2, [sp, #28]
    9654:	e1a0100d 	mov	r1, sp
    9658:	e1a0e00f 	mov	lr, pc
    965c:	e594f104 	ldr	pc, [r4, #260]
    9660:	e2504000 	subs	r4, r0, #0	; 0x0
    9664:	1a000001 	bne	9670 <mmc_go_idle+0x48>
    9668:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    966c:	eb001407 	bl	e690 <udelay>
    9670:	e1a00004 	mov	r0, r4
    9674:	e28dd020 	add	sp, sp, #32	; 0x20
    9678:	e8bd8010 	pop	{r4, pc}

0000967c <mmc_send_op_cond>:
    967c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    9680:	e24dd020 	sub	sp, sp, #32	; 0x20
    9684:	e1a05000 	mov	r5, r0
    9688:	e3a06001 	mov	r6, #1	; 0x1
    968c:	ebffffe5 	bl	9628 <mmc_go_idle>
    9690:	e3a02000 	mov	r2, #0	; 0x0
    9694:	e1cd60b0 	strh	r6, [sp]
    9698:	e58d6004 	str	r6, [sp, #4]
    969c:	e58d2008 	str	r2, [sp, #8]
    96a0:	e58d201c 	str	r2, [sp, #28]
    96a4:	e1a00005 	mov	r0, r5
    96a8:	e1a0100d 	mov	r1, sp
    96ac:	e1a0e00f 	mov	lr, pc
    96b0:	e595f104 	ldr	pc, [r5, #260]
    96b4:	e2504000 	subs	r4, r0, #0	; 0x0
    96b8:	e1a0700d 	mov	r7, sp
    96bc:	1a000039 	bne	97a8 <mmc_send_op_cond+0x12c>
    96c0:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    96c4:	eb0013f1 	bl	e690 <udelay>
    96c8:	e1a08006 	mov	r8, r6
    96cc:	e1a0700d 	mov	r7, sp
    96d0:	e1a06004 	mov	r6, r4
    96d4:	e3024710 	movw	r4, #10000	; 0x2710
    96d8:	e595302c 	ldr	r3, [r5, #44]
    96dc:	e3a02001 	mov	r2, #1	; 0x1
    96e0:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    96e4:	e1cd20b0 	strh	r2, [sp]
    96e8:	e58d3008 	str	r3, [sp, #8]
    96ec:	e1a02006 	mov	r2, r6
    96f0:	e58d8004 	str	r8, [sp, #4]
    96f4:	e58d601c 	str	r6, [sp, #28]
    96f8:	e1a0100d 	mov	r1, sp
    96fc:	e1a00005 	mov	r0, r5
    9700:	e1a0e00f 	mov	lr, pc
    9704:	e595f104 	ldr	pc, [r5, #260]
    9708:	e2502000 	subs	r2, r0, #0	; 0x0
    970c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    9710:	11a04002 	movne	r4, r2
    9714:	1a000023 	bne	97a8 <mmc_send_op_cond+0x12c>
    9718:	eb0013dc 	bl	e690 <udelay>
    971c:	e59d300c 	ldr	r3, [sp, #12]
    9720:	e3530000 	cmp	r3, #0	; 0x0
    9724:	ba000002 	blt	9734 <mmc_send_op_cond+0xb8>
    9728:	e2544001 	subs	r4, r4, #1	; 0x1
    972c:	2affffe9 	bcs	96d8 <mmc_send_op_cond+0x5c>
    9730:	ea00001b 	b	97a4 <mmc_send_op_cond+0x128>
    9734:	e3540000 	cmp	r4, #0	; 0x0
    9738:	da000019 	ble	97a4 <mmc_send_op_cond+0x128>
    973c:	e5953050 	ldr	r3, [r5, #80]
    9740:	e3130b01 	tst	r3, #1024	; 0x400
    9744:	0a00000b 	beq	9778 <mmc_send_op_cond+0xfc>
    9748:	e3a0303a 	mov	r3, #58	; 0x3a
    974c:	e58d8004 	str	r8, [sp, #4]
    9750:	e1cd30b0 	strh	r3, [sp]
    9754:	e58d6008 	str	r6, [sp, #8]
    9758:	e58d601c 	str	r6, [sp, #28]
    975c:	e1a0100d 	mov	r1, sp
    9760:	e1a02006 	mov	r2, r6
    9764:	e1a00005 	mov	r0, r5
    9768:	e1a0e00f 	mov	lr, pc
    976c:	e595f104 	ldr	pc, [r5, #260]
    9770:	e2504000 	subs	r4, r0, #0	; 0x0
    9774:	1a00000b 	bne	97a8 <mmc_send_op_cond+0x12c>
    9778:	e59d300c 	ldr	r3, [sp, #12]
    977c:	e3a01801 	mov	r1, #65536	; 0x10000
    9780:	e1a02f23 	lsr	r2, r3, #30
    9784:	e2022001 	and	r2, r2, #1	; 0x1
    9788:	e5852040 	str	r2, [r5, #64]
    978c:	e3a02000 	mov	r2, #0	; 0x0
    9790:	e5851030 	str	r1, [r5, #48]
    9794:	e1c528b0 	strh	r2, [r5, #128]
    9798:	e5853054 	str	r3, [r5, #84]
    979c:	e3a04000 	mov	r4, #0	; 0x0
    97a0:	ea000000 	b	97a8 <mmc_send_op_cond+0x12c>
    97a4:	e3e04010 	mvn	r4, #16	; 0x10
    97a8:	e1a00004 	mov	r0, r4
    97ac:	e28dd020 	add	sp, sp, #32	; 0x20
    97b0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000097b4 <mmc_send_status>:
    97b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    97b8:	e1d038b0 	ldrh	r3, [r0, #128]
    97bc:	e24dd024 	sub	sp, sp, #36	; 0x24
    97c0:	e1a03803 	lsl	r3, r3, #16
    97c4:	e58d3008 	str	r3, [sp, #8]
    97c8:	e3a03000 	mov	r3, #0	; 0x0
    97cc:	e3a02015 	mov	r2, #21	; 0x15
    97d0:	e58d301c 	str	r3, [sp, #28]
    97d4:	e3a0300d 	mov	r3, #13	; 0xd
    97d8:	e1a06000 	mov	r6, r0
    97dc:	e58d2004 	str	r2, [sp, #4]
    97e0:	e1a04001 	mov	r4, r1
    97e4:	e1cd30b0 	strh	r3, [sp]
    97e8:	e1a0700d 	mov	r7, sp
    97ec:	e1a0100d 	mov	r1, sp
    97f0:	e3a02000 	mov	r2, #0	; 0x0
    97f4:	e1a00006 	mov	r0, r6
    97f8:	e1a0e00f 	mov	lr, pc
    97fc:	e596f104 	ldr	pc, [r6, #260]
    9800:	e2505000 	subs	r5, r0, #0	; 0x0
    9804:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    9808:	1a000018 	bne	9870 <mmc_send_status+0xbc>
    980c:	e59d300c 	ldr	r3, [sp, #12]
    9810:	e3130c01 	tst	r3, #256	; 0x100
    9814:	1a00000e 	bne	9854 <mmc_send_status+0xa0>
    9818:	eb00139c 	bl	e690 <udelay>
    981c:	e59d100c 	ldr	r1, [sp, #12]
    9820:	e59f3054 	ldr	r3, [pc, #84]	; 987c <mmc_send_status+0xc8>
    9824:	e2444001 	sub	r4, r4, #1	; 0x1
    9828:	e0013003 	and	r3, r1, r3
    982c:	e3530000 	cmp	r3, #0	; 0x0
    9830:	0a000005 	beq	984c <mmc_send_status+0x98>
    9834:	e59f0044 	ldr	r0, [pc, #68]	; 9880 <mmc_send_status+0xcc>
    9838:	e59f3044 	ldr	r3, [pc, #68]	; 9884 <mmc_send_status+0xd0>
    983c:	e1a0e00f 	mov	lr, pc
    9840:	e593f000 	ldr	pc, [r3]
    9844:	e3e05011 	mvn	r5, #17	; 0x11
    9848:	ea000008 	b	9870 <mmc_send_status+0xbc>
    984c:	e3740001 	cmn	r4, #1	; 0x1
    9850:	1affffe5 	bne	97ec <mmc_send_status+0x38>
    9854:	e3540000 	cmp	r4, #0	; 0x0
    9858:	1a000004 	bne	9870 <mmc_send_status+0xbc>
    985c:	e59f0024 	ldr	r0, [pc, #36]	; 9888 <mmc_send_status+0xd4>
    9860:	e59f301c 	ldr	r3, [pc, #28]	; 9884 <mmc_send_status+0xd0>
    9864:	e1a0e00f 	mov	lr, pc
    9868:	e593f000 	ldr	pc, [r3]
    986c:	e3e05012 	mvn	r5, #18	; 0x12
    9870:	e1a00005 	mov	r0, r5
    9874:	e28dd024 	add	sp, sp, #36	; 0x24
    9878:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    987c:	fdf94080 	.word	0xfdf94080
    9880:	0001897f 	.word	0x0001897f
    9884:	3c00a5cc 	.word	0x3c00a5cc
    9888:	00018995 	.word	0x00018995

0000988c <mmc_bwrite>:
    988c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9890:	e24dd034 	sub	sp, sp, #52	; 0x34
    9894:	e1a06001 	mov	r6, r1
    9898:	e1a0b002 	mov	fp, r2
    989c:	e1a0a003 	mov	sl, r3
    98a0:	ebfffda2 	bl	8f30 <find_mmc_device>
    98a4:	e2504000 	subs	r4, r0, #0	; 0x0
    98a8:	0a000060 	beq	9a30 <mmc_bwrite+0x1a4>
    98ac:	e594108c 	ldr	r1, [r4, #140]
    98b0:	ebfffd90 	bl	8ef8 <mmc_set_blocklen>
    98b4:	e3500000 	cmp	r0, #0	; 0x0
    98b8:	01a09000 	moveq	r9, r0
    98bc:	01a0700b 	moveq	r7, fp
    98c0:	0a000005 	beq	98dc <mmc_bwrite+0x50>
    98c4:	e594108c 	ldr	r1, [r4, #140]
    98c8:	e59f016c 	ldr	r0, [pc, #364]	; 9a3c <mmc_bwrite+0x1b0>
    98cc:	e59f316c 	ldr	r3, [pc, #364]	; 9a40 <mmc_bwrite+0x1b4>
    98d0:	e1a0e00f 	mov	lr, pc
    98d4:	e593f000 	ldr	pc, [r3]
    98d8:	ea000054 	b	9a30 <mmc_bwrite+0x1a4>
    98dc:	e5943110 	ldr	r3, [r4, #272]
    98e0:	e59420a8 	ldr	r2, [r4, #168]
    98e4:	e1570003 	cmp	r7, r3
    98e8:	31a05007 	movcc	r5, r7
    98ec:	21a05003 	movcs	r5, r3
    98f0:	e0858006 	add	r8, r5, r6
    98f4:	e1580002 	cmp	r8, r2
    98f8:	9a000005 	bls	9914 <mmc_bwrite+0x88>
    98fc:	e59f0140 	ldr	r0, [pc, #320]	; 9a44 <mmc_bwrite+0x1b8>
    9900:	e1a01008 	mov	r1, r8
    9904:	e59f3134 	ldr	r3, [pc, #308]	; 9a40 <mmc_bwrite+0x1b4>
    9908:	e1a0e00f 	mov	lr, pc
    990c:	e593f000 	ldr	pc, [r3]
    9910:	ea00003d 	b	9a0c <mmc_bwrite+0x180>
    9914:	e3550001 	cmp	r5, #1	; 0x1
    9918:	93a03018 	movls	r3, #24	; 0x18
    991c:	91cd30b0 	strhls	r3, [sp]
    9920:	e5943040 	ldr	r3, [r4, #64]
    9924:	83a02019 	movhi	r2, #25	; 0x19
    9928:	81cd20b0 	strhhi	r2, [sp]
    992c:	e3530000 	cmp	r3, #0	; 0x0
    9930:	0594308c 	ldreq	r3, [r4, #140]
    9934:	e3a02015 	mov	r2, #21	; 0x15
    9938:	00030693 	muleq	r3, r3, r6
    993c:	158d6008 	strne	r6, [sp, #8]
    9940:	058d3008 	streq	r3, [sp, #8]
    9944:	e58d2004 	str	r2, [sp, #4]
    9948:	e58d901c 	str	r9, [sp, #28]
    994c:	e58da020 	str	sl, [sp, #32]
    9950:	e58d5028 	str	r5, [sp, #40]
    9954:	e594308c 	ldr	r3, [r4, #140]
    9958:	e3a02002 	mov	r2, #2	; 0x2
    995c:	e58d2024 	str	r2, [sp, #36]
    9960:	e58d302c 	str	r3, [sp, #44]
    9964:	e1a00004 	mov	r0, r4
    9968:	e1a0100d 	mov	r1, sp
    996c:	e28d2020 	add	r2, sp, #32	; 0x20
    9970:	e1a0e00f 	mov	lr, pc
    9974:	e594f104 	ldr	pc, [r4, #260]
    9978:	e3500000 	cmp	r0, #0	; 0x0
    997c:	0a000004 	beq	9994 <mmc_bwrite+0x108>
    9980:	e59f00c0 	ldr	r0, [pc, #192]	; 9a48 <mmc_bwrite+0x1bc>
    9984:	e59f30b4 	ldr	r3, [pc, #180]	; 9a40 <mmc_bwrite+0x1b4>
    9988:	e1a0e00f 	mov	lr, pc
    998c:	e593f000 	ldr	pc, [r3]
    9990:	ea00001d 	b	9a0c <mmc_bwrite+0x180>
    9994:	e5943050 	ldr	r3, [r4, #80]
    9998:	e1a03523 	lsr	r3, r3, #10
    999c:	e2233001 	eor	r3, r3, #1	; 0x1
    99a0:	e3550001 	cmp	r5, #1	; 0x1
    99a4:	93a03000 	movls	r3, #0	; 0x0
    99a8:	82033001 	andhi	r3, r3, #1	; 0x1
    99ac:	e3530000 	cmp	r3, #0	; 0x0
    99b0:	0a000011 	beq	99fc <mmc_bwrite+0x170>
    99b4:	e3a0200c 	mov	r2, #12	; 0xc
    99b8:	e3a0301d 	mov	r3, #29	; 0x1d
    99bc:	e1cd20b0 	strh	r2, [sp]
    99c0:	e98d0208 	stmib	sp, {r3, r9}
    99c4:	e58d901c 	str	r9, [sp, #28]
    99c8:	e1a00004 	mov	r0, r4
    99cc:	e1a0100d 	mov	r1, sp
    99d0:	e1a02009 	mov	r2, r9
    99d4:	e1a0e00f 	mov	lr, pc
    99d8:	e594f104 	ldr	pc, [r4, #260]
    99dc:	e3500000 	cmp	r0, #0	; 0x0
    99e0:	0a000005 	beq	99fc <mmc_bwrite+0x170>
    99e4:	e59f0060 	ldr	r0, [pc, #96]	; 9a4c <mmc_bwrite+0x1c0>
    99e8:	e59f3050 	ldr	r3, [pc, #80]	; 9a40 <mmc_bwrite+0x1b4>
    99ec:	e1a0e00f 	mov	lr, pc
    99f0:	e593f000 	ldr	pc, [r3]
    99f4:	e1a00009 	mov	r0, r9
    99f8:	ea00000d 	b	9a34 <mmc_bwrite+0x1a8>
    99fc:	e1a00004 	mov	r0, r4
    9a00:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9a04:	ebffff6a 	bl	97b4 <mmc_send_status>
    9a08:	ea000001 	b	9a14 <mmc_bwrite+0x188>
    9a0c:	e3550000 	cmp	r5, #0	; 0x0
    9a10:	1a000006 	bne	9a30 <mmc_bwrite+0x1a4>
    9a14:	e0577005 	subs	r7, r7, r5
    9a18:	e594008c 	ldr	r0, [r4, #140]
    9a1c:	102aa590 	mlane	sl, r0, r5, sl
    9a20:	11a06008 	movne	r6, r8
    9a24:	1affffac 	bne	98dc <mmc_bwrite+0x50>
    9a28:	e000009b 	mul	r0, fp, r0
    9a2c:	ea000000 	b	9a34 <mmc_bwrite+0x1a8>
    9a30:	e3a00000 	mov	r0, #0	; 0x0
    9a34:	e28dd034 	add	sp, sp, #52	; 0x34
    9a38:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    9a3c:	000189b1 	.word	0x000189b1
    9a40:	3c00a5cc 	.word	0x3c00a5cc
    9a44:	000189bb 	.word	0x000189bb
    9a48:	000189e7 	.word	0x000189e7
    9a4c:	000189f9 	.word	0x000189f9

00009a50 <mmc2_vs_write>:
    9a50:	e1a024a2 	lsr	r2, r2, #9
    9a54:	e1821b83 	orr	r1, r2, r3, lsl #23
    9a58:	e59d2000 	ldr	r2, [sp]
    9a5c:	e1a03000 	mov	r3, r0
    9a60:	e1a024c2 	asr	r2, r2, #9
    9a64:	e3a00002 	mov	r0, #2	; 0x2
    9a68:	eaffff87 	b	988c <mmc_bwrite>

00009a6c <mmc1_vs_write>:
    9a6c:	e1a024a2 	lsr	r2, r2, #9
    9a70:	e1821b83 	orr	r1, r2, r3, lsl #23
    9a74:	e59d2000 	ldr	r2, [sp]
    9a78:	e1a03000 	mov	r3, r0
    9a7c:	e1a024c2 	asr	r2, r2, #9
    9a80:	e3a00001 	mov	r0, #1	; 0x1
    9a84:	eaffff80 	b	988c <mmc_bwrite>

00009a88 <mmc0_vs_write>:
    9a88:	e1a024a2 	lsr	r2, r2, #9
    9a8c:	e1821b83 	orr	r1, r2, r3, lsl #23
    9a90:	e59d2000 	ldr	r2, [sp]
    9a94:	e1a03000 	mov	r3, r0
    9a98:	e1a024c2 	asr	r2, r2, #9
    9a9c:	e3a00000 	mov	r0, #0	; 0x0
    9aa0:	eaffff79 	b	988c <mmc_bwrite>

00009aa4 <mmc_switch>:
    9aa4:	e92d4030 	push	{r4, r5, lr}
    9aa8:	e1a02802 	lsl	r2, r2, #16
    9aac:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    9ab0:	e24dd024 	sub	sp, sp, #36	; 0x24
    9ab4:	e1822403 	orr	r2, r2, r3, lsl #8
    9ab8:	e3a0301d 	mov	r3, #29	; 0x1d
    9abc:	e3a01000 	mov	r1, #0	; 0x0
    9ac0:	e58d3004 	str	r3, [sp, #4]
    9ac4:	e3a03006 	mov	r3, #6	; 0x6
    9ac8:	e58d2008 	str	r2, [sp, #8]
    9acc:	e1cd30b0 	strh	r3, [sp]
    9ad0:	e1a02001 	mov	r2, r1
    9ad4:	e58d101c 	str	r1, [sp, #28]
    9ad8:	e1a0100d 	mov	r1, sp
    9adc:	e1a04000 	mov	r4, r0
    9ae0:	e1a0e00f 	mov	lr, pc
    9ae4:	e590f104 	ldr	pc, [r0, #260]
    9ae8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9aec:	e1a05000 	mov	r5, r0
    9af0:	e1a00004 	mov	r0, r4
    9af4:	ebffff2e 	bl	97b4 <mmc_send_status>
    9af8:	e1a00005 	mov	r0, r5
    9afc:	e28dd024 	add	sp, sp, #36	; 0x24
    9b00:	e8bd8030 	pop	{r4, r5, pc}

00009b04 <mmc_change_freq>:
    9b04:	e92d4070 	push	{r4, r5, r6, lr}
    9b08:	e5903050 	ldr	r3, [r0, #80]
    9b0c:	e24ddc02 	sub	sp, sp, #512	; 0x200
    9b10:	e3130b01 	tst	r3, #1024	; 0x400
    9b14:	e3a03000 	mov	r3, #0	; 0x0
    9b18:	e1a04000 	mov	r4, r0
    9b1c:	e580304c 	str	r3, [r0, #76]
    9b20:	1a000020 	bne	9ba8 <mmc_change_freq+0xa4>
    9b24:	e5902030 	ldr	r2, [r0, #48]
    9b28:	e59f3084 	ldr	r3, [pc, #132]	; 9bb4 <mmc_change_freq+0xb0>
    9b2c:	e1520003 	cmp	r2, r3
    9b30:	9a00001c 	bls	9ba8 <mmc_change_freq+0xa4>
    9b34:	e3a03c01 	mov	r3, #256	; 0x100
    9b38:	e580304c 	str	r3, [r0, #76]
    9b3c:	e1a0100d 	mov	r1, sp
    9b40:	ebfffd12 	bl	8f90 <mmc_send_ext_csd>
    9b44:	e3500000 	cmp	r0, #0	; 0x0
    9b48:	e1a0600d 	mov	r6, sp
    9b4c:	1a000016 	bne	9bac <mmc_change_freq+0xa8>
    9b50:	e3a01001 	mov	r1, #1	; 0x1
    9b54:	e1a00004 	mov	r0, r4
    9b58:	e3a020b9 	mov	r2, #185	; 0xb9
    9b5c:	e1a03001 	mov	r3, r1
    9b60:	e5dd50c4 	ldrb	r5, [sp, #196]
    9b64:	ebffffce 	bl	9aa4 <mmc_switch>
    9b68:	e3500000 	cmp	r0, #0	; 0x0
    9b6c:	1a00000e 	bne	9bac <mmc_change_freq+0xa8>
    9b70:	e1a0100d 	mov	r1, sp
    9b74:	e1a00004 	mov	r0, r4
    9b78:	ebfffd04 	bl	8f90 <mmc_send_ext_csd>
    9b7c:	e3500000 	cmp	r0, #0	; 0x0
    9b80:	1a000009 	bne	9bac <mmc_change_freq+0xa8>
    9b84:	e5dd30b9 	ldrb	r3, [sp, #185]
    9b88:	e3530000 	cmp	r3, #0	; 0x0
    9b8c:	0a000006 	beq	9bac <mmc_change_freq+0xa8>
    9b90:	e594304c 	ldr	r3, [r4, #76]
    9b94:	e3150002 	tst	r5, #2	; 0x2
    9b98:	13833011 	orrne	r3, r3, #17	; 0x11
    9b9c:	03833001 	orreq	r3, r3, #1	; 0x1
    9ba0:	e584304c 	str	r3, [r4, #76]
    9ba4:	ea000000 	b	9bac <mmc_change_freq+0xa8>
    9ba8:	e3a00000 	mov	r0, #0	; 0x0
    9bac:	e28ddc02 	add	sp, sp, #512	; 0x200
    9bb0:	e8bd8070 	pop	{r4, r5, r6, pc}
    9bb4:	0001003f 	.word	0x0001003f

00009bb8 <mmc_startup>:
    9bb8:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    9bbc:	e5903050 	ldr	r3, [r0, #80]
    9bc0:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    9bc4:	e3130b01 	tst	r3, #1024	; 0x400
    9bc8:	13a0300a 	movne	r3, #10	; 0xa
    9bcc:	03a03002 	moveq	r3, #2	; 0x2
    9bd0:	e28d1c02 	add	r1, sp, #512	; 0x200
    9bd4:	e3a02000 	mov	r2, #0	; 0x0
    9bd8:	e1c131b0 	strh	r3, [r1, #16]
    9bdc:	e28d5e21 	add	r5, sp, #528	; 0x210
    9be0:	e3a03007 	mov	r3, #7	; 0x7
    9be4:	e58d3214 	str	r3, [sp, #532]
    9be8:	e58d2218 	str	r2, [sp, #536]
    9bec:	e58d222c 	str	r2, [sp, #556]
    9bf0:	e1a01005 	mov	r1, r5
    9bf4:	e1a06000 	mov	r6, r0
    9bf8:	e1a0e00f 	mov	lr, pc
    9bfc:	e590f104 	ldr	pc, [r0, #260]
    9c00:	e2504000 	subs	r4, r0, #0	; 0x0
    9c04:	1a000159 	bne	a170 <mmc_startup+0x5b8>
    9c08:	e3a02010 	mov	r2, #16	; 0x10
    9c0c:	e2860070 	add	r0, r6, #112	; 0x70
    9c10:	e285100c 	add	r1, r5, #12	; 0xc
    9c14:	ebffde12 	bl	1464 <memcpy>
    9c18:	e5963050 	ldr	r3, [r6, #80]
    9c1c:	e2132b01 	ands	r2, r3, #1024	; 0x400
    9c20:	1a000013 	bne	9c74 <mmc_startup+0xbc>
    9c24:	e1d638b0 	ldrh	r3, [r6, #128]
    9c28:	e3a00003 	mov	r0, #3	; 0x3
    9c2c:	e1a03803 	lsl	r3, r3, #16
    9c30:	e58d3218 	str	r3, [sp, #536]
    9c34:	e3a03015 	mov	r3, #21	; 0x15
    9c38:	e58d3214 	str	r3, [sp, #532]
    9c3c:	e28d3c02 	add	r3, sp, #512	; 0x200
    9c40:	e1c301b0 	strh	r0, [r3, #16]
    9c44:	e58d222c 	str	r2, [sp, #556]
    9c48:	e1a01005 	mov	r1, r5
    9c4c:	e1a00006 	mov	r0, r6
    9c50:	e1a0e00f 	mov	lr, pc
    9c54:	e596f104 	ldr	pc, [r6, #260]
    9c58:	e2504000 	subs	r4, r0, #0	; 0x0
    9c5c:	1a000143 	bne	a170 <mmc_startup+0x5b8>
    9c60:	e5963030 	ldr	r3, [r6, #48]
    9c64:	e3130802 	tst	r3, #131072	; 0x20000
    9c68:	128d1c02 	addne	r1, sp, #512	; 0x200
    9c6c:	11d111be 	ldrhne	r1, [r1, #30]
    9c70:	11c618b0 	strhne	r1, [r6, #128]
    9c74:	e1d638b0 	ldrh	r3, [r6, #128]
    9c78:	e3a01000 	mov	r1, #0	; 0x0
    9c7c:	e1a03803 	lsl	r3, r3, #16
    9c80:	e3a02007 	mov	r2, #7	; 0x7
    9c84:	e58d3218 	str	r3, [sp, #536]
    9c88:	e3a00009 	mov	r0, #9	; 0x9
    9c8c:	e28d3c02 	add	r3, sp, #512	; 0x200
    9c90:	e1c301b0 	strh	r0, [r3, #16]
    9c94:	e58d2214 	str	r2, [sp, #532]
    9c98:	e58d122c 	str	r1, [sp, #556]
    9c9c:	e1a02001 	mov	r2, r1
    9ca0:	e1a00006 	mov	r0, r6
    9ca4:	e28d1e21 	add	r1, sp, #528	; 0x210
    9ca8:	e1a0e00f 	mov	lr, pc
    9cac:	e596f104 	ldr	pc, [r6, #260]
    9cb0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9cb4:	e1a04000 	mov	r4, r0
    9cb8:	e1a00006 	mov	r0, r6
    9cbc:	ebfffebc 	bl	97b4 <mmc_send_status>
    9cc0:	e3540000 	cmp	r4, #0	; 0x0
    9cc4:	1a000129 	bne	a170 <mmc_startup+0x5b8>
    9cc8:	e5963030 	ldr	r3, [r6, #48]
    9ccc:	e59d021c 	ldr	r0, [sp, #540]
    9cd0:	e59d2224 	ldr	r2, [sp, #548]
    9cd4:	e59d1228 	ldr	r1, [sp, #552]
    9cd8:	e3530801 	cmp	r3, #65536	; 0x10000
    9cdc:	e59d3220 	ldr	r3, [sp, #544]
    9ce0:	e5862068 	str	r2, [r6, #104]
    9ce4:	e5863064 	str	r3, [r6, #100]
    9ce8:	e586106c 	str	r1, [r6, #108]
    9cec:	e5860060 	str	r0, [r6, #96]
    9cf0:	1a000013 	bne	9d44 <mmc_startup+0x18c>
    9cf4:	e1a03d20 	lsr	r3, r0, #26
    9cf8:	e203300f 	and	r3, r3, #15	; 0xf
    9cfc:	e3530004 	cmp	r3, #4	; 0x4
    9d00:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    9d04:	ea00000c 	b	9d3c <mmc_startup+0x184>
    9d08:	00009d3c 	.word	0x00009d3c
    9d0c:	00009d1c 	.word	0x00009d1c
    9d10:	00009d24 	.word	0x00009d24
    9d14:	00009d2c 	.word	0x00009d2c
    9d18:	00009d34 	.word	0x00009d34
    9d1c:	e59f3458 	ldr	r3, [pc, #1112]	; a17c <mmc_startup+0x5c4>
    9d20:	ea000006 	b	9d40 <mmc_startup+0x188>
    9d24:	e59f3454 	ldr	r3, [pc, #1108]	; a180 <mmc_startup+0x5c8>
    9d28:	ea000004 	b	9d40 <mmc_startup+0x188>
    9d2c:	e59f3450 	ldr	r3, [pc, #1104]	; a184 <mmc_startup+0x5cc>
    9d30:	ea000002 	b	9d40 <mmc_startup+0x188>
    9d34:	e59f344c 	ldr	r3, [pc, #1100]	; a188 <mmc_startup+0x5d0>
    9d38:	ea000000 	b	9d40 <mmc_startup+0x188>
    9d3c:	e59f3448 	ldr	r3, [pc, #1096]	; a18c <mmc_startup+0x5d4>
    9d40:	e5863030 	str	r3, [r6, #48]
    9d44:	e28d1c02 	add	r1, sp, #512	; 0x200
    9d48:	e1d132b2 	ldrh	r3, [r1, #34]
    9d4c:	e3a0e001 	mov	lr, #1	; 0x1
    9d50:	e203300f 	and	r3, r3, #15	; 0xf
    9d54:	e1a0c31e 	lsl	ip, lr, r3
    9d58:	e59d121c 	ldr	r1, [sp, #540]
    9d5c:	e59f242c 	ldr	r2, [pc, #1068]	; a190 <mmc_startup+0x5d8>
    9d60:	e1a031a1 	lsr	r3, r1, #3
    9d64:	e203300f 	and	r3, r3, #15	; 0xf
    9d68:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    9d6c:	e59f3420 	ldr	r3, [pc, #1056]	; a194 <mmc_startup+0x5dc>
    9d70:	e2011007 	and	r1, r1, #7	; 0x7
    9d74:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    9d78:	e5962030 	ldr	r2, [r6, #48]
    9d7c:	e0030093 	mul	r3, r3, r0
    9d80:	e3120802 	tst	r2, #131072	; 0x20000
    9d84:	e5863084 	str	r3, [r6, #132]
    9d88:	e586c088 	str	ip, [r6, #136]
    9d8c:	059d3228 	ldreq	r3, [sp, #552]
    9d90:	1586c08c 	strne	ip, [r6, #140]
    9d94:	01a03b23 	lsreq	r3, r3, #22
    9d98:	0203300f 	andeq	r3, r3, #15	; 0xf
    9d9c:	01a0331e 	lsleq	r3, lr, r3
    9da0:	0586308c 	streq	r3, [r6, #140]
    9da4:	e5963040 	ldr	r3, [r6, #64]
    9da8:	e5962064 	ldr	r2, [r6, #100]
    9dac:	e3530000 	cmp	r3, #0	; 0x0
    9db0:	e5960068 	ldr	r0, [r6, #104]
    9db4:	0a000007 	beq	9dd8 <mmc_startup+0x220>
    9db8:	e202203f 	and	r2, r2, #63	; 0x3f
    9dbc:	e1a03820 	lsr	r3, r0, #16
    9dc0:	e1833802 	orr	r3, r3, r2, lsl #16
    9dc4:	e1a08003 	mov	r8, r3
    9dc8:	e3a09000 	mov	r9, #0	; 0x0
    9dcc:	e3a04008 	mov	r4, #8	; 0x8
    9dd0:	e3a05000 	mov	r5, #0	; 0x0
    9dd4:	ea000009 	b	9e00 <mmc_startup+0x248>
    9dd8:	e1a03b02 	lsl	r3, r2, #22
    9ddc:	e1a03b23 	lsr	r3, r3, #22
    9de0:	e200190e 	and	r1, r0, #229376	; 0x38000
    9de4:	e1a02f20 	lsr	r2, r0, #30
    9de8:	e1822103 	orr	r2, r2, r3, lsl #2
    9dec:	e1a017a1 	lsr	r1, r1, #15
    9df0:	e1a08002 	mov	r8, r2
    9df4:	e3a09000 	mov	r9, #0	; 0x0
    9df8:	e1a04001 	mov	r4, r1
    9dfc:	e3a05000 	mov	r5, #0	; 0x0
    9e00:	e3a02001 	mov	r2, #1	; 0x1
    9e04:	e0980002 	adds	r0, r8, r2
    9e08:	e3a03000 	mov	r3, #0	; 0x0
    9e0c:	e0a91003 	adc	r1, r9, r3
    9e10:	e2842002 	add	r2, r4, #2	; 0x2
    9e14:	eb002cf8 	bl	151fc <__aeabi_llsl>
    9e18:	e596c088 	ldr	ip, [r6, #136]
    9e1c:	e0832c90 	umull	r2, r3, r0, ip
    9e20:	e023319c 	mla	r3, ip, r1, r3
    9e24:	e35c0c02 	cmp	ip, #512	; 0x200
    9e28:	e1c629f0 	strd	r2, [r6, #144]
    9e2c:	83a03c02 	movhi	r3, #512	; 0x200
    9e30:	85863088 	strhi	r3, [r6, #136]
    9e34:	e596308c 	ldr	r3, [r6, #140]
    9e38:	e3530c02 	cmp	r3, #512	; 0x200
    9e3c:	83a03c02 	movhi	r3, #512	; 0x200
    9e40:	8586308c 	strhi	r3, [r6, #140]
    9e44:	e5963050 	ldr	r3, [r6, #80]
    9e48:	e2131b01 	ands	r1, r3, #1024	; 0x400
    9e4c:	1a00000f 	bne	9e90 <mmc_startup+0x2d8>
    9e50:	e1d638b0 	ldrh	r3, [r6, #128]
    9e54:	e3a0201d 	mov	r2, #29	; 0x1d
    9e58:	e1a03803 	lsl	r3, r3, #16
    9e5c:	e58d2214 	str	r2, [sp, #532]
    9e60:	e58d3218 	str	r3, [sp, #536]
    9e64:	e28d2c02 	add	r2, sp, #512	; 0x200
    9e68:	e3a03007 	mov	r3, #7	; 0x7
    9e6c:	e1c231b0 	strh	r3, [r2, #16]
    9e70:	e58d122c 	str	r1, [sp, #556]
    9e74:	e1a02001 	mov	r2, r1
    9e78:	e1a00006 	mov	r0, r6
    9e7c:	e28d1e21 	add	r1, sp, #528	; 0x210
    9e80:	e1a0e00f 	mov	lr, pc
    9e84:	e596f104 	ldr	pc, [r6, #260]
    9e88:	e2504000 	subs	r4, r0, #0	; 0x0
    9e8c:	1a0000b7 	bne	a170 <mmc_startup+0x5b8>
    9e90:	e5962030 	ldr	r2, [r6, #48]
    9e94:	e3e03000 	mvn	r3, #0	; 0x0
    9e98:	e3120802 	tst	r2, #131072	; 0x20000
    9e9c:	e5c63082 	strb	r3, [r6, #130]
    9ea0:	1a00001e 	bne	9f20 <mmc_startup+0x368>
    9ea4:	e59f32ec 	ldr	r3, [pc, #748]	; a198 <mmc_startup+0x5e0>
    9ea8:	e1520003 	cmp	r2, r3
    9eac:	9a00001b 	bls	9f20 <mmc_startup+0x368>
    9eb0:	e28d1010 	add	r1, sp, #16	; 0x10
    9eb4:	e1a00006 	mov	r0, r6
    9eb8:	ebfffc34 	bl	8f90 <mmc_send_ext_csd>
    9ebc:	e5dd30d0 	ldrb	r3, [sp, #208]
    9ec0:	e2701001 	rsbs	r1, r0, #1	; 0x1
    9ec4:	33a01000 	movcc	r1, #0	; 0x0
    9ec8:	e3530001 	cmp	r3, #1	; 0x1
    9ecc:	93a01000 	movls	r1, #0	; 0x0
    9ed0:	e3510000 	cmp	r1, #0	; 0x0
    9ed4:	0a00000d 	beq	9f10 <mmc_startup+0x358>
    9ed8:	e5dd20e6 	ldrb	r2, [sp, #230]
    9edc:	e5dd10e5 	ldrb	r1, [sp, #229]
    9ee0:	e5dd00e4 	ldrb	r0, [sp, #228]
    9ee4:	e1a02802 	lsl	r2, r2, #16
    9ee8:	e1822401 	orr	r2, r2, r1, lsl #8
    9eec:	e5dd10e7 	ldrb	r1, [sp, #231]
    9ef0:	e1822000 	orr	r2, r2, r0
    9ef4:	e1822c01 	orr	r2, r2, r1, lsl #24
    9ef8:	e1a03fc2 	asr	r3, r2, #31
    9efc:	e1a01483 	lsl	r1, r3, #9
    9f00:	e1811ba2 	orr	r1, r1, r2, lsr #23
    9f04:	e1a02482 	lsl	r2, r2, #9
    9f08:	e5861094 	str	r1, [r6, #148]
    9f0c:	e5862090 	str	r2, [r6, #144]
    9f10:	e5dd30b0 	ldrb	r3, [sp, #176]
    9f14:	e3130001 	tst	r3, #1	; 0x1
    9f18:	15dd30c3 	ldrbne	r3, [sp, #195]
    9f1c:	15c63082 	strbne	r3, [r6, #130]
    9f20:	e5963030 	ldr	r3, [r6, #48]
    9f24:	e3130802 	tst	r3, #131072	; 0x20000
    9f28:	0a000002 	beq	9f38 <mmc_startup+0x380>
    9f2c:	e1a00006 	mov	r0, r6
    9f30:	ebfffce8 	bl	92d8 <sd_change_freq>
    9f34:	ea000001 	b	9f40 <mmc_startup+0x388>
    9f38:	e1a00006 	mov	r0, r6
    9f3c:	ebfffef0 	bl	9b04 <mmc_change_freq>
    9f40:	e3500000 	cmp	r0, #0	; 0x0
    9f44:	e1a05000 	mov	r5, r0
    9f48:	11a04000 	movne	r4, r0
    9f4c:	1a000087 	bne	a170 <mmc_startup+0x5b8>
    9f50:	e596304c 	ldr	r3, [r6, #76]
    9f54:	e5962050 	ldr	r2, [r6, #80]
    9f58:	e5961030 	ldr	r1, [r6, #48]
    9f5c:	e0022003 	and	r2, r2, r3
    9f60:	e3110802 	tst	r1, #131072	; 0x20000
    9f64:	e586204c 	str	r2, [r6, #76]
    9f68:	e2023c01 	and	r3, r2, #256	; 0x100
    9f6c:	0a00002c 	beq	a024 <mmc_startup+0x46c>
    9f70:	e3530000 	cmp	r3, #0	; 0x0
    9f74:	0a000023 	beq	a008 <mmc_startup+0x450>
    9f78:	e1d638b0 	ldrh	r3, [r6, #128]
    9f7c:	e28d2c02 	add	r2, sp, #512	; 0x200
    9f80:	e1a03803 	lsl	r3, r3, #16
    9f84:	e58d3218 	str	r3, [sp, #536]
    9f88:	e3a08015 	mov	r8, #21	; 0x15
    9f8c:	e3a03037 	mov	r3, #55	; 0x37
    9f90:	e28d9e21 	add	r9, sp, #528	; 0x210
    9f94:	e1c231b0 	strh	r3, [r2, #16]
    9f98:	e58d022c 	str	r0, [sp, #556]
    9f9c:	e58d8214 	str	r8, [sp, #532]
    9fa0:	e1a00006 	mov	r0, r6
    9fa4:	e1a01009 	mov	r1, r9
    9fa8:	e1a02005 	mov	r2, r5
    9fac:	e1a0e00f 	mov	lr, pc
    9fb0:	e596f104 	ldr	pc, [r6, #260]
    9fb4:	e2504000 	subs	r4, r0, #0	; 0x0
    9fb8:	1a00006c 	bne	a170 <mmc_startup+0x5b8>
    9fbc:	e3a03002 	mov	r3, #2	; 0x2
    9fc0:	e3a01006 	mov	r1, #6	; 0x6
    9fc4:	e28d0c02 	add	r0, sp, #512	; 0x200
    9fc8:	e1c011b0 	strh	r1, [r0, #16]
    9fcc:	e58d8214 	str	r8, [sp, #532]
    9fd0:	e58d3218 	str	r3, [sp, #536]
    9fd4:	e58d522c 	str	r5, [sp, #556]
    9fd8:	e1a01009 	mov	r1, r9
    9fdc:	e1a02005 	mov	r2, r5
    9fe0:	e1a00006 	mov	r0, r6
    9fe4:	e1a0e00f 	mov	lr, pc
    9fe8:	e596f104 	ldr	pc, [r6, #260]
    9fec:	e2504000 	subs	r4, r0, #0	; 0x0
    9ff0:	1a00005e 	bne	a170 <mmc_startup+0x5b8>
    9ff4:	e3a03004 	mov	r3, #4	; 0x4
    9ff8:	e5863044 	str	r3, [r6, #68]
    9ffc:	e1a00006 	mov	r0, r6
    a000:	e1a0e00f 	mov	lr, pc
    a004:	e596f108 	ldr	pc, [r6, #264]
    a008:	e596304c 	ldr	r3, [r6, #76]
    a00c:	e3130001 	tst	r3, #1	; 0x1
    a010:	11a00006 	movne	r0, r6
    a014:	159f1180 	ldrne	r1, [pc, #384]	; a19c <mmc_startup+0x5e4>
    a018:	01a00006 	moveq	r0, r6
    a01c:	059f117c 	ldreq	r1, [pc, #380]	; a1a0 <mmc_startup+0x5e8>
    a020:	ea000022 	b	a0b0 <mmc_startup+0x4f8>
    a024:	e3530000 	cmp	r3, #0	; 0x0
    a028:	0a000008 	beq	a050 <mmc_startup+0x498>
    a02c:	e3a01001 	mov	r1, #1	; 0x1
    a030:	e1a03001 	mov	r3, r1
    a034:	e1a00006 	mov	r0, r6
    a038:	e3a020b7 	mov	r2, #183	; 0xb7
    a03c:	ebfffe98 	bl	9aa4 <mmc_switch>
    a040:	e2504000 	subs	r4, r0, #0	; 0x0
    a044:	03a03004 	moveq	r3, #4	; 0x4
    a048:	1a000048 	bne	a170 <mmc_startup+0x5b8>
    a04c:	ea000009 	b	a078 <mmc_startup+0x4c0>
    a050:	e3120c02 	tst	r2, #512	; 0x200
    a054:	0a00000b 	beq	a088 <mmc_startup+0x4d0>
    a058:	e1a00006 	mov	r0, r6
    a05c:	e3a01001 	mov	r1, #1	; 0x1
    a060:	e3a020b7 	mov	r2, #183	; 0xb7
    a064:	e3a03002 	mov	r3, #2	; 0x2
    a068:	ebfffe8d 	bl	9aa4 <mmc_switch>
    a06c:	e2504000 	subs	r4, r0, #0	; 0x0
    a070:	1a00003e 	bne	a170 <mmc_startup+0x5b8>
    a074:	e3a03008 	mov	r3, #8	; 0x8
    a078:	e5863044 	str	r3, [r6, #68]
    a07c:	e1a00006 	mov	r0, r6
    a080:	e1a0e00f 	mov	lr, pc
    a084:	e596f108 	ldr	pc, [r6, #264]
    a088:	e596304c 	ldr	r3, [r6, #76]
    a08c:	e3130001 	tst	r3, #1	; 0x1
    a090:	01a00006 	moveq	r0, r6
    a094:	059f1108 	ldreq	r1, [pc, #264]	; a1a4 <mmc_startup+0x5ec>
    a098:	0a000004 	beq	a0b0 <mmc_startup+0x4f8>
    a09c:	e3130010 	tst	r3, #16	; 0x10
    a0a0:	159f1100 	ldrne	r1, [pc, #256]	; a1a8 <mmc_startup+0x5f0>
    a0a4:	059f1100 	ldreq	r1, [pc, #256]	; a1ac <mmc_startup+0x5f4>
    a0a8:	11a00006 	movne	r0, r6
    a0ac:	01a00006 	moveq	r0, r6
    a0b0:	ebfffbed 	bl	906c <mmc_set_clock>
    a0b4:	e5964094 	ldr	r4, [r6, #148]
    a0b8:	e5960090 	ldr	r0, [r6, #144]
    a0bc:	e5961088 	ldr	r1, [r6, #136]
    a0c0:	e3a03000 	mov	r3, #0	; 0x0
    a0c4:	e3540000 	cmp	r4, #0	; 0x0
    a0c8:	e5c630a3 	strb	r3, [r6, #163]
    a0cc:	e5c630a2 	strb	r3, [r6, #162]
    a0d0:	e58610ac 	str	r1, [r6, #172]
    a0d4:	e58d0230 	str	r0, [sp, #560]
    a0d8:	e58d4234 	str	r4, [sp, #564]
    a0dc:	1a000003 	bne	a0f0 <mmc_startup+0x538>
    a0e0:	eb002b27 	bl	14d84 <__aeabi_uidiv>
    a0e4:	e58d4234 	str	r4, [sp, #564]
    a0e8:	e58d0230 	str	r0, [sp, #560]
    a0ec:	ea000001 	b	a0f8 <mmc_startup+0x540>
    a0f0:	e28d0e23 	add	r0, sp, #560	; 0x230
    a0f4:	eb001ee1 	bl	11c80 <__div64_32>
    a0f8:	e5d6007f 	ldrb	r0, [r6, #127]
    a0fc:	e59d1230 	ldr	r1, [sp, #560]
    a100:	e5962070 	ldr	r2, [r6, #112]
    a104:	e5963078 	ldr	r3, [r6, #120]
    a108:	e58610a8 	str	r1, [r6, #168]
    a10c:	e1803403 	orr	r3, r0, r3, lsl #8
    a110:	e1a02422 	lsr	r2, r2, #8
    a114:	e59f1094 	ldr	r1, [pc, #148]	; a1b0 <mmc_startup+0x5f8>
    a118:	e28600b0 	add	r0, r6, #176	; 0xb0
    a11c:	eb0022a0 	bl	12ba4 <sprintf>
    a120:	e596c074 	ldr	ip, [r6, #116]
    a124:	e5d62070 	ldrb	r2, [r6, #112]
    a128:	e1a0482c 	lsr	r4, ip, #16
    a12c:	e1a0542c 	lsr	r5, ip, #8
    a130:	e1a03c2c 	lsr	r3, ip, #24
    a134:	e20440ff 	and	r4, r4, #255	; 0xff
    a138:	e20cc0ff 	and	ip, ip, #255	; 0xff
    a13c:	e59f1070 	ldr	r1, [pc, #112]	; a1b4 <mmc_startup+0x5fc>
    a140:	e20550ff 	and	r5, r5, #255	; 0xff
    a144:	e28600d9 	add	r0, r6, #217	; 0xd9
    a148:	e88d1030 	stm	sp, {r4, r5, ip}
    a14c:	eb002294 	bl	12ba4 <sprintf>
    a150:	e5962078 	ldr	r2, [r6, #120]
    a154:	e28600ee 	add	r0, r6, #238	; 0xee
    a158:	e1a03c22 	lsr	r3, r2, #24
    a15c:	e203300f 	and	r3, r3, #15	; 0xf
    a160:	e1a02e22 	lsr	r2, r2, #28
    a164:	e59f104c 	ldr	r1, [pc, #76]	; a1b8 <mmc_startup+0x600>
    a168:	eb00228d 	bl	12ba4 <sprintf>
    a16c:	e3a04000 	mov	r4, #0	; 0x0
    a170:	e1a00004 	mov	r0, r4
    a174:	e28ddf8e 	add	sp, sp, #568	; 0x238
    a178:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    a17c:	00010014 	.word	0x00010014
    a180:	00010022 	.word	0x00010022
    a184:	00010030 	.word	0x00010030
    a188:	00010040 	.word	0x00010040
    a18c:	00010012 	.word	0x00010012
    a190:	000162e4 	.word	0x000162e4
    a194:	000162d4 	.word	0x000162d4
    a198:	0001003f 	.word	0x0001003f
    a19c:	02faf080 	.word	0x02faf080
    a1a0:	017d7840 	.word	0x017d7840
    a1a4:	01312d00 	.word	0x01312d00
    a1a8:	03197500 	.word	0x03197500
    a1ac:	018cba80 	.word	0x018cba80
    a1b0:	00018a14 	.word	0x00018a14
    a1b4:	00018a26 	.word	0x00018a26
    a1b8:	00018a31 	.word	0x00018a31

0000a1bc <mmc_init>:
    a1bc:	e92d4010 	push	{r4, lr}
    a1c0:	ebfffb5a 	bl	8f30 <find_mmc_device>
    a1c4:	e1a04000 	mov	r4, r0
    a1c8:	e1a0e00f 	mov	lr, pc
    a1cc:	e590f10c 	ldr	pc, [r0, #268]
    a1d0:	e3500000 	cmp	r0, #0	; 0x0
    a1d4:	18bd8010 	popne	{r4, pc}
    a1d8:	e3a03001 	mov	r3, #1	; 0x1
    a1dc:	e5843044 	str	r3, [r4, #68]
    a1e0:	e1a00004 	mov	r0, r4
    a1e4:	e1a0e00f 	mov	lr, pc
    a1e8:	e594f108 	ldr	pc, [r4, #264]
    a1ec:	e1a00004 	mov	r0, r4
    a1f0:	e59f1060 	ldr	r1, [pc, #96]	; a258 <mmc_init+0x9c>
    a1f4:	ebfffb9c 	bl	906c <mmc_set_clock>
    a1f8:	e1a00004 	mov	r0, r4
    a1fc:	ebfffd09 	bl	9628 <mmc_go_idle>
    a200:	e3500000 	cmp	r0, #0	; 0x0
    a204:	18bd8010 	popne	{r4, pc}
    a208:	e5c40083 	strb	r0, [r4, #131]
    a20c:	e1a00004 	mov	r0, r4
    a210:	ebfffba9 	bl	90bc <mmc_send_if_cond>
    a214:	e1a00004 	mov	r0, r4
    a218:	ebfffcad 	bl	94d4 <sd_send_op_cond>
    a21c:	e3700013 	cmn	r0, #19	; 0x13
    a220:	1a000009 	bne	a24c <mmc_init+0x90>
    a224:	e1a00004 	mov	r0, r4
    a228:	ebfffd13 	bl	967c <mmc_send_op_cond>
    a22c:	e3500000 	cmp	r0, #0	; 0x0
    a230:	0a000005 	beq	a24c <mmc_init+0x90>
    a234:	e59f0020 	ldr	r0, [pc, #32]	; a25c <mmc_init+0xa0>
    a238:	e59f3020 	ldr	r3, [pc, #32]	; a260 <mmc_init+0xa4>
    a23c:	e1a0e00f 	mov	lr, pc
    a240:	e593f000 	ldr	pc, [r3]
    a244:	e3e00010 	mvn	r0, #16	; 0x10
    a248:	e8bd8010 	pop	{r4, pc}
    a24c:	e1a00004 	mov	r0, r4
    a250:	e8bd4010 	pop	{r4, lr}
    a254:	eafffe57 	b	9bb8 <mmc_startup>
    a258:	00061a80 	.word	0x00061a80
    a25c:	00018a37 	.word	0x00018a37
    a260:	3c00a5cc 	.word	0x3c00a5cc

0000a264 <mmc2_vs_reset>:
    a264:	e3a00002 	mov	r0, #2	; 0x2
    a268:	eaffffd3 	b	a1bc <mmc_init>

0000a26c <mmc1_vs_reset>:
    a26c:	e3a00001 	mov	r0, #1	; 0x1
    a270:	eaffffd1 	b	a1bc <mmc_init>

0000a274 <mmc0_vs_reset>:
    a274:	e3a00000 	mov	r0, #0	; 0x0
    a278:	eaffffcf 	b	a1bc <mmc_init>

0000a27c <mmc_read_blocks>:
    a27c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    a280:	e3530001 	cmp	r3, #1	; 0x1
    a284:	e24dd030 	sub	sp, sp, #48	; 0x30
    a288:	e1a06003 	mov	r6, r3
    a28c:	83a03012 	movhi	r3, #18	; 0x12
    a290:	93a03011 	movls	r3, #17	; 0x11
    a294:	e1cd30b0 	strh	r3, [sp]
    a298:	e5903040 	ldr	r3, [r0, #64]
    a29c:	e3a07000 	mov	r7, #0	; 0x0
    a2a0:	e3530000 	cmp	r3, #0	; 0x0
    a2a4:	05903088 	ldreq	r3, [r0, #136]
    a2a8:	158d2008 	strne	r2, [sp, #8]
    a2ac:	00030293 	muleq	r3, r3, r2
    a2b0:	058d3008 	streq	r3, [sp, #8]
    a2b4:	e5903088 	ldr	r3, [r0, #136]
    a2b8:	e3a02015 	mov	r2, #21	; 0x15
    a2bc:	e58d302c 	str	r3, [sp, #44]
    a2c0:	e3a03001 	mov	r3, #1	; 0x1
    a2c4:	e58d2004 	str	r2, [sp, #4]
    a2c8:	e58d1020 	str	r1, [sp, #32]
    a2cc:	e1a04000 	mov	r4, r0
    a2d0:	e58d3024 	str	r3, [sp, #36]
    a2d4:	e58d701c 	str	r7, [sp, #28]
    a2d8:	e58d6028 	str	r6, [sp, #40]
    a2dc:	e1a0100d 	mov	r1, sp
    a2e0:	e28d2020 	add	r2, sp, #32	; 0x20
    a2e4:	e1a0e00f 	mov	lr, pc
    a2e8:	e594f104 	ldr	pc, [r4, #260]
    a2ec:	e2505000 	subs	r5, r0, #0	; 0x0
    a2f0:	e1a0800d 	mov	r8, sp
    a2f4:	11a00007 	movne	r0, r7
    a2f8:	1a000018 	bne	a360 <mmc_read_blocks+0xe4>
    a2fc:	e3560001 	cmp	r6, #1	; 0x1
    a300:	9a000012 	bls	a350 <mmc_read_blocks+0xd4>
    a304:	e3a0301d 	mov	r3, #29	; 0x1d
    a308:	e58d3004 	str	r3, [sp, #4]
    a30c:	e3a0300c 	mov	r3, #12	; 0xc
    a310:	e1cd30b0 	strh	r3, [sp]
    a314:	e58d5008 	str	r5, [sp, #8]
    a318:	e58d501c 	str	r5, [sp, #28]
    a31c:	e1a0100d 	mov	r1, sp
    a320:	e1a00004 	mov	r0, r4
    a324:	e1a02005 	mov	r2, r5
    a328:	e1a0e00f 	mov	lr, pc
    a32c:	e594f104 	ldr	pc, [r4, #260]
    a330:	e3500000 	cmp	r0, #0	; 0x0
    a334:	0a000005 	beq	a350 <mmc_read_blocks+0xd4>
    a338:	e59f0028 	ldr	r0, [pc, #40]	; a368 <mmc_read_blocks+0xec>
    a33c:	e59f3028 	ldr	r3, [pc, #40]	; a36c <mmc_read_blocks+0xf0>
    a340:	e1a0e00f 	mov	lr, pc
    a344:	e593f000 	ldr	pc, [r3]
    a348:	e1a00005 	mov	r0, r5
    a34c:	ea000003 	b	a360 <mmc_read_blocks+0xe4>
    a350:	e1a00004 	mov	r0, r4
    a354:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    a358:	ebfffd15 	bl	97b4 <mmc_send_status>
    a35c:	e1a00006 	mov	r0, r6
    a360:	e28dd030 	add	sp, sp, #48	; 0x30
    a364:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    a368:	000189f9 	.word	0x000189f9
    a36c:	3c00a5cc 	.word	0x3c00a5cc

0000a370 <mmc_bread>:
    a370:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    a374:	e252a000 	subs	sl, r2, #0	; 0x0
    a378:	e24dd004 	sub	sp, sp, #4	; 0x4
    a37c:	e1a07001 	mov	r7, r1
    a380:	e1a08003 	mov	r8, r3
    a384:	0a000027 	beq	a428 <mmc_bread+0xb8>
    a388:	ebfffae8 	bl	8f30 <find_mmc_device>
    a38c:	e2506000 	subs	r6, r0, #0	; 0x0
    a390:	0a000024 	beq	a428 <mmc_bread+0xb8>
    a394:	e59620a8 	ldr	r2, [r6, #168]
    a398:	e08a1007 	add	r1, sl, r7
    a39c:	e1510002 	cmp	r1, r2
    a3a0:	9a000004 	bls	a3b8 <mmc_bread+0x48>
    a3a4:	e59f0088 	ldr	r0, [pc, #136]	; a434 <mmc_bread+0xc4>
    a3a8:	e59f3088 	ldr	r3, [pc, #136]	; a438 <mmc_bread+0xc8>
    a3ac:	e1a0e00f 	mov	lr, pc
    a3b0:	e593f000 	ldr	pc, [r3]
    a3b4:	ea00001b 	b	a428 <mmc_bread+0xb8>
    a3b8:	e5961088 	ldr	r1, [r6, #136]
    a3bc:	ebfffacd 	bl	8ef8 <mmc_set_blocklen>
    a3c0:	e3500000 	cmp	r0, #0	; 0x0
    a3c4:	01a0500a 	moveq	r5, sl
    a3c8:	0a000005 	beq	a3e4 <mmc_bread+0x74>
    a3cc:	e5961088 	ldr	r1, [r6, #136]
    a3d0:	e59f0064 	ldr	r0, [pc, #100]	; a43c <mmc_bread+0xcc>
    a3d4:	e59f305c 	ldr	r3, [pc, #92]	; a438 <mmc_bread+0xc8>
    a3d8:	e1a0e00f 	mov	lr, pc
    a3dc:	e593f000 	ldr	pc, [r3]
    a3e0:	ea000010 	b	a428 <mmc_bread+0xb8>
    a3e4:	e5964110 	ldr	r4, [r6, #272]
    a3e8:	e1a02007 	mov	r2, r7
    a3ec:	e1550004 	cmp	r5, r4
    a3f0:	31a04005 	movcc	r4, r5
    a3f4:	e1a01008 	mov	r1, r8
    a3f8:	e1a00006 	mov	r0, r6
    a3fc:	e1a03004 	mov	r3, r4
    a400:	ebffff9d 	bl	a27c <mmc_read_blocks>
    a404:	e1500004 	cmp	r0, r4
    a408:	e0877000 	add	r7, r7, r0
    a40c:	1a000005 	bne	a428 <mmc_bread+0xb8>
    a410:	e0555000 	subs	r5, r5, r0
    a414:	e5962088 	ldr	r2, [r6, #136]
    a418:	10288092 	mlane	r8, r2, r0, r8
    a41c:	1afffff0 	bne	a3e4 <mmc_bread+0x74>
    a420:	e000029a 	mul	r0, sl, r2
    a424:	ea000000 	b	a42c <mmc_bread+0xbc>
    a428:	e3a00000 	mov	r0, #0	; 0x0
    a42c:	e28dd004 	add	sp, sp, #4	; 0x4
    a430:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    a434:	000189bb 	.word	0x000189bb
    a438:	3c00a5cc 	.word	0x3c00a5cc
    a43c:	00018a60 	.word	0x00018a60

0000a440 <mmc2_vs_read>:
    a440:	e1a024a2 	lsr	r2, r2, #9
    a444:	e1821b83 	orr	r1, r2, r3, lsl #23
    a448:	e59d2000 	ldr	r2, [sp]
    a44c:	e1a03000 	mov	r3, r0
    a450:	e1a024c2 	asr	r2, r2, #9
    a454:	e3a00002 	mov	r0, #2	; 0x2
    a458:	eaffffc4 	b	a370 <mmc_bread>

0000a45c <mmc1_vs_read>:
    a45c:	e1a024a2 	lsr	r2, r2, #9
    a460:	e1821b83 	orr	r1, r2, r3, lsl #23
    a464:	e59d2000 	ldr	r2, [sp]
    a468:	e1a03000 	mov	r3, r0
    a46c:	e1a024c2 	asr	r2, r2, #9
    a470:	e3a00001 	mov	r0, #1	; 0x1
    a474:	eaffffbd 	b	a370 <mmc_bread>

0000a478 <mmc0_vs_read>:
    a478:	e1a024a2 	lsr	r2, r2, #9
    a47c:	e1821b83 	orr	r1, r2, r3, lsl #23
    a480:	e59d2000 	ldr	r2, [sp]
    a484:	e1a03000 	mov	r3, r0
    a488:	e1a024c2 	asr	r2, r2, #9
    a48c:	e3a00000 	mov	r0, #0	; 0x0
    a490:	eaffffb6 	b	a370 <mmc_bread>

0000a494 <mci_pull_data32>:
    a494:	e1a02142 	asr	r2, r2, #2
    a498:	e3a0c000 	mov	ip, #0	; 0x0
    a49c:	ea000003 	b	a4b0 <mci_pull_data32+0x1c>
    a4a0:	e5903118 	ldr	r3, [r0, #280]
    a4a4:	e5933200 	ldr	r3, [r3, #512]
    a4a8:	e781300c 	str	r3, [r1, ip]
    a4ac:	e28cc004 	add	ip, ip, #4	; 0x4
    a4b0:	e3520000 	cmp	r2, #0	; 0x0
    a4b4:	e2422001 	sub	r2, r2, #1	; 0x1
    a4b8:	cafffff8 	bgt	a4a0 <mci_pull_data32+0xc>
    a4bc:	e12fff1e 	bx	lr

0000a4c0 <mci_push_data32>:
    a4c0:	e92d4010 	push	{r4, lr}
    a4c4:	e3a0c000 	mov	ip, #0	; 0x0
    a4c8:	e1a04000 	mov	r4, r0
    a4cc:	e1a00001 	mov	r0, r1
    a4d0:	e1a01142 	asr	r1, r2, #2
    a4d4:	ea000003 	b	a4e8 <mci_push_data32+0x28>
    a4d8:	e790200c 	ldr	r2, [r0, ip]
    a4dc:	e5943118 	ldr	r3, [r4, #280]
    a4e0:	e28cc004 	add	ip, ip, #4	; 0x4
    a4e4:	e5832200 	str	r2, [r3, #512]
    a4e8:	e3510000 	cmp	r1, #0	; 0x0
    a4ec:	e2411001 	sub	r1, r1, #1	; 0x1
    a4f0:	cafffff8 	bgt	a4d8 <mci_push_data32+0x18>
    a4f4:	e8bd8010 	pop	{r4, pc}

0000a4f8 <imap_sdhc_initialize>:
    a4f8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a4fc:	e3510000 	cmp	r1, #0	; 0x0
    a500:	e24dd004 	sub	sp, sp, #4	; 0x4
    a504:	1a000005 	bne	a520 <imap_sdhc_initialize+0x28>
    a508:	e59f00ec 	ldr	r0, [pc, #236]	; a5fc <imap_sdhc_initialize+0x104>
    a50c:	e59f30ec 	ldr	r3, [pc, #236]	; a600 <imap_sdhc_initialize+0x108>
    a510:	e1a0e00f 	mov	lr, pc
    a514:	e593f000 	ldr	pc, [r3]
    a518:	e3e00004 	mvn	r0, #4	; 0x4
    a51c:	ea000034 	b	a5f4 <imap_sdhc_initialize+0xfc>
    a520:	e3500000 	cmp	r0, #0	; 0x0
    a524:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    a528:	0a000005 	beq	a544 <imap_sdhc_initialize+0x4c>
    a52c:	e3500001 	cmp	r0, #1	; 0x1
    a530:	059f30cc 	ldreq	r3, [pc, #204]	; a604 <imap_sdhc_initialize+0x10c>
    a534:	0a000002 	beq	a544 <imap_sdhc_initialize+0x4c>
    a538:	e3500002 	cmp	r0, #2	; 0x2
    a53c:	1a000018 	bne	a5a4 <imap_sdhc_initialize+0xac>
    a540:	e59f30c0 	ldr	r3, [pc, #192]	; a608 <imap_sdhc_initialize+0x110>
    a544:	e5813118 	str	r3, [r1, #280]
    a548:	e59f30bc 	ldr	r3, [pc, #188]	; a60c <imap_sdhc_initialize+0x114>
    a54c:	e591c130 	ldr	ip, [r1, #304]
    a550:	e5813104 	str	r3, [r1, #260]
    a554:	e59f30b4 	ldr	r3, [pc, #180]	; a610 <imap_sdhc_initialize+0x118>
    a558:	e3a02000 	mov	r2, #0	; 0x0
    a55c:	e5813108 	str	r3, [r1, #264]
    a560:	e59f30ac 	ldr	r3, [pc, #172]	; a614 <imap_sdhc_initialize+0x11c>
    a564:	e5810114 	str	r0, [r1, #276]
    a568:	e581310c 	str	r3, [r1, #268]
    a56c:	e59f30a4 	ldr	r3, [pc, #164]	; a618 <imap_sdhc_initialize+0x120>
    a570:	e5812034 	str	r2, [r1, #52]
    a574:	e581302c 	str	r3, [r1, #44]
    a578:	e3003101 	movw	r3, #257	; 0x101
    a57c:	e5813050 	str	r3, [r1, #80]
    a580:	e59f3094 	ldr	r3, [pc, #148]	; a61c <imap_sdhc_initialize+0x124>
    a584:	e5812028 	str	r2, [r1, #40]
    a588:	e5813038 	str	r3, [r1, #56]
    a58c:	e59f308c 	ldr	r3, [pc, #140]	; a620 <imap_sdhc_initialize+0x128>
    a590:	e5812110 	str	r2, [r1, #272]
    a594:	e581303c 	str	r3, [r1, #60]
    a598:	e1a0000c 	mov	r0, ip
    a59c:	e2822010 	add	r2, r2, #16	; 0x10
    a5a0:	ea000005 	b	a5bc <imap_sdhc_initialize+0xc4>
    a5a4:	e59f0078 	ldr	r0, [pc, #120]	; a624 <imap_sdhc_initialize+0x12c>
    a5a8:	e59f3050 	ldr	r3, [pc, #80]	; a600 <imap_sdhc_initialize+0x108>
    a5ac:	e1a0e00f 	mov	lr, pc
    a5b0:	e593f000 	ldr	pc, [r3]
    a5b4:	e3e00000 	mvn	r0, #0	; 0x0
    a5b8:	ea00000d 	b	a5f4 <imap_sdhc_initialize+0xfc>
    a5bc:	e5913130 	ldr	r3, [r1, #304]
    a5c0:	e0823003 	add	r3, r2, r3
    a5c4:	e2822010 	add	r2, r2, #16	; 0x10
    a5c8:	e3520b01 	cmp	r2, #1024	; 0x400
    a5cc:	e580300c 	str	r3, [r0, #12]
    a5d0:	e2800010 	add	r0, r0, #16	; 0x10
    a5d4:	1afffff8 	bne	a5bc <imap_sdhc_initialize+0xc4>
    a5d8:	e5912130 	ldr	r2, [r1, #304]
    a5dc:	e3a03020 	mov	r3, #32	; 0x20
    a5e0:	e58c33f0 	str	r3, [ip, #1008]
    a5e4:	e58c23fc 	str	r2, [ip, #1020]
    a5e8:	e1a00001 	mov	r0, r1
    a5ec:	ebfffad0 	bl	9134 <mmc_register>
    a5f0:	e3a00000 	mov	r0, #0	; 0x0
    a5f4:	e28dd004 	add	sp, sp, #4	; 0x4
    a5f8:	e8bd8000 	pop	{pc}
    a5fc:	00018a6a 	.word	0x00018a6a
    a600:	3c00a5cc 	.word	0x3c00a5cc
    a604:	27200000 	.word	0x27200000
    a608:	27300000 	.word	0x27300000
    a60c:	0000a698 	.word	0x0000a698
    a610:	0000ac44 	.word	0x0000ac44
    a614:	0000a9b8 	.word	0x0000a9b8
    a618:	00ff8000 	.word	0x00ff8000
    a61c:	00061a80 	.word	0x00061a80
    a620:	018cba80 	.word	0x018cba80
    a624:	00018a80 	.word	0x00018a80

0000a628 <mci_send_cmd>:
    a628:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a62c:	e5903118 	ldr	r3, [r0, #280]
    a630:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    a634:	e5832028 	str	r2, [r3, #40]
    a638:	e5903118 	ldr	r3, [r0, #280]
    a63c:	e24dd004 	sub	sp, sp, #4	; 0x4
    a640:	e583102c 	str	r1, [r3, #44]
    a644:	e1a04000 	mov	r4, r0
    a648:	eb000fff 	bl	e64c <get_ticks>
    a64c:	e59f5040 	ldr	r5, [pc, #64]	; a694 <mci_send_cmd+0x6c>
    a650:	e1a06000 	mov	r6, r0
    a654:	e1a07001 	mov	r7, r1
    a658:	e5943118 	ldr	r3, [r4, #280]
    a65c:	e593302c 	ldr	r3, [r3, #44]
    a660:	e3530000 	cmp	r3, #0	; 0x0
    a664:	aa000008 	bge	a68c <mci_send_cmd+0x64>
    a668:	eb000ff7 	bl	e64c <get_ticks>
    a66c:	e1a02000 	mov	r2, r0
    a670:	e1a03001 	mov	r3, r1
    a674:	e0522006 	subs	r2, r2, r6
    a678:	e0c33007 	sbc	r3, r3, r7
    a67c:	e3530000 	cmp	r3, #0	; 0x0
    a680:	1a000001 	bne	a68c <mci_send_cmd+0x64>
    a684:	e1520005 	cmp	r2, r5
    a688:	9afffff2 	bls	a658 <mci_send_cmd+0x30>
    a68c:	e28dd004 	add	sp, sp, #4	; 0x4
    a690:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    a694:	0007a11f 	.word	0x0007a11f

0000a698 <imap_sdhc_send_cmd>:
    a698:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    a69c:	e252a000 	subs	sl, r2, #0	; 0x0
    a6a0:	e1a04000 	mov	r4, r0
    a6a4:	e1a08001 	mov	r8, r1
    a6a8:	0a000039 	beq	a794 <imap_sdhc_send_cmd+0xfc>
    a6ac:	e99a000c 	ldmib	sl, {r2, r3}
    a6b0:	e59a100c 	ldr	r1, [sl, #12]
    a6b4:	e3120001 	tst	r2, #1	; 0x1
    a6b8:	e0000193 	mul	r0, r3, r1
    a6bc:	15942118 	ldrne	r2, [r4, #280]
    a6c0:	11a03801 	lslne	r3, r1, #16
    a6c4:	13833001 	orrne	r3, r3, #1	; 0x1
    a6c8:	15823100 	strne	r3, [r2, #256]
    a6cc:	e59a200c 	ldr	r2, [sl, #12]
    a6d0:	e5943118 	ldr	r3, [r4, #280]
    a6d4:	e1a0e000 	mov	lr, r0
    a6d8:	e583201c 	str	r2, [r3, #28]
    a6dc:	e5943118 	ldr	r3, [r4, #280]
    a6e0:	e3a05000 	mov	r5, #0	; 0x0
    a6e4:	e5830020 	str	r0, [r3, #32]
    a6e8:	e5942118 	ldr	r2, [r4, #280]
    a6ec:	e3019ff0 	movw	r9, #8176	; 0x1ff0
    a6f0:	e5923000 	ldr	r3, [r2]
    a6f4:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    a6f8:	e3833020 	orr	r3, r3, #32	; 0x20
    a6fc:	e5823000 	str	r3, [r2]
    a700:	e59a6000 	ldr	r6, [sl]
    a704:	e594c130 	ldr	ip, [r4, #304]
    a708:	e15e0009 	cmp	lr, r9
    a70c:	31a0100e 	movcc	r1, lr
    a710:	21a01009 	movcs	r1, r9
    a714:	e59c0004 	ldr	r0, [ip, #4]
    a718:	e59f3288 	ldr	r3, [pc, #648]	; a9a8 <imap_sdhc_send_cmd+0x310>
    a71c:	e1a02981 	lsl	r2, r1, #19
    a720:	e0003003 	and	r3, r0, r3
    a724:	e1a029a2 	lsr	r2, r2, #19
    a728:	e1822003 	orr	r2, r2, r3
    a72c:	e05ee001 	subs	lr, lr, r1
    a730:	e0853006 	add	r3, r5, r6
    a734:	e98c000c 	stmib	ip, {r2, r3}
    a738:	e58c7000 	str	r7, [ip]
    a73c:	e1a0200c 	mov	r2, ip
    a740:	e0855001 	add	r5, r5, r1
    a744:	e28cc010 	add	ip, ip, #16	; 0x10
    a748:	1affffee 	bne	a708 <imap_sdhc_send_cmd+0x70>
    a74c:	e3c73002 	bic	r3, r7, #2	; 0x2
    a750:	e3833004 	orr	r3, r3, #4	; 0x4
    a754:	e5823000 	str	r3, [r2]
    a758:	e5942130 	ldr	r2, [r4, #304]
    a75c:	e5923000 	ldr	r3, [r2]
    a760:	e3833008 	orr	r3, r3, #8	; 0x8
    a764:	e5823000 	str	r3, [r2]
    a768:	e5942118 	ldr	r2, [r4, #280]
    a76c:	e5923000 	ldr	r3, [r2]
    a770:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    a774:	e5823000 	str	r3, [r2]
    a778:	e5942118 	ldr	r2, [r4, #280]
    a77c:	e5923080 	ldr	r3, [r2, #128]
    a780:	e3833080 	orr	r3, r3, #128	; 0x80
    a784:	e5823080 	str	r3, [r2, #128]
    a788:	e5942118 	ldr	r2, [r4, #280]
    a78c:	e3a03001 	mov	r3, #1	; 0x1
    a790:	e5823084 	str	r3, [r2, #132]
    a794:	e1d800b0 	ldrh	r0, [r8]
    a798:	e5982004 	ldr	r2, [r8, #4]
    a79c:	e350000c 	cmp	r0, #12	; 0xc
    a7a0:	03a03901 	moveq	r3, #16384	; 0x4000
    a7a4:	13a03a02 	movne	r3, #8192	; 0x2000
    a7a8:	e3120004 	tst	r2, #4	; 0x4
    a7ac:	13833c01 	orrne	r3, r3, #256	; 0x100
    a7b0:	e3120001 	tst	r2, #1	; 0x1
    a7b4:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    a7b8:	0a000002 	beq	a7c8 <imap_sdhc_send_cmd+0x130>
    a7bc:	e3120002 	tst	r2, #2	; 0x2
    a7c0:	e3811040 	orr	r1, r1, #64	; 0x40
    a7c4:	13811080 	orrne	r1, r1, #128	; 0x80
    a7c8:	e35a0000 	cmp	sl, #0	; 0x0
    a7cc:	0a000003 	beq	a7e0 <imap_sdhc_send_cmd+0x148>
    a7d0:	e59a3004 	ldr	r3, [sl, #4]
    a7d4:	e3811c02 	orr	r1, r1, #512	; 0x200
    a7d8:	e3130002 	tst	r3, #2	; 0x2
    a7dc:	13811b01 	orrne	r1, r1, #1024	; 0x400
    a7e0:	e594312c 	ldr	r3, [r4, #300]
    a7e4:	e1811000 	orr	r1, r1, r0
    a7e8:	e3530000 	cmp	r3, #0	; 0x0
    a7ec:	13a03000 	movne	r3, #0	; 0x0
    a7f0:	1584312c 	strne	r3, [r4, #300]
    a7f4:	e5982008 	ldr	r2, [r8, #8]
    a7f8:	e5943118 	ldr	r3, [r4, #280]
    a7fc:	13811902 	orrne	r1, r1, #32768	; 0x8000
    a800:	e5832028 	str	r2, [r3, #40]
    a804:	e5943118 	ldr	r3, [r4, #280]
    a808:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    a80c:	e583102c 	str	r1, [r3, #44]
    a810:	eb000f8d 	bl	e64c <get_ticks>
    a814:	e3005bb8 	movw	r5, #3000	; 0xbb8
    a818:	e1a06000 	mov	r6, r0
    a81c:	e1a07001 	mov	r7, r1
    a820:	eb000f89 	bl	e64c <get_ticks>
    a824:	e1a02000 	mov	r2, r0
    a828:	e1a03001 	mov	r3, r1
    a82c:	e0522006 	subs	r2, r2, r6
    a830:	e0c33007 	sbc	r3, r3, r7
    a834:	e3530000 	cmp	r3, #0	; 0x0
    a838:	1a000007 	bne	a85c <imap_sdhc_send_cmd+0x1c4>
    a83c:	e1520005 	cmp	r2, r5
    a840:	8a000005 	bhi	a85c <imap_sdhc_send_cmd+0x1c4>
    a844:	e5943118 	ldr	r3, [r4, #280]
    a848:	e5932044 	ldr	r2, [r3, #68]
    a84c:	e2831044 	add	r1, r3, #68	; 0x44
    a850:	e3120004 	tst	r2, #4	; 0x4
    a854:	0afffff1 	beq	a820 <imap_sdhc_send_cmd+0x188>
    a858:	ea000005 	b	a874 <imap_sdhc_send_cmd+0x1dc>
    a85c:	e59f0148 	ldr	r0, [pc, #328]	; a9ac <imap_sdhc_send_cmd+0x314>
    a860:	e59f3148 	ldr	r3, [pc, #328]	; a9b0 <imap_sdhc_send_cmd+0x318>
    a864:	e1a0e00f 	mov	lr, pc
    a868:	e593f000 	ldr	pc, [r3]
    a86c:	e3e00011 	mvn	r0, #17	; 0x11
    a870:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    a874:	e5915000 	ldr	r5, [r1]
    a878:	e3003142 	movw	r3, #322	; 0x142
    a87c:	e0053003 	and	r3, r5, r3
    a880:	e3530000 	cmp	r3, #0	; 0x0
    a884:	e3a03004 	mov	r3, #4	; 0x4
    a888:	e5813000 	str	r3, [r1]
    a88c:	0a00000e 	beq	a8cc <imap_sdhc_send_cmd+0x234>
    a890:	e1d810b0 	ldrh	r1, [r8]
    a894:	e5943118 	ldr	r3, [r4, #280]
    a898:	e3510037 	cmp	r1, #55	; 0x37
    a89c:	13510008 	cmpne	r1, #8	; 0x8
    a8a0:	e5835044 	str	r5, [r3, #68]
    a8a4:	0a000004 	beq	a8bc <imap_sdhc_send_cmd+0x224>
    a8a8:	e59f0104 	ldr	r0, [pc, #260]	; a9b4 <imap_sdhc_send_cmd+0x31c>
    a8ac:	e1a02005 	mov	r2, r5
    a8b0:	e59f30f8 	ldr	r3, [pc, #248]	; a9b0 <imap_sdhc_send_cmd+0x318>
    a8b4:	e1a0e00f 	mov	lr, pc
    a8b8:	e593f000 	ldr	pc, [r3]
    a8bc:	e3150c01 	tst	r5, #256	; 0x100
    a8c0:	13e00012 	mvnne	r0, #18	; 0x12
    a8c4:	03e00011 	mvneq	r0, #17	; 0x11
    a8c8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    a8cc:	e5983004 	ldr	r3, [r8, #4]
    a8d0:	e5942118 	ldr	r2, [r4, #280]
    a8d4:	e3130002 	tst	r3, #2	; 0x2
    a8d8:	05923030 	ldreq	r3, [r2, #48]
    a8dc:	0588300c 	streq	r3, [r8, #12]
    a8e0:	0a00000a 	beq	a910 <imap_sdhc_send_cmd+0x278>
    a8e4:	e592303c 	ldr	r3, [r2, #60]
    a8e8:	e588300c 	str	r3, [r8, #12]
    a8ec:	e5943118 	ldr	r3, [r4, #280]
    a8f0:	e5933038 	ldr	r3, [r3, #56]
    a8f4:	e5883010 	str	r3, [r8, #16]
    a8f8:	e5943118 	ldr	r3, [r4, #280]
    a8fc:	e5933034 	ldr	r3, [r3, #52]
    a900:	e5883014 	str	r3, [r8, #20]
    a904:	e5943118 	ldr	r3, [r4, #280]
    a908:	e5933030 	ldr	r3, [r3, #48]
    a90c:	e5883018 	str	r3, [r8, #24]
    a910:	e35a0000 	cmp	sl, #0	; 0x0
    a914:	01a0000a 	moveq	r0, sl
    a918:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
    a91c:	e594c118 	ldr	ip, [r4, #280]
    a920:	e59a3004 	ldr	r3, [sl, #4]
    a924:	e28c7044 	add	r7, ip, #68	; 0x44
    a928:	e2030001 	and	r0, r3, #1	; 0x1
    a92c:	e28c608c 	add	r6, ip, #140	; 0x8c
    a930:	e5975000 	ldr	r5, [r7]
    a934:	e3500000 	cmp	r0, #0	; 0x0
    a938:	e596e000 	ldr	lr, [r6]
    a93c:	e2051008 	and	r1, r5, #8	; 0x8
    a940:	0a000005 	beq	a95c <imap_sdhc_send_cmd+0x2c4>
    a944:	e3510000 	cmp	r1, #0	; 0x0
    a948:	0afffff8 	beq	a930 <imap_sdhc_send_cmd+0x298>
    a94c:	e31e0002 	tst	lr, #2	; 0x2
    a950:	0afffff6 	beq	a930 <imap_sdhc_send_cmd+0x298>
    a954:	e3a03002 	mov	r3, #2	; 0x2
    a958:	ea000004 	b	a970 <imap_sdhc_send_cmd+0x2d8>
    a95c:	e3510000 	cmp	r1, #0	; 0x0
    a960:	0afffff2 	beq	a930 <imap_sdhc_send_cmd+0x298>
    a964:	e31e0001 	tst	lr, #1	; 0x1
    a968:	0afffff0 	beq	a930 <imap_sdhc_send_cmd+0x298>
    a96c:	e3a03001 	mov	r3, #1	; 0x1
    a970:	e58c308c 	str	r3, [ip, #140]
    a974:	e5943118 	ldr	r3, [r4, #280]
    a978:	e3a00000 	mov	r0, #0	; 0x0
    a97c:	e5835044 	str	r5, [r3, #68]
    a980:	e5942118 	ldr	r2, [r4, #280]
    a984:	e5923000 	ldr	r3, [r2]
    a988:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    a98c:	e3833004 	orr	r3, r3, #4	; 0x4
    a990:	e5823000 	str	r3, [r2]
    a994:	e5942118 	ldr	r2, [r4, #280]
    a998:	e5923080 	ldr	r3, [r2, #128]
    a99c:	e3c33080 	bic	r3, r3, #128	; 0x80
    a9a0:	e5823080 	str	r3, [r2, #128]
    a9a4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    a9a8:	03ffe000 	.word	0x03ffe000
    a9ac:	00018a92 	.word	0x00018a92
    a9b0:	3c00a5cc 	.word	0x3c00a5cc
    a9b4:	00018aa0 	.word	0x00018aa0

0000a9b8 <imap_sdhc_init>:
    a9b8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a9bc:	e5905114 	ldr	r5, [r0, #276]
    a9c0:	e3a07000 	mov	r7, #0	; 0x0
    a9c4:	e3a06001 	mov	r6, #1	; 0x1
    a9c8:	e3550001 	cmp	r5, #1	; 0x1
    a9cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    a9d0:	e1a04000 	mov	r4, r0
    a9d4:	e580711c 	str	r7, [r0, #284]
    a9d8:	e5807048 	str	r7, [r0, #72]
    a9dc:	e580612c 	str	r6, [r0, #300]
    a9e0:	0a00001d 	beq	aa5c <imap_sdhc_init+0xa4>
    a9e4:	3a000002 	bcc	a9f4 <imap_sdhc_init+0x3c>
    a9e8:	e3550002 	cmp	r5, #2	; 0x2
    a9ec:	1a00003e 	bne	aaec <imap_sdhc_init+0x134>
    a9f0:	ea00002b 	b	aaa4 <imap_sdhc_init+0xec>
    a9f4:	e3a00807 	mov	r0, #458752	; 0x70000
    a9f8:	e1a01007 	mov	r1, r7
    a9fc:	e1a02007 	mov	r2, r7
    aa00:	ebffd881 	bl	c0c <pads_chmod>
    aa04:	e59f0208 	ldr	r0, [pc, #520]	; ac14 <imap_sdhc_init+0x25c>
    aa08:	e1a01007 	mov	r1, r7
    aa0c:	e1a02007 	mov	r2, r7
    aa10:	ebffd87d 	bl	c0c <pads_chmod>
    aa14:	e3a00401 	mov	r0, #16777216	; 0x1000000
    aa18:	ebffd98e 	bl	1058 <ecfg_check_flag>
    aa1c:	e3500000 	cmp	r0, #0	; 0x0
    aa20:	0a00000b 	beq	aa54 <imap_sdhc_init+0x9c>
    aa24:	e3a00807 	mov	r0, #458752	; 0x70000
    aa28:	e1a01006 	mov	r1, r6
    aa2c:	e1a02006 	mov	r2, r6
    aa30:	ebffd8d4 	bl	d88 <pads_pull>
    aa34:	e59f01d8 	ldr	r0, [pc, #472]	; ac14 <imap_sdhc_init+0x25c>
    aa38:	e1a01006 	mov	r1, r6
    aa3c:	e1a02006 	mov	r2, r6
    aa40:	ebffd8d0 	bl	d88 <pads_pull>
    aa44:	e1a01006 	mov	r1, r6
    aa48:	e1a02007 	mov	r2, r7
    aa4c:	e3a00010 	mov	r0, #16	; 0x10
    aa50:	ebffd8cc 	bl	d88 <pads_pull>
    aa54:	e59f01bc 	ldr	r0, [pc, #444]	; ac18 <imap_sdhc_init+0x260>
    aa58:	ea000022 	b	aae8 <imap_sdhc_init+0x130>
    aa5c:	e59f01b8 	ldr	r0, [pc, #440]	; ac1c <imap_sdhc_init+0x264>
    aa60:	e1a01007 	mov	r1, r7
    aa64:	e1a02007 	mov	r2, r7
    aa68:	ebffd867 	bl	c0c <pads_chmod>
    aa6c:	e3a00401 	mov	r0, #16777216	; 0x1000000
    aa70:	ebffd978 	bl	1058 <ecfg_check_flag>
    aa74:	e3500000 	cmp	r0, #0	; 0x0
    aa78:	0a000007 	beq	aa9c <imap_sdhc_init+0xe4>
    aa7c:	e59f0198 	ldr	r0, [pc, #408]	; ac1c <imap_sdhc_init+0x264>
    aa80:	e1a01005 	mov	r1, r5
    aa84:	e1a02005 	mov	r2, r5
    aa88:	ebffd8be 	bl	d88 <pads_pull>
    aa8c:	e1a01005 	mov	r1, r5
    aa90:	e1a02007 	mov	r2, r7
    aa94:	e3a00021 	mov	r0, #33	; 0x21
    aa98:	ebffd8ba 	bl	d88 <pads_pull>
    aa9c:	e59f017c 	ldr	r0, [pc, #380]	; ac20 <imap_sdhc_init+0x268>
    aaa0:	ea000010 	b	aae8 <imap_sdhc_init+0x130>
    aaa4:	e59f0178 	ldr	r0, [pc, #376]	; ac24 <imap_sdhc_init+0x26c>
    aaa8:	e1a01007 	mov	r1, r7
    aaac:	e1a02007 	mov	r2, r7
    aab0:	ebffd855 	bl	c0c <pads_chmod>
    aab4:	e3a00401 	mov	r0, #16777216	; 0x1000000
    aab8:	ebffd966 	bl	1058 <ecfg_check_flag>
    aabc:	e3500000 	cmp	r0, #0	; 0x0
    aac0:	0a000007 	beq	aae4 <imap_sdhc_init+0x12c>
    aac4:	e59f0158 	ldr	r0, [pc, #344]	; ac24 <imap_sdhc_init+0x26c>
    aac8:	e1a01006 	mov	r1, r6
    aacc:	e1a02006 	mov	r2, r6
    aad0:	ebffd8ac 	bl	d88 <pads_pull>
    aad4:	e1a01006 	mov	r1, r6
    aad8:	e1a02007 	mov	r2, r7
    aadc:	e3a00028 	mov	r0, #40	; 0x28
    aae0:	ebffd8a8 	bl	d88 <pads_pull>
    aae4:	e59f013c 	ldr	r0, [pc, #316]	; ac28 <imap_sdhc_init+0x270>
    aae8:	ebffd7ea 	bl	a98 <module_enable>
    aaec:	e5942118 	ldr	r2, [r4, #280]
    aaf0:	e59f5134 	ldr	r5, [pc, #308]	; ac2c <imap_sdhc_init+0x274>
    aaf4:	e5923070 	ldr	r3, [r2, #112]
    aaf8:	e1a03943 	asr	r3, r3, #18
    aafc:	e2033007 	and	r3, r3, #7	; 0x7
    ab00:	e3530002 	cmp	r3, #2	; 0x2
    ab04:	03a03003 	moveq	r3, #3	; 0x3
    ab08:	13a03002 	movne	r3, #2	; 0x2
    ab0c:	e5843120 	str	r3, [r4, #288]
    ab10:	e3a03007 	mov	r3, #7	; 0x7
    ab14:	e5823000 	str	r3, [r2]
    ab18:	e59f3110 	ldr	r3, [pc, #272]	; ac30 <imap_sdhc_init+0x278>
    ab1c:	e5843124 	str	r3, [r4, #292]
    ab20:	e59f310c 	ldr	r3, [pc, #268]	; ac34 <imap_sdhc_init+0x27c>
    ab24:	e5843128 	str	r3, [r4, #296]
    ab28:	eb000ec7 	bl	e64c <get_ticks>
    ab2c:	e1a06000 	mov	r6, r0
    ab30:	e1a07001 	mov	r7, r1
    ab34:	e5943118 	ldr	r3, [r4, #280]
    ab38:	e3a00001 	mov	r0, #1	; 0x1
    ab3c:	e5933000 	ldr	r3, [r3]
    ab40:	e3130007 	tst	r3, #7	; 0x7
    ab44:	0a00000d 	beq	ab80 <imap_sdhc_init+0x1c8>
    ab48:	eb000ed0 	bl	e690 <udelay>
    ab4c:	eb000ebe 	bl	e64c <get_ticks>
    ab50:	e1a02000 	mov	r2, r0
    ab54:	e1a03001 	mov	r3, r1
    ab58:	e0522006 	subs	r2, r2, r6
    ab5c:	e0c33007 	sbc	r3, r3, r7
    ab60:	e3530000 	cmp	r3, #0	; 0x0
    ab64:	1a000001 	bne	ab70 <imap_sdhc_init+0x1b8>
    ab68:	e1520005 	cmp	r2, r5
    ab6c:	9afffff0 	bls	ab34 <imap_sdhc_init+0x17c>
    ab70:	e59f00c0 	ldr	r0, [pc, #192]	; ac38 <imap_sdhc_init+0x280>
    ab74:	e59f30c0 	ldr	r3, [pc, #192]	; ac3c <imap_sdhc_init+0x284>
    ab78:	e1a0e00f 	mov	lr, pc
    ab7c:	e593f000 	ldr	pc, [r3]
    ab80:	e5943118 	ldr	r3, [r4, #280]
    ab84:	e3a01000 	mov	r1, #0	; 0x0
    ab88:	e5831024 	str	r1, [r3, #36]
    ab8c:	e5943118 	ldr	r3, [r4, #280]
    ab90:	e3e02000 	mvn	r2, #0	; 0x0
    ab94:	e5832044 	str	r2, [r3, #68]
    ab98:	e5943118 	ldr	r3, [r4, #280]
    ab9c:	e1a00001 	mov	r0, r1
    aba0:	e5832014 	str	r2, [r3, #20]
    aba4:	e5942118 	ldr	r2, [r4, #280]
    aba8:	e592304c 	ldr	r3, [r2, #76]
    abac:	e1a03283 	lsl	r3, r3, #5
    abb0:	e1a03b23 	lsr	r3, r3, #22
    abb4:	e1833803 	orr	r3, r3, r3, lsl #16
    abb8:	e582304c 	str	r3, [r2, #76]
    abbc:	e5943118 	ldr	r3, [r4, #280]
    abc0:	e5831010 	str	r1, [r3, #16]
    abc4:	e5942118 	ldr	r2, [r4, #280]
    abc8:	e5923004 	ldr	r3, [r2, #4]
    abcc:	e3833001 	orr	r3, r3, #1	; 0x1
    abd0:	e5823004 	str	r3, [r2, #4]
    abd4:	e5942118 	ldr	r2, [r4, #280]
    abd8:	e59f3060 	ldr	r3, [pc, #96]	; ac40 <imap_sdhc_init+0x288>
    abdc:	e5823058 	str	r3, [r2, #88]
    abe0:	e5942118 	ldr	r2, [r4, #280]
    abe4:	e3003103 	movw	r3, #259	; 0x103
    abe8:	e5823090 	str	r3, [r2, #144]
    abec:	e5942130 	ldr	r2, [r4, #304]
    abf0:	e5943118 	ldr	r3, [r4, #280]
    abf4:	e5832088 	str	r2, [r3, #136]
    abf8:	e5942118 	ldr	r2, [r4, #280]
    abfc:	e5923080 	ldr	r3, [r2, #128]
    ac00:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    ac04:	e3833008 	orr	r3, r3, #8	; 0x8
    ac08:	e5823080 	str	r3, [r2, #128]
    ac0c:	e28dd004 	add	sp, sp, #4	; 0x4
    ac10:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    ac14:	00120010 	.word	0x00120010
    ac18:	21e39000 	.word	0x21e39000
    ac1c:	0023001d 	.word	0x0023001d
    ac20:	21e38800 	.word	0x21e38800
    ac24:	00290024 	.word	0x00290024
    ac28:	21e38c00 	.word	0x21e38c00
    ac2c:	0007a11f 	.word	0x0007a11f
    ac30:	0000a494 	.word	0x0000a494
    ac34:	0000a4c0 	.word	0x0000a4c0
    ac38:	00018ab8 	.word	0x00018ab8
    ac3c:	3c00a5cc 	.word	0x3c00a5cc
    ac40:	008c2800 	.word	0x008c2800

0000ac44 <imap_sdhc_set_ios>:
    ac44:	e5902048 	ldr	r2, [r0, #72]
    ac48:	e590311c 	ldr	r3, [r0, #284]
    ac4c:	e92d4070 	push	{r4, r5, r6, lr}
    ac50:	e1520003 	cmp	r2, r3
    ac54:	e1a05000 	mov	r5, r0
    ac58:	0a00002a 	beq	ad08 <imap_sdhc_set_ios+0xc4>
    ac5c:	e5903114 	ldr	r3, [r0, #276]
    ac60:	e59f00d0 	ldr	r0, [pc, #208]	; ad38 <imap_sdhc_set_ios+0xf4>
    ac64:	e0830000 	add	r0, r3, r0
    ac68:	e1a00200 	lsl	r0, r0, #4
    ac6c:	eb001af9 	bl	11858 <module_get_clock>
    ac70:	e5954048 	ldr	r4, [r5, #72]
    ac74:	e1a06000 	mov	r6, r0
    ac78:	e1a01004 	mov	r1, r4
    ac7c:	eb0028bc 	bl	14f74 <__aeabi_uidivmod>
    ac80:	e3510000 	cmp	r1, #0	; 0x0
    ac84:	0a000005 	beq	aca0 <imap_sdhc_set_ios+0x5c>
    ac88:	e1a00006 	mov	r0, r6
    ac8c:	e1a01004 	mov	r1, r4
    ac90:	eb00283b 	bl	14d84 <__aeabi_uidiv>
    ac94:	e1a000a0 	lsr	r0, r0, #1
    ac98:	e2806001 	add	r6, r0, #1	; 0x1
    ac9c:	ea000003 	b	acb0 <imap_sdhc_set_ios+0x6c>
    aca0:	e1a00006 	mov	r0, r6
    aca4:	e1a01004 	mov	r1, r4
    aca8:	eb002835 	bl	14d84 <__aeabi_uidiv>
    acac:	e1a060a0 	lsr	r6, r0, #1
    acb0:	e5953118 	ldr	r3, [r5, #280]
    acb4:	e3a04000 	mov	r4, #0	; 0x0
    acb8:	e5834010 	str	r4, [r3, #16]
    acbc:	e5953118 	ldr	r3, [r5, #280]
    acc0:	e1a00005 	mov	r0, r5
    acc4:	e583400c 	str	r4, [r3, #12]
    acc8:	e59f106c 	ldr	r1, [pc, #108]	; ad3c <imap_sdhc_set_ios+0xf8>
    accc:	e1a02004 	mov	r2, r4
    acd0:	ebfffe54 	bl	a628 <mci_send_cmd>
    acd4:	e5953118 	ldr	r3, [r5, #280]
    acd8:	e1a00005 	mov	r0, r5
    acdc:	e5836008 	str	r6, [r3, #8]
    ace0:	e59f1054 	ldr	r1, [pc, #84]	; ad3c <imap_sdhc_set_ios+0xf8>
    ace4:	e1a02004 	mov	r2, r4
    ace8:	ebfffe4e 	bl	a628 <mci_send_cmd>
    acec:	e5952118 	ldr	r2, [r5, #280]
    acf0:	e3a03001 	mov	r3, #1	; 0x1
    acf4:	e5823010 	str	r3, [r2, #16]
    acf8:	e1a00005 	mov	r0, r5
    acfc:	e1a02004 	mov	r2, r4
    ad00:	e59f1034 	ldr	r1, [pc, #52]	; ad3c <imap_sdhc_set_ios+0xf8>
    ad04:	ebfffe47 	bl	a628 <mci_send_cmd>
    ad08:	e5953044 	ldr	r3, [r5, #68]
    ad0c:	e3530004 	cmp	r3, #4	; 0x4
    ad10:	05952118 	ldreq	r2, [r5, #280]
    ad14:	03a03001 	moveq	r3, #1	; 0x1
    ad18:	05823018 	streq	r3, [r2, #24]
    ad1c:	08bd8070 	popeq	{r4, r5, r6, pc}
    ad20:	e3530008 	cmp	r3, #8	; 0x8
    ad24:	e5950118 	ldr	r0, [r5, #280]
    ad28:	059f3010 	ldreq	r3, [pc, #16]	; ad40 <imap_sdhc_set_ios+0xfc>
    ad2c:	13a03000 	movne	r3, #0	; 0x0
    ad30:	e5803018 	str	r3, [r0, #24]
    ad34:	e8bd8070 	pop	{r4, r5, r6, pc}
    ad38:	021e0a24 	.word	0x021e0a24
    ad3c:	00202000 	.word	0x00202000
    ad40:	00010001 	.word	0x00010001

0000ad44 <generic_ffs>:
    ad44:	e3500000 	cmp	r0, #0	; 0x0
    ad48:	01a03000 	moveq	r3, r0
    ad4c:	0a000010 	beq	ad94 <generic_ffs+0x50>
    ad50:	e1a03800 	lsl	r3, r0, #16
    ad54:	e1a03823 	lsr	r3, r3, #16
    ad58:	e3530000 	cmp	r3, #0	; 0x0
    ad5c:	01a00840 	asreq	r0, r0, #16
    ad60:	13a03001 	movne	r3, #1	; 0x1
    ad64:	03a03011 	moveq	r3, #17	; 0x11
    ad68:	e31000ff 	tst	r0, #255	; 0xff
    ad6c:	01a00440 	asreq	r0, r0, #8
    ad70:	02833008 	addeq	r3, r3, #8	; 0x8
    ad74:	e310000f 	tst	r0, #15	; 0xf
    ad78:	01a00240 	asreq	r0, r0, #4
    ad7c:	02833004 	addeq	r3, r3, #4	; 0x4
    ad80:	e3100003 	tst	r0, #3	; 0x3
    ad84:	01a00140 	asreq	r0, r0, #2
    ad88:	02833002 	addeq	r3, r3, #2	; 0x2
    ad8c:	e3100001 	tst	r0, #1	; 0x1
    ad90:	02833001 	addeq	r3, r3, #1	; 0x1
    ad94:	e1a00003 	mov	r0, r3
    ad98:	e12fff1e 	bx	lr

0000ad9c <print_nand_cfg>:
    ad9c:	e92d4030 	push	{r4, r5, lr}
    ada0:	e1a05000 	mov	r5, r0
    ada4:	e24dd004 	sub	sp, sp, #4	; 0x4
    ada8:	e59f4110 	ldr	r4, [pc, #272]	; aec0 <print_nand_cfg+0x124>
    adac:	e59f0110 	ldr	r0, [pc, #272]	; aec4 <print_nand_cfg+0x128>
    adb0:	e1a0e00f 	mov	lr, pc
    adb4:	e594f000 	ldr	pc, [r4]
    adb8:	e5d51000 	ldrb	r1, [r5]
    adbc:	e59f0104 	ldr	r0, [pc, #260]	; aec8 <print_nand_cfg+0x12c>
    adc0:	e1a0e00f 	mov	lr, pc
    adc4:	e594f000 	ldr	pc, [r4]
    adc8:	e5d51001 	ldrb	r1, [r5, #1]
    adcc:	e59f00f8 	ldr	r0, [pc, #248]	; aecc <print_nand_cfg+0x130>
    add0:	e1a0e00f 	mov	lr, pc
    add4:	e594f000 	ldr	pc, [r4]
    add8:	e5d51002 	ldrb	r1, [r5, #2]
    addc:	e59f00ec 	ldr	r0, [pc, #236]	; aed0 <print_nand_cfg+0x134>
    ade0:	e1a0e00f 	mov	lr, pc
    ade4:	e594f000 	ldr	pc, [r4]
    ade8:	e5d51003 	ldrb	r1, [r5, #3]
    adec:	e59f00e0 	ldr	r0, [pc, #224]	; aed4 <print_nand_cfg+0x138>
    adf0:	e1a0e00f 	mov	lr, pc
    adf4:	e594f000 	ldr	pc, [r4]
    adf8:	e5d51004 	ldrb	r1, [r5, #4]
    adfc:	e59f00d4 	ldr	r0, [pc, #212]	; aed8 <print_nand_cfg+0x13c>
    ae00:	e1a0e00f 	mov	lr, pc
    ae04:	e594f000 	ldr	pc, [r4]
    ae08:	e5d51005 	ldrb	r1, [r5, #5]
    ae0c:	e59f00c8 	ldr	r0, [pc, #200]	; aedc <print_nand_cfg+0x140>
    ae10:	e1a0e00f 	mov	lr, pc
    ae14:	e594f000 	ldr	pc, [r4]
    ae18:	e5951008 	ldr	r1, [r5, #8]
    ae1c:	e59f00bc 	ldr	r0, [pc, #188]	; aee0 <print_nand_cfg+0x144>
    ae20:	e1a0e00f 	mov	lr, pc
    ae24:	e594f000 	ldr	pc, [r4]
    ae28:	e595100c 	ldr	r1, [r5, #12]
    ae2c:	e59f00b0 	ldr	r0, [pc, #176]	; aee4 <print_nand_cfg+0x148>
    ae30:	e1a0e00f 	mov	lr, pc
    ae34:	e594f000 	ldr	pc, [r4]
    ae38:	e5951010 	ldr	r1, [r5, #16]
    ae3c:	e59f00a4 	ldr	r0, [pc, #164]	; aee8 <print_nand_cfg+0x14c>
    ae40:	e1a0e00f 	mov	lr, pc
    ae44:	e594f000 	ldr	pc, [r4]
    ae48:	e5951014 	ldr	r1, [r5, #20]
    ae4c:	e59f0098 	ldr	r0, [pc, #152]	; aeec <print_nand_cfg+0x150>
    ae50:	e1a0e00f 	mov	lr, pc
    ae54:	e594f000 	ldr	pc, [r4]
    ae58:	e5951018 	ldr	r1, [r5, #24]
    ae5c:	e59f008c 	ldr	r0, [pc, #140]	; aef0 <print_nand_cfg+0x154>
    ae60:	e1a0e00f 	mov	lr, pc
    ae64:	e594f000 	ldr	pc, [r4]
    ae68:	e595101c 	ldr	r1, [r5, #28]
    ae6c:	e59f0080 	ldr	r0, [pc, #128]	; aef4 <print_nand_cfg+0x158>
    ae70:	e1a0e00f 	mov	lr, pc
    ae74:	e594f000 	ldr	pc, [r4]
    ae78:	e5951028 	ldr	r1, [r5, #40]
    ae7c:	e59f0074 	ldr	r0, [pc, #116]	; aef8 <print_nand_cfg+0x15c>
    ae80:	e1a0e00f 	mov	lr, pc
    ae84:	e594f000 	ldr	pc, [r4]
    ae88:	e5951030 	ldr	r1, [r5, #48]
    ae8c:	e59f0068 	ldr	r0, [pc, #104]	; aefc <print_nand_cfg+0x160>
    ae90:	e1a0e00f 	mov	lr, pc
    ae94:	e594f000 	ldr	pc, [r4]
    ae98:	e5951034 	ldr	r1, [r5, #52]
    ae9c:	e59f005c 	ldr	r0, [pc, #92]	; af00 <print_nand_cfg+0x164>
    aea0:	e1a0e00f 	mov	lr, pc
    aea4:	e594f000 	ldr	pc, [r4]
    aea8:	e5d51006 	ldrb	r1, [r5, #6]
    aeac:	e59f0050 	ldr	r0, [pc, #80]	; af04 <print_nand_cfg+0x168>
    aeb0:	e1a0e00f 	mov	lr, pc
    aeb4:	e594f000 	ldr	pc, [r4]
    aeb8:	e28dd004 	add	sp, sp, #4	; 0x4
    aebc:	e8bd8030 	pop	{r4, r5, pc}
    aec0:	3c00a5cc 	.word	0x3c00a5cc
    aec4:	00018acb 	.word	0x00018acb
    aec8:	00018add 	.word	0x00018add
    aecc:	00018ae5 	.word	0x00018ae5
    aed0:	00018af5 	.word	0x00018af5
    aed4:	00018b00 	.word	0x00018b00
    aed8:	00018b10 	.word	0x00018b10
    aedc:	00018b20 	.word	0x00018b20
    aee0:	00018b30 	.word	0x00018b30
    aee4:	00018b3a 	.word	0x00018b3a
    aee8:	00018b44 	.word	0x00018b44
    aeec:	00018b51 	.word	0x00018b51
    aef0:	00018b5f 	.word	0x00018b5f
    aef4:	00018b6e 	.word	0x00018b6e
    aef8:	00018b7d 	.word	0x00018b7d
    aefc:	00018b89 	.word	0x00018b89
    af00:	00018b96 	.word	0x00018b96
    af04:	00018ba4 	.word	0x00018ba4

0000af08 <nand_get_config>:
    af08:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    af0c:	e3500000 	cmp	r0, #0	; 0x0
    af10:	e24dd004 	sub	sp, sp, #4	; 0x4
    af14:	0a000007 	beq	af38 <nand_get_config+0x30>
    af18:	e3500001 	cmp	r0, #1	; 0x1
    af1c:	1a00000b 	bne	af50 <nand_get_config+0x48>
    af20:	e59f0048 	ldr	r0, [pc, #72]	; af70 <nand_get_config+0x68>
    af24:	e5903000 	ldr	r3, [r0]
    af28:	e3530000 	cmp	r3, #0	; 0x0
    af2c:	128000b0 	addne	r0, r0, #176	; 0xb0
    af30:	0a00000b 	beq	af64 <nand_get_config+0x5c>
    af34:	ea00000b 	b	af68 <nand_get_config+0x60>
    af38:	e59f0030 	ldr	r0, [pc, #48]	; af70 <nand_get_config+0x68>
    af3c:	e5903004 	ldr	r3, [r0, #4]
    af40:	e3530000 	cmp	r3, #0	; 0x0
    af44:	12800014 	addne	r0, r0, #20	; 0x14
    af48:	0a000005 	beq	af64 <nand_get_config+0x5c>
    af4c:	ea000005 	b	af68 <nand_get_config+0x60>
    af50:	e1a01000 	mov	r1, r0
    af54:	e59f3018 	ldr	r3, [pc, #24]	; af74 <nand_get_config+0x6c>
    af58:	e59f0018 	ldr	r0, [pc, #24]	; af78 <nand_get_config+0x70>
    af5c:	e1a0e00f 	mov	lr, pc
    af60:	e593f000 	ldr	pc, [r3]
    af64:	e3a00000 	mov	r0, #0	; 0x0
    af68:	e28dd004 	add	sp, sp, #4	; 0x4
    af6c:	e8bd8000 	pop	{pc}
    af70:	3c00a634 	.word	0x3c00a634
    af74:	3c00a5cc 	.word	0x3c00a5cc
    af78:	00018bae 	.word	0x00018bae

0000af7c <bnd_vs_align>:
    af7c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    af80:	e3a00000 	mov	r0, #0	; 0x0
    af84:	e24dd004 	sub	sp, sp, #4	; 0x4
    af88:	ebffffde 	bl	af08 <nand_get_config>
    af8c:	e3500000 	cmp	r0, #0	; 0x0
    af90:	15900014 	ldrne	r0, [r0, #20]
    af94:	e28dd004 	add	sp, sp, #4	; 0x4
    af98:	e8bd8000 	pop	{pc}

0000af9c <nnd_vs_align>:
    af9c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    afa0:	e3a00001 	mov	r0, #1	; 0x1
    afa4:	e24dd004 	sub	sp, sp, #4	; 0x4
    afa8:	ebffffd6 	bl	af08 <nand_get_config>
    afac:	e3500000 	cmp	r0, #0	; 0x0
    afb0:	15900014 	ldrne	r0, [r0, #20]
    afb4:	e28dd004 	add	sp, sp, #4	; 0x4
    afb8:	e8bd8000 	pop	{pc}

0000afbc <fnd_vs_align>:
    afbc:	eafffff6 	b	af9c <nnd_vs_align>

0000afc0 <fnd_align>:
    afc0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    afc4:	e3a00001 	mov	r0, #1	; 0x1
    afc8:	e24dd004 	sub	sp, sp, #4	; 0x4
    afcc:	ebffffcd 	bl	af08 <nand_get_config>
    afd0:	e3500000 	cmp	r0, #0	; 0x0
    afd4:	15902014 	ldrne	r2, [r0, #20]
    afd8:	15903010 	ldrne	r3, [r0, #16]
    afdc:	10830002 	addne	r0, r3, r2
    afe0:	e28dd004 	add	sp, sp, #4	; 0x4
    afe4:	e8bd8000 	pop	{pc}

0000afe8 <fnd_size_match>:
    afe8:	e92d4010 	push	{r4, lr}
    afec:	e1a04000 	mov	r4, r0
    aff0:	e3a00001 	mov	r0, #1	; 0x1
    aff4:	ebffffc3 	bl	af08 <nand_get_config>
    aff8:	e3500000 	cmp	r0, #0	; 0x0
    affc:	08bd8010 	popeq	{r4, pc}
    b000:	e5903014 	ldr	r3, [r0, #20]
    b004:	e5901010 	ldr	r1, [r0, #16]
    b008:	e1a00004 	mov	r0, r4
    b00c:	e0811003 	add	r1, r1, r3
    b010:	eb0027d7 	bl	14f74 <__aeabi_uidivmod>
    b014:	e2710001 	rsbs	r0, r1, #1	; 0x1
    b018:	33a00000 	movcc	r0, #0	; 0x0
    b01c:	e8bd8010 	pop	{r4, pc}

0000b020 <fnd_size_shrink>:
    b020:	e92d4030 	push	{r4, r5, lr}
    b024:	e1a05000 	mov	r5, r0
    b028:	e24dd004 	sub	sp, sp, #4	; 0x4
    b02c:	e3a00001 	mov	r0, #1	; 0x1
    b030:	ebffffb4 	bl	af08 <nand_get_config>
    b034:	e3500000 	cmp	r0, #0	; 0x0
    b038:	0a000005 	beq	b054 <fnd_size_shrink+0x34>
    b03c:	e5904014 	ldr	r4, [r0, #20]
    b040:	e5901010 	ldr	r1, [r0, #16]
    b044:	e1a00005 	mov	r0, r5
    b048:	e0841001 	add	r1, r4, r1
    b04c:	eb00274c 	bl	14d84 <__aeabi_uidiv>
    b050:	e0000094 	mul	r0, r4, r0
    b054:	e28dd004 	add	sp, sp, #4	; 0x4
    b058:	e8bd8030 	pop	{r4, r5, pc}

0000b05c <init_nand>:
    b05c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    b060:	e3a01000 	mov	r1, #0	; 0x0
    b064:	e24dd004 	sub	sp, sp, #4	; 0x4
    b068:	e3a02f53 	mov	r2, #332	; 0x14c
    b06c:	e59f000c 	ldr	r0, [pc, #12]	; b080 <init_nand+0x24>
    b070:	ebffd8f3 	bl	1444 <memset>
    b074:	e3a00000 	mov	r0, #0	; 0x0
    b078:	e28dd004 	add	sp, sp, #4	; 0x4
    b07c:	e8bd8000 	pop	{pc}
    b080:	3c00a634 	.word	0x3c00a634

0000b084 <__base_block_markbad>:
    b084:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    b088:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    b08c:	e1cd06f0 	strd	r0, [sp, #96]
    b090:	e3a00001 	mov	r0, #1	; 0x1
    b094:	ebffff9b 	bl	af08 <nand_get_config>
    b098:	e250a000 	subs	sl, r0, #0	; 0x0
    b09c:	03e00000 	mvneq	r0, #0	; 0x0
    b0a0:	0a000035 	beq	b17c <__base_block_markbad+0xf8>
    b0a4:	e3a04000 	mov	r4, #0	; 0x0
    b0a8:	e58d4074 	str	r4, [sp, #116]
    b0ac:	e59a5014 	ldr	r5, [sl, #20]
    b0b0:	e5da7005 	ldrb	r7, [sl, #5]
    b0b4:	e5da8003 	ldrb	r8, [sl, #3]
    b0b8:	e59f30c4 	ldr	r3, [pc, #196]	; b184 <__base_block_markbad+0x100>
    b0bc:	e58d5000 	str	r5, [sp]
    b0c0:	e59ac010 	ldr	ip, [sl, #16]
    b0c4:	e5932010 	ldr	r2, [r3, #16]
    b0c8:	e3a06001 	mov	r6, #1	; 0x1
    b0cc:	e1cd06d0 	ldrd	r0, [sp, #96]
    b0d0:	e58dc004 	str	ip, [sp, #4]
    b0d4:	e58d6008 	str	r6, [sp, #8]
    b0d8:	eb002840 	bl	151e0 <__aeabi_lasr>
    b0dc:	e58d5014 	str	r5, [sp, #20]
    b0e0:	e58d000c 	str	r0, [sp, #12]
    b0e4:	e58d4010 	str	r4, [sp, #16]
    b0e8:	e58d7018 	str	r7, [sp, #24]
    b0ec:	e5da3004 	ldrb	r3, [sl, #4]
    b0f0:	e58d4024 	str	r4, [sp, #36]
    b0f4:	e58d4020 	str	r4, [sp, #32]
    b0f8:	e58d301c 	str	r3, [sp, #28]
    b0fc:	e5da3002 	ldrb	r3, [sl, #2]
    b100:	e28a204c 	add	r2, sl, #76	; 0x4c
    b104:	e58d3028 	str	r3, [sp, #40]
    b108:	e5da3001 	ldrb	r3, [sl, #1]
    b10c:	e28a105c 	add	r1, sl, #92	; 0x5c
    b110:	e58d302c 	str	r3, [sp, #44]
    b114:	e28d3070 	add	r3, sp, #112	; 0x70
    b118:	e58d3048 	str	r3, [sp, #72]
    b11c:	e28d306c 	add	r3, sp, #108	; 0x6c
    b120:	e58d304c 	str	r3, [sp, #76]
    b124:	e28d3074 	add	r3, sp, #116	; 0x74
    b128:	e28a006c 	add	r0, sl, #108	; 0x6c
    b12c:	e28ac07c 	add	ip, sl, #124	; 0x7c
    b130:	e58d3050 	str	r3, [sp, #80]
    b134:	e28a5038 	add	r5, sl, #56	; 0x38
    b138:	e3a03004 	mov	r3, #4	; 0x4
    b13c:	e28a408c 	add	r4, sl, #140	; 0x8c
    b140:	e58d2034 	str	r2, [sp, #52]
    b144:	e58d1038 	str	r1, [sp, #56]
    b148:	e58d003c 	str	r0, [sp, #60]
    b14c:	e58d3054 	str	r3, [sp, #84]
    b150:	e58d5030 	str	r5, [sp, #48]
    b154:	e58dc040 	str	ip, [sp, #64]
    b158:	e58d4044 	str	r4, [sp, #68]
    b15c:	e58d6058 	str	r6, [sp, #88]
    b160:	e5dac006 	ldrb	ip, [sl, #6]
    b164:	e1a01006 	mov	r1, r6
    b168:	e1a00007 	mov	r0, r7
    b16c:	e1a02008 	mov	r2, r8
    b170:	e1a03006 	mov	r3, r6
    b174:	e58dc05c 	str	ip, [sp, #92]
    b178:	eb00093c 	bl	d670 <nf2_page_op>
    b17c:	e28dd07c 	add	sp, sp, #124	; 0x7c
    b180:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    b184:	3c00a634 	.word	0x3c00a634

0000b188 <bnd_vs_write>:
    b188:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b18c:	e3a05000 	mov	r5, #0	; 0x0
    b190:	e24dd094 	sub	sp, sp, #148	; 0x94
    b194:	e1a08000 	mov	r8, r0
    b198:	e1a00005 	mov	r0, r5
    b19c:	e1a06002 	mov	r6, r2
    b1a0:	e1a07003 	mov	r7, r3
    b1a4:	e58d508c 	str	r5, [sp, #140]
    b1a8:	e58d5088 	str	r5, [sp, #136]
    b1ac:	e58d5084 	str	r5, [sp, #132]
    b1b0:	ebffff54 	bl	af08 <nand_get_config>
    b1b4:	e1a04000 	mov	r4, r0
    b1b8:	e3a00001 	mov	r0, #1	; 0x1
    b1bc:	ebffff51 	bl	af08 <nand_get_config>
    b1c0:	e1540005 	cmp	r4, r5
    b1c4:	11500005 	cmpne	r0, r5
    b1c8:	0a000055 	beq	b324 <bnd_vs_write+0x19c>
    b1cc:	e59f3160 	ldr	r3, [pc, #352]	; b334 <bnd_vs_write+0x1ac>
    b1d0:	e1a00006 	mov	r0, r6
    b1d4:	e593200c 	ldr	r2, [r3, #12]
    b1d8:	e1a01007 	mov	r1, r7
    b1dc:	eb0027ff 	bl	151e0 <__aeabi_lasr>
    b1e0:	e5941014 	ldr	r1, [r4, #20]
    b1e4:	e58d0060 	str	r0, [sp, #96]
    b1e8:	e59d00b8 	ldr	r0, [sp, #184]
    b1ec:	eb0026e4 	bl	14d84 <__aeabi_uidiv>
    b1f0:	e58d0074 	str	r0, [sp, #116]
    b1f4:	e5d42005 	ldrb	r2, [r4, #5]
    b1f8:	e284c038 	add	ip, r4, #56	; 0x38
    b1fc:	e58d2078 	str	r2, [sp, #120]
    b200:	e5d43004 	ldrb	r3, [r4, #4]
    b204:	e284e04c 	add	lr, r4, #76	; 0x4c
    b208:	e58d307c 	str	r3, [sp, #124]
    b20c:	e284205c 	add	r2, r4, #92	; 0x5c
    b210:	e284306c 	add	r3, r4, #108	; 0x6c
    b214:	e1a07008 	mov	r7, r8
    b218:	e58dc064 	str	ip, [sp, #100]
    b21c:	e58de068 	str	lr, [sp, #104]
    b220:	e58d206c 	str	r2, [sp, #108]
    b224:	e58d3070 	str	r3, [sp, #112]
    b228:	e284b07c 	add	fp, r4, #124	; 0x7c
    b22c:	e284908c 	add	r9, r4, #140	; 0x8c
    b230:	e1a06005 	mov	r6, r5
    b234:	e1a0a005 	mov	sl, r5
    b238:	e3a08001 	mov	r8, #1	; 0x1
    b23c:	ea00002f 	b	b300 <bnd_vs_write+0x178>
    b240:	e59f20f0 	ldr	r2, [pc, #240]	; b338 <bnd_vs_write+0x1b0>
    b244:	e594c014 	ldr	ip, [r4, #20]
    b248:	e58d2084 	str	r2, [sp, #132]
    b24c:	e5d40005 	ldrb	r0, [r4, #5]
    b250:	e5d42003 	ldrb	r2, [r4, #3]
    b254:	e58de00c 	str	lr, [sp, #12]
    b258:	e58dc014 	str	ip, [sp, #20]
    b25c:	e58dc000 	str	ip, [sp]
    b260:	e59de07c 	ldr	lr, [sp, #124]
    b264:	e59dc078 	ldr	ip, [sp, #120]
    b268:	e58de01c 	str	lr, [sp, #28]
    b26c:	e58d7010 	str	r7, [sp, #16]
    b270:	e58da004 	str	sl, [sp, #4]
    b274:	e58d8008 	str	r8, [sp, #8]
    b278:	e58dc018 	str	ip, [sp, #24]
    b27c:	e58da020 	str	sl, [sp, #32]
    b280:	e58d8024 	str	r8, [sp, #36]
    b284:	e5d4c002 	ldrb	ip, [r4, #2]
    b288:	e59de064 	ldr	lr, [sp, #100]
    b28c:	e58dc028 	str	ip, [sp, #40]
    b290:	e5d4c001 	ldrb	ip, [r4, #1]
    b294:	e58de030 	str	lr, [sp, #48]
    b298:	e58dc02c 	str	ip, [sp, #44]
    b29c:	e59dc068 	ldr	ip, [sp, #104]
    b2a0:	e59de06c 	ldr	lr, [sp, #108]
    b2a4:	e58dc034 	str	ip, [sp, #52]
    b2a8:	e59dc070 	ldr	ip, [sp, #112]
    b2ac:	e58de038 	str	lr, [sp, #56]
    b2b0:	e58dc03c 	str	ip, [sp, #60]
    b2b4:	e28de08c 	add	lr, sp, #140	; 0x8c
    b2b8:	e28dc088 	add	ip, sp, #136	; 0x88
    b2bc:	e58de048 	str	lr, [sp, #72]
    b2c0:	e58dc04c 	str	ip, [sp, #76]
    b2c4:	e28de084 	add	lr, sp, #132	; 0x84
    b2c8:	e3a0c004 	mov	ip, #4	; 0x4
    b2cc:	e58de050 	str	lr, [sp, #80]
    b2d0:	e58db040 	str	fp, [sp, #64]
    b2d4:	e58d9044 	str	r9, [sp, #68]
    b2d8:	e58dc054 	str	ip, [sp, #84]
    b2dc:	e58d8058 	str	r8, [sp, #88]
    b2e0:	e5d4c006 	ldrb	ip, [r4, #6]
    b2e4:	e58dc05c 	str	ip, [sp, #92]
    b2e8:	eb0008e0 	bl	d670 <nf2_page_op>
    b2ec:	e3500000 	cmp	r0, #0	; 0x0
    b2f0:	1a00000b 	bne	b324 <bnd_vs_write+0x19c>
    b2f4:	e5943014 	ldr	r3, [r4, #20]
    b2f8:	e0866003 	add	r6, r6, r3
    b2fc:	e0877003 	add	r7, r7, r3
    b300:	e59de074 	ldr	lr, [sp, #116]
    b304:	e59d2060 	ldr	r2, [sp, #96]
    b308:	e155000e 	cmp	r5, lr
    b30c:	e1a0100a 	mov	r1, sl
    b310:	e085e002 	add	lr, r5, r2
    b314:	e1a03008 	mov	r3, r8
    b318:	e2855001 	add	r5, r5, #1	; 0x1
    b31c:	baffffc7 	blt	b240 <bnd_vs_write+0xb8>
    b320:	ea000000 	b	b328 <bnd_vs_write+0x1a0>
    b324:	e3e06000 	mvn	r6, #0	; 0x0
    b328:	e1a00006 	mov	r0, r6
    b32c:	e28dd094 	add	sp, sp, #148	; 0x94
    b330:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b334:	3c00a634 	.word	0x3c00a634
    b338:	031800ff 	.word	0x031800ff

0000b33c <bnd_vs_read>:
    b33c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b340:	e3a05000 	mov	r5, #0	; 0x0
    b344:	e24dd094 	sub	sp, sp, #148	; 0x94
    b348:	e1a08000 	mov	r8, r0
    b34c:	e1a00005 	mov	r0, r5
    b350:	e1a06002 	mov	r6, r2
    b354:	e1a07003 	mov	r7, r3
    b358:	e58d508c 	str	r5, [sp, #140]
    b35c:	e58d5088 	str	r5, [sp, #136]
    b360:	e58d5084 	str	r5, [sp, #132]
    b364:	ebfffee7 	bl	af08 <nand_get_config>
    b368:	e2504000 	subs	r4, r0, #0	; 0x0
    b36c:	0a00005c 	beq	b4e4 <bnd_vs_read+0x1a8>
    b370:	e59f317c 	ldr	r3, [pc, #380]	; b4f4 <bnd_vs_read+0x1b8>
    b374:	e1a00006 	mov	r0, r6
    b378:	e593200c 	ldr	r2, [r3, #12]
    b37c:	e1a01007 	mov	r1, r7
    b380:	eb002796 	bl	151e0 <__aeabi_lasr>
    b384:	e5941014 	ldr	r1, [r4, #20]
    b388:	e58d0060 	str	r0, [sp, #96]
    b38c:	e59d00b8 	ldr	r0, [sp, #184]
    b390:	eb00267b 	bl	14d84 <__aeabi_uidiv>
    b394:	e58d0074 	str	r0, [sp, #116]
    b398:	e5d42005 	ldrb	r2, [r4, #5]
    b39c:	e284c038 	add	ip, r4, #56	; 0x38
    b3a0:	e58d2078 	str	r2, [sp, #120]
    b3a4:	e5d43004 	ldrb	r3, [r4, #4]
    b3a8:	e284e04c 	add	lr, r4, #76	; 0x4c
    b3ac:	e58d307c 	str	r3, [sp, #124]
    b3b0:	e284205c 	add	r2, r4, #92	; 0x5c
    b3b4:	e284306c 	add	r3, r4, #108	; 0x6c
    b3b8:	e1a07008 	mov	r7, r8
    b3bc:	e58dc064 	str	ip, [sp, #100]
    b3c0:	e58de068 	str	lr, [sp, #104]
    b3c4:	e58d206c 	str	r2, [sp, #108]
    b3c8:	e58d3070 	str	r3, [sp, #112]
    b3cc:	e284b07c 	add	fp, r4, #124	; 0x7c
    b3d0:	e284908c 	add	r9, r4, #140	; 0x8c
    b3d4:	e1a06005 	mov	r6, r5
    b3d8:	e1a08005 	mov	r8, r5
    b3dc:	e3a0a001 	mov	sl, #1	; 0x1
    b3e0:	ea000036 	b	b4c0 <bnd_vs_read+0x184>
    b3e4:	e594c014 	ldr	ip, [r4, #20]
    b3e8:	e5d40005 	ldrb	r0, [r4, #5]
    b3ec:	e5d42003 	ldrb	r2, [r4, #3]
    b3f0:	e58de00c 	str	lr, [sp, #12]
    b3f4:	e58dc014 	str	ip, [sp, #20]
    b3f8:	e58dc000 	str	ip, [sp]
    b3fc:	e59de07c 	ldr	lr, [sp, #124]
    b400:	e59dc078 	ldr	ip, [sp, #120]
    b404:	e58de01c 	str	lr, [sp, #28]
    b408:	e58d7010 	str	r7, [sp, #16]
    b40c:	e58d8004 	str	r8, [sp, #4]
    b410:	e58d8008 	str	r8, [sp, #8]
    b414:	e58dc018 	str	ip, [sp, #24]
    b418:	e58d8020 	str	r8, [sp, #32]
    b41c:	e58da024 	str	sl, [sp, #36]
    b420:	e5d4c002 	ldrb	ip, [r4, #2]
    b424:	e59de064 	ldr	lr, [sp, #100]
    b428:	e58dc028 	str	ip, [sp, #40]
    b42c:	e5d4c001 	ldrb	ip, [r4, #1]
    b430:	e58de030 	str	lr, [sp, #48]
    b434:	e58dc02c 	str	ip, [sp, #44]
    b438:	e59dc068 	ldr	ip, [sp, #104]
    b43c:	e59de06c 	ldr	lr, [sp, #108]
    b440:	e58dc034 	str	ip, [sp, #52]
    b444:	e59dc070 	ldr	ip, [sp, #112]
    b448:	e58de038 	str	lr, [sp, #56]
    b44c:	e58dc03c 	str	ip, [sp, #60]
    b450:	e28de08c 	add	lr, sp, #140	; 0x8c
    b454:	e28dc088 	add	ip, sp, #136	; 0x88
    b458:	e58de048 	str	lr, [sp, #72]
    b45c:	e58dc04c 	str	ip, [sp, #76]
    b460:	e28de084 	add	lr, sp, #132	; 0x84
    b464:	e3a0c004 	mov	ip, #4	; 0x4
    b468:	e58de050 	str	lr, [sp, #80]
    b46c:	e58dc054 	str	ip, [sp, #84]
    b470:	e58db040 	str	fp, [sp, #64]
    b474:	e58d9044 	str	r9, [sp, #68]
    b478:	e58da058 	str	sl, [sp, #88]
    b47c:	e5d4c006 	ldrb	ip, [r4, #6]
    b480:	e58dc05c 	str	ip, [sp, #92]
    b484:	eb000879 	bl	d670 <nf2_page_op>
    b488:	e3500000 	cmp	r0, #0	; 0x0
    b48c:	1a000014 	bne	b4e4 <bnd_vs_read+0x1a8>
    b490:	e59d308c 	ldr	r3, [sp, #140]
    b494:	e1dd28b6 	ldrh	r2, [sp, #134]
    b498:	e3530000 	cmp	r3, #0	; 0x0
    b49c:	1a000006 	bne	b4bc <bnd_vs_read+0x180>
    b4a0:	e59d3088 	ldr	r3, [sp, #136]
    b4a4:	e3530000 	cmp	r3, #0	; 0x0
    b4a8:	1a000003 	bne	b4bc <bnd_vs_read+0x180>
    b4ac:	e3520fc6 	cmp	r2, #792	; 0x318
    b4b0:	05943014 	ldreq	r3, [r4, #20]
    b4b4:	00877003 	addeq	r7, r7, r3
    b4b8:	00866003 	addeq	r6, r6, r3
    b4bc:	e58d2084 	str	r2, [sp, #132]
    b4c0:	e59de074 	ldr	lr, [sp, #116]
    b4c4:	e59d2060 	ldr	r2, [sp, #96]
    b4c8:	e155000e 	cmp	r5, lr
    b4cc:	e1a01008 	mov	r1, r8
    b4d0:	e085e002 	add	lr, r5, r2
    b4d4:	e1a0300a 	mov	r3, sl
    b4d8:	e2855001 	add	r5, r5, #1	; 0x1
    b4dc:	baffffc0 	blt	b3e4 <bnd_vs_read+0xa8>
    b4e0:	ea000000 	b	b4e8 <bnd_vs_read+0x1ac>
    b4e4:	e3e06000 	mvn	r6, #0	; 0x0
    b4e8:	e1a00006 	mov	r0, r6
    b4ec:	e28dd094 	add	sp, sp, #148	; 0x94
    b4f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b4f4:	3c00a634 	.word	0x3c00a634

0000b4f8 <base_vs_write>:
    b4f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b4fc:	e3a01000 	mov	r1, #0	; 0x0
    b500:	e24dd094 	sub	sp, sp, #148	; 0x94
    b504:	e1a0a000 	mov	sl, r0
    b508:	e3a00001 	mov	r0, #1	; 0x1
    b50c:	e58d1088 	str	r1, [sp, #136]
    b510:	e1a08002 	mov	r8, r2
    b514:	e1a09003 	mov	r9, r3
    b518:	e58d108c 	str	r1, [sp, #140]
    b51c:	e59db0bc 	ldr	fp, [sp, #188]
    b520:	ebfffe78 	bl	af08 <nand_get_config>
    b524:	e1a04000 	mov	r4, r0
    b528:	e5900010 	ldr	r0, [r0, #16]
    b52c:	e2800004 	add	r0, r0, #4	; 0x4
    b530:	ebffd830 	bl	15f8 <malloc>
    b534:	e2507000 	subs	r7, r0, #0	; 0x0
    b538:	1a000007 	bne	b55c <base_vs_write+0x64>
    b53c:	e5941010 	ldr	r1, [r4, #16]
    b540:	e59f0240 	ldr	r0, [pc, #576]	; b788 <base_vs_write+0x290>
    b544:	e2811004 	add	r1, r1, #4	; 0x4
    b548:	e59f323c 	ldr	r3, [pc, #572]	; b78c <base_vs_write+0x294>
    b54c:	e1a0e00f 	mov	lr, pc
    b550:	e593f000 	ldr	pc, [r3]
    b554:	e3e05000 	mvn	r5, #0	; 0x0
    b558:	ea000087 	b	b77c <base_vs_write+0x284>
    b55c:	e35b0000 	cmp	fp, #0	; 0x0
    b560:	01a0200b 	moveq	r2, fp
    b564:	03e01000 	mvneq	r1, #0	; 0x0
    b568:	0a000002 	beq	b578 <base_vs_write+0x80>
    b56c:	ea000007 	b	b590 <base_vs_write+0x98>
    b570:	e7c71002 	strb	r1, [r7, r2]
    b574:	e2822001 	add	r2, r2, #1	; 0x1
    b578:	e5943010 	ldr	r3, [r4, #16]
    b57c:	e2833004 	add	r3, r3, #4	; 0x4
    b580:	e1520003 	cmp	r2, r3
    b584:	3afffff9 	bcc	b570 <base_vs_write+0x78>
    b588:	e3a03000 	mov	r3, #0	; 0x0
    b58c:	e5c73001 	strb	r3, [r7, #1]
    b590:	e59f31f8 	ldr	r3, [pc, #504]	; b790 <base_vs_write+0x298>
    b594:	e35b0000 	cmp	fp, #0	; 0x0
    b598:	e5932010 	ldr	r2, [r3, #16]
    b59c:	e1a00008 	mov	r0, r8
    b5a0:	e1a01009 	mov	r1, r9
    b5a4:	01a0500b 	moveq	r5, fp
    b5a8:	15945010 	ldrne	r5, [r4, #16]
    b5ac:	eb00270b 	bl	151e0 <__aeabi_lasr>
    b5b0:	e5946014 	ldr	r6, [r4, #20]
    b5b4:	e58d0060 	str	r0, [sp, #96]
    b5b8:	e0851006 	add	r1, r5, r6
    b5bc:	e59d00b8 	ldr	r0, [sp, #184]
    b5c0:	eb0025ef 	bl	14d84 <__aeabi_uidiv>
    b5c4:	e58d007c 	str	r0, [sp, #124]
    b5c8:	e5d43005 	ldrb	r3, [r4, #5]
    b5cc:	e3a06000 	mov	r6, #0	; 0x0
    b5d0:	e58d3080 	str	r3, [sp, #128]
    b5d4:	e5d43004 	ldrb	r3, [r4, #4]
    b5d8:	e1a0800a 	mov	r8, sl
    b5dc:	e58d3084 	str	r3, [sp, #132]
    b5e0:	e2843038 	add	r3, r4, #56	; 0x38
    b5e4:	e58d3064 	str	r3, [sp, #100]
    b5e8:	e284304c 	add	r3, r4, #76	; 0x4c
    b5ec:	e58d3068 	str	r3, [sp, #104]
    b5f0:	e284305c 	add	r3, r4, #92	; 0x5c
    b5f4:	e58d306c 	str	r3, [sp, #108]
    b5f8:	e284306c 	add	r3, r4, #108	; 0x6c
    b5fc:	e58d3070 	str	r3, [sp, #112]
    b600:	e284307c 	add	r3, r4, #124	; 0x7c
    b604:	e58d3074 	str	r3, [sp, #116]
    b608:	e284308c 	add	r3, r4, #140	; 0x8c
    b60c:	e58d3078 	str	r3, [sp, #120]
    b610:	e1a05006 	mov	r5, r6
    b614:	e1a0a006 	mov	sl, r6
    b618:	e3a09001 	mov	r9, #1	; 0x1
    b61c:	ea00004f 	b	b760 <base_vs_write+0x268>
    b620:	e35b0001 	cmp	fp, #1	; 0x1
    b624:	1a000012 	bne	b674 <base_vs_write+0x17c>
    b628:	e3e03000 	mvn	r3, #0	; 0x0
    b62c:	e5c73000 	strb	r3, [r7]
    b630:	e3a03000 	mov	r3, #0	; 0x0
    b634:	e5c73001 	strb	r3, [r7, #1]
    b638:	e3e03000 	mvn	r3, #0	; 0x0
    b63c:	e5c73002 	strb	r3, [r7, #2]
    b640:	e5c73003 	strb	r3, [r7, #3]
    b644:	e1a01007 	mov	r1, r7
    b648:	e3a02000 	mov	r2, #0	; 0x0
    b64c:	ea000004 	b	b664 <base_vs_write+0x16c>
    b650:	e5943014 	ldr	r3, [r4, #20]
    b654:	e0883003 	add	r3, r8, r3
    b658:	e7d33002 	ldrb	r3, [r3, r2]
    b65c:	e2822001 	add	r2, r2, #1	; 0x1
    b660:	e5c13003 	strb	r3, [r1, #3]
    b664:	e5943010 	ldr	r3, [r4, #16]
    b668:	e2811001 	add	r1, r1, #1	; 0x1
    b66c:	e1520003 	cmp	r2, r3
    b670:	3afffff6 	bcc	b650 <base_vs_write+0x158>
    b674:	e5943014 	ldr	r3, [r4, #20]
    b678:	e5941010 	ldr	r1, [r4, #16]
    b67c:	e5d40005 	ldrb	r0, [r4, #5]
    b680:	e5d42003 	ldrb	r2, [r4, #3]
    b684:	e58d3014 	str	r3, [sp, #20]
    b688:	e58d3000 	str	r3, [sp]
    b68c:	e59d3080 	ldr	r3, [sp, #128]
    b690:	e58dc00c 	str	ip, [sp, #12]
    b694:	e58d3018 	str	r3, [sp, #24]
    b698:	e59d3084 	ldr	r3, [sp, #132]
    b69c:	e58d1004 	str	r1, [sp, #4]
    b6a0:	e58d301c 	str	r3, [sp, #28]
    b6a4:	e3a03000 	mov	r3, #0	; 0x0
    b6a8:	e58d9008 	str	r9, [sp, #8]
    b6ac:	e58d8010 	str	r8, [sp, #16]
    b6b0:	e58d3020 	str	r3, [sp, #32]
    b6b4:	e58d9024 	str	r9, [sp, #36]
    b6b8:	e5d43002 	ldrb	r3, [r4, #2]
    b6bc:	e2811004 	add	r1, r1, #4	; 0x4
    b6c0:	e58d3028 	str	r3, [sp, #40]
    b6c4:	e5d43001 	ldrb	r3, [r4, #1]
    b6c8:	e58d1054 	str	r1, [sp, #84]
    b6cc:	e58d302c 	str	r3, [sp, #44]
    b6d0:	e59d3064 	ldr	r3, [sp, #100]
    b6d4:	e3a01000 	mov	r1, #0	; 0x0
    b6d8:	e58d3030 	str	r3, [sp, #48]
    b6dc:	e59d3068 	ldr	r3, [sp, #104]
    b6e0:	e58d3034 	str	r3, [sp, #52]
    b6e4:	e59d306c 	ldr	r3, [sp, #108]
    b6e8:	e58d3038 	str	r3, [sp, #56]
    b6ec:	e59d3070 	ldr	r3, [sp, #112]
    b6f0:	e58d303c 	str	r3, [sp, #60]
    b6f4:	e59d3074 	ldr	r3, [sp, #116]
    b6f8:	e58d3040 	str	r3, [sp, #64]
    b6fc:	e59d3078 	ldr	r3, [sp, #120]
    b700:	e58d3044 	str	r3, [sp, #68]
    b704:	e28d308c 	add	r3, sp, #140	; 0x8c
    b708:	e58d3048 	str	r3, [sp, #72]
    b70c:	e28d3088 	add	r3, sp, #136	; 0x88
    b710:	e58d304c 	str	r3, [sp, #76]
    b714:	e58d7050 	str	r7, [sp, #80]
    b718:	e58d9058 	str	r9, [sp, #88]
    b71c:	e5d4c006 	ldrb	ip, [r4, #6]
    b720:	e1a03009 	mov	r3, r9
    b724:	e58dc05c 	str	ip, [sp, #92]
    b728:	eb0007d0 	bl	d670 <nf2_page_op>
    b72c:	e3500000 	cmp	r0, #0	; 0x0
    b730:	13e05000 	mvnne	r5, #0	; 0x0
    b734:	1a00000e 	bne	b774 <base_vs_write+0x27c>
    b738:	e35b0000 	cmp	fp, #0	; 0x0
    b73c:	e5942014 	ldr	r2, [r4, #20]
    b740:	15943010 	ldrne	r3, [r4, #16]
    b744:	008aa002 	addeq	sl, sl, r2
    b748:	10833002 	addne	r3, r3, r2
    b74c:	108aa003 	addne	sl, sl, r3
    b750:	00888002 	addeq	r8, r8, r2
    b754:	10888003 	addne	r8, r8, r3
    b758:	e2866001 	add	r6, r6, #1	; 0x1
    b75c:	e1a0500a 	mov	r5, sl
    b760:	e59d307c 	ldr	r3, [sp, #124]
    b764:	e1560003 	cmp	r6, r3
    b768:	e59d3060 	ldr	r3, [sp, #96]
    b76c:	e086c003 	add	ip, r6, r3
    b770:	baffffaa 	blt	b620 <base_vs_write+0x128>
    b774:	e1a00007 	mov	r0, r7
    b778:	ebffd806 	bl	1798 <free>
    b77c:	e1a00005 	mov	r0, r5
    b780:	e28dd094 	add	sp, sp, #148	; 0x94
    b784:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b788:	00018bcc 	.word	0x00018bcc
    b78c:	3c00a5cc 	.word	0x3c00a5cc
    b790:	3c00a634 	.word	0x3c00a634

0000b794 <fnd_vs_write>:
    b794:	e3a01001 	mov	r1, #1	; 0x1
    b798:	e58d1004 	str	r1, [sp, #4]
    b79c:	eaffff55 	b	b4f8 <base_vs_write>

0000b7a0 <nnd_vs_write>:
    b7a0:	e3a01000 	mov	r1, #0	; 0x0
    b7a4:	e58d1004 	str	r1, [sp, #4]
    b7a8:	eaffff52 	b	b4f8 <base_vs_write>

0000b7ac <base_vs_read>:
    b7ac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b7b0:	e3a01000 	mov	r1, #0	; 0x0
    b7b4:	e24dd094 	sub	sp, sp, #148	; 0x94
    b7b8:	e1a09000 	mov	r9, r0
    b7bc:	e3a00001 	mov	r0, #1	; 0x1
    b7c0:	e58d1088 	str	r1, [sp, #136]
    b7c4:	e1a06002 	mov	r6, r2
    b7c8:	e1a07003 	mov	r7, r3
    b7cc:	e58d108c 	str	r1, [sp, #140]
    b7d0:	e59db0bc 	ldr	fp, [sp, #188]
    b7d4:	ebfffdcb 	bl	af08 <nand_get_config>
    b7d8:	e1a04000 	mov	r4, r0
    b7dc:	e5900010 	ldr	r0, [r0, #16]
    b7e0:	e2800004 	add	r0, r0, #4	; 0x4
    b7e4:	ebffd783 	bl	15f8 <malloc>
    b7e8:	e250a000 	subs	sl, r0, #0	; 0x0
    b7ec:	1a000007 	bne	b810 <base_vs_read+0x64>
    b7f0:	e5941010 	ldr	r1, [r4, #16]
    b7f4:	e59f0278 	ldr	r0, [pc, #632]	; ba74 <base_vs_read+0x2c8>
    b7f8:	e2811004 	add	r1, r1, #4	; 0x4
    b7fc:	e59f3274 	ldr	r3, [pc, #628]	; ba78 <base_vs_read+0x2cc>
    b800:	e1a0e00f 	mov	lr, pc
    b804:	e593f000 	ldr	pc, [r3]
    b808:	e3e05000 	mvn	r5, #0	; 0x0
    b80c:	ea000095 	b	ba68 <base_vs_read+0x2bc>
    b810:	e59f3264 	ldr	r3, [pc, #612]	; ba7c <base_vs_read+0x2d0>
    b814:	e35b0000 	cmp	fp, #0	; 0x0
    b818:	e5932010 	ldr	r2, [r3, #16]
    b81c:	e1a00006 	mov	r0, r6
    b820:	e1a01007 	mov	r1, r7
    b824:	01a0500b 	moveq	r5, fp
    b828:	15945010 	ldrne	r5, [r4, #16]
    b82c:	eb00266b 	bl	151e0 <__aeabi_lasr>
    b830:	e5948014 	ldr	r8, [r4, #20]
    b834:	e58d0060 	str	r0, [sp, #96]
    b838:	e0851008 	add	r1, r5, r8
    b83c:	e59d00b8 	ldr	r0, [sp, #184]
    b840:	eb00254f 	bl	14d84 <__aeabi_uidiv>
    b844:	e58d007c 	str	r0, [sp, #124]
    b848:	e5d43005 	ldrb	r3, [r4, #5]
    b84c:	e3a07000 	mov	r7, #0	; 0x0
    b850:	e58d3080 	str	r3, [sp, #128]
    b854:	e5d43004 	ldrb	r3, [r4, #4]
    b858:	e1a06009 	mov	r6, r9
    b85c:	e58d3084 	str	r3, [sp, #132]
    b860:	e2843038 	add	r3, r4, #56	; 0x38
    b864:	e58d3064 	str	r3, [sp, #100]
    b868:	e284304c 	add	r3, r4, #76	; 0x4c
    b86c:	e58d3068 	str	r3, [sp, #104]
    b870:	e284305c 	add	r3, r4, #92	; 0x5c
    b874:	e58d306c 	str	r3, [sp, #108]
    b878:	e284306c 	add	r3, r4, #108	; 0x6c
    b87c:	e58d3070 	str	r3, [sp, #112]
    b880:	e284307c 	add	r3, r4, #124	; 0x7c
    b884:	e58d3074 	str	r3, [sp, #116]
    b888:	e284308c 	add	r3, r4, #140	; 0x8c
    b88c:	e58d3078 	str	r3, [sp, #120]
    b890:	e1a05007 	mov	r5, r7
    b894:	e1a08007 	mov	r8, r7
    b898:	e3a09001 	mov	r9, #1	; 0x1
    b89c:	ea00006a 	b	ba4c <base_vs_read+0x2a0>
    b8a0:	e5943014 	ldr	r3, [r4, #20]
    b8a4:	e5941010 	ldr	r1, [r4, #16]
    b8a8:	e5d40005 	ldrb	r0, [r4, #5]
    b8ac:	e5d42003 	ldrb	r2, [r4, #3]
    b8b0:	e58d3014 	str	r3, [sp, #20]
    b8b4:	e58d3000 	str	r3, [sp]
    b8b8:	e3a03000 	mov	r3, #0	; 0x0
    b8bc:	e58d3008 	str	r3, [sp, #8]
    b8c0:	e59d3080 	ldr	r3, [sp, #128]
    b8c4:	e58dc00c 	str	ip, [sp, #12]
    b8c8:	e58d3018 	str	r3, [sp, #24]
    b8cc:	e59d3084 	ldr	r3, [sp, #132]
    b8d0:	e58d1004 	str	r1, [sp, #4]
    b8d4:	e58d301c 	str	r3, [sp, #28]
    b8d8:	e3a03000 	mov	r3, #0	; 0x0
    b8dc:	e58d6010 	str	r6, [sp, #16]
    b8e0:	e58d3020 	str	r3, [sp, #32]
    b8e4:	e58d9024 	str	r9, [sp, #36]
    b8e8:	e5d43002 	ldrb	r3, [r4, #2]
    b8ec:	e2811004 	add	r1, r1, #4	; 0x4
    b8f0:	e58d3028 	str	r3, [sp, #40]
    b8f4:	e5d43001 	ldrb	r3, [r4, #1]
    b8f8:	e58d1054 	str	r1, [sp, #84]
    b8fc:	e58d302c 	str	r3, [sp, #44]
    b900:	e59d3064 	ldr	r3, [sp, #100]
    b904:	e3a01000 	mov	r1, #0	; 0x0
    b908:	e58d3030 	str	r3, [sp, #48]
    b90c:	e59d3068 	ldr	r3, [sp, #104]
    b910:	e58d3034 	str	r3, [sp, #52]
    b914:	e59d306c 	ldr	r3, [sp, #108]
    b918:	e58d3038 	str	r3, [sp, #56]
    b91c:	e59d3070 	ldr	r3, [sp, #112]
    b920:	e58d303c 	str	r3, [sp, #60]
    b924:	e59d3074 	ldr	r3, [sp, #116]
    b928:	e58d3040 	str	r3, [sp, #64]
    b92c:	e59d3078 	ldr	r3, [sp, #120]
    b930:	e58d3044 	str	r3, [sp, #68]
    b934:	e28d308c 	add	r3, sp, #140	; 0x8c
    b938:	e58d3048 	str	r3, [sp, #72]
    b93c:	e28d3088 	add	r3, sp, #136	; 0x88
    b940:	e58d304c 	str	r3, [sp, #76]
    b944:	e58da050 	str	sl, [sp, #80]
    b948:	e58d9058 	str	r9, [sp, #88]
    b94c:	e5d4c006 	ldrb	ip, [r4, #6]
    b950:	e1a03009 	mov	r3, r9
    b954:	e58dc05c 	str	ip, [sp, #92]
    b958:	eb000744 	bl	d670 <nf2_page_op>
    b95c:	e3500000 	cmp	r0, #0	; 0x0
    b960:	1a00001d 	bne	b9dc <base_vs_read+0x230>
    b964:	e5da1001 	ldrb	r1, [sl, #1]
    b968:	e35100ff 	cmp	r1, #255	; 0xff
    b96c:	1a00000b 	bne	b9a0 <base_vs_read+0x1f4>
    b970:	e35b0000 	cmp	fp, #0	; 0x0
    b974:	1a000003 	bne	b988 <base_vs_read+0x1dc>
    b978:	e1a00006 	mov	r0, r6
    b97c:	e5942014 	ldr	r2, [r4, #20]
    b980:	ebffd6af 	bl	1444 <memset>
    b984:	ea000027 	b	ba28 <base_vs_read+0x27c>
    b988:	e5943010 	ldr	r3, [r4, #16]
    b98c:	e5942014 	ldr	r2, [r4, #20]
    b990:	e1a00006 	mov	r0, r6
    b994:	e0832002 	add	r2, r3, r2
    b998:	ebffd6a9 	bl	1444 <memset>
    b99c:	ea000010 	b	b9e4 <base_vs_read+0x238>
    b9a0:	e59d108c 	ldr	r1, [sp, #140]
    b9a4:	e3510000 	cmp	r1, #0	; 0x0
    b9a8:	1a000002 	bne	b9b8 <base_vs_read+0x20c>
    b9ac:	e59d3088 	ldr	r3, [sp, #136]
    b9b0:	e3530000 	cmp	r3, #0	; 0x0
    b9b4:	0a00000a 	beq	b9e4 <base_vs_read+0x238>
    b9b8:	e59d2088 	ldr	r2, [sp, #136]
    b9bc:	e2511000 	subs	r1, r1, #0	; 0x0
    b9c0:	13a01001 	movne	r1, #1	; 0x1
    b9c4:	e2522000 	subs	r2, r2, #0	; 0x0
    b9c8:	13a02001 	movne	r2, #1	; 0x1
    b9cc:	e59f00ac 	ldr	r0, [pc, #172]	; ba80 <base_vs_read+0x2d4>
    b9d0:	e59f30a0 	ldr	r3, [pc, #160]	; ba78 <base_vs_read+0x2cc>
    b9d4:	e1a0e00f 	mov	lr, pc
    b9d8:	e593f000 	ldr	pc, [r3]
    b9dc:	e3e05000 	mvn	r5, #0	; 0x0
    b9e0:	ea00001e 	b	ba60 <base_vs_read+0x2b4>
    b9e4:	e35b0001 	cmp	fp, #1	; 0x1
    b9e8:	01a0000a 	moveq	r0, sl
    b9ec:	03a01000 	moveq	r1, #0	; 0x0
    b9f0:	1a00000a 	bne	ba20 <base_vs_read+0x274>
    b9f4:	ea000004 	b	ba0c <base_vs_read+0x260>
    b9f8:	e5943014 	ldr	r3, [r4, #20]
    b9fc:	e5d02003 	ldrb	r2, [r0, #3]
    ba00:	e0863003 	add	r3, r6, r3
    ba04:	e7c32001 	strb	r2, [r3, r1]
    ba08:	e2811001 	add	r1, r1, #1	; 0x1
    ba0c:	e5943010 	ldr	r3, [r4, #16]
    ba10:	e2800001 	add	r0, r0, #1	; 0x1
    ba14:	e1510003 	cmp	r1, r3
    ba18:	3afffff6 	bcc	b9f8 <base_vs_read+0x24c>
    ba1c:	ea000003 	b	ba30 <base_vs_read+0x284>
    ba20:	e35b0000 	cmp	fp, #0	; 0x0
    ba24:	1a000001 	bne	ba30 <base_vs_read+0x284>
    ba28:	e5943014 	ldr	r3, [r4, #20]
    ba2c:	ea000002 	b	ba3c <base_vs_read+0x290>
    ba30:	e5943010 	ldr	r3, [r4, #16]
    ba34:	e5942014 	ldr	r2, [r4, #20]
    ba38:	e0833002 	add	r3, r3, r2
    ba3c:	e0888003 	add	r8, r8, r3
    ba40:	e0866003 	add	r6, r6, r3
    ba44:	e2877001 	add	r7, r7, #1	; 0x1
    ba48:	e1a05008 	mov	r5, r8
    ba4c:	e59d307c 	ldr	r3, [sp, #124]
    ba50:	e1570003 	cmp	r7, r3
    ba54:	e59d3060 	ldr	r3, [sp, #96]
    ba58:	e087c003 	add	ip, r7, r3
    ba5c:	baffff8f 	blt	b8a0 <base_vs_read+0xf4>
    ba60:	e1a0000a 	mov	r0, sl
    ba64:	ebffd74b 	bl	1798 <free>
    ba68:	e1a00005 	mov	r0, r5
    ba6c:	e28dd094 	add	sp, sp, #148	; 0x94
    ba70:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ba74:	00018bea 	.word	0x00018bea
    ba78:	3c00a5cc 	.word	0x3c00a5cc
    ba7c:	3c00a634 	.word	0x3c00a634
    ba80:	00018c07 	.word	0x00018c07

0000ba84 <fnd_vs_read>:
    ba84:	e3a01001 	mov	r1, #1	; 0x1
    ba88:	e58d1004 	str	r1, [sp, #4]
    ba8c:	eaffff46 	b	b7ac <base_vs_read>

0000ba90 <nnd_vs_read>:
    ba90:	e3a01000 	mov	r1, #0	; 0x0
    ba94:	e58d1004 	str	r1, [sp, #4]
    ba98:	eaffff43 	b	b7ac <base_vs_read>

0000ba9c <base_get_badmark>:
    ba9c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    baa0:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    baa4:	e58d0064 	str	r0, [sp, #100]
    baa8:	e3a05000 	mov	r5, #0	; 0x0
    baac:	e3a00001 	mov	r0, #1	; 0x1
    bab0:	e58d5074 	str	r5, [sp, #116]
    bab4:	e58d5070 	str	r5, [sp, #112]
    bab8:	ebfffd12 	bl	af08 <nand_get_config>
    babc:	e2504000 	subs	r4, r0, #0	; 0x0
    bac0:	0a000008 	beq	bae8 <base_get_badmark+0x4c>
    bac4:	e5940010 	ldr	r0, [r4, #16]
    bac8:	e2800004 	add	r0, r0, #4	; 0x4
    bacc:	ebffd6c9 	bl	15f8 <malloc>
    bad0:	e2506000 	subs	r6, r0, #0	; 0x0
    bad4:	1a000005 	bne	baf0 <base_get_badmark+0x54>
    bad8:	e59f01b0 	ldr	r0, [pc, #432]	; bc90 <base_get_badmark+0x1f4>
    badc:	e59f31b0 	ldr	r3, [pc, #432]	; bc94 <base_get_badmark+0x1f8>
    bae0:	e1a0e00f 	mov	lr, pc
    bae4:	e593f000 	ldr	pc, [r3]
    bae8:	e3e05000 	mvn	r5, #0	; 0x0
    baec:	ea000064 	b	bc84 <base_get_badmark+0x1e8>
    baf0:	e5941010 	ldr	r1, [r4, #16]
    baf4:	e5d42003 	ldrb	r2, [r4, #3]
    baf8:	e5943014 	ldr	r3, [r4, #20]
    bafc:	e58d1014 	str	r1, [sp, #20]
    bb00:	e58d1004 	str	r1, [sp, #4]
    bb04:	e59d1064 	ldr	r1, [sp, #100]
    bb08:	e3a07001 	mov	r7, #1	; 0x1
    bb0c:	e58d5008 	str	r5, [sp, #8]
    bb10:	e58d100c 	str	r1, [sp, #12]
    bb14:	e58d5018 	str	r5, [sp, #24]
    bb18:	e58d5020 	str	r5, [sp, #32]
    bb1c:	e58d5024 	str	r5, [sp, #36]
    bb20:	e58d3000 	str	r3, [sp]
    bb24:	e58d6010 	str	r6, [sp, #16]
    bb28:	e58d701c 	str	r7, [sp, #28]
    bb2c:	e5d43002 	ldrb	r3, [r4, #2]
    bb30:	e2841038 	add	r1, r4, #56	; 0x38
    bb34:	e58d1068 	str	r1, [sp, #104]
    bb38:	e58d3028 	str	r3, [sp, #40]
    bb3c:	e59d3068 	ldr	r3, [sp, #104]
    bb40:	e284104c 	add	r1, r4, #76	; 0x4c
    bb44:	e58d106c 	str	r1, [sp, #108]
    bb48:	e58d3030 	str	r3, [sp, #48]
    bb4c:	e58d1034 	str	r1, [sp, #52]
    bb50:	e28d3070 	add	r3, sp, #112	; 0x70
    bb54:	e28d1074 	add	r1, sp, #116	; 0x74
    bb58:	e284b05c 	add	fp, r4, #92	; 0x5c
    bb5c:	e284906c 	add	r9, r4, #108	; 0x6c
    bb60:	e284a07c 	add	sl, r4, #124	; 0x7c
    bb64:	e284808c 	add	r8, r4, #140	; 0x8c
    bb68:	e58d502c 	str	r5, [sp, #44]
    bb6c:	e58d1048 	str	r1, [sp, #72]
    bb70:	e58d304c 	str	r3, [sp, #76]
    bb74:	e58d5050 	str	r5, [sp, #80]
    bb78:	e58d5054 	str	r5, [sp, #84]
    bb7c:	e58d5058 	str	r5, [sp, #88]
    bb80:	e58db038 	str	fp, [sp, #56]
    bb84:	e58d903c 	str	r9, [sp, #60]
    bb88:	e58da040 	str	sl, [sp, #64]
    bb8c:	e58d8044 	str	r8, [sp, #68]
    bb90:	e5d4c006 	ldrb	ip, [r4, #6]
    bb94:	e1a00005 	mov	r0, r5
    bb98:	e1a01007 	mov	r1, r7
    bb9c:	e1a03007 	mov	r3, r7
    bba0:	e58dc05c 	str	ip, [sp, #92]
    bba4:	eb0006b1 	bl	d670 <nf2_page_op>
    bba8:	e2505000 	subs	r5, r0, #0	; 0x0
    bbac:	1a000032 	bne	bc7c <base_get_badmark+0x1e0>
    bbb0:	e5d63000 	ldrb	r3, [r6]
    bbb4:	e35300ff 	cmp	r3, #255	; 0xff
    bbb8:	0a00002f 	beq	bc7c <base_get_badmark+0x1e0>
    bbbc:	e5943014 	ldr	r3, [r4, #20]
    bbc0:	e5d40005 	ldrb	r0, [r4, #5]
    bbc4:	e5941010 	ldr	r1, [r4, #16]
    bbc8:	e5d42003 	ldrb	r2, [r4, #3]
    bbcc:	e58d3000 	str	r3, [sp]
    bbd0:	e59d3064 	ldr	r3, [sp, #100]
    bbd4:	e58d1014 	str	r1, [sp, #20]
    bbd8:	e98d0022 	stmib	sp, {r1, r5}
    bbdc:	e58d0018 	str	r0, [sp, #24]
    bbe0:	e58d300c 	str	r3, [sp, #12]
    bbe4:	e58d6010 	str	r6, [sp, #16]
    bbe8:	e5d43004 	ldrb	r3, [r4, #4]
    bbec:	e58d5020 	str	r5, [sp, #32]
    bbf0:	e58d5024 	str	r5, [sp, #36]
    bbf4:	e58d301c 	str	r3, [sp, #28]
    bbf8:	e5d43002 	ldrb	r3, [r4, #2]
    bbfc:	e1a01007 	mov	r1, r7
    bc00:	e58d3028 	str	r3, [sp, #40]
    bc04:	e5d43001 	ldrb	r3, [r4, #1]
    bc08:	e58d5058 	str	r5, [sp, #88]
    bc0c:	e58d302c 	str	r3, [sp, #44]
    bc10:	e59d3068 	ldr	r3, [sp, #104]
    bc14:	e58d5050 	str	r5, [sp, #80]
    bc18:	e58d3030 	str	r3, [sp, #48]
    bc1c:	e59d306c 	ldr	r3, [sp, #108]
    bc20:	e58d5054 	str	r5, [sp, #84]
    bc24:	e58d3034 	str	r3, [sp, #52]
    bc28:	e28d3074 	add	r3, sp, #116	; 0x74
    bc2c:	e58d3048 	str	r3, [sp, #72]
    bc30:	e28d3070 	add	r3, sp, #112	; 0x70
    bc34:	e58d304c 	str	r3, [sp, #76]
    bc38:	e58db038 	str	fp, [sp, #56]
    bc3c:	e58d903c 	str	r9, [sp, #60]
    bc40:	e58da040 	str	sl, [sp, #64]
    bc44:	e58d8044 	str	r8, [sp, #68]
    bc48:	e5d4c006 	ldrb	ip, [r4, #6]
    bc4c:	e1a03007 	mov	r3, r7
    bc50:	e58dc05c 	str	ip, [sp, #92]
    bc54:	eb000685 	bl	d670 <nf2_page_op>
    bc58:	e2505000 	subs	r5, r0, #0	; 0x0
    bc5c:	1a000006 	bne	bc7c <base_get_badmark+0x1e0>
    bc60:	e59d3070 	ldr	r3, [sp, #112]
    bc64:	e3530000 	cmp	r3, #0	; 0x0
    bc68:	1a000002 	bne	bc78 <base_get_badmark+0x1dc>
    bc6c:	e5d63000 	ldrb	r3, [r6]
    bc70:	e35300ff 	cmp	r3, #255	; 0xff
    bc74:	0a000000 	beq	bc7c <base_get_badmark+0x1e0>
    bc78:	e3a05001 	mov	r5, #1	; 0x1
    bc7c:	e1a00006 	mov	r0, r6
    bc80:	ebffd6c4 	bl	1798 <free>
    bc84:	e1a00005 	mov	r0, r5
    bc88:	e28dd07c 	add	sp, sp, #124	; 0x7c
    bc8c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    bc90:	00018c1b 	.word	0x00018c1b
    bc94:	3c00a5cc 	.word	0x3c00a5cc

0000bc98 <nnd_vs_isbad>:
    bc98:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    bc9c:	e1a08000 	mov	r8, r0
    bca0:	e3a00001 	mov	r0, #1	; 0x1
    bca4:	e1a09001 	mov	r9, r1
    bca8:	ebfffc96 	bl	af08 <nand_get_config>
    bcac:	e2506000 	subs	r6, r0, #0	; 0x0
    bcb0:	0a00001f 	beq	bd34 <nnd_vs_isbad+0x9c>
    bcb4:	e596301c 	ldr	r3, [r6, #28]
    bcb8:	e1a01009 	mov	r1, r9
    bcbc:	e2433001 	sub	r3, r3, #1	; 0x1
    bcc0:	e1c84003 	bic	r4, r8, r3
    bcc4:	e59f307c 	ldr	r3, [pc, #124]	; bd48 <nnd_vs_isbad+0xb0>
    bcc8:	e1a00004 	mov	r0, r4
    bccc:	e5932010 	ldr	r2, [r3, #16]
    bcd0:	eb002542 	bl	151e0 <__aeabi_lasr>
    bcd4:	e1a08000 	mov	r8, r0
    bcd8:	e5960008 	ldr	r0, [r6, #8]
    bcdc:	e1a05009 	mov	r5, r9
    bce0:	e0880000 	add	r0, r8, r0
    bce4:	ebffff6c 	bl	ba9c <base_get_badmark>
    bce8:	e2502000 	subs	r2, r0, #0	; 0x0
    bcec:	ba00000a 	blt	bd1c <nnd_vs_isbad+0x84>
    bcf0:	13a00001 	movne	r0, #1	; 0x1
    bcf4:	18bd8370 	popne	{r4, r5, r6, r8, r9, pc}
    bcf8:	e596000c 	ldr	r0, [r6, #12]
    bcfc:	e5963008 	ldr	r3, [r6, #8]
    bd00:	e1500003 	cmp	r0, r3
    bd04:	01a00002 	moveq	r0, r2
    bd08:	08bd8370 	popeq	{r4, r5, r6, r8, r9, pc}
    bd0c:	e0880000 	add	r0, r8, r0
    bd10:	ebffff61 	bl	ba9c <base_get_badmark>
    bd14:	e3500000 	cmp	r0, #0	; 0x0
    bd18:	aa000007 	bge	bd3c <nnd_vs_isbad+0xa4>
    bd1c:	e1a02004 	mov	r2, r4
    bd20:	e1a03005 	mov	r3, r5
    bd24:	e59f0020 	ldr	r0, [pc, #32]	; bd4c <nnd_vs_isbad+0xb4>
    bd28:	e59f1020 	ldr	r1, [pc, #32]	; bd50 <nnd_vs_isbad+0xb8>
    bd2c:	e1a0e00f 	mov	lr, pc
    bd30:	e591f000 	ldr	pc, [r1]
    bd34:	e3e00000 	mvn	r0, #0	; 0x0
    bd38:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    bd3c:	03a00000 	moveq	r0, #0	; 0x0
    bd40:	13a00001 	movne	r0, #1	; 0x1
    bd44:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    bd48:	3c00a634 	.word	0x3c00a634
    bd4c:	00018c42 	.word	0x00018c42
    bd50:	3c00a5cc 	.word	0x3c00a5cc

0000bd54 <base_scan_badblocks>:
    bd54:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    bd58:	e1a0b000 	mov	fp, r0
    bd5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    bd60:	e3a00001 	mov	r0, #1	; 0x1
    bd64:	ebfffc67 	bl	af08 <nand_get_config>
    bd68:	e250a000 	subs	sl, r0, #0	; 0x0
    bd6c:	0a000005 	beq	bd88 <base_scan_badblocks+0x34>
    bd70:	e3a07000 	mov	r7, #0	; 0x0
    bd74:	e1a08007 	mov	r8, r7
    bd78:	e3a04000 	mov	r4, #0	; 0x0
    bd7c:	e3a05000 	mov	r5, #0	; 0x0
    bd80:	e59f90b8 	ldr	r9, [pc, #184]	; be40 <base_scan_badblocks+0xec>
    bd84:	ea00001a 	b	bdf4 <base_scan_badblocks+0xa0>
    bd88:	e59f00b4 	ldr	r0, [pc, #180]	; be44 <base_scan_badblocks+0xf0>
    bd8c:	e59f30ac 	ldr	r3, [pc, #172]	; be40 <base_scan_badblocks+0xec>
    bd90:	e1a0e00f 	mov	lr, pc
    bd94:	e593f000 	ldr	pc, [r3]
    bd98:	e3e00000 	mvn	r0, #0	; 0x0
    bd9c:	ea000025 	b	be38 <base_scan_badblocks+0xe4>
    bda0:	ebffffbc 	bl	bc98 <nnd_vs_isbad>
    bda4:	e3500000 	cmp	r0, #0	; 0x0
    bda8:	0a00000d 	beq	bde4 <base_scan_badblocks+0x90>
    bdac:	aa000004 	bge	bdc4 <base_scan_badblocks+0x70>
    bdb0:	e59f0090 	ldr	r0, [pc, #144]	; be48 <base_scan_badblocks+0xf4>
    bdb4:	e1a01007 	mov	r1, r7
    bdb8:	e1a0e00f 	mov	lr, pc
    bdbc:	e599f000 	ldr	pc, [r9]
    bdc0:	ea000006 	b	bde0 <base_scan_badblocks+0x8c>
    bdc4:	e35b0000 	cmp	fp, #0	; 0x0
    bdc8:	0a000004 	beq	bde0 <base_scan_badblocks+0x8c>
    bdcc:	e1a02004 	mov	r2, r4
    bdd0:	e1a03005 	mov	r3, r5
    bdd4:	e59f0070 	ldr	r0, [pc, #112]	; be4c <base_scan_badblocks+0xf8>
    bdd8:	e1a0e00f 	mov	lr, pc
    bddc:	e599f000 	ldr	pc, [r9]
    bde0:	e2888001 	add	r8, r8, #1	; 0x1
    bde4:	e59a301c 	ldr	r3, [sl, #28]
    bde8:	e2877001 	add	r7, r7, #1	; 0x1
    bdec:	e0944003 	adds	r4, r4, r3
    bdf0:	e2a55000 	adc	r5, r5, #0	; 0x0
    bdf4:	e59a6024 	ldr	r6, [sl, #36]
    bdf8:	e1a00004 	mov	r0, r4
    bdfc:	e1570006 	cmp	r7, r6
    be00:	e1a01005 	mov	r1, r5
    be04:	3affffe5 	bcc	bda0 <base_scan_badblocks+0x4c>
    be08:	e3a00064 	mov	r0, #100	; 0x64
    be0c:	e1a01006 	mov	r1, r6
    be10:	e0000890 	mul	r0, r0, r8
    be14:	eb0023da 	bl	14d84 <__aeabi_uidiv>
    be18:	e1a01008 	mov	r1, r8
    be1c:	e1a03000 	mov	r3, r0
    be20:	e1a02006 	mov	r2, r6
    be24:	e59f0024 	ldr	r0, [pc, #36]	; be50 <base_scan_badblocks+0xfc>
    be28:	e59fc010 	ldr	ip, [pc, #16]	; be40 <base_scan_badblocks+0xec>
    be2c:	e1a0e00f 	mov	lr, pc
    be30:	e59cf000 	ldr	pc, [ip]
    be34:	e3a00000 	mov	r0, #0	; 0x0
    be38:	e28dd004 	add	sp, sp, #4	; 0x4
    be3c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    be40:	3c00a5cc 	.word	0x3c00a5cc
    be44:	00018c60 	.word	0x00018c60
    be48:	00018c7c 	.word	0x00018c7c
    be4c:	00016dce 	.word	0x00016dce
    be50:	00018c96 	.word	0x00018c96

0000be54 <fnd_vs_isbad>:
    be54:	eaffff8f 	b	bc98 <nnd_vs_isbad>

0000be58 <bnd_vs_isbad>:
    be58:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    be5c:	e59f3014 	ldr	r3, [pc, #20]	; be78 <bnd_vs_isbad+0x20>
    be60:	e24dd004 	sub	sp, sp, #4	; 0x4
    be64:	e5932008 	ldr	r2, [r3, #8]
    be68:	eb0024e3 	bl	151fc <__aeabi_llsl>
    be6c:	e28dd004 	add	sp, sp, #4	; 0x4
    be70:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    be74:	eaffff87 	b	bc98 <nnd_vs_isbad>
    be78:	3c00a634 	.word	0x3c00a634

0000be7c <nnd_vs_reset>:
    be7c:	e92d4030 	push	{r4, r5, lr}
    be80:	e3a00001 	mov	r0, #1	; 0x1
    be84:	e24dd004 	sub	sp, sp, #4	; 0x4
    be88:	ebfffc1e 	bl	af08 <nand_get_config>
    be8c:	e2504000 	subs	r4, r0, #0	; 0x0
    be90:	0a000014 	beq	bee8 <nnd_vs_reset+0x6c>
    be94:	e3a01001 	mov	r1, #1	; 0x1
    be98:	eb000952 	bl	e3e8 <nf2_hardware_reset>
    be9c:	e3500000 	cmp	r0, #0	; 0x0
    bea0:	0a000011 	beq	beec <nnd_vs_reset+0x70>
    bea4:	e5d41000 	ldrb	r1, [r4]
    bea8:	e59f5044 	ldr	r5, [pc, #68]	; bef4 <nnd_vs_reset+0x78>
    beac:	e3510001 	cmp	r1, #1	; 0x1
    beb0:	1a000009 	bne	bedc <nnd_vs_reset+0x60>
    beb4:	e59f003c 	ldr	r0, [pc, #60]	; bef8 <nnd_vs_reset+0x7c>
    beb8:	e1a0e00f 	mov	lr, pc
    bebc:	e595f000 	ldr	pc, [r5]
    bec0:	e3a01000 	mov	r1, #0	; 0x0
    bec4:	e5c41000 	strb	r1, [r4]
    bec8:	e1a00004 	mov	r0, r4
    becc:	eb000945 	bl	e3e8 <nf2_hardware_reset>
    bed0:	e3500000 	cmp	r0, #0	; 0x0
    bed4:	0a000004 	beq	beec <nnd_vs_reset+0x70>
    bed8:	e5d41000 	ldrb	r1, [r4]
    bedc:	e59f0018 	ldr	r0, [pc, #24]	; befc <nnd_vs_reset+0x80>
    bee0:	e1a0e00f 	mov	lr, pc
    bee4:	e595f000 	ldr	pc, [r5]
    bee8:	e3e00003 	mvn	r0, #3	; 0x3
    beec:	e28dd004 	add	sp, sp, #4	; 0x4
    bef0:	e8bd8030 	pop	{r4, r5, pc}
    bef4:	3c00a5cc 	.word	0x3c00a5cc
    bef8:	00018cd7 	.word	0x00018cd7
    befc:	00018cb5 	.word	0x00018cb5

0000bf00 <fnd_vs_reset>:
    bf00:	eaffffdd 	b	be7c <nnd_vs_reset>

0000bf04 <base_vs_erase>:
    bf04:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    bf08:	e1a04000 	mov	r4, r0
    bf0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    bf10:	e3a00001 	mov	r0, #1	; 0x1
    bf14:	e1a05001 	mov	r5, r1
    bf18:	e1a06002 	mov	r6, r2
    bf1c:	e58d3004 	str	r3, [sp, #4]
    bf20:	ebfffbf8 	bl	af08 <nand_get_config>
    bf24:	e2507000 	subs	r7, r0, #0	; 0x0
    bf28:	0a000047 	beq	c04c <base_vs_erase+0x148>
    bf2c:	e597201c 	ldr	r2, [r7, #28]
    bf30:	e242c001 	sub	ip, r2, #1	; 0x1
    bf34:	e00c3004 	and	r3, ip, r4
    bf38:	e3530000 	cmp	r3, #0	; 0x0
    bf3c:	e0836006 	add	r6, r3, r6
    bf40:	1a000001 	bne	bf4c <base_vs_erase+0x48>
    bf44:	e116000c 	tst	r6, ip
    bf48:	0a00000a 	beq	bf78 <base_vs_erase+0x74>
    bf4c:	e08c3006 	add	r3, ip, r6
    bf50:	e2622000 	rsb	r2, r2, #0	; 0x0
    bf54:	e0036002 	and	r6, r3, r2
    bf58:	e1c4400c 	bic	r4, r4, ip
    bf5c:	e59f0108 	ldr	r0, [pc, #264]	; c06c <base_vs_erase+0x168>
    bf60:	e1a01006 	mov	r1, r6
    bf64:	e1a02004 	mov	r2, r4
    bf68:	e1a03005 	mov	r3, r5
    bf6c:	e59fc0fc 	ldr	ip, [pc, #252]	; c070 <base_vs_erase+0x16c>
    bf70:	e1a0e00f 	mov	lr, pc
    bf74:	e59cf000 	ldr	pc, [ip]
    bf78:	e59f30f4 	ldr	r3, [pc, #244]	; c074 <base_vs_erase+0x170>
    bf7c:	e1a00004 	mov	r0, r4
    bf80:	e5932010 	ldr	r2, [r3, #16]
    bf84:	e1a01005 	mov	r1, r5
    bf88:	eb002494 	bl	151e0 <__aeabi_lasr>
    bf8c:	e597101c 	ldr	r1, [r7, #28]
    bf90:	e1a08000 	mov	r8, r0
    bf94:	e1a00006 	mov	r0, r6
    bf98:	eb002379 	bl	14d84 <__aeabi_uidiv>
    bf9c:	e3a09000 	mov	r9, #0	; 0x0
    bfa0:	e1a0b000 	mov	fp, r0
    bfa4:	e1a06009 	mov	r6, r9
    bfa8:	ea000022 	b	c038 <base_vs_erase+0x134>
    bfac:	e59d3004 	ldr	r3, [sp, #4]
    bfb0:	e3530000 	cmp	r3, #0	; 0x0
    bfb4:	1a000002 	bne	bfc4 <base_vs_erase+0xc0>
    bfb8:	ebffff36 	bl	bc98 <nnd_vs_isbad>
    bfbc:	e3500000 	cmp	r0, #0	; 0x0
    bfc0:	1a000010 	bne	c008 <base_vs_erase+0x104>
    bfc4:	e1a01008 	mov	r1, r8
    bfc8:	e5d72002 	ldrb	r2, [r7, #2]
    bfcc:	e3a00000 	mov	r0, #0	; 0x0
    bfd0:	eb000563 	bl	d564 <nf2_erase>
    bfd4:	e250a000 	subs	sl, r0, #0	; 0x0
    bfd8:	0a000010 	beq	c020 <base_vs_erase+0x11c>
    bfdc:	e5971020 	ldr	r1, [r7, #32]
    bfe0:	e1a00008 	mov	r0, r8
    bfe4:	eb002366 	bl	14d84 <__aeabi_uidiv>
    bfe8:	e1a0200a 	mov	r2, sl
    bfec:	e1a01000 	mov	r1, r0
    bff0:	e59f3078 	ldr	r3, [pc, #120]	; c070 <base_vs_erase+0x16c>
    bff4:	e59f007c 	ldr	r0, [pc, #124]	; c078 <base_vs_erase+0x174>
    bff8:	e1a0e00f 	mov	lr, pc
    bffc:	e593f000 	ldr	pc, [r3]
    c000:	e3e06000 	mvn	r6, #0	; 0x0
    c004:	ea000005 	b	c020 <base_vs_erase+0x11c>
    c008:	e5971014 	ldr	r1, [r7, #20]
    c00c:	e59f0068 	ldr	r0, [pc, #104]	; c07c <base_vs_erase+0x178>
    c010:	e0010891 	mul	r1, r1, r8
    c014:	e59f3054 	ldr	r3, [pc, #84]	; c070 <base_vs_erase+0x16c>
    c018:	e1a0e00f 	mov	lr, pc
    c01c:	e593f000 	ldr	pc, [r3]
    c020:	e597201c 	ldr	r2, [r7, #28]
    c024:	e5973020 	ldr	r3, [r7, #32]
    c028:	e0944002 	adds	r4, r4, r2
    c02c:	e0888003 	add	r8, r8, r3
    c030:	e2a55000 	adc	r5, r5, #0	; 0x0
    c034:	e2899001 	add	r9, r9, #1	; 0x1
    c038:	e159000b 	cmp	r9, fp
    c03c:	e1a00004 	mov	r0, r4
    c040:	e1a01005 	mov	r1, r5
    c044:	baffffd8 	blt	bfac <base_vs_erase+0xa8>
    c048:	ea000004 	b	c060 <base_vs_erase+0x15c>
    c04c:	e59f002c 	ldr	r0, [pc, #44]	; c080 <base_vs_erase+0x17c>
    c050:	e59f3018 	ldr	r3, [pc, #24]	; c070 <base_vs_erase+0x16c>
    c054:	e1a0e00f 	mov	lr, pc
    c058:	e593f000 	ldr	pc, [r3]
    c05c:	e3e06000 	mvn	r6, #0	; 0x0
    c060:	e1a00006 	mov	r0, r6
    c064:	e28dd00c 	add	sp, sp, #12	; 0xc
    c068:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c06c:	00018cf7 	.word	0x00018cf7
    c070:	3c00a5cc 	.word	0x3c00a5cc
    c074:	3c00a634 	.word	0x3c00a634
    c078:	00018d1e 	.word	0x00018d1e
    c07c:	00018d3e 	.word	0x00018d3e
    c080:	00018d54 	.word	0x00018d54

0000c084 <nnd_vs_scrub>:
    c084:	e3a03001 	mov	r3, #1	; 0x1
    c088:	eaffff9d 	b	bf04 <base_vs_erase>

0000c08c <bnd_vs_erase>:
    c08c:	e92d4030 	push	{r4, r5, lr}
    c090:	e59f3020 	ldr	r3, [pc, #32]	; c0b8 <bnd_vs_erase+0x2c>
    c094:	e1a04002 	mov	r4, r2
    c098:	e5935008 	ldr	r5, [r3, #8]
    c09c:	e24dd004 	sub	sp, sp, #4	; 0x4
    c0a0:	e1a02005 	mov	r2, r5
    c0a4:	eb002454 	bl	151fc <__aeabi_llsl>
    c0a8:	e1a02514 	lsl	r2, r4, r5
    c0ac:	e28dd004 	add	sp, sp, #4	; 0x4
    c0b0:	e8bd4030 	pop	{r4, r5, lr}
    c0b4:	eafffff2 	b	c084 <nnd_vs_scrub>
    c0b8:	3c00a634 	.word	0x3c00a634

0000c0bc <nnd_vs_erase>:
    c0bc:	e3a03000 	mov	r3, #0	; 0x0
    c0c0:	eaffff8f 	b	bf04 <base_vs_erase>

0000c0c4 <base_block_markbad>:
    c0c4:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
    c0c8:	e1a08000 	mov	r8, r0
    c0cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    c0d0:	e3a00001 	mov	r0, #1	; 0x1
    c0d4:	e1a09001 	mov	r9, r1
    c0d8:	ebfffb8a 	bl	af08 <nand_get_config>
    c0dc:	e2504000 	subs	r4, r0, #0	; 0x0
    c0e0:	03e05000 	mvneq	r5, #0	; 0x0
    c0e4:	0a000029 	beq	c190 <base_block_markbad+0xcc>
    c0e8:	e594301c 	ldr	r3, [r4, #28]
    c0ec:	e59f00a8 	ldr	r0, [pc, #168]	; c19c <base_block_markbad+0xd8>
    c0f0:	e2433001 	sub	r3, r3, #1	; 0x1
    c0f4:	e1c86003 	bic	r6, r8, r3
    c0f8:	e1a02006 	mov	r2, r6
    c0fc:	e1a03009 	mov	r3, r9
    c100:	e59f5098 	ldr	r5, [pc, #152]	; c1a0 <base_block_markbad+0xdc>
    c104:	e1a0e00f 	mov	lr, pc
    c108:	e595f000 	ldr	pc, [r5]
    c10c:	e1a00006 	mov	r0, r6
    c110:	e1a01009 	mov	r1, r9
    c114:	e594201c 	ldr	r2, [r4, #28]
    c118:	ebffffe7 	bl	c0bc <nnd_vs_erase>
    c11c:	e3500000 	cmp	r0, #0	; 0x0
    c120:	e1a07009 	mov	r7, r9
    c124:	0a000002 	beq	c134 <base_block_markbad+0x70>
    c128:	e59f0074 	ldr	r0, [pc, #116]	; c1a4 <base_block_markbad+0xe0>
    c12c:	e1a0e00f 	mov	lr, pc
    c130:	e595f000 	ldr	pc, [r5]
    c134:	e5943014 	ldr	r3, [r4, #20]
    c138:	e5942008 	ldr	r2, [r4, #8]
    c13c:	e0020392 	mul	r2, r2, r3
    c140:	e0960002 	adds	r0, r6, r2
    c144:	e2a71000 	adc	r1, r7, #0	; 0x0
    c148:	ebfffbcd 	bl	b084 <__base_block_markbad>
    c14c:	e5943008 	ldr	r3, [r4, #8]
    c150:	e1a05000 	mov	r5, r0
    c154:	e594000c 	ldr	r0, [r4, #12]
    c158:	e1500003 	cmp	r0, r3
    c15c:	0a000005 	beq	c178 <base_block_markbad+0xb4>
    c160:	e5943014 	ldr	r3, [r4, #20]
    c164:	e0030093 	mul	r3, r3, r0
    c168:	e0960003 	adds	r0, r6, r3
    c16c:	e2a71000 	adc	r1, r7, #0	; 0x0
    c170:	ebfffbc3 	bl	b084 <__base_block_markbad>
    c174:	e1855000 	orr	r5, r5, r0
    c178:	e3550000 	cmp	r5, #0	; 0x0
    c17c:	0a000003 	beq	c190 <base_block_markbad+0xcc>
    c180:	e59f0020 	ldr	r0, [pc, #32]	; c1a8 <base_block_markbad+0xe4>
    c184:	e59f3014 	ldr	r3, [pc, #20]	; c1a0 <base_block_markbad+0xdc>
    c188:	e1a0e00f 	mov	lr, pc
    c18c:	e593f000 	ldr	pc, [r3]
    c190:	e1a00005 	mov	r0, r5
    c194:	e28dd004 	add	sp, sp, #4	; 0x4
    c198:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
    c19c:	00018d68 	.word	0x00018d68
    c1a0:	3c00a5cc 	.word	0x3c00a5cc
    c1a4:	00018d88 	.word	0x00018d88
    c1a8:	00018da0 	.word	0x00018da0

0000c1ac <fnd_vs_erase>:
    c1ac:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    c1b0:	e1a04000 	mov	r4, r0
    c1b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    c1b8:	e3a00001 	mov	r0, #1	; 0x1
    c1bc:	e1a06002 	mov	r6, r2
    c1c0:	e1a05001 	mov	r5, r1
    c1c4:	ebfffb4f 	bl	af08 <nand_get_config>
    c1c8:	e1a07000 	mov	r7, r0
    c1cc:	ebfffb7b 	bl	afc0 <fnd_align>
    c1d0:	e1a01000 	mov	r1, r0
    c1d4:	e1a00006 	mov	r0, r6
    c1d8:	eb0022e9 	bl	14d84 <__aeabi_uidiv>
    c1dc:	e5972014 	ldr	r2, [r7, #20]
    c1e0:	e1a01005 	mov	r1, r5
    c1e4:	e0020092 	mul	r2, r2, r0
    c1e8:	e1a00004 	mov	r0, r4
    c1ec:	e28dd004 	add	sp, sp, #4	; 0x4
    c1f0:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    c1f4:	eaffffb0 	b	c0bc <nnd_vs_erase>

0000c1f8 <nand_rescan_bootcfg>:
    c1f8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c1fc:	ebffd39c 	bl	1074 <ecfg_paras>
    c200:	e59f3198 	ldr	r3, [pc, #408]	; c3a0 <nand_rescan_bootcfg+0x1a8>
    c204:	e1a08000 	mov	r8, r0
    c208:	e1d300b0 	ldrh	r0, [r3]
    c20c:	e3500000 	cmp	r0, #0	; 0x0
    c210:	1a00000a 	bne	c240 <nand_rescan_bootcfg+0x48>
    c214:	ebffd17f 	bl	818 <boot_device>
    c218:	e3500000 	cmp	r0, #0	; 0x0
    c21c:	1a000001 	bne	c228 <nand_rescan_bootcfg+0x30>
    c220:	ebffd169 	bl	7cc <boot_info>
    c224:	ea000005 	b	c240 <nand_rescan_bootcfg+0x48>
    c228:	e59f0174 	ldr	r0, [pc, #372]	; c3a4 <nand_rescan_bootcfg+0x1ac>
    c22c:	e59f3174 	ldr	r3, [pc, #372]	; c3a8 <nand_rescan_bootcfg+0x1b0>
    c230:	e1a0e00f 	mov	lr, pc
    c234:	e593f000 	ldr	pc, [r3]
    c238:	e3e00000 	mvn	r0, #0	; 0x0
    c23c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c240:	e3100070 	tst	r0, #112	; 0x70
    c244:	e59f7160 	ldr	r7, [pc, #352]	; c3ac <nand_rescan_bootcfg+0x1b4>
    c248:	13a06010 	movne	r6, #16	; 0x10
    c24c:	03a06008 	moveq	r6, #8	; 0x8
    c250:	e1a01420 	lsr	r1, r0, #8
    c254:	e3100002 	tst	r0, #2	; 0x2
    c258:	e1a0c3a0 	lsr	ip, r0, #7
    c25c:	e3a04b01 	mov	r4, #1024	; 0x400
    c260:	e1a02120 	lsr	r2, r0, #2
    c264:	13a05005 	movne	r5, #5	; 0x5
    c268:	03a05004 	moveq	r5, #4	; 0x4
    c26c:	e200e001 	and	lr, r0, #1	; 0x1
    c270:	e20cc001 	and	ip, ip, #1	; 0x1
    c274:	e201100f 	and	r1, r1, #15	; 0xf
    c278:	e2022003 	and	r2, r2, #3	; 0x3
    c27c:	e3a03001 	mov	r3, #1	; 0x1
    c280:	e1a00004 	mov	r0, r4
    c284:	e5c7c019 	strb	ip, [r7, #25]
    c288:	e5c7e015 	strb	lr, [r7, #21]
    c28c:	e5c71017 	strb	r1, [r7, #23]
    c290:	e5873004 	str	r3, [r7, #4]
    c294:	e5c72014 	strb	r2, [r7, #20]
    c298:	e5c75016 	strb	r5, [r7, #22]
    c29c:	e5874028 	str	r4, [r7, #40]
    c2a0:	e5c7601a 	strb	r6, [r7, #26]
    c2a4:	ebfffaa6 	bl	ad44 <generic_ffs>
    c2a8:	e59730c4 	ldr	r3, [r7, #196]
    c2ac:	e2402001 	sub	r2, r0, #1	; 0x1
    c2b0:	e1a00523 	lsr	r0, r3, #10
    c2b4:	e587200c 	str	r2, [r7, #12]
    c2b8:	ebfffaa1 	bl	ad44 <generic_ffs>
    c2bc:	e2400001 	sub	r0, r0, #1	; 0x1
    c2c0:	e5870008 	str	r0, [r7, #8]
    c2c4:	e1a01006 	mov	r1, r6
    c2c8:	e59f00e0 	ldr	r0, [pc, #224]	; c3b0 <nand_rescan_bootcfg+0x1b8>
    c2cc:	e59f30d4 	ldr	r3, [pc, #212]	; c3a8 <nand_rescan_bootcfg+0x1b0>
    c2d0:	e1a0e00f 	mov	lr, pc
    c2d4:	e593f000 	ldr	pc, [r3]
    c2d8:	e3a00c02 	mov	r0, #512	; 0x200
    c2dc:	ebffd35d 	bl	1058 <ecfg_check_flag>
    c2e0:	e3500000 	cmp	r0, #0	; 0x0
    c2e4:	0a000008 	beq	c30c <nand_rescan_bootcfg+0x114>
    c2e8:	e30c30c0 	movw	r3, #49344	; 0xc0c0
    c2ec:	e5873048 	str	r3, [r7, #72]
    c2f0:	e3e034ae 	mvn	r3, #-1375731712	; 0xae000000
    c2f4:	e587303c 	str	r3, [r7, #60]
    c2f8:	e59f30b4 	ldr	r3, [pc, #180]	; c3b4 <nand_rescan_bootcfg+0x1bc>
    c2fc:	e5873040 	str	r3, [r7, #64]
    c300:	e3043023 	movw	r3, #16419	; 0x4023
    c304:	e5873044 	str	r3, [r7, #68]
    c308:	ea000008 	b	c330 <nand_rescan_bootcfg+0x138>
    c30c:	e5d83007 	ldrb	r3, [r8, #7]
    c310:	e1d810ba 	ldrh	r1, [r8, #10]
    c314:	e1a03c03 	lsl	r3, r3, #24
    c318:	e1e03c23 	mvn	r3, r3, lsr #24
    c31c:	e1d820bc 	ldrh	r2, [r8, #12]
    c320:	e1e03c03 	mvn	r3, r3, lsl #24
    c324:	e5872048 	str	r2, [r7, #72]
    c328:	e587303c 	str	r3, [r7, #60]
    c32c:	e5871044 	str	r1, [r7, #68]
    c330:	e3a00c01 	mov	r0, #256	; 0x100
    c334:	ebffd347 	bl	1058 <ecfg_check_flag>
    c338:	e3500000 	cmp	r0, #0	; 0x0
    c33c:	e59f1068 	ldr	r1, [pc, #104]	; c3ac <nand_rescan_bootcfg+0x1b4>
    c340:	0a00000b 	beq	c374 <nand_rescan_bootcfg+0x17c>
    c344:	e3003994 	movw	r3, #2452	; 0x994
    c348:	e581305c 	str	r3, [r1, #92]
    c34c:	e59f3064 	ldr	r3, [pc, #100]	; c3b8 <nand_rescan_bootcfg+0x1c0>
    c350:	e3a00000 	mov	r0, #0	; 0x0
    c354:	e581304c 	str	r3, [r1, #76]
    c358:	e59f305c 	ldr	r3, [pc, #92]	; c3bc <nand_rescan_bootcfg+0x1c4>
    c35c:	e5813050 	str	r3, [r1, #80]
    c360:	e59f3058 	ldr	r3, [pc, #88]	; c3c0 <nand_rescan_bootcfg+0x1c8>
    c364:	e5813054 	str	r3, [r1, #84]
    c368:	e59f3054 	ldr	r3, [pc, #84]	; c3c4 <nand_rescan_bootcfg+0x1cc>
    c36c:	e5813058 	str	r3, [r1, #88]
    c370:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c374:	e5983010 	ldr	r3, [r8, #16]
    c378:	e1d820be 	ldrh	r2, [r8, #14]
    c37c:	e581304c 	str	r3, [r1, #76]
    c380:	e5983014 	ldr	r3, [r8, #20]
    c384:	e5813050 	str	r3, [r1, #80]
    c388:	e5983018 	ldr	r3, [r8, #24]
    c38c:	e5813054 	str	r3, [r1, #84]
    c390:	e598301c 	ldr	r3, [r8, #28]
    c394:	e581205c 	str	r2, [r1, #92]
    c398:	e5813058 	str	r3, [r1, #88]
    c39c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c3a0:	3c00a0ec 	.word	0x3c00a0ec
    c3a4:	00018dc7 	.word	0x00018dc7
    c3a8:	3c00a5cc 	.word	0x3c00a5cc
    c3ac:	3c00a634 	.word	0x3c00a634
    c3b0:	00018de5 	.word	0x00018de5
    c3b4:	00fffaf0 	.word	0x00fffaf0
    c3b8:	1c723550 	.word	0x1c723550
    c3bc:	20fe074e 	.word	0x20fe074e
    c3c0:	1698347e 	.word	0x1698347e
    c3c4:	3fff2c80 	.word	0x3fff2c80

0000c3c8 <nand_init_randomizer>:
    c3c8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c3cc:	e5d03001 	ldrb	r3, [r0, #1]
    c3d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    c3d4:	e3530000 	cmp	r3, #0	; 0x0
    c3d8:	e1a05000 	mov	r5, r0
    c3dc:	0a000036 	beq	c4bc <nand_init_randomizer+0xf4>
    c3e0:	e5901014 	ldr	r1, [r0, #20]
    c3e4:	e3a02000 	mov	r2, #0	; 0x0
    c3e8:	e5d00003 	ldrb	r0, [r0, #3]
    c3ec:	eb0002ef 	bl	cfb0 <nf2_ecc_num>
    c3f0:	e1a04000 	mov	r4, r0
    c3f4:	e1a01004 	mov	r1, r4
    c3f8:	e59f00c8 	ldr	r0, [pc, #200]	; c4c8 <nand_init_randomizer+0x100>
    c3fc:	e59f30c8 	ldr	r3, [pc, #200]	; c4cc <nand_init_randomizer+0x104>
    c400:	e1a0e00f 	mov	lr, pc
    c404:	e593f000 	ldr	pc, [r3]
    c408:	e5d53006 	ldrb	r3, [r5, #6]
    c40c:	e5952014 	ldr	r2, [r5, #20]
    c410:	e3530010 	cmp	r3, #16	; 0x10
    c414:	e5953010 	ldr	r3, [r5, #16]
    c418:	e59f00b0 	ldr	r0, [pc, #176]	; c4d0 <nand_init_randomizer+0x108>
    c41c:	11a07002 	movne	r7, r2
    c420:	11a06003 	movne	r6, r3
    c424:	01a070a2 	lsreq	r7, r2, #1
    c428:	01a060a3 	lsreq	r6, r3, #1
    c42c:	01a040c4 	asreq	r4, r4, #1
    c430:	ebffd198 	bl	a98 <module_enable>
    c434:	e5950048 	ldr	r0, [r5, #72]
    c438:	eb00038c 	bl	d270 <nf2_set_polynomial>
    c43c:	e247b001 	sub	fp, r7, #1	; 0x1
    c440:	e2869001 	add	r9, r6, #1	; 0x1
    c444:	e244a001 	sub	sl, r4, #1	; 0x1
    c448:	e2858038 	add	r8, r5, #56	; 0x38
    c44c:	e1a04005 	mov	r4, r5
    c450:	e3a07000 	mov	r7, #0	; 0x0
    c454:	e2866003 	add	r6, r6, #3	; 0x3
    c458:	e19800b7 	ldrh	r0, [r8, r7]
    c45c:	e1a0100b 	mov	r1, fp
    c460:	e1a00880 	lsl	r0, r0, #17
    c464:	e5952048 	ldr	r2, [r5, #72]
    c468:	e1a008a0 	lsr	r0, r0, #17
    c46c:	eb0006a9 	bl	df18 <nf2_randc_seed_hw>
    c470:	e5d53006 	ldrb	r3, [r5, #6]
    c474:	e6ff2070 	uxth	r2, r0
    c478:	e3530010 	cmp	r3, #16	; 0x10
    c47c:	e1a01006 	mov	r1, r6
    c480:	e1c424bc 	strh	r2, [r4, #76]
    c484:	01a01009 	moveq	r1, r9
    c488:	e1a00002 	mov	r0, r2
    c48c:	e5952048 	ldr	r2, [r5, #72]
    c490:	eb0006a0 	bl	df18 <nf2_randc_seed_hw>
    c494:	e1c405bc 	strh	r0, [r4, #92]
    c498:	e1d405bc 	ldrh	r0, [r4, #92]
    c49c:	e1a0100a 	mov	r1, sl
    c4a0:	e5952048 	ldr	r2, [r5, #72]
    c4a4:	eb00069b 	bl	df18 <nf2_randc_seed_hw>
    c4a8:	e2877002 	add	r7, r7, #2	; 0x2
    c4ac:	e3570010 	cmp	r7, #16	; 0x10
    c4b0:	e1c406bc 	strh	r0, [r4, #108]
    c4b4:	e2844002 	add	r4, r4, #2	; 0x2
    c4b8:	1affffe6 	bne	c458 <nand_init_randomizer+0x90>
    c4bc:	e3a00000 	mov	r0, #0	; 0x0
    c4c0:	e28dd004 	add	sp, sp, #4	; 0x4
    c4c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c4c8:	00018df3 	.word	0x00018df3
    c4cc:	3c00a5cc 	.word	0x3c00a5cc
    c4d0:	21e3c000 	.word	0x21e3c000

0000c4d4 <nand_rescan_config>:
    c4d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c4d8:	e24dd024 	sub	sp, sp, #36	; 0x24
    c4dc:	e1a04000 	mov	r4, r0
    c4e0:	e1a0a001 	mov	sl, r1
    c4e4:	ebffff43 	bl	c1f8 <nand_rescan_bootcfg>
    c4e8:	e3500000 	cmp	r0, #0	; 0x0
    c4ec:	1a000001 	bne	c4f8 <nand_rescan_config+0x24>
    c4f0:	e59f0290 	ldr	r0, [pc, #656]	; c788 <nand_rescan_config+0x2b4>
    c4f4:	ebffffb3 	bl	c3c8 <nand_init_randomizer>
    c4f8:	e3540000 	cmp	r4, #0	; 0x0
    c4fc:	0a00009e 	beq	c77c <nand_rescan_config+0x2a8>
    c500:	e1a07004 	mov	r7, r4
    c504:	e3a08000 	mov	r8, #0	; 0x0
    c508:	e28db018 	add	fp, sp, #24	; 0x18
    c50c:	e59f9278 	ldr	r9, [pc, #632]	; c78c <nand_rescan_config+0x2b8>
    c510:	ea000090 	b	c758 <nand_rescan_config+0x284>
    c514:	e5973030 	ldr	r3, [r7, #48]
    c518:	e5d71008 	ldrb	r1, [r7, #8]
    c51c:	e58d3000 	str	r3, [sp]
    c520:	e5973034 	ldr	r3, [r7, #52]
    c524:	e1a0000b 	mov	r0, fp
    c528:	e58d3004 	str	r3, [sp, #4]
    c52c:	e597304c 	ldr	r3, [r7, #76]
    c530:	e58d3008 	str	r3, [sp, #8]
    c534:	e5972028 	ldr	r2, [r7, #40]
    c538:	e597302c 	ldr	r3, [r7, #44]
    c53c:	eb000753 	bl	e290 <nand_readid>
    c540:	e5dd001b 	ldrb	r0, [sp, #27]
    c544:	e5dd401d 	ldrb	r4, [sp, #29]
    c548:	e5dd501e 	ldrb	r5, [sp, #30]
    c54c:	e5ddc01c 	ldrb	ip, [sp, #28]
    c550:	e5dd601f 	ldrb	r6, [sp, #31]
    c554:	e58d0000 	str	r0, [sp]
    c558:	e58d4008 	str	r4, [sp, #8]
    c55c:	e58d500c 	str	r5, [sp, #12]
    c560:	e5dd1018 	ldrb	r1, [sp, #24]
    c564:	e5dd2019 	ldrb	r2, [sp, #25]
    c568:	e5dd301a 	ldrb	r3, [sp, #26]
    c56c:	e58dc004 	str	ip, [sp, #4]
    c570:	e58d6010 	str	r6, [sp, #16]
    c574:	e59f0214 	ldr	r0, [pc, #532]	; c790 <nand_rescan_config+0x2bc>
    c578:	e1a0e00f 	mov	lr, pc
    c57c:	e599f000 	ldr	pc, [r9]
    c580:	e1a04007 	mov	r4, r7
    c584:	e3a05000 	mov	r5, #0	; 0x0
    c588:	e7d73005 	ldrb	r3, [r7, r5]
    c58c:	e3530000 	cmp	r3, #0	; 0x0
    c590:	0a000007 	beq	c5b4 <nand_rescan_config+0xe0>
    c594:	e7db2005 	ldrb	r2, [fp, r5]
    c598:	e1520003 	cmp	r2, r3
    c59c:	0a000004 	beq	c5b4 <nand_rescan_config+0xe0>
    c5a0:	e59f01ec 	ldr	r0, [pc, #492]	; c794 <nand_rescan_config+0x2c0>
    c5a4:	e1a01005 	mov	r1, r5
    c5a8:	e1a0e00f 	mov	lr, pc
    c5ac:	e599f000 	ldr	pc, [r9]
    c5b0:	ea000002 	b	c5c0 <nand_rescan_config+0xec>
    c5b4:	e2855001 	add	r5, r5, #1	; 0x1
    c5b8:	e3550008 	cmp	r5, #8	; 0x8
    c5bc:	1afffff1 	bne	c588 <nand_rescan_config+0xb4>
    c5c0:	e3550008 	cmp	r5, #8	; 0x8
    c5c4:	e2877050 	add	r7, r7, #80	; 0x50
    c5c8:	1a000061 	bne	c754 <nand_rescan_config+0x280>
    c5cc:	e59f01c4 	ldr	r0, [pc, #452]	; c798 <nand_rescan_config+0x2c4>
    c5d0:	e59f31b4 	ldr	r3, [pc, #436]	; c78c <nand_rescan_config+0x2b8>
    c5d4:	e1a0e00f 	mov	lr, pc
    c5d8:	e593f000 	ldr	pc, [r3]
    c5dc:	e59f31b8 	ldr	r3, [pc, #440]	; c79c <nand_rescan_config+0x2c8>
    c5e0:	e5d42008 	ldrb	r2, [r4, #8]
    c5e4:	e5c320b0 	strb	r2, [r3, #176]
    c5e8:	e5d42009 	ldrb	r2, [r4, #9]
    c5ec:	e5c320b1 	strb	r2, [r3, #177]
    c5f0:	e5d4200a 	ldrb	r2, [r4, #10]
    c5f4:	e5c320b2 	strb	r2, [r3, #178]
    c5f8:	e5d4200b 	ldrb	r2, [r4, #11]
    c5fc:	e5c320b3 	strb	r2, [r3, #179]
    c600:	e5d4200c 	ldrb	r2, [r4, #12]
    c604:	e5c320b4 	strb	r2, [r3, #180]
    c608:	e5d4200d 	ldrb	r2, [r4, #13]
    c60c:	e5c320b5 	strb	r2, [r3, #181]
    c610:	e5942010 	ldr	r2, [r4, #16]
    c614:	e1d410be 	ldrh	r1, [r4, #14]
    c618:	e58320b8 	str	r2, [r3, #184]
    c61c:	e5942014 	ldr	r2, [r4, #20]
    c620:	e58320bc 	str	r2, [r3, #188]
    c624:	e5942018 	ldr	r2, [r4, #24]
    c628:	e58320c0 	str	r2, [r3, #192]
    c62c:	e594201c 	ldr	r2, [r4, #28]
    c630:	e58320c4 	str	r2, [r3, #196]
    c634:	e5942020 	ldr	r2, [r4, #32]
    c638:	e58320c8 	str	r2, [r3, #200]
    c63c:	e5942024 	ldr	r2, [r4, #36]
    c640:	e58310d4 	str	r1, [r3, #212]
    c644:	e58320cc 	str	r2, [r3, #204]
    c648:	e5942028 	ldr	r2, [r4, #40]
    c64c:	e58320d8 	str	r2, [r3, #216]
    c650:	e594202c 	ldr	r2, [r4, #44]
    c654:	e58320dc 	str	r2, [r3, #220]
    c658:	e5942030 	ldr	r2, [r4, #48]
    c65c:	e58320e0 	str	r2, [r3, #224]
    c660:	e5942034 	ldr	r2, [r4, #52]
    c664:	e58320e4 	str	r2, [r3, #228]
    c668:	e5942038 	ldr	r2, [r4, #56]
    c66c:	e58320e8 	str	r2, [r3, #232]
    c670:	e594203c 	ldr	r2, [r4, #60]
    c674:	e58320ec 	str	r2, [r3, #236]
    c678:	e5942040 	ldr	r2, [r4, #64]
    c67c:	e58320f0 	str	r2, [r3, #240]
    c680:	e5942044 	ldr	r2, [r4, #68]
    c684:	e58320f4 	str	r2, [r3, #244]
    c688:	e5942048 	ldr	r2, [r4, #72]
    c68c:	e58320f8 	str	r2, [r3, #248]
    c690:	e594104c 	ldr	r1, [r4, #76]
    c694:	e3510000 	cmp	r1, #0	; 0x0
    c698:	01a01005 	moveq	r1, r5
    c69c:	e59f50f8 	ldr	r5, [pc, #248]	; c79c <nand_rescan_config+0x2c8>
    c6a0:	16ef1071 	uxtbne	r1, r1
    c6a4:	e5c510b6 	strb	r1, [r5, #182]
    c6a8:	e594101c 	ldr	r1, [r4, #28]
    c6ac:	e5940024 	ldr	r0, [r4, #36]
    c6b0:	eb0021b3 	bl	14d84 <__aeabi_uidiv>
    c6b4:	e3a03001 	mov	r3, #1	; 0x1
    c6b8:	e58500d0 	str	r0, [r5, #208]
    c6bc:	e1a00005 	mov	r0, r5
    c6c0:	e48030b0 	str	r3, [r0], #176
    c6c4:	ebfff9b4 	bl	ad9c <print_nand_cfg>
    c6c8:	e59500c4 	ldr	r0, [r5, #196]
    c6cc:	ebfff99c 	bl	ad44 <generic_ffs>
    c6d0:	e2400001 	sub	r0, r0, #1	; 0x1
    c6d4:	e5850010 	str	r0, [r5, #16]
    c6d8:	ebffd04e 	bl	818 <boot_device>
    c6dc:	e3500000 	cmp	r0, #0	; 0x0
    c6e0:	1a000005 	bne	c6fc <nand_rescan_config+0x228>
    c6e4:	e5951028 	ldr	r1, [r5, #40]
    c6e8:	e59500c4 	ldr	r0, [r5, #196]
    c6ec:	eb0021a4 	bl	14d84 <__aeabi_uidiv>
    c6f0:	ebfff993 	bl	ad44 <generic_ffs>
    c6f4:	e2400001 	sub	r0, r0, #1	; 0x1
    c6f8:	e5850008 	str	r0, [r5, #8]
    c6fc:	e59f2098 	ldr	r2, [pc, #152]	; c79c <nand_rescan_config+0x2c8>
    c700:	e5d230b6 	ldrb	r3, [r2, #182]
    c704:	e3530008 	cmp	r3, #8	; 0x8
    c708:	1a000006 	bne	c728 <nand_rescan_config+0x254>
    c70c:	e5d230b0 	ldrb	r3, [r2, #176]
    c710:	e59f2088 	ldr	r2, [pc, #136]	; c7a0 <nand_rescan_config+0x2cc>
    c714:	e3530000 	cmp	r3, #0	; 0x0
    c718:	03003f83 	movweq	r3, #3971	; 0xf83
    c71c:	13003f8b 	movwne	r3, #3979	; 0xf8b
    c720:	e1c230b0 	strh	r3, [r2]
    c724:	ea000002 	b	c734 <nand_rescan_config+0x260>
    c728:	e59f3070 	ldr	r3, [pc, #112]	; c7a0 <nand_rescan_config+0x2cc>
    c72c:	e3002f93 	movw	r2, #3987	; 0xf93
    c730:	e1c320b0 	strh	r2, [r3]
    c734:	e59f3060 	ldr	r3, [pc, #96]	; c79c <nand_rescan_config+0x2c8>
    c738:	e59320cc 	ldr	r2, [r3, #204]
    c73c:	e5931008 	ldr	r1, [r3, #8]
    c740:	e28300b0 	add	r0, r3, #176	; 0xb0
    c744:	e1a02132 	lsr	r2, r2, r1
    c748:	e5832030 	str	r2, [r3, #48]
    c74c:	ebffff1d 	bl	c3c8 <nand_init_randomizer>
    c750:	ea000009 	b	c77c <nand_rescan_config+0x2a8>
    c754:	e2888001 	add	r8, r8, #1	; 0x1
    c758:	e158000a 	cmp	r8, sl
    c75c:	baffff6c 	blt	c514 <nand_rescan_config+0x40>
    c760:	1a000005 	bne	c77c <nand_rescan_config+0x2a8>
    c764:	e59f0038 	ldr	r0, [pc, #56]	; c7a4 <nand_rescan_config+0x2d0>
    c768:	e59f301c 	ldr	r3, [pc, #28]	; c78c <nand_rescan_config+0x2b8>
    c76c:	e1a0e00f 	mov	lr, pc
    c770:	e593f000 	ldr	pc, [r3]
    c774:	e3e00000 	mvn	r0, #0	; 0x0
    c778:	ea000000 	b	c780 <nand_rescan_config+0x2ac>
    c77c:	e3a00000 	mov	r0, #0	; 0x0
    c780:	e28dd024 	add	sp, sp, #36	; 0x24
    c784:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c788:	3c00a648 	.word	0x3c00a648
    c78c:	3c00a5cc 	.word	0x3c00a5cc
    c790:	00018e07 	.word	0x00018e07
    c794:	00018e38 	.word	0x00018e38
    c798:	00018e54 	.word	0x00018e54
    c79c:	3c00a634 	.word	0x3c00a634
    c7a0:	3c00a0ec 	.word	0x3c00a0ec
    c7a4:	00018e71 	.word	0x00018e71

0000c7a8 <bnd_vs_reset>:
    c7a8:	e3a00000 	mov	r0, #0	; 0x0
    c7ac:	e92d4010 	push	{r4, lr}
    c7b0:	e1a01000 	mov	r1, r0
    c7b4:	ebffff46 	bl	c4d4 <nand_rescan_config>
    c7b8:	e3a00000 	mov	r0, #0	; 0x0
    c7bc:	ebfff9d1 	bl	af08 <nand_get_config>
    c7c0:	e1a04000 	mov	r4, r0
    c7c4:	e3a00001 	mov	r0, #1	; 0x1
    c7c8:	ebfff9ce 	bl	af08 <nand_get_config>
    c7cc:	e3540000 	cmp	r4, #0	; 0x0
    c7d0:	03e04000 	mvneq	r4, #0	; 0x0
    c7d4:	0a00000a 	beq	c804 <bnd_vs_reset+0x5c>
    c7d8:	e3500000 	cmp	r0, #0	; 0x0
    c7dc:	01a00004 	moveq	r0, r4
    c7e0:	e3a01000 	mov	r1, #0	; 0x0
    c7e4:	eb0006ff 	bl	e3e8 <nf2_hardware_reset>
    c7e8:	e2504000 	subs	r4, r0, #0	; 0x0
    c7ec:	0a000004 	beq	c804 <bnd_vs_reset+0x5c>
    c7f0:	e59f0014 	ldr	r0, [pc, #20]	; c80c <bnd_vs_reset+0x64>
    c7f4:	e1a01004 	mov	r1, r4
    c7f8:	e59f3010 	ldr	r3, [pc, #16]	; c810 <bnd_vs_reset+0x68>
    c7fc:	e1a0e00f 	mov	lr, pc
    c800:	e593f000 	ldr	pc, [r3]
    c804:	e1a00004 	mov	r0, r4
    c808:	e8bd8010 	pop	{r4, pc}
    c80c:	00018e8c 	.word	0x00018e8c
    c810:	3c00a5cc 	.word	0x3c00a5cc

0000c814 <ecc_add_stat>:
    c814:	e92d4070 	push	{r4, r5, r6, lr}
    c818:	e59f3134 	ldr	r3, [pc, #308]	; c954 <ecc_add_stat+0x140>
    c81c:	e24dd030 	sub	sp, sp, #48	; 0x30
    c820:	e28d4004 	add	r4, sp, #4	; 0x4
    c824:	e1a0c003 	mov	ip, r3
    c828:	e1a06000 	mov	r6, r0
    c82c:	e1a05001 	mov	r5, r1
    c830:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    c834:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    c838:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    c83c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    c840:	e59c1000 	ldr	r1, [ip]
    c844:	e59f210c 	ldr	r2, [pc, #268]	; c958 <ecc_add_stat+0x144>
    c848:	e28d3028 	add	r3, sp, #40	; 0x28
    c84c:	e3560000 	cmp	r6, #0	; 0x0
    c850:	e5841000 	str	r1, [r4]
    c854:	e8920003 	ldm	r2, {r0, r1}
    c858:	e8830003 	stm	r3, {r0, r1}
    c85c:	0a00001b 	beq	c8d0 <ecc_add_stat+0xbc>
    c860:	e3550000 	cmp	r5, #0	; 0x0
    c864:	b59f20f0 	ldrlt	r2, [pc, #240]	; c95c <ecc_add_stat+0x148>
    c868:	b5923008 	ldrlt	r3, [r2, #8]
    c86c:	b2833001 	addlt	r3, r3, #1	; 0x1
    c870:	b5823008 	strlt	r3, [r2, #8]
    c874:	ba000034 	blt	c94c <ecc_add_stat+0x138>
    c878:	059f20dc 	ldreq	r2, [pc, #220]	; c95c <ecc_add_stat+0x148>
    c87c:	0592300c 	ldreq	r3, [r2, #12]
    c880:	02833001 	addeq	r3, r3, #1	; 0x1
    c884:	0582300c 	streq	r3, [r2, #12]
    c888:	0a00002f 	beq	c94c <ecc_add_stat+0x138>
    c88c:	e3550008 	cmp	r5, #8	; 0x8
    c890:	c1a01005 	movgt	r1, r5
    c894:	c59f00c4 	ldrgt	r0, [pc, #196]	; c960 <ecc_add_stat+0x14c>
    c898:	ca00001d 	bgt	c914 <ecc_add_stat+0x100>
    c89c:	e59d3028 	ldr	r3, [sp, #40]
    c8a0:	e1550003 	cmp	r5, r3
    c8a4:	d3a01000 	movle	r1, #0	; 0x0
    c8a8:	da000003 	ble	c8bc <ecc_add_stat+0xa8>
    c8ac:	e59d302c 	ldr	r3, [sp, #44]
    c8b0:	e1550003 	cmp	r5, r3
    c8b4:	d3a01001 	movle	r1, #1	; 0x1
    c8b8:	c3a01002 	movgt	r1, #2	; 0x2
    c8bc:	e59f2098 	ldr	r2, [pc, #152]	; c95c <ecc_add_stat+0x148>
    c8c0:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    c8c4:	e2833001 	add	r3, r3, #1	; 0x1
    c8c8:	e7823101 	str	r3, [r2, r1, lsl #2]
    c8cc:	ea00001e 	b	c94c <ecc_add_stat+0x138>
    c8d0:	e3550000 	cmp	r5, #0	; 0x0
    c8d4:	b59f2088 	ldrlt	r2, [pc, #136]	; c964 <ecc_add_stat+0x150>
    c8d8:	b5923024 	ldrlt	r3, [r2, #36]
    c8dc:	b2833001 	addlt	r3, r3, #1	; 0x1
    c8e0:	b5823024 	strlt	r3, [r2, #36]
    c8e4:	ba000018 	blt	c94c <ecc_add_stat+0x138>
    c8e8:	059f2074 	ldreq	r2, [pc, #116]	; c964 <ecc_add_stat+0x150>
    c8ec:	05923028 	ldreq	r3, [r2, #40]
    c8f0:	02833001 	addeq	r3, r3, #1	; 0x1
    c8f4:	05823028 	streq	r3, [r2, #40]
    c8f8:	0a000013 	beq	c94c <ecc_add_stat+0x138>
    c8fc:	e355007f 	cmp	r5, #127	; 0x7f
    c900:	d1a00006 	movle	r0, r6
    c904:	d28d2004 	addle	r2, sp, #4	; 0x4
    c908:	da000005 	ble	c924 <ecc_add_stat+0x110>
    c90c:	e59f0054 	ldr	r0, [pc, #84]	; c968 <ecc_add_stat+0x154>
    c910:	e1a01005 	mov	r1, r5
    c914:	e59f3050 	ldr	r3, [pc, #80]	; c96c <ecc_add_stat+0x158>
    c918:	e1a0e00f 	mov	lr, pc
    c91c:	e593f000 	ldr	pc, [r3]
    c920:	ea000009 	b	c94c <ecc_add_stat+0x138>
    c924:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    c928:	e1550003 	cmp	r5, r3
    c92c:	da000002 	ble	c93c <ecc_add_stat+0x128>
    c930:	e2800001 	add	r0, r0, #1	; 0x1
    c934:	e3500009 	cmp	r0, #9	; 0x9
    c938:	1afffff9 	bne	c924 <ecc_add_stat+0x110>
    c93c:	e59f2020 	ldr	r2, [pc, #32]	; c964 <ecc_add_stat+0x150>
    c940:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    c944:	e2833001 	add	r3, r3, #1	; 0x1
    c948:	e7823100 	str	r3, [r2, r0, lsl #2]
    c94c:	e28dd030 	add	sp, sp, #48	; 0x30
    c950:	e8bd8070 	pop	{r4, r5, r6, pc}
    c954:	0001632c 	.word	0x0001632c
    c958:	00016324 	.word	0x00016324
    c95c:	3c00a120 	.word	0x3c00a120
    c960:	00018eac 	.word	0x00018eac
    c964:	3c00a0f0 	.word	0x3c00a0f0
    c968:	00018ed7 	.word	0x00018ed7
    c96c:	3c00a5cc 	.word	0x3c00a5cc

0000c970 <ecc_print_stat>:
    c970:	e92d4070 	push	{r4, r5, r6, lr}
    c974:	e59f1204 	ldr	r1, [pc, #516]	; cb80 <ecc_print_stat+0x210>
    c978:	e3a06000 	mov	r6, #0	; 0x0
    c97c:	e1a02006 	mov	r2, r6
    c980:	e7913002 	ldr	r3, [r1, r2]
    c984:	e2822004 	add	r2, r2, #4	; 0x4
    c988:	e352002c 	cmp	r2, #44	; 0x2c
    c98c:	e0866003 	add	r6, r6, r3
    c990:	e59f41e8 	ldr	r4, [pc, #488]	; cb80 <ecc_print_stat+0x210>
    c994:	1afffff9 	bne	c980 <ecc_print_stat+0x10>
    c998:	e3560063 	cmp	r6, #99	; 0x63
    c99c:	e59f51e0 	ldr	r5, [pc, #480]	; cb84 <ecc_print_stat+0x214>
    c9a0:	ca000003 	bgt	c9b4 <ecc_print_stat+0x44>
    c9a4:	e59f01dc 	ldr	r0, [pc, #476]	; cb88 <ecc_print_stat+0x218>
    c9a8:	e1a0e00f 	mov	lr, pc
    c9ac:	e595f000 	ldr	pc, [r5]
    c9b0:	e8bd8070 	pop	{r4, r5, r6, pc}
    c9b4:	e59f01d0 	ldr	r0, [pc, #464]	; cb8c <ecc_print_stat+0x21c>
    c9b8:	e1a0e00f 	mov	lr, pc
    c9bc:	e595f000 	ldr	pc, [r5]
    c9c0:	e59f01c8 	ldr	r0, [pc, #456]	; cb90 <ecc_print_stat+0x220>
    c9c4:	e1a0e00f 	mov	lr, pc
    c9c8:	e595f000 	ldr	pc, [r5]
    c9cc:	e5941028 	ldr	r1, [r4, #40]
    c9d0:	e59f01bc 	ldr	r0, [pc, #444]	; cb94 <ecc_print_stat+0x224>
    c9d4:	e1a0e00f 	mov	lr, pc
    c9d8:	e595f000 	ldr	pc, [r5]
    c9dc:	e5941000 	ldr	r1, [r4]
    c9e0:	e59f01b0 	ldr	r0, [pc, #432]	; cb98 <ecc_print_stat+0x228>
    c9e4:	e1a0e00f 	mov	lr, pc
    c9e8:	e595f000 	ldr	pc, [r5]
    c9ec:	e5941004 	ldr	r1, [r4, #4]
    c9f0:	e59f01a4 	ldr	r0, [pc, #420]	; cb9c <ecc_print_stat+0x22c>
    c9f4:	e1a0e00f 	mov	lr, pc
    c9f8:	e595f000 	ldr	pc, [r5]
    c9fc:	e5941008 	ldr	r1, [r4, #8]
    ca00:	e59f0198 	ldr	r0, [pc, #408]	; cba0 <ecc_print_stat+0x230>
    ca04:	e1a0e00f 	mov	lr, pc
    ca08:	e595f000 	ldr	pc, [r5]
    ca0c:	e594100c 	ldr	r1, [r4, #12]
    ca10:	e59f018c 	ldr	r0, [pc, #396]	; cba4 <ecc_print_stat+0x234>
    ca14:	e1a0e00f 	mov	lr, pc
    ca18:	e595f000 	ldr	pc, [r5]
    ca1c:	e5941010 	ldr	r1, [r4, #16]
    ca20:	e59f0180 	ldr	r0, [pc, #384]	; cba8 <ecc_print_stat+0x238>
    ca24:	e1a0e00f 	mov	lr, pc
    ca28:	e595f000 	ldr	pc, [r5]
    ca2c:	e5941014 	ldr	r1, [r4, #20]
    ca30:	e59f0174 	ldr	r0, [pc, #372]	; cbac <ecc_print_stat+0x23c>
    ca34:	e1a0e00f 	mov	lr, pc
    ca38:	e595f000 	ldr	pc, [r5]
    ca3c:	e5941018 	ldr	r1, [r4, #24]
    ca40:	e59f0168 	ldr	r0, [pc, #360]	; cbb0 <ecc_print_stat+0x240>
    ca44:	e1a0e00f 	mov	lr, pc
    ca48:	e595f000 	ldr	pc, [r5]
    ca4c:	e594101c 	ldr	r1, [r4, #28]
    ca50:	e59f015c 	ldr	r0, [pc, #348]	; cbb4 <ecc_print_stat+0x244>
    ca54:	e1a0e00f 	mov	lr, pc
    ca58:	e595f000 	ldr	pc, [r5]
    ca5c:	e5941020 	ldr	r1, [r4, #32]
    ca60:	e59f0150 	ldr	r0, [pc, #336]	; cbb8 <ecc_print_stat+0x248>
    ca64:	e1a0e00f 	mov	lr, pc
    ca68:	e595f000 	ldr	pc, [r5]
    ca6c:	e5941024 	ldr	r1, [r4, #36]
    ca70:	e59f0144 	ldr	r0, [pc, #324]	; cbbc <ecc_print_stat+0x24c>
    ca74:	e1a0e00f 	mov	lr, pc
    ca78:	e595f000 	ldr	pc, [r5]
    ca7c:	e59f010c 	ldr	r0, [pc, #268]	; cb90 <ecc_print_stat+0x220>
    ca80:	e1a0e00f 	mov	lr, pc
    ca84:	e595f000 	ldr	pc, [r5]
    ca88:	e1a00006 	mov	r0, r6
    ca8c:	e3a01064 	mov	r1, #100	; 0x64
    ca90:	eb00213d 	bl	14f8c <__aeabi_idiv>
    ca94:	e5944028 	ldr	r4, [r4, #40]
    ca98:	e1a01000 	mov	r1, r0
    ca9c:	e0644006 	rsb	r4, r4, r6
    caa0:	e1a00004 	mov	r0, r4
    caa4:	eb002138 	bl	14f8c <__aeabi_idiv>
    caa8:	e59f4110 	ldr	r4, [pc, #272]	; cbc0 <ecc_print_stat+0x250>
    caac:	e1a02546 	asr	r2, r6, #10
    cab0:	e1a03000 	mov	r3, r0
    cab4:	e1a01006 	mov	r1, r6
    cab8:	e59f0104 	ldr	r0, [pc, #260]	; cbc4 <ecc_print_stat+0x254>
    cabc:	e1a0e00f 	mov	lr, pc
    cac0:	e595f000 	ldr	pc, [r5]
    cac4:	e894000c 	ldm	r4, {r2, r3}
    cac8:	e0833002 	add	r3, r3, r2
    cacc:	e5942008 	ldr	r2, [r4, #8]
    cad0:	e594100c 	ldr	r1, [r4, #12]
    cad4:	e0833002 	add	r3, r3, r2
    cad8:	e0836001 	add	r6, r3, r1
    cadc:	e3560063 	cmp	r6, #99	; 0x63
    cae0:	d8bd8070 	pople	{r4, r5, r6, pc}
    cae4:	e59f00dc 	ldr	r0, [pc, #220]	; cbc8 <ecc_print_stat+0x258>
    cae8:	e1a0e00f 	mov	lr, pc
    caec:	e595f000 	ldr	pc, [r5]
    caf0:	e59f0098 	ldr	r0, [pc, #152]	; cb90 <ecc_print_stat+0x220>
    caf4:	e1a0e00f 	mov	lr, pc
    caf8:	e595f000 	ldr	pc, [r5]
    cafc:	e594100c 	ldr	r1, [r4, #12]
    cb00:	e59f008c 	ldr	r0, [pc, #140]	; cb94 <ecc_print_stat+0x224>
    cb04:	e1a0e00f 	mov	lr, pc
    cb08:	e595f000 	ldr	pc, [r5]
    cb0c:	e5941000 	ldr	r1, [r4]
    cb10:	e59f00b4 	ldr	r0, [pc, #180]	; cbcc <ecc_print_stat+0x25c>
    cb14:	e1a0e00f 	mov	lr, pc
    cb18:	e595f000 	ldr	pc, [r5]
    cb1c:	e5941004 	ldr	r1, [r4, #4]
    cb20:	e59f0078 	ldr	r0, [pc, #120]	; cba0 <ecc_print_stat+0x230>
    cb24:	e1a0e00f 	mov	lr, pc
    cb28:	e595f000 	ldr	pc, [r5]
    cb2c:	e5941008 	ldr	r1, [r4, #8]
    cb30:	e59f0084 	ldr	r0, [pc, #132]	; cbbc <ecc_print_stat+0x24c>
    cb34:	e1a0e00f 	mov	lr, pc
    cb38:	e595f000 	ldr	pc, [r5]
    cb3c:	e59f004c 	ldr	r0, [pc, #76]	; cb90 <ecc_print_stat+0x220>
    cb40:	e1a0e00f 	mov	lr, pc
    cb44:	e595f000 	ldr	pc, [r5]
    cb48:	e1a00006 	mov	r0, r6
    cb4c:	e3a01064 	mov	r1, #100	; 0x64
    cb50:	eb00210d 	bl	14f8c <__aeabi_idiv>
    cb54:	e594400c 	ldr	r4, [r4, #12]
    cb58:	e1a01000 	mov	r1, r0
    cb5c:	e0644006 	rsb	r4, r4, r6
    cb60:	e1a00004 	mov	r0, r4
    cb64:	eb002108 	bl	14f8c <__aeabi_idiv>
    cb68:	e1a01006 	mov	r1, r6
    cb6c:	e1a02000 	mov	r2, r0
    cb70:	e59f0058 	ldr	r0, [pc, #88]	; cbd0 <ecc_print_stat+0x260>
    cb74:	e1a0e00f 	mov	lr, pc
    cb78:	e595f000 	ldr	pc, [r5]
    cb7c:	e8bd8070 	pop	{r4, r5, r6, pc}
    cb80:	3c00a0f0 	.word	0x3c00a0f0
    cb84:	3c00a5cc 	.word	0x3c00a5cc
    cb88:	00018f02 	.word	0x00018f02
    cb8c:	00018f16 	.word	0x00018f16
    cb90:	00018f2f 	.word	0x00018f2f
    cb94:	00018f4d 	.word	0x00018f4d
    cb98:	00018f5c 	.word	0x00018f5c
    cb9c:	00018f6b 	.word	0x00018f6b
    cba0:	00018f7a 	.word	0x00018f7a
    cba4:	00018f89 	.word	0x00018f89
    cba8:	00018f98 	.word	0x00018f98
    cbac:	00018fa7 	.word	0x00018fa7
    cbb0:	00018fb6 	.word	0x00018fb6
    cbb4:	00018fc5 	.word	0x00018fc5
    cbb8:	00018fd4 	.word	0x00018fd4
    cbbc:	00018fe3 	.word	0x00018fe3
    cbc0:	3c00a120 	.word	0x3c00a120
    cbc4:	00018ff2 	.word	0x00018ff2
    cbc8:	00019026 	.word	0x00019026
    cbcc:	00019040 	.word	0x00019040
    cbd0:	0001904f 	.word	0x0001904f

0000cbd4 <nf2_set_busw>:
    cbd4:	e3500010 	cmp	r0, #16	; 0x10
    cbd8:	03a02001 	moveq	r2, #1	; 0x1
    cbdc:	13a02000 	movne	r2, #0	; 0x0
    cbe0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cbe4:	e58321b8 	str	r2, [r3, #440]
    cbe8:	e3a00000 	mov	r0, #0	; 0x0
    cbec:	e12fff1e 	bx	lr

0000cbf0 <nf2_ecc_cap>:
    cbf0:	e350000f 	cmp	r0, #15	; 0xf
    cbf4:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    cbf8:	ea000011 	b	cc44 <nf2_ecc_cap+0x54>
    cbfc:	0000cc3c 	.word	0x0000cc3c
    cc00:	0000cc4c 	.word	0x0000cc4c
    cc04:	0000cc54 	.word	0x0000cc54
    cc08:	0000cc5c 	.word	0x0000cc5c
    cc0c:	0000cc64 	.word	0x0000cc64
    cc10:	0000cc6c 	.word	0x0000cc6c
    cc14:	0000cc74 	.word	0x0000cc74
    cc18:	0000cc7c 	.word	0x0000cc7c
    cc1c:	0000cc84 	.word	0x0000cc84
    cc20:	0000cc8c 	.word	0x0000cc8c
    cc24:	0000cc94 	.word	0x0000cc94
    cc28:	0000cc9c 	.word	0x0000cc9c
    cc2c:	0000cca4 	.word	0x0000cca4
    cc30:	0000ccac 	.word	0x0000ccac
    cc34:	0000ccb4 	.word	0x0000ccb4
    cc38:	0000ccbc 	.word	0x0000ccbc
    cc3c:	e3a00002 	mov	r0, #2	; 0x2
    cc40:	e12fff1e 	bx	lr
    cc44:	e3a00000 	mov	r0, #0	; 0x0
    cc48:	e12fff1e 	bx	lr
    cc4c:	e3a00004 	mov	r0, #4	; 0x4
    cc50:	e12fff1e 	bx	lr
    cc54:	e3a00008 	mov	r0, #8	; 0x8
    cc58:	e12fff1e 	bx	lr
    cc5c:	e3a00010 	mov	r0, #16	; 0x10
    cc60:	e12fff1e 	bx	lr
    cc64:	e3a00018 	mov	r0, #24	; 0x18
    cc68:	e12fff1e 	bx	lr
    cc6c:	e3a00020 	mov	r0, #32	; 0x20
    cc70:	e12fff1e 	bx	lr
    cc74:	e3a00028 	mov	r0, #40	; 0x28
    cc78:	e12fff1e 	bx	lr
    cc7c:	e3a00030 	mov	r0, #48	; 0x30
    cc80:	e12fff1e 	bx	lr
    cc84:	e3a00038 	mov	r0, #56	; 0x38
    cc88:	e12fff1e 	bx	lr
    cc8c:	e3a0003c 	mov	r0, #60	; 0x3c
    cc90:	e12fff1e 	bx	lr
    cc94:	e3a00040 	mov	r0, #64	; 0x40
    cc98:	e12fff1e 	bx	lr
    cc9c:	e3a00048 	mov	r0, #72	; 0x48
    cca0:	e12fff1e 	bx	lr
    cca4:	e3a00050 	mov	r0, #80	; 0x50
    cca8:	e12fff1e 	bx	lr
    ccac:	e3a00060 	mov	r0, #96	; 0x60
    ccb0:	e12fff1e 	bx	lr
    ccb4:	e3a00070 	mov	r0, #112	; 0x70
    ccb8:	e12fff1e 	bx	lr
    ccbc:	e3a0007f 	mov	r0, #127	; 0x7f
    ccc0:	e12fff1e 	bx	lr

0000ccc4 <trx_afifo_reg>:
    ccc4:	e2503000 	subs	r3, r0, #0	; 0x0
    ccc8:	e1a00002 	mov	r0, r2
    cccc:	1a000008 	bne	ccf4 <trx_afifo_reg+0x30>
    ccd0:	e1a02802 	lsl	r2, r2, #16
    ccd4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ccd8:	e3811a0f 	orr	r1, r1, #61440	; 0xf000
    ccdc:	e1a02822 	lsr	r2, r2, #16
    cce0:	e1a00820 	lsr	r0, r0, #16
    cce4:	e5831060 	str	r1, [r3, #96]
    cce8:	e5832060 	str	r2, [r3, #96]
    ccec:	e5830060 	str	r0, [r3, #96]
    ccf0:	e12fff1e 	bx	lr
    ccf4:	e3530001 	cmp	r3, #1	; 0x1
    ccf8:	01a03802 	lsleq	r3, r2, #16
    ccfc:	11813403 	orrne	r3, r1, r3, lsl #8
    cd00:	03a0230a 	moveq	r2, #671088640	; 0x28000000
    cd04:	03811cf1 	orreq	r1, r1, #61696	; 0xf100
    cd08:	01a03823 	lsreq	r3, r3, #16
    cd0c:	13833a0f 	orrne	r3, r3, #61440	; 0xf000
    cd10:	13a0230a 	movne	r2, #671088640	; 0x28000000
    cd14:	05821060 	streq	r1, [r2, #96]
    cd18:	e5823060 	str	r3, [r2, #96]
    cd1c:	e12fff1e 	bx	lr

0000cd20 <trx_afifo_enable>:
    cd20:	e24dd008 	sub	sp, sp, #8	; 0x8
    cd24:	e3a03000 	mov	r3, #0	; 0x0
    cd28:	e58d3004 	str	r3, [sp, #4]
    cd2c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cd30:	e5923054 	ldr	r3, [r2, #84]
    cd34:	e58d3004 	str	r3, [sp, #4]
    cd38:	e59d3004 	ldr	r3, [sp, #4]
    cd3c:	e3833001 	orr	r3, r3, #1	; 0x1
    cd40:	e58d3004 	str	r3, [sp, #4]
    cd44:	e59d3004 	ldr	r3, [sp, #4]
    cd48:	e5823054 	str	r3, [r2, #84]
    cd4c:	e28dd008 	add	sp, sp, #8	; 0x8
    cd50:	e12fff1e 	bx	lr

0000cd54 <trx_afifo_disable>:
    cd54:	e3a02000 	mov	r2, #0	; 0x0
    cd58:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cd5c:	e5832054 	str	r2, [r3, #84]
    cd60:	e12fff1e 	bx	lr

0000cd64 <trx_nand_cs>:
    cd64:	e3500001 	cmp	r0, #1	; 0x1
    cd68:	03e02001 	mvneq	r2, #1	; 0x1
    cd6c:	13a0200f 	movne	r2, #15	; 0xf
    cd70:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cd74:	e583204c 	str	r2, [r3, #76]
    cd78:	e12fff1e 	bx	lr

0000cd7c <trx_afifo_start>:
    cd7c:	e3a02001 	mov	r2, #1	; 0x1
    cd80:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cd84:	e5832084 	str	r2, [r3, #132]
    cd88:	e12fff1e 	bx	lr

0000cd8c <trx_afifo_is_finish>:
    cd8c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cd90:	e593308c 	ldr	r3, [r3, #140]
    cd94:	e0000003 	and	r0, r0, r3
    cd98:	e12fff1e 	bx	lr

0000cd9c <trx_afifo_intr_clear>:
    cd9c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    cda0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cda4:	e583208c 	str	r2, [r3, #140]
    cda8:	e12fff1e 	bx	lr

0000cdac <trx_afifo_read_state>:
    cdac:	e3500003 	cmp	r0, #3	; 0x3
    cdb0:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    cdb4:	ea000003 	b	cdc8 <trx_afifo_read_state+0x1c>
    cdb8:	0000cdd0 	.word	0x0000cdd0
    cdbc:	0000cddc 	.word	0x0000cddc
    cdc0:	0000cde8 	.word	0x0000cde8
    cdc4:	0000cdf4 	.word	0x0000cdf4
    cdc8:	e3e00000 	mvn	r0, #0	; 0x0
    cdcc:	e12fff1e 	bx	lr
    cdd0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cdd4:	e5930010 	ldr	r0, [r3, #16]
    cdd8:	e12fff1e 	bx	lr
    cddc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cde0:	e5930014 	ldr	r0, [r3, #20]
    cde4:	e12fff1e 	bx	lr
    cde8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cdec:	e59300e0 	ldr	r0, [r3, #224]
    cdf0:	e12fff1e 	bx	lr
    cdf4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cdf8:	e59300e4 	ldr	r0, [r3, #228]
    cdfc:	e12fff1e 	bx	lr

0000ce00 <trx_afifo_etd>:
    ce00:	e1811080 	orr	r1, r1, r0, lsl #1
    ce04:	e3811902 	orr	r1, r1, #32768	; 0x8000
    ce08:	e1811102 	orr	r1, r1, r2, lsl #2
    ce0c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ce10:	e5831060 	str	r1, [r3, #96]
    ce14:	e12fff1e 	bx	lr

0000ce18 <cpu_afifo_disable>:
    ce18:	e24dd008 	sub	sp, sp, #8	; 0x8
    ce1c:	e3a03000 	mov	r3, #0	; 0x0
    ce20:	e58d3004 	str	r3, [sp, #4]
    ce24:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ce28:	e5923054 	ldr	r3, [r2, #84]
    ce2c:	e58d3004 	str	r3, [sp, #4]
    ce30:	e59d3004 	ldr	r3, [sp, #4]
    ce34:	e3c33002 	bic	r3, r3, #2	; 0x2
    ce38:	e58d3004 	str	r3, [sp, #4]
    ce3c:	e59d3004 	ldr	r3, [sp, #4]
    ce40:	e5823054 	str	r3, [r2, #84]
    ce44:	e28dd008 	add	sp, sp, #8	; 0x8
    ce48:	e12fff1e 	bx	lr

0000ce4c <cpu_afifo_enable>:
    ce4c:	e24dd008 	sub	sp, sp, #8	; 0x8
    ce50:	e3a03000 	mov	r3, #0	; 0x0
    ce54:	e58d3004 	str	r3, [sp, #4]
    ce58:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ce5c:	e5923054 	ldr	r3, [r2, #84]
    ce60:	e58d3004 	str	r3, [sp, #4]
    ce64:	e59d3004 	ldr	r3, [sp, #4]
    ce68:	e3833002 	orr	r3, r3, #2	; 0x2
    ce6c:	e58d3004 	str	r3, [sp, #4]
    ce70:	e59d3004 	ldr	r3, [sp, #4]
    ce74:	e5823054 	str	r3, [r2, #84]
    ce78:	e28dd008 	add	sp, sp, #8	; 0x8
    ce7c:	e12fff1e 	bx	lr

0000ce80 <trx_afifo_nop>:
    ce80:	e1a01b01 	lsl	r1, r1, #22
    ce84:	e1a01b21 	lsr	r1, r1, #22
    ce88:	e200000f 	and	r0, r0, #15	; 0xf
    ce8c:	e1811500 	orr	r1, r1, r0, lsl #10
    ce90:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ce94:	e5831060 	str	r1, [r3, #96]
    ce98:	e12fff1e 	bx	lr

0000ce9c <trx_afifo_ncmd>:
    ce9c:	e200000f 	and	r0, r0, #15	; 0xf
    cea0:	e1a01401 	lsl	r1, r1, #8
    cea4:	e1811500 	orr	r1, r1, r0, lsl #10
    cea8:	e3811901 	orr	r1, r1, #16384	; 0x4000
    ceac:	e1811002 	orr	r1, r1, r2
    ceb0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ceb4:	e5831060 	str	r1, [r3, #96]
    ceb8:	e12fff1e 	bx	lr

0000cebc <nf2_secc_cap>:
    cebc:	e3500000 	cmp	r0, #0	; 0x0
    cec0:	0a000003 	beq	ced4 <nf2_secc_cap+0x18>
    cec4:	e3500001 	cmp	r0, #1	; 0x1
    cec8:	03a00008 	moveq	r0, #8	; 0x8
    cecc:	13a00000 	movne	r0, #0	; 0x0
    ced0:	e12fff1e 	bx	lr
    ced4:	e3a00004 	mov	r0, #4	; 0x4
    ced8:	e12fff1e 	bx	lr

0000cedc <nf2_secc_cfg>:
    cedc:	e3a0c30a 	mov	ip, #671088640	; 0x28000000
    cee0:	e59c3018 	ldr	r3, [ip, #24]
    cee4:	e24dd008 	sub	sp, sp, #8	; 0x8
    cee8:	e58d3004 	str	r3, [sp, #4]
    ceec:	e1a00800 	lsl	r0, r0, #16
    cef0:	e59d3004 	ldr	r3, [sp, #4]
    cef4:	e1800881 	orr	r0, r0, r1, lsl #17
    cef8:	e1800182 	orr	r0, r0, r2, lsl #3
    cefc:	e1800003 	orr	r0, r0, r3
    cf00:	e58d0004 	str	r0, [sp, #4]
    cf04:	e59d3004 	ldr	r3, [sp, #4]
    cf08:	e3510000 	cmp	r1, #0	; 0x0
    cf0c:	e58c3018 	str	r3, [ip, #24]
    cf10:	03a01004 	moveq	r1, #4	; 0x4
    cf14:	0a000002 	beq	cf24 <nf2_secc_cfg+0x48>
    cf18:	e3510001 	cmp	r1, #1	; 0x1
    cf1c:	03a01008 	moveq	r1, #8	; 0x8
    cf20:	13a01000 	movne	r1, #0	; 0x0
    cf24:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cf28:	e592311c 	ldr	r3, [r2, #284]
    cf2c:	e58d3004 	str	r3, [sp, #4]
    cf30:	e59d3004 	ldr	r3, [sp, #4]
    cf34:	e3c3380f 	bic	r3, r3, #983040	; 0xf0000
    cf38:	e58d3004 	str	r3, [sp, #4]
    cf3c:	e59d3004 	ldr	r3, [sp, #4]
    cf40:	e1833801 	orr	r3, r3, r1, lsl #16
    cf44:	e58d3004 	str	r3, [sp, #4]
    cf48:	e59d3004 	ldr	r3, [sp, #4]
    cf4c:	e582311c 	str	r3, [r2, #284]
    cf50:	e28dd008 	add	sp, sp, #8	; 0x8
    cf54:	e12fff1e 	bx	lr

0000cf58 <nf2_ecc_cfg>:
    cf58:	e380cc03 	orr	ip, r0, #768	; 0x300
    cf5c:	e18cc581 	orr	ip, ip, r1, lsl #11
    cf60:	e92d4010 	push	{r4, lr}
    cf64:	e18cc202 	orr	ip, ip, r2, lsl #4
    cf68:	e18cc603 	orr	ip, ip, r3, lsl #12
    cf6c:	e59d3010 	ldr	r3, [sp, #16]
    cf70:	e59d100c 	ldr	r1, [sp, #12]
    cf74:	e18cc083 	orr	ip, ip, r3, lsl #1
    cf78:	e59d3014 	ldr	r3, [sp, #20]
    cf7c:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    cf80:	e18cc903 	orr	ip, ip, r3, lsl #18
    cf84:	e59d3008 	ldr	r3, [sp, #8]
    cf88:	e1a00002 	mov	r0, r2
    cf8c:	e1811803 	orr	r1, r1, r3, lsl #16
    cf90:	e584c018 	str	ip, [r4, #24]
    cf94:	e584101c 	str	r1, [r4, #28]
    cf98:	ebffff14 	bl	cbf0 <nf2_ecc_cap>
    cf9c:	e594311c 	ldr	r3, [r4, #284]
    cfa0:	e3c33c7f 	bic	r3, r3, #32512	; 0x7f00
    cfa4:	e1833400 	orr	r3, r3, r0, lsl #8
    cfa8:	e584311c 	str	r3, [r4, #284]
    cfac:	e8bd8010 	pop	{r4, pc}

0000cfb0 <nf2_ecc_num>:
    cfb0:	e1a01541 	asr	r1, r1, #10
    cfb4:	e350000f 	cmp	r0, #15	; 0xf
    cfb8:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    cfbc:	ea00000f 	b	d000 <nf2_ecc_num+0x50>
    cfc0:	0000d008 	.word	0x0000d008
    cfc4:	0000d010 	.word	0x0000d010
    cfc8:	0000d018 	.word	0x0000d018
    cfcc:	0000d020 	.word	0x0000d020
    cfd0:	0000d028 	.word	0x0000d028
    cfd4:	0000d030 	.word	0x0000d030
    cfd8:	0000d038 	.word	0x0000d038
    cfdc:	0000d040 	.word	0x0000d040
    cfe0:	0000d048 	.word	0x0000d048
    cfe4:	0000d050 	.word	0x0000d050
    cfe8:	0000d058 	.word	0x0000d058
    cfec:	0000d060 	.word	0x0000d060
    cff0:	0000d068 	.word	0x0000d068
    cff4:	0000d070 	.word	0x0000d070
    cff8:	0000d078 	.word	0x0000d078
    cffc:	0000d080 	.word	0x0000d080
    d000:	e3a00000 	mov	r0, #0	; 0x0
    d004:	ea00001f 	b	d088 <nf2_ecc_num+0xd8>
    d008:	e1a00101 	lsl	r0, r1, #2
    d00c:	ea00001d 	b	d088 <nf2_ecc_num+0xd8>
    d010:	e1a00181 	lsl	r0, r1, #3
    d014:	ea00001b 	b	d088 <nf2_ecc_num+0xd8>
    d018:	e1a00201 	lsl	r0, r1, #4
    d01c:	ea000019 	b	d088 <nf2_ecc_num+0xd8>
    d020:	e3a0301c 	mov	r3, #28	; 0x1c
    d024:	ea000016 	b	d084 <nf2_ecc_num+0xd4>
    d028:	e3a0302c 	mov	r3, #44	; 0x2c
    d02c:	ea000014 	b	d084 <nf2_ecc_num+0xd4>
    d030:	e3a03038 	mov	r3, #56	; 0x38
    d034:	ea000012 	b	d084 <nf2_ecc_num+0xd4>
    d038:	e3a03048 	mov	r3, #72	; 0x48
    d03c:	ea000010 	b	d084 <nf2_ecc_num+0xd4>
    d040:	e3a03054 	mov	r3, #84	; 0x54
    d044:	ea00000e 	b	d084 <nf2_ecc_num+0xd4>
    d048:	e3a03064 	mov	r3, #100	; 0x64
    d04c:	ea00000c 	b	d084 <nf2_ecc_num+0xd4>
    d050:	e3a0306c 	mov	r3, #108	; 0x6c
    d054:	ea00000a 	b	d084 <nf2_ecc_num+0xd4>
    d058:	e3a03070 	mov	r3, #112	; 0x70
    d05c:	ea000008 	b	d084 <nf2_ecc_num+0xd4>
    d060:	e1a00381 	lsl	r0, r1, #7
    d064:	ea000007 	b	d088 <nf2_ecc_num+0xd8>
    d068:	e3a0308c 	mov	r3, #140	; 0x8c
    d06c:	ea000004 	b	d084 <nf2_ecc_num+0xd4>
    d070:	e3a030a8 	mov	r3, #168	; 0xa8
    d074:	ea000002 	b	d084 <nf2_ecc_num+0xd4>
    d078:	e3a030c4 	mov	r3, #196	; 0xc4
    d07c:	ea000000 	b	d084 <nf2_ecc_num+0xd4>
    d080:	e3a030e0 	mov	r3, #224	; 0xe0
    d084:	e0000193 	mul	r0, r3, r1
    d088:	e3520001 	cmp	r2, #1	; 0x1
    d08c:	01a00080 	lsleq	r0, r0, #1
    d090:	e12fff1e 	bx	lr

0000d094 <nf2_addr_cfg>:
    d094:	e3530010 	cmp	r3, #16	; 0x10
    d098:	01a010c1 	asreq	r1, r1, #1
    d09c:	01a020c2 	asreq	r2, r2, #1
    d0a0:	e1813802 	orr	r3, r1, r2, lsl #16
    d0a4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d0a8:	e3a01000 	mov	r1, #0	; 0x0
    d0ac:	e5820020 	str	r0, [r2, #32]
    d0b0:	e5821024 	str	r1, [r2, #36]
    d0b4:	e5821028 	str	r1, [r2, #40]
    d0b8:	e582102c 	str	r1, [r2, #44]
    d0bc:	e5823030 	str	r3, [r2, #48]
    d0c0:	e12fff1e 	bx	lr

0000d0c4 <nf2_sdma_cfg>:
    d0c4:	e59dc000 	ldr	ip, [sp]
    d0c8:	e1813803 	orr	r3, r1, r3, lsl #16
    d0cc:	e1a0c10c 	lsl	ip, ip, #2
    d0d0:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    d0d4:	e5810038 	str	r0, [r1, #56]
    d0d8:	e38cc001 	orr	ip, ip, #1	; 0x1
    d0dc:	e581203c 	str	r2, [r1, #60]
    d0e0:	e5813040 	str	r3, [r1, #64]
    d0e4:	e3a03341 	mov	r3, #67108865	; 0x4000001
    d0e8:	e5813044 	str	r3, [r1, #68]
    d0ec:	e581c048 	str	ip, [r1, #72]
    d0f0:	e12fff1e 	bx	lr

0000d0f4 <nf2_check_ecc>:
    d0f4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d0f8:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d0fc:	e59230e8 	ldr	r3, [r2, #232]
    d100:	e24dd028 	sub	sp, sp, #40	; 0x28
    d104:	e58d3000 	str	r3, [sp]
    d108:	e59230ec 	ldr	r3, [r2, #236]
    d10c:	e3a0c000 	mov	ip, #0	; 0x0
    d110:	e58d3004 	str	r3, [sp, #4]
    d114:	e59230f0 	ldr	r3, [r2, #240]
    d118:	e1a06001 	mov	r6, r1
    d11c:	e58d3008 	str	r3, [sp, #8]
    d120:	e59230f4 	ldr	r3, [r2, #244]
    d124:	e1a0400c 	mov	r4, ip
    d128:	e58d300c 	str	r3, [sp, #12]
    d12c:	e59230f8 	ldr	r3, [r2, #248]
    d130:	e1a08540 	asr	r8, r0, #10
    d134:	e58d3010 	str	r3, [sp, #16]
    d138:	e59230fc 	ldr	r3, [r2, #252]
    d13c:	e1a0500c 	mov	r5, ip
    d140:	e58d3014 	str	r3, [sp, #20]
    d144:	e5923100 	ldr	r3, [r2, #256]
    d148:	e58d3018 	str	r3, [sp, #24]
    d14c:	e5923104 	ldr	r3, [r2, #260]
    d150:	e58d301c 	str	r3, [sp, #28]
    d154:	e5927108 	ldr	r7, [r2, #264]
    d158:	e581c000 	str	ip, [r1]
    d15c:	e58d7020 	str	r7, [sp, #32]
    d160:	ea000028 	b	d208 <nf2_check_ecc+0x114>
    d164:	e1a03457 	asr	r3, r7, r4
    d168:	e3130001 	tst	r3, #1	; 0x1
    d16c:	0a000004 	beq	d184 <nf2_check_ecc+0x90>
    d170:	e3a00000 	mov	r0, #0	; 0x0
    d174:	e3e01000 	mvn	r1, #0	; 0x0
    d178:	ebfffda5 	bl	c814 <ecc_add_stat>
    d17c:	e3e00000 	mvn	r0, #0	; 0x0
    d180:	ea000024 	b	d218 <nf2_check_ecc+0x124>
    d184:	e1a03144 	asr	r3, r4, #2
    d188:	e3520003 	cmp	r2, #3	; 0x3
    d18c:	979ff102 	ldrls	pc, [pc, r2, lsl #2]
    d190:	ea000015 	b	d1ec <nf2_check_ecc+0xf8>
    d194:	0000d1a4 	.word	0x0000d1a4
    d198:	0000d1b4 	.word	0x0000d1b4
    d19c:	0000d1cc 	.word	0x0000d1cc
    d1a0:	0000d1dc 	.word	0x0000d1dc
    d1a4:	e28d2028 	add	r2, sp, #40	; 0x28
    d1a8:	e0823103 	add	r3, r2, r3, lsl #2
    d1ac:	e5133028 	ldr	r3, [r3, #-40]
    d1b0:	ea00000c 	b	d1e8 <nf2_check_ecc+0xf4>
    d1b4:	e28d2028 	add	r2, sp, #40	; 0x28
    d1b8:	e0823103 	add	r3, r2, r3, lsl #2
    d1bc:	e5133028 	ldr	r3, [r3, #-40]
    d1c0:	e2033c7f 	and	r3, r3, #32512	; 0x7f00
    d1c4:	e1a05443 	asr	r5, r3, #8
    d1c8:	ea000007 	b	d1ec <nf2_check_ecc+0xf8>
    d1cc:	e28d2028 	add	r2, sp, #40	; 0x28
    d1d0:	e0823103 	add	r3, r2, r3, lsl #2
    d1d4:	e15332b6 	ldrh	r3, [r3, #-38]
    d1d8:	ea000002 	b	d1e8 <nf2_check_ecc+0xf4>
    d1dc:	e28d2028 	add	r2, sp, #40	; 0x28
    d1e0:	e0823103 	add	r3, r2, r3, lsl #2
    d1e4:	e5533025 	ldrb	r3, [r3, #-37]
    d1e8:	e203507f 	and	r5, r3, #127	; 0x7f
    d1ec:	e3a00000 	mov	r0, #0	; 0x0
    d1f0:	e1a01005 	mov	r1, r5
    d1f4:	ebfffd86 	bl	c814 <ecc_add_stat>
    d1f8:	e5963000 	ldr	r3, [r6]
    d1fc:	e2844001 	add	r4, r4, #1	; 0x1
    d200:	e0833005 	add	r3, r3, r5
    d204:	e5863000 	str	r3, [r6]
    d208:	e1540008 	cmp	r4, r8
    d20c:	e2042003 	and	r2, r4, #3	; 0x3
    d210:	baffffd3 	blt	d164 <nf2_check_ecc+0x70>
    d214:	e3a00000 	mov	r0, #0	; 0x0
    d218:	e28dd028 	add	sp, sp, #40	; 0x28
    d21c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000d220 <nf2_check_secc>:
    d220:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d224:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d228:	e593310c 	ldr	r3, [r3, #268]
    d22c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d230:	e3130040 	tst	r3, #64	; 0x40
    d234:	0a000004 	beq	d24c <nf2_check_secc+0x2c>
    d238:	e3a00001 	mov	r0, #1	; 0x1
    d23c:	e3e01000 	mvn	r1, #0	; 0x0
    d240:	ebfffd73 	bl	c814 <ecc_add_stat>
    d244:	e3e00000 	mvn	r0, #0	; 0x0
    d248:	ea000006 	b	d268 <nf2_check_secc+0x48>
    d24c:	e3130080 	tst	r3, #128	; 0x80
    d250:	1203300f 	andne	r3, r3, #15	; 0xf
    d254:	15803000 	strne	r3, [r0]
    d258:	e5901000 	ldr	r1, [r0]
    d25c:	e3a00001 	mov	r0, #1	; 0x1
    d260:	ebfffd6b 	bl	c814 <ecc_add_stat>
    d264:	e3a00000 	mov	r0, #0	; 0x0
    d268:	e28dd004 	add	sp, sp, #4	; 0x4
    d26c:	e8bd8000 	pop	{pc}

0000d270 <nf2_set_polynomial>:
    d270:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d274:	e5830160 	str	r0, [r3, #352]
    d278:	e3a00001 	mov	r0, #1	; 0x1
    d27c:	e12fff1e 	bx	lr

0000d280 <nf2_afifo_reset>:
    d280:	e92d4070 	push	{r4, r5, r6, lr}
    d284:	e3a02000 	mov	r2, #0	; 0x0
    d288:	e24dd008 	sub	sp, sp, #8	; 0x8
    d28c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d290:	e58d2004 	str	r2, [sp, #4]
    d294:	e2822010 	add	r2, r2, #16	; 0x10
    d298:	e5832080 	str	r2, [r3, #128]
    d29c:	e1a05003 	mov	r5, r3
    d2a0:	eb0004e9 	bl	e64c <get_ticks>
    d2a4:	e59f6034 	ldr	r6, [pc, #52]	; d2e0 <nf2_afifo_reset+0x60>
    d2a8:	e1a04000 	mov	r4, r0
    d2ac:	eb0004e6 	bl	e64c <get_ticks>
    d2b0:	e0640000 	rsb	r0, r4, r0
    d2b4:	e1500006 	cmp	r0, r6
    d2b8:	c3e00000 	mvngt	r0, #0	; 0x0
    d2bc:	ca000005 	bgt	d2d8 <nf2_afifo_reset+0x58>
    d2c0:	e5953080 	ldr	r3, [r5, #128]
    d2c4:	e203301f 	and	r3, r3, #31	; 0x1f
    d2c8:	e58d3004 	str	r3, [sp, #4]
    d2cc:	e59d0004 	ldr	r0, [sp, #4]
    d2d0:	e3500000 	cmp	r0, #0	; 0x0
    d2d4:	1afffff4 	bne	d2ac <nf2_afifo_reset+0x2c>
    d2d8:	e28dd008 	add	sp, sp, #8	; 0x8
    d2dc:	e8bd8070 	pop	{r4, r5, r6, pc}
    d2e0:	0007a120 	.word	0x0007a120

0000d2e4 <nf2_soft_reset>:
    d2e4:	e92d4070 	push	{r4, r5, r6, lr}
    d2e8:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d2ec:	e24dd008 	sub	sp, sp, #8	; 0x8
    d2f0:	e3a03000 	mov	r3, #0	; 0x0
    d2f4:	e58d3004 	str	r3, [sp, #4]
    d2f8:	e5820080 	str	r0, [r2, #128]
    d2fc:	e1a05002 	mov	r5, r2
    d300:	eb0004d1 	bl	e64c <get_ticks>
    d304:	e59f6034 	ldr	r6, [pc, #52]	; d340 <nf2_soft_reset+0x5c>
    d308:	e1a04000 	mov	r4, r0
    d30c:	eb0004ce 	bl	e64c <get_ticks>
    d310:	e0640000 	rsb	r0, r4, r0
    d314:	e1500006 	cmp	r0, r6
    d318:	c3e00000 	mvngt	r0, #0	; 0x0
    d31c:	ca000005 	bgt	d338 <nf2_soft_reset+0x54>
    d320:	e5953080 	ldr	r3, [r5, #128]
    d324:	e203301f 	and	r3, r3, #31	; 0x1f
    d328:	e58d3004 	str	r3, [sp, #4]
    d32c:	e59d0004 	ldr	r0, [sp, #4]
    d330:	e3500000 	cmp	r0, #0	; 0x0
    d334:	1afffff4 	bne	d30c <nf2_soft_reset+0x28>
    d338:	e28dd008 	add	sp, sp, #8	; 0x8
    d33c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d340:	0007a120 	.word	0x0007a120

0000d344 <nf2_check_irq>:
    d344:	e92d4030 	push	{r4, r5, lr}
    d348:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d34c:	e594208c 	ldr	r2, [r4, #140]
    d350:	e5943090 	ldr	r3, [r4, #144]
    d354:	e24dd004 	sub	sp, sp, #4	; 0x4
    d358:	e0035002 	and	r5, r3, r2
    d35c:	e3150701 	tst	r5, #262144	; 0x40000
    d360:	0a000006 	beq	d380 <nf2_check_irq+0x3c>
    d364:	e5943090 	ldr	r3, [r4, #144]
    d368:	e3c55701 	bic	r5, r5, #262144	; 0x40000
    d36c:	e3c33701 	bic	r3, r3, #262144	; 0x40000
    d370:	e5843090 	str	r3, [r4, #144]
    d374:	e3a03701 	mov	r3, #262144	; 0x40000
    d378:	e584308c 	str	r3, [r4, #140]
    d37c:	ea000072 	b	d54c <nf2_check_irq+0x208>
    d380:	e3150802 	tst	r5, #131072	; 0x20000
    d384:	0a000006 	beq	d3a4 <nf2_check_irq+0x60>
    d388:	e5943090 	ldr	r3, [r4, #144]
    d38c:	e3c55802 	bic	r5, r5, #131072	; 0x20000
    d390:	e3c33802 	bic	r3, r3, #131072	; 0x20000
    d394:	e5843090 	str	r3, [r4, #144]
    d398:	e3a03802 	mov	r3, #131072	; 0x20000
    d39c:	e584308c 	str	r3, [r4, #140]
    d3a0:	ea000069 	b	d54c <nf2_check_irq+0x208>
    d3a4:	e59f21ac 	ldr	r2, [pc, #428]	; d558 <nf2_check_irq+0x214>
    d3a8:	e59f31a8 	ldr	r3, [pc, #424]	; d558 <nf2_check_irq+0x214>
    d3ac:	e0052002 	and	r2, r5, r2
    d3b0:	e1520003 	cmp	r2, r3
    d3b4:	1a000006 	bne	d3d4 <nf2_check_irq+0x90>
    d3b8:	e5943090 	ldr	r3, [r4, #144]
    d3bc:	e3c55702 	bic	r5, r5, #524288	; 0x80000
    d3c0:	e3c33702 	bic	r3, r3, #524288	; 0x80000
    d3c4:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    d3c8:	e5843090 	str	r3, [r4, #144]
    d3cc:	e584208c 	str	r2, [r4, #140]
    d3d0:	ea000006 	b	d3f0 <nf2_check_irq+0xac>
    d3d4:	e3150b01 	tst	r5, #1024	; 0x400
    d3d8:	0a000006 	beq	d3f8 <nf2_check_irq+0xb4>
    d3dc:	e5943090 	ldr	r3, [r4, #144]
    d3e0:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    d3e4:	e5843090 	str	r3, [r4, #144]
    d3e8:	e3a03b01 	mov	r3, #1024	; 0x400
    d3ec:	e584308c 	str	r3, [r4, #140]
    d3f0:	e3c55b01 	bic	r5, r5, #1024	; 0x400
    d3f4:	ea000054 	b	d54c <nf2_check_irq+0x208>
    d3f8:	e3150a01 	tst	r5, #4096	; 0x1000
    d3fc:	0a000006 	beq	d41c <nf2_check_irq+0xd8>
    d400:	e59f3154 	ldr	r3, [pc, #340]	; d55c <nf2_check_irq+0x218>
    d404:	e59f0154 	ldr	r0, [pc, #340]	; d560 <nf2_check_irq+0x21c>
    d408:	e1a0e00f 	mov	lr, pc
    d40c:	e593f000 	ldr	pc, [r3]
    d410:	e3a03a01 	mov	r3, #4096	; 0x1000
    d414:	e5843090 	str	r3, [r4, #144]
    d418:	ea00004b 	b	d54c <nf2_check_irq+0x208>
    d41c:	e3150001 	tst	r5, #1	; 0x1
    d420:	13a0300f 	movne	r3, #15	; 0xf
    d424:	1584508c 	strne	r5, [r4, #140]
    d428:	1584304c 	strne	r3, [r4, #76]
    d42c:	1a000046 	bne	d54c <nf2_check_irq+0x208>
    d430:	e3150002 	tst	r5, #2	; 0x2
    d434:	13a03002 	movne	r3, #2	; 0x2
    d438:	1584308c 	strne	r3, [r4, #140]
    d43c:	13a03001 	movne	r3, #1	; 0x1
    d440:	15843088 	strne	r3, [r4, #136]
    d444:	1a000040 	bne	d54c <nf2_check_irq+0x208>
    d448:	e3150902 	tst	r5, #32768	; 0x8000
    d44c:	13a03902 	movne	r3, #32768	; 0x8000
    d450:	1584308c 	strne	r3, [r4, #140]
    d454:	13c55902 	bicne	r5, r5, #32768	; 0x8000
    d458:	1a00003b 	bne	d54c <nf2_check_irq+0x208>
    d45c:	e3150c01 	tst	r5, #256	; 0x100
    d460:	13a03c01 	movne	r3, #256	; 0x100
    d464:	1584308c 	strne	r3, [r4, #140]
    d468:	13c55c01 	bicne	r5, r5, #256	; 0x100
    d46c:	1a000036 	bne	d54c <nf2_check_irq+0x208>
    d470:	e3150c02 	tst	r5, #512	; 0x200
    d474:	0a000007 	beq	d498 <nf2_check_irq+0x154>
    d478:	e3a02c02 	mov	r2, #512	; 0x200
    d47c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d480:	e583208c 	str	r2, [r3, #140]
    d484:	e3a02039 	mov	r2, #57	; 0x39
    d488:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    d48c:	e5032f77 	str	r2, [r3, #-3959]
    d490:	e3c55c02 	bic	r5, r5, #512	; 0x200
    d494:	ea00002c 	b	d54c <nf2_check_irq+0x208>
    d498:	e3150010 	tst	r5, #16	; 0x10
    d49c:	0a000008 	beq	d4c4 <nf2_check_irq+0x180>
    d4a0:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d4a4:	e3a03010 	mov	r3, #16	; 0x10
    d4a8:	e582308c 	str	r3, [r2, #140]
    d4ac:	e592309c 	ldr	r3, [r2, #156]
    d4b0:	e3c55010 	bic	r5, r5, #16	; 0x10
    d4b4:	e5823034 	str	r3, [r2, #52]
    d4b8:	e3a03002 	mov	r3, #2	; 0x2
    d4bc:	e5823088 	str	r3, [r2, #136]
    d4c0:	ea000021 	b	d54c <nf2_check_irq+0x208>
    d4c4:	e3150080 	tst	r5, #128	; 0x80
    d4c8:	0a00000b 	beq	d4fc <nf2_check_irq+0x1b8>
    d4cc:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d4d0:	e3a03080 	mov	r3, #128	; 0x80
    d4d4:	e584308c 	str	r3, [r4, #140]
    d4d8:	e594309c 	ldr	r3, [r4, #156]
    d4dc:	e3a00008 	mov	r0, #8	; 0x8
    d4e0:	e0833000 	add	r3, r3, r0
    d4e4:	e5843034 	str	r3, [r4, #52]
    d4e8:	ebffff7d 	bl	d2e4 <nf2_soft_reset>
    d4ec:	e3a03002 	mov	r3, #2	; 0x2
    d4f0:	e5843088 	str	r3, [r4, #136]
    d4f4:	e3c55080 	bic	r5, r5, #128	; 0x80
    d4f8:	ea000013 	b	d54c <nf2_check_irq+0x208>
    d4fc:	e3150040 	tst	r5, #64	; 0x40
    d500:	0a000009 	beq	d52c <nf2_check_irq+0x1e8>
    d504:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d508:	e3a03040 	mov	r3, #64	; 0x40
    d50c:	e582308c 	str	r3, [r2, #140]
    d510:	e3a03004 	mov	r3, #4	; 0x4
    d514:	e5823088 	str	r3, [r2, #136]
    d518:	e3a02038 	mov	r2, #56	; 0x38
    d51c:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    d520:	e5032f77 	str	r2, [r3, #-3959]
    d524:	e3c55040 	bic	r5, r5, #64	; 0x40
    d528:	ea000007 	b	d54c <nf2_check_irq+0x208>
    d52c:	e3150801 	tst	r5, #65536	; 0x10000
    d530:	0a000005 	beq	d54c <nf2_check_irq+0x208>
    d534:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d538:	e3a02801 	mov	r2, #65536	; 0x10000
    d53c:	e583208c 	str	r2, [r3, #140]
    d540:	e3a02008 	mov	r2, #8	; 0x8
    d544:	e5832088 	str	r2, [r3, #136]
    d548:	e3c55801 	bic	r5, r5, #65536	; 0x10000
    d54c:	e1a00005 	mov	r0, r5
    d550:	e28dd004 	add	sp, sp, #4	; 0x4
    d554:	e8bd8030 	pop	{r4, r5, pc}
    d558:	00080400 	.word	0x00080400
    d55c:	3c00a5cc 	.word	0x3c00a5cc
    d560:	00019077 	.word	0x00019077

0000d564 <nf2_erase>:
    d564:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d568:	e3a00001 	mov	r0, #1	; 0x1
    d56c:	e24dd008 	sub	sp, sp, #8	; 0x8
    d570:	e1a06001 	mov	r6, r1
    d574:	e1a05002 	mov	r5, r2
    d578:	ebffff59 	bl	d2e4 <nf2_soft_reset>
    d57c:	e2504000 	subs	r4, r0, #0	; 0x0
    d580:	13e04000 	mvnne	r4, #0	; 0x0
    d584:	1a000035 	bne	d660 <nf2_erase+0xfc>
    d588:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d58c:	e5826020 	str	r6, [r2, #32]
    d590:	e58d4004 	str	r4, [sp, #4]
    d594:	e5923054 	ldr	r3, [r2, #84]
    d598:	e1a0e002 	mov	lr, r2
    d59c:	e58d3004 	str	r3, [sp, #4]
    d5a0:	e59d3004 	ldr	r3, [sp, #4]
    d5a4:	e2450002 	sub	r0, r5, #2	; 0x2
    d5a8:	e3833001 	orr	r3, r3, #1	; 0x1
    d5ac:	e58d3004 	str	r3, [sp, #4]
    d5b0:	e59d3004 	ldr	r3, [sp, #4]
    d5b4:	e3a0c0ff 	mov	ip, #255	; 0xff
    d5b8:	e5823054 	str	r3, [r2, #84]
    d5bc:	e3043060 	movw	r3, #16480	; 0x4060
    d5c0:	e5823060 	str	r3, [r2, #96]
    d5c4:	e1a02004 	mov	r2, r4
    d5c8:	ea000003 	b	d5dc <nf2_erase+0x78>
    d5cc:	e006311c 	and	r3, r6, ip, lsl r1
    d5d0:	e1a03153 	asr	r3, r3, r1
    d5d4:	e3833c41 	orr	r3, r3, #16640	; 0x4100
    d5d8:	e58e3060 	str	r3, [lr, #96]
    d5dc:	e1520000 	cmp	r2, r0
    d5e0:	e1a01182 	lsl	r1, r2, #3
    d5e4:	e2822001 	add	r2, r2, #1	; 0x1
    d5e8:	bafffff7 	blt	d5cc <nf2_erase+0x68>
    d5ec:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d5f0:	e30438d0 	movw	r3, #18640	; 0x48d0
    d5f4:	e5823060 	str	r3, [r2, #96]
    d5f8:	e3a03000 	mov	r3, #0	; 0x0
    d5fc:	e5823054 	str	r3, [r2, #84]
    d600:	e3e03001 	mvn	r3, #1	; 0x1
    d604:	e582304c 	str	r3, [r2, #76]
    d608:	e2833003 	add	r3, r3, #3	; 0x3
    d60c:	e5823084 	str	r3, [r2, #132]
    d610:	e1a07002 	mov	r7, r2
    d614:	eb00040c 	bl	e64c <get_ticks>
    d618:	e59f804c 	ldr	r8, [pc, #76]	; d66c <nf2_erase+0x108>
    d61c:	e1a05000 	mov	r5, r0
    d620:	e3006801 	movw	r6, #2049	; 0x801
    d624:	eb000408 	bl	e64c <get_ticks>
    d628:	e0650000 	rsb	r0, r5, r0
    d62c:	e1500008 	cmp	r0, r8
    d630:	e3002801 	movw	r2, #2049	; 0x801
    d634:	c3e04000 	mvngt	r4, #0	; 0x0
    d638:	ca000003 	bgt	d64c <nf2_erase+0xe8>
    d63c:	e597308c 	ldr	r3, [r7, #140]
    d640:	e0032002 	and	r2, r3, r2
    d644:	e1520006 	cmp	r2, r6
    d648:	1afffff5 	bne	d624 <nf2_erase+0xc0>
    d64c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d650:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    d654:	e583208c 	str	r2, [r3, #140]
    d658:	e3a0200f 	mov	r2, #15	; 0xf
    d65c:	e583204c 	str	r2, [r3, #76]
    d660:	e1a00004 	mov	r0, r4
    d664:	e28dd008 	add	sp, sp, #8	; 0x8
    d668:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d66c:	0007a120 	.word	0x0007a120

0000d670 <nf2_page_op>:
    d670:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d674:	e24dd0b4 	sub	sp, sp, #180	; 0xb4
    d678:	e59d9120 	ldr	r9, [sp, #288]
    d67c:	e59da124 	ldr	sl, [sp, #292]
    d680:	e3a0c000 	mov	ip, #0	; 0x0
    d684:	e589c000 	str	ip, [r9]
    d688:	e58d0074 	str	r0, [sp, #116]
    d68c:	e58ac000 	str	ip, [sl]
    d690:	e3a00001 	mov	r0, #1	; 0x1
    d694:	e58dc0ac 	str	ip, [sp, #172]
    d698:	e58d1070 	str	r1, [sp, #112]
    d69c:	e58d206c 	str	r2, [sp, #108]
    d6a0:	e58d3068 	str	r3, [sp, #104]
    d6a4:	e59db0f0 	ldr	fp, [sp, #240]
    d6a8:	e59d5104 	ldr	r5, [sp, #260]
    d6ac:	e59d8108 	ldr	r8, [sp, #264]
    d6b0:	e59d710c 	ldr	r7, [sp, #268]
    d6b4:	e59d4110 	ldr	r4, [sp, #272]
    d6b8:	e59da118 	ldr	sl, [sp, #280]
    d6bc:	e59d911c 	ldr	r9, [sp, #284]
    d6c0:	ebffff07 	bl	d2e4 <nf2_soft_reset>
    d6c4:	e3500000 	cmp	r0, #0	; 0x0
    d6c8:	13e0c000 	mvnne	ip, #0	; 0x0
    d6cc:	e58d009c 	str	r0, [sp, #156]
    d6d0:	158dc09c 	strne	ip, [sp, #156]
    d6d4:	1a000208 	bne	defc <nf2_page_op+0x88c>
    d6d8:	e59d30dc 	ldr	r3, [sp, #220]
    d6dc:	e3550001 	cmp	r5, #1	; 0x1
    d6e0:	e2833004 	add	r3, r3, #4	; 0x4
    d6e4:	159de09c 	ldrne	lr, [sp, #156]
    d6e8:	e59d1070 	ldr	r1, [sp, #112]
    d6ec:	e58d3098 	str	r3, [sp, #152]
    d6f0:	03a0330a 	moveq	r3, #671088640	; 0x28000000
    d6f4:	13a0330a 	movne	r3, #671088640	; 0x28000000
    d6f8:	05835164 	streq	r5, [r3, #356]
    d6fc:	1583e164 	strne	lr, [r3, #356]
    d700:	e3510000 	cmp	r1, #0	; 0x0
    d704:	1a000042 	bne	d814 <nf2_page_op+0x1a4>
    d708:	e59d2074 	ldr	r2, [sp, #116]
    d70c:	e3520001 	cmp	r2, #1	; 0x1
    d710:	1a00002f 	bne	d7d4 <nf2_page_op+0x164>
    d714:	e3550001 	cmp	r5, #1	; 0x1
    d718:	1a000018 	bne	d780 <nf2_page_op+0x110>
    d71c:	e5982000 	ldr	r2, [r8]
    d720:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d724:	e5832140 	str	r2, [r3, #320]
    d728:	e5982004 	ldr	r2, [r8, #4]
    d72c:	e1d4c0bc 	ldrh	ip, [r4, #12]
    d730:	e5832144 	str	r2, [r3, #324]
    d734:	e5982008 	ldr	r2, [r8, #8]
    d738:	e1d470b2 	ldrh	r7, [r4, #2]
    d73c:	e5832148 	str	r2, [r3, #328]
    d740:	e1d400b0 	ldrh	r0, [r4]
    d744:	e1d460b6 	ldrh	r6, [r4, #6]
    d748:	e1d410b4 	ldrh	r1, [r4, #4]
    d74c:	e1d450ba 	ldrh	r5, [r4, #10]
    d750:	e1d420b8 	ldrh	r2, [r4, #8]
    d754:	e1d4e0be 	ldrh	lr, [r4, #14]
    d758:	e598400c 	ldr	r4, [r8, #12]
    d75c:	e1800807 	orr	r0, r0, r7, lsl #16
    d760:	e1811806 	orr	r1, r1, r6, lsl #16
    d764:	e1822805 	orr	r2, r2, r5, lsl #16
    d768:	e18cc80e 	orr	ip, ip, lr, lsl #16
    d76c:	e583414c 	str	r4, [r3, #332]
    d770:	e5830150 	str	r0, [r3, #336]
    d774:	e5831154 	str	r1, [r3, #340]
    d778:	e5832158 	str	r2, [r3, #344]
    d77c:	e583c15c 	str	ip, [r3, #348]
    d780:	e59d006c 	ldr	r0, [sp, #108]
    d784:	e59d10d8 	ldr	r1, [sp, #216]
    d788:	e59d20f8 	ldr	r2, [sp, #248]
    d78c:	ebfffe07 	bl	cfb0 <nf2_ecc_num>
    d790:	e59d9098 	ldr	r9, [sp, #152]
    d794:	e59da0d8 	ldr	sl, [sp, #216]
    d798:	e35b0001 	cmp	fp, #1	; 0x1
    d79c:	e089300a 	add	r3, r9, sl
    d7a0:	11a04003 	movne	r4, r3
    d7a4:	11a0c000 	movne	ip, r0
    d7a8:	1a000005 	bne	d7c4 <nf2_page_op+0x154>
    d7ac:	e59dc0f4 	ldr	ip, [sp, #244]
    d7b0:	e1a04003 	mov	r4, r3
    d7b4:	e35c0000 	cmp	ip, #0	; 0x0
    d7b8:	03a03008 	moveq	r3, #8	; 0x8
    d7bc:	13a0300c 	movne	r3, #12	; 0xc
    d7c0:	e083c000 	add	ip, r3, r0
    d7c4:	e1a0e004 	mov	lr, r4
    d7c8:	e1a06004 	mov	r6, r4
    d7cc:	e3a05000 	mov	r5, #0	; 0x0
    d7d0:	ea0000ec 	b	db88 <nf2_page_op+0x518>
    d7d4:	e3550001 	cmp	r5, #1	; 0x1
    d7d8:	1a000008 	bne	d800 <nf2_page_op+0x190>
    d7dc:	e5983000 	ldr	r3, [r8]
    d7e0:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d7e4:	e5823140 	str	r3, [r2, #320]
    d7e8:	e5983004 	ldr	r3, [r8, #4]
    d7ec:	e5823144 	str	r3, [r2, #324]
    d7f0:	e5983008 	ldr	r3, [r8, #8]
    d7f4:	e5823148 	str	r3, [r2, #328]
    d7f8:	e598300c 	ldr	r3, [r8, #12]
    d7fc:	e582314c 	str	r3, [r2, #332]
    d800:	e59de098 	ldr	lr, [sp, #152]
    d804:	e59d10d8 	ldr	r1, [sp, #216]
    d808:	e3a0b000 	mov	fp, #0	; 0x0
    d80c:	e08e4001 	add	r4, lr, r1
    d810:	ea000030 	b	d8d8 <nf2_page_op+0x268>
    d814:	e59d30fc 	ldr	r3, [sp, #252]
    d818:	e3530001 	cmp	r3, #1	; 0x1
    d81c:	1a00003f 	bne	d920 <nf2_page_op+0x2b0>
    d820:	e59d2074 	ldr	r2, [sp, #116]
    d824:	e3520001 	cmp	r2, #1	; 0x1
    d828:	1a000026 	bne	d8c8 <nf2_page_op+0x258>
    d82c:	e3550001 	cmp	r5, #1	; 0x1
    d830:	1a000018 	bne	d898 <nf2_page_op+0x228>
    d834:	e5982000 	ldr	r2, [r8]
    d838:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d83c:	e5832140 	str	r2, [r3, #320]
    d840:	e5982004 	ldr	r2, [r8, #4]
    d844:	e1d4c0bc 	ldrh	ip, [r4, #12]
    d848:	e5832144 	str	r2, [r3, #324]
    d84c:	e5982008 	ldr	r2, [r8, #8]
    d850:	e1d470b2 	ldrh	r7, [r4, #2]
    d854:	e5832148 	str	r2, [r3, #328]
    d858:	e1d400b0 	ldrh	r0, [r4]
    d85c:	e1d460b6 	ldrh	r6, [r4, #6]
    d860:	e1d410b4 	ldrh	r1, [r4, #4]
    d864:	e1d450ba 	ldrh	r5, [r4, #10]
    d868:	e1d420b8 	ldrh	r2, [r4, #8]
    d86c:	e1d4e0be 	ldrh	lr, [r4, #14]
    d870:	e598400c 	ldr	r4, [r8, #12]
    d874:	e1800807 	orr	r0, r0, r7, lsl #16
    d878:	e1811806 	orr	r1, r1, r6, lsl #16
    d87c:	e1822805 	orr	r2, r2, r5, lsl #16
    d880:	e18cc80e 	orr	ip, ip, lr, lsl #16
    d884:	e583414c 	str	r4, [r3, #332]
    d888:	e5830150 	str	r0, [r3, #336]
    d88c:	e5831154 	str	r1, [r3, #340]
    d890:	e5832158 	str	r2, [r3, #344]
    d894:	e583c15c 	str	ip, [r3, #348]
    d898:	e59d006c 	ldr	r0, [sp, #108]
    d89c:	e59d10d8 	ldr	r1, [sp, #216]
    d8a0:	e59d20f8 	ldr	r2, [sp, #248]
    d8a4:	ebfffdc1 	bl	cfb0 <nf2_ecc_num>
    d8a8:	e59d90d8 	ldr	r9, [sp, #216]
    d8ac:	e59d3098 	ldr	r3, [sp, #152]
    d8b0:	e3a0b000 	mov	fp, #0	; 0x0
    d8b4:	e1a0c000 	mov	ip, r0
    d8b8:	e0836009 	add	r6, r3, r9
    d8bc:	e1a04009 	mov	r4, r9
    d8c0:	e1a0e009 	mov	lr, r9
    d8c4:	ea000013 	b	d918 <nf2_page_op+0x2a8>
    d8c8:	e3550001 	cmp	r5, #1	; 0x1
    d8cc:	0a000003 	beq	d8e0 <nf2_page_op+0x270>
    d8d0:	e59d40d8 	ldr	r4, [sp, #216]
    d8d4:	e3a0b000 	mov	fp, #0	; 0x0
    d8d8:	e1a0e004 	mov	lr, r4
    d8dc:	ea00000b 	b	d910 <nf2_page_op+0x2a0>
    d8e0:	e5983000 	ldr	r3, [r8]
    d8e4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d8e8:	e5823140 	str	r3, [r2, #320]
    d8ec:	e5983004 	ldr	r3, [r8, #4]
    d8f0:	e3a0b000 	mov	fp, #0	; 0x0
    d8f4:	e5823144 	str	r3, [r2, #324]
    d8f8:	e5983008 	ldr	r3, [r8, #8]
    d8fc:	e59d40d8 	ldr	r4, [sp, #216]
    d900:	e5823148 	str	r3, [r2, #328]
    d904:	e598300c 	ldr	r3, [r8, #12]
    d908:	e59de0d8 	ldr	lr, [sp, #216]
    d90c:	e582314c 	str	r3, [r2, #332]
    d910:	e1a0c00b 	mov	ip, fp
    d914:	e1a0600b 	mov	r6, fp
    d918:	e1a0500b 	mov	r5, fp
    d91c:	ea000099 	b	db88 <nf2_page_op+0x518>
    d920:	e59dc074 	ldr	ip, [sp, #116]
    d924:	e35c0001 	cmp	ip, #1	; 0x1
    d928:	1a00007b 	bne	db1c <nf2_page_op+0x4ac>
    d92c:	e59d10d8 	ldr	r1, [sp, #216]
    d930:	e59d006c 	ldr	r0, [sp, #108]
    d934:	e59d20f8 	ldr	r2, [sp, #248]
    d938:	ebfffd9c 	bl	cfb0 <nf2_ecc_num>
    d93c:	e59de098 	ldr	lr, [sp, #152]
    d940:	e59d10d8 	ldr	r1, [sp, #216]
    d944:	e35b0001 	cmp	fp, #1	; 0x1
    d948:	e08ee001 	add	lr, lr, r1
    d94c:	e58d00a0 	str	r0, [sp, #160]
    d950:	e58de0a4 	str	lr, [sp, #164]
    d954:	1a000037 	bne	da38 <nf2_page_op+0x3c8>
    d958:	e3550001 	cmp	r5, #1	; 0x1
    d95c:	1a00002a 	bne	da0c <nf2_page_op+0x39c>
    d960:	e1d790b6 	ldrh	r9, [r7, #6]
    d964:	e1d730b2 	ldrh	r3, [r7, #2]
    d968:	e58d9060 	str	r9, [sp, #96]
    d96c:	e59d9114 	ldr	r9, [sp, #276]
    d970:	e59d2114 	ldr	r2, [sp, #276]
    d974:	e58d3064 	str	r3, [sp, #100]
    d978:	e1d7a0ba 	ldrh	sl, [r7, #10]
    d97c:	e1d930be 	ldrh	r3, [r9, #14]
    d980:	e1d740b0 	ldrh	r4, [r7]
    d984:	e59d9064 	ldr	r9, [sp, #100]
    d988:	e1d210b2 	ldrh	r1, [r2, #2]
    d98c:	e1d750bc 	ldrh	r5, [r7, #12]
    d990:	e1d7e0b4 	ldrh	lr, [r7, #4]
    d994:	e58da05c 	str	sl, [sp, #92]
    d998:	e1844809 	orr	r4, r4, r9, lsl #16
    d99c:	e1d7a0be 	ldrh	sl, [r7, #14]
    d9a0:	e59d9060 	ldr	r9, [sp, #96]
    d9a4:	e58d1058 	str	r1, [sp, #88]
    d9a8:	e1d260bc 	ldrh	r6, [r2, #12]
    d9ac:	e1d7c0b8 	ldrh	ip, [r7, #8]
    d9b0:	e1d200b0 	ldrh	r0, [r2]
    d9b4:	e1d280b6 	ldrh	r8, [r2, #6]
    d9b8:	e1d210b4 	ldrh	r1, [r2, #4]
    d9bc:	e1d270ba 	ldrh	r7, [r2, #10]
    d9c0:	e18ee809 	orr	lr, lr, r9, lsl #16
    d9c4:	e1d220b8 	ldrh	r2, [r2, #8]
    d9c8:	e59d905c 	ldr	r9, [sp, #92]
    d9cc:	e185580a 	orr	r5, r5, sl, lsl #16
    d9d0:	e59da058 	ldr	sl, [sp, #88]
    d9d4:	e1866803 	orr	r6, r6, r3, lsl #16
    d9d8:	e18cc809 	orr	ip, ip, r9, lsl #16
    d9dc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d9e0:	e180080a 	orr	r0, r0, sl, lsl #16
    d9e4:	e1811808 	orr	r1, r1, r8, lsl #16
    d9e8:	e1822807 	orr	r2, r2, r7, lsl #16
    d9ec:	e5834140 	str	r4, [r3, #320]
    d9f0:	e583e144 	str	lr, [r3, #324]
    d9f4:	e583c148 	str	ip, [r3, #328]
    d9f8:	e583514c 	str	r5, [r3, #332]
    d9fc:	e5830150 	str	r0, [r3, #336]
    da00:	e5831154 	str	r1, [r3, #340]
    da04:	e5832158 	str	r2, [r3, #344]
    da08:	e583615c 	str	r6, [r3, #348]
    da0c:	e59d10f4 	ldr	r1, [sp, #244]
    da10:	e59dc0a4 	ldr	ip, [sp, #164]
    da14:	e59de0a0 	ldr	lr, [sp, #160]
    da18:	e59d4098 	ldr	r4, [sp, #152]
    da1c:	e3510000 	cmp	r1, #0	; 0x0
    da20:	e08c600e 	add	r6, ip, lr
    da24:	e59d50d8 	ldr	r5, [sp, #216]
    da28:	03a0c008 	moveq	ip, #8	; 0x8
    da2c:	13a0c00c 	movne	ip, #12	; 0xc
    da30:	e1a0e004 	mov	lr, r4
    da34:	ea000053 	b	db88 <nf2_page_op+0x518>
    da38:	e3550001 	cmp	r5, #1	; 0x1
    da3c:	1a000028 	bne	dae4 <nf2_page_op+0x474>
    da40:	e1da20b2 	ldrh	r2, [sl, #2]
    da44:	e1da30b6 	ldrh	r3, [sl, #6]
    da48:	e1d910b2 	ldrh	r1, [r9, #2]
    da4c:	e58d2054 	str	r2, [sp, #84]
    da50:	e1dac0ba 	ldrh	ip, [sl, #10]
    da54:	e1d960bc 	ldrh	r6, [r9, #12]
    da58:	e1da40b0 	ldrh	r4, [sl]
    da5c:	e58d3050 	str	r3, [sp, #80]
    da60:	e58d1048 	str	r1, [sp, #72]
    da64:	e1d930be 	ldrh	r3, [r9, #14]
    da68:	e1d900b0 	ldrh	r0, [r9]
    da6c:	e1d980b6 	ldrh	r8, [r9, #6]
    da70:	e1d910b4 	ldrh	r1, [r9, #4]
    da74:	e1d970ba 	ldrh	r7, [r9, #10]
    da78:	e1d920b8 	ldrh	r2, [r9, #8]
    da7c:	e59d9054 	ldr	r9, [sp, #84]
    da80:	e1da50bc 	ldrh	r5, [sl, #12]
    da84:	e1dae0b4 	ldrh	lr, [sl, #4]
    da88:	e58dc04c 	str	ip, [sp, #76]
    da8c:	e1844809 	orr	r4, r4, r9, lsl #16
    da90:	e1dac0b8 	ldrh	ip, [sl, #8]
    da94:	e59d9050 	ldr	r9, [sp, #80]
    da98:	e1daa0be 	ldrh	sl, [sl, #14]
    da9c:	e18ee809 	orr	lr, lr, r9, lsl #16
    daa0:	e185580a 	orr	r5, r5, sl, lsl #16
    daa4:	e59d904c 	ldr	r9, [sp, #76]
    daa8:	e59da048 	ldr	sl, [sp, #72]
    daac:	e1866803 	orr	r6, r6, r3, lsl #16
    dab0:	e18cc809 	orr	ip, ip, r9, lsl #16
    dab4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    dab8:	e180080a 	orr	r0, r0, sl, lsl #16
    dabc:	e1811808 	orr	r1, r1, r8, lsl #16
    dac0:	e1822807 	orr	r2, r2, r7, lsl #16
    dac4:	e5834140 	str	r4, [r3, #320]
    dac8:	e583e144 	str	lr, [r3, #324]
    dacc:	e583c148 	str	ip, [r3, #328]
    dad0:	e583514c 	str	r5, [r3, #332]
    dad4:	e5830150 	str	r0, [r3, #336]
    dad8:	e5831154 	str	r1, [r3, #340]
    dadc:	e5832158 	str	r2, [r3, #344]
    dae0:	e583615c 	str	r6, [r3, #348]
    dae4:	e59dc0d8 	ldr	ip, [sp, #216]
    dae8:	e59d00a0 	ldr	r0, [sp, #160]
    daec:	e1a0454c 	asr	r4, ip, #10
    daf0:	e1a01004 	mov	r1, r4
    daf4:	eb001d24 	bl	14f8c <__aeabi_idiv>
    daf8:	e59de0a4 	ldr	lr, [sp, #164]
    dafc:	e2444001 	sub	r4, r4, #1	; 0x1
    db00:	e026e490 	mla	r6, r0, r4, lr
    db04:	e59d10d8 	ldr	r1, [sp, #216]
    db08:	e59d4098 	ldr	r4, [sp, #152]
    db0c:	e1a0c000 	mov	ip, r0
    db10:	e284eb01 	add	lr, r4, #1024	; 0x400
    db14:	e2415b01 	sub	r5, r1, #1024	; 0x400
    db18:	ea00001a 	b	db88 <nf2_page_op+0x518>
    db1c:	e3550001 	cmp	r5, #1	; 0x1
    db20:	159d4098 	ldrne	r4, [sp, #152]
    db24:	13a0b000 	movne	fp, #0	; 0x0
    db28:	1a000012 	bne	db78 <nf2_page_op+0x508>
    db2c:	e1d740b6 	ldrh	r4, [r7, #6]
    db30:	e1d730be 	ldrh	r3, [r7, #14]
    db34:	e1d7c0bc 	ldrh	ip, [r7, #12]
    db38:	e1d750b2 	ldrh	r5, [r7, #2]
    db3c:	e1d700b0 	ldrh	r0, [r7]
    db40:	e1d710b4 	ldrh	r1, [r7, #4]
    db44:	e1d7e0ba 	ldrh	lr, [r7, #10]
    db48:	e1d720b8 	ldrh	r2, [r7, #8]
    db4c:	e1811804 	orr	r1, r1, r4, lsl #16
    db50:	e18cc803 	orr	ip, ip, r3, lsl #16
    db54:	e1800805 	orr	r0, r0, r5, lsl #16
    db58:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    db5c:	e182280e 	orr	r2, r2, lr, lsl #16
    db60:	e59d4098 	ldr	r4, [sp, #152]
    db64:	e5830140 	str	r0, [r3, #320]
    db68:	e3a0b000 	mov	fp, #0	; 0x0
    db6c:	e5831144 	str	r1, [r3, #324]
    db70:	e5832148 	str	r2, [r3, #328]
    db74:	e583c14c 	str	ip, [r3, #332]
    db78:	e59d50d8 	ldr	r5, [sp, #216]
    db7c:	e1a0e004 	mov	lr, r4
    db80:	e1a0c00b 	mov	ip, fp
    db84:	e1a0600b 	mov	r6, fp
    db88:	e59d2130 	ldr	r2, [sp, #304]
    db8c:	e59d90f8 	ldr	r9, [sp, #248]
    db90:	e3520001 	cmp	r2, #1	; 0x1
    db94:	059d312c 	ldreq	r3, [sp, #300]
    db98:	e59da0e0 	ldr	sl, [sp, #224]
    db9c:	00634004 	rsbeq	r4, r3, r4
    dba0:	e59d1070 	ldr	r1, [sp, #112]
    dba4:	e3a03000 	mov	r3, #0	; 0x0
    dba8:	e59d206c 	ldr	r2, [sp, #108]
    dbac:	e59d0074 	ldr	r0, [sp, #116]
    dbb0:	e88d5000 	stm	sp, {ip, lr}
    dbb4:	e58d900c 	str	r9, [sp, #12]
    dbb8:	e58da008 	str	sl, [sp, #8]
    dbbc:	ebfffce5 	bl	cf58 <nf2_ecc_cfg>
    dbc0:	e1a0000b 	mov	r0, fp
    dbc4:	e59d10f4 	ldr	r1, [sp, #244]
    dbc8:	e59d2068 	ldr	r2, [sp, #104]
    dbcc:	ebfffcc2 	bl	cedc <nf2_secc_cfg>
    dbd0:	e59d3134 	ldr	r3, [sp, #308]
    dbd4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    dbd8:	e3530010 	cmp	r3, #16	; 0x10
    dbdc:	e59d30e8 	ldr	r3, [sp, #232]
    dbe0:	e59dc130 	ldr	ip, [sp, #304]
    dbe4:	e59de12c 	ldr	lr, [sp, #300]
    dbe8:	e5823038 	str	r3, [r2, #56]
    dbec:	e59d3128 	ldr	r3, [sp, #296]
    dbf0:	e1a0110c 	lsl	r1, ip, #2
    dbf4:	e582303c 	str	r3, [r2, #60]
    dbf8:	e184080e 	orr	r0, r4, lr, lsl #16
    dbfc:	e3811001 	orr	r1, r1, #1	; 0x1
    dc00:	e3a03341 	mov	r3, #67108865	; 0x4000001
    dc04:	e5820040 	str	r0, [r2, #64]
    dc08:	e5823044 	str	r3, [r2, #68]
    dc0c:	e5821048 	str	r1, [r2, #72]
    dc10:	e59d20e4 	ldr	r2, [sp, #228]
    dc14:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    dc18:	e59d10e0 	ldr	r1, [sp, #224]
    dc1c:	e5842020 	str	r2, [r4, #32]
    dc20:	e59d2100 	ldr	r2, [sp, #256]
    dc24:	01a050c5 	asreq	r5, r5, #1
    dc28:	01a060c6 	asreq	r6, r6, #1
    dc2c:	e1853806 	orr	r3, r5, r6, lsl #16
    dc30:	e3510000 	cmp	r1, #0	; 0x0
    dc34:	e3a05000 	mov	r5, #0	; 0x0
    dc38:	e5845024 	str	r5, [r4, #36]
    dc3c:	e2422001 	sub	r2, r2, #1	; 0x1
    dc40:	e5845028 	str	r5, [r4, #40]
    dc44:	e584502c 	str	r5, [r4, #44]
    dc48:	e5843030 	str	r3, [r4, #48]
    dc4c:	1a000041 	bne	dd58 <nf2_page_op+0x6e8>
    dc50:	e382c008 	orr	ip, r2, #8	; 0x8
    dc54:	e3a03020 	mov	r3, #32	; 0x20
    dc58:	e20cc00f 	and	ip, ip, #15	; 0xf
    dc5c:	e59d20e0 	ldr	r2, [sp, #224]
    dc60:	e3a0e001 	mov	lr, #1	; 0x1
    dc64:	e5843090 	str	r3, [r4, #144]
    dc68:	e1a0c50c 	lsl	ip, ip, #10
    dc6c:	e3a03901 	mov	r3, #16384	; 0x4000
    dc70:	e584e054 	str	lr, [r4, #84]
    dc74:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    dc78:	e5843060 	str	r3, [r4, #96]
    dc7c:	e2833e43 	add	r3, r3, #1072	; 0x430
    dc80:	e584c060 	str	ip, [r4, #96]
    dc84:	e1a0100e 	mov	r1, lr
    dc88:	e5843060 	str	r3, [r4, #96]
    dc8c:	e3a00008 	mov	r0, #8	; 0x8
    dc90:	ebfffc0b 	bl	ccc4 <trx_afifo_reg>
    dc94:	e59d30e0 	ldr	r3, [sp, #224]
    dc98:	e59d2074 	ldr	r2, [sp, #116]
    dc9c:	e5843054 	str	r3, [r4, #84]
    dca0:	e1520005 	cmp	r2, r5
    dca4:	e3a0300e 	mov	r3, #14	; 0xe
    dca8:	e584304c 	str	r3, [r4, #76]
    dcac:	13a03007 	movne	r3, #7	; 0x7
    dcb0:	03a03005 	moveq	r3, #5	; 0x5
    dcb4:	e5843084 	str	r3, [r4, #132]
    dcb8:	eb000263 	bl	e64c <get_ticks>
    dcbc:	e59f5244 	ldr	r5, [pc, #580]	; df08 <nf2_page_op+0x898>
    dcc0:	e1a04000 	mov	r4, r0
    dcc4:	eb000260 	bl	e64c <get_ticks>
    dcc8:	e0640000 	rsb	r0, r4, r0
    dccc:	e1500005 	cmp	r0, r5
    dcd0:	da000006 	ble	dcf0 <nf2_page_op+0x680>
    dcd4:	e59f0230 	ldr	r0, [pc, #560]	; df0c <nf2_page_op+0x89c>
    dcd8:	e59f3230 	ldr	r3, [pc, #560]	; df10 <nf2_page_op+0x8a0>
    dcdc:	e3e09000 	mvn	r9, #0	; 0x0
    dce0:	e1a0e00f 	mov	lr, pc
    dce4:	e593f000 	ldr	pc, [r3]
    dce8:	e58d909c 	str	r9, [sp, #156]
    dcec:	ea000002 	b	dcfc <nf2_page_op+0x68c>
    dcf0:	ebfffd93 	bl	d344 <nf2_check_irq>
    dcf4:	e3500020 	cmp	r0, #32	; 0x20
    dcf8:	1afffff1 	bne	dcc4 <nf2_page_op+0x654>
    dcfc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    dd00:	e593208c 	ldr	r2, [r3, #140]
    dd04:	e59d00d8 	ldr	r0, [sp, #216]
    dd08:	e583208c 	str	r2, [r3, #140]
    dd0c:	e3a0200f 	mov	r2, #15	; 0xf
    dd10:	e583204c 	str	r2, [r3, #76]
    dd14:	e28d10ac 	add	r1, sp, #172	; 0xac
    dd18:	ebfffcf5 	bl	d0f4 <nf2_check_ecc>
    dd1c:	e3500000 	cmp	r0, #0	; 0x0
    dd20:	059da120 	ldreq	sl, [sp, #288]
    dd24:	e59dc120 	ldr	ip, [sp, #288]
    dd28:	059a3000 	ldreq	r3, [sl]
    dd2c:	13a030ff 	movne	r3, #255	; 0xff
    dd30:	e35b0001 	cmp	fp, #1	; 0x1
    dd34:	e58c3000 	str	r3, [ip]
    dd38:	1a00006f 	bne	defc <nf2_page_op+0x88c>
    dd3c:	e28d00ac 	add	r0, sp, #172	; 0xac
    dd40:	ebfffd36 	bl	d220 <nf2_check_secc>
    dd44:	e3500000 	cmp	r0, #0	; 0x0
    dd48:	159de124 	ldrne	lr, [sp, #292]
    dd4c:	13a030ff 	movne	r3, #255	; 0xff
    dd50:	158e3000 	strne	r3, [lr]
    dd54:	ea000068 	b	defc <nf2_page_op+0x88c>
    dd58:	e382c008 	orr	ip, r2, #8	; 0x8
    dd5c:	e3013001 	movw	r3, #4097	; 0x1001
    dd60:	e20cc00f 	and	ip, ip, #15	; 0xf
    dd64:	e5843090 	str	r3, [r4, #144]
    dd68:	e3a0e001 	mov	lr, #1	; 0x1
    dd6c:	e3043080 	movw	r3, #16512	; 0x4080
    dd70:	e1a0c50c 	lsl	ip, ip, #10
    dd74:	e584e054 	str	lr, [r4, #84]
    dd78:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    dd7c:	e5843060 	str	r3, [r4, #96]
    dd80:	e2433dd2 	sub	r3, r3, #13440	; 0x3480
    dd84:	e584c060 	str	ip, [r4, #96]
    dd88:	e5843060 	str	r3, [r4, #96]
    dd8c:	e3043810 	movw	r3, #18448	; 0x4810
    dd90:	e1a0100e 	mov	r1, lr
    dd94:	e1a02005 	mov	r2, r5
    dd98:	e5843060 	str	r3, [r4, #96]
    dd9c:	e3a00008 	mov	r0, #8	; 0x8
    dda0:	ebfffbc7 	bl	ccc4 <trx_afifo_reg>
    dda4:	e59d1074 	ldr	r1, [sp, #116]
    dda8:	e3a0300e 	mov	r3, #14	; 0xe
    ddac:	e3510000 	cmp	r1, #0	; 0x0
    ddb0:	e5845054 	str	r5, [r4, #84]
    ddb4:	e584304c 	str	r3, [r4, #76]
    ddb8:	13a03007 	movne	r3, #7	; 0x7
    ddbc:	03a03005 	moveq	r3, #5	; 0x5
    ddc0:	e5843084 	str	r3, [r4, #132]
    ddc4:	eb000220 	bl	e64c <get_ticks>
    ddc8:	e59f5138 	ldr	r5, [pc, #312]	; df08 <nf2_page_op+0x898>
    ddcc:	e1a04000 	mov	r4, r0
    ddd0:	eb00021d 	bl	e64c <get_ticks>
    ddd4:	e0640000 	rsb	r0, r4, r0
    ddd8:	e1500005 	cmp	r0, r5
    dddc:	da00003d 	ble	ded8 <nf2_page_op+0x868>
    dde0:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    dde4:	e590108c 	ldr	r1, [r0, #140]
    dde8:	e5902094 	ldr	r2, [r0, #148]
    ddec:	e59030a0 	ldr	r3, [r0, #160]
    ddf0:	e590c0a4 	ldr	ip, [r0, #164]
    ddf4:	e590e0a8 	ldr	lr, [r0, #168]
    ddf8:	e590405c 	ldr	r4, [r0, #92]
    ddfc:	e5905098 	ldr	r5, [r0, #152]
    de00:	e5906038 	ldr	r6, [r0, #56]
    de04:	e590703c 	ldr	r7, [r0, #60]
    de08:	e5908168 	ldr	r8, [r0, #360]
    de0c:	e590a16c 	ldr	sl, [r0, #364]
    de10:	e5909170 	ldr	r9, [r0, #368]
    de14:	e58d9078 	str	r9, [sp, #120]
    de18:	e5909174 	ldr	r9, [r0, #372]
    de1c:	e58d907c 	str	r9, [sp, #124]
    de20:	e5909178 	ldr	r9, [r0, #376]
    de24:	e58d9080 	str	r9, [sp, #128]
    de28:	e590917c 	ldr	r9, [r0, #380]
    de2c:	e58d9084 	str	r9, [sp, #132]
    de30:	e5909180 	ldr	r9, [r0, #384]
    de34:	e58d9088 	str	r9, [sp, #136]
    de38:	e5909184 	ldr	r9, [r0, #388]
    de3c:	e58d908c 	str	r9, [sp, #140]
    de40:	e5909188 	ldr	r9, [r0, #392]
    de44:	e58d9090 	str	r9, [sp, #144]
    de48:	e590918c 	ldr	r9, [r0, #396]
    de4c:	e590b190 	ldr	fp, [r0, #400]
    de50:	e58d9094 	str	r9, [sp, #148]
    de54:	e5900194 	ldr	r0, [r0, #404]
    de58:	e88d5000 	stm	sp, {ip, lr}
    de5c:	e58d4008 	str	r4, [sp, #8]
    de60:	e58d500c 	str	r5, [sp, #12]
    de64:	e58d6010 	str	r6, [sp, #16]
    de68:	e59dc07c 	ldr	ip, [sp, #124]
    de6c:	e59de080 	ldr	lr, [sp, #128]
    de70:	e59d9084 	ldr	r9, [sp, #132]
    de74:	e58da01c 	str	sl, [sp, #28]
    de78:	e59da078 	ldr	sl, [sp, #120]
    de7c:	e58dc024 	str	ip, [sp, #36]
    de80:	e58da020 	str	sl, [sp, #32]
    de84:	e59dc08c 	ldr	ip, [sp, #140]
    de88:	e59da088 	ldr	sl, [sp, #136]
    de8c:	e58de028 	str	lr, [sp, #40]
    de90:	e58d902c 	str	r9, [sp, #44]
    de94:	e59de090 	ldr	lr, [sp, #144]
    de98:	e59d9094 	ldr	r9, [sp, #148]
    de9c:	e58da030 	str	sl, [sp, #48]
    dea0:	e58dc034 	str	ip, [sp, #52]
    dea4:	e58d0044 	str	r0, [sp, #68]
    dea8:	e58d7014 	str	r7, [sp, #20]
    deac:	e58d8018 	str	r8, [sp, #24]
    deb0:	e58de038 	str	lr, [sp, #56]
    deb4:	e58d903c 	str	r9, [sp, #60]
    deb8:	e58db040 	str	fp, [sp, #64]
    debc:	e59f0050 	ldr	r0, [pc, #80]	; df14 <nf2_page_op+0x8a4>
    dec0:	e59fc048 	ldr	ip, [pc, #72]	; df10 <nf2_page_op+0x8a0>
    dec4:	e3e0a000 	mvn	sl, #0	; 0x0
    dec8:	e1a0e00f 	mov	lr, pc
    decc:	e59cf000 	ldr	pc, [ip]
    ded0:	e58da09c 	str	sl, [sp, #156]
    ded4:	ea000002 	b	dee4 <nf2_page_op+0x874>
    ded8:	ebfffd19 	bl	d344 <nf2_check_irq>
    dedc:	e3500001 	cmp	r0, #1	; 0x1
    dee0:	1affffba 	bne	ddd0 <nf2_page_op+0x760>
    dee4:	ebfffd16 	bl	d344 <nf2_check_irq>
    dee8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    deec:	e593208c 	ldr	r2, [r3, #140]
    def0:	e583208c 	str	r2, [r3, #140]
    def4:	e3a0200f 	mov	r2, #15	; 0xf
    def8:	e583204c 	str	r2, [r3, #76]
    defc:	e59d009c 	ldr	r0, [sp, #156]
    df00:	e28dd0b4 	add	sp, sp, #180	; 0xb4
    df04:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    df08:	0007a120 	.word	0x0007a120
    df0c:	0001908a 	.word	0x0001908a
    df10:	3c00a5cc 	.word	0x3c00a5cc
    df14:	00019098 	.word	0x00019098

0000df18 <nf2_randc_seed_hw>:
    df18:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    df1c:	e1801801 	orr	r1, r0, r1, lsl #16
    df20:	e3a03001 	mov	r3, #1	; 0x1
    df24:	e58211c4 	str	r1, [r2, #452]
    df28:	e58231c8 	str	r3, [r2, #456]
    df2c:	e3a03000 	mov	r3, #0	; 0x0
    df30:	e92d4070 	push	{r4, r5, r6, lr}
    df34:	e58231c8 	str	r3, [r2, #456]
    df38:	e1a05002 	mov	r5, r2
    df3c:	eb0001c2 	bl	e64c <get_ticks>
    df40:	e59f6048 	ldr	r6, [pc, #72]	; df90 <nf2_randc_seed_hw+0x78>
    df44:	e1a04000 	mov	r4, r0
    df48:	eb0001bf 	bl	e64c <get_ticks>
    df4c:	e0640000 	rsb	r0, r4, r0
    df50:	e1500006 	cmp	r0, r6
    df54:	da000005 	ble	df70 <nf2_randc_seed_hw+0x58>
    df58:	e59f0034 	ldr	r0, [pc, #52]	; df94 <nf2_randc_seed_hw+0x7c>
    df5c:	e59f3034 	ldr	r3, [pc, #52]	; df98 <nf2_randc_seed_hw+0x80>
    df60:	e1a0e00f 	mov	lr, pc
    df64:	e593f000 	ldr	pc, [r3]
    df68:	e3e00000 	mvn	r0, #0	; 0x0
    df6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    df70:	e59531cc 	ldr	r3, [r5, #460]
    df74:	e3130801 	tst	r3, #65536	; 0x10000
    df78:	1afffff2 	bne	df48 <nf2_randc_seed_hw+0x30>
    df7c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    df80:	e59331cc 	ldr	r3, [r3, #460]
    df84:	e1a00803 	lsl	r0, r3, #16
    df88:	e1a00820 	lsr	r0, r0, #16
    df8c:	e8bd8070 	pop	{r4, r5, r6, pc}
    df90:	0007a120 	.word	0x0007a120
    df94:	0001915e 	.word	0x0001915e
    df98:	3c00a5cc 	.word	0x3c00a5cc

0000df9c <nf2_asyn_reset>:
    df9c:	e92d4070 	push	{r4, r5, r6, lr}
    dfa0:	e1a04000 	mov	r4, r0
    dfa4:	e3a00001 	mov	r0, #1	; 0x1
    dfa8:	ebfffccd 	bl	d2e4 <nf2_soft_reset>
    dfac:	e3a01001 	mov	r1, #1	; 0x1
    dfb0:	e1e04411 	mvn	r4, r1, lsl r4
    dfb4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    dfb8:	e30520ff 	movw	r2, #20735	; 0x50ff
    dfbc:	e5831054 	str	r1, [r3, #84]
    dfc0:	e5832060 	str	r2, [r3, #96]
    dfc4:	e3a02000 	mov	r2, #0	; 0x0
    dfc8:	e5832054 	str	r2, [r3, #84]
    dfcc:	e593204c 	ldr	r2, [r3, #76]
    dfd0:	e583404c 	str	r4, [r3, #76]
    dfd4:	e5831084 	str	r1, [r3, #132]
    dfd8:	e1a06003 	mov	r6, r3
    dfdc:	eb00019a 	bl	e64c <get_ticks>
    dfe0:	e3005bb8 	movw	r5, #3000	; 0xbb8
    dfe4:	e1a04000 	mov	r4, r0
    dfe8:	eb000197 	bl	e64c <get_ticks>
    dfec:	e0640000 	rsb	r0, r4, r0
    dff0:	e1500005 	cmp	r0, r5
    dff4:	da000005 	ble	e010 <nf2_asyn_reset+0x74>
    dff8:	e59f0038 	ldr	r0, [pc, #56]	; e038 <nf2_asyn_reset+0x9c>
    dffc:	e59f3038 	ldr	r3, [pc, #56]	; e03c <nf2_asyn_reset+0xa0>
    e000:	e1a0e00f 	mov	lr, pc
    e004:	e593f000 	ldr	pc, [r3]
    e008:	e3e00000 	mvn	r0, #0	; 0x0
    e00c:	ea000003 	b	e020 <nf2_asyn_reset+0x84>
    e010:	e596308c 	ldr	r3, [r6, #140]
    e014:	e3130001 	tst	r3, #1	; 0x1
    e018:	0afffff2 	beq	dfe8 <nf2_asyn_reset+0x4c>
    e01c:	e3a00000 	mov	r0, #0	; 0x0
    e020:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    e024:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    e028:	e583208c 	str	r2, [r3, #140]
    e02c:	e3a0200f 	mov	r2, #15	; 0xf
    e030:	e583204c 	str	r2, [r3, #76]
    e034:	e8bd8070 	pop	{r4, r5, r6, pc}
    e038:	00019179 	.word	0x00019179
    e03c:	3c00a5cc 	.word	0x3c00a5cc

0000e040 <nf2_active_sync>:
    e040:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e044:	e1a05000 	mov	r5, r0
    e048:	e3a00001 	mov	r0, #1	; 0x1
    e04c:	ebfffca4 	bl	d2e4 <nf2_soft_reset>
    e050:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    e054:	e3a06000 	mov	r6, #0	; 0x0
    e058:	e3a07001 	mov	r7, #1	; 0x1
    e05c:	e30430ef 	movw	r3, #16623	; 0x40ef
    e060:	e58460c4 	str	r6, [r4, #196]
    e064:	e5847054 	str	r7, [r4, #84]
    e068:	e5843060 	str	r3, [r4, #96]
    e06c:	e2833012 	add	r3, r3, #18	; 0x12
    e070:	e5843060 	str	r3, [r4, #96]
    e074:	e3a0cc42 	mov	ip, #16896	; 0x4200
    e078:	e3a03c52 	mov	r3, #20992	; 0x5200
    e07c:	e3855c42 	orr	r5, r5, #16896	; 0x4200
    e080:	e5845060 	str	r5, [r4, #96]
    e084:	e3a00002 	mov	r0, #2	; 0x2
    e088:	e584c060 	str	ip, [r4, #96]
    e08c:	e3a0100f 	mov	r1, #15	; 0xf
    e090:	e584c060 	str	ip, [r4, #96]
    e094:	e1a02006 	mov	r2, r6
    e098:	e5843060 	str	r3, [r4, #96]
    e09c:	ebfffb08 	bl	ccc4 <trx_afifo_reg>
    e0a0:	e3a03c01 	mov	r3, #256	; 0x100
    e0a4:	e3a00003 	mov	r0, #3	; 0x3
    e0a8:	e1a01007 	mov	r1, r7
    e0ac:	e1a02006 	mov	r2, r6
    e0b0:	e5843060 	str	r3, [r4, #96]
    e0b4:	ebfffb02 	bl	ccc4 <trx_afifo_reg>
    e0b8:	e3a0300e 	mov	r3, #14	; 0xe
    e0bc:	e5846054 	str	r6, [r4, #84]
    e0c0:	e584304c 	str	r3, [r4, #76]
    e0c4:	e5847084 	str	r7, [r4, #132]
    e0c8:	eb00015f 	bl	e64c <get_ticks>
    e0cc:	e59f8048 	ldr	r8, [pc, #72]	; e11c <nf2_active_sync+0xdc>
    e0d0:	e1a05000 	mov	r5, r0
    e0d4:	eb00015c 	bl	e64c <get_ticks>
    e0d8:	e0650000 	rsb	r0, r5, r0
    e0dc:	e1500008 	cmp	r0, r8
    e0e0:	da000005 	ble	e0fc <nf2_active_sync+0xbc>
    e0e4:	e59f0034 	ldr	r0, [pc, #52]	; e120 <nf2_active_sync+0xe0>
    e0e8:	e59f3034 	ldr	r3, [pc, #52]	; e124 <nf2_active_sync+0xe4>
    e0ec:	e1a0e00f 	mov	lr, pc
    e0f0:	e593f000 	ldr	pc, [r3]
    e0f4:	e3e00000 	mvn	r0, #0	; 0x0
    e0f8:	ea000003 	b	e10c <nf2_active_sync+0xcc>
    e0fc:	e594308c 	ldr	r3, [r4, #140]
    e100:	e3130001 	tst	r3, #1	; 0x1
    e104:	0afffff2 	beq	e0d4 <nf2_active_sync+0x94>
    e108:	e3a00000 	mov	r0, #0	; 0x0
    e10c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    e110:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    e114:	e583208c 	str	r2, [r3, #140]
    e118:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e11c:	0007a120 	.word	0x0007a120
    e120:	00019191 	.word	0x00019191
    e124:	3c00a5cc 	.word	0x3c00a5cc

0000e128 <nf2_timing_init>:
    e128:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e12c:	e1a04000 	mov	r4, r0
    e130:	e24dd004 	sub	sp, sp, #4	; 0x4
    e134:	e59f0144 	ldr	r0, [pc, #324]	; e280 <nf2_timing_init+0x158>
    e138:	e1a07003 	mov	r7, r3
    e13c:	e1a05001 	mov	r5, r1
    e140:	e1a06002 	mov	r6, r2
    e144:	ebffca53 	bl	a98 <module_enable>
    e148:	e3a01000 	mov	r1, #0	; 0x0
    e14c:	e1a02001 	mov	r2, r1
    e150:	e3a00807 	mov	r0, #458752	; 0x70000
    e154:	ebffcaac 	bl	c0c <pads_chmod>
    e158:	e3a01000 	mov	r1, #0	; 0x0
    e15c:	e59f0120 	ldr	r0, [pc, #288]	; e284 <nf2_timing_init+0x15c>
    e160:	e1a02001 	mov	r2, r1
    e164:	ebffcaa8 	bl	c0c <pads_chmod>
    e168:	e3a00301 	mov	r0, #67108864	; 0x4000000
    e16c:	ebffcbb9 	bl	1058 <ecfg_check_flag>
    e170:	e3500000 	cmp	r0, #0	; 0x0
    e174:	0a00000f 	beq	e1b8 <nf2_timing_init+0x90>
    e178:	e3a01001 	mov	r1, #1	; 0x1
    e17c:	e1a02001 	mov	r2, r1
    e180:	e3a00807 	mov	r0, #458752	; 0x70000
    e184:	ebffcaff 	bl	d88 <pads_pull>
    e188:	e3a01001 	mov	r1, #1	; 0x1
    e18c:	e1a02001 	mov	r2, r1
    e190:	e59f00ec 	ldr	r0, [pc, #236]	; e284 <nf2_timing_init+0x15c>
    e194:	ebffcafb 	bl	d88 <pads_pull>
    e198:	e59f00e8 	ldr	r0, [pc, #232]	; e288 <nf2_timing_init+0x160>
    e19c:	e3a01001 	mov	r1, #1	; 0x1
    e1a0:	e3a02000 	mov	r2, #0	; 0x0
    e1a4:	ebffcaf7 	bl	d88 <pads_pull>
    e1a8:	e3a00017 	mov	r0, #23	; 0x17
    e1ac:	e3a01001 	mov	r1, #1	; 0x1
    e1b0:	e3a02000 	mov	r2, #0	; 0x0
    e1b4:	ebffcaf3 	bl	d88 <pads_pull>
    e1b8:	e59d301c 	ldr	r3, [sp, #28]
    e1bc:	e3530010 	cmp	r3, #16	; 0x10
    e1c0:	13a02000 	movne	r2, #0	; 0x0
    e1c4:	1a00000c 	bne	e1fc <nf2_timing_init+0xd4>
    e1c8:	e3a01000 	mov	r1, #0	; 0x0
    e1cc:	e59f00b8 	ldr	r0, [pc, #184]	; e28c <nf2_timing_init+0x164>
    e1d0:	e1a02001 	mov	r2, r1
    e1d4:	ebffca8c 	bl	c0c <pads_chmod>
    e1d8:	e3a00301 	mov	r0, #67108864	; 0x4000000
    e1dc:	ebffcb9d 	bl	1058 <ecfg_check_flag>
    e1e0:	e3500000 	cmp	r0, #0	; 0x0
    e1e4:	0a000003 	beq	e1f8 <nf2_timing_init+0xd0>
    e1e8:	e3a01001 	mov	r1, #1	; 0x1
    e1ec:	e59f0098 	ldr	r0, [pc, #152]	; e28c <nf2_timing_init+0x164>
    e1f0:	e1a02001 	mov	r2, r1
    e1f4:	ebffcae3 	bl	d88 <pads_pull>
    e1f8:	e3a02001 	mov	r2, #1	; 0x1
    e1fc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    e200:	e58321b8 	str	r2, [r3, #440]
    e204:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    e208:	e59230c8 	ldr	r3, [r2, #200]
    e20c:	e3540000 	cmp	r4, #0	; 0x0
    e210:	e3833030 	orr	r3, r3, #48	; 0x30
    e214:	e58230c8 	str	r3, [r2, #200]
    e218:	058240c4 	streq	r4, [r2, #196]
    e21c:	059230c8 	ldreq	r3, [r2, #200]
    e220:	03833001 	orreq	r3, r3, #1	; 0x1
    e224:	0a000005 	beq	e240 <nf2_timing_init+0x118>
    e228:	e3540001 	cmp	r4, #1	; 0x1
    e22c:	13a03002 	movne	r3, #2	; 0x2
    e230:	058240c4 	streq	r4, [r2, #196]
    e234:	158230c4 	strne	r3, [r2, #196]
    e238:	e59230c8 	ldr	r3, [r2, #200]
    e23c:	e3c33001 	bic	r3, r3, #1	; 0x1
    e240:	e58230c8 	str	r3, [r2, #200]
    e244:	e3c534ff 	bic	r3, r5, #-16777216	; 0xff000000
    e248:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    e24c:	e3c3360e 	bic	r3, r3, #14680064	; 0xe00000
    e250:	e5823000 	str	r3, [r2]
    e254:	e1a01c25 	lsr	r1, r5, #24
    e258:	e30e3001 	movw	r3, #57345	; 0xe001
    e25c:	e5821004 	str	r1, [r2, #4]
    e260:	e582300c 	str	r3, [r2, #12]
    e264:	e58270d0 	str	r7, [r2, #208]
    e268:	e5826008 	str	r6, [r2, #8]
    e26c:	e59d3018 	ldr	r3, [sp, #24]
    e270:	e3a00001 	mov	r0, #1	; 0x1
    e274:	e58230d4 	str	r3, [r2, #212]
    e278:	e28dd004 	add	sp, sp, #4	; 0x4
    e27c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e280:	21e3c000 	.word	0x21e3c000
    e284:	00170010 	.word	0x00170010
    e288:	00130012 	.word	0x00130012
    e28c:	000f0008 	.word	0x000f0008

0000e290 <nand_readid>:
    e290:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e294:	e1a06000 	mov	r6, r0
    e298:	e24dd010 	sub	sp, sp, #16	; 0x10
    e29c:	e59f013c 	ldr	r0, [pc, #316]	; e3e0 <nand_readid+0x150>
    e2a0:	e1a04002 	mov	r4, r2
    e2a4:	e1a05003 	mov	r5, r3
    e2a8:	ebffcb4b 	bl	fdc <module_reset>
    e2ac:	e59dc02c 	ldr	ip, [sp, #44]
    e2b0:	e1a01004 	mov	r1, r4
    e2b4:	e58dc000 	str	ip, [sp]
    e2b8:	e59dc030 	ldr	ip, [sp, #48]
    e2bc:	e1a02005 	mov	r2, r5
    e2c0:	e59d3028 	ldr	r3, [sp, #40]
    e2c4:	e3a00000 	mov	r0, #0	; 0x0
    e2c8:	e58dc004 	str	ip, [sp, #4]
    e2cc:	ebffff95 	bl	e128 <nf2_timing_init>
    e2d0:	e3a00000 	mov	r0, #0	; 0x0
    e2d4:	ebffff30 	bl	df9c <nf2_asyn_reset>
    e2d8:	e3500000 	cmp	r0, #0	; 0x0
    e2dc:	1a00003b 	bne	e3d0 <nand_readid+0x140>
    e2e0:	e2800001 	add	r0, r0, #1	; 0x1
    e2e4:	ebfffbfe 	bl	d2e4 <nf2_soft_reset>
    e2e8:	e2505000 	subs	r5, r0, #0	; 0x0
    e2ec:	1a000037 	bne	e3d0 <nand_readid+0x140>
    e2f0:	e58d500c 	str	r5, [sp, #12]
    e2f4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    e2f8:	e5923054 	ldr	r3, [r2, #84]
    e2fc:	e1a07002 	mov	r7, r2
    e300:	e58d300c 	str	r3, [sp, #12]
    e304:	e59d300c 	ldr	r3, [sp, #12]
    e308:	e59f80d4 	ldr	r8, [pc, #212]	; e3e4 <nand_readid+0x154>
    e30c:	e3833001 	orr	r3, r3, #1	; 0x1
    e310:	e58d300c 	str	r3, [sp, #12]
    e314:	e59d300c 	ldr	r3, [sp, #12]
    e318:	e5823054 	str	r3, [r2, #84]
    e31c:	e3043090 	movw	r3, #16528	; 0x4090
    e320:	e5823060 	str	r3, [r2, #96]
    e324:	e2833070 	add	r3, r3, #112	; 0x70
    e328:	e5823060 	str	r3, [r2, #96]
    e32c:	e3a03008 	mov	r3, #8	; 0x8
    e330:	e5823060 	str	r3, [r2, #96]
    e334:	e3a03c7f 	mov	r3, #32512	; 0x7f00
    e338:	e5823060 	str	r3, [r2, #96]
    e33c:	e3e03001 	mvn	r3, #1	; 0x1
    e340:	e5825054 	str	r5, [r2, #84]
    e344:	e582304c 	str	r3, [r2, #76]
    e348:	e2833003 	add	r3, r3, #3	; 0x3
    e34c:	e5823084 	str	r3, [r2, #132]
    e350:	eb0000bd 	bl	e64c <get_ticks>
    e354:	e1a04000 	mov	r4, r0
    e358:	eb0000bb 	bl	e64c <get_ticks>
    e35c:	e0640000 	rsb	r0, r4, r0
    e360:	e1500008 	cmp	r0, r8
    e364:	c3e05000 	mvngt	r5, #0	; 0x0
    e368:	ca000002 	bgt	e378 <nand_readid+0xe8>
    e36c:	e597308c 	ldr	r3, [r7, #140]
    e370:	e3130001 	tst	r3, #1	; 0x1
    e374:	0afffff7 	beq	e358 <nand_readid+0xc8>
    e378:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    e37c:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    e380:	e580308c 	str	r3, [r0, #140]
    e384:	e3a0300f 	mov	r3, #15	; 0xf
    e388:	e580304c 	str	r3, [r0, #76]
    e38c:	e5903010 	ldr	r3, [r0, #16]
    e390:	e1a02423 	lsr	r2, r3, #8
    e394:	e1a01823 	lsr	r1, r3, #16
    e398:	e1a0cc23 	lsr	ip, r3, #24
    e39c:	e5c62006 	strb	r2, [r6, #6]
    e3a0:	e5c61005 	strb	r1, [r6, #5]
    e3a4:	e5c6c004 	strb	ip, [r6, #4]
    e3a8:	e5c63007 	strb	r3, [r6, #7]
    e3ac:	e5903014 	ldr	r3, [r0, #20]
    e3b0:	e1a02c23 	lsr	r2, r3, #24
    e3b4:	e1a01423 	lsr	r1, r3, #8
    e3b8:	e1a00823 	lsr	r0, r3, #16
    e3bc:	e5c62000 	strb	r2, [r6]
    e3c0:	e5c61002 	strb	r1, [r6, #2]
    e3c4:	e5c60001 	strb	r0, [r6, #1]
    e3c8:	e5c63003 	strb	r3, [r6, #3]
    e3cc:	ea000000 	b	e3d4 <nand_readid+0x144>
    e3d0:	e3e05000 	mvn	r5, #0	; 0x0
    e3d4:	e1a00005 	mov	r0, r5
    e3d8:	e28dd010 	add	sp, sp, #16	; 0x10
    e3dc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e3e0:	21e3c000 	.word	0x21e3c000
    e3e4:	0007a120 	.word	0x0007a120

0000e3e8 <nf2_hardware_reset>:
    e3e8:	e92d4030 	push	{r4, r5, lr}
    e3ec:	e1a04000 	mov	r4, r0
    e3f0:	e5d00000 	ldrb	r0, [r0]
    e3f4:	e594c034 	ldr	ip, [r4, #52]
    e3f8:	e3500002 	cmp	r0, #2	; 0x2
    e3fc:	e5d4e006 	ldrb	lr, [r4, #6]
    e400:	e5943030 	ldr	r3, [r4, #48]
    e404:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e408:	e594202c 	ldr	r2, [r4, #44]
    e40c:	03a00002 	moveq	r0, #2	; 0x2
    e410:	13a00000 	movne	r0, #0	; 0x0
    e414:	e1a05001 	mov	r5, r1
    e418:	e5941028 	ldr	r1, [r4, #40]
    e41c:	e88d5000 	stm	sp, {ip, lr}
    e420:	ebffff40 	bl	e128 <nf2_timing_init>
    e424:	e3a00000 	mov	r0, #0	; 0x0
    e428:	ebfffedb 	bl	df9c <nf2_asyn_reset>
    e42c:	e2503000 	subs	r3, r0, #0	; 0x0
    e430:	1a00000e 	bne	e470 <nf2_hardware_reset+0x88>
    e434:	e3550000 	cmp	r5, #0	; 0x0
    e438:	0a00000c 	beq	e470 <nf2_hardware_reset+0x88>
    e43c:	e5d40000 	ldrb	r0, [r4]
    e440:	e3500001 	cmp	r0, #1	; 0x1
    e444:	1a000009 	bne	e470 <nf2_hardware_reset+0x88>
    e448:	e594c034 	ldr	ip, [r4, #52]
    e44c:	e5d4e006 	ldrb	lr, [r4, #6]
    e450:	e2841028 	add	r1, r4, #40	; 0x28
    e454:	e891000e 	ldm	r1, {r1, r2, r3}
    e458:	e88d5000 	stm	sp, {ip, lr}
    e45c:	ebffff31 	bl	e128 <nf2_timing_init>
    e460:	e3a00010 	mov	r0, #16	; 0x10
    e464:	e28dd00c 	add	sp, sp, #12	; 0xc
    e468:	e8bd4030 	pop	{r4, r5, lr}
    e46c:	eafffef3 	b	e040 <nf2_active_sync>
    e470:	e1a00003 	mov	r0, r3
    e474:	e28dd00c 	add	sp, sp, #12	; 0xc
    e478:	e8bd8030 	pop	{r4, r5, pc}

0000e47c <serial_tstc>:
    e47c:	e59f3010 	ldr	r3, [pc, #16]	; e494 <serial_tstc+0x18>
    e480:	e5930018 	ldr	r0, [r3, #24]
    e484:	e1a00220 	lsr	r0, r0, #4
    e488:	e2200001 	eor	r0, r0, #1	; 0x1
    e48c:	e2000001 	and	r0, r0, #1	; 0x1
    e490:	e12fff1e 	bx	lr
    e494:	20803000 	.word	0x20803000

0000e498 <serial_putc>:
    e498:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e49c:	e24dd004 	sub	sp, sp, #4	; 0x4
    e4a0:	e1a06000 	mov	r6, r0
    e4a4:	eb000068 	bl	e64c <get_ticks>
    e4a8:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    e4ac:	e3a05000 	mov	r5, #0	; 0x0
    e4b0:	e0944000 	adds	r4, r4, r0
    e4b4:	e59f7034 	ldr	r7, [pc, #52]	; e4f0 <serial_putc+0x58>
    e4b8:	e0a55001 	adc	r5, r5, r1
    e4bc:	e5973018 	ldr	r3, [r7, #24]
    e4c0:	e3130020 	tst	r3, #32	; 0x20
    e4c4:	0a000005 	beq	e4e0 <serial_putc+0x48>
    e4c8:	eb00005f 	bl	e64c <get_ticks>
    e4cc:	e1510005 	cmp	r1, r5
    e4d0:	8a000002 	bhi	e4e0 <serial_putc+0x48>
    e4d4:	1afffff8 	bne	e4bc <serial_putc+0x24>
    e4d8:	e1500004 	cmp	r0, r4
    e4dc:	9afffff6 	bls	e4bc <serial_putc+0x24>
    e4e0:	e59f3008 	ldr	r3, [pc, #8]	; e4f0 <serial_putc+0x58>
    e4e4:	e5836000 	str	r6, [r3]
    e4e8:	e28dd004 	add	sp, sp, #4	; 0x4
    e4ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e4f0:	20803000 	.word	0x20803000

0000e4f4 <serial_puts>:
    e4f4:	e92d4030 	push	{r4, r5, lr}
    e4f8:	e1a05000 	mov	r5, r0
    e4fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    e500:	e3a04000 	mov	r4, #0	; 0x0
    e504:	ea000006 	b	e524 <serial_puts+0x30>
    e508:	e353000a 	cmp	r3, #10	; 0xa
    e50c:	1a000001 	bne	e518 <serial_puts+0x24>
    e510:	e3a0000d 	mov	r0, #13	; 0xd
    e514:	ebffffdf 	bl	e498 <serial_putc>
    e518:	e7d50004 	ldrb	r0, [r5, r4]
    e51c:	ebffffdd 	bl	e498 <serial_putc>
    e520:	e2844001 	add	r4, r4, #1	; 0x1
    e524:	e7d53004 	ldrb	r3, [r5, r4]
    e528:	e3530000 	cmp	r3, #0	; 0x0
    e52c:	1afffff5 	bne	e508 <serial_puts+0x14>
    e530:	e28dd004 	add	sp, sp, #4	; 0x4
    e534:	e8bd8030 	pop	{r4, r5, pc}

0000e538 <serial_getc>:
    e538:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e53c:	e59f2024 	ldr	r2, [pc, #36]	; e568 <serial_getc+0x30>
    e540:	e24dd004 	sub	sp, sp, #4	; 0x4
    e544:	e5923018 	ldr	r3, [r2, #24]
    e548:	e3130010 	tst	r3, #16	; 0x10
    e54c:	1afffffc 	bne	e544 <serial_getc+0xc>
    e550:	ebffd824 	bl	45e8 <tt_rnd_seed>
    e554:	e59f300c 	ldr	r3, [pc, #12]	; e568 <serial_getc+0x30>
    e558:	e5930000 	ldr	r0, [r3]
    e55c:	e6ef0070 	uxtb	r0, r0
    e560:	e28dd004 	add	sp, sp, #4	; 0x4
    e564:	e8bd8000 	pop	{pc}
    e568:	20803000 	.word	0x20803000

0000e56c <serial_setbrg>:
    e56c:	e92d4010 	push	{r4, lr}
    e570:	e59f0038 	ldr	r0, [pc, #56]	; e5b0 <serial_setbrg+0x44>
    e574:	eb000cb7 	bl	11858 <module_get_clock>
    e578:	e59f1034 	ldr	r1, [pc, #52]	; e5b4 <serial_setbrg+0x48>
    e57c:	e1a04000 	mov	r4, r0
    e580:	eb001a81 	bl	14f8c <__aeabi_idiv>
    e584:	e6bf3070 	sxth	r3, r0
    e588:	e59f0028 	ldr	r0, [pc, #40]	; e5b8 <serial_setbrg+0x4c>
    e58c:	e59f1020 	ldr	r1, [pc, #32]	; e5b4 <serial_setbrg+0x48>
    e590:	e0204390 	mla	r0, r0, r3, r4
    e594:	e59f4020 	ldr	r4, [pc, #32]	; e5bc <serial_setbrg+0x50>
    e598:	e1a00300 	lsl	r0, r0, #6
    e59c:	e5843024 	str	r3, [r4, #36]
    e5a0:	eb001a79 	bl	14f8c <__aeabi_idiv>
    e5a4:	e200003f 	and	r0, r0, #63	; 0x3f
    e5a8:	e5840028 	str	r0, [r4, #40]
    e5ac:	e8bd8010 	pop	{r4, pc}
    e5b0:	21e0a290 	.word	0x21e0a290
    e5b4:	001c2000 	.word	0x001c2000
    e5b8:	ffe3e000 	.word	0xffe3e000
    e5bc:	20803000 	.word	0x20803000

0000e5c0 <init_serial>:
    e5c0:	e92d4010 	push	{r4, lr}
    e5c4:	e59f0074 	ldr	r0, [pc, #116]	; e640 <init_serial+0x80>
    e5c8:	ebffc932 	bl	a98 <module_enable>
    e5cc:	e3a01000 	mov	r1, #0	; 0x0
    e5d0:	e59f006c 	ldr	r0, [pc, #108]	; e644 <init_serial+0x84>
    e5d4:	e1a02001 	mov	r2, r1
    e5d8:	ebffc98b 	bl	c0c <pads_chmod>
    e5dc:	e3a00201 	mov	r0, #268435456	; 0x10000000
    e5e0:	ebffca9c 	bl	1058 <ecfg_check_flag>
    e5e4:	e3500000 	cmp	r0, #0	; 0x0
    e5e8:	0a000003 	beq	e5fc <init_serial+0x3c>
    e5ec:	e3a01001 	mov	r1, #1	; 0x1
    e5f0:	e59f004c 	ldr	r0, [pc, #76]	; e644 <init_serial+0x84>
    e5f4:	e1a02001 	mov	r2, r1
    e5f8:	ebffc9e2 	bl	d88 <pads_pull>
    e5fc:	e59f4044 	ldr	r4, [pc, #68]	; e648 <init_serial+0x88>
    e600:	e594302c 	ldr	r3, [r4, #44]
    e604:	e3c330ff 	bic	r3, r3, #255	; 0xff
    e608:	e3833060 	orr	r3, r3, #96	; 0x60
    e60c:	e584302c 	str	r3, [r4, #44]
    e610:	ebffffd5 	bl	e56c <serial_setbrg>
    e614:	e5943030 	ldr	r3, [r4, #48]
    e618:	e3a00000 	mov	r0, #0	; 0x0
    e61c:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    e620:	e3c33087 	bic	r3, r3, #135	; 0x87
    e624:	e3833c03 	orr	r3, r3, #768	; 0x300
    e628:	e3833001 	orr	r3, r3, #1	; 0x1
    e62c:	e5843030 	str	r3, [r4, #48]
    e630:	e594302c 	ldr	r3, [r4, #44]
    e634:	e3833010 	orr	r3, r3, #16	; 0x10
    e638:	e584302c 	str	r3, [r4, #44]
    e63c:	e8bd8010 	pop	{r4, pc}
    e640:	21e12000 	.word	0x21e12000
    e644:	00380037 	.word	0x00380037
    e648:	20803000 	.word	0x20803000

0000e64c <get_ticks>:
    e64c:	e59f1038 	ldr	r1, [pc, #56]	; e68c <get_ticks+0x40>
    e650:	e5913204 	ldr	r3, [r1, #516]
    e654:	e5912200 	ldr	r2, [r1, #512]
    e658:	e591c204 	ldr	ip, [r1, #516]
    e65c:	e15c0003 	cmp	ip, r3
    e660:	15913200 	ldrne	r3, [r1, #512]
    e664:	01a00002 	moveq	r0, r2
    e668:	03a01000 	moveq	r1, #0	; 0x0
    e66c:	11a00003 	movne	r0, r3
    e670:	13a01000 	movne	r1, #0	; 0x0
    e674:	e3a02000 	mov	r2, #0	; 0x0
    e678:	e1822000 	orr	r2, r2, r0
    e67c:	e18c3001 	orr	r3, ip, r1
    e680:	e1a01003 	mov	r1, r3
    e684:	e1a00002 	mov	r0, r2
    e688:	e12fff1e 	bx	lr
    e68c:	21b00000 	.word	0x21b00000

0000e690 <udelay>:
    e690:	e59f1084 	ldr	r1, [pc, #132]	; e71c <udelay+0x8c>
    e694:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e698:	e5913204 	ldr	r3, [r1, #516]
    e69c:	e5912200 	ldr	r2, [r1, #512]
    e6a0:	e591c204 	ldr	ip, [r1, #516]
    e6a4:	e15c0003 	cmp	ip, r3
    e6a8:	15913200 	ldrne	r3, [r1, #512]
    e6ac:	01a04002 	moveq	r4, r2
    e6b0:	03a05000 	moveq	r5, #0	; 0x0
    e6b4:	11a04003 	movne	r4, r3
    e6b8:	13a05000 	movne	r5, #0	; 0x0
    e6bc:	e3a02000 	mov	r2, #0	; 0x0
    e6c0:	e1822004 	orr	r2, r2, r4
    e6c4:	e18c3005 	orr	r3, ip, r5
    e6c8:	e0926000 	adds	r6, r2, r0
    e6cc:	e59fc048 	ldr	ip, [pc, #72]	; e71c <udelay+0x8c>
    e6d0:	e2a37000 	adc	r7, r3, #0	; 0x0
    e6d4:	e59c3204 	ldr	r3, [ip, #516]
    e6d8:	e59c2200 	ldr	r2, [ip, #512]
    e6dc:	e59c1204 	ldr	r1, [ip, #516]
    e6e0:	e1510003 	cmp	r1, r3
    e6e4:	159c3200 	ldrne	r3, [ip, #512]
    e6e8:	01a04002 	moveq	r4, r2
    e6ec:	03a05000 	moveq	r5, #0	; 0x0
    e6f0:	11a04003 	movne	r4, r3
    e6f4:	13a05000 	movne	r5, #0	; 0x0
    e6f8:	e1851001 	orr	r1, r5, r1
    e6fc:	e3a02000 	mov	r2, #0	; 0x0
    e700:	e1570001 	cmp	r7, r1
    e704:	e1840002 	orr	r0, r4, r2
    e708:	8afffff1 	bhi	e6d4 <udelay+0x44>
    e70c:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    e710:	e1560000 	cmp	r6, r0
    e714:	8affffee 	bhi	e6d4 <udelay+0x44>
    e718:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e71c:	21b00000 	.word	0x21b00000

0000e720 <get_timer>:
    e720:	e59f2074 	ldr	r2, [pc, #116]	; e79c <get_timer+0x7c>
    e724:	e92d4070 	push	{r4, r5, r6, lr}
    e728:	e5923204 	ldr	r3, [r2, #516]
    e72c:	e1a06000 	mov	r6, r0
    e730:	e5921200 	ldr	r1, [r2, #512]
    e734:	e5920204 	ldr	r0, [r2, #516]
    e738:	e24dd008 	sub	sp, sp, #8	; 0x8
    e73c:	e1500003 	cmp	r0, r3
    e740:	15923200 	ldrne	r3, [r2, #512]
    e744:	01a04001 	moveq	r4, r1
    e748:	03a05000 	moveq	r5, #0	; 0x0
    e74c:	11a04003 	movne	r4, r3
    e750:	13a05000 	movne	r5, #0	; 0x0
    e754:	e1a03000 	mov	r3, r0
    e758:	e3a02000 	mov	r2, #0	; 0x0
    e75c:	e1851003 	orr	r1, r5, r3
    e760:	e1840002 	orr	r0, r4, r2
    e764:	e1510002 	cmp	r1, r2
    e768:	e88d0003 	stm	sp, {r0, r1}
    e76c:	1a000003 	bne	e780 <get_timer+0x60>
    e770:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    e774:	eb001982 	bl	14d84 <__aeabi_uidiv>
    e778:	e58d0000 	str	r0, [sp]
    e77c:	ea000002 	b	e78c <get_timer+0x6c>
    e780:	e1a0000d 	mov	r0, sp
    e784:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    e788:	eb000d3c 	bl	11c80 <__div64_32>
    e78c:	e59d0000 	ldr	r0, [sp]
    e790:	e0660000 	rsb	r0, r6, r0
    e794:	e28dd008 	add	sp, sp, #8	; 0x8
    e798:	e8bd8070 	pop	{r4, r5, r6, pc}
    e79c:	21b00000 	.word	0x21b00000

0000e7a0 <init_timer>:
    e7a0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e7a4:	e3a00003 	mov	r0, #3	; 0x3
    e7a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    e7ac:	eb000c29 	bl	11858 <module_get_clock>
    e7b0:	e59f1030 	ldr	r1, [pc, #48]	; e7e8 <init_timer+0x48>
    e7b4:	eb0019f4 	bl	14f8c <__aeabi_idiv>
    e7b8:	e2401001 	sub	r1, r0, #1	; 0x1
    e7bc:	e59f2028 	ldr	r2, [pc, #40]	; e7ec <init_timer+0x4c>
    e7c0:	e1a01401 	lsl	r1, r1, #8
    e7c4:	e3a03000 	mov	r3, #0	; 0x0
    e7c8:	e3811001 	orr	r1, r1, #1	; 0x1
    e7cc:	e5823208 	str	r3, [r2, #520]
    e7d0:	e1a00003 	mov	r0, r3
    e7d4:	e5823200 	str	r3, [r2, #512]
    e7d8:	e5823204 	str	r3, [r2, #516]
    e7dc:	e5821208 	str	r1, [r2, #520]
    e7e0:	e28dd004 	add	sp, sp, #4	; 0x4
    e7e4:	e8bd8000 	pop	{pc}
    e7e8:	000f4240 	.word	0x000f4240
    e7ec:	21b00000 	.word	0x21b00000

0000e7f0 <e2p_vs_align>:
    e7f0:	e3a00010 	mov	r0, #16	; 0x10
    e7f4:	e12fff1e 	bx	lr

0000e7f8 <e2p_vs_reset>:
    e7f8:	e3a00000 	mov	r0, #0	; 0x0
    e7fc:	e12fff1e 	bx	lr

0000e800 <iic_address_num>:
    e800:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e804:	e24dd004 	sub	sp, sp, #4	; 0x4
    e808:	ebffc7ef 	bl	7cc <boot_info>
    e80c:	e59f3014 	ldr	r3, [pc, #20]	; e828 <iic_address_num+0x28>
    e810:	e1a00080 	lsl	r0, r0, #1
    e814:	e20000ff 	and	r0, r0, #255	; 0xff
    e818:	e5830000 	str	r0, [r3]
    e81c:	e3a00001 	mov	r0, #1	; 0x1
    e820:	e28dd004 	add	sp, sp, #4	; 0x4
    e824:	e8bd8000 	pop	{pc}
    e828:	3c00a130 	.word	0x3c00a130

0000e82c <iic_read_get_address>:
    e82c:	e1d020b2 	ldrh	r2, [r0, #2]
    e830:	e590300c 	ldr	r3, [r0, #12]
    e834:	e92d4010 	push	{r4, lr}
    e838:	e0833002 	add	r3, r3, r2
    e83c:	e1c030b2 	strh	r3, [r0, #2]
    e840:	e1a04000 	mov	r4, r0
    e844:	ebffffed 	bl	e800 <iic_address_num>
    e848:	e3500000 	cmp	r0, #0	; 0x0
    e84c:	1a000008 	bne	e874 <iic_read_get_address+0x48>
    e850:	e1d430b2 	ldrh	r3, [r4, #2]
    e854:	e3530c01 	cmp	r3, #256	; 0x100
    e858:	1a000005 	bne	e874 <iic_read_get_address+0x48>
    e85c:	e1d430b0 	ldrh	r3, [r4]
    e860:	e1c400b2 	strh	r0, [r4, #2]
    e864:	e2833002 	add	r3, r3, #2	; 0x2
    e868:	e1c430b0 	strh	r3, [r4]
    e86c:	e3a03001 	mov	r3, #1	; 0x1
    e870:	e1c431b0 	strh	r3, [r4, #16]
    e874:	ebffffe1 	bl	e800 <iic_address_num>
    e878:	e3500000 	cmp	r0, #0	; 0x0
    e87c:	e5941008 	ldr	r1, [r4, #8]
    e880:	0a000004 	beq	e898 <iic_read_get_address+0x6c>
    e884:	e3510000 	cmp	r1, #0	; 0x0
    e888:	13a03000 	movne	r3, #0	; 0x0
    e88c:	15843008 	strne	r3, [r4, #8]
    e890:	1a00000e 	bne	e8d0 <iic_read_get_address+0xa4>
    e894:	ea000010 	b	e8dc <iic_read_get_address+0xb0>
    e898:	e1d430b2 	ldrh	r3, [r4, #2]
    e89c:	e2632c01 	rsb	r2, r3, #256	; 0x100
    e8a0:	e1520001 	cmp	r2, r1
    e8a4:	e5842004 	str	r2, [r4, #4]
    e8a8:	2a000005 	bcs	e8c4 <iic_read_get_address+0x98>
    e8ac:	e0623001 	rsb	r3, r2, r1
    e8b0:	e5843008 	str	r3, [r4, #8]
    e8b4:	e3a03001 	mov	r3, #1	; 0x1
    e8b8:	e1c431b0 	strh	r3, [r4, #16]
    e8bc:	e584200c 	str	r2, [r4, #12]
    e8c0:	ea000003 	b	e8d4 <iic_read_get_address+0xa8>
    e8c4:	e3510000 	cmp	r1, #0	; 0x0
    e8c8:	0a000003 	beq	e8dc <iic_read_get_address+0xb0>
    e8cc:	e5840008 	str	r0, [r4, #8]
    e8d0:	e584100c 	str	r1, [r4, #12]
    e8d4:	e3a00001 	mov	r0, #1	; 0x1
    e8d8:	e8bd8010 	pop	{r4, pc}
    e8dc:	e3a00000 	mov	r0, #0	; 0x0
    e8e0:	e8bd8010 	pop	{r4, pc}

0000e8e4 <iic_write_get_address>:
    e8e4:	e1d020b2 	ldrh	r2, [r0, #2]
    e8e8:	e590300c 	ldr	r3, [r0, #12]
    e8ec:	e92d4010 	push	{r4, lr}
    e8f0:	e0833002 	add	r3, r3, r2
    e8f4:	e1c030b2 	strh	r3, [r0, #2]
    e8f8:	e1a04000 	mov	r4, r0
    e8fc:	ebffffbf 	bl	e800 <iic_address_num>
    e900:	e3500000 	cmp	r0, #0	; 0x0
    e904:	1a000008 	bne	e92c <iic_write_get_address+0x48>
    e908:	e1d430b2 	ldrh	r3, [r4, #2]
    e90c:	e3530c01 	cmp	r3, #256	; 0x100
    e910:	1a000005 	bne	e92c <iic_write_get_address+0x48>
    e914:	e1d430b0 	ldrh	r3, [r4]
    e918:	e1c400b2 	strh	r0, [r4, #2]
    e91c:	e2833002 	add	r3, r3, #2	; 0x2
    e920:	e1c430b0 	strh	r3, [r4]
    e924:	e3a03001 	mov	r3, #1	; 0x1
    e928:	e1c431b0 	strh	r3, [r4, #16]
    e92c:	e1d430b2 	ldrh	r3, [r4, #2]
    e930:	e5940008 	ldr	r0, [r4, #8]
    e934:	e203300f 	and	r3, r3, #15	; 0xf
    e938:	e2632010 	rsb	r2, r3, #16	; 0x10
    e93c:	e1520000 	cmp	r2, r0
    e940:	e5842004 	str	r2, [r4, #4]
    e944:	2a000004 	bcs	e95c <iic_write_get_address+0x78>
    e948:	e0623000 	rsb	r3, r2, r0
    e94c:	e5843008 	str	r3, [r4, #8]
    e950:	e584200c 	str	r2, [r4, #12]
    e954:	e3a00001 	mov	r0, #1	; 0x1
    e958:	e8bd8010 	pop	{r4, pc}
    e95c:	e3500000 	cmp	r0, #0	; 0x0
    e960:	13a03000 	movne	r3, #0	; 0x0
    e964:	1584000c 	strne	r0, [r4, #12]
    e968:	15843008 	strne	r3, [r4, #8]
    e96c:	13a00001 	movne	r0, #1	; 0x1
    e970:	e8bd8010 	pop	{r4, pc}

0000e974 <iic_word_addr_func>:
    e974:	e92d4030 	push	{r4, r5, lr}
    e978:	e24dd004 	sub	sp, sp, #4	; 0x4
    e97c:	e1a04000 	mov	r4, r0
    e980:	e1a05001 	mov	r5, r1
    e984:	ebffff9d 	bl	e800 <iic_address_num>
    e988:	e3500000 	cmp	r0, #0	; 0x0
    e98c:	16ff0074 	uxthne	r0, r4
    e990:	1a00000c 	bne	e9c8 <iic_word_addr_func+0x54>
    e994:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    e998:	e0051003 	and	r1, r5, r3
    e99c:	e3a020ff 	mov	r2, #255	; 0xff
    e9a0:	e3510000 	cmp	r1, #0	; 0x0
    e9a4:	e0040002 	and	r0, r4, r2
    e9a8:	aa000005 	bge	e9c4 <iic_word_addr_func+0x50>
    e9ac:	e3e02000 	mvn	r2, #0	; 0x0
    e9b0:	e0900002 	adds	r0, r0, r2
    e9b4:	e3e020ff 	mvn	r2, #255	; 0xff
    e9b8:	e1800002 	orr	r0, r0, r2
    e9bc:	e3a02001 	mov	r2, #1	; 0x1
    e9c0:	e0900002 	adds	r0, r0, r2
    e9c4:	e6ff0070 	uxth	r0, r0
    e9c8:	e28dd004 	add	sp, sp, #4	; 0x4
    e9cc:	e8bd8030 	pop	{r4, r5, pc}

0000e9d0 <iic_addr_func>:
    e9d0:	e92d4030 	push	{r4, r5, lr}
    e9d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    e9d8:	e1a04000 	mov	r4, r0
    e9dc:	e1a05001 	mov	r5, r1
    e9e0:	ebffff86 	bl	e800 <iic_address_num>
    e9e4:	e3500000 	cmp	r0, #0	; 0x0
    e9e8:	e59f003c 	ldr	r0, [pc, #60]	; ea2c <iic_addr_func+0x5c>
    e9ec:	11d000b0 	ldrhne	r0, [r0]
    e9f0:	1a00000b 	bne	ea24 <iic_addr_func+0x54>
    e9f4:	e1a02005 	mov	r2, r5
    e9f8:	e1a03fc2 	asr	r3, r2, #31
    e9fc:	e1a02fc3 	asr	r2, r3, #31
    ea00:	e20220ff 	and	r2, r2, #255	; 0xff
    ea04:	e3a03000 	mov	r3, #0	; 0x0
    ea08:	e0922004 	adds	r2, r2, r4
    ea0c:	e0a33005 	adc	r3, r3, r5
    ea10:	e5900000 	ldr	r0, [r0]
    ea14:	e1a01422 	lsr	r1, r2, #8
    ea18:	e1811c03 	orr	r1, r1, r3, lsl #24
    ea1c:	e1800081 	orr	r0, r0, r1, lsl #1
    ea20:	e6ff0070 	uxth	r0, r0
    ea24:	e28dd004 	add	sp, sp, #4	; 0x4
    ea28:	e8bd8030 	pop	{r4, r5, pc}
    ea2c:	3c00a130 	.word	0x3c00a130

0000ea30 <e2p_vs_write>:
    ea30:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ea34:	e1a01003 	mov	r1, r3
    ea38:	e24dd024 	sub	sp, sp, #36	; 0x24
    ea3c:	e1a06000 	mov	r6, r0
    ea40:	e1a00002 	mov	r0, r2
    ea44:	e1a05003 	mov	r5, r3
    ea48:	e1a04002 	mov	r4, r2
    ea4c:	ebffffdf 	bl	e9d0 <iic_addr_func>
    ea50:	e1a01005 	mov	r1, r5
    ea54:	e1cd00bc 	strh	r0, [sp, #12]
    ea58:	e1a00004 	mov	r0, r4
    ea5c:	ebffffc4 	bl	e974 <iic_word_addr_func>
    ea60:	e3a03000 	mov	r3, #0	; 0x0
    ea64:	e59d2048 	ldr	r2, [sp, #72]
    ea68:	e1a05003 	mov	r5, r3
    ea6c:	e58d3018 	str	r3, [sp, #24]
    ea70:	e28dc00c 	add	ip, sp, #12	; 0xc
    ea74:	e3a03001 	mov	r3, #1	; 0x1
    ea78:	e1cd00be 	strh	r0, [sp, #14]
    ea7c:	e58d2014 	str	r2, [sp, #20]
    ea80:	e1cd31bc 	strh	r3, [sp, #28]
    ea84:	e59fb10c 	ldr	fp, [pc, #268]	; eb98 <e2p_vs_write+0x168>
    ea88:	e3a0a001 	mov	sl, #1	; 0x1
    ea8c:	e1a08005 	mov	r8, r5
    ea90:	e28c9012 	add	r9, ip, #18	; 0x12
    ea94:	ea000037 	b	eb78 <e2p_vs_write+0x148>
    ea98:	ebffff58 	bl	e800 <iic_address_num>
    ea9c:	e3500000 	cmp	r0, #0	; 0x0
    eaa0:	e1dd10be 	ldrh	r1, [sp, #14]
    eaa4:	11a03421 	lsrne	r3, r1, #8
    eaa8:	15cd301e 	strbne	r3, [sp, #30]
    eaac:	15cd101f 	strbne	r1, [sp, #31]
    eab0:	13a07002 	movne	r7, #2	; 0x2
    eab4:	1a000005 	bne	ead0 <e2p_vs_write+0xa0>
    eab8:	e6ef1071 	uxtb	r1, r1
    eabc:	e5cd101e 	strb	r1, [sp, #30]
    eac0:	e59f00d4 	ldr	r0, [pc, #212]	; eb9c <e2p_vs_write+0x16c>
    eac4:	e1a0e00f 	mov	lr, pc
    eac8:	e59bf000 	ldr	pc, [fp]
    eacc:	e3a07001 	mov	r7, #1	; 0x1
    ead0:	ebfffedd 	bl	e64c <get_ticks>
    ead4:	e1dd31bc 	ldrh	r3, [sp, #28]
    ead8:	e1a04000 	mov	r4, r0
    eadc:	e3530000 	cmp	r3, #0	; 0x0
    eae0:	0a000016 	beq	eb40 <e2p_vs_write+0x110>
    eae4:	e3a0c000 	mov	ip, #0	; 0x0
    eae8:	e5dd200c 	ldrb	r2, [sp, #12]
    eaec:	e1a00008 	mov	r0, r8
    eaf0:	e1a0100a 	mov	r1, sl
    eaf4:	e1a03008 	mov	r3, r8
    eaf8:	e1cdc1bc 	strh	ip, [sp, #28]
    eafc:	e58da000 	str	sl, [sp]
    eb00:	e58d8004 	str	r8, [sp, #4]
    eb04:	eb000136 	bl	efe4 <iic_init>
    eb08:	e3500000 	cmp	r0, #0	; 0x0
    eb0c:	1a00000b 	bne	eb40 <e2p_vs_write+0x110>
    eb10:	e1a00008 	mov	r0, r8
    eb14:	ea00001d 	b	eb90 <e2p_vs_write+0x160>
    eb18:	ebfffecb 	bl	e64c <get_ticks>
    eb1c:	e1540000 	cmp	r4, r0
    eb20:	2a000008 	bcs	eb48 <e2p_vs_write+0x118>
    eb24:	e59f0074 	ldr	r0, [pc, #116]	; eba0 <e2p_vs_write+0x170>
    eb28:	e1a0e00f 	mov	lr, pc
    eb2c:	e59bf000 	ldr	pc, [fp]
    eb30:	e59d3018 	ldr	r3, [sp, #24]
    eb34:	e0853003 	add	r3, r5, r3
    eb38:	e6ff5073 	uxth	r5, r3
    eb3c:	ea00000b 	b	eb70 <e2p_vs_write+0x140>
    eb40:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    eb44:	e2844e12 	add	r4, r4, #288	; 0x120
    eb48:	e59dc018 	ldr	ip, [sp, #24]
    eb4c:	e1a01009 	mov	r1, r9
    eb50:	e1a02007 	mov	r2, r7
    eb54:	e1a03006 	mov	r3, r6
    eb58:	e3a00000 	mov	r0, #0	; 0x0
    eb5c:	e58dc000 	str	ip, [sp]
    eb60:	e58da004 	str	sl, [sp, #4]
    eb64:	eb000078 	bl	ed4c <iicreg_write>
    eb68:	e3500000 	cmp	r0, #0	; 0x0
    eb6c:	0affffe9 	beq	eb18 <e2p_vs_write+0xe8>
    eb70:	e59d3018 	ldr	r3, [sp, #24]
    eb74:	e0866003 	add	r6, r6, r3
    eb78:	e28d000c 	add	r0, sp, #12	; 0xc
    eb7c:	ebffff58 	bl	e8e4 <iic_write_get_address>
    eb80:	e3500000 	cmp	r0, #0	; 0x0
    eb84:	1affffc3 	bne	ea98 <e2p_vs_write+0x68>
    eb88:	e59d2048 	ldr	r2, [sp, #72]
    eb8c:	e0650002 	rsb	r0, r5, r2
    eb90:	e28dd024 	add	sp, sp, #36	; 0x24
    eb94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    eb98:	3c00a5cc 	.word	0x3c00a5cc
    eb9c:	000191aa 	.word	0x000191aa
    eba0:	000191b4 	.word	0x000191b4

0000eba4 <e2p_vs_read>:
    eba4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    eba8:	e1a01003 	mov	r1, r3
    ebac:	e24dd024 	sub	sp, sp, #36	; 0x24
    ebb0:	e1a06000 	mov	r6, r0
    ebb4:	e1a00002 	mov	r0, r2
    ebb8:	e1a05003 	mov	r5, r3
    ebbc:	e1a04002 	mov	r4, r2
    ebc0:	e59d7048 	ldr	r7, [sp, #72]
    ebc4:	ebffff81 	bl	e9d0 <iic_addr_func>
    ebc8:	e1a01005 	mov	r1, r5
    ebcc:	e1cd00bc 	strh	r0, [sp, #12]
    ebd0:	e1a00004 	mov	r0, r4
    ebd4:	ebffff66 	bl	e974 <iic_word_addr_func>
    ebd8:	e28db00c 	add	fp, sp, #12	; 0xc
    ebdc:	e3a03000 	mov	r3, #0	; 0x0
    ebe0:	e3a02001 	mov	r2, #1	; 0x1
    ebe4:	e1cd00be 	strh	r0, [sp, #14]
    ebe8:	e1cd21bc 	strh	r2, [sp, #28]
    ebec:	e58d7014 	str	r7, [sp, #20]
    ebf0:	e58d3018 	str	r3, [sp, #24]
    ebf4:	e1a05003 	mov	r5, r3
    ebf8:	e3a09001 	mov	r9, #1	; 0x1
    ebfc:	e28ba012 	add	sl, fp, #18	; 0x12
    ec00:	ea00002b 	b	ecb4 <e2p_vs_read+0x110>
    ec04:	ebfffefd 	bl	e800 <iic_address_num>
    ec08:	e1dd20be 	ldrh	r2, [sp, #14]
    ec0c:	e3500000 	cmp	r0, #0	; 0x0
    ec10:	11a03422 	lsrne	r3, r2, #8
    ec14:	15cd301e 	strbne	r3, [sp, #30]
    ec18:	15cd201f 	strbne	r2, [sp, #31]
    ec1c:	13a08002 	movne	r8, #2	; 0x2
    ec20:	05cd201e 	strbeq	r2, [sp, #30]
    ec24:	03a08001 	moveq	r8, #1	; 0x1
    ec28:	ebfffe87 	bl	e64c <get_ticks>
    ec2c:	e1dd31bc 	ldrh	r3, [sp, #28]
    ec30:	e1a04000 	mov	r4, r0
    ec34:	e3530000 	cmp	r3, #0	; 0x0
    ec38:	0a000010 	beq	ec80 <e2p_vs_read+0xdc>
    ec3c:	e3a0c000 	mov	ip, #0	; 0x0
    ec40:	e5dd200c 	ldrb	r2, [sp, #12]
    ec44:	e1a00005 	mov	r0, r5
    ec48:	e1a01009 	mov	r1, r9
    ec4c:	e1a03005 	mov	r3, r5
    ec50:	e1cdc1bc 	strh	ip, [sp, #28]
    ec54:	e58d9000 	str	r9, [sp]
    ec58:	e58d5004 	str	r5, [sp, #4]
    ec5c:	eb0000e0 	bl	efe4 <iic_init>
    ec60:	e3500000 	cmp	r0, #0	; 0x0
    ec64:	1a000005 	bne	ec80 <e2p_vs_read+0xdc>
    ec68:	e1a07005 	mov	r7, r5
    ec6c:	ea000014 	b	ecc4 <e2p_vs_read+0x120>
    ec70:	ebfffe75 	bl	e64c <get_ticks>
    ec74:	e1540000 	cmp	r4, r0
    ec78:	2a000002 	bcs	ec88 <e2p_vs_read+0xe4>
    ec7c:	ea00000a 	b	ecac <e2p_vs_read+0x108>
    ec80:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    ec84:	e2844e12 	add	r4, r4, #288	; 0x120
    ec88:	e59dc018 	ldr	ip, [sp, #24]
    ec8c:	e1a0100a 	mov	r1, sl
    ec90:	e1a02008 	mov	r2, r8
    ec94:	e1a03006 	mov	r3, r6
    ec98:	e3a00000 	mov	r0, #0	; 0x0
    ec9c:	e58dc000 	str	ip, [sp]
    eca0:	eb000061 	bl	ee2c <iic_read_func>
    eca4:	e3500000 	cmp	r0, #0	; 0x0
    eca8:	0afffff0 	beq	ec70 <e2p_vs_read+0xcc>
    ecac:	e59d3018 	ldr	r3, [sp, #24]
    ecb0:	e0866003 	add	r6, r6, r3
    ecb4:	e1a0000b 	mov	r0, fp
    ecb8:	ebfffedb 	bl	e82c <iic_read_get_address>
    ecbc:	e3500000 	cmp	r0, #0	; 0x0
    ecc0:	1affffcf 	bne	ec04 <e2p_vs_read+0x60>
    ecc4:	e1a00007 	mov	r0, r7
    ecc8:	e28dd024 	add	sp, sp, #36	; 0x24
    eccc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

0000ecd0 <iicreg_read>:
    ecd0:	e3500001 	cmp	r0, #1	; 0x1
    ecd4:	e92d4070 	push	{r4, r5, r6, lr}
    ecd8:	83a00000 	movhi	r0, #0	; 0x0
    ecdc:	e1a05001 	mov	r5, r1
    ece0:	e1a04002 	mov	r4, r2
    ece4:	88bd8070 	pophi	{r4, r5, r6, pc}
    ece8:	e59f3058 	ldr	r3, [pc, #88]	; ed48 <iicreg_read+0x78>
    ecec:	e5932070 	ldr	r2, [r3, #112]
    ecf0:	e2022008 	and	r2, r2, #8	; 0x8
    ecf4:	ea000000 	b	ecfc <iicreg_read+0x2c>
    ecf8:	e5931010 	ldr	r1, [r3, #16]
    ecfc:	e3520000 	cmp	r2, #0	; 0x0
    ed00:	1afffffc 	bne	ecf8 <iicreg_read+0x28>
    ed04:	e59f103c 	ldr	r1, [pc, #60]	; ed48 <iicreg_read+0x78>
    ed08:	e1a00004 	mov	r0, r4
    ed0c:	e3a06c01 	mov	r6, #256	; 0x100
    ed10:	e591c070 	ldr	ip, [r1, #112]
    ed14:	e2503000 	subs	r3, r0, #0	; 0x0
    ed18:	13a03001 	movne	r3, #1	; 0x1
    ed1c:	e01330ac 	ands	r3, r3, ip, lsr #1
    ed20:	15816010 	strne	r6, [r1, #16]
    ed24:	12400001 	subne	r0, r0, #1	; 0x1
    ed28:	e31c0008 	tst	ip, #8	; 0x8
    ed2c:	15913010 	ldrne	r3, [r1, #16]
    ed30:	12822001 	addne	r2, r2, #1	; 0x1
    ed34:	14c53001 	strbne	r3, [r5], #1
    ed38:	e1520004 	cmp	r2, r4
    ed3c:	1afffff3 	bne	ed10 <iicreg_read+0x40>
    ed40:	e3a00001 	mov	r0, #1	; 0x1
    ed44:	e8bd8070 	pop	{r4, r5, r6, pc}
    ed48:	20300000 	.word	0x20300000

0000ed4c <iicreg_write>:
    ed4c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ed50:	e59d4018 	ldr	r4, [sp, #24]
    ed54:	e3500001 	cmp	r0, #1	; 0x1
    ed58:	e1a05001 	mov	r5, r1
    ed5c:	e5dd801c 	ldrb	r8, [sp, #28]
    ed60:	e1a01003 	mov	r1, r3
    ed64:	e0847002 	add	r7, r4, r2
    ed68:	8a00002c 	bhi	ee20 <iicreg_write+0xd4>
    ed6c:	e59f00b4 	ldr	r0, [pc, #180]	; ee28 <iicreg_write+0xdc>
    ed70:	e5903034 	ldr	r3, [r0, #52]
    ed74:	e213c040 	ands	ip, r3, #64	; 0x40
    ed78:	15903080 	ldrne	r3, [r0, #128]
    ed7c:	15903054 	ldrne	r3, [r0, #84]
    ed80:	13a00000 	movne	r0, #0	; 0x0
    ed84:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    ed88:	e5903070 	ldr	r3, [r0, #112]
    ed8c:	e3130004 	tst	r3, #4	; 0x4
    ed90:	11a06000 	movne	r6, r0
    ed94:	11a0000c 	movne	r0, ip
    ed98:	11a0c006 	movne	ip, r6
    ed9c:	1a00000d 	bne	edd8 <iicreg_write+0x8c>
    eda0:	ea00001e 	b	ee20 <iicreg_write+0xd4>
    eda4:	e5963070 	ldr	r3, [r6, #112]
    eda8:	e3130002 	tst	r3, #2	; 0x2
    edac:	0afffffc 	beq	eda4 <iicreg_write+0x58>
    edb0:	e3520000 	cmp	r2, #0	; 0x0
    edb4:	14d53001 	ldrbne	r3, [r5], #1
    edb8:	12422001 	subne	r2, r2, #1	; 0x1
    edbc:	1a000003 	bne	edd0 <iicreg_write+0x84>
    edc0:	e3540000 	cmp	r4, #0	; 0x0
    edc4:	0a000002 	beq	edd4 <iicreg_write+0x88>
    edc8:	e4d13001 	ldrb	r3, [r1], #1
    edcc:	e2444001 	sub	r4, r4, #1	; 0x1
    edd0:	e58c3010 	str	r3, [ip, #16]
    edd4:	e2800001 	add	r0, r0, #1	; 0x1
    edd8:	e1500007 	cmp	r0, r7
    eddc:	3afffff0 	bcc	eda4 <iicreg_write+0x58>
    ede0:	e59f2040 	ldr	r2, [pc, #64]	; ee28 <iicreg_write+0xdc>
    ede4:	e5923070 	ldr	r3, [r2, #112]
    ede8:	e3130004 	tst	r3, #4	; 0x4
    edec:	0afffffc 	beq	ede4 <iicreg_write+0x98>
    edf0:	e3580000 	cmp	r8, #0	; 0x0
    edf4:	159f202c 	ldrne	r2, [pc, #44]	; ee28 <iicreg_write+0xdc>
    edf8:	0a000006 	beq	ee18 <iicreg_write+0xcc>
    edfc:	e5923034 	ldr	r3, [r2, #52]
    ee00:	e3130c02 	tst	r3, #512	; 0x200
    ee04:	0afffffc 	beq	edfc <iicreg_write+0xb0>
    ee08:	e59f3018 	ldr	r3, [pc, #24]	; ee28 <iicreg_write+0xdc>
    ee0c:	e3010964 	movw	r0, #6500	; 0x1964
    ee10:	e5933060 	ldr	r3, [r3, #96]
    ee14:	ebfffe1d 	bl	e690 <udelay>
    ee18:	e3a00001 	mov	r0, #1	; 0x1
    ee1c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ee20:	e3a00000 	mov	r0, #0	; 0x0
    ee24:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ee28:	20300000 	.word	0x20300000

0000ee2c <iic_read_func>:
    ee2c:	e92d4070 	push	{r4, r5, r6, lr}
    ee30:	e59f4080 	ldr	r4, [pc, #128]	; eeb8 <iic_read_func+0x8c>
    ee34:	e1a06003 	mov	r6, r3
    ee38:	e5d43000 	ldrb	r3, [r4]
    ee3c:	e24dd008 	sub	sp, sp, #8	; 0x8
    ee40:	e243300a 	sub	r3, r3, #10	; 0xa
    ee44:	e6ef3073 	uxtb	r3, r3
    ee48:	e3530001 	cmp	r3, #1	; 0x1
    ee4c:	e1a05000 	mov	r5, r0
    ee50:	9a00000b 	bls	ee84 <iic_read_func+0x58>
    ee54:	e3a0e000 	mov	lr, #0	; 0x0
    ee58:	e1a0300e 	mov	r3, lr
    ee5c:	e3a0c001 	mov	ip, #1	; 0x1
    ee60:	e58dc004 	str	ip, [sp, #4]
    ee64:	e58de000 	str	lr, [sp]
    ee68:	ebffffb7 	bl	ed4c <iicreg_write>
    ee6c:	e3500000 	cmp	r0, #0	; 0x0
    ee70:	03a0300c 	moveq	r3, #12	; 0xc
    ee74:	05c43000 	strbeq	r3, [r4]
    ee78:	0a00000c 	beq	eeb0 <iic_read_func+0x84>
    ee7c:	e3a0300a 	mov	r3, #10	; 0xa
    ee80:	e5c43000 	strb	r3, [r4]
    ee84:	e59d2018 	ldr	r2, [sp, #24]
    ee88:	e1a00005 	mov	r0, r5
    ee8c:	e1a01006 	mov	r1, r6
    ee90:	ebffff8e 	bl	ecd0 <iicreg_read>
    ee94:	e59f201c 	ldr	r2, [pc, #28]	; eeb8 <iic_read_func+0x8c>
    ee98:	e3500000 	cmp	r0, #0	; 0x0
    ee9c:	13a0300d 	movne	r3, #13	; 0xd
    eea0:	03a0300b 	moveq	r3, #11	; 0xb
    eea4:	15c23000 	strbne	r3, [r2]
    eea8:	13a00001 	movne	r0, #1	; 0x1
    eeac:	05c23000 	strbeq	r3, [r2]
    eeb0:	e28dd008 	add	sp, sp, #8	; 0x8
    eeb4:	e8bd8070 	pop	{r4, r5, r6, pc}
    eeb8:	3c00a134 	.word	0x3c00a134

0000eebc <iic_writeread>:
    eebc:	e92d4070 	push	{r4, r5, r6, lr}
    eec0:	e59f4088 	ldr	r4, [pc, #136]	; ef50 <iic_writeread+0x94>
    eec4:	e24dd010 	sub	sp, sp, #16	; 0x10
    eec8:	e5d43000 	ldrb	r3, [r4]
    eecc:	e5cd100f 	strb	r1, [sp, #15]
    eed0:	e243300a 	sub	r3, r3, #10	; 0xa
    eed4:	e6ef3073 	uxtb	r3, r3
    eed8:	e3530001 	cmp	r3, #1	; 0x1
    eedc:	e1a06002 	mov	r6, r2
    eee0:	e1a05000 	mov	r5, r0
    eee4:	9a00000c 	bls	ef1c <iic_writeread+0x60>
    eee8:	e3a0c000 	mov	ip, #0	; 0x0
    eeec:	e1a0300c 	mov	r3, ip
    eef0:	e28d100f 	add	r1, sp, #15	; 0xf
    eef4:	e3a02001 	mov	r2, #1	; 0x1
    eef8:	e58dc000 	str	ip, [sp]
    eefc:	e58dc004 	str	ip, [sp, #4]
    ef00:	ebffff91 	bl	ed4c <iicreg_write>
    ef04:	e3500000 	cmp	r0, #0	; 0x0
    ef08:	03a0300c 	moveq	r3, #12	; 0xc
    ef0c:	05c43000 	strbeq	r3, [r4]
    ef10:	0a00000c 	beq	ef48 <iic_writeread+0x8c>
    ef14:	e3a0300a 	mov	r3, #10	; 0xa
    ef18:	e5c43000 	strb	r3, [r4]
    ef1c:	e3a02001 	mov	r2, #1	; 0x1
    ef20:	e1a00005 	mov	r0, r5
    ef24:	e1a01006 	mov	r1, r6
    ef28:	ebffff68 	bl	ecd0 <iicreg_read>
    ef2c:	e59f201c 	ldr	r2, [pc, #28]	; ef50 <iic_writeread+0x94>
    ef30:	e3500000 	cmp	r0, #0	; 0x0
    ef34:	13a0300d 	movne	r3, #13	; 0xd
    ef38:	03a0300b 	moveq	r3, #11	; 0xb
    ef3c:	15c23000 	strbne	r3, [r2]
    ef40:	13a00001 	movne	r0, #1	; 0x1
    ef44:	05c23000 	strbeq	r3, [r2]
    ef48:	e28dd010 	add	sp, sp, #16	; 0x10
    ef4c:	e8bd8070 	pop	{r4, r5, r6, pc}
    ef50:	3c00a134 	.word	0x3c00a134

0000ef54 <iicreg_init>:
    ef54:	e92d4030 	push	{r4, r5, lr}
    ef58:	e1a04000 	mov	r4, r0
    ef5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    ef60:	e3a00000 	mov	r0, #0	; 0x0
    ef64:	e1a05002 	mov	r5, r2
    ef68:	eb000a3a 	bl	11858 <module_get_clock>
    ef6c:	e3540001 	cmp	r4, #1	; 0x1
    ef70:	83a00000 	movhi	r0, #0	; 0x0
    ef74:	8a000016 	bhi	efd4 <iicreg_init+0x80>
    ef78:	e59f105c 	ldr	r1, [pc, #92]	; efdc <iicreg_init+0x88>
    ef7c:	eb001780 	bl	14d84 <__aeabi_uidiv>
    ef80:	e59f2058 	ldr	r2, [pc, #88]	; efe0 <iicreg_init+0x8c>
    ef84:	e1a010a0 	lsr	r1, r0, #1
    ef88:	e3a0c000 	mov	ip, #0	; 0x0
    ef8c:	e3a03043 	mov	r3, #67	; 0x43
    ef90:	e582c06c 	str	ip, [r2, #108]
    ef94:	e5823000 	str	r3, [r2]
    ef98:	e5820014 	str	r0, [r2, #20]
    ef9c:	e5820018 	str	r0, [r2, #24]
    efa0:	e58210a0 	str	r1, [r2, #160]
    efa4:	e59230a0 	ldr	r3, [r2, #160]
    efa8:	e1a000a5 	lsr	r0, r5, #1
    efac:	e3c33c01 	bic	r3, r3, #256	; 0x100
    efb0:	e58230a0 	str	r3, [r2, #160]
    efb4:	e5923000 	ldr	r3, [r2]
    efb8:	e3a01001 	mov	r1, #1	; 0x1
    efbc:	e3833020 	orr	r3, r3, #32	; 0x20
    efc0:	e5823000 	str	r3, [r2]
    efc4:	e582c030 	str	ip, [r2, #48]
    efc8:	e5820004 	str	r0, [r2, #4]
    efcc:	e582106c 	str	r1, [r2, #108]
    efd0:	e1a00001 	mov	r0, r1
    efd4:	e28dd004 	add	sp, sp, #4	; 0x4
    efd8:	e8bd8030 	pop	{r4, r5, pc}
    efdc:	0009c400 	.word	0x0009c400
    efe0:	20300000 	.word	0x20300000

0000efe4 <iic_init>:
    efe4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    efe8:	e59fc07c 	ldr	ip, [pc, #124]	; f06c <iic_init+0x88>
    efec:	e24dd004 	sub	sp, sp, #4	; 0x4
    eff0:	e3a0e00d 	mov	lr, #13	; 0xd
    eff4:	e1a0a000 	mov	sl, r0
    eff8:	e59f0070 	ldr	r0, [pc, #112]	; f070 <iic_init+0x8c>
    effc:	e5cce000 	strb	lr, [ip]
    f000:	e1a08003 	mov	r8, r3
    f004:	e1a04001 	mov	r4, r1
    f008:	e1a05002 	mov	r5, r2
    f00c:	e5dd6020 	ldrb	r6, [sp, #32]
    f010:	e5dd7024 	ldrb	r7, [sp, #36]
    f014:	ebffc69f 	bl	a98 <module_enable>
    f018:	e3a01000 	mov	r1, #0	; 0x0
    f01c:	e59f0050 	ldr	r0, [pc, #80]	; f074 <iic_init+0x90>
    f020:	e1a02001 	mov	r2, r1
    f024:	ebffc6f8 	bl	c0c <pads_chmod>
    f028:	e3a00302 	mov	r0, #134217728	; 0x8000000
    f02c:	ebffc809 	bl	1058 <ecfg_check_flag>
    f030:	e3500000 	cmp	r0, #0	; 0x0
    f034:	0a000003 	beq	f048 <iic_init+0x64>
    f038:	e3a01001 	mov	r1, #1	; 0x1
    f03c:	e59f0030 	ldr	r0, [pc, #48]	; f074 <iic_init+0x90>
    f040:	e1a02001 	mov	r2, r1
    f044:	ebffc74f 	bl	d88 <pads_pull>
    f048:	e1a0000a 	mov	r0, sl
    f04c:	e1a01004 	mov	r1, r4
    f050:	e1a02005 	mov	r2, r5
    f054:	e1a03008 	mov	r3, r8
    f058:	e58d6020 	str	r6, [sp, #32]
    f05c:	e58d7024 	str	r7, [sp, #36]
    f060:	e28dd004 	add	sp, sp, #4	; 0x4
    f064:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    f068:	eaffffb9 	b	ef54 <iicreg_init>
    f06c:	3c00a134 	.word	0x3c00a134
    f070:	21e10c00 	.word	0x21e10c00
    f074:	002e002d 	.word	0x002e002d

0000f078 <spi_set_bytes>:
    f078:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    f07c:	e59f3020 	ldr	r3, [pc, #32]	; f0a4 <spi_set_bytes+0x2c>
    f080:	e1a01000 	mov	r1, r0
    f084:	e5830000 	str	r0, [r3]
    f088:	e24dd004 	sub	sp, sp, #4	; 0x4
    f08c:	e59f0014 	ldr	r0, [pc, #20]	; f0a8 <spi_set_bytes+0x30>
    f090:	e59f3014 	ldr	r3, [pc, #20]	; f0ac <spi_set_bytes+0x34>
    f094:	e1a0e00f 	mov	lr, pc
    f098:	e593f000 	ldr	pc, [r3]
    f09c:	e28dd004 	add	sp, sp, #4	; 0x4
    f0a0:	e8bd8000 	pop	{pc}
    f0a4:	3c00a02c 	.word	0x3c00a02c
    f0a8:	000191bb 	.word	0x000191bb
    f0ac:	3c00a5cc 	.word	0x3c00a5cc

0000f0b0 <spi_fifo_clear>:
    f0b0:	e59f2028 	ldr	r2, [pc, #40]	; f0e0 <spi_fifo_clear+0x30>
    f0b4:	e592300c 	ldr	r3, [r2, #12]
    f0b8:	e3130010 	tst	r3, #16	; 0x10
    f0bc:	1afffffc 	bne	f0b4 <spi_fifo_clear+0x4>
    f0c0:	ea000001 	b	f0cc <spi_fifo_clear+0x1c>
    f0c4:	e5923008 	ldr	r3, [r2, #8]
    f0c8:	ea000000 	b	f0d0 <spi_fifo_clear+0x20>
    f0cc:	e59f200c 	ldr	r2, [pc, #12]	; f0e0 <spi_fifo_clear+0x30>
    f0d0:	e592300c 	ldr	r3, [r2, #12]
    f0d4:	e3130004 	tst	r3, #4	; 0x4
    f0d8:	1afffff9 	bne	f0c4 <spi_fifo_clear+0x14>
    f0dc:	e12fff1e 	bx	lr
    f0e0:	20500000 	.word	0x20500000

0000f0e4 <flash_vs_align>:
    f0e4:	e3a00c01 	mov	r0, #256	; 0x100
    f0e8:	e12fff1e 	bx	lr

0000f0ec <spi_exchange>:
    f0ec:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f0f0:	e5905008 	ldr	r5, [r0, #8]
    f0f4:	e3510001 	cmp	r1, #1	; 0x1
    f0f8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f0fc:	e1a0a000 	mov	sl, r0
    f100:	e5908004 	ldr	r8, [r0, #4]
    f104:	d1a04005 	movle	r4, r5
    f108:	d3a07000 	movle	r7, #0	; 0x0
    f10c:	da000020 	ble	f194 <spi_exchange+0xa8>
    f110:	e280a00c 	add	sl, r0, #12	; 0xc
    f114:	e1a01008 	mov	r1, r8
    f118:	e1a0000d 	mov	r0, sp
    f11c:	e1a02005 	mov	r2, r5
    f120:	ebffc8cf 	bl	1464 <memcpy>
    f124:	e99a0012 	ldmib	sl, {r1, r4}
    f128:	e0843005 	add	r3, r4, r5
    f12c:	e3530008 	cmp	r3, #8	; 0x8
    f130:	a3a03008 	movge	r3, #8	; 0x8
    f134:	e6ef6073 	uxtb	r6, r3
    f138:	e3510000 	cmp	r1, #0	; 0x0
    f13c:	e1a0700d 	mov	r7, sp
    f140:	059a8000 	ldreq	r8, [sl]
    f144:	00667084 	rsbeq	r7, r6, r4, lsl #1
    f148:	0a000008 	beq	f170 <spi_exchange+0x84>
    f14c:	e0654006 	rsb	r4, r5, r6
    f150:	e0870005 	add	r0, r7, r5
    f154:	e1a02004 	mov	r2, r4
    f158:	ebffc8c1 	bl	1464 <memcpy>
    f15c:	e99a000c 	ldmib	sl, {r2, r3}
    f160:	e0855003 	add	r5, r5, r3
    f164:	e0828004 	add	r8, r2, r4
    f168:	e3a07000 	mov	r7, #0	; 0x0
    f16c:	e0664005 	rsb	r4, r6, r5
    f170:	e3a02000 	mov	r2, #0	; 0x0
    f174:	e1a0000d 	mov	r0, sp
    f178:	e59f126c 	ldr	r1, [pc, #620]	; f3ec <spi_exchange+0x300>
    f17c:	ea000002 	b	f18c <spi_exchange+0xa0>
    f180:	e7d03002 	ldrb	r3, [r0, r2]
    f184:	e2822001 	add	r2, r2, #1	; 0x1
    f188:	e5813008 	str	r3, [r1, #8]
    f18c:	e1520006 	cmp	r2, r6
    f190:	bafffffa 	blt	f180 <spi_exchange+0x94>
    f194:	e59f6250 	ldr	r6, [pc, #592]	; f3ec <spi_exchange+0x300>
    f198:	e3a0b005 	mov	fp, #5	; 0x5
    f19c:	e5969018 	ldr	r9, [r6, #24]
    f1a0:	e3190004 	tst	r9, #4	; 0x4
    f1a4:	0a000030 	beq	f26c <spi_exchange+0x180>
    f1a8:	e3550003 	cmp	r5, #3	; 0x3
    f1ac:	da000005 	ble	f1c8 <spi_exchange+0xdc>
    f1b0:	e5963008 	ldr	r3, [r6, #8]
    f1b4:	e2455004 	sub	r5, r5, #4	; 0x4
    f1b8:	e5963008 	ldr	r3, [r6, #8]
    f1bc:	e5963008 	ldr	r3, [r6, #8]
    f1c0:	e5963008 	ldr	r3, [r6, #8]
    f1c4:	ea000028 	b	f26c <spi_exchange+0x180>
    f1c8:	e3550000 	cmp	r5, #0	; 0x0
    f1cc:	0a000003 	beq	f1e0 <spi_exchange+0xf4>
    f1d0:	e2555001 	subs	r5, r5, #1	; 0x1
    f1d4:	e5963008 	ldr	r3, [r6, #8]
    f1d8:	1afffffc 	bne	f1d0 <spi_exchange+0xe4>
    f1dc:	ea000022 	b	f26c <spi_exchange+0x180>
    f1e0:	e59a3000 	ldr	r3, [sl]
    f1e4:	e3530000 	cmp	r3, #0	; 0x0
    f1e8:	1a000004 	bne	f200 <spi_exchange+0x114>
    f1ec:	e59f01fc 	ldr	r0, [pc, #508]	; f3f0 <spi_exchange+0x304>
    f1f0:	e59f31fc 	ldr	r3, [pc, #508]	; f3f4 <spi_exchange+0x308>
    f1f4:	e1a0e00f 	mov	lr, pc
    f1f8:	e593f000 	ldr	pc, [r3]
    f1fc:	ea00001a 	b	f26c <spi_exchange+0x180>
    f200:	e3540003 	cmp	r4, #3	; 0x3
    f204:	da00000e 	ble	f244 <spi_exchange+0x158>
    f208:	e5962008 	ldr	r2, [r6, #8]
    f20c:	e1a03008 	mov	r3, r8
    f210:	e4c32001 	strb	r2, [r3], #1
    f214:	e5962008 	ldr	r2, [r6, #8]
    f218:	e2831001 	add	r1, r3, #1	; 0x1
    f21c:	e5c82001 	strb	r2, [r8, #1]
    f220:	e5962008 	ldr	r2, [r6, #8]
    f224:	e2544004 	subs	r4, r4, #4	; 0x4
    f228:	e5c32001 	strb	r2, [r3, #1]
    f22c:	e5963008 	ldr	r3, [r6, #8]
    f230:	e2812001 	add	r2, r1, #1	; 0x1
    f234:	e5c13001 	strb	r3, [r1, #1]
    f238:	0a00005e 	beq	f3b8 <spi_exchange+0x2cc>
    f23c:	e2828001 	add	r8, r2, #1	; 0x1
    f240:	ea000009 	b	f26c <spi_exchange+0x180>
    f244:	e3540000 	cmp	r4, #0	; 0x0
    f248:	0a00005a 	beq	f3b8 <spi_exchange+0x2cc>
    f24c:	e59f2198 	ldr	r2, [pc, #408]	; f3ec <spi_exchange+0x300>
    f250:	e1a01005 	mov	r1, r5
    f254:	e5923008 	ldr	r3, [r2, #8]
    f258:	e7c83001 	strb	r3, [r8, r1]
    f25c:	e2811001 	add	r1, r1, #1	; 0x1
    f260:	e1510004 	cmp	r1, r4
    f264:	1afffffa 	bne	f254 <spi_exchange+0x168>
    f268:	ea000052 	b	f3b8 <spi_exchange+0x2cc>
    f26c:	e3190008 	tst	r9, #8	; 0x8
    f270:	0a00002a 	beq	f320 <spi_exchange+0x234>
    f274:	e59a3004 	ldr	r3, [sl, #4]
    f278:	e3530000 	cmp	r3, #0	; 0x0
    f27c:	0a000019 	beq	f2e8 <spi_exchange+0x1fc>
    f280:	e3540003 	cmp	r4, #3	; 0x3
    f284:	da00000d 	ble	f2c0 <spi_exchange+0x1d4>
    f288:	e1a03008 	mov	r3, r8
    f28c:	e4d32001 	ldrb	r2, [r3], #1
    f290:	e2544004 	subs	r4, r4, #4	; 0x4
    f294:	e5862008 	str	r2, [r6, #8]
    f298:	e5d82001 	ldrb	r2, [r8, #1]
    f29c:	e2831001 	add	r1, r3, #1	; 0x1
    f2a0:	e5862008 	str	r2, [r6, #8]
    f2a4:	e5d33001 	ldrb	r3, [r3, #1]
    f2a8:	e2818002 	add	r8, r1, #2	; 0x2
    f2ac:	e5863008 	str	r3, [r6, #8]
    f2b0:	e5d13001 	ldrb	r3, [r1, #1]
    f2b4:	e5863008 	str	r3, [r6, #8]
    f2b8:	1a000018 	bne	f320 <spi_exchange+0x234>
    f2bc:	ea00003d 	b	f3b8 <spi_exchange+0x2cc>
    f2c0:	e3540000 	cmp	r4, #0	; 0x0
    f2c4:	0a00003b 	beq	f3b8 <spi_exchange+0x2cc>
    f2c8:	e59f111c 	ldr	r1, [pc, #284]	; f3ec <spi_exchange+0x300>
    f2cc:	e3a02000 	mov	r2, #0	; 0x0
    f2d0:	e7d83002 	ldrb	r3, [r8, r2]
    f2d4:	e2822001 	add	r2, r2, #1	; 0x1
    f2d8:	e1520004 	cmp	r2, r4
    f2dc:	e5813008 	str	r3, [r1, #8]
    f2e0:	1afffffa 	bne	f2d0 <spi_exchange+0x1e4>
    f2e4:	ea000033 	b	f3b8 <spi_exchange+0x2cc>
    f2e8:	e3570003 	cmp	r7, #3	; 0x3
    f2ec:	da000005 	ble	f308 <spi_exchange+0x21c>
    f2f0:	e586b008 	str	fp, [r6, #8]
    f2f4:	e2477004 	sub	r7, r7, #4	; 0x4
    f2f8:	e586b008 	str	fp, [r6, #8]
    f2fc:	e586b008 	str	fp, [r6, #8]
    f300:	e586b008 	str	fp, [r6, #8]
    f304:	ea000005 	b	f320 <spi_exchange+0x234>
    f308:	e3570000 	cmp	r7, #0	; 0x0
    f30c:	0a000003 	beq	f320 <spi_exchange+0x234>
    f310:	e3a03005 	mov	r3, #5	; 0x5
    f314:	e2577001 	subs	r7, r7, #1	; 0x1
    f318:	e5863008 	str	r3, [r6, #8]
    f31c:	1afffffb 	bne	f310 <spi_exchange+0x224>
    f320:	e3190002 	tst	r9, #2	; 0x2
    f324:	0affff9c 	beq	f19c <spi_exchange+0xb0>
    f328:	ea000009 	b	f354 <spi_exchange+0x268>
    f32c:	e596300c 	ldr	r3, [r6, #12]
    f330:	e2455001 	sub	r5, r5, #1	; 0x1
    f334:	e3130004 	tst	r3, #4	; 0x4
    f338:	15963008 	ldrne	r3, [r6, #8]
    f33c:	1a000006 	bne	f35c <spi_exchange+0x270>
    f340:	e59f00b0 	ldr	r0, [pc, #176]	; f3f8 <spi_exchange+0x30c>
    f344:	e1a01005 	mov	r1, r5
    f348:	e1a0e00f 	mov	lr, pc
    f34c:	e597f000 	ldr	pc, [r7]
    f350:	ea000001 	b	f35c <spi_exchange+0x270>
    f354:	e59f6090 	ldr	r6, [pc, #144]	; f3ec <spi_exchange+0x300>
    f358:	e59f7094 	ldr	r7, [pc, #148]	; f3f4 <spi_exchange+0x308>
    f35c:	e3550000 	cmp	r5, #0	; 0x0
    f360:	1afffff1 	bne	f32c <spi_exchange+0x240>
    f364:	ea00000a 	b	f394 <spi_exchange+0x2a8>
    f368:	e595300c 	ldr	r3, [r5, #12]
    f36c:	e2444001 	sub	r4, r4, #1	; 0x1
    f370:	e3130004 	tst	r3, #4	; 0x4
    f374:	15953008 	ldrne	r3, [r5, #8]
    f378:	14c83001 	strbne	r3, [r8], #1
    f37c:	1a000006 	bne	f39c <spi_exchange+0x2b0>
    f380:	e59f0074 	ldr	r0, [pc, #116]	; f3fc <spi_exchange+0x310>
    f384:	e1a01004 	mov	r1, r4
    f388:	e1a0e00f 	mov	lr, pc
    f38c:	e596f000 	ldr	pc, [r6]
    f390:	ea000001 	b	f39c <spi_exchange+0x2b0>
    f394:	e59f5050 	ldr	r5, [pc, #80]	; f3ec <spi_exchange+0x300>
    f398:	e59f6054 	ldr	r6, [pc, #84]	; f3f4 <spi_exchange+0x308>
    f39c:	e59a3000 	ldr	r3, [sl]
    f3a0:	e3530000 	cmp	r3, #0	; 0x0
    f3a4:	13540000 	cmpne	r4, #0	; 0x0
    f3a8:	1affffee 	bne	f368 <spi_exchange+0x27c>
    f3ac:	e59f3038 	ldr	r3, [pc, #56]	; f3ec <spi_exchange+0x300>
    f3b0:	e3a02003 	mov	r2, #3	; 0x3
    f3b4:	e5832020 	str	r2, [r3, #32]
    f3b8:	e59f202c 	ldr	r2, [pc, #44]	; f3ec <spi_exchange+0x300>
    f3bc:	e592300c 	ldr	r3, [r2, #12]
    f3c0:	e3130010 	tst	r3, #16	; 0x10
    f3c4:	1afffffc 	bne	f3bc <spi_exchange+0x2d0>
    f3c8:	ea000001 	b	f3d4 <spi_exchange+0x2e8>
    f3cc:	e5923008 	ldr	r3, [r2, #8]
    f3d0:	ea000000 	b	f3d8 <spi_exchange+0x2ec>
    f3d4:	e59f2010 	ldr	r2, [pc, #16]	; f3ec <spi_exchange+0x300>
    f3d8:	e592300c 	ldr	r3, [r2, #12]
    f3dc:	e3130004 	tst	r3, #4	; 0x4
    f3e0:	1afffff9 	bne	f3cc <spi_exchange+0x2e0>
    f3e4:	e28dd00c 	add	sp, sp, #12	; 0xc
    f3e8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f3ec:	20500000 	.word	0x20500000
    f3f0:	000191d2 	.word	0x000191d2
    f3f4:	3c00a5cc 	.word	0x3c00a5cc
    f3f8:	000191e9 	.word	0x000191e9
    f3fc:	00019201 	.word	0x00019201

0000f400 <flash_vs_read>:
    f400:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    f404:	e24dd024 	sub	sp, sp, #36	; 0x24
    f408:	e28d4018 	add	r4, sp, #24	; 0x18
    f40c:	e2841003 	add	r1, r4, #3	; 0x3
    f410:	e59da040 	ldr	sl, [sp, #64]
    f414:	e58d1004 	str	r1, [sp, #4]
    f418:	e3a01005 	mov	r1, #5	; 0x5
    f41c:	e3a0c000 	mov	ip, #0	; 0x0
    f420:	e58d1008 	str	r1, [sp, #8]
    f424:	e2811006 	add	r1, r1, #6	; 0x6
    f428:	e58dc018 	str	ip, [sp, #24]
    f42c:	e2845004 	add	r5, r4, #4	; 0x4
    f430:	e58d000c 	str	r0, [sp, #12]
    f434:	e58dc010 	str	ip, [sp, #16]
    f438:	e5cd101b 	strb	r1, [sp, #27]
    f43c:	e1a06002 	mov	r6, r2
    f440:	e1a07003 	mov	r7, r3
    f444:	e58dc000 	str	ip, [sp]
    f448:	e1a0400a 	mov	r4, sl
    f44c:	e1a0800d 	mov	r8, sp
    f450:	ea00000c 	b	f488 <flash_vs_read+0x88>
    f454:	e3540a01 	cmp	r4, #4096	; 0x1000
    f458:	b1a03004 	movlt	r3, r4
    f45c:	a3a03a01 	movge	r3, #4096	; 0x1000
    f460:	e5852000 	str	r2, [r5]
    f464:	e58d3014 	str	r3, [sp, #20]
    f468:	ebffff1f 	bl	f0ec <spi_exchange>
    f46c:	e59d2014 	ldr	r2, [sp, #20]
    f470:	e59d300c 	ldr	r3, [sp, #12]
    f474:	e0966002 	adds	r6, r6, r2
    f478:	e0833002 	add	r3, r3, r2
    f47c:	e0a77fc2 	adc	r7, r7, r2, asr #31
    f480:	e58d300c 	str	r3, [sp, #12]
    f484:	e0624004 	rsb	r4, r2, r4
    f488:	e1a02c06 	lsl	r2, r6, #24
    f48c:	e2061cff 	and	r1, r6, #65280	; 0xff00
    f490:	e1822c26 	orr	r2, r2, r6, lsr #24
    f494:	e1822401 	orr	r2, r2, r1, lsl #8
    f498:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    f49c:	e1822423 	orr	r2, r2, r3, lsr #8
    f4a0:	e3540000 	cmp	r4, #0	; 0x0
    f4a4:	e1a02442 	asr	r2, r2, #8
    f4a8:	e1a0000d 	mov	r0, sp
    f4ac:	e3a01002 	mov	r1, #2	; 0x2
    f4b0:	1affffe7 	bne	f454 <flash_vs_read+0x54>
    f4b4:	e1a0000a 	mov	r0, sl
    f4b8:	e28dd024 	add	sp, sp, #36	; 0x24
    f4bc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

0000f4c0 <flash_poll>:
    f4c0:	e92d4030 	push	{r4, r5, lr}
    f4c4:	e24dd024 	sub	sp, sp, #36	; 0x24
    f4c8:	e28dc018 	add	ip, sp, #24	; 0x18
    f4cc:	e28c3001 	add	r3, ip, #1	; 0x1
    f4d0:	e3a02000 	mov	r2, #0	; 0x0
    f4d4:	e3a01001 	mov	r1, #1	; 0x1
    f4d8:	e58d300c 	str	r3, [sp, #12]
    f4dc:	e3a03005 	mov	r3, #5	; 0x5
    f4e0:	e58d2018 	str	r2, [sp, #24]
    f4e4:	e58d2010 	str	r2, [sp, #16]
    f4e8:	e58d1014 	str	r1, [sp, #20]
    f4ec:	e5cd3018 	strb	r3, [sp, #24]
    f4f0:	e1a04000 	mov	r4, r0
    f4f4:	e58d201c 	str	r2, [sp, #28]
    f4f8:	e88d1004 	stm	sp, {r2, ip}
    f4fc:	e58d1008 	str	r1, [sp, #8]
    f500:	e1a0500d 	mov	r5, sp
    f504:	e1a0000d 	mov	r0, sp
    f508:	e3a01002 	mov	r1, #2	; 0x2
    f50c:	ebfffef6 	bl	f0ec <spi_exchange>
    f510:	e5dd3019 	ldrb	r3, [sp, #25]
    f514:	e3540000 	cmp	r4, #0	; 0x0
    f518:	11a030a3 	lsrne	r3, r3, #1
    f51c:	12233001 	eorne	r3, r3, #1	; 0x1
    f520:	e2033001 	and	r3, r3, #1	; 0x1
    f524:	e3530000 	cmp	r3, #0	; 0x0
    f528:	1afffff5 	bne	f504 <flash_poll+0x44>
    f52c:	e28dd024 	add	sp, sp, #36	; 0x24
    f530:	e8bd8030 	pop	{r4, r5, pc}

0000f534 <flash_wren>:
    f534:	e92d4010 	push	{r4, lr}
    f538:	e24dd020 	sub	sp, sp, #32	; 0x20
    f53c:	e3a04001 	mov	r4, #1	; 0x1
    f540:	e28dc018 	add	ip, sp, #24	; 0x18
    f544:	e08c3004 	add	r3, ip, r4
    f548:	e3a02000 	mov	r2, #0	; 0x0
    f54c:	e58d3010 	str	r3, [sp, #16]
    f550:	e3a03006 	mov	r3, #6	; 0x6
    f554:	e58d2018 	str	r2, [sp, #24]
    f558:	e1a0000d 	mov	r0, sp
    f55c:	e5cd3018 	strb	r3, [sp, #24]
    f560:	e1a01004 	mov	r1, r4
    f564:	e3a03004 	mov	r3, #4	; 0x4
    f568:	e58d200c 	str	r2, [sp, #12]
    f56c:	e5cd3019 	strb	r3, [sp, #25]
    f570:	e58d201c 	str	r2, [sp, #28]
    f574:	e88d1004 	stm	sp, {r2, ip}
    f578:	e58d4008 	str	r4, [sp, #8]
    f57c:	e58d4014 	str	r4, [sp, #20]
    f580:	ebfffed9 	bl	f0ec <spi_exchange>
    f584:	e1a00004 	mov	r0, r4
    f588:	ebffffcc 	bl	f4c0 <flash_poll>
    f58c:	e28dd020 	add	sp, sp, #32	; 0x20
    f590:	e8bd8010 	pop	{r4, pc}

0000f594 <flash_vs_erase>:
    f594:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f598:	e24dd034 	sub	sp, sp, #52	; 0x34
    f59c:	e28d4028 	add	r4, sp, #40	; 0x28
    f5a0:	e3a06001 	mov	r6, #1	; 0x1
    f5a4:	e59f109c 	ldr	r1, [pc, #156]	; f648 <flash_vs_erase+0xb4>
    f5a8:	e3a02008 	mov	r2, #8	; 0x8
    f5ac:	e1a00004 	mov	r0, r4
    f5b0:	ebffc7ab 	bl	1464 <memcpy>
    f5b4:	e0843006 	add	r3, r4, r6
    f5b8:	e2842003 	add	r2, r4, #3	; 0x3
    f5bc:	e3a05000 	mov	r5, #0	; 0x0
    f5c0:	e28d7004 	add	r7, sp, #4	; 0x4
    f5c4:	e58d3008 	str	r3, [sp, #8]
    f5c8:	e3a03002 	mov	r3, #2	; 0x2
    f5cc:	e58d300c 	str	r3, [sp, #12]
    f5d0:	e58d2014 	str	r2, [sp, #20]
    f5d4:	e58d4020 	str	r4, [sp, #32]
    f5d8:	e58d5004 	str	r5, [sp, #4]
    f5dc:	e58d5010 	str	r5, [sp, #16]
    f5e0:	e58d6018 	str	r6, [sp, #24]
    f5e4:	e58d501c 	str	r5, [sp, #28]
    f5e8:	e58d6024 	str	r6, [sp, #36]
    f5ec:	ebffffd0 	bl	f534 <flash_wren>
    f5f0:	e1a01006 	mov	r1, r6
    f5f4:	e1a00007 	mov	r0, r7
    f5f8:	ebfffebb 	bl	f0ec <spi_exchange>
    f5fc:	e1a00005 	mov	r0, r5
    f600:	ebffffae 	bl	f4c0 <flash_poll>
    f604:	e2870018 	add	r0, r7, #24	; 0x18
    f608:	e1a01006 	mov	r1, r6
    f60c:	ebfffeb6 	bl	f0ec <spi_exchange>
    f610:	e1a01006 	mov	r1, r6
    f614:	e1a00007 	mov	r0, r7
    f618:	ebfffeb3 	bl	f0ec <spi_exchange>
    f61c:	e1a00005 	mov	r0, r5
    f620:	ebffffa6 	bl	f4c0 <flash_poll>
    f624:	ebffffc2 	bl	f534 <flash_wren>
    f628:	e1a01006 	mov	r1, r6
    f62c:	e287000c 	add	r0, r7, #12	; 0xc
    f630:	ebfffead 	bl	f0ec <spi_exchange>
    f634:	e1a00005 	mov	r0, r5
    f638:	ebffffa0 	bl	f4c0 <flash_poll>
    f63c:	e1a00005 	mov	r0, r5
    f640:	e28dd034 	add	sp, sp, #52	; 0x34
    f644:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f648:	00016576 	.word	0x00016576

0000f64c <flash_vs_write>:
    f64c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    f650:	e3a01004 	mov	r1, #4	; 0x4
    f654:	e24dd020 	sub	sp, sp, #32	; 0x20
    f658:	e3a0c000 	mov	ip, #0	; 0x0
    f65c:	e59d9040 	ldr	r9, [sp, #64]
    f660:	e58d1008 	str	r1, [sp, #8]
    f664:	e2811002 	add	r1, r1, #2	; 0x2
    f668:	e58dc018 	str	ip, [sp, #24]
    f66c:	e28d701c 	add	r7, sp, #28	; 0x1c
    f670:	e5cd1018 	strb	r1, [sp, #24]
    f674:	e3a01002 	mov	r1, #2	; 0x2
    f678:	e58dc00c 	str	ip, [sp, #12]
    f67c:	e58d0010 	str	r0, [sp, #16]
    f680:	e5cd101b 	strb	r1, [sp, #27]
    f684:	e1a04002 	mov	r4, r2
    f688:	e1a05003 	mov	r5, r3
    f68c:	e58dc000 	str	ip, [sp]
    f690:	e58d7004 	str	r7, [sp, #4]
    f694:	e1a06009 	mov	r6, r9
    f698:	e59fa080 	ldr	sl, [pc, #128]	; f720 <flash_vs_write+0xd4>
    f69c:	e1a0800d 	mov	r8, sp
    f6a0:	ea000011 	b	f6ec <flash_vs_write+0xa0>
    f6a4:	e59a3000 	ldr	r3, [sl]
    f6a8:	e5872000 	str	r2, [r7]
    f6ac:	e1560003 	cmp	r6, r3
    f6b0:	d58d6014 	strle	r6, [sp, #20]
    f6b4:	c58d3014 	strgt	r3, [sp, #20]
    f6b8:	ebffff9d 	bl	f534 <flash_wren>
    f6bc:	e1a0000d 	mov	r0, sp
    f6c0:	e3a01002 	mov	r1, #2	; 0x2
    f6c4:	ebfffe88 	bl	f0ec <spi_exchange>
    f6c8:	e3a00000 	mov	r0, #0	; 0x0
    f6cc:	ebffff7b 	bl	f4c0 <flash_poll>
    f6d0:	e59d2014 	ldr	r2, [sp, #20]
    f6d4:	e59d3010 	ldr	r3, [sp, #16]
    f6d8:	e0944002 	adds	r4, r4, r2
    f6dc:	e0833002 	add	r3, r3, r2
    f6e0:	e0a55fc2 	adc	r5, r5, r2, asr #31
    f6e4:	e58d3010 	str	r3, [sp, #16]
    f6e8:	e0626006 	rsb	r6, r2, r6
    f6ec:	e1a02c04 	lsl	r2, r4, #24
    f6f0:	e1822c24 	orr	r2, r2, r4, lsr #24
    f6f4:	e2041cff 	and	r1, r4, #65280	; 0xff00
    f6f8:	e1822401 	orr	r2, r2, r1, lsl #8
    f6fc:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    f700:	e1822423 	orr	r2, r2, r3, lsr #8
    f704:	e3c220ff 	bic	r2, r2, #255	; 0xff
    f708:	e3560000 	cmp	r6, #0	; 0x0
    f70c:	e3822002 	orr	r2, r2, #2	; 0x2
    f710:	1affffe3 	bne	f6a4 <flash_vs_write+0x58>
    f714:	e1a00009 	mov	r0, r9
    f718:	e28dd020 	add	sp, sp, #32	; 0x20
    f71c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    f720:	3c00a02c 	.word	0x3c00a02c

0000f724 <spi_init_pads>:
    f724:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    f728:	e3a01000 	mov	r1, #0	; 0x0
    f72c:	e24dd004 	sub	sp, sp, #4	; 0x4
    f730:	e59f0040 	ldr	r0, [pc, #64]	; f778 <spi_init_pads+0x54>
    f734:	e1a02001 	mov	r2, r1
    f738:	ebffc533 	bl	c0c <pads_chmod>
    f73c:	e3a00402 	mov	r0, #33554432	; 0x2000000
    f740:	ebffc644 	bl	1058 <ecfg_check_flag>
    f744:	e3500000 	cmp	r0, #0	; 0x0
    f748:	0a000007 	beq	f76c <spi_init_pads+0x48>
    f74c:	e3a01001 	mov	r1, #1	; 0x1
    f750:	e1a02001 	mov	r2, r1
    f754:	e59f001c 	ldr	r0, [pc, #28]	; f778 <spi_init_pads+0x54>
    f758:	ebffc58a 	bl	d88 <pads_pull>
    f75c:	e3a00070 	mov	r0, #112	; 0x70
    f760:	e3a01001 	mov	r1, #1	; 0x1
    f764:	e3a02000 	mov	r2, #0	; 0x0
    f768:	ebffc586 	bl	d88 <pads_pull>
    f76c:	e3a00000 	mov	r0, #0	; 0x0
    f770:	e28dd004 	add	sp, sp, #4	; 0x4
    f774:	e8bd8000 	pop	{pc}
    f778:	0071006e 	.word	0x0071006e

0000f77c <spi_init_clk>:
    f77c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    f780:	e3a00000 	mov	r0, #0	; 0x0
    f784:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f788:	eb000832 	bl	11858 <module_get_clock>
    f78c:	e3a05002 	mov	r5, #2	; 0x2
    f790:	e59fa0d8 	ldr	sl, [pc, #216]	; f870 <spi_init_clk+0xf4>
    f794:	e1a08000 	mov	r8, r0
    f798:	ea00000c 	b	f7d0 <spi_init_clk+0x54>
    f79c:	e1a06004 	mov	r6, r4
    f7a0:	e1a01007 	mov	r1, r7
    f7a4:	e1a00008 	mov	r0, r8
    f7a8:	eb0015f7 	bl	14f8c <__aeabi_idiv>
    f7ac:	e150000a 	cmp	r0, sl
    f7b0:	e0877005 	add	r7, r7, r5
    f7b4:	e2864001 	add	r4, r6, #1	; 0x1
    f7b8:	da000007 	ble	f7dc <spi_init_clk+0x60>
    f7bc:	e3540c01 	cmp	r4, #256	; 0x100
    f7c0:	1afffff5 	bne	f79c <spi_init_clk+0x20>
    f7c4:	e2855002 	add	r5, r5, #2	; 0x2
    f7c8:	e3550c01 	cmp	r5, #256	; 0x100
    f7cc:	0a000003 	beq	f7e0 <spi_init_clk+0x64>
    f7d0:	e1a07005 	mov	r7, r5
    f7d4:	e3a06000 	mov	r6, #0	; 0x0
    f7d8:	eafffff0 	b	f7a0 <spi_init_clk+0x24>
    f7dc:	e1a04006 	mov	r4, r6
    f7e0:	e35400ff 	cmp	r4, #255	; 0xff
    f7e4:	d35500fe 	cmple	r5, #254	; 0xfe
    f7e8:	da000007 	ble	f80c <spi_init_clk+0x90>
    f7ec:	e1a01005 	mov	r1, r5
    f7f0:	e1a02004 	mov	r2, r4
    f7f4:	e59f0078 	ldr	r0, [pc, #120]	; f874 <spi_init_clk+0xf8>
    f7f8:	e59f3078 	ldr	r3, [pc, #120]	; f878 <spi_init_clk+0xfc>
    f7fc:	e1a0e00f 	mov	lr, pc
    f800:	e593f000 	ldr	pc, [r3]
    f804:	e3a05002 	mov	r5, #2	; 0x2
    f808:	e3a04027 	mov	r4, #39	; 0x27
    f80c:	e2841001 	add	r1, r4, #1	; 0x1
    f810:	e0010195 	mul	r1, r5, r1
    f814:	e1a00008 	mov	r0, r8
    f818:	eb0015db 	bl	14f8c <__aeabi_idiv>
    f81c:	e1a01008 	mov	r1, r8
    f820:	e58d0000 	str	r0, [sp]
    f824:	e1a02005 	mov	r2, r5
    f828:	e1a03004 	mov	r3, r4
    f82c:	e59f0048 	ldr	r0, [pc, #72]	; f87c <spi_init_clk+0x100>
    f830:	e59fc040 	ldr	ip, [pc, #64]	; f878 <spi_init_clk+0xfc>
    f834:	e1a0e00f 	mov	lr, pc
    f838:	e59cf000 	ldr	pc, [ip]
    f83c:	e59f203c 	ldr	r2, [pc, #60]	; f880 <spi_init_clk+0x104>
    f840:	e3a03000 	mov	r3, #0	; 0x0
    f844:	e1a01404 	lsl	r1, r4, #8
    f848:	e5823004 	str	r3, [r2, #4]
    f84c:	e38110c7 	orr	r1, r1, #199	; 0xc7
    f850:	e5823014 	str	r3, [r2, #20]
    f854:	e1a00003 	mov	r0, r3
    f858:	e283300a 	add	r3, r3, #10	; 0xa
    f85c:	e5825010 	str	r5, [r2, #16]
    f860:	e5821000 	str	r1, [r2]
    f864:	e5823004 	str	r3, [r2, #4]
    f868:	e28dd00c 	add	sp, sp, #12	; 0xc
    f86c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    f870:	00551928 	.word	0x00551928
    f874:	00019219 	.word	0x00019219
    f878:	3c00a5cc 	.word	0x3c00a5cc
    f87c:	00019247 	.word	0x00019247
    f880:	20500000 	.word	0x20500000

0000f884 <flash_vs_reset>:
    f884:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    f888:	e59f0040 	ldr	r0, [pc, #64]	; f8d0 <flash_vs_reset+0x4c>
    f88c:	e24dd004 	sub	sp, sp, #4	; 0x4
    f890:	ebffc480 	bl	a98 <module_enable>
    f894:	ebffffb8 	bl	f77c <spi_init_clk>
    f898:	ebffffa1 	bl	f724 <spi_init_pads>
    f89c:	e59f2030 	ldr	r2, [pc, #48]	; f8d4 <flash_vs_reset+0x50>
    f8a0:	e592300c 	ldr	r3, [r2, #12]
    f8a4:	e3130010 	tst	r3, #16	; 0x10
    f8a8:	1afffffc 	bne	f8a0 <flash_vs_reset+0x1c>
    f8ac:	ea000001 	b	f8b8 <flash_vs_reset+0x34>
    f8b0:	e5923008 	ldr	r3, [r2, #8]
    f8b4:	ea000000 	b	f8bc <flash_vs_reset+0x38>
    f8b8:	e59f2014 	ldr	r2, [pc, #20]	; f8d4 <flash_vs_reset+0x50>
    f8bc:	e592300c 	ldr	r3, [r2, #12]
    f8c0:	e2130004 	ands	r0, r3, #4	; 0x4
    f8c4:	1afffff9 	bne	f8b0 <flash_vs_reset+0x2c>
    f8c8:	e28dd004 	add	sp, sp, #4	; 0x4
    f8cc:	e8bd8000 	pop	{pc}
    f8d0:	21e11400 	.word	0x21e11400
    f8d4:	20500000 	.word	0x20500000

0000f8d8 <ram_vs_align>:
    f8d8:	e3a00c02 	mov	r0, #512	; 0x200
    f8dc:	e12fff1e 	bx	lr

0000f8e0 <ram_vs_reset>:
    f8e0:	e3a00000 	mov	r0, #0	; 0x0
    f8e4:	e12fff1e 	bx	lr

0000f8e8 <ram_vs_write>:
    f8e8:	e92d4010 	push	{r4, lr}
    f8ec:	e59d4008 	ldr	r4, [sp, #8]
    f8f0:	e1a01000 	mov	r1, r0
    f8f4:	e2820102 	add	r0, r2, #-2147483648	; 0x80000000
    f8f8:	e1a02004 	mov	r2, r4
    f8fc:	ebffc6d8 	bl	1464 <memcpy>
    f900:	e1a00004 	mov	r0, r4
    f904:	e8bd8010 	pop	{r4, pc}

0000f908 <ram_vs_read>:
    f908:	e92d4010 	push	{r4, lr}
    f90c:	e59d4008 	ldr	r4, [sp, #8]
    f910:	e2821102 	add	r1, r2, #-2147483648	; 0x80000000
    f914:	e1a02004 	mov	r2, r4
    f918:	ebffc6d1 	bl	1464 <memcpy>
    f91c:	e1a00004 	mov	r0, r4
    f920:	e8bd8010 	pop	{r4, pc}

0000f924 <CreateAtaCFIS>:
    f924:	e3a0c000 	mov	ip, #0	; 0x0
    f928:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    f92c:	e1a05003 	mov	r5, r3
    f930:	e1a04000 	mov	r4, r0
    f934:	e1a06001 	mov	r6, r1
    f938:	e1a08002 	mov	r8, r2
    f93c:	e1a0300c 	mov	r3, ip
    f940:	e7c4300c 	strb	r3, [r4, ip]
    f944:	e28cc001 	add	ip, ip, #1	; 0x1
    f948:	e35c0014 	cmp	ip, #20	; 0x14
    f94c:	e3a07000 	mov	r7, #0	; 0x0
    f950:	1afffffa 	bne	f940 <CreateAtaCFIS+0x1c>
    f954:	e5d43001 	ldrb	r3, [r4, #1]
    f958:	e59d2018 	ldr	r2, [sp, #24]
    f95c:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    f960:	e1520007 	cmp	r2, r7
    f964:	e6ef3073 	uxtb	r3, r3
    f968:	03a02080 	moveq	r2, #128	; 0x80
    f96c:	11a02007 	movne	r2, r7
    f970:	e3c33080 	bic	r3, r3, #128	; 0x80
    f974:	e35600ca 	cmp	r6, #202	; 0xca
    f978:	135600c8 	cmpne	r6, #200	; 0xc8
    f97c:	e1822003 	orr	r2, r2, r3
    f980:	e3a03027 	mov	r3, #39	; 0x27
    f984:	e5c43000 	strb	r3, [r4]
    f988:	01a03c25 	lsreq	r3, r5, #24
    f98c:	e1a0c428 	lsr	ip, r8, #8
    f990:	e1a01425 	lsr	r1, r5, #8
    f994:	e1a00825 	lsr	r0, r5, #16
    f998:	0203300f 	andeq	r3, r3, #15	; 0xf
    f99c:	e5c47007 	strb	r7, [r4, #7]
    f9a0:	e5c42001 	strb	r2, [r4, #1]
    f9a4:	e5c4700b 	strb	r7, [r4, #11]
    f9a8:	e5c4c00d 	strb	ip, [r4, #13]
    f9ac:	e5c41005 	strb	r1, [r4, #5]
    f9b0:	e5c40006 	strb	r0, [r4, #6]
    f9b4:	e5c46002 	strb	r6, [r4, #2]
    f9b8:	e5c47003 	strb	r7, [r4, #3]
    f9bc:	e5c4800c 	strb	r8, [r4, #12]
    f9c0:	e5c45004 	strb	r5, [r4, #4]
    f9c4:	05c43007 	strbeq	r3, [r4, #7]
    f9c8:	0a000003 	beq	f9dc <CreateAtaCFIS+0xb8>
    f9cc:	e3560035 	cmp	r6, #53	; 0x35
    f9d0:	13560025 	cmpne	r6, #37	; 0x25
    f9d4:	01a03c25 	lsreq	r3, r5, #24
    f9d8:	05c43008 	strbeq	r3, [r4, #8]
    f9dc:	e59d201c 	ldr	r2, [sp, #28]
    f9e0:	e5d43007 	ldrb	r3, [r4, #7]
    f9e4:	e3520000 	cmp	r2, #0	; 0x0
    f9e8:	e3a01000 	mov	r1, #0	; 0x0
    f9ec:	13a02004 	movne	r2, #4	; 0x4
    f9f0:	03a02000 	moveq	r2, #0	; 0x0
    f9f4:	e3833040 	orr	r3, r3, #64	; 0x40
    f9f8:	e35600ec 	cmp	r6, #236	; 0xec
    f9fc:	e5c43007 	strb	r3, [r4, #7]
    fa00:	e5c4200f 	strb	r2, [r4, #15]
    fa04:	e5c41009 	strb	r1, [r4, #9]
    fa08:	e5c4100a 	strb	r1, [r4, #10]
    fa0c:	e5c4100e 	strb	r1, [r4, #14]
    fa10:	e5841010 	str	r1, [r4, #16]
    fa14:	05c41007 	strbeq	r1, [r4, #7]
    fa18:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000fa1c <CreateCommandHeader>:
    fa1c:	e92d4070 	push	{r4, r5, r6, lr}
    fa20:	e59d6018 	ldr	r6, [sp, #24]
    fa24:	e1a04000 	mov	r4, r0
    fa28:	e1a00001 	mov	r0, r1
    fa2c:	e3a01000 	mov	r1, #0	; 0x0
    fa30:	e1a05003 	mov	r5, r3
    fa34:	e084c280 	add	ip, r4, r0, lsl #5
    fa38:	e1a03001 	mov	r3, r1
    fa3c:	e7c1300c 	strb	r3, [r1, ip]
    fa40:	e2811001 	add	r1, r1, #1	; 0x1
    fa44:	e3510020 	cmp	r1, #32	; 0x20
    fa48:	1afffffb 	bne	fa3c <CreateCommandHeader+0x20>
    fa4c:	e3a03000 	mov	r3, #0	; 0x0
    fa50:	e58c2008 	str	r2, [ip, #8]
    fa54:	e58c300c 	str	r3, [ip, #12]
    fa58:	e1cc50b2 	strh	r5, [ip, #2]
    fa5c:	e1560003 	cmp	r6, r3
    fa60:	e7d43280 	ldrb	r3, [r4, r0, lsl #5]
    fa64:	13c3301c 	bicne	r3, r3, #28	; 0x1c
    fa68:	03c3301a 	biceq	r3, r3, #26	; 0x1a
    fa6c:	13833003 	orrne	r3, r3, #3	; 0x3
    fa70:	03833005 	orreq	r3, r3, #5	; 0x5
    fa74:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    fa78:	e59d3010 	ldr	r3, [sp, #16]
    fa7c:	e7d42280 	ldrb	r2, [r4, r0, lsl #5]
    fa80:	e3530000 	cmp	r3, #0	; 0x0
    fa84:	13a03020 	movne	r3, #32	; 0x20
    fa88:	03a03000 	moveq	r3, #0	; 0x0
    fa8c:	e3c22020 	bic	r2, r2, #32	; 0x20
    fa90:	e1833002 	orr	r3, r3, r2
    fa94:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    fa98:	e6ef2073 	uxtb	r2, r3
    fa9c:	e59d3014 	ldr	r3, [sp, #20]
    faa0:	e3c22040 	bic	r2, r2, #64	; 0x40
    faa4:	e3530000 	cmp	r3, #0	; 0x0
    faa8:	13a03040 	movne	r3, #64	; 0x40
    faac:	03a03000 	moveq	r3, #0	; 0x0
    fab0:	e1833002 	orr	r3, r3, r2
    fab4:	e6ef3073 	uxtb	r3, r3
    fab8:	e3c33080 	bic	r3, r3, #128	; 0x80
    fabc:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    fac0:	e5dc3001 	ldrb	r3, [ip, #1]
    fac4:	e3560000 	cmp	r6, #0	; 0x0
    fac8:	e3c330f0 	bic	r3, r3, #240	; 0xf0
    facc:	e6ef3073 	uxtb	r3, r3
    fad0:	13a01002 	movne	r1, #2	; 0x2
    fad4:	03a01000 	moveq	r1, #0	; 0x0
    fad8:	e3c33002 	bic	r3, r3, #2	; 0x2
    fadc:	e59d001c 	ldr	r0, [sp, #28]
    fae0:	e1811003 	orr	r1, r1, r3
    fae4:	e6ef3071 	uxtb	r3, r1
    fae8:	e2500000 	subs	r0, r0, #0	; 0x0
    faec:	13a00001 	movne	r0, #1	; 0x1
    faf0:	e3c33001 	bic	r3, r3, #1	; 0x1
    faf4:	e1803003 	orr	r3, r0, r3
    faf8:	e6ef3073 	uxtb	r3, r3
    fafc:	e3c33004 	bic	r3, r3, #4	; 0x4
    fb00:	e3a02000 	mov	r2, #0	; 0x0
    fb04:	e1833100 	orr	r3, r3, r0, lsl #2
    fb08:	e58c201c 	str	r2, [ip, #28]
    fb0c:	e58c2004 	str	r2, [ip, #4]
    fb10:	e5cc3001 	strb	r3, [ip, #1]
    fb14:	e58c2010 	str	r2, [ip, #16]
    fb18:	e58c2014 	str	r2, [ip, #20]
    fb1c:	e58c2018 	str	r2, [ip, #24]
    fb20:	e8bd8070 	pop	{r4, r5, r6, pc}

0000fb24 <get_max_lba>:
    fb24:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    fb28:	e3a04000 	mov	r4, #0	; 0x0
    fb2c:	e2807c05 	add	r7, r0, #1280	; 0x500
    fb30:	e1a06000 	mov	r6, r0
    fb34:	e24dd010 	sub	sp, sp, #16	; 0x10
    fb38:	e3a010ec 	mov	r1, #236	; 0xec
    fb3c:	e3a02001 	mov	r2, #1	; 0x1
    fb40:	e1a03004 	mov	r3, r4
    fb44:	e1a00007 	mov	r0, r7
    fb48:	e58d4000 	str	r4, [sp]
    fb4c:	e58d4004 	str	r4, [sp, #4]
    fb50:	e2868b01 	add	r8, r6, #1024	; 0x400
    fb54:	ebffff72 	bl	f924 <CreateAtaCFIS>
    fb58:	e2862b02 	add	r2, r6, #2048	; 0x800
    fb5c:	e2863d16 	add	r3, r6, #1408	; 0x580
    fb60:	e1a01004 	mov	r1, r4
    fb64:	e7c31004 	strb	r1, [r3, r4]
    fb68:	e2844001 	add	r4, r4, #1	; 0x1
    fb6c:	e3540010 	cmp	r4, #16	; 0x10
    fb70:	e3a05000 	mov	r5, #0	; 0x0
    fb74:	1afffffa 	bne	fb64 <get_max_lba+0x40>
    fb78:	e5872080 	str	r2, [r7, #128]
    fb7c:	e596258c 	ldr	r2, [r6, #1420]
    fb80:	e5865584 	str	r5, [r6, #1412]
    fb84:	e1a03b22 	lsr	r3, r2, #22
    fb88:	e1a03b03 	lsl	r3, r3, #22
    fb8c:	e1e024a3 	mvn	r2, r3, lsr #9
    fb90:	e1e02482 	mvn	r2, r2, lsl #9
    fb94:	e1a03c22 	lsr	r3, r2, #24
    fb98:	e3c33080 	bic	r3, r3, #128	; 0x80
    fb9c:	e586258c 	str	r2, [r6, #1420]
    fba0:	e1a01005 	mov	r1, r5
    fba4:	e5c6358f 	strb	r3, [r6, #1423]
    fba8:	e1a02007 	mov	r2, r7
    fbac:	e3a03001 	mov	r3, #1	; 0x1
    fbb0:	e1a00006 	mov	r0, r6
    fbb4:	e58d5000 	str	r5, [sp]
    fbb8:	e58d5004 	str	r5, [sp, #4]
    fbbc:	e58d5008 	str	r5, [sp, #8]
    fbc0:	e58d500c 	str	r5, [sp, #12]
    fbc4:	ebffff94 	bl	fa1c <CreateCommandHeader>
    fbc8:	e1a01005 	mov	r1, r5
    fbcc:	e1a03005 	mov	r3, r5
    fbd0:	e7c83001 	strb	r3, [r8, r1]
    fbd4:	e2811001 	add	r1, r1, #1	; 0x1
    fbd8:	e3510c01 	cmp	r1, #256	; 0x100
    fbdc:	1afffffb 	bne	fbd0 <get_max_lba+0xac>
    fbe0:	e59f20c8 	ldr	r2, [pc, #200]	; fcb0 <get_max_lba+0x18c>
    fbe4:	e3e03000 	mvn	r3, #0	; 0x0
    fbe8:	e5826100 	str	r6, [r2, #256]
    fbec:	e5828108 	str	r8, [r2, #264]
    fbf0:	e5823110 	str	r3, [r2, #272]
    fbf4:	e5823130 	str	r3, [r2, #304]
    fbf8:	e5923118 	ldr	r3, [r2, #280]
    fbfc:	e1a00002 	mov	r0, r2
    fc00:	e3833010 	orr	r3, r3, #16	; 0x10
    fc04:	e5823118 	str	r3, [r2, #280]
    fc08:	e5923118 	ldr	r3, [r2, #280]
    fc0c:	e3833001 	orr	r3, r3, #1	; 0x1
    fc10:	e5823118 	str	r3, [r2, #280]
    fc14:	e3a03001 	mov	r3, #1	; 0x1
    fc18:	e5823138 	str	r3, [r2, #312]
    fc1c:	e5903110 	ldr	r3, [r0, #272]
    fc20:	e59f1088 	ldr	r1, [pc, #136]	; fcb0 <get_max_lba+0x18c>
    fc24:	e3130002 	tst	r3, #2	; 0x2
    fc28:	0afffffb 	beq	fc1c <get_max_lba+0xf8>
    fc2c:	e3e03000 	mvn	r3, #0	; 0x0
    fc30:	e5912008 	ldr	r2, [r1, #8]
    fc34:	e5813110 	str	r3, [r1, #272]
    fc38:	e5813008 	str	r3, [r1, #8]
    fc3c:	e5912118 	ldr	r2, [r1, #280]
    fc40:	e30c3011 	movw	r3, #49169	; 0xc011
    fc44:	e0023003 	and	r3, r2, r3
    fc48:	e3530000 	cmp	r3, #0	; 0x0
    fc4c:	e596e8c8 	ldr	lr, [r6, #2248]
    fc50:	e59648cc 	ldr	r4, [r6, #2252]
    fc54:	0a00000e 	beq	fc94 <get_max_lba+0x170>
    fc58:	e5913118 	ldr	r3, [r1, #280]
    fc5c:	e1a0c001 	mov	ip, r1
    fc60:	e3c33001 	bic	r3, r3, #1	; 0x1
    fc64:	e5813118 	str	r3, [r1, #280]
    fc68:	e59c3118 	ldr	r3, [ip, #280]
    fc6c:	e59f203c 	ldr	r2, [pc, #60]	; fcb0 <get_max_lba+0x18c>
    fc70:	e3130902 	tst	r3, #32768	; 0x8000
    fc74:	1afffffb 	bne	fc68 <get_max_lba+0x144>
    fc78:	e5923118 	ldr	r3, [r2, #280]
    fc7c:	e1a01002 	mov	r1, r2
    fc80:	e3c33010 	bic	r3, r3, #16	; 0x10
    fc84:	e5823118 	str	r3, [r2, #280]
    fc88:	e5913118 	ldr	r3, [r1, #280]
    fc8c:	e3130901 	tst	r3, #16384	; 0x4000
    fc90:	1afffffc 	bne	fc88 <get_max_lba+0x164>
    fc94:	e59f2014 	ldr	r2, [pc, #20]	; fcb0 <get_max_lba+0x18c>
    fc98:	e5923120 	ldr	r3, [r2, #288]
    fc9c:	e2130088 	ands	r0, r3, #136	; 0x88
    fca0:	1afffffc 	bne	fc98 <get_max_lba+0x174>
    fca4:	e08e0000 	add	r0, lr, r0
    fca8:	e28dd010 	add	sp, sp, #16	; 0x10
    fcac:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    fcb0:	28200000 	.word	0x28200000

0000fcb4 <sata_vs_align>:
    fcb4:	e3a00c02 	mov	r0, #512	; 0x200
    fcb8:	e12fff1e 	bx	lr

0000fcbc <init_sata>:
    fcbc:	e12fff1e 	bx	lr

0000fcc0 <sata_vs_write>:
    fcc0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    fcc4:	e1a08000 	mov	r8, r0
    fcc8:	e24dd034 	sub	sp, sp, #52	; 0x34
    fccc:	e3a00c0e 	mov	r0, #3584	; 0xe00
    fcd0:	e1a06002 	mov	r6, r2
    fcd4:	e1a07003 	mov	r7, r3
    fcd8:	ebffc646 	bl	15f8 <malloc>
    fcdc:	e3500000 	cmp	r0, #0	; 0x0
    fce0:	e58d0014 	str	r0, [sp, #20]
    fce4:	1a000004 	bne	fcfc <sata_vs_write+0x3c>
    fce8:	e59f0680 	ldr	r0, [pc, #1664]	; 10370 <sata_vs_write+0x6b0>
    fcec:	e59f3680 	ldr	r3, [pc, #1664]	; 10374 <sata_vs_write+0x6b4>
    fcf0:	e1a0e00f 	mov	lr, pc
    fcf4:	e593f000 	ldr	pc, [r3]
    fcf8:	ea0000ea 	b	100a8 <sata_vs_write+0x3e8>
    fcfc:	e59d0014 	ldr	r0, [sp, #20]
    fd00:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    fd04:	e2833003 	add	r3, r3, #3	; 0x3
    fd08:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    fd0c:	e3c33003 	bic	r3, r3, #3	; 0x3
    fd10:	e1530000 	cmp	r3, r0
    fd14:	e58d3018 	str	r3, [sp, #24]
    fd18:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    fd1c:	058d3018 	streq	r3, [sp, #24]
    fd20:	e59d0018 	ldr	r0, [sp, #24]
    fd24:	ebffff7e 	bl	fb24 <get_max_lba>
    fd28:	e59d1058 	ldr	r1, [sp, #88]
    fd2c:	e1a0c000 	mov	ip, r0
    fd30:	e1a03b81 	lsl	r3, r1, #23
    fd34:	e1a03ba3 	lsr	r3, r3, #23
    fd38:	e3530000 	cmp	r3, #0	; 0x0
    fd3c:	1a0000d3 	bne	10090 <sata_vs_write+0x3d0>
    fd40:	e30041ff 	movw	r4, #511	; 0x1ff
    fd44:	e3a05000 	mov	r5, #0	; 0x0
    fd48:	e0062004 	and	r2, r6, r4
    fd4c:	e0073005 	and	r3, r7, r5
    fd50:	e1920003 	orrs	r0, r2, r3
    fd54:	1a0000cd 	bne	10090 <sata_vs_write+0x3d0>
    fd58:	e3180001 	tst	r8, #1	; 0x1
    fd5c:	1a0000cb 	bne	10090 <sata_vs_write+0x3d0>
    fd60:	e1a00007 	mov	r0, r7
    fd64:	e1a01fc0 	asr	r1, r0, #31
    fd68:	e1a02fc1 	asr	r2, r1, #31
    fd6c:	e1a03002 	mov	r3, r2
    fd70:	e0022004 	and	r2, r2, r4
    fd74:	e0922006 	adds	r2, r2, r6
    fd78:	e0033005 	and	r3, r3, r5
    fd7c:	e0a33007 	adc	r3, r3, r7
    fd80:	e1a014a2 	lsr	r1, r2, #9
    fd84:	e1a0600c 	mov	r6, ip
    fd88:	e1a07fc6 	asr	r7, r6, #31
    fd8c:	e1811b83 	orr	r1, r1, r3, lsl #23
    fd90:	e3570000 	cmp	r7, #0	; 0x0
    fd94:	e58d1028 	str	r1, [sp, #40]
    fd98:	e1a00001 	mov	r0, r1
    fd9c:	e3a01000 	mov	r1, #0	; 0x0
    fda0:	8a000002 	bhi	fdb0 <sata_vs_write+0xf0>
    fda4:	1a0000b9 	bne	10090 <sata_vs_write+0x3d0>
    fda8:	e1560000 	cmp	r6, r0
    fdac:	9a0000b7 	bls	10090 <sata_vs_write+0x3d0>
    fdb0:	e59d2058 	ldr	r2, [sp, #88]
    fdb4:	e3520402 	cmp	r2, #33554432	; 0x2000000
    fdb8:	ba000007 	blt	fddc <sata_vs_write+0x11c>
    fdbc:	e1a02006 	mov	r2, r6
    fdc0:	e1a03007 	mov	r3, r7
    fdc4:	e0522000 	subs	r2, r2, r0
    fdc8:	e0c33001 	sbc	r3, r3, r1
    fdcc:	e3530000 	cmp	r3, #0	; 0x0
    fdd0:	1a000001 	bne	fddc <sata_vs_write+0x11c>
    fdd4:	e3520801 	cmp	r2, #65536	; 0x10000
    fdd8:	3a0000ac 	bcc	10090 <sata_vs_write+0x3d0>
    fddc:	e59d3058 	ldr	r3, [sp, #88]
    fde0:	e3530401 	cmp	r3, #16777216	; 0x1000000
    fde4:	ba000007 	blt	fe08 <sata_vs_write+0x148>
    fde8:	e1a02006 	mov	r2, r6
    fdec:	e1a03007 	mov	r3, r7
    fdf0:	e0522000 	subs	r2, r2, r0
    fdf4:	e0c33001 	sbc	r3, r3, r1
    fdf8:	e3530000 	cmp	r3, #0	; 0x0
    fdfc:	1a000001 	bne	fe08 <sata_vs_write+0x148>
    fe00:	e3520902 	cmp	r2, #32768	; 0x8000
    fe04:	3a0000a1 	bcc	10090 <sata_vs_write+0x3d0>
    fe08:	e59dc058 	ldr	ip, [sp, #88]
    fe0c:	e35c0502 	cmp	ip, #8388608	; 0x800000
    fe10:	ba000007 	blt	fe34 <sata_vs_write+0x174>
    fe14:	e1a02006 	mov	r2, r6
    fe18:	e1a03007 	mov	r3, r7
    fe1c:	e0522000 	subs	r2, r2, r0
    fe20:	e0c33001 	sbc	r3, r3, r1
    fe24:	e3530000 	cmp	r3, #0	; 0x0
    fe28:	1a000001 	bne	fe34 <sata_vs_write+0x174>
    fe2c:	e3520901 	cmp	r2, #16384	; 0x4000
    fe30:	3a000096 	bcc	10090 <sata_vs_write+0x3d0>
    fe34:	e59d2058 	ldr	r2, [sp, #88]
    fe38:	e3520501 	cmp	r2, #4194304	; 0x400000
    fe3c:	ba000007 	blt	fe60 <sata_vs_write+0x1a0>
    fe40:	e1a02006 	mov	r2, r6
    fe44:	e1a03007 	mov	r3, r7
    fe48:	e0522000 	subs	r2, r2, r0
    fe4c:	e0c33001 	sbc	r3, r3, r1
    fe50:	e3530000 	cmp	r3, #0	; 0x0
    fe54:	1a000001 	bne	fe60 <sata_vs_write+0x1a0>
    fe58:	e3520a02 	cmp	r2, #8192	; 0x2000
    fe5c:	3a00008b 	bcc	10090 <sata_vs_write+0x3d0>
    fe60:	e59d3058 	ldr	r3, [sp, #88]
    fe64:	e3530602 	cmp	r3, #2097152	; 0x200000
    fe68:	ba000007 	blt	fe8c <sata_vs_write+0x1cc>
    fe6c:	e1a02006 	mov	r2, r6
    fe70:	e1a03007 	mov	r3, r7
    fe74:	e0522000 	subs	r2, r2, r0
    fe78:	e0c33001 	sbc	r3, r3, r1
    fe7c:	e3530000 	cmp	r3, #0	; 0x0
    fe80:	1a000001 	bne	fe8c <sata_vs_write+0x1cc>
    fe84:	e3520a01 	cmp	r2, #4096	; 0x1000
    fe88:	3a000080 	bcc	10090 <sata_vs_write+0x3d0>
    fe8c:	e59dc058 	ldr	ip, [sp, #88]
    fe90:	e35c0601 	cmp	ip, #1048576	; 0x100000
    fe94:	ba000007 	blt	feb8 <sata_vs_write+0x1f8>
    fe98:	e1a02006 	mov	r2, r6
    fe9c:	e1a03007 	mov	r3, r7
    fea0:	e0522000 	subs	r2, r2, r0
    fea4:	e0c33001 	sbc	r3, r3, r1
    fea8:	e3530000 	cmp	r3, #0	; 0x0
    feac:	1a000001 	bne	feb8 <sata_vs_write+0x1f8>
    feb0:	e3520b02 	cmp	r2, #2048	; 0x800
    feb4:	3a000075 	bcc	10090 <sata_vs_write+0x3d0>
    feb8:	e59d2058 	ldr	r2, [sp, #88]
    febc:	e3520702 	cmp	r2, #524288	; 0x80000
    fec0:	ba000007 	blt	fee4 <sata_vs_write+0x224>
    fec4:	e1a02006 	mov	r2, r6
    fec8:	e1a03007 	mov	r3, r7
    fecc:	e0522000 	subs	r2, r2, r0
    fed0:	e0c33001 	sbc	r3, r3, r1
    fed4:	e3530000 	cmp	r3, #0	; 0x0
    fed8:	1a000001 	bne	fee4 <sata_vs_write+0x224>
    fedc:	e3520b01 	cmp	r2, #1024	; 0x400
    fee0:	3a00006a 	bcc	10090 <sata_vs_write+0x3d0>
    fee4:	e59d3058 	ldr	r3, [sp, #88]
    fee8:	e3530701 	cmp	r3, #262144	; 0x40000
    feec:	ba000007 	blt	ff10 <sata_vs_write+0x250>
    fef0:	e1a02006 	mov	r2, r6
    fef4:	e1a03007 	mov	r3, r7
    fef8:	e0522000 	subs	r2, r2, r0
    fefc:	e0c33001 	sbc	r3, r3, r1
    ff00:	e3530000 	cmp	r3, #0	; 0x0
    ff04:	1a000001 	bne	ff10 <sata_vs_write+0x250>
    ff08:	e3520c02 	cmp	r2, #512	; 0x200
    ff0c:	3a00005f 	bcc	10090 <sata_vs_write+0x3d0>
    ff10:	e59dc058 	ldr	ip, [sp, #88]
    ff14:	e35c0802 	cmp	ip, #131072	; 0x20000
    ff18:	ba000008 	blt	ff40 <sata_vs_write+0x280>
    ff1c:	e3e020ff 	mvn	r2, #255	; 0xff
    ff20:	e0922006 	adds	r2, r2, r6
    ff24:	e3e03000 	mvn	r3, #0	; 0x0
    ff28:	e0a33007 	adc	r3, r3, r7
    ff2c:	e1510003 	cmp	r1, r3
    ff30:	8a000056 	bhi	10090 <sata_vs_write+0x3d0>
    ff34:	1a000001 	bne	ff40 <sata_vs_write+0x280>
    ff38:	e1500002 	cmp	r0, r2
    ff3c:	8a000053 	bhi	10090 <sata_vs_write+0x3d0>
    ff40:	e59d2058 	ldr	r2, [sp, #88]
    ff44:	e3520801 	cmp	r2, #65536	; 0x10000
    ff48:	ba000007 	blt	ff6c <sata_vs_write+0x2ac>
    ff4c:	e1a02006 	mov	r2, r6
    ff50:	e1a03007 	mov	r3, r7
    ff54:	e0522000 	subs	r2, r2, r0
    ff58:	e0c33001 	sbc	r3, r3, r1
    ff5c:	e3530000 	cmp	r3, #0	; 0x0
    ff60:	1a000001 	bne	ff6c <sata_vs_write+0x2ac>
    ff64:	e352007f 	cmp	r2, #127	; 0x7f
    ff68:	9a000048 	bls	10090 <sata_vs_write+0x3d0>
    ff6c:	e59d3058 	ldr	r3, [sp, #88]
    ff70:	e3530902 	cmp	r3, #32768	; 0x8000
    ff74:	ba000007 	blt	ff98 <sata_vs_write+0x2d8>
    ff78:	e1a02006 	mov	r2, r6
    ff7c:	e1a03007 	mov	r3, r7
    ff80:	e0522000 	subs	r2, r2, r0
    ff84:	e0c33001 	sbc	r3, r3, r1
    ff88:	e3530000 	cmp	r3, #0	; 0x0
    ff8c:	1a000001 	bne	ff98 <sata_vs_write+0x2d8>
    ff90:	e352003f 	cmp	r2, #63	; 0x3f
    ff94:	9a00003d 	bls	10090 <sata_vs_write+0x3d0>
    ff98:	e59dc058 	ldr	ip, [sp, #88]
    ff9c:	e35c0901 	cmp	ip, #16384	; 0x4000
    ffa0:	ba000007 	blt	ffc4 <sata_vs_write+0x304>
    ffa4:	e1a02006 	mov	r2, r6
    ffa8:	e1a03007 	mov	r3, r7
    ffac:	e0522000 	subs	r2, r2, r0
    ffb0:	e0c33001 	sbc	r3, r3, r1
    ffb4:	e3530000 	cmp	r3, #0	; 0x0
    ffb8:	1a000001 	bne	ffc4 <sata_vs_write+0x304>
    ffbc:	e352001f 	cmp	r2, #31	; 0x1f
    ffc0:	9a000032 	bls	10090 <sata_vs_write+0x3d0>
    ffc4:	e59d2058 	ldr	r2, [sp, #88]
    ffc8:	e3520a02 	cmp	r2, #8192	; 0x2000
    ffcc:	ba000007 	blt	fff0 <sata_vs_write+0x330>
    ffd0:	e1a02006 	mov	r2, r6
    ffd4:	e1a03007 	mov	r3, r7
    ffd8:	e0522000 	subs	r2, r2, r0
    ffdc:	e0c33001 	sbc	r3, r3, r1
    ffe0:	e3530000 	cmp	r3, #0	; 0x0
    ffe4:	1a000001 	bne	fff0 <sata_vs_write+0x330>
    ffe8:	e352000f 	cmp	r2, #15	; 0xf
    ffec:	9a000027 	bls	10090 <sata_vs_write+0x3d0>
    fff0:	e59d3058 	ldr	r3, [sp, #88]
    fff4:	e3530a01 	cmp	r3, #4096	; 0x1000
    fff8:	ba000007 	blt	1001c <sata_vs_write+0x35c>
    fffc:	e1a02006 	mov	r2, r6
   10000:	e1a03007 	mov	r3, r7
   10004:	e0522000 	subs	r2, r2, r0
   10008:	e0c33001 	sbc	r3, r3, r1
   1000c:	e3530000 	cmp	r3, #0	; 0x0
   10010:	1a000001 	bne	1001c <sata_vs_write+0x35c>
   10014:	e3520007 	cmp	r2, #7	; 0x7
   10018:	9a00001c 	bls	10090 <sata_vs_write+0x3d0>
   1001c:	e59dc058 	ldr	ip, [sp, #88]
   10020:	e35c0b02 	cmp	ip, #2048	; 0x800
   10024:	ba000007 	blt	10048 <sata_vs_write+0x388>
   10028:	e1a02006 	mov	r2, r6
   1002c:	e1a03007 	mov	r3, r7
   10030:	e0522000 	subs	r2, r2, r0
   10034:	e0c33001 	sbc	r3, r3, r1
   10038:	e3530000 	cmp	r3, #0	; 0x0
   1003c:	1a000001 	bne	10048 <sata_vs_write+0x388>
   10040:	e3520003 	cmp	r2, #3	; 0x3
   10044:	9a000011 	bls	10090 <sata_vs_write+0x3d0>
   10048:	e59d2058 	ldr	r2, [sp, #88]
   1004c:	e3520b01 	cmp	r2, #1024	; 0x400
   10050:	ba000007 	blt	10074 <sata_vs_write+0x3b4>
   10054:	e1a02006 	mov	r2, r6
   10058:	e1a03007 	mov	r3, r7
   1005c:	e0522000 	subs	r2, r2, r0
   10060:	e0c33001 	sbc	r3, r3, r1
   10064:	e3530000 	cmp	r3, #0	; 0x0
   10068:	1a000001 	bne	10074 <sata_vs_write+0x3b4>
   1006c:	e3520001 	cmp	r2, #1	; 0x1
   10070:	9a000006 	bls	10090 <sata_vs_write+0x3d0>
   10074:	e59d3058 	ldr	r3, [sp, #88]
   10078:	e3530c02 	cmp	r3, #512	; 0x200
   1007c:	ba00000b 	blt	100b0 <sata_vs_write+0x3f0>
   10080:	e1560000 	cmp	r6, r0
   10084:	1a000009 	bne	100b0 <sata_vs_write+0x3f0>
   10088:	e1570001 	cmp	r7, r1
   1008c:	1a000007 	bne	100b0 <sata_vs_write+0x3f0>
   10090:	e59f02e0 	ldr	r0, [pc, #736]	; 10378 <sata_vs_write+0x6b8>
   10094:	e59f32d8 	ldr	r3, [pc, #728]	; 10374 <sata_vs_write+0x6b4>
   10098:	e1a0e00f 	mov	lr, pc
   1009c:	e593f000 	ldr	pc, [r3]
   100a0:	e59d0014 	ldr	r0, [sp, #20]
   100a4:	ebffc5bb 	bl	1798 <free>
   100a8:	e3e04000 	mvn	r4, #0	; 0x0
   100ac:	ea0000ac 	b	10364 <sata_vs_write+0x6a4>
   100b0:	e59dc018 	ldr	ip, [sp, #24]
   100b4:	e59d0018 	ldr	r0, [sp, #24]
   100b8:	e59d1018 	ldr	r1, [sp, #24]
   100bc:	e28ccb01 	add	ip, ip, #1024	; 0x400
   100c0:	e2800c05 	add	r0, r0, #1280	; 0x500
   100c4:	e2811d16 	add	r1, r1, #1408	; 0x580
   100c8:	e59fb2ac 	ldr	fp, [pc, #684]	; 1037c <sata_vs_write+0x6bc>
   100cc:	e59d5058 	ldr	r5, [sp, #88]
   100d0:	e58d8024 	str	r8, [sp, #36]
   100d4:	e58dc01c 	str	ip, [sp, #28]
   100d8:	e58d0020 	str	r0, [sp, #32]
   100dc:	e58d102c 	str	r1, [sp, #44]
   100e0:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
   100e4:	e1550002 	cmp	r5, r2
   100e8:	c3a09501 	movgt	r9, #4194304	; 0x400000
   100ec:	c3a08008 	movgt	r8, #8	; 0x8
   100f0:	c3a0a801 	movgt	sl, #65536	; 0x10000
   100f4:	ca00001c 	bgt	1016c <sata_vs_write+0x4ac>
   100f8:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
   100fc:	e1550003 	cmp	r5, r3
   10100:	c3a09501 	movgt	r9, #4194304	; 0x400000
   10104:	c3a08004 	movgt	r8, #4	; 0x4
   10108:	c3a0a902 	movgt	sl, #32768	; 0x8000
   1010c:	ca000016 	bgt	1016c <sata_vs_write+0x4ac>
   10110:	e59fc268 	ldr	ip, [pc, #616]	; 10380 <sata_vs_write+0x6c0>
   10114:	e155000c 	cmp	r5, ip
   10118:	c3a09501 	movgt	r9, #4194304	; 0x400000
   1011c:	c3a08002 	movgt	r8, #2	; 0x2
   10120:	c3a0a901 	movgt	sl, #16384	; 0x4000
   10124:	ca000010 	bgt	1016c <sata_vs_write+0x4ac>
   10128:	e59f0254 	ldr	r0, [pc, #596]	; 10384 <sata_vs_write+0x6c4>
   1012c:	e1550000 	cmp	r5, r0
   10130:	c3a09501 	movgt	r9, #4194304	; 0x400000
   10134:	c3a08001 	movgt	r8, #1	; 0x1
   10138:	c3a0aa02 	movgt	sl, #8192	; 0x2000
   1013c:	ca00000a 	bgt	1016c <sata_vs_write+0x4ac>
   10140:	e59f1240 	ldr	r1, [pc, #576]	; 10388 <sata_vs_write+0x6c8>
   10144:	e2453c02 	sub	r3, r5, #512	; 0x200
   10148:	e1530001 	cmp	r3, r1
   1014c:	8a000080 	bhi	10354 <sata_vs_write+0x694>
   10150:	e2853f7f 	add	r3, r5, #508	; 0x1fc
   10154:	e3550000 	cmp	r5, #0	; 0x0
   10158:	e2833003 	add	r3, r3, #3	; 0x3
   1015c:	a1a03005 	movge	r3, r5
   10160:	e1a0a4c3 	asr	sl, r3, #9
   10164:	e1a0948a 	lsl	r9, sl, #9
   10168:	e3a08001 	mov	r8, #1	; 0x1
   1016c:	e3a04000 	mov	r4, #0	; 0x0
   10170:	e1a0200a 	mov	r2, sl
   10174:	e59d3028 	ldr	r3, [sp, #40]
   10178:	e59d0020 	ldr	r0, [sp, #32]
   1017c:	e3a01035 	mov	r1, #53	; 0x35
   10180:	e58d4000 	str	r4, [sp]
   10184:	e58d4004 	str	r4, [sp, #4]
   10188:	ebfffde5 	bl	f924 <CreateAtaCFIS>
   1018c:	e1a03208 	lsl	r3, r8, #4
   10190:	e1a02004 	mov	r2, r4
   10194:	e59dc02c 	ldr	ip, [sp, #44]
   10198:	e7cc2004 	strb	r2, [ip, r4]
   1019c:	e2844001 	add	r4, r4, #1	; 0x1
   101a0:	e1540003 	cmp	r4, r3
   101a4:	3afffffa 	bcc	10194 <sata_vs_write+0x4d4>
   101a8:	e2493001 	sub	r3, r9, #1	; 0x1
   101ac:	e3a01000 	mov	r1, #0	; 0x0
   101b0:	e1a04503 	lsl	r4, r3, #10
   101b4:	e59dc024 	ldr	ip, [sp, #36]
   101b8:	ea00000b 	b	101ec <sata_vs_write+0x52c>
   101bc:	e3a03000 	mov	r3, #0	; 0x0
   101c0:	e58e3004 	str	r3, [lr, #4]
   101c4:	e59d3020 	ldr	r3, [sp, #32]
   101c8:	e783c202 	str	ip, [r3, r2, lsl #4]
   101cc:	e5902004 	ldr	r2, [r0, #4]
   101d0:	e08cc009 	add	ip, ip, r9
   101d4:	e1843b22 	orr	r3, r4, r2, lsr #22
   101d8:	e1a02563 	ror	r2, r3, #10
   101dc:	e1a03c22 	lsr	r3, r2, #24
   101e0:	e3c33080 	bic	r3, r3, #128	; 0x80
   101e4:	e5802004 	str	r2, [r0, #4]
   101e8:	e5c03007 	strb	r3, [r0, #7]
   101ec:	e59d0020 	ldr	r0, [sp, #32]
   101f0:	e2812008 	add	r2, r1, #8	; 0x8
   101f4:	e0803201 	add	r3, r0, r1, lsl #4
   101f8:	e080e202 	add	lr, r0, r2, lsl #4
   101fc:	e1510008 	cmp	r1, r8
   10200:	e2830088 	add	r0, r3, #136	; 0x88
   10204:	e2813001 	add	r3, r1, #1	; 0x1
   10208:	e6ff1073 	uxth	r1, r3
   1020c:	3affffea 	bcc	101bc <sata_vs_write+0x4fc>
   10210:	e3a04000 	mov	r4, #0	; 0x0
   10214:	e1a03008 	mov	r3, r8
   10218:	e3a0c001 	mov	ip, #1	; 0x1
   1021c:	e59d0018 	ldr	r0, [sp, #24]
   10220:	e1a01004 	mov	r1, r4
   10224:	e59d2020 	ldr	r2, [sp, #32]
   10228:	e88d1010 	stm	sp, {r4, ip}
   1022c:	e58d4008 	str	r4, [sp, #8]
   10230:	e58d400c 	str	r4, [sp, #12]
   10234:	ebfffdf8 	bl	fa1c <CreateCommandHeader>
   10238:	e1a03004 	mov	r3, r4
   1023c:	e59d001c 	ldr	r0, [sp, #28]
   10240:	e7c03004 	strb	r3, [r0, r4]
   10244:	e2844001 	add	r4, r4, #1	; 0x1
   10248:	e3540c01 	cmp	r4, #256	; 0x100
   1024c:	1afffffa 	bne	1023c <sata_vs_write+0x57c>
   10250:	e59d1018 	ldr	r1, [sp, #24]
   10254:	e3e03000 	mvn	r3, #0	; 0x0
   10258:	e58b1100 	str	r1, [fp, #256]
   1025c:	e58b0108 	str	r0, [fp, #264]
   10260:	e58b3110 	str	r3, [fp, #272]
   10264:	e58b3130 	str	r3, [fp, #304]
   10268:	e59b3118 	ldr	r3, [fp, #280]
   1026c:	e3a02001 	mov	r2, #1	; 0x1
   10270:	e3833010 	orr	r3, r3, #16	; 0x10
   10274:	e58b3118 	str	r3, [fp, #280]
   10278:	e59b3118 	ldr	r3, [fp, #280]
   1027c:	e3833001 	orr	r3, r3, #1	; 0x1
   10280:	e58b3118 	str	r3, [fp, #280]
   10284:	e58b2138 	str	r2, [fp, #312]
   10288:	e59b3110 	ldr	r3, [fp, #272]
   1028c:	e3130001 	tst	r3, #1	; 0x1
   10290:	0afffffc 	beq	10288 <sata_vs_write+0x5c8>
   10294:	e59fc0e0 	ldr	ip, [pc, #224]	; 1037c <sata_vs_write+0x6bc>
   10298:	e3e03000 	mvn	r3, #0	; 0x0
   1029c:	e58c3110 	str	r3, [ip, #272]
   102a0:	e58c3008 	str	r3, [ip, #8]
   102a4:	e59c2118 	ldr	r2, [ip, #280]
   102a8:	e30c3011 	movw	r3, #49169	; 0xc011
   102ac:	e0023003 	and	r3, r2, r3
   102b0:	e3530000 	cmp	r3, #0	; 0x0
   102b4:	0a00000c 	beq	102ec <sata_vs_write+0x62c>
   102b8:	e59c3118 	ldr	r3, [ip, #280]
   102bc:	e3c33001 	bic	r3, r3, #1	; 0x1
   102c0:	e58c3118 	str	r3, [ip, #280]
   102c4:	e59b3118 	ldr	r3, [fp, #280]
   102c8:	e3130902 	tst	r3, #32768	; 0x8000
   102cc:	1afffffc 	bne	102c4 <sata_vs_write+0x604>
   102d0:	e59f00a4 	ldr	r0, [pc, #164]	; 1037c <sata_vs_write+0x6bc>
   102d4:	e5903118 	ldr	r3, [r0, #280]
   102d8:	e3c33010 	bic	r3, r3, #16	; 0x10
   102dc:	e5803118 	str	r3, [r0, #280]
   102e0:	e59b3118 	ldr	r3, [fp, #280]
   102e4:	e3130901 	tst	r3, #16384	; 0x4000
   102e8:	1afffffc 	bne	102e0 <sata_vs_write+0x620>
   102ec:	e59b3120 	ldr	r3, [fp, #288]
   102f0:	e3130088 	tst	r3, #136	; 0x88
   102f4:	1afffffc 	bne	102ec <sata_vs_write+0x62c>
   102f8:	e0010899 	mul	r1, r9, r8
   102fc:	e0555001 	subs	r5, r5, r1
   10300:	0a000013 	beq	10354 <sata_vs_write+0x694>
   10304:	e59dc028 	ldr	ip, [sp, #40]
   10308:	e1b02b85 	lsls	r2, r5, #23
   1030c:	13a02001 	movne	r2, #1	; 0x1
   10310:	e08cc00a 	add	ip, ip, sl
   10314:	e3550000 	cmp	r5, #0	; 0x0
   10318:	e2853f7f 	add	r3, r5, #508	; 0x1fc
   1031c:	e2833003 	add	r3, r3, #3	; 0x3
   10320:	e082200c 	add	r2, r2, ip
   10324:	a1a03005 	movge	r3, r5
   10328:	e3570000 	cmp	r7, #0	; 0x0
   1032c:	e58dc028 	str	ip, [sp, #40]
   10330:	e08234c3 	add	r3, r2, r3, asr #9
   10334:	8a000002 	bhi	10344 <sata_vs_write+0x684>
   10338:	1a000005 	bne	10354 <sata_vs_write+0x694>
   1033c:	e1560003 	cmp	r6, r3
   10340:	9a000003 	bls	10354 <sata_vs_write+0x694>
   10344:	e59d0024 	ldr	r0, [sp, #36]
   10348:	e0800001 	add	r0, r0, r1
   1034c:	e58d0024 	str	r0, [sp, #36]
   10350:	eaffff62 	b	100e0 <sata_vs_write+0x420>
   10354:	e59d1058 	ldr	r1, [sp, #88]
   10358:	e59d0014 	ldr	r0, [sp, #20]
   1035c:	e0654001 	rsb	r4, r5, r1
   10360:	ebffc50c 	bl	1798 <free>
   10364:	e1a00004 	mov	r0, r4
   10368:	e28dd034 	add	sp, sp, #52	; 0x34
   1036c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   10370:	0001926c 	.word	0x0001926c
   10374:	3c00a5cc 	.word	0x3c00a5cc
   10378:	0001928f 	.word	0x0001928f
   1037c:	28200000 	.word	0x28200000
   10380:	007fffff 	.word	0x007fffff
   10384:	003fffff 	.word	0x003fffff
   10388:	003ffdff 	.word	0x003ffdff

0001038c <sata_vs_read>:
   1038c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   10390:	e1a08000 	mov	r8, r0
   10394:	e24dd034 	sub	sp, sp, #52	; 0x34
   10398:	e3a00c0e 	mov	r0, #3584	; 0xe00
   1039c:	e1a06002 	mov	r6, r2
   103a0:	e1a07003 	mov	r7, r3
   103a4:	ebffc493 	bl	15f8 <malloc>
   103a8:	e3500000 	cmp	r0, #0	; 0x0
   103ac:	e58d0014 	str	r0, [sp, #20]
   103b0:	1a000004 	bne	103c8 <sata_vs_read+0x3c>
   103b4:	e59f0680 	ldr	r0, [pc, #1664]	; 10a3c <sata_vs_read+0x6b0>
   103b8:	e59f3680 	ldr	r3, [pc, #1664]	; 10a40 <sata_vs_read+0x6b4>
   103bc:	e1a0e00f 	mov	lr, pc
   103c0:	e593f000 	ldr	pc, [r3]
   103c4:	ea0000ea 	b	10774 <sata_vs_read+0x3e8>
   103c8:	e59d0014 	ldr	r0, [sp, #20]
   103cc:	e2803fff 	add	r3, r0, #1020	; 0x3fc
   103d0:	e2833003 	add	r3, r3, #3	; 0x3
   103d4:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
   103d8:	e3c33003 	bic	r3, r3, #3	; 0x3
   103dc:	e1530000 	cmp	r3, r0
   103e0:	e58d3018 	str	r3, [sp, #24]
   103e4:	02833a01 	addeq	r3, r3, #4096	; 0x1000
   103e8:	058d3018 	streq	r3, [sp, #24]
   103ec:	e59d0018 	ldr	r0, [sp, #24]
   103f0:	ebfffdcb 	bl	fb24 <get_max_lba>
   103f4:	e59d1058 	ldr	r1, [sp, #88]
   103f8:	e1a0c000 	mov	ip, r0
   103fc:	e1a03b81 	lsl	r3, r1, #23
   10400:	e1a03ba3 	lsr	r3, r3, #23
   10404:	e3530000 	cmp	r3, #0	; 0x0
   10408:	1a0000d3 	bne	1075c <sata_vs_read+0x3d0>
   1040c:	e30041ff 	movw	r4, #511	; 0x1ff
   10410:	e3a05000 	mov	r5, #0	; 0x0
   10414:	e0062004 	and	r2, r6, r4
   10418:	e0073005 	and	r3, r7, r5
   1041c:	e1920003 	orrs	r0, r2, r3
   10420:	1a0000cd 	bne	1075c <sata_vs_read+0x3d0>
   10424:	e3180001 	tst	r8, #1	; 0x1
   10428:	1a0000cb 	bne	1075c <sata_vs_read+0x3d0>
   1042c:	e1a00007 	mov	r0, r7
   10430:	e1a01fc0 	asr	r1, r0, #31
   10434:	e1a02fc1 	asr	r2, r1, #31
   10438:	e1a03002 	mov	r3, r2
   1043c:	e0022004 	and	r2, r2, r4
   10440:	e0922006 	adds	r2, r2, r6
   10444:	e0033005 	and	r3, r3, r5
   10448:	e0a33007 	adc	r3, r3, r7
   1044c:	e1a014a2 	lsr	r1, r2, #9
   10450:	e1a0600c 	mov	r6, ip
   10454:	e1a07fc6 	asr	r7, r6, #31
   10458:	e1811b83 	orr	r1, r1, r3, lsl #23
   1045c:	e3570000 	cmp	r7, #0	; 0x0
   10460:	e58d1028 	str	r1, [sp, #40]
   10464:	e1a00001 	mov	r0, r1
   10468:	e3a01000 	mov	r1, #0	; 0x0
   1046c:	8a000002 	bhi	1047c <sata_vs_read+0xf0>
   10470:	1a0000b9 	bne	1075c <sata_vs_read+0x3d0>
   10474:	e1560000 	cmp	r6, r0
   10478:	9a0000b7 	bls	1075c <sata_vs_read+0x3d0>
   1047c:	e59d2058 	ldr	r2, [sp, #88]
   10480:	e3520402 	cmp	r2, #33554432	; 0x2000000
   10484:	ba000007 	blt	104a8 <sata_vs_read+0x11c>
   10488:	e1a02006 	mov	r2, r6
   1048c:	e1a03007 	mov	r3, r7
   10490:	e0522000 	subs	r2, r2, r0
   10494:	e0c33001 	sbc	r3, r3, r1
   10498:	e3530000 	cmp	r3, #0	; 0x0
   1049c:	1a000001 	bne	104a8 <sata_vs_read+0x11c>
   104a0:	e3520801 	cmp	r2, #65536	; 0x10000
   104a4:	3a0000ac 	bcc	1075c <sata_vs_read+0x3d0>
   104a8:	e59d3058 	ldr	r3, [sp, #88]
   104ac:	e3530401 	cmp	r3, #16777216	; 0x1000000
   104b0:	ba000007 	blt	104d4 <sata_vs_read+0x148>
   104b4:	e1a02006 	mov	r2, r6
   104b8:	e1a03007 	mov	r3, r7
   104bc:	e0522000 	subs	r2, r2, r0
   104c0:	e0c33001 	sbc	r3, r3, r1
   104c4:	e3530000 	cmp	r3, #0	; 0x0
   104c8:	1a000001 	bne	104d4 <sata_vs_read+0x148>
   104cc:	e3520902 	cmp	r2, #32768	; 0x8000
   104d0:	3a0000a1 	bcc	1075c <sata_vs_read+0x3d0>
   104d4:	e59d2058 	ldr	r2, [sp, #88]
   104d8:	e3520502 	cmp	r2, #8388608	; 0x800000
   104dc:	ba000007 	blt	10500 <sata_vs_read+0x174>
   104e0:	e1a02006 	mov	r2, r6
   104e4:	e1a03007 	mov	r3, r7
   104e8:	e0522000 	subs	r2, r2, r0
   104ec:	e0c33001 	sbc	r3, r3, r1
   104f0:	e3530000 	cmp	r3, #0	; 0x0
   104f4:	1a000001 	bne	10500 <sata_vs_read+0x174>
   104f8:	e3520901 	cmp	r2, #16384	; 0x4000
   104fc:	3a000096 	bcc	1075c <sata_vs_read+0x3d0>
   10500:	e59d3058 	ldr	r3, [sp, #88]
   10504:	e3530501 	cmp	r3, #4194304	; 0x400000
   10508:	ba000007 	blt	1052c <sata_vs_read+0x1a0>
   1050c:	e1a02006 	mov	r2, r6
   10510:	e1a03007 	mov	r3, r7
   10514:	e0522000 	subs	r2, r2, r0
   10518:	e0c33001 	sbc	r3, r3, r1
   1051c:	e3530000 	cmp	r3, #0	; 0x0
   10520:	1a000001 	bne	1052c <sata_vs_read+0x1a0>
   10524:	e3520a02 	cmp	r2, #8192	; 0x2000
   10528:	3a00008b 	bcc	1075c <sata_vs_read+0x3d0>
   1052c:	e59d2058 	ldr	r2, [sp, #88]
   10530:	e3520602 	cmp	r2, #2097152	; 0x200000
   10534:	ba000007 	blt	10558 <sata_vs_read+0x1cc>
   10538:	e1a02006 	mov	r2, r6
   1053c:	e1a03007 	mov	r3, r7
   10540:	e0522000 	subs	r2, r2, r0
   10544:	e0c33001 	sbc	r3, r3, r1
   10548:	e3530000 	cmp	r3, #0	; 0x0
   1054c:	1a000001 	bne	10558 <sata_vs_read+0x1cc>
   10550:	e3520a01 	cmp	r2, #4096	; 0x1000
   10554:	3a000080 	bcc	1075c <sata_vs_read+0x3d0>
   10558:	e59d3058 	ldr	r3, [sp, #88]
   1055c:	e3530601 	cmp	r3, #1048576	; 0x100000
   10560:	ba000007 	blt	10584 <sata_vs_read+0x1f8>
   10564:	e1a02006 	mov	r2, r6
   10568:	e1a03007 	mov	r3, r7
   1056c:	e0522000 	subs	r2, r2, r0
   10570:	e0c33001 	sbc	r3, r3, r1
   10574:	e3530000 	cmp	r3, #0	; 0x0
   10578:	1a000001 	bne	10584 <sata_vs_read+0x1f8>
   1057c:	e3520b02 	cmp	r2, #2048	; 0x800
   10580:	3a000075 	bcc	1075c <sata_vs_read+0x3d0>
   10584:	e59d2058 	ldr	r2, [sp, #88]
   10588:	e3520702 	cmp	r2, #524288	; 0x80000
   1058c:	ba000007 	blt	105b0 <sata_vs_read+0x224>
   10590:	e1a02006 	mov	r2, r6
   10594:	e1a03007 	mov	r3, r7
   10598:	e0522000 	subs	r2, r2, r0
   1059c:	e0c33001 	sbc	r3, r3, r1
   105a0:	e3530000 	cmp	r3, #0	; 0x0
   105a4:	1a000001 	bne	105b0 <sata_vs_read+0x224>
   105a8:	e3520b01 	cmp	r2, #1024	; 0x400
   105ac:	3a00006a 	bcc	1075c <sata_vs_read+0x3d0>
   105b0:	e59d3058 	ldr	r3, [sp, #88]
   105b4:	e3530701 	cmp	r3, #262144	; 0x40000
   105b8:	ba000007 	blt	105dc <sata_vs_read+0x250>
   105bc:	e1a02006 	mov	r2, r6
   105c0:	e1a03007 	mov	r3, r7
   105c4:	e0522000 	subs	r2, r2, r0
   105c8:	e0c33001 	sbc	r3, r3, r1
   105cc:	e3530000 	cmp	r3, #0	; 0x0
   105d0:	1a000001 	bne	105dc <sata_vs_read+0x250>
   105d4:	e3520c02 	cmp	r2, #512	; 0x200
   105d8:	3a00005f 	bcc	1075c <sata_vs_read+0x3d0>
   105dc:	e59d2058 	ldr	r2, [sp, #88]
   105e0:	e3520802 	cmp	r2, #131072	; 0x20000
   105e4:	ba000008 	blt	1060c <sata_vs_read+0x280>
   105e8:	e3e020ff 	mvn	r2, #255	; 0xff
   105ec:	e0922006 	adds	r2, r2, r6
   105f0:	e3e03000 	mvn	r3, #0	; 0x0
   105f4:	e0a33007 	adc	r3, r3, r7
   105f8:	e1510003 	cmp	r1, r3
   105fc:	8a000056 	bhi	1075c <sata_vs_read+0x3d0>
   10600:	1a000001 	bne	1060c <sata_vs_read+0x280>
   10604:	e1500002 	cmp	r0, r2
   10608:	8a000053 	bhi	1075c <sata_vs_read+0x3d0>
   1060c:	e59d3058 	ldr	r3, [sp, #88]
   10610:	e3530801 	cmp	r3, #65536	; 0x10000
   10614:	ba000007 	blt	10638 <sata_vs_read+0x2ac>
   10618:	e1a02006 	mov	r2, r6
   1061c:	e1a03007 	mov	r3, r7
   10620:	e0522000 	subs	r2, r2, r0
   10624:	e0c33001 	sbc	r3, r3, r1
   10628:	e3530000 	cmp	r3, #0	; 0x0
   1062c:	1a000001 	bne	10638 <sata_vs_read+0x2ac>
   10630:	e352007f 	cmp	r2, #127	; 0x7f
   10634:	9a000048 	bls	1075c <sata_vs_read+0x3d0>
   10638:	e59d2058 	ldr	r2, [sp, #88]
   1063c:	e3520902 	cmp	r2, #32768	; 0x8000
   10640:	ba000007 	blt	10664 <sata_vs_read+0x2d8>
   10644:	e1a02006 	mov	r2, r6
   10648:	e1a03007 	mov	r3, r7
   1064c:	e0522000 	subs	r2, r2, r0
   10650:	e0c33001 	sbc	r3, r3, r1
   10654:	e3530000 	cmp	r3, #0	; 0x0
   10658:	1a000001 	bne	10664 <sata_vs_read+0x2d8>
   1065c:	e352003f 	cmp	r2, #63	; 0x3f
   10660:	9a00003d 	bls	1075c <sata_vs_read+0x3d0>
   10664:	e59d3058 	ldr	r3, [sp, #88]
   10668:	e3530901 	cmp	r3, #16384	; 0x4000
   1066c:	ba000007 	blt	10690 <sata_vs_read+0x304>
   10670:	e1a02006 	mov	r2, r6
   10674:	e1a03007 	mov	r3, r7
   10678:	e0522000 	subs	r2, r2, r0
   1067c:	e0c33001 	sbc	r3, r3, r1
   10680:	e3530000 	cmp	r3, #0	; 0x0
   10684:	1a000001 	bne	10690 <sata_vs_read+0x304>
   10688:	e352001f 	cmp	r2, #31	; 0x1f
   1068c:	9a000032 	bls	1075c <sata_vs_read+0x3d0>
   10690:	e59d2058 	ldr	r2, [sp, #88]
   10694:	e3520a02 	cmp	r2, #8192	; 0x2000
   10698:	ba000007 	blt	106bc <sata_vs_read+0x330>
   1069c:	e1a02006 	mov	r2, r6
   106a0:	e1a03007 	mov	r3, r7
   106a4:	e0522000 	subs	r2, r2, r0
   106a8:	e0c33001 	sbc	r3, r3, r1
   106ac:	e3530000 	cmp	r3, #0	; 0x0
   106b0:	1a000001 	bne	106bc <sata_vs_read+0x330>
   106b4:	e352000f 	cmp	r2, #15	; 0xf
   106b8:	9a000027 	bls	1075c <sata_vs_read+0x3d0>
   106bc:	e59d3058 	ldr	r3, [sp, #88]
   106c0:	e3530a01 	cmp	r3, #4096	; 0x1000
   106c4:	ba000007 	blt	106e8 <sata_vs_read+0x35c>
   106c8:	e1a02006 	mov	r2, r6
   106cc:	e1a03007 	mov	r3, r7
   106d0:	e0522000 	subs	r2, r2, r0
   106d4:	e0c33001 	sbc	r3, r3, r1
   106d8:	e3530000 	cmp	r3, #0	; 0x0
   106dc:	1a000001 	bne	106e8 <sata_vs_read+0x35c>
   106e0:	e3520007 	cmp	r2, #7	; 0x7
   106e4:	9a00001c 	bls	1075c <sata_vs_read+0x3d0>
   106e8:	e59d2058 	ldr	r2, [sp, #88]
   106ec:	e3520b02 	cmp	r2, #2048	; 0x800
   106f0:	ba000007 	blt	10714 <sata_vs_read+0x388>
   106f4:	e1a02006 	mov	r2, r6
   106f8:	e1a03007 	mov	r3, r7
   106fc:	e0522000 	subs	r2, r2, r0
   10700:	e0c33001 	sbc	r3, r3, r1
   10704:	e3530000 	cmp	r3, #0	; 0x0
   10708:	1a000001 	bne	10714 <sata_vs_read+0x388>
   1070c:	e3520003 	cmp	r2, #3	; 0x3
   10710:	9a000011 	bls	1075c <sata_vs_read+0x3d0>
   10714:	e59d3058 	ldr	r3, [sp, #88]
   10718:	e3530b01 	cmp	r3, #1024	; 0x400
   1071c:	ba000007 	blt	10740 <sata_vs_read+0x3b4>
   10720:	e1a02006 	mov	r2, r6
   10724:	e1a03007 	mov	r3, r7
   10728:	e0522000 	subs	r2, r2, r0
   1072c:	e0c33001 	sbc	r3, r3, r1
   10730:	e3530000 	cmp	r3, #0	; 0x0
   10734:	1a000001 	bne	10740 <sata_vs_read+0x3b4>
   10738:	e3520001 	cmp	r2, #1	; 0x1
   1073c:	9a000006 	bls	1075c <sata_vs_read+0x3d0>
   10740:	e59d2058 	ldr	r2, [sp, #88]
   10744:	e3520c02 	cmp	r2, #512	; 0x200
   10748:	ba00000b 	blt	1077c <sata_vs_read+0x3f0>
   1074c:	e1560000 	cmp	r6, r0
   10750:	1a000009 	bne	1077c <sata_vs_read+0x3f0>
   10754:	e1570001 	cmp	r7, r1
   10758:	1a000007 	bne	1077c <sata_vs_read+0x3f0>
   1075c:	e59f02e0 	ldr	r0, [pc, #736]	; 10a44 <sata_vs_read+0x6b8>
   10760:	e59f32d8 	ldr	r3, [pc, #728]	; 10a40 <sata_vs_read+0x6b4>
   10764:	e1a0e00f 	mov	lr, pc
   10768:	e593f000 	ldr	pc, [r3]
   1076c:	e59d0014 	ldr	r0, [sp, #20]
   10770:	ebffc408 	bl	1798 <free>
   10774:	e3e04000 	mvn	r4, #0	; 0x0
   10778:	ea0000ac 	b	10a30 <sata_vs_read+0x6a4>
   1077c:	e59d3018 	ldr	r3, [sp, #24]
   10780:	e59d0018 	ldr	r0, [sp, #24]
   10784:	e59d1018 	ldr	r1, [sp, #24]
   10788:	e2833b01 	add	r3, r3, #1024	; 0x400
   1078c:	e2800c05 	add	r0, r0, #1280	; 0x500
   10790:	e2811d16 	add	r1, r1, #1408	; 0x580
   10794:	e59fb2ac 	ldr	fp, [pc, #684]	; 10a48 <sata_vs_read+0x6bc>
   10798:	e59d5058 	ldr	r5, [sp, #88]
   1079c:	e58d8024 	str	r8, [sp, #36]
   107a0:	e58d301c 	str	r3, [sp, #28]
   107a4:	e58d0020 	str	r0, [sp, #32]
   107a8:	e58d102c 	str	r1, [sp, #44]
   107ac:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
   107b0:	e1550002 	cmp	r5, r2
   107b4:	c3a09501 	movgt	r9, #4194304	; 0x400000
   107b8:	c3a08008 	movgt	r8, #8	; 0x8
   107bc:	c3a0a801 	movgt	sl, #65536	; 0x10000
   107c0:	ca00001c 	bgt	10838 <sata_vs_read+0x4ac>
   107c4:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
   107c8:	e1550003 	cmp	r5, r3
   107cc:	c3a09501 	movgt	r9, #4194304	; 0x400000
   107d0:	c3a08004 	movgt	r8, #4	; 0x4
   107d4:	c3a0a902 	movgt	sl, #32768	; 0x8000
   107d8:	ca000016 	bgt	10838 <sata_vs_read+0x4ac>
   107dc:	e59f0268 	ldr	r0, [pc, #616]	; 10a4c <sata_vs_read+0x6c0>
   107e0:	e1550000 	cmp	r5, r0
   107e4:	c3a09501 	movgt	r9, #4194304	; 0x400000
   107e8:	c3a08002 	movgt	r8, #2	; 0x2
   107ec:	c3a0a901 	movgt	sl, #16384	; 0x4000
   107f0:	ca000010 	bgt	10838 <sata_vs_read+0x4ac>
   107f4:	e59f1254 	ldr	r1, [pc, #596]	; 10a50 <sata_vs_read+0x6c4>
   107f8:	e1550001 	cmp	r5, r1
   107fc:	c3a09501 	movgt	r9, #4194304	; 0x400000
   10800:	c3a08001 	movgt	r8, #1	; 0x1
   10804:	c3a0aa02 	movgt	sl, #8192	; 0x2000
   10808:	ca00000a 	bgt	10838 <sata_vs_read+0x4ac>
   1080c:	e59f2240 	ldr	r2, [pc, #576]	; 10a54 <sata_vs_read+0x6c8>
   10810:	e2453c02 	sub	r3, r5, #512	; 0x200
   10814:	e1530002 	cmp	r3, r2
   10818:	8a000080 	bhi	10a20 <sata_vs_read+0x694>
   1081c:	e2853f7f 	add	r3, r5, #508	; 0x1fc
   10820:	e3550000 	cmp	r5, #0	; 0x0
   10824:	e2833003 	add	r3, r3, #3	; 0x3
   10828:	a1a03005 	movge	r3, r5
   1082c:	e1a0a4c3 	asr	sl, r3, #9
   10830:	e1a0948a 	lsl	r9, sl, #9
   10834:	e3a08001 	mov	r8, #1	; 0x1
   10838:	e3a04000 	mov	r4, #0	; 0x0
   1083c:	e1a0200a 	mov	r2, sl
   10840:	e59d3028 	ldr	r3, [sp, #40]
   10844:	e59d0020 	ldr	r0, [sp, #32]
   10848:	e3a01025 	mov	r1, #37	; 0x25
   1084c:	e58d4000 	str	r4, [sp]
   10850:	e58d4004 	str	r4, [sp, #4]
   10854:	ebfffc32 	bl	f924 <CreateAtaCFIS>
   10858:	e1a03208 	lsl	r3, r8, #4
   1085c:	e1a02004 	mov	r2, r4
   10860:	e59d002c 	ldr	r0, [sp, #44]
   10864:	e7c02004 	strb	r2, [r0, r4]
   10868:	e2844001 	add	r4, r4, #1	; 0x1
   1086c:	e1540003 	cmp	r4, r3
   10870:	3afffffa 	bcc	10860 <sata_vs_read+0x4d4>
   10874:	e2493001 	sub	r3, r9, #1	; 0x1
   10878:	e3a01000 	mov	r1, #0	; 0x0
   1087c:	e1a04503 	lsl	r4, r3, #10
   10880:	e59dc024 	ldr	ip, [sp, #36]
   10884:	ea00000b 	b	108b8 <sata_vs_read+0x52c>
   10888:	e3a03000 	mov	r3, #0	; 0x0
   1088c:	e58e3004 	str	r3, [lr, #4]
   10890:	e59d3020 	ldr	r3, [sp, #32]
   10894:	e783c202 	str	ip, [r3, r2, lsl #4]
   10898:	e5902004 	ldr	r2, [r0, #4]
   1089c:	e08cc009 	add	ip, ip, r9
   108a0:	e1843b22 	orr	r3, r4, r2, lsr #22
   108a4:	e1a02563 	ror	r2, r3, #10
   108a8:	e1a03c22 	lsr	r3, r2, #24
   108ac:	e3c33080 	bic	r3, r3, #128	; 0x80
   108b0:	e5802004 	str	r2, [r0, #4]
   108b4:	e5c03007 	strb	r3, [r0, #7]
   108b8:	e59d0020 	ldr	r0, [sp, #32]
   108bc:	e2812008 	add	r2, r1, #8	; 0x8
   108c0:	e0803201 	add	r3, r0, r1, lsl #4
   108c4:	e080e202 	add	lr, r0, r2, lsl #4
   108c8:	e1510008 	cmp	r1, r8
   108cc:	e2830088 	add	r0, r3, #136	; 0x88
   108d0:	e2813001 	add	r3, r1, #1	; 0x1
   108d4:	e6ff1073 	uxth	r1, r3
   108d8:	3affffea 	bcc	10888 <sata_vs_read+0x4fc>
   108dc:	e3a04000 	mov	r4, #0	; 0x0
   108e0:	e1a03008 	mov	r3, r8
   108e4:	e59d0018 	ldr	r0, [sp, #24]
   108e8:	e1a01004 	mov	r1, r4
   108ec:	e59d2020 	ldr	r2, [sp, #32]
   108f0:	e58d4000 	str	r4, [sp]
   108f4:	e58d4004 	str	r4, [sp, #4]
   108f8:	e58d4008 	str	r4, [sp, #8]
   108fc:	e58d400c 	str	r4, [sp, #12]
   10900:	ebfffc45 	bl	fa1c <CreateCommandHeader>
   10904:	e1a03004 	mov	r3, r4
   10908:	e59d101c 	ldr	r1, [sp, #28]
   1090c:	e7c13004 	strb	r3, [r1, r4]
   10910:	e2844001 	add	r4, r4, #1	; 0x1
   10914:	e3540c01 	cmp	r4, #256	; 0x100
   10918:	1afffffa 	bne	10908 <sata_vs_read+0x57c>
   1091c:	e59d2018 	ldr	r2, [sp, #24]
   10920:	e3e03000 	mvn	r3, #0	; 0x0
   10924:	e58b2100 	str	r2, [fp, #256]
   10928:	e58b1108 	str	r1, [fp, #264]
   1092c:	e58b3110 	str	r3, [fp, #272]
   10930:	e58b3130 	str	r3, [fp, #304]
   10934:	e59b3118 	ldr	r3, [fp, #280]
   10938:	e3833010 	orr	r3, r3, #16	; 0x10
   1093c:	e58b3118 	str	r3, [fp, #280]
   10940:	e59b3118 	ldr	r3, [fp, #280]
   10944:	e3833001 	orr	r3, r3, #1	; 0x1
   10948:	e58b3118 	str	r3, [fp, #280]
   1094c:	e3a03001 	mov	r3, #1	; 0x1
   10950:	e58b3138 	str	r3, [fp, #312]
   10954:	e59b3110 	ldr	r3, [fp, #272]
   10958:	e3130001 	tst	r3, #1	; 0x1
   1095c:	0afffffc 	beq	10954 <sata_vs_read+0x5c8>
   10960:	e59f00e0 	ldr	r0, [pc, #224]	; 10a48 <sata_vs_read+0x6bc>
   10964:	e3e03000 	mvn	r3, #0	; 0x0
   10968:	e5803110 	str	r3, [r0, #272]
   1096c:	e5803008 	str	r3, [r0, #8]
   10970:	e5902118 	ldr	r2, [r0, #280]
   10974:	e30c3011 	movw	r3, #49169	; 0xc011
   10978:	e0023003 	and	r3, r2, r3
   1097c:	e3530000 	cmp	r3, #0	; 0x0
   10980:	0a00000c 	beq	109b8 <sata_vs_read+0x62c>
   10984:	e5903118 	ldr	r3, [r0, #280]
   10988:	e3c33001 	bic	r3, r3, #1	; 0x1
   1098c:	e5803118 	str	r3, [r0, #280]
   10990:	e59b3118 	ldr	r3, [fp, #280]
   10994:	e3130902 	tst	r3, #32768	; 0x8000
   10998:	1afffffc 	bne	10990 <sata_vs_read+0x604>
   1099c:	e59f10a4 	ldr	r1, [pc, #164]	; 10a48 <sata_vs_read+0x6bc>
   109a0:	e5913118 	ldr	r3, [r1, #280]
   109a4:	e3c33010 	bic	r3, r3, #16	; 0x10
   109a8:	e5813118 	str	r3, [r1, #280]
   109ac:	e59b3118 	ldr	r3, [fp, #280]
   109b0:	e3130901 	tst	r3, #16384	; 0x4000
   109b4:	1afffffc 	bne	109ac <sata_vs_read+0x620>
   109b8:	e59b3120 	ldr	r3, [fp, #288]
   109bc:	e3130088 	tst	r3, #136	; 0x88
   109c0:	1afffffc 	bne	109b8 <sata_vs_read+0x62c>
   109c4:	e0010899 	mul	r1, r9, r8
   109c8:	e0555001 	subs	r5, r5, r1
   109cc:	0a000013 	beq	10a20 <sata_vs_read+0x694>
   109d0:	e59d0028 	ldr	r0, [sp, #40]
   109d4:	e1b02b85 	lsls	r2, r5, #23
   109d8:	13a02001 	movne	r2, #1	; 0x1
   109dc:	e080000a 	add	r0, r0, sl
   109e0:	e3550000 	cmp	r5, #0	; 0x0
   109e4:	e2853f7f 	add	r3, r5, #508	; 0x1fc
   109e8:	e2833003 	add	r3, r3, #3	; 0x3
   109ec:	e0822000 	add	r2, r2, r0
   109f0:	a1a03005 	movge	r3, r5
   109f4:	e3570000 	cmp	r7, #0	; 0x0
   109f8:	e58d0028 	str	r0, [sp, #40]
   109fc:	e08234c3 	add	r3, r2, r3, asr #9
   10a00:	8a000002 	bhi	10a10 <sata_vs_read+0x684>
   10a04:	1a000005 	bne	10a20 <sata_vs_read+0x694>
   10a08:	e1560003 	cmp	r6, r3
   10a0c:	9a000003 	bls	10a20 <sata_vs_read+0x694>
   10a10:	e59d2024 	ldr	r2, [sp, #36]
   10a14:	e0822001 	add	r2, r2, r1
   10a18:	e58d2024 	str	r2, [sp, #36]
   10a1c:	eaffff62 	b	107ac <sata_vs_read+0x420>
   10a20:	e59d3058 	ldr	r3, [sp, #88]
   10a24:	e59d0014 	ldr	r0, [sp, #20]
   10a28:	e0654003 	rsb	r4, r5, r3
   10a2c:	ebffc359 	bl	1798 <free>
   10a30:	e1a00004 	mov	r0, r4
   10a34:	e28dd034 	add	sp, sp, #52	; 0x34
   10a38:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   10a3c:	000192b2 	.word	0x000192b2
   10a40:	3c00a5cc 	.word	0x3c00a5cc
   10a44:	0001928f 	.word	0x0001928f
   10a48:	28200000 	.word	0x28200000
   10a4c:	007fffff 	.word	0x007fffff
   10a50:	003fffff 	.word	0x003fffff
   10a54:	003ffdff 	.word	0x003ffdff

00010a58 <sata_vs_reset>:
   10a58:	e92d4070 	push	{r4, r5, r6, lr}
   10a5c:	e59f02d4 	ldr	r0, [pc, #724]	; 10d38 <sata_vs_reset+0x2e0>
   10a60:	ebffc00c 	bl	a98 <module_enable>
   10a64:	e59f32d0 	ldr	r3, [pc, #720]	; 10d3c <sata_vs_reset+0x2e4>
   10a68:	e3a01000 	mov	r1, #0	; 0x0
   10a6c:	e3a0c001 	mov	ip, #1	; 0x1
   10a70:	e3a06010 	mov	r6, #16	; 0x10
   10a74:	e3a02006 	mov	r2, #6	; 0x6
   10a78:	e583c830 	str	ip, [r3, #2096]
   10a7c:	e3a04002 	mov	r4, #2	; 0x2
   10a80:	e5836820 	str	r6, [r3, #2080]
   10a84:	e5831824 	str	r1, [r3, #2084]
   10a88:	e5832828 	str	r2, [r3, #2088]
   10a8c:	e282200c 	add	r2, r2, #12	; 0xc
   10a90:	e583482c 	str	r4, [r3, #2092]
   10a94:	e5834834 	str	r4, [r3, #2100]
   10a98:	e5831838 	str	r1, [r3, #2104]
   10a9c:	e583183c 	str	r1, [r3, #2108]
   10aa0:	e5831840 	str	r1, [r3, #2112]
   10aa4:	e5832844 	str	r2, [r3, #2116]
   10aa8:	e3a02009 	mov	r2, #9	; 0x9
   10aac:	e583c848 	str	ip, [r3, #2120]
   10ab0:	e3a05003 	mov	r5, #3	; 0x3
   10ab4:	e583184c 	str	r1, [r3, #2124]
   10ab8:	e5832850 	str	r2, [r3, #2128]
   10abc:	e082200c 	add	r2, r2, ip
   10ac0:	e5831854 	str	r1, [r3, #2132]
   10ac4:	e3040e20 	movw	r0, #20000	; 0x4e20
   10ac8:	e5832858 	str	r2, [r3, #2136]
   10acc:	e583585c 	str	r5, [r3, #2140]
   10ad0:	e583c860 	str	ip, [r3, #2144]
   10ad4:	e5831864 	str	r1, [r3, #2148]
   10ad8:	e5831868 	str	r1, [r3, #2152]
   10adc:	e583486c 	str	r4, [r3, #2156]
   10ae0:	e583c870 	str	ip, [r3, #2160]
   10ae4:	e5835874 	str	r5, [r3, #2164]
   10ae8:	e5836878 	str	r6, [r3, #2168]
   10aec:	e5831830 	str	r1, [r3, #2096]
   10af0:	ebfff6e6 	bl	e690 <udelay>
   10af4:	e59f2244 	ldr	r2, [pc, #580]	; 10d40 <sata_vs_reset+0x2e8>
   10af8:	e5923004 	ldr	r3, [r2, #4]
   10afc:	e1a01002 	mov	r1, r2
   10b00:	e3833001 	orr	r3, r3, #1	; 0x1
   10b04:	e5823004 	str	r3, [r2, #4]
   10b08:	e5913004 	ldr	r3, [r1, #4]
   10b0c:	e59f222c 	ldr	r2, [pc, #556]	; 10d40 <sata_vs_reset+0x2e8>
   10b10:	e3130001 	tst	r3, #1	; 0x1
   10b14:	1afffffb 	bne	10b08 <sata_vs_reset+0xb0>
   10b18:	e5923000 	ldr	r3, [r2]
   10b1c:	e3a00b03 	mov	r0, #3072	; 0xc00
   10b20:	e3833306 	orr	r3, r3, #402653184	; 0x18000000
   10b24:	e5823000 	str	r3, [r2]
   10b28:	e3a03001 	mov	r3, #1	; 0x1
   10b2c:	e582300c 	str	r3, [r2, #12]
   10b30:	e5923118 	ldr	r3, [r2, #280]
   10b34:	e3833707 	orr	r3, r3, #1835008	; 0x1c0000
   10b38:	e5823118 	str	r3, [r2, #280]
   10b3c:	ebffc2ad 	bl	15f8 <malloc>
   10b40:	e2505000 	subs	r5, r0, #0	; 0x0
   10b44:	1a000004 	bne	10b5c <sata_vs_reset+0x104>
   10b48:	e59f01f4 	ldr	r0, [pc, #500]	; 10d44 <sata_vs_reset+0x2ec>
   10b4c:	e59f31f4 	ldr	r3, [pc, #500]	; 10d48 <sata_vs_reset+0x2f0>
   10b50:	e1a0e00f 	mov	lr, pc
   10b54:	e593f000 	ldr	pc, [r3]
   10b58:	ea000041 	b	10c64 <sata_vs_reset+0x20c>
   10b5c:	e2853fff 	add	r3, r5, #1020	; 0x3fc
   10b60:	e2833003 	add	r3, r3, #3	; 0x3
   10b64:	e59f11d4 	ldr	r1, [pc, #468]	; 10d40 <sata_vs_reset+0x2e8>
   10b68:	e3c30fff 	bic	r0, r3, #1020	; 0x3fc
   10b6c:	e3c00003 	bic	r0, r0, #3	; 0x3
   10b70:	e5913000 	ldr	r3, [r1]
   10b74:	e1500005 	cmp	r0, r5
   10b78:	02800a01 	addeq	r0, r0, #4096	; 0x1000
   10b7c:	e3130302 	tst	r3, #134217728	; 0x8000000
   10b80:	0a00002e 	beq	10c40 <sata_vs_reset+0x1e8>
   10b84:	e5912118 	ldr	r2, [r1, #280]
   10b88:	e30c3011 	movw	r3, #49169	; 0xc011
   10b8c:	e0023003 	and	r3, r2, r3
   10b90:	e3530000 	cmp	r3, #0	; 0x0
   10b94:	0a00000e 	beq	10bd4 <sata_vs_reset+0x17c>
   10b98:	e5913118 	ldr	r3, [r1, #280]
   10b9c:	e1a0c001 	mov	ip, r1
   10ba0:	e3c33001 	bic	r3, r3, #1	; 0x1
   10ba4:	e5813118 	str	r3, [r1, #280]
   10ba8:	e59c3118 	ldr	r3, [ip, #280]
   10bac:	e59f218c 	ldr	r2, [pc, #396]	; 10d40 <sata_vs_reset+0x2e8>
   10bb0:	e3130902 	tst	r3, #32768	; 0x8000
   10bb4:	1afffffb 	bne	10ba8 <sata_vs_reset+0x150>
   10bb8:	e5923118 	ldr	r3, [r2, #280]
   10bbc:	e1a01002 	mov	r1, r2
   10bc0:	e3c33010 	bic	r3, r3, #16	; 0x10
   10bc4:	e5823118 	str	r3, [r2, #280]
   10bc8:	e5913118 	ldr	r3, [r1, #280]
   10bcc:	e3130901 	tst	r3, #16384	; 0x4000
   10bd0:	1afffffc 	bne	10bc8 <sata_vs_reset+0x170>
   10bd4:	e59f2164 	ldr	r2, [pc, #356]	; 10d40 <sata_vs_reset+0x2e8>
   10bd8:	e280eb01 	add	lr, r0, #1024	; 0x400
   10bdc:	e592312c 	ldr	r3, [r2, #300]
   10be0:	e1a01002 	mov	r1, r2
   10be4:	e3c3300f 	bic	r3, r3, #15	; 0xf
   10be8:	e582312c 	str	r3, [r2, #300]
   10bec:	e5820100 	str	r0, [r2, #256]
   10bf0:	e582e108 	str	lr, [r2, #264]
   10bf4:	e5923118 	ldr	r3, [r2, #280]
   10bf8:	e3833010 	orr	r3, r3, #16	; 0x10
   10bfc:	e5823118 	str	r3, [r2, #280]
   10c00:	e5923118 	ldr	r3, [r2, #280]
   10c04:	e3833002 	orr	r3, r3, #2	; 0x2
   10c08:	e5823118 	str	r3, [r2, #280]
   10c0c:	e5913128 	ldr	r3, [r1, #296]
   10c10:	e59f2128 	ldr	r2, [pc, #296]	; 10d40 <sata_vs_reset+0x2e8>
   10c14:	e203300f 	and	r3, r3, #15	; 0xf
   10c18:	e3530003 	cmp	r3, #3	; 0x3
   10c1c:	13530001 	cmpne	r3, #1	; 0x1
   10c20:	1afffff9 	bne	10c0c <sata_vs_reset+0x1b4>
   10c24:	e3e03000 	mvn	r3, #0	; 0x0
   10c28:	e5823130 	str	r3, [r2, #304]
   10c2c:	e5923120 	ldr	r3, [r2, #288]
   10c30:	e59f1108 	ldr	r1, [pc, #264]	; 10d40 <sata_vs_reset+0x2e8>
   10c34:	e3130089 	tst	r3, #137	; 0x89
   10c38:	1afffffb 	bne	10c2c <sata_vs_reset+0x1d4>
   10c3c:	ea00000a 	b	10c6c <sata_vs_reset+0x214>
   10c40:	e59f4100 	ldr	r4, [pc, #256]	; 10d48 <sata_vs_reset+0x2f0>
   10c44:	e59f0100 	ldr	r0, [pc, #256]	; 10d4c <sata_vs_reset+0x2f4>
   10c48:	e1a0e00f 	mov	lr, pc
   10c4c:	e594f000 	ldr	pc, [r4]
   10c50:	e59f00f8 	ldr	r0, [pc, #248]	; 10d50 <sata_vs_reset+0x2f8>
   10c54:	e1a0e00f 	mov	lr, pc
   10c58:	e594f000 	ldr	pc, [r4]
   10c5c:	e1a00005 	mov	r0, r5
   10c60:	ebffc2cc 	bl	1798 <free>
   10c64:	e3e00000 	mvn	r0, #0	; 0x0
   10c68:	e8bd8070 	pop	{r4, r5, r6, pc}
   10c6c:	e5912118 	ldr	r2, [r1, #280]
   10c70:	e30c3011 	movw	r3, #49169	; 0xc011
   10c74:	e0023003 	and	r3, r2, r3
   10c78:	e3530000 	cmp	r3, #0	; 0x0
   10c7c:	0a00000e 	beq	10cbc <sata_vs_reset+0x264>
   10c80:	e5913118 	ldr	r3, [r1, #280]
   10c84:	e1a0c001 	mov	ip, r1
   10c88:	e3c33001 	bic	r3, r3, #1	; 0x1
   10c8c:	e5813118 	str	r3, [r1, #280]
   10c90:	e59c3118 	ldr	r3, [ip, #280]
   10c94:	e59f20a4 	ldr	r2, [pc, #164]	; 10d40 <sata_vs_reset+0x2e8>
   10c98:	e3130902 	tst	r3, #32768	; 0x8000
   10c9c:	1afffffb 	bne	10c90 <sata_vs_reset+0x238>
   10ca0:	e5923118 	ldr	r3, [r2, #280]
   10ca4:	e1a01002 	mov	r1, r2
   10ca8:	e3c33010 	bic	r3, r3, #16	; 0x10
   10cac:	e5823118 	str	r3, [r2, #280]
   10cb0:	e5913118 	ldr	r3, [r1, #280]
   10cb4:	e3130901 	tst	r3, #16384	; 0x4000
   10cb8:	1afffffc 	bne	10cb0 <sata_vs_reset+0x258>
   10cbc:	e59f207c 	ldr	r2, [pc, #124]	; 10d40 <sata_vs_reset+0x2e8>
   10cc0:	e3e01000 	mvn	r1, #0	; 0x0
   10cc4:	e5820100 	str	r0, [r2, #256]
   10cc8:	e582e108 	str	lr, [r2, #264]
   10ccc:	e5923118 	ldr	r3, [r2, #280]
   10cd0:	e3833010 	orr	r3, r3, #16	; 0x10
   10cd4:	e5823118 	str	r3, [r2, #280]
   10cd8:	e3e03502 	mvn	r3, #8388608	; 0x800000
   10cdc:	e5821130 	str	r1, [r2, #304]
   10ce0:	e5821110 	str	r1, [r2, #272]
   10ce4:	e5821008 	str	r1, [r2, #8]
   10ce8:	e5823114 	str	r3, [r2, #276]
   10cec:	e5923004 	ldr	r3, [r2, #4]
   10cf0:	e1a01002 	mov	r1, r2
   10cf4:	e3833002 	orr	r3, r3, #2	; 0x2
   10cf8:	e5823004 	str	r3, [r2, #4]
   10cfc:	e5913128 	ldr	r3, [r1, #296]
   10d00:	e5912120 	ldr	r2, [r1, #288]
   10d04:	e203300f 	and	r3, r3, #15	; 0xf
   10d08:	e3530003 	cmp	r3, #3	; 0x3
   10d0c:	1afffffa 	bne	10cfc <sata_vs_reset+0x2a4>
   10d10:	e2124089 	ands	r4, r2, #137	; 0x89
   10d14:	1afffff8 	bne	10cfc <sata_vs_reset+0x2a4>
   10d18:	e1a00005 	mov	r0, r5
   10d1c:	ebffc29d 	bl	1798 <free>
   10d20:	e59f002c 	ldr	r0, [pc, #44]	; 10d54 <sata_vs_reset+0x2fc>
   10d24:	e59f301c 	ldr	r3, [pc, #28]	; 10d48 <sata_vs_reset+0x2f0>
   10d28:	e1a0e00f 	mov	lr, pc
   10d2c:	e593f000 	ldr	pc, [r3]
   10d30:	e1a00004 	mov	r0, r4
   10d34:	e8bd8070 	pop	{r4, r5, r6, pc}
   10d38:	21e3c800 	.word	0x21e3c800
   10d3c:	21e3c000 	.word	0x21e3c000
   10d40:	28200000 	.word	0x28200000
   10d44:	000192d4 	.word	0x000192d4
   10d48:	3c00a5cc 	.word	0x3c00a5cc
   10d4c:	000192f5 	.word	0x000192f5
   10d50:	00019318 	.word	0x00019318
   10d54:	0001932e 	.word	0x0001932e

00010d58 <init_led>:
   10d58:	e3a0c000 	mov	ip, #0	; 0x0
   10d5c:	e59f3010 	ldr	r3, [pc, #16]	; 10d74 <init_led+0x1c>
   10d60:	e59f0010 	ldr	r0, [pc, #16]	; 10d78 <init_led+0x20>
   10d64:	e1a0200c 	mov	r2, ip
   10d68:	e3a01002 	mov	r1, #2	; 0x2
   10d6c:	e583c000 	str	ip, [r3]
   10d70:	eaffbfa5 	b	c0c <pads_chmod>
   10d74:	3c00a780 	.word	0x3c00a780
   10d78:	002e002d 	.word	0x002e002d

00010d7c <led_off>:
   10d7c:	e3a02000 	mov	r2, #0	; 0x0
   10d80:	e59f300c 	ldr	r3, [pc, #12]	; 10d94 <led_off+0x18>
   10d84:	e1a01002 	mov	r1, r2
   10d88:	e3a0002e 	mov	r0, #46	; 0x2e
   10d8c:	e5832000 	str	r2, [r3]
   10d90:	eaffc034 	b	e68 <pads_setpin>
   10d94:	3c00a780 	.word	0x3c00a780

00010d98 <led_on>:
   10d98:	e3a02001 	mov	r2, #1	; 0x1
   10d9c:	e59f300c 	ldr	r3, [pc, #12]	; 10db0 <led_on+0x18>
   10da0:	e1a01002 	mov	r1, r2
   10da4:	e3a0002e 	mov	r0, #46	; 0x2e
   10da8:	e5832000 	str	r2, [r3]
   10dac:	eaffc02d 	b	e68 <pads_setpin>
   10db0:	3c00a780 	.word	0x3c00a780

00010db4 <led_pulse>:
   10db4:	e92d4010 	push	{r4, lr}
   10db8:	e1a04000 	mov	r4, r0
   10dbc:	ebfffff5 	bl	10d98 <led_on>
   10dc0:	e3a00005 	mov	r0, #5	; 0x5
   10dc4:	ea000001 	b	10dd0 <led_pulse+0x1c>
   10dc8:	ebfffff2 	bl	10d98 <led_on>
   10dcc:	e3a00002 	mov	r0, #2	; 0x2
   10dd0:	ebfff62e 	bl	e690 <udelay>
   10dd4:	ebffffe8 	bl	10d7c <led_off>
   10dd8:	e3a00002 	mov	r0, #2	; 0x2
   10ddc:	ebfff62b 	bl	e690 <udelay>
   10de0:	e3540000 	cmp	r4, #0	; 0x0
   10de4:	e2444001 	sub	r4, r4, #1	; 0x1
   10de8:	cafffff6 	bgt	10dc8 <led_pulse+0x14>
   10dec:	e8bd8010 	pop	{r4, pc}

00010df0 <led_ctrl>:
   10df0:	e3500000 	cmp	r0, #0	; 0x0
   10df4:	0a000000 	beq	10dfc <led_ctrl+0xc>
   10df8:	eaffffe6 	b	10d98 <led_on>
   10dfc:	eaffffde 	b	10d7c <led_off>

00010e00 <led_toggle>:
   10e00:	e59f300c 	ldr	r3, [pc, #12]	; 10e14 <led_toggle+0x14>
   10e04:	e5930000 	ldr	r0, [r3]
   10e08:	e2700001 	rsbs	r0, r0, #1	; 0x1
   10e0c:	33a00000 	movcc	r0, #0	; 0x0
   10e10:	eafffff6 	b	10df0 <led_ctrl>
   10e14:	3c00a780 	.word	0x3c00a780

00010e18 <udc_event_handle>:
   10e18:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   10e1c:	e59f4688 	ldr	r4, [pc, #1672]	; 114ac <udc_event_handle+0x694>
   10e20:	e24dd008 	sub	sp, sp, #8	; 0x8
   10e24:	e5948014 	ldr	r8, [r4, #20]
   10e28:	e3180b01 	tst	r8, #1024	; 0x400
   10e2c:	0a000005 	beq	10e48 <udc_event_handle+0x30>
   10e30:	e59f3678 	ldr	r3, [pc, #1656]	; 114b0 <udc_event_handle+0x698>
   10e34:	e59f0678 	ldr	r0, [pc, #1656]	; 114b4 <udc_event_handle+0x69c>
   10e38:	e1a0e00f 	mov	lr, pc
   10e3c:	e593f000 	ldr	pc, [r3]
   10e40:	e3a03b01 	mov	r3, #1024	; 0x400
   10e44:	e5843014 	str	r3, [r4, #20]
   10e48:	e3180b02 	tst	r8, #2048	; 0x800
   10e4c:	0a000006 	beq	10e6c <udc_event_handle+0x54>
   10e50:	e59f3658 	ldr	r3, [pc, #1624]	; 114b0 <udc_event_handle+0x698>
   10e54:	e59f065c 	ldr	r0, [pc, #1628]	; 114b8 <udc_event_handle+0x6a0>
   10e58:	e1a0e00f 	mov	lr, pc
   10e5c:	e593f000 	ldr	pc, [r3]
   10e60:	e59f3644 	ldr	r3, [pc, #1604]	; 114ac <udc_event_handle+0x694>
   10e64:	e3a02b02 	mov	r2, #2048	; 0x800
   10e68:	e5832014 	str	r2, [r3, #20]
   10e6c:	e3180a01 	tst	r8, #4096	; 0x1000
   10e70:	0a000033 	beq	10f44 <udc_event_handle+0x12c>
   10e74:	e59f2630 	ldr	r2, [pc, #1584]	; 114ac <udc_event_handle+0x694>
   10e78:	e3a03302 	mov	r3, #134217728	; 0x8000000
   10e7c:	e5823b00 	str	r3, [r2, #2816]
   10e80:	e5823b40 	str	r3, [r2, #2880]
   10e84:	e59f3630 	ldr	r3, [pc, #1584]	; 114bc <udc_event_handle+0x6a4>
   10e88:	e3a01009 	mov	r1, #9	; 0x9
   10e8c:	e582381c 	str	r3, [r2, #2076]
   10e90:	e3a03b02 	mov	r3, #2048	; 0x800
   10e94:	e5821814 	str	r1, [r2, #2068]
   10e98:	e5821810 	str	r1, [r2, #2064]
   10e9c:	e5823024 	str	r3, [r2, #36]
   10ea0:	e2833301 	add	r3, r3, #67108864	; 0x4000000
   10ea4:	e5823028 	str	r3, [r2, #40]
   10ea8:	e2833b01 	add	r3, r3, #1024	; 0x400
   10eac:	e5823104 	str	r3, [r2, #260]
   10eb0:	e59f3608 	ldr	r3, [pc, #1544]	; 114c0 <udc_event_handle+0x6a8>
   10eb4:	e1a06002 	mov	r6, r2
   10eb8:	e582305c 	str	r3, [r2, #92]
   10ebc:	e5923010 	ldr	r3, [r2, #16]
   10ec0:	e59f75fc 	ldr	r7, [pc, #1532]	; 114c4 <udc_event_handle+0x6ac>
   10ec4:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
   10ec8:	e3833e43 	orr	r3, r3, #1072	; 0x430
   10ecc:	e5823010 	str	r3, [r2, #16]
   10ed0:	ebfff5dd 	bl	e64c <get_ticks>
   10ed4:	e1a04000 	mov	r4, r0
   10ed8:	e1a05001 	mov	r5, r1
   10edc:	e3a00001 	mov	r0, #1	; 0x1
   10ee0:	ebfff5ea 	bl	e690 <udelay>
   10ee4:	e5963010 	ldr	r3, [r6, #16]
   10ee8:	e3130030 	tst	r3, #48	; 0x30
   10eec:	0a000008 	beq	10f14 <udc_event_handle+0xfc>
   10ef0:	ebfff5d5 	bl	e64c <get_ticks>
   10ef4:	e1a02000 	mov	r2, r0
   10ef8:	e1a03001 	mov	r3, r1
   10efc:	e0522004 	subs	r2, r2, r4
   10f00:	e0c33005 	sbc	r3, r3, r5
   10f04:	e3530000 	cmp	r3, #0	; 0x0
   10f08:	1a000001 	bne	10f14 <udc_event_handle+0xfc>
   10f0c:	e1520007 	cmp	r2, r7
   10f10:	9afffff1 	bls	10edc <udc_event_handle+0xc4>
   10f14:	e59f35ac 	ldr	r3, [pc, #1452]	; 114c8 <udc_event_handle+0x6b0>
   10f18:	e59f258c 	ldr	r2, [pc, #1420]	; 114ac <udc_event_handle+0x694>
   10f1c:	e5931024 	ldr	r1, [r3, #36]
   10f20:	e59f35a4 	ldr	r3, [pc, #1444]	; 114cc <udc_event_handle+0x6b4>
   10f24:	e59f05a4 	ldr	r0, [pc, #1444]	; 114d0 <udc_event_handle+0x6b8>
   10f28:	e5823b10 	str	r3, [r2, #2832]
   10f2c:	e3a03a01 	mov	r3, #4096	; 0x1000
   10f30:	e5821b14 	str	r1, [r2, #2836]
   10f34:	e5823014 	str	r3, [r2, #20]
   10f38:	e59f3570 	ldr	r3, [pc, #1392]	; 114b0 <udc_event_handle+0x698>
   10f3c:	e1a0e00f 	mov	lr, pc
   10f40:	e593f000 	ldr	pc, [r3]
   10f44:	e3180a02 	tst	r8, #8192	; 0x2000
   10f48:	0a000010 	beq	10f90 <udc_event_handle+0x178>
   10f4c:	e59f4558 	ldr	r4, [pc, #1368]	; 114ac <udc_event_handle+0x694>
   10f50:	e59f3570 	ldr	r3, [pc, #1392]	; 114c8 <udc_event_handle+0x6b0>
   10f54:	e5941808 	ldr	r1, [r4, #2056]
   10f58:	e59f0574 	ldr	r0, [pc, #1396]	; 114d4 <udc_event_handle+0x6bc>
   10f5c:	e1a010a1 	lsr	r1, r1, #1
   10f60:	e2011003 	and	r1, r1, #3	; 0x3
   10f64:	e5c3101c 	strb	r1, [r3, #28]
   10f68:	e59f5540 	ldr	r5, [pc, #1344]	; 114b0 <udc_event_handle+0x698>
   10f6c:	e1a0e00f 	mov	lr, pc
   10f70:	e595f000 	ldr	pc, [r5]
   10f74:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10f78:	e5843b00 	str	r3, [r4, #2816]
   10f7c:	e3a03a02 	mov	r3, #8192	; 0x2000
   10f80:	e5843014 	str	r3, [r4, #20]
   10f84:	e59f054c 	ldr	r0, [pc, #1356]	; 114d8 <udc_event_handle+0x6c0>
   10f88:	e1a0e00f 	mov	lr, pc
   10f8c:	e595f000 	ldr	pc, [r5]
   10f90:	e3180701 	tst	r8, #262144	; 0x40000
   10f94:	0a00006b 	beq	11148 <udc_event_handle+0x330>
   10f98:	e59f450c 	ldr	r4, [pc, #1292]	; 114ac <udc_event_handle+0x694>
   10f9c:	e5947818 	ldr	r7, [r4, #2072]
   10fa0:	e3170001 	tst	r7, #1	; 0x1
   10fa4:	0a000047 	beq	110c8 <udc_event_handle+0x2b0>
   10fa8:	e5946908 	ldr	r6, [r4, #2312]
   10fac:	e59f5514 	ldr	r5, [pc, #1300]	; 114c8 <udc_event_handle+0x6b0>
   10fb0:	e5943b00 	ldr	r3, [r4, #2816]
   10fb4:	e59f0520 	ldr	r0, [pc, #1312]	; 114dc <udc_event_handle+0x6c4>
   10fb8:	e1a01006 	mov	r1, r6
   10fbc:	e5952020 	ldr	r2, [r5, #32]
   10fc0:	e59fc4e8 	ldr	ip, [pc, #1256]	; 114b0 <udc_event_handle+0x698>
   10fc4:	e1a0e00f 	mov	lr, pc
   10fc8:	e59cf000 	ldr	pc, [ip]
   10fcc:	e3160001 	tst	r6, #1	; 0x1
   10fd0:	0a000026 	beq	11070 <udc_event_handle+0x258>
   10fd4:	e5953020 	ldr	r3, [r5, #32]
   10fd8:	e3a02001 	mov	r2, #1	; 0x1
   10fdc:	e3530002 	cmp	r3, #2	; 0x2
   10fe0:	e5842908 	str	r2, [r4, #2312]
   10fe4:	0a000004 	beq	10ffc <udc_event_handle+0x1e4>
   10fe8:	3a00001c 	bcc	11060 <udc_event_handle+0x248>
   10fec:	e2433004 	sub	r3, r3, #4	; 0x4
   10ff0:	e1530002 	cmp	r3, r2
   10ff4:	8a000019 	bhi	11060 <udc_event_handle+0x248>
   10ff8:	ea00000a 	b	11028 <udc_event_handle+0x210>
   10ffc:	e5943b10 	ldr	r3, [r4, #2832]
   11000:	e5951024 	ldr	r1, [r5, #36]
   11004:	e3c3307f 	bic	r3, r3, #127	; 0x7f
   11008:	e3833702 	orr	r3, r3, #524288	; 0x80000
   1100c:	e5843b10 	str	r3, [r4, #2832]
   11010:	e3a02005 	mov	r2, #5	; 0x5
   11014:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   11018:	e5841b14 	str	r1, [r4, #2836]
   1101c:	e5852020 	str	r2, [r5, #32]
   11020:	e5843b00 	str	r3, [r4, #2816]
   11024:	ea000011 	b	11070 <udc_event_handle+0x258>
   11028:	e5d53015 	ldrb	r3, [r5, #21]
   1102c:	e59f1494 	ldr	r1, [pc, #1172]	; 114c8 <udc_event_handle+0x6b0>
   11030:	e3530009 	cmp	r3, #9	; 0x9
   11034:	05c52000 	strbeq	r2, [r5]
   11038:	e59f348c 	ldr	r3, [pc, #1164]	; 114cc <udc_event_handle+0x6b4>
   1103c:	e59f2468 	ldr	r2, [pc, #1128]	; 114ac <udc_event_handle+0x694>
   11040:	e5910024 	ldr	r0, [r1, #36]
   11044:	e5823b10 	str	r3, [r2, #2832]
   11048:	e3a03001 	mov	r3, #1	; 0x1
   1104c:	e5813020 	str	r3, [r1, #32]
   11050:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   11054:	e5820b14 	str	r0, [r2, #2836]
   11058:	e5823b00 	str	r3, [r2, #2816]
   1105c:	ea000003 	b	11070 <udc_event_handle+0x258>
   11060:	e59f0478 	ldr	r0, [pc, #1144]	; 114e0 <udc_event_handle+0x6c8>
   11064:	e59f3444 	ldr	r3, [pc, #1092]	; 114b0 <udc_event_handle+0x698>
   11068:	e1a0e00f 	mov	lr, pc
   1106c:	e593f000 	ldr	pc, [r3]
   11070:	e3160004 	tst	r6, #4	; 0x4
   11074:	0a000006 	beq	11094 <udc_event_handle+0x27c>
   11078:	e59f242c 	ldr	r2, [pc, #1068]	; 114ac <udc_event_handle+0x694>
   1107c:	e3a03004 	mov	r3, #4	; 0x4
   11080:	e5823908 	str	r3, [r2, #2312]
   11084:	e59f0458 	ldr	r0, [pc, #1112]	; 114e4 <udc_event_handle+0x6cc>
   11088:	e59f3420 	ldr	r3, [pc, #1056]	; 114b0 <udc_event_handle+0x698>
   1108c:	e1a0e00f 	mov	lr, pc
   11090:	e593f000 	ldr	pc, [r3]
   11094:	e3160008 	tst	r6, #8	; 0x8
   11098:	0a000006 	beq	110b8 <udc_event_handle+0x2a0>
   1109c:	e59f2408 	ldr	r2, [pc, #1032]	; 114ac <udc_event_handle+0x694>
   110a0:	e3a03008 	mov	r3, #8	; 0x8
   110a4:	e5823908 	str	r3, [r2, #2312]
   110a8:	e59f0438 	ldr	r0, [pc, #1080]	; 114e8 <udc_event_handle+0x6d0>
   110ac:	e59f33fc 	ldr	r3, [pc, #1020]	; 114b0 <udc_event_handle+0x698>
   110b0:	e1a0e00f 	mov	lr, pc
   110b4:	e593f000 	ldr	pc, [r3]
   110b8:	e3160c02 	tst	r6, #512	; 0x200
   110bc:	159f33e8 	ldrne	r3, [pc, #1000]	; 114ac <udc_event_handle+0x694>
   110c0:	13a02c02 	movne	r2, #512	; 0x200
   110c4:	15832908 	strne	r2, [r3, #2312]
   110c8:	e3170002 	tst	r7, #2	; 0x2
   110cc:	0a00001d 	beq	11148 <udc_event_handle+0x330>
   110d0:	e59f43d4 	ldr	r4, [pc, #980]	; 114ac <udc_event_handle+0x694>
   110d4:	e59f63d4 	ldr	r6, [pc, #980]	; 114b0 <udc_event_handle+0x698>
   110d8:	e5945928 	ldr	r5, [r4, #2344]
   110dc:	e59f0408 	ldr	r0, [pc, #1032]	; 114ec <udc_event_handle+0x6d4>
   110e0:	e1a01005 	mov	r1, r5
   110e4:	e1a0e00f 	mov	lr, pc
   110e8:	e596f000 	ldr	pc, [r6]
   110ec:	e3150001 	tst	r5, #1	; 0x1
   110f0:	0a000010 	beq	11138 <udc_event_handle+0x320>
   110f4:	e59f73cc 	ldr	r7, [pc, #972]	; 114c8 <udc_event_handle+0x6b0>
   110f8:	e3a03001 	mov	r3, #1	; 0x1
   110fc:	e5d72002 	ldrb	r2, [r7, #2]
   11100:	e5843928 	str	r3, [r4, #2344]
   11104:	e3520000 	cmp	r2, #0	; 0x0
   11108:	1a000003 	bne	1111c <udc_event_handle+0x304>
   1110c:	e59f03dc 	ldr	r0, [pc, #988]	; 114f0 <udc_event_handle+0x6d8>
   11110:	e1a0e00f 	mov	lr, pc
   11114:	e596f000 	ldr	pc, [r6]
   11118:	ea00000a 	b	11148 <udc_event_handle+0x330>
   1111c:	e5944930 	ldr	r4, [r4, #2352]
   11120:	e59f03cc 	ldr	r0, [pc, #972]	; 114f4 <udc_event_handle+0x6dc>
   11124:	e1a01004 	mov	r1, r4
   11128:	e1a0e00f 	mov	lr, pc
   1112c:	e596f000 	ldr	pc, [r6]
   11130:	e3d4420f 	bics	r4, r4, #-268435456	; 0xf0000000
   11134:	05c74002 	strbeq	r4, [r7, #2]
   11138:	e3150004 	tst	r5, #4	; 0x4
   1113c:	159f3368 	ldrne	r3, [pc, #872]	; 114ac <udc_event_handle+0x694>
   11140:	13a02004 	movne	r2, #4	; 0x4
   11144:	15832928 	strne	r2, [r3, #2344]
   11148:	e3180702 	tst	r8, #524288	; 0x80000
   1114c:	0a0000d4 	beq	114a4 <udc_event_handle+0x68c>
   11150:	e59f4354 	ldr	r4, [pc, #852]	; 114ac <udc_event_handle+0x694>
   11154:	e5948818 	ldr	r8, [r4, #2072]
   11158:	e3180801 	tst	r8, #65536	; 0x10000
   1115c:	0a00009c 	beq	113d4 <udc_event_handle+0x5bc>
   11160:	e5947b08 	ldr	r7, [r4, #2824]
   11164:	e59f535c 	ldr	r5, [pc, #860]	; 114c8 <udc_event_handle+0x6b0>
   11168:	e5943b10 	ldr	r3, [r4, #2832]
   1116c:	e59f633c 	ldr	r6, [pc, #828]	; 114b0 <udc_event_handle+0x698>
   11170:	e59f0380 	ldr	r0, [pc, #896]	; 114f8 <udc_event_handle+0x6e0>
   11174:	e1a01007 	mov	r1, r7
   11178:	e5952020 	ldr	r2, [r5, #32]
   1117c:	e1a0e00f 	mov	lr, pc
   11180:	e596f000 	ldr	pc, [r6]
   11184:	e3170001 	tst	r7, #1	; 0x1
   11188:	0a000010 	beq	111d0 <udc_event_handle+0x3b8>
   1118c:	e5953020 	ldr	r3, [r5, #32]
   11190:	e3a01001 	mov	r1, #1	; 0x1
   11194:	e2433004 	sub	r3, r3, #4	; 0x4
   11198:	e1530001 	cmp	r3, r1
   1119c:	e5841b08 	str	r1, [r4, #2824]
   111a0:	8a000007 	bhi	111c4 <udc_event_handle+0x3ac>
   111a4:	e5953024 	ldr	r3, [r5, #36]
   111a8:	e59f231c 	ldr	r2, [pc, #796]	; 114cc <udc_event_handle+0x6b4>
   111ac:	e5851020 	str	r1, [r5, #32]
   111b0:	e5842b10 	str	r2, [r4, #2832]
   111b4:	e5843b14 	str	r3, [r4, #2836]
   111b8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   111bc:	e5843b00 	str	r3, [r4, #2816]
   111c0:	ea000002 	b	111d0 <udc_event_handle+0x3b8>
   111c4:	e59f0330 	ldr	r0, [pc, #816]	; 114fc <udc_event_handle+0x6e4>
   111c8:	e1a0e00f 	mov	lr, pc
   111cc:	e596f000 	ldr	pc, [r6]
   111d0:	e3170004 	tst	r7, #4	; 0x4
   111d4:	0a000006 	beq	111f4 <udc_event_handle+0x3dc>
   111d8:	e59f22cc 	ldr	r2, [pc, #716]	; 114ac <udc_event_handle+0x694>
   111dc:	e3a03004 	mov	r3, #4	; 0x4
   111e0:	e5823b08 	str	r3, [r2, #2824]
   111e4:	e59f0314 	ldr	r0, [pc, #788]	; 11500 <udc_event_handle+0x6e8>
   111e8:	e59f32c0 	ldr	r3, [pc, #704]	; 114b0 <udc_event_handle+0x698>
   111ec:	e1a0e00f 	mov	lr, pc
   111f0:	e593f000 	ldr	pc, [r3]
   111f4:	e3170008 	tst	r7, #8	; 0x8
   111f8:	0a000071 	beq	113c4 <udc_event_handle+0x5ac>
   111fc:	e3170040 	tst	r7, #64	; 0x40
   11200:	e59fc2fc 	ldr	ip, [pc, #764]	; 11504 <udc_event_handle+0x6ec>
   11204:	0a000006 	beq	11224 <udc_event_handle+0x40c>
   11208:	e59f329c 	ldr	r3, [pc, #668]	; 114ac <udc_event_handle+0x694>
   1120c:	e5933b14 	ldr	r3, [r3, #2836]
   11210:	e2432004 	sub	r2, r3, #4	; 0x4
   11214:	e2433008 	sub	r3, r3, #8	; 0x8
   11218:	e58c2004 	str	r2, [ip, #4]
   1121c:	e58c3000 	str	r3, [ip]
   11220:	ea000009 	b	1124c <udc_event_handle+0x434>
   11224:	e59f329c 	ldr	r3, [pc, #668]	; 114c8 <udc_event_handle+0x6b0>
   11228:	e59f02d8 	ldr	r0, [pc, #728]	; 11508 <udc_event_handle+0x6f0>
   1122c:	e5933024 	ldr	r3, [r3, #36]
   11230:	e5931000 	ldr	r1, [r3]
   11234:	e58c1000 	str	r1, [ip]
   11238:	e5932004 	ldr	r2, [r3, #4]
   1123c:	e59f326c 	ldr	r3, [pc, #620]	; 114b0 <udc_event_handle+0x698>
   11240:	e58c2004 	str	r2, [ip, #4]
   11244:	e1a0e00f 	mov	lr, pc
   11248:	e593f000 	ldr	pc, [r3]
   1124c:	e59f1274 	ldr	r1, [pc, #628]	; 114c8 <udc_event_handle+0x6b0>
   11250:	e3a02008 	mov	r2, #8	; 0x8
   11254:	e1d131d4 	ldrsb	r3, [r1, #20]
   11258:	e59f4268 	ldr	r4, [pc, #616]	; 114c8 <udc_event_handle+0x6b0>
   1125c:	e3530000 	cmp	r3, #0	; 0x0
   11260:	e59f3244 	ldr	r3, [pc, #580]	; 114ac <udc_event_handle+0x694>
   11264:	e5832b08 	str	r2, [r3, #2824]
   11268:	e59f2258 	ldr	r2, [pc, #600]	; 114c8 <udc_event_handle+0x6b0>
   1126c:	b3a03002 	movlt	r3, #2	; 0x2
   11270:	a3a03003 	movge	r3, #3	; 0x3
   11274:	e5813020 	str	r3, [r1, #32]
   11278:	e1d231ba 	ldrh	r3, [r2, #26]
   1127c:	e3530000 	cmp	r3, #0	; 0x0
   11280:	02833004 	addeq	r3, r3, #4	; 0x4
   11284:	05823020 	streq	r3, [r2, #32]
   11288:	e5d43015 	ldrb	r3, [r4, #21]
   1128c:	e3530006 	cmp	r3, #6	; 0x6
   11290:	0a000004 	beq	112a8 <udc_event_handle+0x490>
   11294:	e3530009 	cmp	r3, #9	; 0x9
   11298:	0a000042 	beq	113a8 <udc_event_handle+0x590>
   1129c:	e3530005 	cmp	r3, #5	; 0x5
   112a0:	1a000047 	bne	113c4 <udc_event_handle+0x5ac>
   112a4:	ea000034 	b	1137c <udc_event_handle+0x564>
   112a8:	e3a00040 	mov	r0, #64	; 0x40
   112ac:	ebffc0d1 	bl	15f8 <malloc>
   112b0:	e2505000 	subs	r5, r0, #0	; 0x0
   112b4:	1a000004 	bne	112cc <udc_event_handle+0x4b4>
   112b8:	e59f024c 	ldr	r0, [pc, #588]	; 1150c <udc_event_handle+0x6f4>
   112bc:	e59f31ec 	ldr	r3, [pc, #492]	; 114b0 <udc_event_handle+0x698>
   112c0:	e1a0e00f 	mov	lr, pc
   112c4:	e593f000 	ldr	pc, [r3]
   112c8:	ea00003d 	b	113c4 <udc_event_handle+0x5ac>
   112cc:	e1d4c1b6 	ldrh	ip, [r4, #22]
   112d0:	e35c0c01 	cmp	ip, #256	; 0x100
   112d4:	0a000002 	beq	112e4 <udc_event_handle+0x4cc>
   112d8:	e35c0c02 	cmp	ip, #512	; 0x200
   112dc:	1a00001a 	bne	1134c <udc_event_handle+0x534>
   112e0:	ea000004 	b	112f8 <udc_event_handle+0x4e0>
   112e4:	e59f1224 	ldr	r1, [pc, #548]	; 11510 <udc_event_handle+0x6f8>
   112e8:	e3a02012 	mov	r2, #18	; 0x12
   112ec:	ebffc05c 	bl	1464 <memcpy>
   112f0:	e3a03012 	mov	r3, #18	; 0x12
   112f4:	ea000003 	b	11308 <udc_event_handle+0x4f0>
   112f8:	e59f1214 	ldr	r1, [pc, #532]	; 11514 <udc_event_handle+0x6fc>
   112fc:	e3a02020 	mov	r2, #32	; 0x20
   11300:	ebffc057 	bl	1464 <memcpy>
   11304:	e3a03020 	mov	r3, #32	; 0x20
   11308:	e59f11b8 	ldr	r1, [pc, #440]	; 114c8 <udc_event_handle+0x6b0>
   1130c:	e5843004 	str	r3, [r4, #4]
   11310:	e1d120b4 	ldrh	r2, [r1, #4]
   11314:	e1d131ba 	ldrh	r3, [r1, #26]
   11318:	e59f018c 	ldr	r0, [pc, #396]	; 114ac <udc_event_handle+0x694>
   1131c:	e1520003 	cmp	r2, r3
   11320:	21a02003 	movcs	r2, r3
   11324:	e3823702 	orr	r3, r2, #524288	; 0x80000
   11328:	e5803910 	str	r3, [r0, #2320]
   1132c:	e3a03000 	mov	r3, #0	; 0x0
   11330:	e5c13001 	strb	r3, [r1, #1]
   11334:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
   11338:	e5812004 	str	r2, [r1, #4]
   1133c:	e5805914 	str	r5, [r0, #2324]
   11340:	e5803900 	str	r3, [r0, #2304]
   11344:	e581500c 	str	r5, [r1, #12]
   11348:	ea000052 	b	11498 <udc_event_handle+0x680>
   1134c:	e1d401ba 	ldrh	r0, [r4, #26]
   11350:	e1d431b8 	ldrh	r3, [r4, #24]
   11354:	e5d41014 	ldrb	r1, [r4, #20]
   11358:	e5d42015 	ldrb	r2, [r4, #21]
   1135c:	e58d0004 	str	r0, [sp, #4]
   11360:	e58d3000 	str	r3, [sp]
   11364:	e59f01ac 	ldr	r0, [pc, #428]	; 11518 <udc_event_handle+0x700>
   11368:	e1a0300c 	mov	r3, ip
   1136c:	e59fc13c 	ldr	ip, [pc, #316]	; 114b0 <udc_event_handle+0x698>
   11370:	e1a0e00f 	mov	lr, pc
   11374:	e59cf000 	ldr	pc, [ip]
   11378:	ea000046 	b	11498 <udc_event_handle+0x680>
   1137c:	e59f2128 	ldr	r2, [pc, #296]	; 114ac <udc_event_handle+0x694>
   11380:	e1d411b6 	ldrh	r1, [r4, #22]
   11384:	e5923800 	ldr	r3, [r2, #2048]
   11388:	e5940024 	ldr	r0, [r4, #36]
   1138c:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
   11390:	e1833201 	orr	r3, r3, r1, lsl #4
   11394:	e5823800 	str	r3, [r2, #2048]
   11398:	e3a03702 	mov	r3, #524288	; 0x80000
   1139c:	e5823910 	str	r3, [r2, #2320]
   113a0:	e5820914 	str	r0, [r2, #2324]
   113a4:	ea000004 	b	113bc <udc_event_handle+0x5a4>
   113a8:	e59f20fc 	ldr	r2, [pc, #252]	; 114ac <udc_event_handle+0x694>
   113ac:	e5941024 	ldr	r1, [r4, #36]
   113b0:	e3a03702 	mov	r3, #524288	; 0x80000
   113b4:	e5823910 	str	r3, [r2, #2320]
   113b8:	e5821914 	str	r1, [r2, #2324]
   113bc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   113c0:	e5823900 	str	r3, [r2, #2304]
   113c4:	e3170c02 	tst	r7, #512	; 0x200
   113c8:	159f30dc 	ldrne	r3, [pc, #220]	; 114ac <udc_event_handle+0x694>
   113cc:	13a02c02 	movne	r2, #512	; 0x200
   113d0:	15832b08 	strne	r2, [r3, #2824]
   113d4:	e3180701 	tst	r8, #262144	; 0x40000
   113d8:	0a000031 	beq	114a4 <udc_event_handle+0x68c>
   113dc:	e59f60c8 	ldr	r6, [pc, #200]	; 114ac <udc_event_handle+0x694>
   113e0:	e59f80c8 	ldr	r8, [pc, #200]	; 114b0 <udc_event_handle+0x698>
   113e4:	e5967b48 	ldr	r7, [r6, #2888]
   113e8:	e59f012c 	ldr	r0, [pc, #300]	; 1151c <udc_event_handle+0x704>
   113ec:	e1a01007 	mov	r1, r7
   113f0:	e1a0e00f 	mov	lr, pc
   113f4:	e598f000 	ldr	pc, [r8]
   113f8:	e3170001 	tst	r7, #1	; 0x1
   113fc:	0a000020 	beq	11484 <udc_event_handle+0x66c>
   11400:	e59f50c0 	ldr	r5, [pc, #192]	; 114c8 <udc_event_handle+0x6b0>
   11404:	e3a03001 	mov	r3, #1	; 0x1
   11408:	e5d52003 	ldrb	r2, [r5, #3]
   1140c:	e5863b48 	str	r3, [r6, #2888]
   11410:	e3520000 	cmp	r2, #0	; 0x0
   11414:	1a000003 	bne	11428 <udc_event_handle+0x610>
   11418:	e59f0100 	ldr	r0, [pc, #256]	; 11520 <udc_event_handle+0x708>
   1141c:	e1a0e00f 	mov	lr, pc
   11420:	e598f000 	ldr	pc, [r8]
   11424:	ea00001e 	b	114a4 <udc_event_handle+0x68c>
   11428:	e5964b50 	ldr	r4, [r6, #2896]
   1142c:	e59f00f0 	ldr	r0, [pc, #240]	; 11524 <udc_event_handle+0x70c>
   11430:	e1a01004 	mov	r1, r4
   11434:	e1a0e00f 	mov	lr, pc
   11438:	e598f000 	ldr	pc, [r8]
   1143c:	e3c444ff 	bic	r4, r4, #-16777216	; 0xff000000
   11440:	e5953028 	ldr	r3, [r5, #40]
   11444:	e3c4473e 	bic	r4, r4, #16252928	; 0xf80000
   11448:	e0641003 	rsb	r1, r4, r3
   1144c:	e3510000 	cmp	r1, #0	; 0x0
   11450:	e5851028 	str	r1, [r5, #40]
   11454:	05c51003 	strbeq	r1, [r5, #3]
   11458:	0a000009 	beq	11484 <udc_event_handle+0x66c>
   1145c:	e5953008 	ldr	r3, [r5, #8]
   11460:	e5952010 	ldr	r2, [r5, #16]
   11464:	e0613003 	rsb	r3, r1, r3
   11468:	e0833002 	add	r3, r3, r2
   1146c:	e3812702 	orr	r2, r1, #524288	; 0x80000
   11470:	e5862b50 	str	r2, [r6, #2896]
   11474:	e5863b14 	str	r3, [r6, #2836]
   11478:	e5963b40 	ldr	r3, [r6, #2880]
   1147c:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   11480:	e5863b40 	str	r3, [r6, #2880]
   11484:	e3170004 	tst	r7, #4	; 0x4
   11488:	159f301c 	ldrne	r3, [pc, #28]	; 114ac <udc_event_handle+0x694>
   1148c:	13a02004 	movne	r2, #4	; 0x4
   11490:	15832b48 	strne	r2, [r3, #2888]
   11494:	ea000002 	b	114a4 <udc_event_handle+0x68c>
   11498:	e1a00005 	mov	r0, r5
   1149c:	ebffc0bd 	bl	1798 <free>
   114a0:	eaffffc7 	b	113c4 <udc_event_handle+0x5ac>
   114a4:	e28dd008 	add	sp, sp, #8	; 0x8
   114a8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   114ac:	27100000 	.word	0x27100000
   114b0:	3c00a5cc 	.word	0x3c00a5cc
   114b4:	0001933e 	.word	0x0001933e
   114b8:	0001934c 	.word	0x0001934c
   114bc:	00010001 	.word	0x00010001
   114c0:	0c001780 	.word	0x0c001780
   114c4:	0007a11f 	.word	0x0007a11f
   114c8:	3c00a138 	.word	0x3c00a138
   114cc:	60080040 	.word	0x60080040
   114d0:	0001935c 	.word	0x0001935c
   114d4:	00019361 	.word	0x00019361
   114d8:	0001936e 	.word	0x0001936e
   114dc:	00019374 	.word	0x00019374
   114e0:	00019387 	.word	0x00019387
   114e4:	000193b2 	.word	0x000193b2
   114e8:	000193cb 	.word	0x000193cb
   114ec:	000193e3 	.word	0x000193e3
   114f0:	000193f0 	.word	0x000193f0
   114f4:	0001940e 	.word	0x0001940e
   114f8:	0001941e 	.word	0x0001941e
   114fc:	00019431 	.word	0x00019431
   11500:	0001945d 	.word	0x0001945d
   11504:	3c00a14c 	.word	0x3c00a14c
   11508:	0001954d 	.word	0x0001954d
   1150c:	00019477 	.word	0x00019477
   11510:	0001657e 	.word	0x0001657e
   11514:	00016590 	.word	0x00016590
   11518:	00019496 	.word	0x00019496
   1151c:	000194e2 	.word	0x000194e2
   11520:	000194ef 	.word	0x000194ef
   11524:	0001950d 	.word	0x0001950d

00011528 <udc_write>:
   11528:	e92d4030 	push	{r4, r5, lr}
   1152c:	e59f50a8 	ldr	r5, [pc, #168]	; 115dc <udc_write+0xb4>
   11530:	e24dd004 	sub	sp, sp, #4	; 0x4
   11534:	e1d530b2 	ldrh	r3, [r5, #2]
   11538:	e1a04001 	mov	r4, r1
   1153c:	e3530000 	cmp	r3, #0	; 0x0
   11540:	e59fc098 	ldr	ip, [pc, #152]	; 115e0 <udc_write+0xb8>
   11544:	0a000004 	beq	1155c <udc_write+0x34>
   11548:	e59f0094 	ldr	r0, [pc, #148]	; 115e4 <udc_write+0xbc>
   1154c:	e1a0e00f 	mov	lr, pc
   11550:	e59cf000 	ldr	pc, [ip]
   11554:	e3e04000 	mvn	r4, #0	; 0x0
   11558:	ea00001c 	b	115d0 <udc_write+0xa8>
   1155c:	e2813f7f 	add	r3, r1, #508	; 0x1fc
   11560:	e2833003 	add	r3, r3, #3	; 0x3
   11564:	e2812fff 	add	r2, r1, #1020	; 0x3fc
   11568:	e3530000 	cmp	r3, #0	; 0x0
   1156c:	e2822002 	add	r2, r2, #2	; 0x2
   11570:	b1a03002 	movlt	r3, r2
   11574:	e1a034c3 	asr	r3, r3, #9
   11578:	e59f2068 	ldr	r2, [pc, #104]	; 115e8 <udc_write+0xc0>
   1157c:	e1813983 	orr	r3, r1, r3, lsl #19
   11580:	e5823930 	str	r3, [r2, #2352]
   11584:	e5820934 	str	r0, [r2, #2356]
   11588:	e5923920 	ldr	r3, [r2, #2336]
   1158c:	e3a01001 	mov	r1, #1	; 0x1
   11590:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   11594:	e5823920 	str	r3, [r2, #2336]
   11598:	e5c51002 	strb	r1, [r5, #2]
   1159c:	e5c51001 	strb	r1, [r5, #1]
   115a0:	e585000c 	str	r0, [r5, #12]
   115a4:	e1a01000 	mov	r1, r0
   115a8:	e5854004 	str	r4, [r5, #4]
   115ac:	e59f0038 	ldr	r0, [pc, #56]	; 115ec <udc_write+0xc4>
   115b0:	e1a02004 	mov	r2, r4
   115b4:	e1a0e00f 	mov	lr, pc
   115b8:	e59cf000 	ldr	pc, [ip]
   115bc:	ea000000 	b	115c4 <udc_write+0x9c>
   115c0:	ebfffe14 	bl	10e18 <udc_event_handle>
   115c4:	e5d53002 	ldrb	r3, [r5, #2]
   115c8:	e3530000 	cmp	r3, #0	; 0x0
   115cc:	1afffffb 	bne	115c0 <udc_write+0x98>
   115d0:	e1a00004 	mov	r0, r4
   115d4:	e28dd004 	add	sp, sp, #4	; 0x4
   115d8:	e8bd8030 	pop	{r4, r5, pc}
   115dc:	3c00a138 	.word	0x3c00a138
   115e0:	3c00a5cc 	.word	0x3c00a5cc
   115e4:	0001951c 	.word	0x0001951c
   115e8:	27100000 	.word	0x27100000
   115ec:	00019542 	.word	0x00019542

000115f0 <udc_read>:
   115f0:	e92d4030 	push	{r4, r5, lr}
   115f4:	e59fc094 	ldr	ip, [pc, #148]	; 11690 <udc_read+0xa0>
   115f8:	e24dd004 	sub	sp, sp, #4	; 0x4
   115fc:	e1dc30b2 	ldrh	r3, [ip, #2]
   11600:	e1a04001 	mov	r4, r1
   11604:	e3530000 	cmp	r3, #0	; 0x0
   11608:	0a000005 	beq	11624 <udc_read+0x34>
   1160c:	e59f0080 	ldr	r0, [pc, #128]	; 11694 <udc_read+0xa4>
   11610:	e59f3080 	ldr	r3, [pc, #128]	; 11698 <udc_read+0xa8>
   11614:	e1a0e00f 	mov	lr, pc
   11618:	e593f000 	ldr	pc, [r3]
   1161c:	e3e04000 	mvn	r4, #0	; 0x0
   11620:	ea000017 	b	11684 <udc_read+0x94>
   11624:	e2813f7f 	add	r3, r1, #508	; 0x1fc
   11628:	e2833003 	add	r3, r3, #3	; 0x3
   1162c:	e2812fff 	add	r2, r1, #1020	; 0x3fc
   11630:	e2822002 	add	r2, r2, #2	; 0x2
   11634:	e3530000 	cmp	r3, #0	; 0x0
   11638:	b1a03002 	movlt	r3, r2
   1163c:	e1a034c3 	asr	r3, r3, #9
   11640:	e59f2054 	ldr	r2, [pc, #84]	; 1169c <udc_read+0xac>
   11644:	e1813983 	orr	r3, r1, r3, lsl #19
   11648:	e5823b50 	str	r3, [r2, #2896]
   1164c:	e5820b54 	str	r0, [r2, #2900]
   11650:	e5923b40 	ldr	r3, [r2, #2880]
   11654:	e1a0500c 	mov	r5, ip
   11658:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   1165c:	e5823b40 	str	r3, [r2, #2880]
   11660:	e3a03001 	mov	r3, #1	; 0x1
   11664:	e5cc3003 	strb	r3, [ip, #3]
   11668:	e58c1008 	str	r1, [ip, #8]
   1166c:	e58c0010 	str	r0, [ip, #16]
   11670:	ea000000 	b	11678 <udc_read+0x88>
   11674:	ebfffde7 	bl	10e18 <udc_event_handle>
   11678:	e5d53003 	ldrb	r3, [r5, #3]
   1167c:	e3530000 	cmp	r3, #0	; 0x0
   11680:	1afffffb 	bne	11674 <udc_read+0x84>
   11684:	e1a00004 	mov	r0, r4
   11688:	e28dd004 	add	sp, sp, #4	; 0x4
   1168c:	e8bd8030 	pop	{r4, r5, pc}
   11690:	3c00a138 	.word	0x3c00a138
   11694:	00019558 	.word	0x00019558
   11698:	3c00a5cc 	.word	0x3c00a5cc
   1169c:	27100000 	.word	0x27100000

000116a0 <udc_connect>:
   116a0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   116a4:	e3a00028 	mov	r0, #40	; 0x28
   116a8:	e24dd004 	sub	sp, sp, #4	; 0x4
   116ac:	ebffbfd1 	bl	15f8 <malloc>
   116b0:	e59f3168 	ldr	r3, [pc, #360]	; 11820 <udc_connect+0x180>
   116b4:	e59f5168 	ldr	r5, [pc, #360]	; 11824 <udc_connect+0x184>
   116b8:	e3a06001 	mov	r6, #1	; 0x1
   116bc:	e5830024 	str	r0, [r3, #36]
   116c0:	e5836020 	str	r6, [r3, #32]
   116c4:	e59f015c 	ldr	r0, [pc, #348]	; 11828 <udc_connect+0x188>
   116c8:	ebffbcf2 	bl	a98 <module_enable>
   116cc:	e5953420 	ldr	r3, [r5, #1056]
   116d0:	e1a00006 	mov	r0, r6
   116d4:	e3c33007 	bic	r3, r3, #7	; 0x7
   116d8:	e3833005 	orr	r3, r3, #5	; 0x5
   116dc:	e5853420 	str	r3, [r5, #1056]
   116e0:	e5953424 	ldr	r3, [r5, #1060]
   116e4:	e59f7140 	ldr	r7, [pc, #320]	; 1182c <udc_connect+0x18c>
   116e8:	e3833003 	orr	r3, r3, #3	; 0x3
   116ec:	e5853424 	str	r3, [r5, #1060]
   116f0:	e595342c 	ldr	r3, [r5, #1068]
   116f4:	e1833006 	orr	r3, r3, r6
   116f8:	e585342c 	str	r3, [r5, #1068]
   116fc:	e5954400 	ldr	r4, [r5, #1024]
   11700:	e384400f 	orr	r4, r4, #15	; 0xf
   11704:	e5854400 	str	r4, [r5, #1024]
   11708:	ebfff3e0 	bl	e690 <udelay>
   1170c:	e3c4300e 	bic	r3, r4, #14	; 0xe
   11710:	e5853400 	str	r3, [r5, #1024]
   11714:	e1a00006 	mov	r0, r6
   11718:	e3c4400f 	bic	r4, r4, #15	; 0xf
   1171c:	ebfff3db 	bl	e690 <udelay>
   11720:	e1a00006 	mov	r0, r6
   11724:	e5854400 	str	r4, [r5, #1024]
   11728:	ebfff3d8 	bl	e690 <udelay>
   1172c:	e59f30fc 	ldr	r3, [pc, #252]	; 11830 <udc_connect+0x190>
   11730:	e59f00fc 	ldr	r0, [pc, #252]	; 11834 <udc_connect+0x194>
   11734:	e587300c 	str	r3, [r7, #12]
   11738:	e5973010 	ldr	r3, [r7, #16]
   1173c:	e1833006 	orr	r3, r3, r6
   11740:	e5873010 	str	r3, [r7, #16]
   11744:	ebfff3d1 	bl	e690 <udelay>
   11748:	e5973010 	ldr	r3, [r7, #16]
   1174c:	e59f50d8 	ldr	r5, [pc, #216]	; 1182c <udc_connect+0x18c>
   11750:	e3130001 	tst	r3, #1	; 0x1
   11754:	1afffffb 	bne	11748 <udc_connect+0xa8>
   11758:	e3a03027 	mov	r3, #39	; 0x27
   1175c:	e5853008 	str	r3, [r5, #8]
   11760:	e59f30b8 	ldr	r3, [pc, #184]	; 11820 <udc_connect+0x180>
   11764:	e59f40cc 	ldr	r4, [pc, #204]	; 11838 <udc_connect+0x198>
   11768:	e5931024 	ldr	r1, [r3, #36]
   1176c:	e59f00c8 	ldr	r0, [pc, #200]	; 1183c <udc_connect+0x19c>
   11770:	e1a0e00f 	mov	lr, pc
   11774:	e594f000 	ldr	pc, [r4]
   11778:	e595103c 	ldr	r1, [r5, #60]
   1177c:	e59f30bc 	ldr	r3, [pc, #188]	; 11840 <udc_connect+0x1a0>
   11780:	e1510003 	cmp	r1, r3
   11784:	0a000002 	beq	11794 <udc_connect+0xf4>
   11788:	e59f00b4 	ldr	r0, [pc, #180]	; 11844 <udc_connect+0x1a4>
   1178c:	e1a0e00f 	mov	lr, pc
   11790:	e594f000 	ldr	pc, [r4]
   11794:	e59f2090 	ldr	r2, [pc, #144]	; 1182c <udc_connect+0x18c>
   11798:	e5923014 	ldr	r3, [r2, #20]
   1179c:	e3130001 	tst	r3, #1	; 0x1
   117a0:	0a000009 	beq	117cc <udc_connect+0x12c>
   117a4:	e59f408c 	ldr	r4, [pc, #140]	; 11838 <udc_connect+0x198>
   117a8:	e59f0098 	ldr	r0, [pc, #152]	; 11848 <udc_connect+0x1a8>
   117ac:	e1a0e00f 	mov	lr, pc
   117b0:	e594f000 	ldr	pc, [r4]
   117b4:	e59f0090 	ldr	r0, [pc, #144]	; 1184c <udc_connect+0x1ac>
   117b8:	e3e01000 	mvn	r1, #0	; 0x0
   117bc:	e1a0e00f 	mov	lr, pc
   117c0:	e594f000 	ldr	pc, [r4]
   117c4:	e3e00000 	mvn	r0, #0	; 0x0
   117c8:	ea000012 	b	11818 <udc_connect+0x178>
   117cc:	e3a03301 	mov	r3, #67108864	; 0x4000000
   117d0:	e5823800 	str	r3, [r2, #2048]
   117d4:	e59f3074 	ldr	r3, [pc, #116]	; 11850 <udc_connect+0x1b0>
   117d8:	e59f4040 	ldr	r4, [pc, #64]	; 11820 <udc_connect+0x180>
   117dc:	e5823018 	str	r3, [r2, #24]
   117e0:	ebfff399 	bl	e64c <get_ticks>
   117e4:	ebfffd8b 	bl	10e18 <udc_event_handle>
   117e8:	e5d43000 	ldrb	r3, [r4]
   117ec:	e3530000 	cmp	r3, #0	; 0x0
   117f0:	0afffffb 	beq	117e4 <udc_connect+0x144>
   117f4:	e59f3030 	ldr	r3, [pc, #48]	; 1182c <udc_connect+0x18c>
   117f8:	e59f2054 	ldr	r2, [pc, #84]	; 11854 <udc_connect+0x1b4>
   117fc:	e3a00000 	mov	r0, #0	; 0x0
   11800:	e5832920 	str	r2, [r3, #2336]
   11804:	e5832b40 	str	r2, [r3, #2880]
   11808:	e593281c 	ldr	r2, [r3, #2076]
   1180c:	e3822701 	orr	r2, r2, #262144	; 0x40000
   11810:	e3822002 	orr	r2, r2, #2	; 0x2
   11814:	e583281c 	str	r2, [r3, #2076]
   11818:	e28dd004 	add	sp, sp, #4	; 0x4
   1181c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   11820:	3c00a138 	.word	0x3c00a138
   11824:	21e38000 	.word	0x21e38000
   11828:	21e38400 	.word	0x21e38400
   1182c:	27100000 	.word	0x27100000
   11830:	40001408 	.word	0x40001408
   11834:	000f4240 	.word	0x000f4240
   11838:	3c00a5cc 	.word	0x3c00a5cc
   1183c:	00019581 	.word	0x00019581
   11840:	12345678 	.word	0x12345678
   11844:	00019592 	.word	0x00019592
   11848:	000195bb 	.word	0x000195bb
   1184c:	000195e7 	.word	0x000195e7
   11850:	000c3c00 	.word	0x000c3c00
   11854:	18088200 	.word	0x18088200

00011858 <module_get_clock>:
   11858:	e59f3124 	ldr	r3, [pc, #292]	; 11984 <module_get_clock+0x12c>
   1185c:	e1500003 	cmp	r0, r3
   11860:	0a00003b 	beq	11954 <module_get_clock+0xfc>
   11864:	8a000016 	bhi	118c4 <module_get_clock+0x6c>
   11868:	e2433050 	sub	r3, r3, #80	; 0x50
   1186c:	e1500003 	cmp	r0, r3
   11870:	0a000037 	beq	11954 <module_get_clock+0xfc>
   11874:	8a000007 	bhi	11898 <module_get_clock+0x40>
   11878:	e3500001 	cmp	r0, #1	; 0x1
   1187c:	0a00003e 	beq	1197c <module_get_clock+0x124>
   11880:	359f0100 	ldrcc	r0, [pc, #256]	; 11988 <module_get_clock+0x130>
   11884:	312fff1e 	bxcc	lr
   11888:	e3500003 	cmp	r0, #3	; 0x3
   1188c:	059f00f8 	ldreq	r0, [pc, #248]	; 1198c <module_get_clock+0x134>
   11890:	1a000004 	bne	118a8 <module_get_clock+0x50>
   11894:	e12fff1e 	bx	lr
   11898:	e59f30f0 	ldr	r3, [pc, #240]	; 11990 <module_get_clock+0x138>
   1189c:	e1500003 	cmp	r0, r3
   118a0:	0a00002b 	beq	11954 <module_get_clock+0xfc>
   118a4:	8a000001 	bhi	118b0 <module_get_clock+0x58>
   118a8:	e2433010 	sub	r3, r3, #16	; 0x10
   118ac:	ea000026 	b	1194c <module_get_clock+0xf4>
   118b0:	e59f30dc 	ldr	r3, [pc, #220]	; 11994 <module_get_clock+0x13c>
   118b4:	e1500003 	cmp	r0, r3
   118b8:	12833010 	addne	r3, r3, #16	; 0x10
   118bc:	1a000022 	bne	1194c <module_get_clock+0xf4>
   118c0:	ea000023 	b	11954 <module_get_clock+0xfc>
   118c4:	e59f30cc 	ldr	r3, [pc, #204]	; 11998 <module_get_clock+0x140>
   118c8:	e1500003 	cmp	r0, r3
   118cc:	059f00c8 	ldreq	r0, [pc, #200]	; 1199c <module_get_clock+0x144>
   118d0:	012fff1e 	bxeq	lr
   118d4:	8a00000e 	bhi	11914 <module_get_clock+0xbc>
   118d8:	e2433040 	sub	r3, r3, #64	; 0x40
   118dc:	e1500003 	cmp	r0, r3
   118e0:	0a00001f 	beq	11964 <module_get_clock+0x10c>
   118e4:	8a000003 	bhi	118f8 <module_get_clock+0xa0>
   118e8:	e2433010 	sub	r3, r3, #16	; 0x10
   118ec:	e1500003 	cmp	r0, r3
   118f0:	1a000019 	bne	1195c <module_get_clock+0x104>
   118f4:	ea00001a 	b	11964 <module_get_clock+0x10c>
   118f8:	e59f30a0 	ldr	r3, [pc, #160]	; 119a0 <module_get_clock+0x148>
   118fc:	e1500003 	cmp	r0, r3
   11900:	0a000017 	beq	11964 <module_get_clock+0x10c>
   11904:	e2833010 	add	r3, r3, #16	; 0x10
   11908:	e1500003 	cmp	r0, r3
   1190c:	1a000012 	bne	1195c <module_get_clock+0x104>
   11910:	ea000017 	b	11974 <module_get_clock+0x11c>
   11914:	e59f3088 	ldr	r3, [pc, #136]	; 119a4 <module_get_clock+0x14c>
   11918:	e1500003 	cmp	r0, r3
   1191c:	059f0084 	ldreq	r0, [pc, #132]	; 119a8 <module_get_clock+0x150>
   11920:	012fff1e 	bxeq	lr
   11924:	8a000003 	bhi	11938 <module_get_clock+0xe0>
   11928:	e2433010 	sub	r3, r3, #16	; 0x10
   1192c:	e1500003 	cmp	r0, r3
   11930:	1a000009 	bne	1195c <module_get_clock+0x104>
   11934:	ea00000c 	b	1196c <module_get_clock+0x114>
   11938:	e59f306c 	ldr	r3, [pc, #108]	; 119ac <module_get_clock+0x154>
   1193c:	e1500003 	cmp	r0, r3
   11940:	059f0068 	ldreq	r0, [pc, #104]	; 119b0 <module_get_clock+0x158>
   11944:	012fff1e 	bxeq	lr
   11948:	e2833030 	add	r3, r3, #48	; 0x30
   1194c:	e1500003 	cmp	r0, r3
   11950:	1a000001 	bne	1195c <module_get_clock+0x104>
   11954:	e59f0058 	ldr	r0, [pc, #88]	; 119b4 <module_get_clock+0x15c>
   11958:	e12fff1e 	bx	lr
   1195c:	e3a00000 	mov	r0, #0	; 0x0
   11960:	e12fff1e 	bx	lr
   11964:	e59f004c 	ldr	r0, [pc, #76]	; 119b8 <module_get_clock+0x160>
   11968:	e12fff1e 	bx	lr
   1196c:	e59f0048 	ldr	r0, [pc, #72]	; 119bc <module_get_clock+0x164>
   11970:	e12fff1e 	bx	lr
   11974:	e59f0044 	ldr	r0, [pc, #68]	; 119c0 <module_get_clock+0x168>
   11978:	e12fff1e 	bx	lr
   1197c:	e59f0040 	ldr	r0, [pc, #64]	; 119c4 <module_get_clock+0x16c>
   11980:	e12fff1e 	bx	lr
   11984:	21e0a190 	.word	0x21e0a190
   11988:	07fcad80 	.word	0x07fcad80
   1198c:	05fd8220 	.word	0x05fd8220
   11990:	21e0a160 	.word	0x21e0a160
   11994:	21e0a170 	.word	0x21e0a170
   11998:	21e0a290 	.word	0x21e0a290
   1199c:	0160203b 	.word	0x0160203b
   119a0:	21e0a260 	.word	0x21e0a260
   119a4:	21e0a2d0 	.word	0x21e0a2d0
   119a8:	02faf080 	.word	0x02faf080
   119ac:	21e0a2f0 	.word	0x21e0a2f0
   119b0:	05f5e100 	.word	0x05f5e100
   119b4:	0bfb0440 	.word	0x0bfb0440
   119b8:	02625a00 	.word	0x02625a00
   119bc:	016e3600 	.word	0x016e3600
   119c0:	04c4b400 	.word	0x04c4b400
   119c4:	0ff95b00 	.word	0x0ff95b00

000119c8 <set_mux>:
   119c8:	e59f3004 	ldr	r3, [pc, #4]	; 119d4 <set_mux+0xc>
   119cc:	e5830000 	str	r0, [r3]
   119d0:	e12fff1e 	bx	lr
   119d4:	21e09000 	.word	0x21e09000

000119d8 <pp_list>:
   119d8:	00000994 	.word	0x00000994
   119dc:	00003771 	.word	0x00003771
   119e0:	00003739 	.word	0x00003739
   119e4:	00003cb9 	.word	0x00003cb9
   119e8:	000029a0 	.word	0x000029a0
   119ec:	00003f91 	.word	0x00003f91
   119f0:	000031e1 	.word	0x000031e1
   119f4:	00002e6b 	.word	0x00002e6b
   119f8:	0000275d 	.word	0x0000275d
   119fc:	00002297 	.word	0x00002297
   11a00:	00002378 	.word	0x00002378
   11a04:	000034db 	.word	0x000034db
   11a08:	000029fc 	.word	0x000029fc
   11a0c:	0000271c 	.word	0x0000271c
   11a10:	000036ec 	.word	0x000036ec
   11a14:	00003ab9 	.word	0x00003ab9
   11a18:	00003218 	.word	0x00003218
   11a1c:	0000307f 	.word	0x0000307f
   11a20:	0000267b 	.word	0x0000267b
   11a24:	00003d95 	.word	0x00003d95
   11a28:	00003e0f 	.word	0x00003e0f
   11a2c:	00002d75 	.word	0x00002d75
   11a30:	000031a3 	.word	0x000031a3

00011a34 <print_size>:
   11a34:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
   11a38:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   11a3c:	e1a05000 	mov	r5, r0
   11a40:	e1a08001 	mov	r8, r1
   11a44:	83a04101 	movhi	r4, #1073741824	; 0x40000000
   11a48:	83a07047 	movhi	r7, #71	; 0x47
   11a4c:	8a000005 	bhi	11a68 <print_size+0x34>
   11a50:	e59f30a8 	ldr	r3, [pc, #168]	; 11b00 <print_size+0xcc>
   11a54:	e1500003 	cmp	r0, r3
   11a58:	83a0704d 	movhi	r7, #77	; 0x4d
   11a5c:	93a0706b 	movls	r7, #107	; 0x6b
   11a60:	83a04601 	movhi	r4, #1048576	; 0x100000
   11a64:	93a04b01 	movls	r4, #1024	; 0x400
   11a68:	e1a01004 	mov	r1, r4
   11a6c:	e1a00005 	mov	r0, r5
   11a70:	eb000cc3 	bl	14d84 <__aeabi_uidiv>
   11a74:	e1a01004 	mov	r1, r4
   11a78:	e1a06000 	mov	r6, r0
   11a7c:	e1a00005 	mov	r0, r5
   11a80:	eb000d3b 	bl	14f74 <__aeabi_uidivmod>
   11a84:	e2510000 	subs	r0, r1, #0	; 0x0
   11a88:	01a04000 	moveq	r4, r0
   11a8c:	0a000009 	beq	11ab8 <print_size+0x84>
   11a90:	e0625694 	mls	r2, r4, r6, r5
   11a94:	e1a030a4 	lsr	r3, r4, #1
   11a98:	e3a0000a 	mov	r0, #10	; 0xa
   11a9c:	e1a01004 	mov	r1, r4
   11aa0:	e0203290 	mla	r0, r0, r2, r3
   11aa4:	eb000cb6 	bl	14d84 <__aeabi_uidiv>
   11aa8:	e3500009 	cmp	r0, #9	; 0x9
   11aac:	e1a04000 	mov	r4, r0
   11ab0:	82866001 	addhi	r6, r6, #1	; 0x1
   11ab4:	8240400a 	subhi	r4, r0, #10	; 0xa
   11ab8:	e1a01006 	mov	r1, r6
   11abc:	e59f5040 	ldr	r5, [pc, #64]	; 11b04 <print_size+0xd0>
   11ac0:	e59f0040 	ldr	r0, [pc, #64]	; 11b08 <print_size+0xd4>
   11ac4:	e1a0e00f 	mov	lr, pc
   11ac8:	e595f000 	ldr	pc, [r5]
   11acc:	e3540000 	cmp	r4, #0	; 0x0
   11ad0:	0a000003 	beq	11ae4 <print_size+0xb0>
   11ad4:	e1a01004 	mov	r1, r4
   11ad8:	e59f002c 	ldr	r0, [pc, #44]	; 11b0c <print_size+0xd8>
   11adc:	e1a0e00f 	mov	lr, pc
   11ae0:	e595f000 	ldr	pc, [r5]
   11ae4:	e1a01007 	mov	r1, r7
   11ae8:	e1a02008 	mov	r2, r8
   11aec:	e59f001c 	ldr	r0, [pc, #28]	; 11b10 <print_size+0xdc>
   11af0:	e59f300c 	ldr	r3, [pc, #12]	; 11b04 <print_size+0xd0>
   11af4:	e1a0e00f 	mov	lr, pc
   11af8:	e593f000 	ldr	pc, [r3]
   11afc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   11b00:	000fffff 	.word	0x000fffff
   11b04:	3c00a5cc 	.word	0x3c00a5cc
   11b08:	00019606 	.word	0x00019606
   11b0c:	0001960b 	.word	0x0001960b
   11b10:	00019610 	.word	0x00019610

00011b14 <print_buffer>:
   11b14:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11b18:	e24dd044 	sub	sp, sp, #68	; 0x44
   11b1c:	e59d6068 	ldr	r6, [sp, #104]
   11b20:	e1a07002 	mov	r7, r2
   11b24:	e0020692 	mul	r2, r2, r6
   11b28:	e3520040 	cmp	r2, #64	; 0x40
   11b2c:	e1a09000 	mov	r9, r0
   11b30:	e1a0a001 	mov	sl, r1
   11b34:	e1a08003 	mov	r8, r3
   11b38:	9a000003 	bls	11b4c <print_buffer+0x38>
   11b3c:	e3a00040 	mov	r0, #64	; 0x40
   11b40:	e1a01007 	mov	r1, r7
   11b44:	eb000c8e 	bl	14d84 <__aeabi_uidiv>
   11b48:	e1a06000 	mov	r6, r0
   11b4c:	e3560000 	cmp	r6, #0	; 0x0
   11b50:	1a00003c 	bne	11c48 <print_buffer+0x134>
   11b54:	e3a00010 	mov	r0, #16	; 0x10
   11b58:	e1a01007 	mov	r1, r7
   11b5c:	eb000c88 	bl	14d84 <__aeabi_uidiv>
   11b60:	e1a06000 	mov	r6, r0
   11b64:	ea000037 	b	11c48 <print_buffer+0x134>
   11b68:	e59f00f0 	ldr	r0, [pc, #240]	; 11c60 <print_buffer+0x14c>
   11b6c:	e1a01009 	mov	r1, r9
   11b70:	e1a0e00f 	mov	lr, pc
   11b74:	e59bf000 	ldr	pc, [fp]
   11b78:	e1560008 	cmp	r6, r8
   11b7c:	21a06008 	movcs	r6, r8
   11b80:	e1a0500a 	mov	r5, sl
   11b84:	e3a04000 	mov	r4, #0	; 0x0
   11b88:	ea00000f 	b	11bcc <print_buffer+0xb8>
   11b8c:	e3570004 	cmp	r7, #4	; 0x4
   11b90:	05951000 	ldreq	r1, [r5]
   11b94:	059f00c8 	ldreq	r0, [pc, #200]	; 11c64 <print_buffer+0x150>
   11b98:	078d1104 	streq	r1, [sp, r4, lsl #2]
   11b9c:	0a000006 	beq	11bbc <print_buffer+0xa8>
   11ba0:	e3570002 	cmp	r7, #2	; 0x2
   11ba4:	01d510b0 	ldrheq	r1, [r5]
   11ba8:	15d51000 	ldrbne	r1, [r5]
   11bac:	01a03084 	lsleq	r3, r4, #1
   11bb0:	059f00b0 	ldreq	r0, [pc, #176]	; 11c68 <print_buffer+0x154>
   11bb4:	018d10b3 	strheq	r1, [sp, r3]
   11bb8:	17cd1004 	strbne	r1, [sp, r4]
   11bbc:	e1a0e00f 	mov	lr, pc
   11bc0:	e59bf000 	ldr	pc, [fp]
   11bc4:	e0855007 	add	r5, r5, r7
   11bc8:	e2844001 	add	r4, r4, #1	; 0x1
   11bcc:	e1540006 	cmp	r4, r6
   11bd0:	e59f0094 	ldr	r0, [pc, #148]	; 11c6c <print_buffer+0x158>
   11bd4:	3affffec 	bcc	11b8c <print_buffer+0x78>
   11bd8:	e0050697 	mul	r5, r7, r6
   11bdc:	e59f008c 	ldr	r0, [pc, #140]	; 11c70 <print_buffer+0x15c>
   11be0:	e08aa005 	add	sl, sl, r5
   11be4:	eb000450 	bl	12d2c <puts>
   11be8:	e3a04000 	mov	r4, #0	; 0x0
   11bec:	ea000009 	b	11c18 <print_buffer+0x104>
   11bf0:	e7dd0004 	ldrb	r0, [sp, r4]
   11bf4:	e59f2078 	ldr	r2, [pc, #120]	; 11c74 <print_buffer+0x160>
   11bf8:	e2844001 	add	r4, r4, #1	; 0x1
   11bfc:	e7d23000 	ldrb	r3, [r2, r0]
   11c00:	e3130097 	tst	r3, #151	; 0x97
   11c04:	0a000001 	beq	11c10 <print_buffer+0xfc>
   11c08:	e3100080 	tst	r0, #128	; 0x80
   11c0c:	0a000000 	beq	11c14 <print_buffer+0x100>
   11c10:	e3a0002e 	mov	r0, #46	; 0x2e
   11c14:	eb00046c 	bl	12dcc <putc>
   11c18:	e1540005 	cmp	r4, r5
   11c1c:	3afffff3 	bcc	11bf0 <print_buffer+0xdc>
   11c20:	e59f0050 	ldr	r0, [pc, #80]	; 11c78 <print_buffer+0x164>
   11c24:	e1a0e00f 	mov	lr, pc
   11c28:	e59bf000 	ldr	pc, [fp]
   11c2c:	eb000469 	bl	12dd8 <ctrlc>
   11c30:	e3500000 	cmp	r0, #0	; 0x0
   11c34:	13e00000 	mvnne	r0, #0	; 0x0
   11c38:	1a000006 	bne	11c58 <print_buffer+0x144>
   11c3c:	e0899005 	add	r9, r9, r5
   11c40:	e0668008 	rsb	r8, r6, r8
   11c44:	ea000000 	b	11c4c <print_buffer+0x138>
   11c48:	e59fb02c 	ldr	fp, [pc, #44]	; 11c7c <print_buffer+0x168>
   11c4c:	e3580000 	cmp	r8, #0	; 0x0
   11c50:	1affffc4 	bne	11b68 <print_buffer+0x54>
   11c54:	e1a00008 	mov	r0, r8
   11c58:	e28dd044 	add	sp, sp, #68	; 0x44
   11c5c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11c60:	00019617 	.word	0x00019617
   11c64:	00018a20 	.word	0x00018a20
   11c68:	0001961e 	.word	0x0001961e
   11c6c:	00019624 	.word	0x00019624
   11c70:	0001962a 	.word	0x0001962a
   11c74:	000165d1 	.word	0x000165d1
   11c78:	000171fb 	.word	0x000171fb
   11c7c:	3c00a5cc 	.word	0x3c00a5cc

00011c80 <__div64_32>:
   11c80:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   11c84:	e8900300 	ldm	r0, {r8, r9}
   11c88:	e1510009 	cmp	r1, r9
   11c8c:	e1a0a000 	mov	sl, r0
   11c90:	e1a04001 	mov	r4, r1
   11c94:	83a06000 	movhi	r6, #0	; 0x0
   11c98:	83a07000 	movhi	r7, #0	; 0x0
   11c9c:	8a000007 	bhi	11cc0 <__div64_32+0x40>
   11ca0:	e1a00009 	mov	r0, r9
   11ca4:	eb000c36 	bl	14d84 <__aeabi_uidiv>
   11ca8:	e0030094 	mul	r3, r4, r0
   11cac:	e3a06000 	mov	r6, #0	; 0x0
   11cb0:	e1a02006 	mov	r2, r6
   11cb4:	e1a07000 	mov	r7, r0
   11cb8:	e0588002 	subs	r8, r8, r2
   11cbc:	e0c99003 	sbc	r9, r9, r3
   11cc0:	e1a00004 	mov	r0, r4
   11cc4:	e3a01000 	mov	r1, #0	; 0x0
   11cc8:	e3a05000 	mov	r5, #0	; 0x0
   11ccc:	e3a04001 	mov	r4, #1	; 0x1
   11cd0:	ea000003 	b	11ce4 <__div64_32+0x64>
   11cd4:	e0900000 	adds	r0, r0, r0
   11cd8:	e0a11001 	adc	r1, r1, r1
   11cdc:	e0944004 	adds	r4, r4, r4
   11ce0:	e0a55005 	adc	r5, r5, r5
   11ce4:	e3510000 	cmp	r1, #0	; 0x0
   11ce8:	e3a03000 	mov	r3, #0	; 0x0
   11cec:	ca000002 	bgt	11cfc <__div64_32+0x7c>
   11cf0:	1a000002 	bne	11d00 <__div64_32+0x80>
   11cf4:	e3500000 	cmp	r0, #0	; 0x0
   11cf8:	0a000000 	beq	11d00 <__div64_32+0x80>
   11cfc:	e3a03001 	mov	r3, #1	; 0x1
   11d00:	e1590001 	cmp	r9, r1
   11d04:	e3a02000 	mov	r2, #0	; 0x0
   11d08:	8a000002 	bhi	11d18 <__div64_32+0x98>
   11d0c:	1a000002 	bne	11d1c <__div64_32+0x9c>
   11d10:	e1580000 	cmp	r8, r0
   11d14:	9a000000 	bls	11d1c <__div64_32+0x9c>
   11d18:	e3a02001 	mov	r2, #1	; 0x1
   11d1c:	e0033002 	and	r3, r3, r2
   11d20:	e31300ff 	tst	r3, #255	; 0xff
   11d24:	1affffea 	bne	11cd4 <__div64_32+0x54>
   11d28:	e1510009 	cmp	r1, r9
   11d2c:	8a000006 	bhi	11d4c <__div64_32+0xcc>
   11d30:	1a000001 	bne	11d3c <__div64_32+0xbc>
   11d34:	e1500008 	cmp	r0, r8
   11d38:	8a000003 	bhi	11d4c <__div64_32+0xcc>
   11d3c:	e0588000 	subs	r8, r8, r0
   11d40:	e0c99001 	sbc	r9, r9, r1
   11d44:	e0966004 	adds	r6, r6, r4
   11d48:	e0a77005 	adc	r7, r7, r5
   11d4c:	e1b050a5 	lsrs	r5, r5, #1
   11d50:	e1a04064 	rrx	r4, r4
   11d54:	e1943005 	orrs	r3, r4, r5
   11d58:	0a000002 	beq	11d68 <__div64_32+0xe8>
   11d5c:	e1b010a1 	lsrs	r1, r1, #1
   11d60:	e1a00060 	rrx	r0, r0
   11d64:	eaffffef 	b	11d28 <__div64_32+0xa8>
   11d68:	e88a00c0 	stm	sl, {r6, r7}
   11d6c:	e1a00008 	mov	r0, r8
   11d70:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

00011d74 <simple_strtoul>:
   11d74:	e92d4070 	push	{r4, r5, r6, lr}
   11d78:	e5d03000 	ldrb	r3, [r0]
   11d7c:	e1a05001 	mov	r5, r1
   11d80:	e3530030 	cmp	r3, #48	; 0x30
   11d84:	e1a01002 	mov	r1, r2
   11d88:	1a00000d 	bne	11dc4 <simple_strtoul+0x50>
   11d8c:	e5f03001 	ldrb	r3, [r0, #1]!
   11d90:	e3530078 	cmp	r3, #120	; 0x78
   11d94:	1a000007 	bne	11db8 <simple_strtoul+0x44>
   11d98:	e5d03001 	ldrb	r3, [r0, #1]
   11d9c:	e59f2084 	ldr	r2, [pc, #132]	; 11e28 <simple_strtoul+0xb4>
   11da0:	e280c001 	add	ip, r0, #1	; 0x1
   11da4:	e7d23003 	ldrb	r3, [r2, r3]
   11da8:	e3130044 	tst	r3, #68	; 0x44
   11dac:	11a0000c 	movne	r0, ip
   11db0:	13a01010 	movne	r1, #16	; 0x10
   11db4:	1a000004 	bne	11dcc <simple_strtoul+0x58>
   11db8:	e3510000 	cmp	r1, #0	; 0x0
   11dbc:	03a01008 	moveq	r1, #8	; 0x8
   11dc0:	ea000001 	b	11dcc <simple_strtoul+0x58>
   11dc4:	e3520000 	cmp	r2, #0	; 0x0
   11dc8:	03a0100a 	moveq	r1, #10	; 0xa
   11dcc:	e3a04000 	mov	r4, #0	; 0x0
   11dd0:	e59f6050 	ldr	r6, [pc, #80]	; 11e28 <simple_strtoul+0xb4>
   11dd4:	ea000001 	b	11de0 <simple_strtoul+0x6c>
   11dd8:	e0243491 	mla	r4, r1, r4, r3
   11ddc:	e2800001 	add	r0, r0, #1	; 0x1
   11de0:	e5d0c000 	ldrb	ip, [r0]
   11de4:	e7d6200c 	ldrb	r2, [r6, ip]
   11de8:	e24c3030 	sub	r3, ip, #48	; 0x30
   11dec:	e3120044 	tst	r2, #68	; 0x44
   11df0:	0a000008 	beq	11e18 <simple_strtoul+0xa4>
   11df4:	e3120004 	tst	r2, #4	; 0x4
   11df8:	1a000004 	bne	11e10 <simple_strtoul+0x9c>
   11dfc:	e24c3020 	sub	r3, ip, #32	; 0x20
   11e00:	e3120002 	tst	r2, #2	; 0x2
   11e04:	e6ef3073 	uxtb	r3, r3
   11e08:	12433037 	subne	r3, r3, #55	; 0x37
   11e0c:	024c3037 	subeq	r3, ip, #55	; 0x37
   11e10:	e1530001 	cmp	r3, r1
   11e14:	3affffef 	bcc	11dd8 <simple_strtoul+0x64>
   11e18:	e3550000 	cmp	r5, #0	; 0x0
   11e1c:	15850000 	strne	r0, [r5]
   11e20:	e1a00004 	mov	r0, r4
   11e24:	e8bd8070 	pop	{r4, r5, r6, pc}
   11e28:	000165d1 	.word	0x000165d1

00011e2c <simple_strtol>:
   11e2c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   11e30:	e5d03000 	ldrb	r3, [r0]
   11e34:	e24dd004 	sub	sp, sp, #4	; 0x4
   11e38:	e353002d 	cmp	r3, #45	; 0x2d
   11e3c:	1a000004 	bne	11e54 <simple_strtol+0x28>
   11e40:	e2800001 	add	r0, r0, #1	; 0x1
   11e44:	ebffffca 	bl	11d74 <simple_strtoul>
   11e48:	e2600000 	rsb	r0, r0, #0	; 0x0
   11e4c:	e28dd004 	add	sp, sp, #4	; 0x4
   11e50:	e8bd8000 	pop	{pc}
   11e54:	e28dd004 	add	sp, sp, #4	; 0x4
   11e58:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   11e5c:	eaffffc4 	b	11d74 <simple_strtoul>

00011e60 <ustrtoul>:
   11e60:	e92d4010 	push	{r4, lr}
   11e64:	e1a04001 	mov	r4, r1
   11e68:	ebffffc1 	bl	11d74 <simple_strtoul>
   11e6c:	e5941000 	ldr	r1, [r4]
   11e70:	e5d13000 	ldrb	r3, [r1]
   11e74:	e353004b 	cmp	r3, #75	; 0x4b
   11e78:	0a00000a 	beq	11ea8 <ustrtoul+0x48>
   11e7c:	8a000002 	bhi	11e8c <ustrtoul+0x2c>
   11e80:	e3530047 	cmp	r3, #71	; 0x47
   11e84:	18bd8010 	popne	{r4, pc}
   11e88:	ea000004 	b	11ea0 <ustrtoul+0x40>
   11e8c:	e353004d 	cmp	r3, #77	; 0x4d
   11e90:	0a000003 	beq	11ea4 <ustrtoul+0x44>
   11e94:	e353006b 	cmp	r3, #107	; 0x6b
   11e98:	18bd8010 	popne	{r4, pc}
   11e9c:	ea000001 	b	11ea8 <ustrtoul+0x48>
   11ea0:	e1a00500 	lsl	r0, r0, #10
   11ea4:	e1a00500 	lsl	r0, r0, #10
   11ea8:	e5d13001 	ldrb	r3, [r1, #1]
   11eac:	e1a00500 	lsl	r0, r0, #10
   11eb0:	e3530069 	cmp	r3, #105	; 0x69
   11eb4:	18bd8010 	popne	{r4, pc}
   11eb8:	e5d13002 	ldrb	r3, [r1, #2]
   11ebc:	e2812002 	add	r2, r1, #2	; 0x2
   11ec0:	e3530042 	cmp	r3, #66	; 0x42
   11ec4:	02813003 	addeq	r3, r1, #3	; 0x3
   11ec8:	05843000 	streq	r3, [r4]
   11ecc:	15842000 	strne	r2, [r4]
   11ed0:	e8bd8010 	pop	{r4, pc}

00011ed4 <simple_strtoull>:
   11ed4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   11ed8:	e5d03000 	ldrb	r3, [r0]
   11edc:	e1a07000 	mov	r7, r0
   11ee0:	e3530030 	cmp	r3, #48	; 0x30
   11ee4:	e1a08001 	mov	r8, r1
   11ee8:	e1a06002 	mov	r6, r2
   11eec:	1a00000d 	bne	11f28 <simple_strtoull+0x54>
   11ef0:	e5f73001 	ldrb	r3, [r7, #1]!
   11ef4:	e3530078 	cmp	r3, #120	; 0x78
   11ef8:	1a000007 	bne	11f1c <simple_strtoull+0x48>
   11efc:	e5d73001 	ldrb	r3, [r7, #1]
   11f00:	e59f20ac 	ldr	r2, [pc, #172]	; 11fb4 <simple_strtoull+0xe0>
   11f04:	e2870001 	add	r0, r7, #1	; 0x1
   11f08:	e7d23003 	ldrb	r3, [r2, r3]
   11f0c:	e3130044 	tst	r3, #68	; 0x44
   11f10:	11a07000 	movne	r7, r0
   11f14:	13a06010 	movne	r6, #16	; 0x10
   11f18:	1a000004 	bne	11f30 <simple_strtoull+0x5c>
   11f1c:	e3560000 	cmp	r6, #0	; 0x0
   11f20:	03a06008 	moveq	r6, #8	; 0x8
   11f24:	ea000001 	b	11f30 <simple_strtoull+0x5c>
   11f28:	e3520000 	cmp	r2, #0	; 0x0
   11f2c:	03a0600a 	moveq	r6, #10	; 0xa
   11f30:	e3a04000 	mov	r4, #0	; 0x0
   11f34:	e3a05000 	mov	r5, #0	; 0x0
   11f38:	e59f9074 	ldr	r9, [pc, #116]	; 11fb4 <simple_strtoull+0xe0>
   11f3c:	e3a0a000 	mov	sl, #0	; 0x0
   11f40:	ea000004 	b	11f58 <simple_strtoull+0x84>
   11f44:	e0832694 	umull	r2, r3, r4, r6
   11f48:	e0233596 	mla	r3, r6, r5, r3
   11f4c:	e0904002 	adds	r4, r0, r2
   11f50:	e0a15003 	adc	r5, r1, r3
   11f54:	e2877001 	add	r7, r7, #1	; 0x1
   11f58:	e5d7c000 	ldrb	ip, [r7]
   11f5c:	e7d9200c 	ldrb	r2, [r9, ip]
   11f60:	e24c3030 	sub	r3, ip, #48	; 0x30
   11f64:	e3120044 	tst	r2, #68	; 0x44
   11f68:	0a00000c 	beq	11fa0 <simple_strtoull+0xcc>
   11f6c:	e3120004 	tst	r2, #4	; 0x4
   11f70:	1a000004 	bne	11f88 <simple_strtoull+0xb4>
   11f74:	e24c3020 	sub	r3, ip, #32	; 0x20
   11f78:	e3120002 	tst	r2, #2	; 0x2
   11f7c:	e6ef3073 	uxtb	r3, r3
   11f80:	12433037 	subne	r3, r3, #55	; 0x37
   11f84:	024c3037 	subeq	r3, ip, #55	; 0x37
   11f88:	e1a00003 	mov	r0, r3
   11f8c:	e1a01fc0 	asr	r1, r0, #31
   11f90:	e15a0001 	cmp	sl, r1
   11f94:	1a000001 	bne	11fa0 <simple_strtoull+0xcc>
   11f98:	e1560000 	cmp	r6, r0
   11f9c:	8affffe8 	bhi	11f44 <simple_strtoull+0x70>
   11fa0:	e3580000 	cmp	r8, #0	; 0x0
   11fa4:	15887000 	strne	r7, [r8]
   11fa8:	e1a01005 	mov	r1, r5
   11fac:	e1a00004 	mov	r0, r4
   11fb0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
   11fb4:	000165d1 	.word	0x000165d1

00011fb8 <put_dec_trunc>:
   11fb8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   11fbc:	e1a03421 	lsr	r3, r1, #8
   11fc0:	e1a04221 	lsr	r4, r1, #4
   11fc4:	e204400f 	and	r4, r4, #15	; 0xf
   11fc8:	e203600f 	and	r6, r3, #15	; 0xf
   11fcc:	e1a05621 	lsr	r5, r1, #12
   11fd0:	e0842006 	add	r2, r4, r6
   11fd4:	e201100f 	and	r1, r1, #15	; 0xf
   11fd8:	e3a03006 	mov	r3, #6	; 0x6
   11fdc:	e0822005 	add	r2, r2, r5
   11fe0:	e0221293 	mla	r2, r3, r2, r1
   11fe4:	e3a080cd 	mov	r8, #205	; 0xcd
   11fe8:	e00c0298 	mul	ip, r8, r2
   11fec:	e3a01009 	mov	r1, #9	; 0x9
   11ff0:	e0214591 	mla	r1, r1, r5, r4
   11ff4:	e1a0c5ac 	lsr	ip, ip, #11
   11ff8:	e3a0700a 	mov	r7, #10	; 0xa
   11ffc:	e3a03005 	mov	r3, #5	; 0x5
   12000:	e0231693 	mla	r3, r3, r6, r1
   12004:	e0622c97 	mls	r2, r7, ip, r2
   12008:	e093100c 	adds	r1, r3, ip
   1200c:	e2822030 	add	r2, r2, #48	; 0x30
   12010:	e1a04000 	mov	r4, r0
   12014:	e4c02001 	strb	r2, [r0], #1
   12018:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   1201c:	e0030198 	mul	r3, r8, r1
   12020:	e1a035a3 	lsr	r3, r3, #11
   12024:	e0621397 	mls	r2, r7, r3, r1
   12028:	e0831086 	add	r1, r3, r6, lsl #1
   1202c:	e2822030 	add	r2, r2, #48	; 0x30
   12030:	e1913005 	orrs	r3, r1, r5
   12034:	e5c42001 	strb	r2, [r4, #1]
   12038:	e2800001 	add	r0, r0, #1	; 0x1
   1203c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   12040:	e3a0300d 	mov	r3, #13	; 0xd
   12044:	e0030193 	mul	r3, r3, r1
   12048:	e1a033a3 	lsr	r3, r3, #7
   1204c:	e0621397 	mls	r2, r7, r3, r1
   12050:	e0931105 	adds	r1, r3, r5, lsl #2
   12054:	e2822030 	add	r2, r2, #48	; 0x30
   12058:	e4c02001 	strb	r2, [r0], #1
   1205c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   12060:	e0030198 	mul	r3, r8, r1
   12064:	e1a0c5a3 	lsr	ip, r3, #11
   12068:	e0631c97 	mls	r3, r7, ip, r1
   1206c:	e35c0000 	cmp	ip, #0	; 0x0
   12070:	e2833030 	add	r3, r3, #48	; 0x30
   12074:	e4c03001 	strb	r3, [r0], #1
   12078:	128c3030 	addne	r3, ip, #48	; 0x30
   1207c:	14c03001 	strbne	r3, [r0], #1
   12080:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00012084 <string>:
   12084:	e59fc0a4 	ldr	ip, [pc, #164]	; 12130 <string+0xac>
   12088:	e3510000 	cmp	r1, #0	; 0x0
   1208c:	e92d4070 	push	{r4, r5, r6, lr}
   12090:	11a06001 	movne	r6, r1
   12094:	01a0600c 	moveq	r6, ip
   12098:	e1a01003 	mov	r1, r3
   1209c:	e1a04000 	mov	r4, r0
   120a0:	e1a00006 	mov	r0, r6
   120a4:	e1a05002 	mov	r5, r2
   120a8:	ebffbcdb 	bl	141c <strnlen>
   120ac:	e59d3010 	ldr	r3, [sp, #16]
   120b0:	e1a0c000 	mov	ip, r0
   120b4:	e3130010 	tst	r3, #16	; 0x10
   120b8:	01a02004 	moveq	r2, r4
   120bc:	03a01020 	moveq	r1, #32	; 0x20
   120c0:	0a000001 	beq	120cc <string+0x48>
   120c4:	ea000006 	b	120e4 <string+0x60>
   120c8:	e5421001 	strb	r1, [r2, #-1]
   120cc:	e2455001 	sub	r5, r5, #1	; 0x1
   120d0:	e2853001 	add	r3, r5, #1	; 0x1
   120d4:	e15c0003 	cmp	ip, r3
   120d8:	e1a04002 	mov	r4, r2
   120dc:	e2822001 	add	r2, r2, #1	; 0x1
   120e0:	bafffff8 	blt	120c8 <string+0x44>
   120e4:	e3a02000 	mov	r2, #0	; 0x0
   120e8:	ea000002 	b	120f8 <string+0x74>
   120ec:	e7d63002 	ldrb	r3, [r6, r2]
   120f0:	e7c43002 	strb	r3, [r4, r2]
   120f4:	e2822001 	add	r2, r2, #1	; 0x1
   120f8:	e152000c 	cmp	r2, ip
   120fc:	e0843002 	add	r3, r4, r2
   12100:	bafffff9 	blt	120ec <string+0x68>
   12104:	e1a01003 	mov	r1, r3
   12108:	e1a02005 	mov	r2, r5
   1210c:	e3a03020 	mov	r3, #32	; 0x20
   12110:	ea000000 	b	12118 <string+0x94>
   12114:	e5413001 	strb	r3, [r1, #-1]
   12118:	e15c0002 	cmp	ip, r2
   1211c:	e1a00001 	mov	r0, r1
   12120:	e2422001 	sub	r2, r2, #1	; 0x1
   12124:	e2811001 	add	r1, r1, #1	; 0x1
   12128:	bafffff9 	blt	12114 <string+0x90>
   1212c:	e8bd8070 	pop	{r4, r5, r6, pc}
   12130:	0001962f 	.word	0x0001962f

00012134 <put_dec>:
   12134:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   12138:	e24dd00c 	sub	sp, sp, #12	; 0xc
   1213c:	e28d8008 	add	r8, sp, #8	; 0x8
   12140:	e16820f8 	strd	r2, [r8, #-8]!
   12144:	e1a07000 	mov	r7, r0
   12148:	e3a090cd 	mov	r9, #205	; 0xcd
   1214c:	e3a0a00a 	mov	sl, #10	; 0xa
   12150:	e3a0b00d 	mov	fp, #13	; 0xd
   12154:	e59d5004 	ldr	r5, [sp, #4]
   12158:	e1a0000d 	mov	r0, sp
   1215c:	e3550000 	cmp	r5, #0	; 0x0
   12160:	e59f10f0 	ldr	r1, [pc, #240]	; 12258 <put_dec+0x124>
   12164:	e59d4000 	ldr	r4, [sp]
   12168:	1a00000f 	bne	121ac <put_dec+0x78>
   1216c:	e59f30e8 	ldr	r3, [pc, #232]	; 1225c <put_dec+0x128>
   12170:	e1a00004 	mov	r0, r4
   12174:	e1540003 	cmp	r4, r3
   12178:	8a000004 	bhi	12190 <put_dec+0x5c>
   1217c:	e1a00007 	mov	r0, r7
   12180:	e1a01004 	mov	r1, r4
   12184:	ebffff8b 	bl	11fb8 <put_dec_trunc>
   12188:	e28dd00c 	add	sp, sp, #12	; 0xc
   1218c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   12190:	eb000b77 	bl	14f74 <__aeabi_uidivmod>
   12194:	e1a00004 	mov	r0, r4
   12198:	e1a06001 	mov	r6, r1
   1219c:	e59f10b4 	ldr	r1, [pc, #180]	; 12258 <put_dec+0x124>
   121a0:	eb000af7 	bl	14d84 <__aeabi_uidiv>
   121a4:	e88d0021 	stm	sp, {r0, r5}
   121a8:	ea000001 	b	121b4 <put_dec+0x80>
   121ac:	ebfffeb3 	bl	11c80 <__div64_32>
   121b0:	e1a06000 	mov	r6, r0
   121b4:	e1a02226 	lsr	r2, r6, #4
   121b8:	e1a0c426 	lsr	ip, r6, #8
   121bc:	e20cc00f 	and	ip, ip, #15	; 0xf
   121c0:	e202200f 	and	r2, r2, #15	; 0xf
   121c4:	e1a0e626 	lsr	lr, r6, #12
   121c8:	e082500c 	add	r5, r2, ip
   121cc:	e085500e 	add	r5, r5, lr
   121d0:	e206300f 	and	r3, r6, #15	; 0xf
   121d4:	e3a01006 	mov	r1, #6	; 0x6
   121d8:	e0253591 	mla	r5, r1, r5, r3
   121dc:	e3a03009 	mov	r3, #9	; 0x9
   121e0:	e0222e93 	mla	r2, r3, lr, r2
   121e4:	e3a01005 	mov	r1, #5	; 0x5
   121e8:	e0000599 	mul	r0, r9, r5
   121ec:	e0222c91 	mla	r2, r1, ip, r2
   121f0:	e1a005a0 	lsr	r0, r0, #11
   121f4:	e0822000 	add	r2, r2, r0
   121f8:	e0010299 	mul	r1, r9, r2
   121fc:	e1a015a1 	lsr	r1, r1, #11
   12200:	e081c08c 	add	ip, r1, ip, lsl #1
   12204:	e0030c9b 	mul	r3, fp, ip
   12208:	e1a033a3 	lsr	r3, r3, #7
   1220c:	e083e10e 	add	lr, r3, lr, lsl #2
   12210:	e0040e99 	mul	r4, r9, lr
   12214:	e1a045a4 	lsr	r4, r4, #11
   12218:	e06ee49a 	mls	lr, sl, r4, lr
   1221c:	e060509a 	mls	r0, sl, r0, r5
   12220:	e061219a 	mls	r1, sl, r1, r2
   12224:	e063c39a 	mls	r3, sl, r3, ip
   12228:	e2800030 	add	r0, r0, #48	; 0x30
   1222c:	e2811030 	add	r1, r1, #48	; 0x30
   12230:	e2833030 	add	r3, r3, #48	; 0x30
   12234:	e28ee030 	add	lr, lr, #48	; 0x30
   12238:	e2844030 	add	r4, r4, #48	; 0x30
   1223c:	e5c70000 	strb	r0, [r7]
   12240:	e5c71001 	strb	r1, [r7, #1]
   12244:	e5c73002 	strb	r3, [r7, #2]
   12248:	e5c7e003 	strb	lr, [r7, #3]
   1224c:	e5c74004 	strb	r4, [r7, #4]
   12250:	e2877005 	add	r7, r7, #5	; 0x5
   12254:	eaffffbe 	b	12154 <put_dec+0x20>
   12258:	000186a0 	.word	0x000186a0
   1225c:	0001869f 	.word	0x0001869f

00012260 <number>:
   12260:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   12264:	e24dd05c 	sub	sp, sp, #92	; 0x5c
   12268:	e1a0a000 	mov	sl, r0
   1226c:	e59d0080 	ldr	r0, [sp, #128]
   12270:	e59d808c 	ldr	r8, [sp, #140]
   12274:	e250100a 	subs	r1, r0, #10	; 0xa
   12278:	13a01001 	movne	r1, #1	; 0x1
   1227c:	e0011328 	and	r1, r1, r8, lsr #6
   12280:	e3180010 	tst	r8, #16	; 0x10
   12284:	e58d1008 	str	r1, [sp, #8]
   12288:	e2081020 	and	r1, r8, #32	; 0x20
   1228c:	13c88001 	bicne	r8, r8, #1	; 0x1
   12290:	e3180002 	tst	r8, #2	; 0x2
   12294:	e1a06002 	mov	r6, r2
   12298:	e1a07003 	mov	r7, r3
   1229c:	e59d5084 	ldr	r5, [sp, #132]
   122a0:	e58d1004 	str	r1, [sp, #4]
   122a4:	0a000010 	beq	122ec <number+0x8c>
   122a8:	e3530000 	cmp	r3, #0	; 0x0
   122ac:	ba000002 	blt	122bc <number+0x5c>
   122b0:	e3180004 	tst	r8, #4	; 0x4
   122b4:	0a000008 	beq	122dc <number+0x7c>
   122b8:	ea000004 	b	122d0 <number+0x70>
   122bc:	e2766000 	rsbs	r6, r6, #0	; 0x0
   122c0:	e2e77000 	rsc	r7, r7, #0	; 0x0
   122c4:	e2455001 	sub	r5, r5, #1	; 0x1
   122c8:	e3a0902d 	mov	r9, #45	; 0x2d
   122cc:	ea000007 	b	122f0 <number+0x90>
   122d0:	e2455001 	sub	r5, r5, #1	; 0x1
   122d4:	e3a0902b 	mov	r9, #43	; 0x2b
   122d8:	ea000004 	b	122f0 <number+0x90>
   122dc:	e3180008 	tst	r8, #8	; 0x8
   122e0:	12455001 	subne	r5, r5, #1	; 0x1
   122e4:	13a09020 	movne	r9, #32	; 0x20
   122e8:	1a000000 	bne	122f0 <number+0x90>
   122ec:	e3a09000 	mov	r9, #0	; 0x0
   122f0:	e59d2008 	ldr	r2, [sp, #8]
   122f4:	e3520000 	cmp	r2, #0	; 0x0
   122f8:	0a000003 	beq	1230c <number+0xac>
   122fc:	e59d3080 	ldr	r3, [sp, #128]
   12300:	e2455001 	sub	r5, r5, #1	; 0x1
   12304:	e3530010 	cmp	r3, #16	; 0x10
   12308:	02455001 	subeq	r5, r5, #1	; 0x1
   1230c:	e196c007 	orrs	ip, r6, r7
   12310:	03a03030 	moveq	r3, #48	; 0x30
   12314:	05cd3016 	strbeq	r3, [sp, #22]
   12318:	03a04001 	moveq	r4, #1	; 0x1
   1231c:	0a000020 	beq	123a4 <number+0x144>
   12320:	e59d0080 	ldr	r0, [sp, #128]
   12324:	e350000a 	cmp	r0, #10	; 0xa
   12328:	0a000017 	beq	1238c <number+0x12c>
   1232c:	e3500010 	cmp	r0, #16	; 0x10
   12330:	03a01004 	moveq	r1, #4	; 0x4
   12334:	13a01003 	movne	r1, #3	; 0x3
   12338:	e58d100c 	str	r1, [sp, #12]
   1233c:	e240b001 	sub	fp, r0, #1	; 0x1
   12340:	e3a04000 	mov	r4, #0	; 0x0
   12344:	e6ef3076 	uxtb	r3, r6
   12348:	e59f2164 	ldr	r2, [pc, #356]	; 124b4 <number+0x254>
   1234c:	e003300b 	and	r3, r3, fp
   12350:	e59dc004 	ldr	ip, [sp, #4]
   12354:	e7d23003 	ldrb	r3, [r2, r3]
   12358:	e1a00006 	mov	r0, r6
   1235c:	e18c3003 	orr	r3, ip, r3
   12360:	e28dc016 	add	ip, sp, #22	; 0x16
   12364:	e1a01007 	mov	r1, r7
   12368:	e59d200c 	ldr	r2, [sp, #12]
   1236c:	e7cc3004 	strb	r3, [ip, r4]
   12370:	eb000b93 	bl	151c4 <__aeabi_llsr>
   12374:	e1a06000 	mov	r6, r0
   12378:	e1960001 	orrs	r0, r6, r1
   1237c:	e1a07001 	mov	r7, r1
   12380:	e2844001 	add	r4, r4, #1	; 0x1
   12384:	1affffee 	bne	12344 <number+0xe4>
   12388:	ea000005 	b	123a4 <number+0x144>
   1238c:	e28d4016 	add	r4, sp, #22	; 0x16
   12390:	e1a00004 	mov	r0, r4
   12394:	e1a02006 	mov	r2, r6
   12398:	e1a03007 	mov	r3, r7
   1239c:	ebffff64 	bl	12134 <put_dec>
   123a0:	e0644000 	rsb	r4, r4, r0
   123a4:	e59d3088 	ldr	r3, [sp, #136]
   123a8:	e1540003 	cmp	r4, r3
   123ac:	a1a01004 	movge	r1, r4
   123b0:	b1a01003 	movlt	r1, r3
   123b4:	e3180011 	tst	r8, #17	; 0x11
   123b8:	e061c005 	rsb	ip, r1, r5
   123bc:	01a0300a 	moveq	r3, sl
   123c0:	03a02020 	moveq	r2, #32	; 0x20
   123c4:	0a000001 	beq	123d0 <number+0x170>
   123c8:	ea000004 	b	123e0 <number+0x180>
   123cc:	e5432001 	strb	r2, [r3, #-1]
   123d0:	e25cc001 	subs	ip, ip, #1	; 0x1
   123d4:	e1a0a003 	mov	sl, r3
   123d8:	e2833001 	add	r3, r3, #1	; 0x1
   123dc:	5afffffa 	bpl	123cc <number+0x16c>
   123e0:	e3590000 	cmp	r9, #0	; 0x0
   123e4:	14ca9001 	strbne	r9, [sl], #1
   123e8:	e59d2008 	ldr	r2, [sp, #8]
   123ec:	e3520000 	cmp	r2, #0	; 0x0
   123f0:	0a000006 	beq	12410 <number+0x1b0>
   123f4:	e59d3080 	ldr	r3, [sp, #128]
   123f8:	e3530010 	cmp	r3, #16	; 0x10
   123fc:	e3a03030 	mov	r3, #48	; 0x30
   12400:	e4ca3001 	strb	r3, [sl], #1
   12404:	059d0004 	ldreq	r0, [sp, #4]
   12408:	03803058 	orreq	r3, r0, #88	; 0x58
   1240c:	04ca3001 	strbeq	r3, [sl], #1
   12410:	e3180010 	tst	r8, #16	; 0x10
   12414:	1a000009 	bne	12440 <number+0x1e0>
   12418:	e3180001 	tst	r8, #1	; 0x1
   1241c:	13a02030 	movne	r2, #48	; 0x30
   12420:	03a02020 	moveq	r2, #32	; 0x20
   12424:	e1a0300a 	mov	r3, sl
   12428:	ea000000 	b	12430 <number+0x1d0>
   1242c:	e5432001 	strb	r2, [r3, #-1]
   12430:	e25cc001 	subs	ip, ip, #1	; 0x1
   12434:	e1a0a003 	mov	sl, r3
   12438:	e2833001 	add	r3, r3, #1	; 0x1
   1243c:	5afffffa 	bpl	1242c <number+0x1cc>
   12440:	e1a0000a 	mov	r0, sl
   12444:	e3a03030 	mov	r3, #48	; 0x30
   12448:	ea000000 	b	12450 <number+0x1f0>
   1244c:	e5403001 	strb	r3, [r0, #-1]
   12450:	e2411001 	sub	r1, r1, #1	; 0x1
   12454:	e1540001 	cmp	r4, r1
   12458:	e1a02000 	mov	r2, r0
   1245c:	e2800001 	add	r0, r0, #1	; 0x1
   12460:	dafffff9 	ble	1244c <number+0x1ec>
   12464:	e28d1058 	add	r1, sp, #88	; 0x58
   12468:	e0813004 	add	r3, r1, r4
   1246c:	e2431043 	sub	r1, r3, #67	; 0x43
   12470:	ea000001 	b	1247c <number+0x21c>
   12474:	e5d13001 	ldrb	r3, [r1, #1]
   12478:	e5423001 	strb	r3, [r2, #-1]
   1247c:	e2544001 	subs	r4, r4, #1	; 0x1
   12480:	e1a03002 	mov	r3, r2
   12484:	e2411001 	sub	r1, r1, #1	; 0x1
   12488:	e2822001 	add	r2, r2, #1	; 0x1
   1248c:	5afffff8 	bpl	12474 <number+0x214>
   12490:	e3a02020 	mov	r2, #32	; 0x20
   12494:	ea000000 	b	1249c <number+0x23c>
   12498:	e5432001 	strb	r2, [r3, #-1]
   1249c:	e25cc001 	subs	ip, ip, #1	; 0x1
   124a0:	e1a00003 	mov	r0, r3
   124a4:	e2833001 	add	r3, r3, #1	; 0x1
   124a8:	5afffffa 	bpl	12498 <number+0x238>
   124ac:	e28dd05c 	add	sp, sp, #92	; 0x5c
   124b0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   124b4:	000165c1 	.word	0x000165c1

000124b8 <vsprintf>:
   124b8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   124bc:	e24dd084 	sub	sp, sp, #132	; 0x84
   124c0:	e58d0028 	str	r0, [sp, #40]
   124c4:	e1a08001 	mov	r8, r1
   124c8:	e1a0a000 	mov	sl, r0
   124cc:	e28d107b 	add	r1, sp, #123	; 0x7b
   124d0:	e28d007d 	add	r0, sp, #125	; 0x7d
   124d4:	e58d2024 	str	r2, [sp, #36]
   124d8:	e1a06002 	mov	r6, r2
   124dc:	e58d001c 	str	r0, [sp, #28]
   124e0:	e58d1018 	str	r1, [sp, #24]
   124e4:	ea0001a3 	b	12b78 <vsprintf+0x6c0>
   124e8:	e3530025 	cmp	r3, #37	; 0x25
   124ec:	1a00015f 	bne	12a70 <vsprintf+0x5b8>
   124f0:	e3a07000 	mov	r7, #0	; 0x0
   124f4:	e5f82001 	ldrb	r2, [r8, #1]!
   124f8:	e352002b 	cmp	r2, #43	; 0x2b
   124fc:	0a00000c 	beq	12534 <vsprintf+0x7c>
   12500:	8a000004 	bhi	12518 <vsprintf+0x60>
   12504:	e3520020 	cmp	r2, #32	; 0x20
   12508:	0a00000b 	beq	1253c <vsprintf+0x84>
   1250c:	e3520023 	cmp	r2, #35	; 0x23
   12510:	1a00000f 	bne	12554 <vsprintf+0x9c>
   12514:	ea00000a 	b	12544 <vsprintf+0x8c>
   12518:	e352002d 	cmp	r2, #45	; 0x2d
   1251c:	0a000002 	beq	1252c <vsprintf+0x74>
   12520:	e3520030 	cmp	r2, #48	; 0x30
   12524:	1a00000a 	bne	12554 <vsprintf+0x9c>
   12528:	ea000007 	b	1254c <vsprintf+0x94>
   1252c:	e3877010 	orr	r7, r7, #16	; 0x10
   12530:	eaffffef 	b	124f4 <vsprintf+0x3c>
   12534:	e3877004 	orr	r7, r7, #4	; 0x4
   12538:	eaffffed 	b	124f4 <vsprintf+0x3c>
   1253c:	e3877008 	orr	r7, r7, #8	; 0x8
   12540:	eaffffeb 	b	124f4 <vsprintf+0x3c>
   12544:	e3877040 	orr	r7, r7, #64	; 0x40
   12548:	eaffffe9 	b	124f4 <vsprintf+0x3c>
   1254c:	e3877001 	orr	r7, r7, #1	; 0x1
   12550:	eaffffe7 	b	124f4 <vsprintf+0x3c>
   12554:	e2423030 	sub	r3, r2, #48	; 0x30
   12558:	e6ef3073 	uxtb	r3, r3
   1255c:	e3530009 	cmp	r3, #9	; 0x9
   12560:	93a09000 	movls	r9, #0	; 0x0
   12564:	93a0100a 	movls	r1, #10	; 0xa
   12568:	9a000002 	bls	12578 <vsprintf+0xc0>
   1256c:	ea000007 	b	12590 <vsprintf+0xd8>
   12570:	e0292991 	mla	r9, r1, r9, r2
   12574:	e2888001 	add	r8, r8, #1	; 0x1
   12578:	e5d83000 	ldrb	r3, [r8]
   1257c:	e2432030 	sub	r2, r3, #48	; 0x30
   12580:	e6ef3072 	uxtb	r3, r2
   12584:	e3530009 	cmp	r3, #9	; 0x9
   12588:	9afffff8 	bls	12570 <vsprintf+0xb8>
   1258c:	ea000008 	b	125b4 <vsprintf+0xfc>
   12590:	e352002a 	cmp	r2, #42	; 0x2a
   12594:	13e09000 	mvnne	r9, #0	; 0x0
   12598:	1a000005 	bne	125b4 <vsprintf+0xfc>
   1259c:	e5969000 	ldr	r9, [r6]
   125a0:	e2888001 	add	r8, r8, #1	; 0x1
   125a4:	e3590000 	cmp	r9, #0	; 0x0
   125a8:	e2866004 	add	r6, r6, #4	; 0x4
   125ac:	b2699000 	rsblt	r9, r9, #0	; 0x0
   125b0:	b3877010 	orrlt	r7, r7, #16	; 0x10
   125b4:	e5d83000 	ldrb	r3, [r8]
   125b8:	e353002e 	cmp	r3, #46	; 0x2e
   125bc:	13e02000 	mvnne	r2, #0	; 0x0
   125c0:	158d202c 	strne	r2, [sp, #44]
   125c4:	1a00001d 	bne	12640 <vsprintf+0x188>
   125c8:	e5f82001 	ldrb	r2, [r8, #1]!
   125cc:	e2423030 	sub	r3, r2, #48	; 0x30
   125d0:	e6ef3073 	uxtb	r3, r3
   125d4:	e3530009 	cmp	r3, #9	; 0x9
   125d8:	93a03000 	movls	r3, #0	; 0x0
   125dc:	958d302c 	strls	r3, [sp, #44]
   125e0:	93a0100a 	movls	r1, #10	; 0xa
   125e4:	9a000004 	bls	125fc <vsprintf+0x144>
   125e8:	ea000009 	b	12614 <vsprintf+0x15c>
   125ec:	e59d002c 	ldr	r0, [sp, #44]
   125f0:	e2888001 	add	r8, r8, #1	; 0x1
   125f4:	e0202091 	mla	r0, r1, r0, r2
   125f8:	e58d002c 	str	r0, [sp, #44]
   125fc:	e5d83000 	ldrb	r3, [r8]
   12600:	e2432030 	sub	r2, r3, #48	; 0x30
   12604:	e6ef3072 	uxtb	r3, r2
   12608:	e3530009 	cmp	r3, #9	; 0x9
   1260c:	9afffff6 	bls	125ec <vsprintf+0x134>
   12610:	ea000005 	b	1262c <vsprintf+0x174>
   12614:	e352002a 	cmp	r2, #42	; 0x2a
   12618:	1a000006 	bne	12638 <vsprintf+0x180>
   1261c:	e5961000 	ldr	r1, [r6]
   12620:	e2888001 	add	r8, r8, #1	; 0x1
   12624:	e58d102c 	str	r1, [sp, #44]
   12628:	e2866004 	add	r6, r6, #4	; 0x4
   1262c:	e59d202c 	ldr	r2, [sp, #44]
   12630:	e3520000 	cmp	r2, #0	; 0x0
   12634:	aa000001 	bge	12640 <vsprintf+0x188>
   12638:	e3a03000 	mov	r3, #0	; 0x0
   1263c:	e58d302c 	str	r3, [sp, #44]
   12640:	e5d83000 	ldrb	r3, [r8]
   12644:	e353006c 	cmp	r3, #108	; 0x6c
   12648:	13530068 	cmpne	r3, #104	; 0x68
   1264c:	0a000008 	beq	12674 <vsprintf+0x1bc>
   12650:	e353004c 	cmp	r3, #76	; 0x4c
   12654:	0a000006 	beq	12674 <vsprintf+0x1bc>
   12658:	e353005a 	cmp	r3, #90	; 0x5a
   1265c:	0a000004 	beq	12674 <vsprintf+0x1bc>
   12660:	e353007a 	cmp	r3, #122	; 0x7a
   12664:	0a000002 	beq	12674 <vsprintf+0x1bc>
   12668:	e3530074 	cmp	r3, #116	; 0x74
   1266c:	13e02000 	mvnne	r2, #0	; 0x0
   12670:	1a000007 	bne	12694 <vsprintf+0x1dc>
   12674:	e353006c 	cmp	r3, #108	; 0x6c
   12678:	e1a02003 	mov	r2, r3
   1267c:	e2888001 	add	r8, r8, #1	; 0x1
   12680:	1a000003 	bne	12694 <vsprintf+0x1dc>
   12684:	e5d83000 	ldrb	r3, [r8]
   12688:	e353006c 	cmp	r3, #108	; 0x6c
   1268c:	02888001 	addeq	r8, r8, #1	; 0x1
   12690:	03a0204c 	moveq	r2, #76	; 0x4c
   12694:	e5d83000 	ldrb	r3, [r8]
   12698:	e353006e 	cmp	r3, #110	; 0x6e
   1269c:	0a0000df 	beq	12a20 <vsprintf+0x568>
   126a0:	8a00000c 	bhi	126d8 <vsprintf+0x220>
   126a4:	e3530063 	cmp	r3, #99	; 0x63
   126a8:	0a000017 	beq	1270c <vsprintf+0x254>
   126ac:	8a000004 	bhi	126c4 <vsprintf+0x20c>
   126b0:	e3530025 	cmp	r3, #37	; 0x25
   126b4:	0a0000ed 	beq	12a70 <vsprintf+0x5b8>
   126b8:	e3530058 	cmp	r3, #88	; 0x58
   126bc:	1a0000e5 	bne	12a58 <vsprintf+0x5a0>
   126c0:	ea0000e0 	b	12a48 <vsprintf+0x590>
   126c4:	e3530064 	cmp	r3, #100	; 0x64
   126c8:	0a0000e0 	beq	12a50 <vsprintf+0x598>
   126cc:	e3530069 	cmp	r3, #105	; 0x69
   126d0:	1a0000e0 	bne	12a58 <vsprintf+0x5a0>
   126d4:	ea0000dd 	b	12a50 <vsprintf+0x598>
   126d8:	e3530073 	cmp	r3, #115	; 0x73
   126dc:	0a000021 	beq	12768 <vsprintf+0x2b0>
   126e0:	8a000004 	bhi	126f8 <vsprintf+0x240>
   126e4:	e353006f 	cmp	r3, #111	; 0x6f
   126e8:	0a0000d3 	beq	12a3c <vsprintf+0x584>
   126ec:	e3530070 	cmp	r3, #112	; 0x70
   126f0:	1a0000d8 	bne	12a58 <vsprintf+0x5a0>
   126f4:	ea000023 	b	12788 <vsprintf+0x2d0>
   126f8:	e3530075 	cmp	r3, #117	; 0x75
   126fc:	0a0000dd 	beq	12a78 <vsprintf+0x5c0>
   12700:	e3530078 	cmp	r3, #120	; 0x78
   12704:	1a0000d3 	bne	12a58 <vsprintf+0x5a0>
   12708:	ea0000cd 	b	12a44 <vsprintf+0x58c>
   1270c:	e3170010 	tst	r7, #16	; 0x10
   12710:	01a0300a 	moveq	r3, sl
   12714:	03a02020 	moveq	r2, #32	; 0x20
   12718:	0a000001 	beq	12724 <vsprintf+0x26c>
   1271c:	ea000005 	b	12738 <vsprintf+0x280>
   12720:	e5432001 	strb	r2, [r3, #-1]
   12724:	e2499001 	sub	r9, r9, #1	; 0x1
   12728:	e3590000 	cmp	r9, #0	; 0x0
   1272c:	e1a0a003 	mov	sl, r3
   12730:	e2833001 	add	r3, r3, #1	; 0x1
   12734:	cafffff9 	bgt	12720 <vsprintf+0x268>
   12738:	e5963000 	ldr	r3, [r6]
   1273c:	e1a0200a 	mov	r2, sl
   12740:	e4c23001 	strb	r3, [r2], #1
   12744:	e3a03020 	mov	r3, #32	; 0x20
   12748:	ea000000 	b	12750 <vsprintf+0x298>
   1274c:	e5423001 	strb	r3, [r2, #-1]
   12750:	e2499001 	sub	r9, r9, #1	; 0x1
   12754:	e3590000 	cmp	r9, #0	; 0x0
   12758:	e1a0a002 	mov	sl, r2
   1275c:	e2822001 	add	r2, r2, #1	; 0x1
   12760:	cafffff9 	bgt	1274c <vsprintf+0x294>
   12764:	ea0000ab 	b	12a18 <vsprintf+0x560>
   12768:	e58d7000 	str	r7, [sp]
   1276c:	e1a0000a 	mov	r0, sl
   12770:	e5961000 	ldr	r1, [r6]
   12774:	e1a02009 	mov	r2, r9
   12778:	e59d302c 	ldr	r3, [sp, #44]
   1277c:	ebfffe40 	bl	12084 <string>
   12780:	e1a0a000 	mov	sl, r0
   12784:	ea0000a3 	b	12a18 <vsprintf+0x560>
   12788:	e596b000 	ldr	fp, [r6]
   1278c:	e35b0000 	cmp	fp, #0	; 0x0
   12790:	1a000005 	bne	127ac <vsprintf+0x2f4>
   12794:	e1a0000a 	mov	r0, sl
   12798:	e1a02009 	mov	r2, r9
   1279c:	e59d302c 	ldr	r3, [sp, #44]
   127a0:	e59f13f0 	ldr	r1, [pc, #1008]	; 12b98 <vsprintf+0x6e0>
   127a4:	e58d7000 	str	r7, [sp]
   127a8:	ea000081 	b	129b4 <vsprintf+0x4fc>
   127ac:	e5d83001 	ldrb	r3, [r8, #1]
   127b0:	e2882001 	add	r2, r8, #1	; 0x1
   127b4:	e353004d 	cmp	r3, #77	; 0x4d
   127b8:	0a000008 	beq	127e0 <vsprintf+0x328>
   127bc:	8a000002 	bhi	127cc <vsprintf+0x314>
   127c0:	e3530049 	cmp	r3, #73	; 0x49
   127c4:	1a00007c 	bne	129bc <vsprintf+0x504>
   127c8:	ea000025 	b	12864 <vsprintf+0x3ac>
   127cc:	e3530069 	cmp	r3, #105	; 0x69
   127d0:	0a000022 	beq	12860 <vsprintf+0x3a8>
   127d4:	e353006d 	cmp	r3, #109	; 0x6d
   127d8:	1a000077 	bne	129bc <vsprintf+0x504>
   127dc:	e3877040 	orr	r7, r7, #64	; 0x40
   127e0:	e1a03327 	lsr	r3, r7, #6
   127e4:	e2233001 	eor	r3, r3, #1	; 0x1
   127e8:	e2034001 	and	r4, r3, #1	; 0x1
   127ec:	e3a0e000 	mov	lr, #0	; 0x0
   127f0:	e28dc05b 	add	ip, sp, #91	; 0x5b
   127f4:	e3a0503a 	mov	r5, #58	; 0x3a
   127f8:	e7db200e 	ldrb	r2, [fp, lr]
   127fc:	e59f0398 	ldr	r0, [pc, #920]	; 12b9c <vsprintf+0x6e4>
   12800:	e202300f 	and	r3, r2, #15	; 0xf
   12804:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
   12808:	e7d03003 	ldrb	r3, [r0, r3]
   1280c:	e1a0000c 	mov	r0, ip
   12810:	e4c02001 	strb	r2, [r0], #1
   12814:	e35e0005 	cmp	lr, #5	; 0x5
   12818:	03a01000 	moveq	r1, #0	; 0x0
   1281c:	12041001 	andne	r1, r4, #1	; 0x1
   12820:	e3510000 	cmp	r1, #0	; 0x0
   12824:	e5cc3001 	strb	r3, [ip, #1]
   12828:	e28ee001 	add	lr, lr, #1	; 0x1
   1282c:	e280c001 	add	ip, r0, #1	; 0x1
   12830:	128cc001 	addne	ip, ip, #1	; 0x1
   12834:	15c05001 	strbne	r5, [r0, #1]
   12838:	e35e0006 	cmp	lr, #6	; 0x6
   1283c:	1affffed 	bne	127f8 <vsprintf+0x340>
   12840:	e3a03000 	mov	r3, #0	; 0x0
   12844:	e5cc3000 	strb	r3, [ip]
   12848:	e1a0000a 	mov	r0, sl
   1284c:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12850:	e1a02009 	mov	r2, r9
   12854:	e59d302c 	ldr	r3, [sp, #44]
   12858:	e28d105b 	add	r1, sp, #91	; 0x5b
   1285c:	ea000053 	b	129b0 <vsprintf+0x4f8>
   12860:	e3877040 	orr	r7, r7, #64	; 0x40
   12864:	e5d23001 	ldrb	r3, [r2, #1]
   12868:	e3530036 	cmp	r3, #54	; 0x36
   1286c:	1a00002d 	bne	12928 <vsprintf+0x470>
   12870:	e1a03327 	lsr	r3, r7, #6
   12874:	e2233001 	eor	r3, r3, #1	; 0x1
   12878:	e1a0500b 	mov	r5, fp
   1287c:	e3a04000 	mov	r4, #0	; 0x0
   12880:	e203b001 	and	fp, r3, #1	; 0x1
   12884:	e28dc033 	add	ip, sp, #51	; 0x33
   12888:	e5d52000 	ldrb	r2, [r5]
   1288c:	e59fe308 	ldr	lr, [pc, #776]	; 12b9c <vsprintf+0x6e4>
   12890:	e202300f 	and	r3, r2, #15	; 0xf
   12894:	e7de0003 	ldrb	r0, [lr, r3]
   12898:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
   1289c:	e1a0100c 	mov	r1, ip
   128a0:	e4c13001 	strb	r3, [r1], #1
   128a4:	e5cc0001 	strb	r0, [ip, #1]
   128a8:	e5d52001 	ldrb	r2, [r5, #1]
   128ac:	e59f02e8 	ldr	r0, [pc, #744]	; 12b9c <vsprintf+0x6e4>
   128b0:	e202300f 	and	r3, r2, #15	; 0xf
   128b4:	e7d0c003 	ldrb	ip, [r0, r3]
   128b8:	e59f32dc 	ldr	r3, [pc, #732]	; 12b9c <vsprintf+0x6e4>
   128bc:	e3540007 	cmp	r4, #7	; 0x7
   128c0:	03a0e000 	moveq	lr, #0	; 0x0
   128c4:	120be001 	andne	lr, fp, #1	; 0x1
   128c8:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
   128cc:	e35e0000 	cmp	lr, #0	; 0x0
   128d0:	e58d2014 	str	r2, [sp, #20]
   128d4:	e2810001 	add	r0, r1, #1	; 0x1
   128d8:	e59de014 	ldr	lr, [sp, #20]
   128dc:	e2802001 	add	r2, r0, #1	; 0x1
   128e0:	e5c1e001 	strb	lr, [r1, #1]
   128e4:	e2844001 	add	r4, r4, #1	; 0x1
   128e8:	e5c0c001 	strb	ip, [r0, #1]
   128ec:	13a0003a 	movne	r0, #58	; 0x3a
   128f0:	e282c001 	add	ip, r2, #1	; 0x1
   128f4:	128cc001 	addne	ip, ip, #1	; 0x1
   128f8:	15c20001 	strbne	r0, [r2, #1]
   128fc:	e3540008 	cmp	r4, #8	; 0x8
   12900:	e2855002 	add	r5, r5, #2	; 0x2
   12904:	1affffdf 	bne	12888 <vsprintf+0x3d0>
   12908:	e3a03000 	mov	r3, #0	; 0x0
   1290c:	e5cc3000 	strb	r3, [ip]
   12910:	e1a0000a 	mov	r0, sl
   12914:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12918:	e1a02009 	mov	r2, r9
   1291c:	e59d302c 	ldr	r3, [sp, #44]
   12920:	e28d1033 	add	r1, sp, #51	; 0x33
   12924:	ea000021 	b	129b0 <vsprintf+0x4f8>
   12928:	e3530034 	cmp	r3, #52	; 0x34
   1292c:	13c77040 	bicne	r7, r7, #64	; 0x40
   12930:	1a000021 	bne	129bc <vsprintf+0x504>
   12934:	e3a05000 	mov	r5, #0	; 0x0
   12938:	e28d406d 	add	r4, sp, #109	; 0x6d
   1293c:	e7db1005 	ldrb	r1, [fp, r5]
   12940:	e28d007d 	add	r0, sp, #125	; 0x7d
   12944:	ebfffd9b 	bl	11fb8 <put_dec_trunc>
   12948:	e59d101c 	ldr	r1, [sp, #28]
   1294c:	e2402001 	sub	r2, r0, #1	; 0x1
   12950:	e0610000 	rsb	r0, r1, r0
   12954:	e3a01000 	mov	r1, #0	; 0x0
   12958:	ea000002 	b	12968 <vsprintf+0x4b0>
   1295c:	e5d23001 	ldrb	r3, [r2, #1]
   12960:	e7c43001 	strb	r3, [r4, r1]
   12964:	e2811001 	add	r1, r1, #1	; 0x1
   12968:	e59d3018 	ldr	r3, [sp, #24]
   1296c:	e2422001 	sub	r2, r2, #1	; 0x1
   12970:	e1520003 	cmp	r2, r3
   12974:	1afffff8 	bne	1295c <vsprintf+0x4a4>
   12978:	e3550003 	cmp	r5, #3	; 0x3
   1297c:	13a0e02e 	movne	lr, #46	; 0x2e
   12980:	e0844000 	add	r4, r4, r0
   12984:	e2855001 	add	r5, r5, #1	; 0x1
   12988:	14c4e001 	strbne	lr, [r4], #1
   1298c:	e3550004 	cmp	r5, #4	; 0x4
   12990:	1affffe9 	bne	1293c <vsprintf+0x484>
   12994:	e3a03000 	mov	r3, #0	; 0x0
   12998:	e5c43000 	strb	r3, [r4]
   1299c:	e59d302c 	ldr	r3, [sp, #44]
   129a0:	e3c7c040 	bic	ip, r7, #64	; 0x40
   129a4:	e1a0000a 	mov	r0, sl
   129a8:	e1a02009 	mov	r2, r9
   129ac:	e28d106d 	add	r1, sp, #109	; 0x6d
   129b0:	e58dc000 	str	ip, [sp]
   129b4:	ebfffdb2 	bl	12084 <string>
   129b8:	ea00000d 	b	129f4 <vsprintf+0x53c>
   129bc:	e3a01010 	mov	r1, #16	; 0x10
   129c0:	e3790001 	cmn	r9, #1	; 0x1
   129c4:	e58d1000 	str	r1, [sp]
   129c8:	e59d102c 	ldr	r1, [sp, #44]
   129cc:	e387c020 	orr	ip, r7, #32	; 0x20
   129d0:	02899009 	addeq	r9, r9, #9	; 0x9
   129d4:	0387c021 	orreq	ip, r7, #33	; 0x21
   129d8:	e1a0000a 	mov	r0, sl
   129dc:	e1a0200b 	mov	r2, fp
   129e0:	e3a03000 	mov	r3, #0	; 0x0
   129e4:	e58d9004 	str	r9, [sp, #4]
   129e8:	e58d1008 	str	r1, [sp, #8]
   129ec:	e58dc00c 	str	ip, [sp, #12]
   129f0:	ebfffe1a 	bl	12260 <number>
   129f4:	e1a0a000 	mov	sl, r0
   129f8:	e1a02008 	mov	r2, r8
   129fc:	e5d23001 	ldrb	r3, [r2, #1]
   12a00:	e59fe198 	ldr	lr, [pc, #408]	; 12ba0 <vsprintf+0x6e8>
   12a04:	e1a08002 	mov	r8, r2
   12a08:	e7de3003 	ldrb	r3, [lr, r3]
   12a0c:	e2822001 	add	r2, r2, #1	; 0x1
   12a10:	e3130007 	tst	r3, #7	; 0x7
   12a14:	1afffff8 	bne	129fc <vsprintf+0x544>
   12a18:	e2866004 	add	r6, r6, #4	; 0x4
   12a1c:	ea000054 	b	12b74 <vsprintf+0x6bc>
   12a20:	e59d0028 	ldr	r0, [sp, #40]
   12a24:	e5963000 	ldr	r3, [r6]
   12a28:	e2861004 	add	r1, r6, #4	; 0x4
   12a2c:	e060200a 	rsb	r2, r0, sl
   12a30:	e1a06001 	mov	r6, r1
   12a34:	e5832000 	str	r2, [r3]
   12a38:	ea00004d 	b	12b74 <vsprintf+0x6bc>
   12a3c:	e3a0c008 	mov	ip, #8	; 0x8
   12a40:	ea00000d 	b	12a7c <vsprintf+0x5c4>
   12a44:	e3877020 	orr	r7, r7, #32	; 0x20
   12a48:	e3a0c010 	mov	ip, #16	; 0x10
   12a4c:	ea00000a 	b	12a7c <vsprintf+0x5c4>
   12a50:	e3877002 	orr	r7, r7, #2	; 0x2
   12a54:	ea000007 	b	12a78 <vsprintf+0x5c0>
   12a58:	e3a03025 	mov	r3, #37	; 0x25
   12a5c:	e4ca3001 	strb	r3, [sl], #1
   12a60:	e5d83000 	ldrb	r3, [r8]
   12a64:	e3530000 	cmp	r3, #0	; 0x0
   12a68:	02488001 	subeq	r8, r8, #1	; 0x1
   12a6c:	0a000040 	beq	12b74 <vsprintf+0x6bc>
   12a70:	e4ca3001 	strb	r3, [sl], #1
   12a74:	ea00003e 	b	12b74 <vsprintf+0x6bc>
   12a78:	e3a0c00a 	mov	ip, #10	; 0xa
   12a7c:	e352004c 	cmp	r2, #76	; 0x4c
   12a80:	1a000009 	bne	12aac <vsprintf+0x5f4>
   12a84:	e59d1024 	ldr	r1, [sp, #36]
   12a88:	e3a02000 	mov	r2, #0	; 0x0
   12a8c:	e0613006 	rsb	r3, r1, r6
   12a90:	e3130007 	tst	r3, #7	; 0x7
   12a94:	02866004 	addeq	r6, r6, #4	; 0x4
   12a98:	e896000a 	ldm	r6, {r1, r3}
   12a9c:	e1a05003 	mov	r5, r3
   12aa0:	e1824001 	orr	r4, r2, r1
   12aa4:	e2866008 	add	r6, r6, #8	; 0x8
   12aa8:	ea000028 	b	12b50 <vsprintf+0x698>
   12aac:	e352006c 	cmp	r2, #108	; 0x6c
   12ab0:	1a000006 	bne	12ad0 <vsprintf+0x618>
   12ab4:	e5963000 	ldr	r3, [r6]
   12ab8:	e3170002 	tst	r7, #2	; 0x2
   12abc:	e1a04003 	mov	r4, r3
   12ac0:	e3a05000 	mov	r5, #0	; 0x0
   12ac4:	e2866004 	add	r6, r6, #4	; 0x4
   12ac8:	0a000020 	beq	12b50 <vsprintf+0x698>
   12acc:	ea00001e 	b	12b4c <vsprintf+0x694>
   12ad0:	e352007a 	cmp	r2, #122	; 0x7a
   12ad4:	1352005a 	cmpne	r2, #90	; 0x5a
   12ad8:	05963000 	ldreq	r3, [r6]
   12adc:	02866004 	addeq	r6, r6, #4	; 0x4
   12ae0:	01a04003 	moveq	r4, r3
   12ae4:	03a05000 	moveq	r5, #0	; 0x0
   12ae8:	0a000018 	beq	12b50 <vsprintf+0x698>
   12aec:	e3520074 	cmp	r2, #116	; 0x74
   12af0:	05963000 	ldreq	r3, [r6]
   12af4:	02866004 	addeq	r6, r6, #4	; 0x4
   12af8:	0a00000b 	beq	12b2c <vsprintf+0x674>
   12afc:	e3520068 	cmp	r2, #104	; 0x68
   12b00:	e2861004 	add	r1, r6, #4	; 0x4
   12b04:	e5962000 	ldr	r2, [r6]
   12b08:	e2070002 	and	r0, r7, #2	; 0x2
   12b0c:	1a000009 	bne	12b38 <vsprintf+0x680>
   12b10:	e6ff3072 	uxth	r3, r2
   12b14:	e3500000 	cmp	r0, #0	; 0x0
   12b18:	e1a06001 	mov	r6, r1
   12b1c:	e1a04003 	mov	r4, r3
   12b20:	e3a05000 	mov	r5, #0	; 0x0
   12b24:	0a000009 	beq	12b50 <vsprintf+0x698>
   12b28:	e6bf3074 	sxth	r3, r4
   12b2c:	e1a04003 	mov	r4, r3
   12b30:	e1a05fc4 	asr	r5, r4, #31
   12b34:	ea000005 	b	12b50 <vsprintf+0x698>
   12b38:	e3500000 	cmp	r0, #0	; 0x0
   12b3c:	e1a06001 	mov	r6, r1
   12b40:	e1a04002 	mov	r4, r2
   12b44:	e3a05000 	mov	r5, #0	; 0x0
   12b48:	0a000000 	beq	12b50 <vsprintf+0x698>
   12b4c:	e1a05fc4 	asr	r5, r4, #31
   12b50:	e59de02c 	ldr	lr, [sp, #44]
   12b54:	e1a0000a 	mov	r0, sl
   12b58:	e1a02004 	mov	r2, r4
   12b5c:	e1a03005 	mov	r3, r5
   12b60:	e58dc000 	str	ip, [sp]
   12b64:	e98d4200 	stmib	sp, {r9, lr}
   12b68:	e58d700c 	str	r7, [sp, #12]
   12b6c:	ebfffdbb 	bl	12260 <number>
   12b70:	e1a0a000 	mov	sl, r0
   12b74:	e2888001 	add	r8, r8, #1	; 0x1
   12b78:	e5d83000 	ldrb	r3, [r8]
   12b7c:	e3530000 	cmp	r3, #0	; 0x0
   12b80:	1afffe58 	bne	124e8 <vsprintf+0x30>
   12b84:	e5ca3000 	strb	r3, [sl]
   12b88:	e59d1028 	ldr	r1, [sp, #40]
   12b8c:	e061000a 	rsb	r0, r1, sl
   12b90:	e28dd084 	add	sp, sp, #132	; 0x84
   12b94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   12b98:	00019636 	.word	0x00019636
   12b9c:	000165b0 	.word	0x000165b0
   12ba0:	000165d1 	.word	0x000165d1

00012ba4 <sprintf>:
   12ba4:	e92d000e 	push	{r1, r2, r3}
   12ba8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   12bac:	e59d1004 	ldr	r1, [sp, #4]
   12bb0:	e28d2008 	add	r2, sp, #8	; 0x8
   12bb4:	ebfffe3f 	bl	124b8 <vsprintf>
   12bb8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   12bbc:	e28dd00c 	add	sp, sp, #12	; 0xc
   12bc0:	e12fff1e 	bx	lr

00012bc4 <ius_check_header>:
   12bc4:	e92d4070 	push	{r4, r5, r6, lr}
   12bc8:	e5901000 	ldr	r1, [r0]
   12bcc:	e59f30e8 	ldr	r3, [pc, #232]	; 12cbc <ius_check_header+0xf8>
   12bd0:	e24dd008 	sub	sp, sp, #8	; 0x8
   12bd4:	e1510003 	cmp	r1, r3
   12bd8:	e1a04000 	mov	r4, r0
   12bdc:	0a000005 	beq	12bf8 <ius_check_header+0x34>
   12be0:	e59f00d8 	ldr	r0, [pc, #216]	; 12cc0 <ius_check_header+0xfc>
   12be4:	e59f30d8 	ldr	r3, [pc, #216]	; 12cc4 <ius_check_header+0x100>
   12be8:	e1a0e00f 	mov	lr, pc
   12bec:	e593f000 	ldr	pc, [r3]
   12bf0:	e3e00000 	mvn	r0, #0	; 0x0
   12bf4:	ea00002e 	b	12cb4 <ius_check_header+0xf0>
   12bf8:	ebffd196 	bl	7258 <cdbg_verify_burn>
   12bfc:	e3500000 	cmp	r0, #0	; 0x0
   12c00:	0a000011 	beq	12c4c <ius_check_header+0x88>
   12c04:	e5943004 	ldr	r3, [r4, #4]
   12c08:	e3130002 	tst	r3, #2	; 0x2
   12c0c:	1a000005 	bne	12c28 <ius_check_header+0x64>
   12c10:	e59f00b0 	ldr	r0, [pc, #176]	; 12cc8 <ius_check_header+0x104>
   12c14:	e59f30a8 	ldr	r3, [pc, #168]	; 12cc4 <ius_check_header+0x100>
   12c18:	e1a0e00f 	mov	lr, pc
   12c1c:	e593f000 	ldr	pc, [r3]
   12c20:	e3e00021 	mvn	r0, #33	; 0x21
   12c24:	ea000022 	b	12cb4 <ius_check_header+0xf0>
   12c28:	ebffd18e 	bl	7268 <cdbg_verify_burn_key>
   12c2c:	e59f2098 	ldr	r2, [pc, #152]	; 12ccc <ius_check_header+0x108>
   12c30:	e1a01000 	mov	r1, r0
   12c34:	e3a00000 	mov	r0, #0	; 0x0
   12c38:	ebffce26 	bl	64d8 <ss_aes_init>
   12c3c:	e2840010 	add	r0, r4, #16	; 0x10
   12c40:	e1a01000 	mov	r1, r0
   12c44:	e3a02e1f 	mov	r2, #496	; 0x1f0
   12c48:	ebffce11 	bl	6494 <ss_aes_data>
   12c4c:	e3a06000 	mov	r6, #0	; 0x0
   12c50:	e5945018 	ldr	r5, [r4, #24]
   12c54:	e3a01c02 	mov	r1, #512	; 0x200
   12c58:	e5846018 	str	r6, [r4, #24]
   12c5c:	e1a00006 	mov	r0, r6
   12c60:	ebffcf35 	bl	693c <hash_init>
   12c64:	e3a01c02 	mov	r1, #512	; 0x200
   12c68:	e1a00004 	mov	r0, r4
   12c6c:	ebffcf12 	bl	68bc <hash_data>
   12c70:	e28d0004 	add	r0, sp, #4	; 0x4
   12c74:	ebffcef4 	bl	684c <hash_value>
   12c78:	e59d2004 	ldr	r2, [sp, #4]
   12c7c:	e59f3040 	ldr	r3, [pc, #64]	; 12cc4 <ius_check_header+0x100>
   12c80:	e1550002 	cmp	r5, r2
   12c84:	e5845018 	str	r5, [r4, #24]
   12c88:	0a000005 	beq	12ca4 <ius_check_header+0xe0>
   12c8c:	e59f003c 	ldr	r0, [pc, #60]	; 12cd0 <ius_check_header+0x10c>
   12c90:	e1a01005 	mov	r1, r5
   12c94:	e1a0e00f 	mov	lr, pc
   12c98:	e593f000 	ldr	pc, [r3]
   12c9c:	e3e0001f 	mvn	r0, #31	; 0x1f
   12ca0:	ea000003 	b	12cb4 <ius_check_header+0xf0>
   12ca4:	e59f0028 	ldr	r0, [pc, #40]	; 12cd4 <ius_check_header+0x110>
   12ca8:	e1a0e00f 	mov	lr, pc
   12cac:	e593f000 	ldr	pc, [r3]
   12cb0:	e1a00006 	mov	r0, r6
   12cb4:	e28dd008 	add	sp, sp, #8	; 0x8
   12cb8:	e8bd8070 	pop	{r4, r5, r6, pc}
   12cbc:	7375692b 	.word	0x7375692b
   12cc0:	000182d3 	.word	0x000182d3
   12cc4:	3c00a5cc 	.word	0x3c00a5cc
   12cc8:	0001963d 	.word	0x0001963d
   12ccc:	3c00a004 	.word	0x3c00a004
   12cd0:	000182ed 	.word	0x000182ed
   12cd4:	0001831c 	.word	0x0001831c

00012cd8 <printf_off>:
   12cd8:	e92d000f 	push	{r0, r1, r2, r3}
   12cdc:	e28dd010 	add	sp, sp, #16	; 0x10
   12ce0:	e12fff1e 	bx	lr

00012ce4 <disable_ctrlc>:
   12ce4:	e59f300c 	ldr	r3, [pc, #12]	; 12cf8 <disable_ctrlc+0x14>
   12ce8:	e5932000 	ldr	r2, [r3]
   12cec:	e5830000 	str	r0, [r3]
   12cf0:	e1a00002 	mov	r0, r2
   12cf4:	e12fff1e 	bx	lr
   12cf8:	3c00a16c 	.word	0x3c00a16c

00012cfc <had_ctrlc>:
   12cfc:	e59f3004 	ldr	r3, [pc, #4]	; 12d08 <had_ctrlc+0xc>
   12d00:	e5930000 	ldr	r0, [r3]
   12d04:	e12fff1e 	bx	lr
   12d08:	3c00a168 	.word	0x3c00a168

00012d0c <clear_ctrlc>:
   12d0c:	e59f3008 	ldr	r3, [pc, #8]	; 12d1c <clear_ctrlc+0x10>
   12d10:	e3a02000 	mov	r2, #0	; 0x0
   12d14:	e5832000 	str	r2, [r3]
   12d18:	e12fff1e 	bx	lr
   12d1c:	3c00a168 	.word	0x3c00a168

00012d20 <dbg>:
   12d20:	e92d000f 	push	{r0, r1, r2, r3}
   12d24:	e28dd010 	add	sp, sp, #16	; 0x10
   12d28:	e12fff1e 	bx	lr

00012d2c <puts>:
   12d2c:	eaffedf0 	b	e4f4 <serial_puts>

00012d30 <vprintf>:
   12d30:	e92d4010 	push	{r4, lr}
   12d34:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12d38:	e1a02001 	mov	r2, r1
   12d3c:	e1a01000 	mov	r1, r0
   12d40:	e1a0000d 	mov	r0, sp
   12d44:	ebfffddb 	bl	124b8 <vsprintf>
   12d48:	e1a0000d 	mov	r0, sp
   12d4c:	e1a0400d 	mov	r4, sp
   12d50:	ebffede7 	bl	e4f4 <serial_puts>
   12d54:	e28ddd06 	add	sp, sp, #384	; 0x180
   12d58:	e8bd8010 	pop	{r4, pc}

00012d5c <printf_on>:
   12d5c:	e92d000f 	push	{r0, r1, r2, r3}
   12d60:	e92d4010 	push	{r4, lr}
   12d64:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12d68:	e1a0000d 	mov	r0, sp
   12d6c:	e59d1188 	ldr	r1, [sp, #392]
   12d70:	e28d2f63 	add	r2, sp, #396	; 0x18c
   12d74:	ebfffdcf 	bl	124b8 <vsprintf>
   12d78:	e1a0000d 	mov	r0, sp
   12d7c:	e1a0400d 	mov	r4, sp
   12d80:	ebffeddb 	bl	e4f4 <serial_puts>
   12d84:	e28ddd06 	add	sp, sp, #384	; 0x180
   12d88:	e8bd4010 	pop	{r4, lr}
   12d8c:	e28dd010 	add	sp, sp, #16	; 0x10
   12d90:	e12fff1e 	bx	lr

00012d94 <serial_printf>:
   12d94:	e92d000f 	push	{r0, r1, r2, r3}
   12d98:	e92d4010 	push	{r4, lr}
   12d9c:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12da0:	e1a0000d 	mov	r0, sp
   12da4:	e59d1188 	ldr	r1, [sp, #392]
   12da8:	e28d2f63 	add	r2, sp, #396	; 0x18c
   12dac:	ebfffdc1 	bl	124b8 <vsprintf>
   12db0:	e1a0000d 	mov	r0, sp
   12db4:	e1a0400d 	mov	r4, sp
   12db8:	ebffedcd 	bl	e4f4 <serial_puts>
   12dbc:	e28ddd06 	add	sp, sp, #384	; 0x180
   12dc0:	e8bd4010 	pop	{r4, lr}
   12dc4:	e28dd010 	add	sp, sp, #16	; 0x10
   12dc8:	e12fff1e 	bx	lr

00012dcc <putc>:
   12dcc:	eaffedb1 	b	e498 <serial_putc>

00012dd0 <tstc>:
   12dd0:	eaffeda9 	b	e47c <serial_tstc>

00012dd4 <getc>:
   12dd4:	eaffedd7 	b	e538 <serial_getc>

00012dd8 <ctrlc>:
   12dd8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   12ddc:	e59f3040 	ldr	r3, [pc, #64]	; 12e24 <ctrlc+0x4c>
   12de0:	e24dd004 	sub	sp, sp, #4	; 0x4
   12de4:	e5933000 	ldr	r3, [r3]
   12de8:	e3530000 	cmp	r3, #0	; 0x0
   12dec:	1a000009 	bne	12e18 <ctrlc+0x40>
   12df0:	ebffeda1 	bl	e47c <serial_tstc>
   12df4:	e3500000 	cmp	r0, #0	; 0x0
   12df8:	0a000006 	beq	12e18 <ctrlc+0x40>
   12dfc:	ebffedcd 	bl	e538 <serial_getc>
   12e00:	e3500003 	cmp	r0, #3	; 0x3
   12e04:	059f301c 	ldreq	r3, [pc, #28]	; 12e28 <ctrlc+0x50>
   12e08:	03a02001 	moveq	r2, #1	; 0x1
   12e0c:	05832000 	streq	r2, [r3]
   12e10:	01a00002 	moveq	r0, r2
   12e14:	0a000000 	beq	12e1c <ctrlc+0x44>
   12e18:	e3a00000 	mov	r0, #0	; 0x0
   12e1c:	e28dd004 	add	sp, sp, #4	; 0x4
   12e20:	e8bd8000 	pop	{pc}
   12e24:	3c00a16c 	.word	0x3c00a16c
   12e28:	3c00a168 	.word	0x3c00a168

00012e2c <parse_line>:
   12e2c:	e92d4010 	push	{r4, lr}
   12e30:	e3a04000 	mov	r4, #0	; 0x0
   12e34:	e1a02004 	mov	r2, r4
   12e38:	ea000000 	b	12e40 <parse_line+0x14>
   12e3c:	e2800001 	add	r0, r0, #1	; 0x1
   12e40:	e5d03000 	ldrb	r3, [r0]
   12e44:	e3530009 	cmp	r3, #9	; 0x9
   12e48:	13530020 	cmpne	r3, #32	; 0x20
   12e4c:	0afffffa 	beq	12e3c <parse_line+0x10>
   12e50:	e3530000 	cmp	r3, #0	; 0x0
   12e54:	0a00000c 	beq	12e8c <parse_line+0x60>
   12e58:	e7810104 	str	r0, [r1, r4, lsl #2]
   12e5c:	e2844001 	add	r4, r4, #1	; 0x1
   12e60:	ea000000 	b	12e68 <parse_line+0x3c>
   12e64:	e2800001 	add	r0, r0, #1	; 0x1
   12e68:	e5d03000 	ldrb	r3, [r0]
   12e6c:	e3530000 	cmp	r3, #0	; 0x0
   12e70:	13530020 	cmpne	r3, #32	; 0x20
   12e74:	0a000002 	beq	12e84 <parse_line+0x58>
   12e78:	e3530009 	cmp	r3, #9	; 0x9
   12e7c:	1afffff8 	bne	12e64 <parse_line+0x38>
   12e80:	ea000003 	b	12e94 <parse_line+0x68>
   12e84:	e3530000 	cmp	r3, #0	; 0x0
   12e88:	1a000001 	bne	12e94 <parse_line+0x68>
   12e8c:	e7813104 	str	r3, [r1, r4, lsl #2]
   12e90:	ea000007 	b	12eb4 <parse_line+0x88>
   12e94:	e3540010 	cmp	r4, #16	; 0x10
   12e98:	e5c02000 	strb	r2, [r0]
   12e9c:	1affffe6 	bne	12e3c <parse_line+0x10>
   12ea0:	e59f0014 	ldr	r0, [pc, #20]	; 12ebc <parse_line+0x90>
   12ea4:	e1a01004 	mov	r1, r4
   12ea8:	e59f3010 	ldr	r3, [pc, #16]	; 12ec0 <parse_line+0x94>
   12eac:	e1a0e00f 	mov	lr, pc
   12eb0:	e593f000 	ldr	pc, [r3]
   12eb4:	e1a00004 	mov	r0, r4
   12eb8:	e8bd8010 	pop	{r4, pc}
   12ebc:	0001964c 	.word	0x0001964c
   12ec0:	3c00a5cc 	.word	0x3c00a5cc

00012ec4 <run_command>:
   12ec4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   12ec8:	e1a05000 	mov	r5, r0
   12ecc:	e24ddf93 	sub	sp, sp, #588	; 0x24c
   12ed0:	e1a0a001 	mov	sl, r1
   12ed4:	ebffff8c 	bl	12d0c <clear_ctrlc>
   12ed8:	e3550000 	cmp	r5, #0	; 0x0
   12edc:	0a0000ac 	beq	13194 <run_command+0x2d0>
   12ee0:	e5d53000 	ldrb	r3, [r5]
   12ee4:	e3530000 	cmp	r3, #0	; 0x0
   12ee8:	0a0000a9 	beq	13194 <run_command+0x2d0>
   12eec:	e1a00005 	mov	r0, r5
   12ef0:	ebffb941 	bl	13fc <strlen>
   12ef4:	e35000ff 	cmp	r0, #255	; 0xff
   12ef8:	9a000002 	bls	12f08 <run_command+0x44>
   12efc:	e59f02a0 	ldr	r0, [pc, #672]	; 131a4 <run_command+0x2e0>
   12f00:	ebffff89 	bl	12d2c <puts>
   12f04:	ea0000a2 	b	13194 <run_command+0x2d0>
   12f08:	e28d4f41 	add	r4, sp, #260	; 0x104
   12f0c:	e1a01005 	mov	r1, r5
   12f10:	e1a00004 	mov	r0, r4
   12f14:	e3a06000 	mov	r6, #0	; 0x0
   12f18:	ebffb8e0 	bl	12a0 <strcpy>
   12f1c:	e3a07001 	mov	r7, #1	; 0x1
   12f20:	e1a0b006 	mov	fp, r6
   12f24:	e28d9f81 	add	r9, sp, #516	; 0x204
   12f28:	ea000091 	b	13174 <run_command+0x2b0>
   12f2c:	e3510027 	cmp	r1, #39	; 0x27
   12f30:	1a000002 	bne	12f40 <run_command+0x7c>
   12f34:	e5523001 	ldrb	r3, [r2, #-1]
   12f38:	e353005c 	cmp	r3, #92	; 0x5c
   12f3c:	12200001 	eorne	r0, r0, #1	; 0x1
   12f40:	e3500000 	cmp	r0, #0	; 0x0
   12f44:	1a000009 	bne	12f70 <run_command+0xac>
   12f48:	e351003b 	cmp	r1, #59	; 0x3b
   12f4c:	13a03000 	movne	r3, #0	; 0x0
   12f50:	03a03001 	moveq	r3, #1	; 0x1
   12f54:	e1520004 	cmp	r2, r4
   12f58:	03a03000 	moveq	r3, #0	; 0x0
   12f5c:	e3530000 	cmp	r3, #0	; 0x0
   12f60:	0a000002 	beq	12f70 <run_command+0xac>
   12f64:	e5523001 	ldrb	r3, [r2, #-1]
   12f68:	e353005c 	cmp	r3, #92	; 0x5c
   12f6c:	1a000005 	bne	12f88 <run_command+0xc4>
   12f70:	e2822001 	add	r2, r2, #1	; 0x1
   12f74:	e5d21000 	ldrb	r1, [r2]
   12f78:	e3510000 	cmp	r1, #0	; 0x0
   12f7c:	1affffea 	bne	12f2c <run_command+0x68>
   12f80:	e1a08002 	mov	r8, r2
   12f84:	ea000001 	b	12f90 <run_command+0xcc>
   12f88:	e5c20000 	strb	r0, [r2]
   12f8c:	e2828001 	add	r8, r2, #1	; 0x1
   12f90:	e1a00004 	mov	r0, r4
   12f94:	ebffb918 	bl	13fc <strlen>
   12f98:	e3a0c000 	mov	ip, #0	; 0x0
   12f9c:	e3a0ec01 	mov	lr, #256	; 0x100
   12fa0:	e1a0100c 	mov	r1, ip
   12fa4:	e28d5004 	add	r5, sp, #4	; 0x4
   12fa8:	ea000049 	b	130d4 <run_command+0x210>
   12fac:	e35c0003 	cmp	ip, #3	; 0x3
   12fb0:	e2400001 	sub	r0, r0, #1	; 0x1
   12fb4:	e4d42001 	ldrb	r2, [r4], #1
   12fb8:	0a000035 	beq	13094 <run_command+0x1d0>
   12fbc:	e352005c 	cmp	r2, #92	; 0x5c
   12fc0:	13a03000 	movne	r3, #0	; 0x0
   12fc4:	03a03001 	moveq	r3, #1	; 0x1
   12fc8:	e351005c 	cmp	r1, #92	; 0x5c
   12fcc:	03a03000 	moveq	r3, #0	; 0x0
   12fd0:	e3530000 	cmp	r3, #0	; 0x0
   12fd4:	0a000004 	beq	12fec <run_command+0x128>
   12fd8:	e3500000 	cmp	r0, #0	; 0x0
   12fdc:	0a00003f 	beq	130e0 <run_command+0x21c>
   12fe0:	e4d42001 	ldrb	r2, [r4], #1
   12fe4:	e2400001 	sub	r0, r0, #1	; 0x1
   12fe8:	e3a0105c 	mov	r1, #92	; 0x5c
   12fec:	e35c0003 	cmp	ip, #3	; 0x3
   12ff0:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
   12ff4:	ea000035 	b	130d0 <run_command+0x20c>
   12ff8:	00013008 	.word	0x00013008
   12ffc:	0001304c 	.word	0x0001304c
   13000:	00013084 	.word	0x00013084
   13004:	00013094 	.word	0x00013094
   13008:	e251105c 	subs	r1, r1, #92	; 0x5c
   1300c:	13a01001 	movne	r1, #1	; 0x1
   13010:	e3520027 	cmp	r2, #39	; 0x27
   13014:	13a03000 	movne	r3, #0	; 0x0
   13018:	02013001 	andeq	r3, r1, #1	; 0x1
   1301c:	e3530000 	cmp	r3, #0	; 0x0
   13020:	13a0c003 	movne	ip, #3	; 0x3
   13024:	1a000022 	bne	130b4 <run_command+0x1f0>
   13028:	e3520024 	cmp	r2, #36	; 0x24
   1302c:	13a03000 	movne	r3, #0	; 0x0
   13030:	02013001 	andeq	r3, r1, #1	; 0x1
   13034:	e3530000 	cmp	r3, #0	; 0x0
   13038:	13a0c001 	movne	ip, #1	; 0x1
   1303c:	13a02024 	movne	r2, #36	; 0x24
   13040:	04c52001 	strbeq	r2, [r5], #1
   13044:	024ee001 	subeq	lr, lr, #1	; 0x1
   13048:	ea000020 	b	130d0 <run_command+0x20c>
   1304c:	e352007b 	cmp	r2, #123	; 0x7b
   13050:	13520028 	cmpne	r2, #40	; 0x28
   13054:	13a03000 	movne	r3, #0	; 0x0
   13058:	03a03001 	moveq	r3, #1	; 0x1
   1305c:	03a0c002 	moveq	ip, #2	; 0x2
   13060:	0a00001a 	beq	130d0 <run_command+0x20c>
   13064:	e3a01024 	mov	r1, #36	; 0x24
   13068:	e25ee001 	subs	lr, lr, #1	; 0x1
   1306c:	e4c51001 	strb	r1, [r5], #1
   13070:	0a000015 	beq	130cc <run_command+0x208>
   13074:	e4c52001 	strb	r2, [r5], #1
   13078:	e24ee001 	sub	lr, lr, #1	; 0x1
   1307c:	e1a0c003 	mov	ip, r3
   13080:	ea000012 	b	130d0 <run_command+0x20c>
   13084:	e352007d 	cmp	r2, #125	; 0x7d
   13088:	13520029 	cmpne	r2, #41	; 0x29
   1308c:	0a00000e 	beq	130cc <run_command+0x208>
   13090:	ea00000e 	b	130d0 <run_command+0x20c>
   13094:	e3520027 	cmp	r2, #39	; 0x27
   13098:	13a03000 	movne	r3, #0	; 0x0
   1309c:	03a03001 	moveq	r3, #1	; 0x1
   130a0:	e351005c 	cmp	r1, #92	; 0x5c
   130a4:	03a03000 	moveq	r3, #0	; 0x0
   130a8:	e3530000 	cmp	r3, #0	; 0x0
   130ac:	0a000002 	beq	130bc <run_command+0x1f8>
   130b0:	e3a0c000 	mov	ip, #0	; 0x0
   130b4:	e3a02027 	mov	r2, #39	; 0x27
   130b8:	ea000004 	b	130d0 <run_command+0x20c>
   130bc:	e4c52001 	strb	r2, [r5], #1
   130c0:	e24ee001 	sub	lr, lr, #1	; 0x1
   130c4:	e3a0c003 	mov	ip, #3	; 0x3
   130c8:	ea000000 	b	130d0 <run_command+0x20c>
   130cc:	e3a0c000 	mov	ip, #0	; 0x0
   130d0:	e1a01002 	mov	r1, r2
   130d4:	e3500000 	cmp	r0, #0	; 0x0
   130d8:	135e0000 	cmpne	lr, #0	; 0x0
   130dc:	1affffb2 	bne	12fac <run_command+0xe8>
   130e0:	e35e0000 	cmp	lr, #0	; 0x0
   130e4:	15c5b000 	strbne	fp, [r5]
   130e8:	0545e001 	strbeq	lr, [r5, #-1]
   130ec:	e28d0004 	add	r0, sp, #4	; 0x4
   130f0:	e1a01009 	mov	r1, r9
   130f4:	ebffff4c 	bl	12e2c <parse_line>
   130f8:	e2505000 	subs	r5, r0, #0	; 0x0
   130fc:	0a00000d 	beq	13138 <run_command+0x274>
   13100:	e59d0204 	ldr	r0, [sp, #516]
   13104:	ebffbf48 	bl	2e2c <find_cmd>
   13108:	e2504000 	subs	r4, r0, #0	; 0x0
   1310c:	1a000005 	bne	13128 <run_command+0x264>
   13110:	e59f0090 	ldr	r0, [pc, #144]	; 131a8 <run_command+0x2e4>
   13114:	e59d1204 	ldr	r1, [sp, #516]
   13118:	e59f308c 	ldr	r3, [pc, #140]	; 131ac <run_command+0x2e8>
   1311c:	e1a0e00f 	mov	lr, pc
   13120:	e593f000 	ldr	pc, [r3]
   13124:	ea000003 	b	13138 <run_command+0x274>
   13128:	e5943004 	ldr	r3, [r4, #4]
   1312c:	e1550003 	cmp	r5, r3
   13130:	da000002 	ble	13140 <run_command+0x27c>
   13134:	ebffbef6 	bl	2d14 <cmd_usage>
   13138:	e3e06000 	mvn	r6, #0	; 0x0
   1313c:	ea00000b 	b	13170 <run_command+0x2ac>
   13140:	e1a02005 	mov	r2, r5
   13144:	e1a0100a 	mov	r1, sl
   13148:	e1a03009 	mov	r3, r9
   1314c:	e1a0e00f 	mov	lr, pc
   13150:	e594f00c 	ldr	pc, [r4, #12]
   13154:	e3500000 	cmp	r0, #0	; 0x0
   13158:	13e06000 	mvnne	r6, #0	; 0x0
   1315c:	e5944008 	ldr	r4, [r4, #8]
   13160:	ebfffee5 	bl	12cfc <had_ctrlc>
   13164:	e3500000 	cmp	r0, #0	; 0x0
   13168:	1a000009 	bne	13194 <run_command+0x2d0>
   1316c:	e0077004 	and	r7, r7, r4
   13170:	e1a04008 	mov	r4, r8
   13174:	e5d43000 	ldrb	r3, [r4]
   13178:	e3530000 	cmp	r3, #0	; 0x0
   1317c:	11a02004 	movne	r2, r4
   13180:	13a00000 	movne	r0, #0	; 0x0
   13184:	1affff7a 	bne	12f74 <run_command+0xb0>
   13188:	e3560000 	cmp	r6, #0	; 0x0
   1318c:	01a06007 	moveq	r6, r7
   13190:	ea000000 	b	13198 <run_command+0x2d4>
   13194:	e3e06000 	mvn	r6, #0	; 0x0
   13198:	e1a00006 	mov	r0, r6
   1319c:	e28ddf93 	add	sp, sp, #588	; 0x24c
   131a0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   131a4:	0001966b 	.word	0x0001966b
   131a8:	00019681 	.word	0x00019681
   131ac:	3c00a5cc 	.word	0x3c00a5cc

000131b0 <cread_add_char>:
   131b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   131b4:	e3510000 	cmp	r1, #0	; 0x0
   131b8:	e24dd004 	sub	sp, sp, #4	; 0x4
   131bc:	e1a07000 	mov	r7, r0
   131c0:	e1a04002 	mov	r4, r2
   131c4:	e1a00003 	mov	r0, r3
   131c8:	e59d6018 	ldr	r6, [sp, #24]
   131cc:	e59dc01c 	ldr	ip, [sp, #28]
   131d0:	1a000003 	bne	131e4 <cread_add_char+0x34>
   131d4:	e5922000 	ldr	r2, [r2]
   131d8:	e5933000 	ldr	r3, [r3]
   131dc:	e1520003 	cmp	r2, r3
   131e0:	1a000026 	bne	13280 <cread_add_char+0xd0>
   131e4:	e5902000 	ldr	r2, [r0]
   131e8:	e24c3001 	sub	r3, ip, #1	; 0x1
   131ec:	e1520003 	cmp	r2, r3
   131f0:	9a000003 	bls	13204 <cread_add_char+0x54>
   131f4:	e3a00007 	mov	r0, #7	; 0x7
   131f8:	e28dd004 	add	sp, sp, #4	; 0x4
   131fc:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
   13200:	eafffef1 	b	12dcc <putc>
   13204:	e3510000 	cmp	r1, #0	; 0x0
   13208:	e2821001 	add	r1, r2, #1	; 0x1
   1320c:	e5801000 	str	r1, [r0]
   13210:	0a00001a 	beq	13280 <cread_add_char+0xd0>
   13214:	e5943000 	ldr	r3, [r4]
   13218:	e0635001 	rsb	r5, r3, r1
   1321c:	e3550001 	cmp	r5, #1	; 0x1
   13220:	9a000004 	bls	13238 <cread_add_char+0x88>
   13224:	e2830001 	add	r0, r3, #1	; 0x1
   13228:	e0860000 	add	r0, r6, r0
   1322c:	e0861003 	add	r1, r6, r3
   13230:	e2452001 	sub	r2, r5, #1	; 0x1
   13234:	ebffb893 	bl	1488 <memmove>
   13238:	e5943000 	ldr	r3, [r4]
   1323c:	e59f0074 	ldr	r0, [pc, #116]	; 132b8 <cread_add_char+0x108>
   13240:	e7c67003 	strb	r7, [r6, r3]
   13244:	e5942000 	ldr	r2, [r4]
   13248:	e59f306c 	ldr	r3, [pc, #108]	; 132bc <cread_add_char+0x10c>
   1324c:	e0862002 	add	r2, r6, r2
   13250:	e1a01005 	mov	r1, r5
   13254:	e1a0e00f 	mov	lr, pc
   13258:	e593f000 	ldr	pc, [r3]
   1325c:	e5943000 	ldr	r3, [r4]
   13260:	e2833001 	add	r3, r3, #1	; 0x1
   13264:	e5843000 	str	r3, [r4]
   13268:	ea000000 	b	13270 <cread_add_char+0xc0>
   1326c:	ebfffed6 	bl	12dcc <putc>
   13270:	e2555001 	subs	r5, r5, #1	; 0x1
   13274:	e3a00008 	mov	r0, #8	; 0x8
   13278:	1afffffb 	bne	1326c <cread_add_char+0xbc>
   1327c:	ea00000b 	b	132b0 <cread_add_char+0x100>
   13280:	e5943000 	ldr	r3, [r4]
   13284:	e59f002c 	ldr	r0, [pc, #44]	; 132b8 <cread_add_char+0x108>
   13288:	e7c67003 	strb	r7, [r6, r3]
   1328c:	e5942000 	ldr	r2, [r4]
   13290:	e59f3024 	ldr	r3, [pc, #36]	; 132bc <cread_add_char+0x10c>
   13294:	e0862002 	add	r2, r6, r2
   13298:	e3a01001 	mov	r1, #1	; 0x1
   1329c:	e1a0e00f 	mov	lr, pc
   132a0:	e593f000 	ldr	pc, [r3]
   132a4:	e5943000 	ldr	r3, [r4]
   132a8:	e2833001 	add	r3, r3, #1	; 0x1
   132ac:	e5843000 	str	r3, [r4]
   132b0:	e28dd004 	add	sp, sp, #4	; 0x4
   132b4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   132b8:	000196a4 	.word	0x000196a4
   132bc:	3c00a5cc 	.word	0x3c00a5cc

000132c0 <readline_into_buffer>:
   132c0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   132c4:	e59f367c 	ldr	r3, [pc, #1660]	; 13948 <readline_into_buffer+0x688>
   132c8:	e24dd01c 	sub	sp, sp, #28	; 0x1c
   132cc:	e593e000 	ldr	lr, [r3]
   132d0:	e1a06001 	mov	r6, r1
   132d4:	e35e0000 	cmp	lr, #0	; 0x0
   132d8:	1a000014 	bne	13330 <readline_into_buffer+0x70>
   132dc:	e59f3668 	ldr	r3, [pc, #1640]	; 1394c <readline_into_buffer+0x68c>
   132e0:	e3e02000 	mvn	r2, #0	; 0x0
   132e4:	e5832000 	str	r2, [r3]
   132e8:	e59f3660 	ldr	r3, [pc, #1632]	; 13950 <readline_into_buffer+0x690>
   132ec:	e59fc660 	ldr	ip, [pc, #1632]	; 13954 <readline_into_buffer+0x694>
   132f0:	e583e000 	str	lr, [r3]
   132f4:	e59f365c 	ldr	r3, [pc, #1628]	; 13958 <readline_into_buffer+0x698>
   132f8:	e59f465c 	ldr	r4, [pc, #1628]	; 1395c <readline_into_buffer+0x69c>
   132fc:	e583e000 	str	lr, [r3]
   13300:	e59f3658 	ldr	r3, [pc, #1624]	; 13960 <readline_into_buffer+0x6a0>
   13304:	e1a0100e 	mov	r1, lr
   13308:	e583e000 	str	lr, [r3]
   1330c:	e1a0300e 	mov	r3, lr
   13310:	e784c001 	str	ip, [r4, r1]
   13314:	e2811004 	add	r1, r1, #4	; 0x4
   13318:	e3510010 	cmp	r1, #16	; 0x10
   1331c:	e4cc3100 	strb	r3, [ip], #256
   13320:	1afffffa 	bne	13310 <readline_into_buffer+0x50>
   13324:	e59f361c 	ldr	r3, [pc, #1564]	; 13948 <readline_into_buffer+0x688>
   13328:	e3a02001 	mov	r2, #1	; 0x1
   1332c:	e5832000 	str	r2, [r3]
   13330:	ebfffe7d 	bl	12d2c <puts>
   13334:	e3a03000 	mov	r3, #0	; 0x0
   13338:	e58d3014 	str	r3, [sp, #20]
   1333c:	e58d3010 	str	r3, [sp, #16]
   13340:	e1a05003 	mov	r5, r3
   13344:	e3a09001 	mov	r9, #1	; 0x1
   13348:	e59fb614 	ldr	fp, [pc, #1556]	; 13964 <readline_into_buffer+0x6a4>
   1334c:	e59fa5f8 	ldr	sl, [pc, #1528]	; 1394c <readline_into_buffer+0x68c>
   13350:	ea000000 	b	13358 <readline_into_buffer+0x98>
   13354:	e3a05000 	mov	r5, #0	; 0x0
   13358:	ebfffe9d 	bl	12dd4 <getc>
   1335c:	e6ef0070 	uxtb	r0, r0
   13360:	e350000d 	cmp	r0, #13	; 0xd
   13364:	1350000a 	cmpne	r0, #10	; 0xa
   13368:	13a04000 	movne	r4, #0	; 0x0
   1336c:	03a04001 	moveq	r4, #1	; 0x1
   13370:	1a00000b 	bne	133a4 <readline_into_buffer+0xe4>
   13374:	e59f35e8 	ldr	r3, [pc, #1512]	; 13964 <readline_into_buffer+0x6a4>
   13378:	e59f05e8 	ldr	r0, [pc, #1512]	; 13968 <readline_into_buffer+0x6a8>
   1337c:	e1a0e00f 	mov	lr, pc
   13380:	e593f000 	ldr	pc, [r3]
   13384:	e59d5010 	ldr	r5, [sp, #16]
   13388:	e3a03000 	mov	r3, #0	; 0x0
   1338c:	e7c63005 	strb	r3, [r6, r5]
   13390:	e5d63000 	ldrb	r3, [r6]
   13394:	e3530000 	cmp	r3, #0	; 0x0
   13398:	13530021 	cmpne	r3, #33	; 0x21
   1339c:	0a000162 	beq	1392c <readline_into_buffer+0x66c>
   133a0:	ea00014e 	b	138e0 <readline_into_buffer+0x620>
   133a4:	e3550000 	cmp	r5, #0	; 0x0
   133a8:	0a000033 	beq	1347c <readline_into_buffer+0x1bc>
   133ac:	e3550001 	cmp	r5, #1	; 0x1
   133b0:	1a00000c 	bne	133e8 <readline_into_buffer+0x128>
   133b4:	e350005b 	cmp	r0, #91	; 0x5b
   133b8:	05cd0009 	strbeq	r0, [sp, #9]
   133bc:	03a05002 	moveq	r5, #2	; 0x2
   133c0:	0affffe4 	beq	13358 <readline_into_buffer+0x98>
   133c4:	e3a0cc01 	mov	ip, #256	; 0x100
   133c8:	e5dd0008 	ldrb	r0, [sp, #8]
   133cc:	e1a01009 	mov	r1, r9
   133d0:	e28d2014 	add	r2, sp, #20	; 0x14
   133d4:	e28d3010 	add	r3, sp, #16	; 0x10
   133d8:	e88d1040 	stm	sp, {r6, ip}
   133dc:	e1a05004 	mov	r5, r4
   133e0:	ebffff72 	bl	131b0 <cread_add_char>
   133e4:	eaffffdb 	b	13358 <readline_into_buffer+0x98>
   133e8:	e2403041 	sub	r3, r0, #65	; 0x41
   133ec:	e3530007 	cmp	r3, #7	; 0x7
   133f0:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   133f4:	ea000009 	b	13420 <readline_into_buffer+0x160>
   133f8:	00013764 	.word	0x00013764
   133fc:	000137a4 	.word	0x000137a4
   13400:	0001346c 	.word	0x0001346c
   13404:	00013578 	.word	0x00013578
   13408:	00013420 	.word	0x00013420
   1340c:	00013420 	.word	0x00013420
   13410:	00013420 	.word	0x00013420
   13414:	00013418 	.word	0x00013418
   13418:	e3a05000 	mov	r5, #0	; 0x0
   1341c:	ea000043 	b	13530 <readline_into_buffer+0x270>
   13420:	e28d1018 	add	r1, sp, #24	; 0x18
   13424:	e0813005 	add	r3, r1, r5
   13428:	e5430010 	strb	r0, [r3, #-16]
   1342c:	e2855001 	add	r5, r5, #1	; 0x1
   13430:	e3a04000 	mov	r4, #0	; 0x0
   13434:	e28d8010 	add	r8, sp, #16	; 0x10
   13438:	e3a07c01 	mov	r7, #256	; 0x100
   1343c:	ea000004 	b	13454 <readline_into_buffer+0x194>
   13440:	e28dc008 	add	ip, sp, #8	; 0x8
   13444:	e7dc0004 	ldrb	r0, [ip, r4]
   13448:	e88d00c0 	stm	sp, {r6, r7}
   1344c:	ebffff57 	bl	131b0 <cread_add_char>
   13450:	e2844001 	add	r4, r4, #1	; 0x1
   13454:	e1540005 	cmp	r4, r5
   13458:	e1a01009 	mov	r1, r9
   1345c:	e28d2014 	add	r2, sp, #20	; 0x14
   13460:	e1a03008 	mov	r3, r8
   13464:	1afffff5 	bne	13440 <readline_into_buffer+0x180>
   13468:	eaffffb9 	b	13354 <readline_into_buffer+0x94>
   1346c:	e3a00006 	mov	r0, #6	; 0x6
   13470:	e350000b 	cmp	r0, #11	; 0xb
   13474:	e3a05000 	mov	r5, #0	; 0x0
   13478:	ea000001 	b	13484 <readline_into_buffer+0x1c4>
   1347c:	e350000b 	cmp	r0, #11	; 0xb
   13480:	0a00005f 	beq	13604 <readline_into_buffer+0x344>
   13484:	8a00000d 	bhi	134c0 <readline_into_buffer+0x200>
   13488:	e3500004 	cmp	r0, #4	; 0x4
   1348c:	0a000042 	beq	1359c <readline_into_buffer+0x2dc>
   13490:	8a000005 	bhi	134ac <readline_into_buffer+0x1ec>
   13494:	e3500002 	cmp	r0, #2	; 0x2
   13498:	0a000037 	beq	1357c <readline_into_buffer+0x2bc>
   1349c:	8a000028 	bhi	13544 <readline_into_buffer+0x284>
   134a0:	e3500001 	cmp	r0, #1	; 0x1
   134a4:	1a000106 	bne	138c4 <readline_into_buffer+0x604>
   134a8:	ea000020 	b	13530 <readline_into_buffer+0x270>
   134ac:	e3500006 	cmp	r0, #6	; 0x6
   134b0:	0a000027 	beq	13554 <readline_into_buffer+0x294>
   134b4:	3a000066 	bcc	13654 <readline_into_buffer+0x394>
   134b8:	e3500008 	cmp	r0, #8	; 0x8
   134bc:	ea000015 	b	13518 <readline_into_buffer+0x258>
   134c0:	e3500015 	cmp	r0, #21	; 0x15
   134c4:	0a000070 	beq	1368c <readline_into_buffer+0x3cc>
   134c8:	8a000007 	bhi	134ec <readline_into_buffer+0x22c>
   134cc:	e350000f 	cmp	r0, #15	; 0xf
   134d0:	02299001 	eoreq	r9, r9, #1	; 0x1
   134d4:	0affff9f 	beq	13358 <readline_into_buffer+0x98>
   134d8:	e3500010 	cmp	r0, #16	; 0x10
   134dc:	0a0000a0 	beq	13764 <readline_into_buffer+0x4a4>
   134e0:	e350000e 	cmp	r0, #14	; 0xe
   134e4:	1a0000f6 	bne	138c4 <readline_into_buffer+0x604>
   134e8:	ea0000ad 	b	137a4 <readline_into_buffer+0x4e4>
   134ec:	e350001b 	cmp	r0, #27	; 0x1b
   134f0:	05cd0008 	strbeq	r0, [sp, #8]
   134f4:	03a05001 	moveq	r5, #1	; 0x1
   134f8:	0affff96 	beq	13358 <readline_into_buffer+0x98>
   134fc:	8a000002 	bhi	1350c <readline_into_buffer+0x24c>
   13500:	e3500018 	cmp	r0, #24	; 0x18
   13504:	1a0000ee 	bne	138c4 <readline_into_buffer+0x604>
   13508:	ea00005f 	b	1368c <readline_into_buffer+0x3cc>
   1350c:	e350007f 	cmp	r0, #127	; 0x7f
   13510:	0a000075 	beq	136ec <readline_into_buffer+0x42c>
   13514:	e35000ff 	cmp	r0, #255	; 0xff
   13518:	1a0000e9 	bne	138c4 <readline_into_buffer+0x604>
   1351c:	ea000072 	b	136ec <readline_into_buffer+0x42c>
   13520:	ebfffe29 	bl	12dcc <putc>
   13524:	e59d3014 	ldr	r3, [sp, #20]
   13528:	e2433001 	sub	r3, r3, #1	; 0x1
   1352c:	e58d3014 	str	r3, [sp, #20]
   13530:	e59d3014 	ldr	r3, [sp, #20]
   13534:	e3a00008 	mov	r0, #8	; 0x8
   13538:	e3530000 	cmp	r3, #0	; 0x0
   1353c:	1afffff7 	bne	13520 <readline_into_buffer+0x260>
   13540:	eaffff84 	b	13358 <readline_into_buffer+0x98>
   13544:	e3a03000 	mov	r3, #0	; 0x0
   13548:	e5c63000 	strb	r3, [r6]
   1354c:	e3e00000 	mvn	r0, #0	; 0x0
   13550:	ea0000fa 	b	13940 <readline_into_buffer+0x680>
   13554:	e59d2014 	ldr	r2, [sp, #20]
   13558:	e59d3010 	ldr	r3, [sp, #16]
   1355c:	e1520003 	cmp	r2, r3
   13560:	2affff7c 	bcs	13358 <readline_into_buffer+0x98>
   13564:	e7d60002 	ldrb	r0, [r6, r2]
   13568:	ebfffe17 	bl	12dcc <putc>
   1356c:	e59d3014 	ldr	r3, [sp, #20]
   13570:	e2833001 	add	r3, r3, #1	; 0x1
   13574:	ea0000d0 	b	138bc <readline_into_buffer+0x5fc>
   13578:	e3a05000 	mov	r5, #0	; 0x0
   1357c:	e59d3014 	ldr	r3, [sp, #20]
   13580:	e3530000 	cmp	r3, #0	; 0x0
   13584:	0affff73 	beq	13358 <readline_into_buffer+0x98>
   13588:	e3a00008 	mov	r0, #8	; 0x8
   1358c:	ebfffe0e 	bl	12dcc <putc>
   13590:	e59d3014 	ldr	r3, [sp, #20]
   13594:	e2433001 	sub	r3, r3, #1	; 0x1
   13598:	ea0000c7 	b	138bc <readline_into_buffer+0x5fc>
   1359c:	e59d2014 	ldr	r2, [sp, #20]
   135a0:	e59d3010 	ldr	r3, [sp, #16]
   135a4:	e1520003 	cmp	r2, r3
   135a8:	2affff6a 	bcs	13358 <readline_into_buffer+0x98>
   135ac:	e2433001 	sub	r3, r3, #1	; 0x1
   135b0:	e0534002 	subs	r4, r3, r2
   135b4:	0a00000a 	beq	135e4 <readline_into_buffer+0x324>
   135b8:	e2821001 	add	r1, r2, #1	; 0x1
   135bc:	e0861001 	add	r1, r6, r1
   135c0:	e0860002 	add	r0, r6, r2
   135c4:	e1a02004 	mov	r2, r4
   135c8:	ebffb7ae 	bl	1488 <memmove>
   135cc:	e59d2014 	ldr	r2, [sp, #20]
   135d0:	e59f0394 	ldr	r0, [pc, #916]	; 1396c <readline_into_buffer+0x6ac>
   135d4:	e0862002 	add	r2, r6, r2
   135d8:	e1a01004 	mov	r1, r4
   135dc:	e1a0e00f 	mov	lr, pc
   135e0:	e59bf000 	ldr	pc, [fp]
   135e4:	e3a00020 	mov	r0, #32	; 0x20
   135e8:	ebfffdf7 	bl	12dcc <putc>
   135ec:	e2444001 	sub	r4, r4, #1	; 0x1
   135f0:	e3a00008 	mov	r0, #8	; 0x8
   135f4:	ebfffdf4 	bl	12dcc <putc>
   135f8:	e3740001 	cmn	r4, #1	; 0x1
   135fc:	1afffffa 	bne	135ec <readline_into_buffer+0x32c>
   13600:	ea000053 	b	13754 <readline_into_buffer+0x494>
   13604:	e59d2014 	ldr	r2, [sp, #20]
   13608:	e59d3010 	ldr	r3, [sp, #16]
   1360c:	e1520003 	cmp	r2, r3
   13610:	31a04002 	movcc	r4, r2
   13614:	3a000002 	bcc	13624 <readline_into_buffer+0x364>
   13618:	eaffff4e 	b	13358 <readline_into_buffer+0x98>
   1361c:	ebfffdea 	bl	12dcc <putc>
   13620:	e2844001 	add	r4, r4, #1	; 0x1
   13624:	e59d3010 	ldr	r3, [sp, #16]
   13628:	e3a00020 	mov	r0, #32	; 0x20
   1362c:	e1540003 	cmp	r4, r3
   13630:	3afffff9 	bcc	1361c <readline_into_buffer+0x35c>
   13634:	ea000000 	b	1363c <readline_into_buffer+0x37c>
   13638:	ebfffde3 	bl	12dcc <putc>
   1363c:	e59d2014 	ldr	r2, [sp, #20]
   13640:	e3a00008 	mov	r0, #8	; 0x8
   13644:	e1540002 	cmp	r4, r2
   13648:	e2444001 	sub	r4, r4, #1	; 0x1
   1364c:	8afffff9 	bhi	13638 <readline_into_buffer+0x378>
   13650:	ea000023 	b	136e4 <readline_into_buffer+0x424>
   13654:	e59d3014 	ldr	r3, [sp, #20]
   13658:	e59d1010 	ldr	r1, [sp, #16]
   1365c:	e1530001 	cmp	r3, r1
   13660:	2affff3c 	bcs	13358 <readline_into_buffer+0x98>
   13664:	e0631001 	rsb	r1, r3, r1
   13668:	e0862003 	add	r2, r6, r3
   1366c:	e59f02f8 	ldr	r0, [pc, #760]	; 1396c <readline_into_buffer+0x6ac>
   13670:	e1a0e00f 	mov	lr, pc
   13674:	e59bf000 	ldr	pc, [fp]
   13678:	ea00008e 	b	138b8 <readline_into_buffer+0x5f8>
   1367c:	ebfffdd2 	bl	12dcc <putc>
   13680:	e59d3014 	ldr	r3, [sp, #20]
   13684:	e2433001 	sub	r3, r3, #1	; 0x1
   13688:	e58d3014 	str	r3, [sp, #20]
   1368c:	e59d2014 	ldr	r2, [sp, #20]
   13690:	e3a00008 	mov	r0, #8	; 0x8
   13694:	e3520000 	cmp	r2, #0	; 0x0
   13698:	1afffff7 	bne	1367c <readline_into_buffer+0x3bc>
   1369c:	e59d3010 	ldr	r3, [sp, #16]
   136a0:	e3530000 	cmp	r3, #0	; 0x0
   136a4:	11a04002 	movne	r4, r2
   136a8:	1a000002 	bne	136b8 <readline_into_buffer+0x3f8>
   136ac:	eaffff29 	b	13358 <readline_into_buffer+0x98>
   136b0:	ebfffdc5 	bl	12dcc <putc>
   136b4:	e2844001 	add	r4, r4, #1	; 0x1
   136b8:	e59d3010 	ldr	r3, [sp, #16]
   136bc:	e3a00020 	mov	r0, #32	; 0x20
   136c0:	e1540003 	cmp	r4, r3
   136c4:	3afffff9 	bcc	136b0 <readline_into_buffer+0x3f0>
   136c8:	ea000000 	b	136d0 <readline_into_buffer+0x410>
   136cc:	ebfffdbe 	bl	12dcc <putc>
   136d0:	e59d2014 	ldr	r2, [sp, #20]
   136d4:	e3a00008 	mov	r0, #8	; 0x8
   136d8:	e1540002 	cmp	r4, r2
   136dc:	e2444001 	sub	r4, r4, #1	; 0x1
   136e0:	8afffff9 	bhi	136cc <readline_into_buffer+0x40c>
   136e4:	e58d2010 	str	r2, [sp, #16]
   136e8:	eaffff1a 	b	13358 <readline_into_buffer+0x98>
   136ec:	e59d0014 	ldr	r0, [sp, #20]
   136f0:	e3500000 	cmp	r0, #0	; 0x0
   136f4:	0affff17 	beq	13358 <readline_into_buffer+0x98>
   136f8:	e59d2010 	ldr	r2, [sp, #16]
   136fc:	e2403001 	sub	r3, r0, #1	; 0x1
   13700:	e0604002 	rsb	r4, r0, r2
   13704:	e0861000 	add	r1, r6, r0
   13708:	e1a02004 	mov	r2, r4
   1370c:	e0860003 	add	r0, r6, r3
   13710:	e58d3014 	str	r3, [sp, #20]
   13714:	ebffb75b 	bl	1488 <memmove>
   13718:	e3a00008 	mov	r0, #8	; 0x8
   1371c:	ebfffdaa 	bl	12dcc <putc>
   13720:	e59d2014 	ldr	r2, [sp, #20]
   13724:	e59f0240 	ldr	r0, [pc, #576]	; 1396c <readline_into_buffer+0x6ac>
   13728:	e0862002 	add	r2, r6, r2
   1372c:	e1a01004 	mov	r1, r4
   13730:	e1a0e00f 	mov	lr, pc
   13734:	e59bf000 	ldr	pc, [fp]
   13738:	e3a00020 	mov	r0, #32	; 0x20
   1373c:	ebfffda2 	bl	12dcc <putc>
   13740:	e2444001 	sub	r4, r4, #1	; 0x1
   13744:	e3a00008 	mov	r0, #8	; 0x8
   13748:	ebfffd9f 	bl	12dcc <putc>
   1374c:	e3740001 	cmn	r4, #1	; 0x1
   13750:	1afffffa 	bne	13740 <readline_into_buffer+0x480>
   13754:	e59d3010 	ldr	r3, [sp, #16]
   13758:	e2433001 	sub	r3, r3, #1	; 0x1
   1375c:	e58d3010 	str	r3, [sp, #16]
   13760:	eafffefc 	b	13358 <readline_into_buffer+0x98>
   13764:	e59a2000 	ldr	r2, [sl]
   13768:	e3520000 	cmp	r2, #0	; 0x0
   1376c:	ba000024 	blt	13804 <readline_into_buffer+0x544>
   13770:	e2423001 	sub	r3, r2, #1	; 0x1
   13774:	e3730001 	cmn	r3, #1	; 0x1
   13778:	059f01d0 	ldreq	r0, [pc, #464]	; 13950 <readline_into_buffer+0x690>
   1377c:	e58a3000 	str	r3, [sl]
   13780:	05903000 	ldreq	r3, [r0]
   13784:	e59fc1cc 	ldr	ip, [pc, #460]	; 13958 <readline_into_buffer+0x698>
   13788:	058a3000 	streq	r3, [sl]
   1378c:	e59a1000 	ldr	r1, [sl]
   13790:	e59c3000 	ldr	r3, [ip]
   13794:	e1510003 	cmp	r1, r3
   13798:	058a2000 	streq	r2, [sl]
   1379c:	1a000014 	bne	137f4 <readline_into_buffer+0x534>
   137a0:	ea000017 	b	13804 <readline_into_buffer+0x544>
   137a4:	e59a1000 	ldr	r1, [sl]
   137a8:	e3510000 	cmp	r1, #0	; 0x0
   137ac:	ba000014 	blt	13804 <readline_into_buffer+0x544>
   137b0:	e59f21a0 	ldr	r2, [pc, #416]	; 13958 <readline_into_buffer+0x698>
   137b4:	e5923000 	ldr	r3, [r2]
   137b8:	e1510003 	cmp	r1, r3
   137bc:	0a000010 	beq	13804 <readline_into_buffer+0x544>
   137c0:	e59f3188 	ldr	r3, [pc, #392]	; 13950 <readline_into_buffer+0x690>
   137c4:	e59fc18c 	ldr	ip, [pc, #396]	; 13958 <readline_into_buffer+0x698>
   137c8:	e5932000 	ldr	r2, [r3]
   137cc:	e2813001 	add	r3, r1, #1	; 0x1
   137d0:	e1530002 	cmp	r3, r2
   137d4:	e58a3000 	str	r3, [sl]
   137d8:	c3a03000 	movgt	r3, #0	; 0x0
   137dc:	c58a3000 	strgt	r3, [sl]
   137e0:	e59a1000 	ldr	r1, [sl]
   137e4:	e59c3000 	ldr	r3, [ip]
   137e8:	e1510003 	cmp	r1, r3
   137ec:	059f517c 	ldreq	r5, [pc, #380]	; 13970 <readline_into_buffer+0x6b0>
   137f0:	0a00000a 	beq	13820 <readline_into_buffer+0x560>
   137f4:	e59f0160 	ldr	r0, [pc, #352]	; 1395c <readline_into_buffer+0x69c>
   137f8:	e7905101 	ldr	r5, [r0, r1, lsl #2]
   137fc:	e3550000 	cmp	r5, #0	; 0x0
   13800:	1a000006 	bne	13820 <readline_into_buffer+0x560>
   13804:	e3a00007 	mov	r0, #7	; 0x7
   13808:	ebfffd6f 	bl	12dcc <putc>
   1380c:	eafffed0 	b	13354 <readline_into_buffer+0x94>
   13810:	ebfffd6d 	bl	12dcc <putc>
   13814:	e59d3014 	ldr	r3, [sp, #20]
   13818:	e2433001 	sub	r3, r3, #1	; 0x1
   1381c:	e58d3014 	str	r3, [sp, #20]
   13820:	e59d2014 	ldr	r2, [sp, #20]
   13824:	e3a00008 	mov	r0, #8	; 0x8
   13828:	e3520000 	cmp	r2, #0	; 0x0
   1382c:	1afffff7 	bne	13810 <readline_into_buffer+0x550>
   13830:	e59d3010 	ldr	r3, [sp, #16]
   13834:	e3530000 	cmp	r3, #0	; 0x0
   13838:	11a04002 	movne	r4, r2
   1383c:	1a000002 	bne	1384c <readline_into_buffer+0x58c>
   13840:	ea00000d 	b	1387c <readline_into_buffer+0x5bc>
   13844:	ebfffd60 	bl	12dcc <putc>
   13848:	e2844001 	add	r4, r4, #1	; 0x1
   1384c:	e59d3010 	ldr	r3, [sp, #16]
   13850:	e3a00020 	mov	r0, #32	; 0x20
   13854:	e1540003 	cmp	r4, r3
   13858:	3afffff9 	bcc	13844 <readline_into_buffer+0x584>
   1385c:	ea000000 	b	13864 <readline_into_buffer+0x5a4>
   13860:	ebfffd59 	bl	12dcc <putc>
   13864:	e59d2014 	ldr	r2, [sp, #20]
   13868:	e3a00008 	mov	r0, #8	; 0x8
   1386c:	e1540002 	cmp	r4, r2
   13870:	e2444001 	sub	r4, r4, #1	; 0x1
   13874:	8afffff9 	bhi	13860 <readline_into_buffer+0x5a0>
   13878:	e58d2010 	str	r2, [sp, #16]
   1387c:	e1a01005 	mov	r1, r5
   13880:	e1a00006 	mov	r0, r6
   13884:	ebffb685 	bl	12a0 <strcpy>
   13888:	e1a00006 	mov	r0, r6
   1388c:	ebffb6da 	bl	13fc <strlen>
   13890:	e59d3014 	ldr	r3, [sp, #20]
   13894:	e58d0010 	str	r0, [sp, #16]
   13898:	e1530000 	cmp	r3, r0
   1389c:	2afffeac 	bcs	13354 <readline_into_buffer+0x94>
   138a0:	e0631000 	rsb	r1, r3, r0
   138a4:	e0862003 	add	r2, r6, r3
   138a8:	e59f00bc 	ldr	r0, [pc, #188]	; 1396c <readline_into_buffer+0x6ac>
   138ac:	e1a0e00f 	mov	lr, pc
   138b0:	e59bf000 	ldr	pc, [fp]
   138b4:	e3a05000 	mov	r5, #0	; 0x0
   138b8:	e59d3010 	ldr	r3, [sp, #16]
   138bc:	e58d3014 	str	r3, [sp, #20]
   138c0:	eafffea4 	b	13358 <readline_into_buffer+0x98>
   138c4:	e3a0cc01 	mov	ip, #256	; 0x100
   138c8:	e1a01009 	mov	r1, r9
   138cc:	e28d2014 	add	r2, sp, #20	; 0x14
   138d0:	e28d3010 	add	r3, sp, #16	; 0x10
   138d4:	e88d1040 	stm	sp, {r6, ip}
   138d8:	ebfffe34 	bl	131b0 <cread_add_char>
   138dc:	eafffe9d 	b	13358 <readline_into_buffer+0x98>
   138e0:	e59f4070 	ldr	r4, [pc, #112]	; 13958 <readline_into_buffer+0x698>
   138e4:	e59f3070 	ldr	r3, [pc, #112]	; 1395c <readline_into_buffer+0x69c>
   138e8:	e5942000 	ldr	r2, [r4]
   138ec:	e1a01006 	mov	r1, r6
   138f0:	e7930102 	ldr	r0, [r3, r2, lsl #2]
   138f4:	ebffb669 	bl	12a0 <strcpy>
   138f8:	e5943000 	ldr	r3, [r4]
   138fc:	e59f104c 	ldr	r1, [pc, #76]	; 13950 <readline_into_buffer+0x690>
   13900:	e2833001 	add	r3, r3, #1	; 0x1
   13904:	e5912000 	ldr	r2, [r1]
   13908:	e3530003 	cmp	r3, #3	; 0x3
   1390c:	c3a03000 	movgt	r3, #0	; 0x0
   13910:	e1530002 	cmp	r3, r2
   13914:	e59f2044 	ldr	r2, [pc, #68]	; 13960 <readline_into_buffer+0x6a0>
   13918:	e5843000 	str	r3, [r4]
   1391c:	c5813000 	strgt	r3, [r1]
   13920:	e5923000 	ldr	r3, [r2]
   13924:	e2833001 	add	r3, r3, #1	; 0x1
   13928:	e5823000 	str	r3, [r2]
   1392c:	e59f3024 	ldr	r3, [pc, #36]	; 13958 <readline_into_buffer+0x698>
   13930:	e1a00005 	mov	r0, r5
   13934:	e5932000 	ldr	r2, [r3]
   13938:	e59f300c 	ldr	r3, [pc, #12]	; 1394c <readline_into_buffer+0x68c>
   1393c:	e5832000 	str	r2, [r3]
   13940:	e28dd01c 	add	sp, sp, #28	; 0x1c
   13944:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   13948:	3c00a174 	.word	0x3c00a174
   1394c:	3c00a030 	.word	0x3c00a030
   13950:	3c00a178 	.word	0x3c00a178
   13954:	3c00a894 	.word	0x3c00a894
   13958:	3c00a17c 	.word	0x3c00a17c
   1395c:	3c00a884 	.word	0x3c00a884
   13960:	3c00a170 	.word	0x3c00a170
   13964:	3c00a5cc 	.word	0x3c00a5cc
   13968:	000171fb 	.word	0x000171fb
   1396c:	000196a4 	.word	0x000196a4
   13970:	000171fc 	.word	0x000171fc

00013974 <readline>:
   13974:	e59f1000 	ldr	r1, [pc, #0]	; 1397c <readline+0x8>
   13978:	eafffe50 	b	132c0 <readline_into_buffer>
   1397c:	a784      	.short	0xa784
   1397e:	3c00      	.short	0x3c00

00013980 <main_loop>:
   13980:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   13984:	e24dd004 	sub	sp, sp, #4	; 0x4
   13988:	e59f0030 	ldr	r0, [pc, #48]	; 139c0 <main_loop+0x40>
   1398c:	ebfffff8 	bl	13974 <readline>
   13990:	e1a03000 	mov	r3, r0
   13994:	e3730001 	cmn	r3, #1	; 0x1
   13998:	e3a01000 	mov	r1, #0	; 0x0
   1399c:	e59f0020 	ldr	r0, [pc, #32]	; 139c4 <main_loop+0x44>
   139a0:	1a000002 	bne	139b0 <main_loop+0x30>
   139a4:	e59f001c 	ldr	r0, [pc, #28]	; 139c8 <main_loop+0x48>
   139a8:	ebfffcdf 	bl	12d2c <puts>
   139ac:	eafffff5 	b	13988 <main_loop+0x8>
   139b0:	e3530000 	cmp	r3, #0	; 0x0
   139b4:	dafffff3 	ble	13988 <main_loop+0x8>
   139b8:	ebfffd41 	bl	12ec4 <run_command>
   139bc:	eafffff1 	b	13988 <main_loop+0x8>
   139c0:	000196c4 	.word	0x000196c4
   139c4:	3c00a784 	.word	0x3c00a784
   139c8:	000171fb 	.word	0x000171fb

000139cc <parse_num>:
   139cc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   139d0:	e1a08001 	mov	r8, r1
   139d4:	e1a07002 	mov	r7, r2
   139d8:	e1a06003 	mov	r6, r3
   139dc:	ea000000 	b	139e4 <parse_num+0x18>
   139e0:	e2800001 	add	r0, r0, #1	; 0x1
   139e4:	e5d03000 	ldrb	r3, [r0]
   139e8:	e3530020 	cmp	r3, #32	; 0x20
   139ec:	0afffffb 	beq	139e0 <parse_num+0x14>
   139f0:	ea000037 	b	13ad4 <parse_num+0x108>
   139f4:	e3520000 	cmp	r2, #0	; 0x0
   139f8:	0a00000a 	beq	13a28 <parse_num+0x5c>
   139fc:	e3530030 	cmp	r3, #48	; 0x30
   13a00:	1a000008 	bne	13a28 <parse_num+0x5c>
   13a04:	e5d02001 	ldrb	r2, [r0, #1]
   13a08:	e2423041 	sub	r3, r2, #65	; 0x41
   13a0c:	e6ef3073 	uxtb	r3, r3
   13a10:	e3530019 	cmp	r3, #25	; 0x19
   13a14:	92823020 	addls	r3, r2, #32	; 0x20
   13a18:	96ef2073 	uxtbls	r2, r3
   13a1c:	e3520078 	cmp	r2, #120	; 0x78
   13a20:	02800002 	addeq	r0, r0, #2	; 0x2
   13a24:	03a04010 	moveq	r4, #16	; 0x10
   13a28:	e4d0c001 	ldrb	ip, [r0], #1
   13a2c:	e24c3030 	sub	r3, ip, #48	; 0x30
   13a30:	e24c2041 	sub	r2, ip, #65	; 0x41
   13a34:	e6ef3073 	uxtb	r3, r3
   13a38:	e6ef2072 	uxtb	r2, r2
   13a3c:	e3530009 	cmp	r3, #9	; 0x9
   13a40:	83a01000 	movhi	r1, #0	; 0x0
   13a44:	93a01001 	movls	r1, #1	; 0x1
   13a48:	e3520005 	cmp	r2, #5	; 0x5
   13a4c:	83a02000 	movhi	r2, #0	; 0x0
   13a50:	93a02001 	movls	r2, #1	; 0x1
   13a54:	e1913002 	orrs	r3, r1, r2
   13a58:	1a000003 	bne	13a6c <parse_num+0xa0>
   13a5c:	e24c3061 	sub	r3, ip, #97	; 0x61
   13a60:	e6ef3073 	uxtb	r3, r3
   13a64:	e3530005 	cmp	r3, #5	; 0x5
   13a68:	8a00000f 	bhi	13aac <parse_num+0xe0>
   13a6c:	e3510000 	cmp	r1, #0	; 0x0
   13a70:	124c2030 	subne	r2, ip, #48	; 0x30
   13a74:	1a000007 	bne	13a98 <parse_num+0xcc>
   13a78:	e24c3061 	sub	r3, ip, #97	; 0x61
   13a7c:	e6ef3073 	uxtb	r3, r3
   13a80:	e3530005 	cmp	r3, #5	; 0x5
   13a84:	924c2057 	subls	r2, ip, #87	; 0x57
   13a88:	9a000002 	bls	13a98 <parse_num+0xcc>
   13a8c:	e3520000 	cmp	r2, #0	; 0x0
   13a90:	0a000002 	beq	13aa0 <parse_num+0xd4>
   13a94:	e24c2037 	sub	r2, ip, #55	; 0x37
   13a98:	e1520004 	cmp	r2, r4
   13a9c:	aa000002 	bge	13aac <parse_num+0xe0>
   13aa0:	e0252495 	mla	r5, r5, r4, r2
   13aa4:	e3a02000 	mov	r2, #0	; 0x0
   13aa8:	ea00000c 	b	13ae0 <parse_num+0x114>
   13aac:	e3560000 	cmp	r6, #0	; 0x0
   13ab0:	0a000012 	beq	13b00 <parse_num+0x134>
   13ab4:	e3a03000 	mov	r3, #0	; 0x0
   13ab8:	e7d62003 	ldrb	r2, [r6, r3]
   13abc:	e2833001 	add	r3, r3, #1	; 0x1
   13ac0:	e3520000 	cmp	r2, #0	; 0x0
   13ac4:	0a00000d 	beq	13b00 <parse_num+0x134>
   13ac8:	e15c0002 	cmp	ip, r2
   13acc:	1afffff9 	bne	13ab8 <parse_num+0xec>
   13ad0:	ea000005 	b	13aec <parse_num+0x120>
   13ad4:	e3a02001 	mov	r2, #1	; 0x1
   13ad8:	e3a0400a 	mov	r4, #10	; 0xa
   13adc:	e3a05000 	mov	r5, #0	; 0x0
   13ae0:	e5d03000 	ldrb	r3, [r0]
   13ae4:	e3530000 	cmp	r3, #0	; 0x0
   13ae8:	1affffc1 	bne	139f4 <parse_num+0x28>
   13aec:	e3570000 	cmp	r7, #0	; 0x0
   13af0:	15870000 	strne	r0, [r7]
   13af4:	e5885000 	str	r5, [r8]
   13af8:	e3a00001 	mov	r0, #1	; 0x1
   13afc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13b00:	e3a00000 	mov	r0, #0	; 0x0
   13b04:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00013b08 <xyzModem_stream_close>:
   13b08:	e59f2044 	ldr	r2, [pc, #68]	; 13b54 <xyzModem_stream_close+0x4c>
   13b0c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   13b10:	e5920428 	ldr	r0, [r2, #1064]
   13b14:	e59f303c 	ldr	r3, [pc, #60]	; 13b58 <xyzModem_stream_close+0x50>
   13b18:	e592c424 	ldr	ip, [r2, #1060]
   13b1c:	e592e418 	ldr	lr, [r2, #1048]
   13b20:	e3500000 	cmp	r0, #0	; 0x0
   13b24:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13b28:	e59f102c 	ldr	r1, [pc, #44]	; 13b5c <xyzModem_stream_close+0x54>
   13b2c:	01a01003 	moveq	r1, r3
   13b30:	e59f3028 	ldr	r3, [pc, #40]	; 13b60 <xyzModem_stream_close+0x58>
   13b34:	e88d5000 	stm	sp, {ip, lr}
   13b38:	e593c000 	ldr	ip, [r3]
   13b3c:	e59f0020 	ldr	r0, [pc, #32]	; 13b64 <xyzModem_stream_close+0x5c>
   13b40:	e5923420 	ldr	r3, [r2, #1056]
   13b44:	e592241c 	ldr	r2, [r2, #1052]
   13b48:	e12fff3c 	blx	ip
   13b4c:	e28dd00c 	add	sp, sp, #12	; 0xc
   13b50:	e8bd8000 	pop	{pc}
   13b54:	3c00a190 	.word	0x3c00a190
   13b58:	000196c8 	.word	0x000196c8
   13b5c:	000196ce 	.word	0x000196ce
   13b60:	3c00a5cc 	.word	0x3c00a5cc
   13b64:	000196d2 	.word	0x000196d2

00013b68 <xyzModem_error>:
   13b68:	e2800008 	add	r0, r0, #8	; 0x8
   13b6c:	e3500007 	cmp	r0, #7	; 0x7
   13b70:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
   13b74:	ea000017 	b	13bd8 <xyzModem_error+0x70>
   13b78:	00013bd0 	.word	0x00013bd0
   13b7c:	00013bc8 	.word	0x00013bc8
   13b80:	00013bc0 	.word	0x00013bc0
   13b84:	00013bb8 	.word	0x00013bb8
   13b88:	00013bb0 	.word	0x00013bb0
   13b8c:	00013ba8 	.word	0x00013ba8
   13b90:	00013b98 	.word	0x00013b98
   13b94:	00013ba0 	.word	0x00013ba0
   13b98:	e59f0040 	ldr	r0, [pc, #64]	; 13be0 <xyzModem_error+0x78>
   13b9c:	e12fff1e 	bx	lr
   13ba0:	e59f003c 	ldr	r0, [pc, #60]	; 13be4 <xyzModem_error+0x7c>
   13ba4:	e12fff1e 	bx	lr
   13ba8:	e59f0038 	ldr	r0, [pc, #56]	; 13be8 <xyzModem_error+0x80>
   13bac:	e12fff1e 	bx	lr
   13bb0:	e59f0034 	ldr	r0, [pc, #52]	; 13bec <xyzModem_error+0x84>
   13bb4:	e12fff1e 	bx	lr
   13bb8:	e59f0030 	ldr	r0, [pc, #48]	; 13bf0 <xyzModem_error+0x88>
   13bbc:	e12fff1e 	bx	lr
   13bc0:	e59f002c 	ldr	r0, [pc, #44]	; 13bf4 <xyzModem_error+0x8c>
   13bc4:	e12fff1e 	bx	lr
   13bc8:	e59f0028 	ldr	r0, [pc, #40]	; 13bf8 <xyzModem_error+0x90>
   13bcc:	e12fff1e 	bx	lr
   13bd0:	e59f0024 	ldr	r0, [pc, #36]	; 13bfc <xyzModem_error+0x94>
   13bd4:	e12fff1e 	bx	lr
   13bd8:	e59f0020 	ldr	r0, [pc, #32]	; 13c00 <xyzModem_error+0x98>
   13bdc:	e12fff1e 	bx	lr
   13be0:	00019713 	.word	0x00019713
   13be4:	00019733 	.word	0x00019733
   13be8:	00019745 	.word	0x00019745
   13bec:	0001974f 	.word	0x0001974f
   13bf0:	0001975b 	.word	0x0001975b
   13bf4:	00019765 	.word	0x00019765
   13bf8:	00019775 	.word	0x00019775
   13bfc:	00019788 	.word	0x00019788
   13c00:	0001979d 	.word	0x0001979d

00013c04 <delay>:
   13c04:	e12fff1e 	bx	lr

00013c08 <init_num>:
   13c08:	e59f305c 	ldr	r3, [pc, #92]	; 13c6c <init_num+0x64>
   13c0c:	e3a01001 	mov	r1, #1	; 0x1
   13c10:	e5c31000 	strb	r1, [r3]
   13c14:	e59f3054 	ldr	r3, [pc, #84]	; 13c70 <init_num+0x68>
   13c18:	e3a02000 	mov	r2, #0	; 0x0
   13c1c:	e5c32000 	strb	r2, [r3]
   13c20:	e59f304c 	ldr	r3, [pc, #76]	; 13c74 <init_num+0x6c>
   13c24:	e5c32000 	strb	r2, [r3]
   13c28:	e59f3048 	ldr	r3, [pc, #72]	; 13c78 <init_num+0x70>
   13c2c:	e5c32000 	strb	r2, [r3]
   13c30:	e59f3044 	ldr	r3, [pc, #68]	; 13c7c <init_num+0x74>
   13c34:	e5c32000 	strb	r2, [r3]
   13c38:	e59f3040 	ldr	r3, [pc, #64]	; 13c80 <init_num+0x78>
   13c3c:	e5832000 	str	r2, [r3]
   13c40:	e59f303c 	ldr	r3, [pc, #60]	; 13c84 <init_num+0x7c>
   13c44:	e5c32000 	strb	r2, [r3]
   13c48:	e59f3038 	ldr	r3, [pc, #56]	; 13c88 <init_num+0x80>
   13c4c:	e5c32000 	strb	r2, [r3]
   13c50:	e59f3034 	ldr	r3, [pc, #52]	; 13c8c <init_num+0x84>
   13c54:	e5c32000 	strb	r2, [r3]
   13c58:	e59f3030 	ldr	r3, [pc, #48]	; 13c90 <init_num+0x88>
   13c5c:	e5c32000 	strb	r2, [r3]
   13c60:	e59f302c 	ldr	r3, [pc, #44]	; 13c94 <init_num+0x8c>
   13c64:	e5c32000 	strb	r2, [r3]
   13c68:	e12fff1e 	bx	lr
   13c6c:	3c00a034 	.word	0x3c00a034
   13c70:	3c00a18d 	.word	0x3c00a18d
   13c74:	3c00a180 	.word	0x3c00a180
   13c78:	3c00a181 	.word	0x3c00a181
   13c7c:	3c00a182 	.word	0x3c00a182
   13c80:	3c00a184 	.word	0x3c00a184
   13c84:	3c00a188 	.word	0x3c00a188
   13c88:	3c00a189 	.word	0x3c00a189
   13c8c:	3c00a18a 	.word	0x3c00a18a
   13c90:	3c00a18b 	.word	0x3c00a18b
   13c94:	3c00a18c 	.word	0x3c00a18c

00013c98 <out_message>:
   13c98:	e92d4010 	push	{r4, lr}
   13c9c:	e59f30c8 	ldr	r3, [pc, #200]	; 13d6c <out_message+0xd4>
   13ca0:	e59f40c8 	ldr	r4, [pc, #200]	; 13d70 <out_message+0xd8>
   13ca4:	e5d31000 	ldrb	r1, [r3]
   13ca8:	e59f00c4 	ldr	r0, [pc, #196]	; 13d74 <out_message+0xdc>
   13cac:	e1a0e00f 	mov	lr, pc
   13cb0:	e594f000 	ldr	pc, [r4]
   13cb4:	e59f30bc 	ldr	r3, [pc, #188]	; 13d78 <out_message+0xe0>
   13cb8:	e59f00bc 	ldr	r0, [pc, #188]	; 13d7c <out_message+0xe4>
   13cbc:	e5d31000 	ldrb	r1, [r3]
   13cc0:	e1a0e00f 	mov	lr, pc
   13cc4:	e594f000 	ldr	pc, [r4]
   13cc8:	e59f30b0 	ldr	r3, [pc, #176]	; 13d80 <out_message+0xe8>
   13ccc:	e59f00b0 	ldr	r0, [pc, #176]	; 13d84 <out_message+0xec>
   13cd0:	e5931000 	ldr	r1, [r3]
   13cd4:	e1a0e00f 	mov	lr, pc
   13cd8:	e594f000 	ldr	pc, [r4]
   13cdc:	e59f30a4 	ldr	r3, [pc, #164]	; 13d88 <out_message+0xf0>
   13ce0:	e59f00a4 	ldr	r0, [pc, #164]	; 13d8c <out_message+0xf4>
   13ce4:	e5d31000 	ldrb	r1, [r3]
   13ce8:	e1a0e00f 	mov	lr, pc
   13cec:	e594f000 	ldr	pc, [r4]
   13cf0:	e59f3098 	ldr	r3, [pc, #152]	; 13d90 <out_message+0xf8>
   13cf4:	e59f0098 	ldr	r0, [pc, #152]	; 13d94 <out_message+0xfc>
   13cf8:	e5d31000 	ldrb	r1, [r3]
   13cfc:	e1a0e00f 	mov	lr, pc
   13d00:	e594f000 	ldr	pc, [r4]
   13d04:	e59f308c 	ldr	r3, [pc, #140]	; 13d98 <out_message+0x100>
   13d08:	e59f008c 	ldr	r0, [pc, #140]	; 13d9c <out_message+0x104>
   13d0c:	e5d31000 	ldrb	r1, [r3]
   13d10:	e1a0e00f 	mov	lr, pc
   13d14:	e594f000 	ldr	pc, [r4]
   13d18:	e59f3080 	ldr	r3, [pc, #128]	; 13da0 <out_message+0x108>
   13d1c:	e59f0080 	ldr	r0, [pc, #128]	; 13da4 <out_message+0x10c>
   13d20:	e5d31000 	ldrb	r1, [r3]
   13d24:	e1a0e00f 	mov	lr, pc
   13d28:	e594f000 	ldr	pc, [r4]
   13d2c:	e59f3074 	ldr	r3, [pc, #116]	; 13da8 <out_message+0x110>
   13d30:	e59f0074 	ldr	r0, [pc, #116]	; 13dac <out_message+0x114>
   13d34:	e5d31000 	ldrb	r1, [r3]
   13d38:	e1a0e00f 	mov	lr, pc
   13d3c:	e594f000 	ldr	pc, [r4]
   13d40:	e59f3068 	ldr	r3, [pc, #104]	; 13db0 <out_message+0x118>
   13d44:	e59f0068 	ldr	r0, [pc, #104]	; 13db4 <out_message+0x11c>
   13d48:	e5d31000 	ldrb	r1, [r3]
   13d4c:	e1a0e00f 	mov	lr, pc
   13d50:	e594f000 	ldr	pc, [r4]
   13d54:	e59f305c 	ldr	r3, [pc, #92]	; 13db8 <out_message+0x120>
   13d58:	e59f005c 	ldr	r0, [pc, #92]	; 13dbc <out_message+0x124>
   13d5c:	e5d31000 	ldrb	r1, [r3]
   13d60:	e1a0e00f 	mov	lr, pc
   13d64:	e594f000 	ldr	pc, [r4]
   13d68:	e8bd8010 	pop	{r4, pc}
   13d6c:	3c00a180 	.word	0x3c00a180
   13d70:	3c00a5cc 	.word	0x3c00a5cc
   13d74:	000197ab 	.word	0x000197ab
   13d78:	3c00a181 	.word	0x3c00a181
   13d7c:	000197b9 	.word	0x000197b9
   13d80:	3c00a184 	.word	0x3c00a184
   13d84:	000197cc 	.word	0x000197cc
   13d88:	3c00a188 	.word	0x3c00a188
   13d8c:	000197de 	.word	0x000197de
   13d90:	3c00a189 	.word	0x3c00a189
   13d94:	000197f1 	.word	0x000197f1
   13d98:	3c00a18a 	.word	0x3c00a18a
   13d9c:	00019808 	.word	0x00019808
   13da0:	3c00a034 	.word	0x3c00a034
   13da4:	00019820 	.word	0x00019820
   13da8:	3c00a18b 	.word	0x3c00a18b
   13dac:	0001982f 	.word	0x0001982f
   13db0:	3c00a18c 	.word	0x3c00a18c
   13db4:	00019848 	.word	0x00019848
   13db8:	3c00a18d 	.word	0x3c00a18d
   13dbc:	0001985a 	.word	0x0001985a

00013dc0 <Procpacket>:
   13dc0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   13dc4:	e59fc1ac 	ldr	ip, [pc, #428]	; 13f78 <Procpacket+0x1b8>
   13dc8:	e59f21ac 	ldr	r2, [pc, #428]	; 13f7c <Procpacket+0x1bc>
   13dcc:	e5dc3000 	ldrb	r3, [ip]
   13dd0:	e1a06000 	mov	r6, r0
   13dd4:	e5c03000 	strb	r3, [r0]
   13dd8:	e5923000 	ldr	r3, [r2]
   13ddc:	e1a05001 	mov	r5, r1
   13de0:	e5c03001 	strb	r3, [r0, #1]
   13de4:	e5923000 	ldr	r3, [r2]
   13de8:	e1e03003 	mvn	r3, r3
   13dec:	e5c03002 	strb	r3, [r0, #2]
   13df0:	e59f3188 	ldr	r3, [pc, #392]	; 13f80 <Procpacket+0x1c0>
   13df4:	e5922000 	ldr	r2, [r2]
   13df8:	e5933000 	ldr	r3, [r3]
   13dfc:	e1520003 	cmp	r2, r3
   13e00:	1a000008 	bne	13e28 <Procpacket+0x68>
   13e04:	e59f3178 	ldr	r3, [pc, #376]	; 13f84 <Procpacket+0x1c4>
   13e08:	e5d32000 	ldrb	r2, [r3]
   13e0c:	e59f3174 	ldr	r3, [pc, #372]	; 13f88 <Procpacket+0x1c8>
   13e10:	e3520001 	cmp	r2, #1	; 0x1
   13e14:	e5934000 	ldr	r4, [r3]
   13e18:	e5cc2000 	strb	r2, [ip]
   13e1c:	03a07080 	moveq	r7, #128	; 0x80
   13e20:	13a07b01 	movne	r7, #1024	; 0x400
   13e24:	ea000004 	b	13e3c <Procpacket+0x7c>
   13e28:	e5dc3000 	ldrb	r3, [ip]
   13e2c:	e3530001 	cmp	r3, #1	; 0x1
   13e30:	13a04b01 	movne	r4, #1024	; 0x400
   13e34:	03a04080 	moveq	r4, #128	; 0x80
   13e38:	e1a07004 	mov	r7, r4
   13e3c:	e59f3148 	ldr	r3, [pc, #328]	; 13f8c <Procpacket+0x1cc>
   13e40:	e5d33000 	ldrb	r3, [r3]
   13e44:	e3530000 	cmp	r3, #0	; 0x0
   13e48:	1a000008 	bne	13e70 <Procpacket+0xb0>
   13e4c:	e59f313c 	ldr	r3, [pc, #316]	; 13f90 <Procpacket+0x1d0>
   13e50:	e2860003 	add	r0, r6, #3	; 0x3
   13e54:	e5931000 	ldr	r1, [r3]
   13e58:	e59f3134 	ldr	r3, [pc, #308]	; 13f94 <Procpacket+0x1d4>
   13e5c:	e1a02004 	mov	r2, r4
   13e60:	e5933000 	ldr	r3, [r3]
   13e64:	e0631001 	rsb	r1, r3, r1
   13e68:	e0851001 	add	r1, r5, r1
   13e6c:	ebffb57c 	bl	1464 <memcpy>
   13e70:	e59f111c 	ldr	r1, [pc, #284]	; 13f94 <Procpacket+0x1d4>
   13e74:	e59f311c 	ldr	r3, [pc, #284]	; 13f98 <Procpacket+0x1d8>
   13e78:	e5912000 	ldr	r2, [r1]
   13e7c:	e5d33000 	ldrb	r3, [r3]
   13e80:	e0642002 	rsb	r2, r4, r2
   13e84:	e3530000 	cmp	r3, #0	; 0x0
   13e88:	e5812000 	str	r2, [r1]
   13e8c:	0a000006 	beq	13eac <Procpacket+0xec>
   13e90:	e1a02007 	mov	r2, r7
   13e94:	e2860003 	add	r0, r6, #3	; 0x3
   13e98:	e3a01000 	mov	r1, #0	; 0x0
   13e9c:	ebffb568 	bl	1444 <memset>
   13ea0:	e59f30f4 	ldr	r3, [pc, #244]	; 13f9c <Procpacket+0x1dc>
   13ea4:	e3a02001 	mov	r2, #1	; 0x1
   13ea8:	e5c32000 	strb	r2, [r3]
   13eac:	e59f30c8 	ldr	r3, [pc, #200]	; 13f7c <Procpacket+0x1bc>
   13eb0:	e5932000 	ldr	r2, [r3]
   13eb4:	e59f30c4 	ldr	r3, [pc, #196]	; 13f80 <Procpacket+0x1c0>
   13eb8:	e5933000 	ldr	r3, [r3]
   13ebc:	e1520003 	cmp	r2, r3
   13ec0:	1a000004 	bne	13ed8 <Procpacket+0x118>
   13ec4:	e2840003 	add	r0, r4, #3	; 0x3
   13ec8:	e0860000 	add	r0, r6, r0
   13ecc:	e0642007 	rsb	r2, r4, r7
   13ed0:	e3a0101a 	mov	r1, #26	; 0x1a
   13ed4:	ebffb55a 	bl	1444 <memset>
   13ed8:	e3a00000 	mov	r0, #0	; 0x0
   13edc:	e59f80bc 	ldr	r8, [pc, #188]	; 13fa0 <Procpacket+0x1e0>
   13ee0:	e1a04006 	mov	r4, r6
   13ee4:	e1a0c000 	mov	ip, r0
   13ee8:	e1a05000 	mov	r5, r0
   13eec:	e1a0342c 	lsr	r3, ip, #8
   13ef0:	e5d42003 	ldrb	r2, [r4, #3]
   13ef4:	e1a03083 	lsl	r3, r3, #1
   13ef8:	e19810b3 	ldrh	r1, [r8, r3]
   13efc:	e2800001 	add	r0, r0, #1	; 0x1
   13f00:	e182340c 	orr	r3, r2, ip, lsl #8
   13f04:	e0233001 	eor	r3, r3, r1
   13f08:	e0852002 	add	r2, r5, r2
   13f0c:	e1500007 	cmp	r0, r7
   13f10:	e6ffc073 	uxth	ip, r3
   13f14:	e6ef5072 	uxtb	r5, r2
   13f18:	e2844001 	add	r4, r4, #1	; 0x1
   13f1c:	e59f107c 	ldr	r1, [pc, #124]	; 13fa0 <Procpacket+0x1e0>
   13f20:	3afffff1 	bcc	13eec <Procpacket+0x12c>
   13f24:	e59f3078 	ldr	r3, [pc, #120]	; 13fa4 <Procpacket+0x1e4>
   13f28:	e0860007 	add	r0, r6, r7
   13f2c:	e5d33000 	ldrb	r3, [r3]
   13f30:	e3530000 	cmp	r3, #0	; 0x0
   13f34:	05c05003 	strbeq	r5, [r0, #3]
   13f38:	0a00000c 	beq	13f70 <Procpacket+0x1b0>
   13f3c:	e1a0342c 	lsr	r3, ip, #8
   13f40:	e1a03083 	lsl	r3, r3, #1
   13f44:	e19120b3 	ldrh	r2, [r1, r3]
   13f48:	e022240c 	eor	r2, r2, ip, lsl #8
   13f4c:	e1a03442 	asr	r3, r2, #8
   13f50:	e20330ff 	and	r3, r3, #255	; 0xff
   13f54:	e1a03083 	lsl	r3, r3, #1
   13f58:	e19130b3 	ldrh	r3, [r1, r3]
   13f5c:	e0233402 	eor	r3, r3, r2, lsl #8
   13f60:	e6ff3073 	uxth	r3, r3
   13f64:	e1a02423 	lsr	r2, r3, #8
   13f68:	e5c03004 	strb	r3, [r0, #4]
   13f6c:	e5c02003 	strb	r2, [r0, #3]
   13f70:	e3a00000 	mov	r0, #0	; 0x0
   13f74:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13f78:	3c00a182 	.word	0x3c00a182
   13f7c:	3c00a184 	.word	0x3c00a184
   13f80:	3c00ac94 	.word	0x3c00ac94
   13f84:	3c00ac9c 	.word	0x3c00ac9c
   13f88:	3c00ac98 	.word	0x3c00ac98
   13f8c:	3c00a188 	.word	0x3c00a188
   13f90:	3c00a62c 	.word	0x3c00a62c
   13f94:	3c00a630 	.word	0x3c00a630
   13f98:	3c00a18d 	.word	0x3c00a18d
   13f9c:	3c00a18c 	.word	0x3c00a18c
   13fa0:	0001635c 	.word	0x0001635c
   13fa4:	3c00a628 	.word	0x3c00a628

00013fa8 <procsend>:
   13fa8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   13fac:	e59f31f8 	ldr	r3, [pc, #504]	; 141ac <procsend+0x204>
   13fb0:	e1a06000 	mov	r6, r0
   13fb4:	e5d30000 	ldrb	r0, [r3]
   13fb8:	e24dd004 	sub	sp, sp, #4	; 0x4
   13fbc:	e3500000 	cmp	r0, #0	; 0x0
   13fc0:	e1a0c002 	mov	ip, r2
   13fc4:	e59f41e4 	ldr	r4, [pc, #484]	; 141b0 <procsend+0x208>
   13fc8:	0a000065 	beq	14164 <procsend+0x1bc>
   13fcc:	e59f31e0 	ldr	r3, [pc, #480]	; 141b4 <procsend+0x20c>
   13fd0:	e1a02b02 	lsl	r2, r2, #22
   13fd4:	e1a02b22 	lsr	r2, r2, #22
   13fd8:	e583c000 	str	ip, [r3]
   13fdc:	e59f31d4 	ldr	r3, [pc, #468]	; 141b8 <procsend+0x210>
   13fe0:	e59f01d4 	ldr	r0, [pc, #468]	; 141bc <procsend+0x214>
   13fe4:	e3520000 	cmp	r2, #0	; 0x0
   13fe8:	e1a0152c 	lsr	r1, ip, #10
   13fec:	e583c000 	str	ip, [r3]
   13ff0:	03a03b01 	moveq	r3, #1024	; 0x400
   13ff4:	e5802000 	str	r2, [r0]
   13ff8:	e5841000 	str	r1, [r4]
   13ffc:	05803000 	streq	r3, [r0]
   14000:	0a000006 	beq	14020 <procsend+0x78>
   14004:	e2813001 	add	r3, r1, #1	; 0x1
   14008:	e352007f 	cmp	r2, #127	; 0x7f
   1400c:	e59f21ac 	ldr	r2, [pc, #428]	; 141c0 <procsend+0x218>
   14010:	e5843000 	str	r3, [r4]
   14014:	93a03001 	movls	r3, #1	; 0x1
   14018:	83a03002 	movhi	r3, #2	; 0x2
   1401c:	e5c23000 	strb	r3, [r2]
   14020:	e59f019c 	ldr	r0, [pc, #412]	; 141c4 <procsend+0x21c>
   14024:	ebffb4f4 	bl	13fc <strlen>
   14028:	e59f2198 	ldr	r2, [pc, #408]	; 141c8 <procsend+0x220>
   1402c:	e3500080 	cmp	r0, #128	; 0x80
   14030:	83a03002 	movhi	r3, #2	; 0x2
   14034:	93a03001 	movls	r3, #1	; 0x1
   14038:	85c23000 	strbhi	r3, [r2]
   1403c:	95c23000 	strbls	r3, [r2]
   14040:	e59f3184 	ldr	r3, [pc, #388]	; 141cc <procsend+0x224>
   14044:	83a07b01 	movhi	r7, #1024	; 0x400
   14048:	e5d31000 	ldrb	r1, [r3]
   1404c:	93a07080 	movls	r7, #128	; 0x80
   14050:	e3510000 	cmp	r1, #0	; 0x0
   14054:	13a01000 	movne	r1, #0	; 0x0
   14058:	12872005 	addne	r2, r7, #5	; 0x5
   1405c:	02872004 	addeq	r2, r7, #4	; 0x4
   14060:	11a00006 	movne	r0, r6
   14064:	01a00006 	moveq	r0, r6
   14068:	ebffb4f5 	bl	1444 <memset>
   1406c:	e59f3154 	ldr	r3, [pc, #340]	; 141c8 <procsend+0x220>
   14070:	e3a05000 	mov	r5, #0	; 0x0
   14074:	e5d33000 	ldrb	r3, [r3]
   14078:	e59f0144 	ldr	r0, [pc, #324]	; 141c4 <procsend+0x21c>
   1407c:	e5c63000 	strb	r3, [r6]
   14080:	e3e03000 	mvn	r3, #0	; 0x0
   14084:	e5c63002 	strb	r3, [r6, #2]
   14088:	e5c65001 	strb	r5, [r6, #1]
   1408c:	ebffb4da 	bl	13fc <strlen>
   14090:	e59f112c 	ldr	r1, [pc, #300]	; 141c4 <procsend+0x21c>
   14094:	e1a02000 	mov	r2, r0
   14098:	e1a04000 	mov	r4, r0
   1409c:	e2860003 	add	r0, r6, #3	; 0x3
   140a0:	ebffb485 	bl	12bc <strncpy>
   140a4:	e59f3108 	ldr	r3, [pc, #264]	; 141b4 <procsend+0x20c>
   140a8:	e2844004 	add	r4, r4, #4	; 0x4
   140ac:	e0860004 	add	r0, r6, r4
   140b0:	e5932000 	ldr	r2, [r3]
   140b4:	e59f1114 	ldr	r1, [pc, #276]	; 141d0 <procsend+0x228>
   140b8:	e59f4114 	ldr	r4, [pc, #276]	; 141d4 <procsend+0x22c>
   140bc:	ebfffab8 	bl	12ba4 <sprintf>
   140c0:	e1a00006 	mov	r0, r6
   140c4:	e1a0c005 	mov	ip, r5
   140c8:	e1a0e005 	mov	lr, r5
   140cc:	e1a0342c 	lsr	r3, ip, #8
   140d0:	e5d02003 	ldrb	r2, [r0, #3]
   140d4:	e1a03083 	lsl	r3, r3, #1
   140d8:	e19410b3 	ldrh	r1, [r4, r3]
   140dc:	e2855001 	add	r5, r5, #1	; 0x1
   140e0:	e182340c 	orr	r3, r2, ip, lsl #8
   140e4:	e0233001 	eor	r3, r3, r1
   140e8:	e08e2002 	add	r2, lr, r2
   140ec:	e1550007 	cmp	r5, r7
   140f0:	e6ffc073 	uxth	ip, r3
   140f4:	e6efe072 	uxtb	lr, r2
   140f8:	e2800001 	add	r0, r0, #1	; 0x1
   140fc:	e59f10d0 	ldr	r1, [pc, #208]	; 141d4 <procsend+0x22c>
   14100:	3afffff1 	bcc	140cc <procsend+0x124>
   14104:	e59f30c0 	ldr	r3, [pc, #192]	; 141cc <procsend+0x224>
   14108:	e0860007 	add	r0, r6, r7
   1410c:	e5d33000 	ldrb	r3, [r3]
   14110:	e3530000 	cmp	r3, #0	; 0x0
   14114:	05c0e003 	strbeq	lr, [r0, #3]
   14118:	0a00000c 	beq	14150 <procsend+0x1a8>
   1411c:	e1a0342c 	lsr	r3, ip, #8
   14120:	e1a03083 	lsl	r3, r3, #1
   14124:	e19120b3 	ldrh	r2, [r1, r3]
   14128:	e022240c 	eor	r2, r2, ip, lsl #8
   1412c:	e1a03442 	asr	r3, r2, #8
   14130:	e20330ff 	and	r3, r3, #255	; 0xff
   14134:	e1a03083 	lsl	r3, r3, #1
   14138:	e19130b3 	ldrh	r3, [r1, r3]
   1413c:	e0233402 	eor	r3, r3, r2, lsl #8
   14140:	e6ff3073 	uxth	r3, r3
   14144:	e1a02423 	lsr	r2, r3, #8
   14148:	e5c03004 	strb	r3, [r0, #4]
   1414c:	e5c02003 	strb	r2, [r0, #3]
   14150:	e59f3080 	ldr	r3, [pc, #128]	; 141d8 <procsend+0x230>
   14154:	e3a02001 	mov	r2, #1	; 0x1
   14158:	e5c32000 	strb	r2, [r3]
   1415c:	e3a00000 	mov	r0, #0	; 0x0
   14160:	ea00000f 	b	141a4 <procsend+0x1fc>
   14164:	e59f2070 	ldr	r2, [pc, #112]	; 141dc <procsend+0x234>
   14168:	e5943000 	ldr	r3, [r4]
   1416c:	e5922000 	ldr	r2, [r2]
   14170:	e1520003 	cmp	r2, r3
   14174:	8a00000a 	bhi	141a4 <procsend+0x1fc>
   14178:	e59f3060 	ldr	r3, [pc, #96]	; 141e0 <procsend+0x238>
   1417c:	e59f2044 	ldr	r2, [pc, #68]	; 141c8 <procsend+0x220>
   14180:	e5d33000 	ldrb	r3, [r3]
   14184:	e1a00006 	mov	r0, r6
   14188:	e3530000 	cmp	r3, #0	; 0x0
   1418c:	13a03001 	movne	r3, #1	; 0x1
   14190:	03a03002 	moveq	r3, #2	; 0x2
   14194:	e5c23000 	strb	r3, [r2]
   14198:	e1a0200c 	mov	r2, ip
   1419c:	ebffff07 	bl	13dc0 <Procpacket>
   141a0:	e6ef0070 	uxtb	r0, r0
   141a4:	e28dd004 	add	sp, sp, #4	; 0x4
   141a8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   141ac:	3c00a034 	.word	0x3c00a034
   141b0:	3c00ac94 	.word	0x3c00ac94
   141b4:	3c00a630 	.word	0x3c00a630
   141b8:	3c00a62c 	.word	0x3c00a62c
   141bc:	3c00ac98 	.word	0x3c00ac98
   141c0:	3c00ac9c 	.word	0x3c00ac9c
   141c4:	00019869 	.word	0x00019869
   141c8:	3c00a182 	.word	0x3c00a182
   141cc:	3c00a628 	.word	0x3c00a628
   141d0:	00018978 	.word	0x00018978
   141d4:	0001635c 	.word	0x0001635c
   141d8:	3c00a181 	.word	0x3c00a181
   141dc:	3c00a184 	.word	0x3c00a184
   141e0:	3c00a18d 	.word	0x3c00a18d

000141e4 <CYGACC_COMM_IF_PUTC>:
   141e4:	e1a00001 	mov	r0, r1
   141e8:	eafffaf7 	b	12dcc <putc>

000141ec <real_send>:
   141ec:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   141f0:	e59f30ec 	ldr	r3, [pc, #236]	; 142e4 <real_send+0xf8>
   141f4:	e24ddb01 	sub	sp, sp, #1024	; 0x400
   141f8:	e5d33000 	ldrb	r3, [r3]
   141fc:	e24dd00c 	sub	sp, sp, #12	; 0xc
   14200:	e3530000 	cmp	r3, #0	; 0x0
   14204:	e1a0e000 	mov	lr, r0
   14208:	e1a0c001 	mov	ip, r1
   1420c:	159f30d4 	ldrne	r3, [pc, #212]	; 142e8 <real_send+0xfc>
   14210:	13a02000 	movne	r2, #0	; 0x0
   14214:	15832000 	strne	r2, [r3]
   14218:	1a000006 	bne	14238 <real_send+0x4c>
   1421c:	e59f30c8 	ldr	r3, [pc, #200]	; 142ec <real_send+0x100>
   14220:	e5d33000 	ldrb	r3, [r3]
   14224:	e3530000 	cmp	r3, #0	; 0x0
   14228:	059f20b8 	ldreq	r2, [pc, #184]	; 142e8 <real_send+0xfc>
   1422c:	05923000 	ldreq	r3, [r2]
   14230:	02833001 	addeq	r3, r3, #1	; 0x1
   14234:	05823000 	streq	r3, [r2]
   14238:	e28d0008 	add	r0, sp, #8	; 0x8
   1423c:	e2400005 	sub	r0, r0, #5	; 0x5
   14240:	e1a0100e 	mov	r1, lr
   14244:	e1a0200c 	mov	r2, ip
   14248:	ebffff56 	bl	13fa8 <procsend>
   1424c:	e2507000 	subs	r7, r0, #0	; 0x0
   14250:	1a00001f 	bne	142d4 <real_send+0xe8>
   14254:	e59f3094 	ldr	r3, [pc, #148]	; 142f0 <real_send+0x104>
   14258:	e28d5008 	add	r5, sp, #8	; 0x8
   1425c:	e5d31000 	ldrb	r1, [r3]
   14260:	e59f308c 	ldr	r3, [pc, #140]	; 142f4 <real_send+0x108>
   14264:	e3510001 	cmp	r1, #1	; 0x1
   14268:	e5d32000 	ldrb	r2, [r3]
   1426c:	e3003404 	movw	r3, #1028	; 0x404
   14270:	11a06003 	movne	r6, r3
   14274:	03a06084 	moveq	r6, #132	; 0x84
   14278:	e3520000 	cmp	r2, #0	; 0x0
   1427c:	12866001 	addne	r6, r6, #1	; 0x1
   14280:	e2455005 	sub	r5, r5, #5	; 0x5
   14284:	e3a04000 	mov	r4, #0	; 0x0
   14288:	e7d50004 	ldrb	r0, [r5, r4]
   1428c:	e2844001 	add	r4, r4, #1	; 0x1
   14290:	ebfffacd 	bl	12dcc <putc>
   14294:	e1540006 	cmp	r4, r6
   14298:	3afffffa 	bcc	14288 <real_send+0x9c>
   1429c:	e59f3054 	ldr	r3, [pc, #84]	; 142f8 <real_send+0x10c>
   142a0:	e5d33000 	ldrb	r3, [r3]
   142a4:	e3530000 	cmp	r3, #0	; 0x0
   142a8:	13e07000 	mvnne	r7, #0	; 0x0
   142ac:	1a000008 	bne	142d4 <real_send+0xe8>
   142b0:	e59f3030 	ldr	r3, [pc, #48]	; 142e8 <real_send+0xfc>
   142b4:	e5932000 	ldr	r2, [r3]
   142b8:	e59f303c 	ldr	r3, [pc, #60]	; 142fc <real_send+0x110>
   142bc:	e5933000 	ldr	r3, [r3]
   142c0:	e1520003 	cmp	r2, r3
   142c4:	059f3034 	ldreq	r3, [pc, #52]	; 14300 <real_send+0x114>
   142c8:	03a02001 	moveq	r2, #1	; 0x1
   142cc:	05c32000 	strbeq	r2, [r3]
   142d0:	03a07003 	moveq	r7, #3	; 0x3
   142d4:	e1a00007 	mov	r0, r7
   142d8:	e28dd00c 	add	sp, sp, #12	; 0xc
   142dc:	e28ddb01 	add	sp, sp, #1024	; 0x400
   142e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   142e4:	3c00a18d 	.word	0x3c00a18d
   142e8:	3c00a184 	.word	0x3c00a184
   142ec:	3c00a034 	.word	0x3c00a034
   142f0:	3c00a182 	.word	0x3c00a182
   142f4:	3c00a628 	.word	0x3c00a628
   142f8:	3c00a18c 	.word	0x3c00a18c
   142fc:	3c00ac94 	.word	0x3c00ac94
   14300:	3c00a188 	.word	0x3c00a188

00014304 <CYGACC_COMM_IF_GETC_TIMEOUT>:
   14304:	e92d4070 	push	{r4, r5, r6, lr}
   14308:	e1a05001 	mov	r5, r1
   1430c:	e3a04000 	mov	r4, #0	; 0x0
   14310:	e59f603c 	ldr	r6, [pc, #60]	; 14354 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
   14314:	ea000000 	b	1431c <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
   14318:	ebffe8dc 	bl	e690 <udelay>
   1431c:	ebfffaab 	bl	12dd0 <tstc>
   14320:	e3500000 	cmp	r0, #0	; 0x0
   14324:	e3a00014 	mov	r0, #20	; 0x14
   14328:	1a000002 	bne	14338 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
   1432c:	e1540006 	cmp	r4, r6
   14330:	e2844001 	add	r4, r4, #1	; 0x1
   14334:	1afffff7 	bne	14318 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
   14338:	ebfffaa4 	bl	12dd0 <tstc>
   1433c:	e3500000 	cmp	r0, #0	; 0x0
   14340:	08bd8070 	popeq	{r4, r5, r6, pc}
   14344:	ebfffaa2 	bl	12dd4 <getc>
   14348:	e5c50000 	strb	r0, [r5]
   1434c:	e3a00001 	mov	r0, #1	; 0x1
   14350:	e8bd8070 	pop	{r4, r5, r6, pc}
   14354:	000186a0 	.word	0x000186a0

00014358 <control_send>:
   14358:	e59f323c 	ldr	r3, [pc, #572]	; 1459c <control_send+0x244>
   1435c:	e3a02000 	mov	r2, #0	; 0x0
   14360:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   14364:	e5c32000 	strb	r2, [r3]
   14368:	e59f3230 	ldr	r3, [pc, #560]	; 145a0 <control_send+0x248>
   1436c:	e59f4230 	ldr	r4, [pc, #560]	; 145a4 <control_send+0x24c>
   14370:	e5c32000 	strb	r2, [r3]
   14374:	e59f322c 	ldr	r3, [pc, #556]	; 145a8 <control_send+0x250>
   14378:	e59f522c 	ldr	r5, [pc, #556]	; 145ac <control_send+0x254>
   1437c:	e5832000 	str	r2, [r3]
   14380:	e3a03001 	mov	r3, #1	; 0x1
   14384:	e59fe224 	ldr	lr, [pc, #548]	; 145b0 <control_send+0x258>
   14388:	e59f8224 	ldr	r8, [pc, #548]	; 145b4 <control_send+0x25c>
   1438c:	e59f6224 	ldr	r6, [pc, #548]	; 145b8 <control_send+0x260>
   14390:	e59f7224 	ldr	r7, [pc, #548]	; 145bc <control_send+0x264>
   14394:	e59fc224 	ldr	ip, [pc, #548]	; 145c0 <control_send+0x268>
   14398:	e5c43000 	strb	r3, [r4]
   1439c:	e59f3220 	ldr	r3, [pc, #544]	; 145c4 <control_send+0x26c>
   143a0:	e24dd014 	sub	sp, sp, #20	; 0x14
   143a4:	e5c52000 	strb	r2, [r5]
   143a8:	e5c32000 	strb	r2, [r3]
   143ac:	e58d0004 	str	r0, [sp, #4]
   143b0:	e1a0b001 	mov	fp, r1
   143b4:	e5c82000 	strb	r2, [r8]
   143b8:	e5c62000 	strb	r2, [r6]
   143bc:	e5c72000 	strb	r2, [r7]
   143c0:	e5ce2000 	strb	r2, [lr]
   143c4:	e5cc2000 	strb	r2, [ip]
   143c8:	e1a0a002 	mov	sl, r2
   143cc:	e1a0900e 	mov	r9, lr
   143d0:	e59f51f0 	ldr	r5, [pc, #496]	; 145c8 <control_send+0x270>
   143d4:	ea000000 	b	143dc <control_send+0x84>
   143d8:	e3a0a000 	mov	sl, #0	; 0x0
   143dc:	e3a00000 	mov	r0, #0	; 0x0
   143e0:	e28d100f 	add	r1, sp, #15	; 0xf
   143e4:	ebffffc6 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   143e8:	e3500000 	cmp	r0, #0	; 0x0
   143ec:	0afffffa 	beq	143dc <control_send+0x84>
   143f0:	e5dd300f 	ldrb	r3, [sp, #15]
   143f4:	e3530018 	cmp	r3, #24	; 0x18
   143f8:	0a000042 	beq	14508 <control_send+0x1b0>
   143fc:	8a000004 	bhi	14414 <control_send+0xbc>
   14400:	e3530006 	cmp	r3, #6	; 0x6
   14404:	0a00002c 	beq	144bc <control_send+0x164>
   14408:	e3530015 	cmp	r3, #21	; 0x15
   1440c:	1afffff1 	bne	143d8 <control_send+0x80>
   14410:	ea000010 	b	14458 <control_send+0x100>
   14414:	e3530043 	cmp	r3, #67	; 0x43
   14418:	0a000001 	beq	14424 <control_send+0xcc>
   1441c:	e3530063 	cmp	r3, #99	; 0x63
   14420:	1affffec 	bne	143d8 <control_send+0x80>
   14424:	e5d73000 	ldrb	r3, [r7]
   14428:	e5d42000 	ldrb	r2, [r4]
   1442c:	e5d61000 	ldrb	r1, [r6]
   14430:	e1833002 	orr	r3, r3, r2
   14434:	e1913003 	orrs	r3, r1, r3
   14438:	0affffe6 	beq	143d8 <control_send+0x80>
   1443c:	e3510000 	cmp	r1, #0	; 0x0
   14440:	159f2178 	ldrne	r2, [pc, #376]	; 145c0 <control_send+0x268>
   14444:	13a03001 	movne	r3, #1	; 0x1
   14448:	15c23000 	strbne	r3, [r2]
   1444c:	e3a03001 	mov	r3, #1	; 0x1
   14450:	e5c53000 	strb	r3, [r5]
   14454:	ea00002f 	b	14518 <control_send+0x1c0>
   14458:	e59f313c 	ldr	r3, [pc, #316]	; 1459c <control_send+0x244>
   1445c:	e5d33000 	ldrb	r3, [r3]
   14460:	e3530000 	cmp	r3, #0	; 0x0
   14464:	0a000008 	beq	1448c <control_send+0x134>
   14468:	e59f2138 	ldr	r2, [pc, #312]	; 145a8 <control_send+0x250>
   1446c:	e5923000 	ldr	r3, [r2]
   14470:	e2433001 	sub	r3, r3, #1	; 0x1
   14474:	e5823000 	str	r3, [r2]
   14478:	e3a03001 	mov	r3, #1	; 0x1
   1447c:	e5c43000 	strb	r3, [r4]
   14480:	e3a03000 	mov	r3, #0	; 0x0
   14484:	e5c93000 	strb	r3, [r9]
   14488:	ea000022 	b	14518 <control_send+0x1c0>
   1448c:	e5d42000 	ldrb	r2, [r4]
   14490:	e3520000 	cmp	r2, #0	; 0x0
   14494:	15c53000 	strbne	r3, [r5]
   14498:	1a000007 	bne	144bc <control_send+0x164>
   1449c:	e5d93000 	ldrb	r3, [r9]
   144a0:	e3530000 	cmp	r3, #0	; 0x0
   144a4:	15c92000 	strbne	r2, [r9]
   144a8:	e59f20f8 	ldr	r2, [pc, #248]	; 145a8 <control_send+0x250>
   144ac:	e5923000 	ldr	r3, [r2]
   144b0:	e2433001 	sub	r3, r3, #1	; 0x1
   144b4:	e5823000 	str	r3, [r2]
   144b8:	ea000016 	b	14518 <control_send+0x1c0>
   144bc:	e5d42000 	ldrb	r2, [r4]
   144c0:	e3520000 	cmp	r2, #0	; 0x0
   144c4:	1affffc3 	bne	143d8 <control_send+0x80>
   144c8:	e5d91000 	ldrb	r1, [r9]
   144cc:	e3510000 	cmp	r1, #0	; 0x0
   144d0:	0a000005 	beq	144ec <control_send+0x194>
   144d4:	e59f30c0 	ldr	r3, [pc, #192]	; 1459c <control_send+0x244>
   144d8:	e1a0a002 	mov	sl, r2
   144dc:	e5c32000 	strb	r2, [r3]
   144e0:	e3a03001 	mov	r3, #1	; 0x1
   144e4:	e5c73000 	strb	r3, [r7]
   144e8:	eaffffbb 	b	143dc <control_send+0x84>
   144ec:	e5d83000 	ldrb	r3, [r8]
   144f0:	e3530000 	cmp	r3, #0	; 0x0
   144f4:	13a03001 	movne	r3, #1	; 0x1
   144f8:	15c63000 	strbne	r3, [r6]
   144fc:	11a0a001 	movne	sl, r1
   14500:	1affffb5 	bne	143dc <control_send+0x84>
   14504:	ea000003 	b	14518 <control_send+0x1c0>
   14508:	e28aa001 	add	sl, sl, #1	; 0x1
   1450c:	e35a0002 	cmp	sl, #2	; 0x2
   14510:	1affffb1 	bne	143dc <control_send+0x84>
   14514:	ea00001d 	b	14590 <control_send+0x238>
   14518:	e59f20a0 	ldr	r2, [pc, #160]	; 145c0 <control_send+0x268>
   1451c:	e5d23000 	ldrb	r3, [r2]
   14520:	e3530000 	cmp	r3, #0	; 0x0
   14524:	1a000008 	bne	1454c <control_send+0x1f4>
   14528:	e59f207c 	ldr	r2, [pc, #124]	; 145ac <control_send+0x254>
   1452c:	e5d23000 	ldrb	r3, [r2]
   14530:	e3530000 	cmp	r3, #0	; 0x0
   14534:	0a000004 	beq	1454c <control_send+0x1f4>
   14538:	e3a00004 	mov	r0, #4	; 0x4
   1453c:	ebfffa22 	bl	12dcc <putc>
   14540:	e3a03001 	mov	r3, #1	; 0x1
   14544:	e5c83000 	strb	r3, [r8]
   14548:	eaffffa2 	b	143d8 <control_send+0x80>
   1454c:	e59d0004 	ldr	r0, [sp, #4]
   14550:	e1a0100b 	mov	r1, fp
   14554:	ebffff24 	bl	141ec <real_send>
   14558:	e3500003 	cmp	r0, #3	; 0x3
   1455c:	0affff9d 	beq	143d8 <control_send+0x80>
   14560:	e3500000 	cmp	r0, #0	; 0x0
   14564:	1a00000a 	bne	14594 <control_send+0x23c>
   14568:	e5d93000 	ldrb	r3, [r9]
   1456c:	e3530000 	cmp	r3, #0	; 0x0
   14570:	e5d43000 	ldrb	r3, [r4]
   14574:	15c90000 	strbne	r0, [r9]
   14578:	e3530000 	cmp	r3, #0	; 0x0
   1457c:	13a03000 	movne	r3, #0	; 0x0
   14580:	15c43000 	strbne	r3, [r4]
   14584:	12833001 	addne	r3, r3, #1	; 0x1
   14588:	15c93000 	strbne	r3, [r9]
   1458c:	eaffff91 	b	143d8 <control_send+0x80>
   14590:	e3a00000 	mov	r0, #0	; 0x0
   14594:	e28dd014 	add	sp, sp, #20	; 0x14
   14598:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   1459c:	3c00a181 	.word	0x3c00a181
   145a0:	3c00a182 	.word	0x3c00a182
   145a4:	3c00a034 	.word	0x3c00a034
   145a8:	3c00a184 	.word	0x3c00a184
   145ac:	3c00a188 	.word	0x3c00a188
   145b0:	3c00a18b 	.word	0x3c00a18b
   145b4:	3c00a180 	.word	0x3c00a180
   145b8:	3c00a189 	.word	0x3c00a189
   145bc:	3c00a18a 	.word	0x3c00a18a
   145c0:	3c00a18d 	.word	0x3c00a18d
   145c4:	3c00a18c 	.word	0x3c00a18c
   145c8:	3c00a628 	.word	0x3c00a628

000145cc <xyzModem_stream_terminate>:
   145cc:	e92d4030 	push	{r4, r5, lr}
   145d0:	e3500000 	cmp	r0, #0	; 0x0
   145d4:	e24dd00c 	sub	sp, sp, #12	; 0xc
   145d8:	e1a04001 	mov	r4, r1
   145dc:	0a000020 	beq	14664 <xyzModem_stream_terminate+0x98>
   145e0:	e59f4098 	ldr	r4, [pc, #152]	; 14680 <xyzModem_stream_terminate+0xb4>
   145e4:	e5943414 	ldr	r3, [r4, #1044]
   145e8:	e2433001 	sub	r3, r3, #1	; 0x1
   145ec:	e3530001 	cmp	r3, #1	; 0x1
   145f0:	8a000020 	bhi	14678 <xyzModem_stream_terminate+0xac>
   145f4:	e3a00018 	mov	r0, #24	; 0x18
   145f8:	ebfff9f3 	bl	12dcc <putc>
   145fc:	e3a00018 	mov	r0, #24	; 0x18
   14600:	ebfff9f1 	bl	12dcc <putc>
   14604:	e3a00018 	mov	r0, #24	; 0x18
   14608:	ebfff9ef 	bl	12dcc <putc>
   1460c:	e3a00018 	mov	r0, #24	; 0x18
   14610:	ebfff9ed 	bl	12dcc <putc>
   14614:	e3a00008 	mov	r0, #8	; 0x8
   14618:	ebfff9eb 	bl	12dcc <putc>
   1461c:	e3a00008 	mov	r0, #8	; 0x8
   14620:	ebfff9e9 	bl	12dcc <putc>
   14624:	e3a00008 	mov	r0, #8	; 0x8
   14628:	ebfff9e7 	bl	12dcc <putc>
   1462c:	e3a00008 	mov	r0, #8	; 0x8
   14630:	ebfff9e5 	bl	12dcc <putc>
   14634:	e1a05004 	mov	r5, r4
   14638:	e28d4007 	add	r4, sp, #7	; 0x7
   1463c:	e5953000 	ldr	r3, [r5]
   14640:	e1a01004 	mov	r1, r4
   14644:	e5d30000 	ldrb	r0, [r3]
   14648:	ebffff2d 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   1464c:	e3500000 	cmp	r0, #0	; 0x0
   14650:	1afffff9 	bne	1463c <xyzModem_stream_terminate+0x70>
   14654:	e59f3024 	ldr	r3, [pc, #36]	; 14680 <xyzModem_stream_terminate+0xb4>
   14658:	e3a02001 	mov	r2, #1	; 0x1
   1465c:	e583242c 	str	r2, [r3, #1068]
   14660:	ea000004 	b	14678 <xyzModem_stream_terminate+0xac>
   14664:	e12fff34 	blx	r4
   14668:	e3500000 	cmp	r0, #0	; 0x0
   1466c:	aafffffc 	bge	14664 <xyzModem_stream_terminate+0x98>
   14670:	e59f000c 	ldr	r0, [pc, #12]	; 14684 <xyzModem_stream_terminate+0xb8>
   14674:	ebffe805 	bl	e690 <udelay>
   14678:	e28dd00c 	add	sp, sp, #12	; 0xc
   1467c:	e8bd8030 	pop	{r4, r5, pc}
   14680:	3c00a190 	.word	0x3c00a190
   14684:	0003d090 	.word	0x0003d090

00014688 <xyzModem_get_hdr>:
   14688:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   1468c:	e59f42a0 	ldr	r4, [pc, #672]	; 14934 <xyzModem_get_hdr+0x2ac>
   14690:	e24dd00c 	sub	sp, sp, #12	; 0xc
   14694:	e5943430 	ldr	r3, [r4, #1072]
   14698:	e3530000 	cmp	r3, #0	; 0x0
   1469c:	0a000003 	beq	146b0 <xyzModem_get_hdr+0x28>
   146a0:	e3a00006 	mov	r0, #6	; 0x6
   146a4:	ebfff9c8 	bl	12dcc <putc>
   146a8:	e3a03000 	mov	r3, #0	; 0x0
   146ac:	e5843430 	str	r3, [r4, #1072]
   146b0:	e3a04000 	mov	r4, #0	; 0x0
   146b4:	e59f6278 	ldr	r6, [pc, #632]	; 14934 <xyzModem_get_hdr+0x2ac>
   146b8:	e1a05004 	mov	r5, r4
   146bc:	e28d7007 	add	r7, sp, #7	; 0x7
   146c0:	e5963000 	ldr	r3, [r6]
   146c4:	e1a01007 	mov	r1, r7
   146c8:	e5d30000 	ldrb	r0, [r3]
   146cc:	ebffff0c 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   146d0:	e3500000 	cmp	r0, #0	; 0x0
   146d4:	e2855001 	add	r5, r5, #1	; 0x1
   146d8:	e59f2254 	ldr	r2, [pc, #596]	; 14934 <xyzModem_get_hdr+0x2ac>
   146dc:	0a000021 	beq	14768 <xyzModem_get_hdr+0xe0>
   146e0:	e5dd3007 	ldrb	r3, [sp, #7]
   146e4:	e3530002 	cmp	r3, #2	; 0x2
   146e8:	0a00000c 	beq	14720 <xyzModem_get_hdr+0x98>
   146ec:	8a000002 	bhi	146fc <xyzModem_get_hdr+0x74>
   146f0:	e3530001 	cmp	r3, #1	; 0x1
   146f4:	1afffff1 	bne	146c0 <xyzModem_get_hdr+0x38>
   146f8:	ea000004 	b	14710 <xyzModem_get_hdr+0x88>
   146fc:	e3530004 	cmp	r3, #4	; 0x4
   14700:	0a000012 	beq	14750 <xyzModem_get_hdr+0xc8>
   14704:	e3530018 	cmp	r3, #24	; 0x18
   14708:	1affffec 	bne	146c0 <xyzModem_get_hdr+0x38>
   1470c:	ea000007 	b	14730 <xyzModem_get_hdr+0xa8>
   14710:	e592341c 	ldr	r3, [r2, #1052]
   14714:	e2833001 	add	r3, r3, #1	; 0x1
   14718:	e582341c 	str	r3, [r2, #1052]
   1471c:	ea00007c 	b	14914 <xyzModem_get_hdr+0x28c>
   14720:	e5923420 	ldr	r3, [r2, #1056]
   14724:	e2833001 	add	r3, r3, #1	; 0x1
   14728:	e5823420 	str	r3, [r2, #1056]
   1472c:	ea000078 	b	14914 <xyzModem_get_hdr+0x28c>
   14730:	e5963424 	ldr	r3, [r6, #1060]
   14734:	e2844001 	add	r4, r4, #1	; 0x1
   14738:	e2833001 	add	r3, r3, #1	; 0x1
   1473c:	e3540003 	cmp	r4, #3	; 0x3
   14740:	e5863424 	str	r3, [r6, #1060]
   14744:	1affffdd 	bne	146c0 <xyzModem_get_hdr+0x38>
   14748:	e3e00004 	mvn	r0, #4	; 0x4
   1474c:	ea00006e 	b	1490c <xyzModem_get_hdr+0x284>
   14750:	e3550001 	cmp	r5, #1	; 0x1
   14754:	1affffd9 	bne	146c0 <xyzModem_get_hdr+0x38>
   14758:	e3a00006 	mov	r0, #6	; 0x6
   1475c:	ebfff99a 	bl	12dcc <putc>
   14760:	e3e00003 	mvn	r0, #3	; 0x3
   14764:	ea000068 	b	1490c <xyzModem_get_hdr+0x284>
   14768:	e1a05002 	mov	r5, r2
   1476c:	e28d4006 	add	r4, sp, #6	; 0x6
   14770:	e5953000 	ldr	r3, [r5]
   14774:	e1a01004 	mov	r1, r4
   14778:	e5d30000 	ldrb	r0, [r3]
   1477c:	ebfffee0 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14780:	e3500000 	cmp	r0, #0	; 0x0
   14784:	1afffff9 	bne	14770 <xyzModem_get_hdr+0xe8>
   14788:	e59f01a8 	ldr	r0, [pc, #424]	; 14938 <xyzModem_get_hdr+0x2b0>
   1478c:	ebffe7bf 	bl	e690 <udelay>
   14790:	ea00005a 	b	14900 <xyzModem_get_hdr+0x278>
   14794:	e5943000 	ldr	r3, [r4]
   14798:	e59f119c 	ldr	r1, [pc, #412]	; 1493c <xyzModem_get_hdr+0x2b4>
   1479c:	e5d30000 	ldrb	r0, [r3]
   147a0:	ebfffed7 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   147a4:	e3500000 	cmp	r0, #0	; 0x0
   147a8:	0a000054 	beq	14900 <xyzModem_get_hdr+0x278>
   147ac:	e28d5008 	add	r5, sp, #8	; 0x8
   147b0:	e5753001 	ldrb	r3, [r5, #-1]!
   147b4:	e2842004 	add	r2, r4, #4	; 0x4
   147b8:	e3530001 	cmp	r3, #1	; 0x1
   147bc:	03a03080 	moveq	r3, #128	; 0x80
   147c0:	13a03b01 	movne	r3, #1024	; 0x400
   147c4:	e5843410 	str	r3, [r4, #1040]
   147c8:	e5842404 	str	r2, [r4, #1028]
   147cc:	e1a07004 	mov	r7, r4
   147d0:	e3a04000 	mov	r4, #0	; 0x0
   147d4:	ea000008 	b	147fc <xyzModem_get_hdr+0x174>
   147d8:	e5973000 	ldr	r3, [r7]
   147dc:	e5d30000 	ldrb	r0, [r3]
   147e0:	ebfffec7 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   147e4:	e3500000 	cmp	r0, #0	; 0x0
   147e8:	e0872004 	add	r2, r7, r4
   147ec:	e2844001 	add	r4, r4, #1	; 0x1
   147f0:	0a000042 	beq	14900 <xyzModem_get_hdr+0x278>
   147f4:	e5dd3007 	ldrb	r3, [sp, #7]
   147f8:	e5c23004 	strb	r3, [r2, #4]
   147fc:	e5973410 	ldr	r3, [r7, #1040]
   14800:	e1a01005 	mov	r1, r5
   14804:	e1540003 	cmp	r4, r3
   14808:	e59f6124 	ldr	r6, [pc, #292]	; 14934 <xyzModem_get_hdr+0x2ac>
   1480c:	bafffff1 	blt	147d8 <xyzModem_get_hdr+0x150>
   14810:	e5963000 	ldr	r3, [r6]
   14814:	e59f1124 	ldr	r1, [pc, #292]	; 14940 <xyzModem_get_hdr+0x2b8>
   14818:	e5d30000 	ldrb	r0, [r3]
   1481c:	ebfffeb8 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14820:	e3500000 	cmp	r0, #0	; 0x0
   14824:	0a000035 	beq	14900 <xyzModem_get_hdr+0x278>
   14828:	e5963428 	ldr	r3, [r6, #1064]
   1482c:	e3530000 	cmp	r3, #0	; 0x0
   14830:	0a000005 	beq	1484c <xyzModem_get_hdr+0x1c4>
   14834:	e5963000 	ldr	r3, [r6]
   14838:	e59f1104 	ldr	r1, [pc, #260]	; 14944 <xyzModem_get_hdr+0x2bc>
   1483c:	e5d30000 	ldrb	r0, [r3]
   14840:	ebfffeaf 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14844:	e3500000 	cmp	r0, #0	; 0x0
   14848:	0a00002c 	beq	14900 <xyzModem_get_hdr+0x278>
   1484c:	e59f40e0 	ldr	r4, [pc, #224]	; 14934 <xyzModem_get_hdr+0x2ac>
   14850:	e5d43409 	ldrb	r3, [r4, #1033]
   14854:	e5d42408 	ldrb	r2, [r4, #1032]
   14858:	e0233002 	eor	r3, r3, r2
   1485c:	e35300ff 	cmp	r3, #255	; 0xff
   14860:	0a000009 	beq	1488c <xyzModem_get_hdr+0x204>
   14864:	e1a05004 	mov	r5, r4
   14868:	e28d4006 	add	r4, sp, #6	; 0x6
   1486c:	e5953000 	ldr	r3, [r5]
   14870:	e1a01004 	mov	r1, r4
   14874:	e5d30000 	ldrb	r0, [r3]
   14878:	ebfffea1 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   1487c:	e3500000 	cmp	r0, #0	; 0x0
   14880:	1afffff9 	bne	1486c <xyzModem_get_hdr+0x1e4>
   14884:	e3e00005 	mvn	r0, #5	; 0x5
   14888:	ea00001f 	b	1490c <xyzModem_get_hdr+0x284>
   1488c:	e5943428 	ldr	r3, [r4, #1064]
   14890:	e3530000 	cmp	r3, #0	; 0x0
   14894:	01a02003 	moveq	r2, r3
   14898:	01a0c004 	moveq	ip, r4
   1489c:	05940410 	ldreq	r0, [r4, #1040]
   148a0:	01a01003 	moveq	r1, r3
   148a4:	0a00000a 	beq	148d4 <xyzModem_get_hdr+0x24c>
   148a8:	e2840004 	add	r0, r4, #4	; 0x4
   148ac:	e5941410 	ldr	r1, [r4, #1040]
   148b0:	eb000110 	bl	14cf8 <cyg_crc16>
   148b4:	e5d4340b 	ldrb	r3, [r4, #1035]
   148b8:	e5d4240a 	ldrb	r2, [r4, #1034]
   148bc:	e1833402 	orr	r3, r3, r2, lsl #8
   148c0:	e1500003 	cmp	r0, r3
   148c4:	ea00000a 	b	148f4 <xyzModem_get_hdr+0x26c>
   148c8:	e5d33004 	ldrb	r3, [r3, #4]
   148cc:	e0813003 	add	r3, r1, r3
   148d0:	e6ff1073 	uxth	r1, r3
   148d4:	e1520000 	cmp	r2, r0
   148d8:	e08c3002 	add	r3, ip, r2
   148dc:	e2822001 	add	r2, r2, #1	; 0x1
   148e0:	bafffff8 	blt	148c8 <xyzModem_get_hdr+0x240>
   148e4:	e59f3048 	ldr	r3, [pc, #72]	; 14934 <xyzModem_get_hdr+0x2ac>
   148e8:	e5d3240a 	ldrb	r2, [r3, #1034]
   148ec:	e20130ff 	and	r3, r1, #255	; 0xff
   148f0:	e1520003 	cmp	r2, r3
   148f4:	13e00006 	mvnne	r0, #6	; 0x6
   148f8:	1a000003 	bne	1490c <xyzModem_get_hdr+0x284>
   148fc:	ea000001 	b	14908 <xyzModem_get_hdr+0x280>
   14900:	e3e00002 	mvn	r0, #2	; 0x2
   14904:	ea000000 	b	1490c <xyzModem_get_hdr+0x284>
   14908:	e3a00000 	mov	r0, #0	; 0x0
   1490c:	e28dd00c 	add	sp, sp, #12	; 0xc
   14910:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   14914:	e59f4018 	ldr	r4, [pc, #24]	; 14934 <xyzModem_get_hdr+0x2ac>
   14918:	e59f1028 	ldr	r1, [pc, #40]	; 14948 <xyzModem_get_hdr+0x2c0>
   1491c:	e5943000 	ldr	r3, [r4]
   14920:	e5d30000 	ldrb	r0, [r3]
   14924:	ebfffe76 	bl	14304 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14928:	e3500000 	cmp	r0, #0	; 0x0
   1492c:	0afffff3 	beq	14900 <xyzModem_get_hdr+0x278>
   14930:	eaffff97 	b	14794 <xyzModem_get_hdr+0x10c>
   14934:	3c00a190 	.word	0x3c00a190
   14938:	0003d090 	.word	0x0003d090
   1493c:	3c00a599 	.word	0x3c00a599
   14940:	3c00a59a 	.word	0x3c00a59a
   14944:	3c00a59b 	.word	0x3c00a59b
   14948:	3c00a598 	.word	0x3c00a598

0001494c <xyzModem_stream_read>:
   1494c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   14950:	e59f5240 	ldr	r5, [pc, #576]	; 14b98 <xyzModem_stream_read+0x24c>
   14954:	e24dd004 	sub	sp, sp, #4	; 0x4
   14958:	e1a08000 	mov	r8, r0
   1495c:	e1a07001 	mov	r7, r1
   14960:	e1a0b002 	mov	fp, r2
   14964:	e3e06004 	mvn	r6, #4	; 0x4
   14968:	e3a0a000 	mov	sl, #0	; 0x0
   1496c:	e1a09005 	mov	r9, r5
   14970:	ea00007e 	b	14b70 <xyzModem_stream_read+0x224>
   14974:	e5953410 	ldr	r3, [r5, #1040]
   14978:	e3530000 	cmp	r3, #0	; 0x0
   1497c:	03a04014 	moveq	r4, #20	; 0x14
   14980:	0a00005d 	beq	14afc <xyzModem_stream_read+0x1b0>
   14984:	ea000065 	b	14b20 <xyzModem_stream_read+0x1d4>
   14988:	ebffff3e 	bl	14688 <xyzModem_get_hdr>
   1498c:	e2506000 	subs	r6, r0, #0	; 0x0
   14990:	1a000039 	bne	14a7c <xyzModem_stream_read+0x130>
   14994:	e5d5240c 	ldrb	r2, [r5, #1036]
   14998:	e5d51408 	ldrb	r1, [r5, #1032]
   1499c:	e2423001 	sub	r3, r2, #1	; 0x1
   149a0:	e1510002 	cmp	r1, r2
   149a4:	e20330ff 	and	r3, r3, #255	; 0xff
   149a8:	1a00002d 	bne	14a64 <xyzModem_stream_read+0x118>
   149ac:	e599c414 	ldr	ip, [r9, #1044]
   149b0:	e2813001 	add	r3, r1, #1	; 0x1
   149b4:	e5c9340c 	strb	r3, [r9, #1036]
   149b8:	e35c0001 	cmp	ip, #1	; 0x1
   149bc:	e3a03001 	mov	r3, #1	; 0x1
   149c0:	e5893430 	str	r3, [r9, #1072]
   149c4:	0a000002 	beq	149d4 <xyzModem_stream_read+0x88>
   149c8:	e5993434 	ldr	r3, [r9, #1076]
   149cc:	e3530000 	cmp	r3, #0	; 0x0
   149d0:	1a000017 	bne	14a34 <xyzModem_stream_read+0xe8>
   149d4:	e5953410 	ldr	r3, [r5, #1040]
   149d8:	e5950404 	ldr	r0, [r5, #1028]
   149dc:	e0802003 	add	r2, r0, r3
   149e0:	e5523001 	ldrb	r3, [r2, #-1]
   149e4:	e353001a 	cmp	r3, #26	; 0x1a
   149e8:	1a00000f 	bne	14a2c <xyzModem_stream_read+0xe0>
   149ec:	e5523002 	ldrb	r3, [r2, #-2]
   149f0:	e353001a 	cmp	r3, #26	; 0x1a
   149f4:	1a00000c 	bne	14a2c <xyzModem_stream_read+0xe0>
   149f8:	e5523003 	ldrb	r3, [r2, #-3]
   149fc:	e353001a 	cmp	r3, #26	; 0x1a
   14a00:	1a000009 	bne	14a2c <xyzModem_stream_read+0xe0>
   14a04:	ea000000 	b	14a0c <xyzModem_stream_read+0xc0>
   14a08:	e5851410 	str	r1, [r5, #1040]
   14a0c:	e5953410 	ldr	r3, [r5, #1040]
   14a10:	e3530000 	cmp	r3, #0	; 0x0
   14a14:	e0802003 	add	r2, r0, r3
   14a18:	e2431001 	sub	r1, r3, #1	; 0x1
   14a1c:	0a000002 	beq	14a2c <xyzModem_stream_read+0xe0>
   14a20:	e5523001 	ldrb	r3, [r2, #-1]
   14a24:	e353001a 	cmp	r3, #26	; 0x1a
   14a28:	0afffff6 	beq	14a08 <xyzModem_stream_read+0xbc>
   14a2c:	e35c0001 	cmp	ip, #1	; 0x1
   14a30:	0a00003a 	beq	14b20 <xyzModem_stream_read+0x1d4>
   14a34:	e5951434 	ldr	r1, [r5, #1076]
   14a38:	e3510000 	cmp	r1, #0	; 0x0
   14a3c:	0a000037 	beq	14b20 <xyzModem_stream_read+0x1d4>
   14a40:	e5953438 	ldr	r3, [r5, #1080]
   14a44:	e5950410 	ldr	r0, [r5, #1040]
   14a48:	e0802003 	add	r2, r0, r3
   14a4c:	e1520001 	cmp	r2, r1
   14a50:	80803001 	addhi	r3, r0, r1
   14a54:	80623003 	rsbhi	r3, r2, r3
   14a58:	e5852438 	str	r2, [r5, #1080]
   14a5c:	85853410 	strhi	r3, [r5, #1040]
   14a60:	ea00002e 	b	14b20 <xyzModem_stream_read+0x1d4>
   14a64:	e1510003 	cmp	r1, r3
   14a68:	13e06007 	mvnne	r6, #7	; 0x7
   14a6c:	1a00001a 	bne	14adc <xyzModem_stream_read+0x190>
   14a70:	e3a00006 	mov	r0, #6	; 0x6
   14a74:	ebfff8d4 	bl	12dcc <putc>
   14a78:	ea00001f 	b	14afc <xyzModem_stream_read+0x1b0>
   14a7c:	e3760005 	cmn	r6, #5	; 0x5
   14a80:	0a000021 	beq	14b0c <xyzModem_stream_read+0x1c0>
   14a84:	e3760004 	cmn	r6, #4	; 0x4
   14a88:	1a000013 	bne	14adc <xyzModem_stream_read+0x190>
   14a8c:	e3a00006 	mov	r0, #6	; 0x6
   14a90:	ebfff8cd 	bl	12dcc <putc>
   14a94:	e5953414 	ldr	r3, [r5, #1044]
   14a98:	e3530002 	cmp	r3, #2	; 0x2
   14a9c:	1a00000b 	bne	14ad0 <xyzModem_stream_read+0x184>
   14aa0:	e5950428 	ldr	r0, [r5, #1064]
   14aa4:	e3500000 	cmp	r0, #0	; 0x0
   14aa8:	13a00043 	movne	r0, #67	; 0x43
   14aac:	03a00015 	moveq	r0, #21	; 0x15
   14ab0:	ebfff8c5 	bl	12dcc <putc>
   14ab4:	e5953418 	ldr	r3, [r5, #1048]
   14ab8:	e2833001 	add	r3, r3, #1	; 0x1
   14abc:	e5853418 	str	r3, [r5, #1048]
   14ac0:	ebfffef0 	bl	14688 <xyzModem_get_hdr>
   14ac4:	e1a06000 	mov	r6, r0
   14ac8:	e3a00006 	mov	r0, #6	; 0x6
   14acc:	ebfff8be 	bl	12dcc <putc>
   14ad0:	e3a03001 	mov	r3, #1	; 0x1
   14ad4:	e585342c 	str	r3, [r5, #1068]
   14ad8:	ea000009 	b	14b04 <xyzModem_stream_read+0x1b8>
   14adc:	e5950428 	ldr	r0, [r5, #1064]
   14ae0:	e3500000 	cmp	r0, #0	; 0x0
   14ae4:	13a00043 	movne	r0, #67	; 0x43
   14ae8:	03a00015 	moveq	r0, #21	; 0x15
   14aec:	ebfff8b6 	bl	12dcc <putc>
   14af0:	e5953418 	ldr	r3, [r5, #1048]
   14af4:	e2833001 	add	r3, r3, #1	; 0x1
   14af8:	e5853418 	str	r3, [r5, #1048]
   14afc:	e2544001 	subs	r4, r4, #1	; 0x1
   14b00:	2affffa0 	bcs	14988 <xyzModem_stream_read+0x3c>
   14b04:	e3560000 	cmp	r6, #0	; 0x0
   14b08:	aa000004 	bge	14b20 <xyzModem_stream_read+0x1d4>
   14b0c:	e59f3084 	ldr	r3, [pc, #132]	; 14b98 <xyzModem_stream_read+0x24c>
   14b10:	e3e02000 	mvn	r2, #0	; 0x0
   14b14:	e58b6000 	str	r6, [fp]
   14b18:	e5832410 	str	r2, [r3, #1040]
   14b1c:	ea00001a 	b	14b8c <xyzModem_stream_read+0x240>
   14b20:	e595342c 	ldr	r3, [r5, #1068]
   14b24:	e3530000 	cmp	r3, #0	; 0x0
   14b28:	1a000010 	bne	14b70 <xyzModem_stream_read+0x224>
   14b2c:	e5954410 	ldr	r4, [r5, #1040]
   14b30:	e1a00008 	mov	r0, r8
   14b34:	e1570004 	cmp	r7, r4
   14b38:	b1a04007 	movlt	r4, r7
   14b3c:	a1a04004 	movge	r4, r4
   14b40:	e1a02004 	mov	r2, r4
   14b44:	e5951404 	ldr	r1, [r5, #1028]
   14b48:	ebffb245 	bl	1464 <memcpy>
   14b4c:	e5953410 	ldr	r3, [r5, #1040]
   14b50:	e5952404 	ldr	r2, [r5, #1028]
   14b54:	e0643003 	rsb	r3, r4, r3
   14b58:	e0822004 	add	r2, r2, r4
   14b5c:	e5853410 	str	r3, [r5, #1040]
   14b60:	e5852404 	str	r2, [r5, #1028]
   14b64:	e0647007 	rsb	r7, r4, r7
   14b68:	e0888004 	add	r8, r8, r4
   14b6c:	e08aa004 	add	sl, sl, r4
   14b70:	e595342c 	ldr	r3, [r5, #1068]
   14b74:	e2733001 	rsbs	r3, r3, #1	; 0x1
   14b78:	33a03000 	movcc	r3, #0	; 0x0
   14b7c:	e3570000 	cmp	r7, #0	; 0x0
   14b80:	d3a03000 	movle	r3, #0	; 0x0
   14b84:	e3530000 	cmp	r3, #0	; 0x0
   14b88:	1affff79 	bne	14974 <xyzModem_stream_read+0x28>
   14b8c:	e1a0000a 	mov	r0, sl
   14b90:	e28dd004 	add	sp, sp, #4	; 0x4
   14b94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   14b98:	3c00a190 	.word	0x3c00a190

00014b9c <xyzModem_stream_open>:
   14b9c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   14ba0:	e59f4140 	ldr	r4, [pc, #320]	; 14ce8 <xyzModem_stream_open+0x14c>
   14ba4:	e24dd00c 	sub	sp, sp, #12	; 0xc
   14ba8:	e3a05000 	mov	r5, #0	; 0x0
   14bac:	e28d3008 	add	r3, sp, #8	; 0x8
   14bb0:	e3a02001 	mov	r2, #1	; 0x1
   14bb4:	e5235004 	str	r5, [r3, #-4]!
   14bb8:	e5842428 	str	r2, [r4, #1064]
   14bbc:	e5845410 	str	r5, [r4, #1040]
   14bc0:	e584542c 	str	r5, [r4, #1068]
   14bc4:	e5845430 	str	r5, [r4, #1072]
   14bc8:	e5902004 	ldr	r2, [r0, #4]
   14bcc:	e5843000 	str	r3, [r4]
   14bd0:	e3a00043 	mov	r0, #67	; 0x43
   14bd4:	e5842414 	str	r2, [r4, #1044]
   14bd8:	e5845418 	str	r5, [r4, #1048]
   14bdc:	e584541c 	str	r5, [r4, #1052]
   14be0:	e5845420 	str	r5, [r4, #1056]
   14be4:	e5845424 	str	r5, [r4, #1060]
   14be8:	e5845438 	str	r5, [r4, #1080]
   14bec:	e5845434 	str	r5, [r4, #1076]
   14bf0:	e1a08001 	mov	r8, r1
   14bf4:	ebfff874 	bl	12dcc <putc>
   14bf8:	e5943414 	ldr	r3, [r4, #1044]
   14bfc:	e3530001 	cmp	r3, #1	; 0x1
   14c00:	05c4340c 	strbeq	r3, [r4, #1036]
   14c04:	01a00005 	moveq	r0, r5
   14c08:	0a000034 	beq	14ce0 <xyzModem_stream_open+0x144>
   14c0c:	e1a07004 	mov	r7, r4
   14c10:	e3a06014 	mov	r6, #20	; 0x14
   14c14:	e3a0400a 	mov	r4, #10	; 0xa
   14c18:	e1a0a005 	mov	sl, r5
   14c1c:	ea00002b 	b	14cd0 <xyzModem_stream_open+0x134>
   14c20:	ebfffe98 	bl	14688 <xyzModem_get_hdr>
   14c24:	e2505000 	subs	r5, r0, #0	; 0x0
   14c28:	1a000016 	bne	14c88 <xyzModem_stream_open+0xec>
   14c2c:	e59f20b4 	ldr	r2, [pc, #180]	; 14ce8 <xyzModem_stream_open+0x14c>
   14c30:	e5d23408 	ldrb	r3, [r2, #1032]
   14c34:	e3530000 	cmp	r3, #0	; 0x0
   14c38:	1a00000b 	bne	14c6c <xyzModem_stream_open+0xd0>
   14c3c:	e1a03002 	mov	r3, r2
   14c40:	e5930404 	ldr	r0, [r3, #1028]
   14c44:	e4d02001 	ldrb	r2, [r0], #1
   14c48:	e3520000 	cmp	r2, #0	; 0x0
   14c4c:	e5830404 	str	r0, [r3, #1028]
   14c50:	1afffffa 	bne	14c40 <xyzModem_stream_open+0xa4>
   14c54:	e59f3090 	ldr	r3, [pc, #144]	; 14cec <xyzModem_stream_open+0x150>
   14c58:	e59f1090 	ldr	r1, [pc, #144]	; 14cf0 <xyzModem_stream_open+0x154>
   14c5c:	ebfffb5a 	bl	139cc <parse_num>
   14c60:	e59f3080 	ldr	r3, [pc, #128]	; 14ce8 <xyzModem_stream_open+0x14c>
   14c64:	e3a02001 	mov	r2, #1	; 0x1
   14c68:	e5832430 	str	r2, [r3, #1072]
   14c6c:	e59f3074 	ldr	r3, [pc, #116]	; 14ce8 <xyzModem_stream_open+0x14c>
   14c70:	e3a01000 	mov	r1, #0	; 0x0
   14c74:	e3a02001 	mov	r2, #1	; 0x1
   14c78:	e5c3240c 	strb	r2, [r3, #1036]
   14c7c:	e5831410 	str	r1, [r3, #1040]
   14c80:	e1a00001 	mov	r0, r1
   14c84:	ea000015 	b	14ce0 <xyzModem_stream_open+0x144>
   14c88:	e3750003 	cmn	r5, #3	; 0x3
   14c8c:	1a00000d 	bne	14cc8 <xyzModem_stream_open+0x12c>
   14c90:	e2444001 	sub	r4, r4, #1	; 0x1
   14c94:	e3540000 	cmp	r4, #0	; 0x0
   14c98:	e59f0054 	ldr	r0, [pc, #84]	; 14cf4 <xyzModem_stream_open+0x158>
   14c9c:	d587a428 	strle	sl, [r7, #1064]
   14ca0:	ebffe67a 	bl	e690 <udelay>
   14ca4:	e5970428 	ldr	r0, [r7, #1064]
   14ca8:	e3500000 	cmp	r0, #0	; 0x0
   14cac:	13a00043 	movne	r0, #67	; 0x43
   14cb0:	03a00015 	moveq	r0, #21	; 0x15
   14cb4:	ebfff844 	bl	12dcc <putc>
   14cb8:	e5973418 	ldr	r3, [r7, #1048]
   14cbc:	e2833001 	add	r3, r3, #1	; 0x1
   14cc0:	e5873418 	str	r3, [r7, #1048]
   14cc4:	ea000001 	b	14cd0 <xyzModem_stream_open+0x134>
   14cc8:	e3750005 	cmn	r5, #5	; 0x5
   14ccc:	0a000001 	beq	14cd8 <xyzModem_stream_open+0x13c>
   14cd0:	e2566001 	subs	r6, r6, #1	; 0x1
   14cd4:	2affffd1 	bcs	14c20 <xyzModem_stream_open+0x84>
   14cd8:	e5885000 	str	r5, [r8]
   14cdc:	e3e00000 	mvn	r0, #0	; 0x0
   14ce0:	e28dd00c 	add	sp, sp, #12	; 0xc
   14ce4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   14ce8:	3c00a190 	.word	0x3c00a190
   14cec:	0001962d 	.word	0x0001962d
   14cf0:	3c00a5c4 	.word	0x3c00a5c4
   14cf4:	0007a120 	.word	0x0007a120

00014cf8 <cyg_crc16>:
   14cf8:	e3a02000 	mov	r2, #0	; 0x0
   14cfc:	e92d4010 	push	{r4, lr}
   14d00:	e1a0c000 	mov	ip, r0
   14d04:	e59f402c 	ldr	r4, [pc, #44]	; 14d38 <cyg_crc16+0x40>
   14d08:	e1a00002 	mov	r0, r2
   14d0c:	ea000006 	b	14d2c <cyg_crc16+0x34>
   14d10:	e7dc3002 	ldrb	r3, [ip, r2]
   14d14:	e2822001 	add	r2, r2, #1	; 0x1
   14d18:	e0233420 	eor	r3, r3, r0, lsr #8
   14d1c:	e1a03083 	lsl	r3, r3, #1
   14d20:	e19430b3 	ldrh	r3, [r4, r3]
   14d24:	e0233400 	eor	r3, r3, r0, lsl #8
   14d28:	e6ff0073 	uxth	r0, r3
   14d2c:	e1520001 	cmp	r2, r1
   14d30:	bafffff6 	blt	14d10 <cyg_crc16+0x18>
   14d34:	e8bd8010 	pop	{r4, pc}
   14d38:	0001635c 	.word	0x0001635c

00014d3c <ss_jtag_en>:
   14d3c:	e59f300c 	ldr	r3, [pc, #12]	; 14d50 <ss_jtag_en+0x14>
   14d40:	e2500000 	subs	r0, r0, #0	; 0x0
   14d44:	13a00001 	movne	r0, #1	; 0x1
   14d48:	e58308b8 	str	r0, [r3, #2232]
   14d4c:	e12fff1e 	bx	lr
   14d50:	21e08000 	.word	0x21e08000

00014d54 <raise>:
   14d54:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   14d58:	e1a01000 	mov	r1, r0
   14d5c:	e24dd004 	sub	sp, sp, #4	; 0x4
   14d60:	e59f0014 	ldr	r0, [pc, #20]	; 14d7c <raise+0x28>
   14d64:	e59f3014 	ldr	r3, [pc, #20]	; 14d80 <raise+0x2c>
   14d68:	e1a0e00f 	mov	lr, pc
   14d6c:	e593f000 	ldr	pc, [r3]
   14d70:	e3a00000 	mov	r0, #0	; 0x0
   14d74:	e28dd004 	add	sp, sp, #4	; 0x4
   14d78:	e8bd8000 	pop	{pc}
   14d7c:	00019871 	.word	0x00019871
   14d80:	3c00a5cc 	.word	0x3c00a5cc

00014d84 <__aeabi_uidiv>:
   14d84:	e2512001 	subs	r2, r1, #1	; 0x1
   14d88:	012fff1e 	bxeq	lr
   14d8c:	3a000074 	bcc	14f64 <__aeabi_uidiv+0x1e0>
   14d90:	e1500001 	cmp	r0, r1
   14d94:	9a00006b 	bls	14f48 <__aeabi_uidiv+0x1c4>
   14d98:	e1110002 	tst	r1, r2
   14d9c:	0a00006c 	beq	14f54 <__aeabi_uidiv+0x1d0>
   14da0:	e16f3f10 	clz	r3, r0
   14da4:	e16f2f11 	clz	r2, r1
   14da8:	e0423003 	sub	r3, r2, r3
   14dac:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   14db0:	10833083 	addne	r3, r3, r3, lsl #1
   14db4:	e3a02000 	mov	r2, #0	; 0x0
   14db8:	108ff103 	addne	pc, pc, r3, lsl #2
   14dbc:	e1a00000 	nop			(mov r0,r0)
   14dc0:	e1500f81 	cmp	r0, r1, lsl #31
   14dc4:	e0a22002 	adc	r2, r2, r2
   14dc8:	20400f81 	subcs	r0, r0, r1, lsl #31
   14dcc:	e1500f01 	cmp	r0, r1, lsl #30
   14dd0:	e0a22002 	adc	r2, r2, r2
   14dd4:	20400f01 	subcs	r0, r0, r1, lsl #30
   14dd8:	e1500e81 	cmp	r0, r1, lsl #29
   14ddc:	e0a22002 	adc	r2, r2, r2
   14de0:	20400e81 	subcs	r0, r0, r1, lsl #29
   14de4:	e1500e01 	cmp	r0, r1, lsl #28
   14de8:	e0a22002 	adc	r2, r2, r2
   14dec:	20400e01 	subcs	r0, r0, r1, lsl #28
   14df0:	e1500d81 	cmp	r0, r1, lsl #27
   14df4:	e0a22002 	adc	r2, r2, r2
   14df8:	20400d81 	subcs	r0, r0, r1, lsl #27
   14dfc:	e1500d01 	cmp	r0, r1, lsl #26
   14e00:	e0a22002 	adc	r2, r2, r2
   14e04:	20400d01 	subcs	r0, r0, r1, lsl #26
   14e08:	e1500c81 	cmp	r0, r1, lsl #25
   14e0c:	e0a22002 	adc	r2, r2, r2
   14e10:	20400c81 	subcs	r0, r0, r1, lsl #25
   14e14:	e1500c01 	cmp	r0, r1, lsl #24
   14e18:	e0a22002 	adc	r2, r2, r2
   14e1c:	20400c01 	subcs	r0, r0, r1, lsl #24
   14e20:	e1500b81 	cmp	r0, r1, lsl #23
   14e24:	e0a22002 	adc	r2, r2, r2
   14e28:	20400b81 	subcs	r0, r0, r1, lsl #23
   14e2c:	e1500b01 	cmp	r0, r1, lsl #22
   14e30:	e0a22002 	adc	r2, r2, r2
   14e34:	20400b01 	subcs	r0, r0, r1, lsl #22
   14e38:	e1500a81 	cmp	r0, r1, lsl #21
   14e3c:	e0a22002 	adc	r2, r2, r2
   14e40:	20400a81 	subcs	r0, r0, r1, lsl #21
   14e44:	e1500a01 	cmp	r0, r1, lsl #20
   14e48:	e0a22002 	adc	r2, r2, r2
   14e4c:	20400a01 	subcs	r0, r0, r1, lsl #20
   14e50:	e1500981 	cmp	r0, r1, lsl #19
   14e54:	e0a22002 	adc	r2, r2, r2
   14e58:	20400981 	subcs	r0, r0, r1, lsl #19
   14e5c:	e1500901 	cmp	r0, r1, lsl #18
   14e60:	e0a22002 	adc	r2, r2, r2
   14e64:	20400901 	subcs	r0, r0, r1, lsl #18
   14e68:	e1500881 	cmp	r0, r1, lsl #17
   14e6c:	e0a22002 	adc	r2, r2, r2
   14e70:	20400881 	subcs	r0, r0, r1, lsl #17
   14e74:	e1500801 	cmp	r0, r1, lsl #16
   14e78:	e0a22002 	adc	r2, r2, r2
   14e7c:	20400801 	subcs	r0, r0, r1, lsl #16
   14e80:	e1500781 	cmp	r0, r1, lsl #15
   14e84:	e0a22002 	adc	r2, r2, r2
   14e88:	20400781 	subcs	r0, r0, r1, lsl #15
   14e8c:	e1500701 	cmp	r0, r1, lsl #14
   14e90:	e0a22002 	adc	r2, r2, r2
   14e94:	20400701 	subcs	r0, r0, r1, lsl #14
   14e98:	e1500681 	cmp	r0, r1, lsl #13
   14e9c:	e0a22002 	adc	r2, r2, r2
   14ea0:	20400681 	subcs	r0, r0, r1, lsl #13
   14ea4:	e1500601 	cmp	r0, r1, lsl #12
   14ea8:	e0a22002 	adc	r2, r2, r2
   14eac:	20400601 	subcs	r0, r0, r1, lsl #12
   14eb0:	e1500581 	cmp	r0, r1, lsl #11
   14eb4:	e0a22002 	adc	r2, r2, r2
   14eb8:	20400581 	subcs	r0, r0, r1, lsl #11
   14ebc:	e1500501 	cmp	r0, r1, lsl #10
   14ec0:	e0a22002 	adc	r2, r2, r2
   14ec4:	20400501 	subcs	r0, r0, r1, lsl #10
   14ec8:	e1500481 	cmp	r0, r1, lsl #9
   14ecc:	e0a22002 	adc	r2, r2, r2
   14ed0:	20400481 	subcs	r0, r0, r1, lsl #9
   14ed4:	e1500401 	cmp	r0, r1, lsl #8
   14ed8:	e0a22002 	adc	r2, r2, r2
   14edc:	20400401 	subcs	r0, r0, r1, lsl #8
   14ee0:	e1500381 	cmp	r0, r1, lsl #7
   14ee4:	e0a22002 	adc	r2, r2, r2
   14ee8:	20400381 	subcs	r0, r0, r1, lsl #7
   14eec:	e1500301 	cmp	r0, r1, lsl #6
   14ef0:	e0a22002 	adc	r2, r2, r2
   14ef4:	20400301 	subcs	r0, r0, r1, lsl #6
   14ef8:	e1500281 	cmp	r0, r1, lsl #5
   14efc:	e0a22002 	adc	r2, r2, r2
   14f00:	20400281 	subcs	r0, r0, r1, lsl #5
   14f04:	e1500201 	cmp	r0, r1, lsl #4
   14f08:	e0a22002 	adc	r2, r2, r2
   14f0c:	20400201 	subcs	r0, r0, r1, lsl #4
   14f10:	e1500181 	cmp	r0, r1, lsl #3
   14f14:	e0a22002 	adc	r2, r2, r2
   14f18:	20400181 	subcs	r0, r0, r1, lsl #3
   14f1c:	e1500101 	cmp	r0, r1, lsl #2
   14f20:	e0a22002 	adc	r2, r2, r2
   14f24:	20400101 	subcs	r0, r0, r1, lsl #2
   14f28:	e1500081 	cmp	r0, r1, lsl #1
   14f2c:	e0a22002 	adc	r2, r2, r2
   14f30:	20400081 	subcs	r0, r0, r1, lsl #1
   14f34:	e1500001 	cmp	r0, r1
   14f38:	e0a22002 	adc	r2, r2, r2
   14f3c:	20400001 	subcs	r0, r0, r1
   14f40:	e1a00002 	mov	r0, r2
   14f44:	e12fff1e 	bx	lr
   14f48:	03a00001 	moveq	r0, #1	; 0x1
   14f4c:	13a00000 	movne	r0, #0	; 0x0
   14f50:	e12fff1e 	bx	lr
   14f54:	e16f2f11 	clz	r2, r1
   14f58:	e262201f 	rsb	r2, r2, #31	; 0x1f
   14f5c:	e1a00230 	lsr	r0, r0, r2
   14f60:	e12fff1e 	bx	lr
   14f64:	e52de008 	str	lr, [sp, #-8]!
   14f68:	eb0002cb 	bl	15a9c <__div0>
   14f6c:	e3a00000 	mov	r0, #0	; 0x0
   14f70:	e49df008 	ldr	pc, [sp], #8

00014f74 <__aeabi_uidivmod>:
   14f74:	e92d4003 	push	{r0, r1, lr}
   14f78:	ebffff81 	bl	14d84 <__aeabi_uidiv>
   14f7c:	e8bd4006 	pop	{r1, r2, lr}
   14f80:	e0030092 	mul	r3, r2, r0
   14f84:	e0411003 	sub	r1, r1, r3
   14f88:	e12fff1e 	bx	lr

00014f8c <__aeabi_idiv>:
   14f8c:	e3510000 	cmp	r1, #0	; 0x0
   14f90:	e020c001 	eor	ip, r0, r1
   14f94:	0a000080 	beq	1519c <__aeabi_idiv+0x210>
   14f98:	42611000 	rsbmi	r1, r1, #0	; 0x0
   14f9c:	e2512001 	subs	r2, r1, #1	; 0x1
   14fa0:	0a000070 	beq	15168 <__aeabi_idiv+0x1dc>
   14fa4:	e1b03000 	movs	r3, r0
   14fa8:	42603000 	rsbmi	r3, r0, #0	; 0x0
   14fac:	e1530001 	cmp	r3, r1
   14fb0:	9a00006f 	bls	15174 <__aeabi_idiv+0x1e8>
   14fb4:	e1110002 	tst	r1, r2
   14fb8:	0a000071 	beq	15184 <__aeabi_idiv+0x1f8>
   14fbc:	e16f2f13 	clz	r2, r3
   14fc0:	e16f0f11 	clz	r0, r1
   14fc4:	e0402002 	sub	r2, r0, r2
   14fc8:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   14fcc:	10822082 	addne	r2, r2, r2, lsl #1
   14fd0:	e3a00000 	mov	r0, #0	; 0x0
   14fd4:	108ff102 	addne	pc, pc, r2, lsl #2
   14fd8:	e1a00000 	nop			(mov r0,r0)
   14fdc:	e1530f81 	cmp	r3, r1, lsl #31
   14fe0:	e0a00000 	adc	r0, r0, r0
   14fe4:	20433f81 	subcs	r3, r3, r1, lsl #31
   14fe8:	e1530f01 	cmp	r3, r1, lsl #30
   14fec:	e0a00000 	adc	r0, r0, r0
   14ff0:	20433f01 	subcs	r3, r3, r1, lsl #30
   14ff4:	e1530e81 	cmp	r3, r1, lsl #29
   14ff8:	e0a00000 	adc	r0, r0, r0
   14ffc:	20433e81 	subcs	r3, r3, r1, lsl #29
   15000:	e1530e01 	cmp	r3, r1, lsl #28
   15004:	e0a00000 	adc	r0, r0, r0
   15008:	20433e01 	subcs	r3, r3, r1, lsl #28
   1500c:	e1530d81 	cmp	r3, r1, lsl #27
   15010:	e0a00000 	adc	r0, r0, r0
   15014:	20433d81 	subcs	r3, r3, r1, lsl #27
   15018:	e1530d01 	cmp	r3, r1, lsl #26
   1501c:	e0a00000 	adc	r0, r0, r0
   15020:	20433d01 	subcs	r3, r3, r1, lsl #26
   15024:	e1530c81 	cmp	r3, r1, lsl #25
   15028:	e0a00000 	adc	r0, r0, r0
   1502c:	20433c81 	subcs	r3, r3, r1, lsl #25
   15030:	e1530c01 	cmp	r3, r1, lsl #24
   15034:	e0a00000 	adc	r0, r0, r0
   15038:	20433c01 	subcs	r3, r3, r1, lsl #24
   1503c:	e1530b81 	cmp	r3, r1, lsl #23
   15040:	e0a00000 	adc	r0, r0, r0
   15044:	20433b81 	subcs	r3, r3, r1, lsl #23
   15048:	e1530b01 	cmp	r3, r1, lsl #22
   1504c:	e0a00000 	adc	r0, r0, r0
   15050:	20433b01 	subcs	r3, r3, r1, lsl #22
   15054:	e1530a81 	cmp	r3, r1, lsl #21
   15058:	e0a00000 	adc	r0, r0, r0
   1505c:	20433a81 	subcs	r3, r3, r1, lsl #21
   15060:	e1530a01 	cmp	r3, r1, lsl #20
   15064:	e0a00000 	adc	r0, r0, r0
   15068:	20433a01 	subcs	r3, r3, r1, lsl #20
   1506c:	e1530981 	cmp	r3, r1, lsl #19
   15070:	e0a00000 	adc	r0, r0, r0
   15074:	20433981 	subcs	r3, r3, r1, lsl #19
   15078:	e1530901 	cmp	r3, r1, lsl #18
   1507c:	e0a00000 	adc	r0, r0, r0
   15080:	20433901 	subcs	r3, r3, r1, lsl #18
   15084:	e1530881 	cmp	r3, r1, lsl #17
   15088:	e0a00000 	adc	r0, r0, r0
   1508c:	20433881 	subcs	r3, r3, r1, lsl #17
   15090:	e1530801 	cmp	r3, r1, lsl #16
   15094:	e0a00000 	adc	r0, r0, r0
   15098:	20433801 	subcs	r3, r3, r1, lsl #16
   1509c:	e1530781 	cmp	r3, r1, lsl #15
   150a0:	e0a00000 	adc	r0, r0, r0
   150a4:	20433781 	subcs	r3, r3, r1, lsl #15
   150a8:	e1530701 	cmp	r3, r1, lsl #14
   150ac:	e0a00000 	adc	r0, r0, r0
   150b0:	20433701 	subcs	r3, r3, r1, lsl #14
   150b4:	e1530681 	cmp	r3, r1, lsl #13
   150b8:	e0a00000 	adc	r0, r0, r0
   150bc:	20433681 	subcs	r3, r3, r1, lsl #13
   150c0:	e1530601 	cmp	r3, r1, lsl #12
   150c4:	e0a00000 	adc	r0, r0, r0
   150c8:	20433601 	subcs	r3, r3, r1, lsl #12
   150cc:	e1530581 	cmp	r3, r1, lsl #11
   150d0:	e0a00000 	adc	r0, r0, r0
   150d4:	20433581 	subcs	r3, r3, r1, lsl #11
   150d8:	e1530501 	cmp	r3, r1, lsl #10
   150dc:	e0a00000 	adc	r0, r0, r0
   150e0:	20433501 	subcs	r3, r3, r1, lsl #10
   150e4:	e1530481 	cmp	r3, r1, lsl #9
   150e8:	e0a00000 	adc	r0, r0, r0
   150ec:	20433481 	subcs	r3, r3, r1, lsl #9
   150f0:	e1530401 	cmp	r3, r1, lsl #8
   150f4:	e0a00000 	adc	r0, r0, r0
   150f8:	20433401 	subcs	r3, r3, r1, lsl #8
   150fc:	e1530381 	cmp	r3, r1, lsl #7
   15100:	e0a00000 	adc	r0, r0, r0
   15104:	20433381 	subcs	r3, r3, r1, lsl #7
   15108:	e1530301 	cmp	r3, r1, lsl #6
   1510c:	e0a00000 	adc	r0, r0, r0
   15110:	20433301 	subcs	r3, r3, r1, lsl #6
   15114:	e1530281 	cmp	r3, r1, lsl #5
   15118:	e0a00000 	adc	r0, r0, r0
   1511c:	20433281 	subcs	r3, r3, r1, lsl #5
   15120:	e1530201 	cmp	r3, r1, lsl #4
   15124:	e0a00000 	adc	r0, r0, r0
   15128:	20433201 	subcs	r3, r3, r1, lsl #4
   1512c:	e1530181 	cmp	r3, r1, lsl #3
   15130:	e0a00000 	adc	r0, r0, r0
   15134:	20433181 	subcs	r3, r3, r1, lsl #3
   15138:	e1530101 	cmp	r3, r1, lsl #2
   1513c:	e0a00000 	adc	r0, r0, r0
   15140:	20433101 	subcs	r3, r3, r1, lsl #2
   15144:	e1530081 	cmp	r3, r1, lsl #1
   15148:	e0a00000 	adc	r0, r0, r0
   1514c:	20433081 	subcs	r3, r3, r1, lsl #1
   15150:	e1530001 	cmp	r3, r1
   15154:	e0a00000 	adc	r0, r0, r0
   15158:	20433001 	subcs	r3, r3, r1
   1515c:	e35c0000 	cmp	ip, #0	; 0x0
   15160:	42600000 	rsbmi	r0, r0, #0	; 0x0
   15164:	e12fff1e 	bx	lr
   15168:	e13c0000 	teq	ip, r0
   1516c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   15170:	e12fff1e 	bx	lr
   15174:	33a00000 	movcc	r0, #0	; 0x0
   15178:	01a00fcc 	asreq	r0, ip, #31
   1517c:	03800001 	orreq	r0, r0, #1	; 0x1
   15180:	e12fff1e 	bx	lr
   15184:	e16f2f11 	clz	r2, r1
   15188:	e262201f 	rsb	r2, r2, #31	; 0x1f
   1518c:	e35c0000 	cmp	ip, #0	; 0x0
   15190:	e1a00233 	lsr	r0, r3, r2
   15194:	42600000 	rsbmi	r0, r0, #0	; 0x0
   15198:	e12fff1e 	bx	lr
   1519c:	e52de008 	str	lr, [sp, #-8]!
   151a0:	eb00023d 	bl	15a9c <__div0>
   151a4:	e3a00000 	mov	r0, #0	; 0x0
   151a8:	e49df008 	ldr	pc, [sp], #8

000151ac <__aeabi_idivmod>:
   151ac:	e92d4003 	push	{r0, r1, lr}
   151b0:	ebffff75 	bl	14f8c <__aeabi_idiv>
   151b4:	e8bd4006 	pop	{r1, r2, lr}
   151b8:	e0030092 	mul	r3, r2, r0
   151bc:	e0411003 	sub	r1, r1, r3
   151c0:	e12fff1e 	bx	lr

000151c4 <__aeabi_llsr>:
   151c4:	e2523020 	subs	r3, r2, #32	; 0x20
   151c8:	e262c020 	rsb	ip, r2, #32	; 0x20
   151cc:	41a00230 	lsrmi	r0, r0, r2
   151d0:	51a00331 	lsrpl	r0, r1, r3
   151d4:	41800c11 	orrmi	r0, r0, r1, lsl ip
   151d8:	e1a01231 	lsr	r1, r1, r2
   151dc:	e12fff1e 	bx	lr

000151e0 <__aeabi_lasr>:
   151e0:	e2523020 	subs	r3, r2, #32	; 0x20
   151e4:	e262c020 	rsb	ip, r2, #32	; 0x20
   151e8:	41a00230 	lsrmi	r0, r0, r2
   151ec:	51a00351 	asrpl	r0, r1, r3
   151f0:	41800c11 	orrmi	r0, r0, r1, lsl ip
   151f4:	e1a01251 	asr	r1, r1, r2
   151f8:	e12fff1e 	bx	lr

000151fc <__aeabi_llsl>:
   151fc:	e2523020 	subs	r3, r2, #32	; 0x20
   15200:	e262c020 	rsb	ip, r2, #32	; 0x20
   15204:	41a01211 	lslmi	r1, r1, r2
   15208:	51a01310 	lslpl	r1, r0, r3
   1520c:	41811c30 	orrmi	r1, r1, r0, lsr ip
   15210:	e1a00210 	lsl	r0, r0, r2
   15214:	e12fff1e 	bx	lr

00015218 <__aeabi_drsub>:
   15218:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   1521c:	ea000000 	b	15224 <__adddf3>

00015220 <__aeabi_dsub>:
   15220:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

00015224 <__adddf3>:
   15224:	e92d4030 	push	{r4, r5, lr}
   15228:	e1a04081 	lsl	r4, r1, #1
   1522c:	e1a05083 	lsl	r5, r3, #1
   15230:	e1340005 	teq	r4, r5
   15234:	01300002 	teqeq	r0, r2
   15238:	1194c000 	orrsne	ip, r4, r0
   1523c:	1195c002 	orrsne	ip, r5, r2
   15240:	11f0cac4 	mvnsne	ip, r4, asr #21
   15244:	11f0cac5 	mvnsne	ip, r5, asr #21
   15248:	0a000079 	beq	15434 <__adddf3+0x210>
   1524c:	e1a04aa4 	lsr	r4, r4, #21
   15250:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   15254:	b2655000 	rsblt	r5, r5, #0	; 0x0
   15258:	da000006 	ble	15278 <__adddf3+0x54>
   1525c:	e0844005 	add	r4, r4, r5
   15260:	e0202002 	eor	r2, r0, r2
   15264:	e0213003 	eor	r3, r1, r3
   15268:	e0220000 	eor	r0, r2, r0
   1526c:	e0231001 	eor	r1, r3, r1
   15270:	e0202002 	eor	r2, r0, r2
   15274:	e0213003 	eor	r3, r1, r3
   15278:	e3550036 	cmp	r5, #54	; 0x36
   1527c:	88bd8030 	pophi	{r4, r5, pc}
   15280:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   15284:	e1a01601 	lsl	r1, r1, #12
   15288:	e3a0c601 	mov	ip, #1048576	; 0x100000
   1528c:	e18c1621 	orr	r1, ip, r1, lsr #12
   15290:	0a000001 	beq	1529c <__adddf3+0x78>
   15294:	e2700000 	rsbs	r0, r0, #0	; 0x0
   15298:	e2e11000 	rsc	r1, r1, #0	; 0x0
   1529c:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   152a0:	e1a03603 	lsl	r3, r3, #12
   152a4:	e18c3623 	orr	r3, ip, r3, lsr #12
   152a8:	0a000001 	beq	152b4 <__adddf3+0x90>
   152ac:	e2722000 	rsbs	r2, r2, #0	; 0x0
   152b0:	e2e33000 	rsc	r3, r3, #0	; 0x0
   152b4:	e1340005 	teq	r4, r5
   152b8:	0a000057 	beq	1541c <__adddf3+0x1f8>
   152bc:	e2444001 	sub	r4, r4, #1	; 0x1
   152c0:	e275e020 	rsbs	lr, r5, #32	; 0x20
   152c4:	ba000005 	blt	152e0 <__adddf3+0xbc>
   152c8:	e1a0ce12 	lsl	ip, r2, lr
   152cc:	e0900532 	adds	r0, r0, r2, lsr r5
   152d0:	e2a11000 	adc	r1, r1, #0	; 0x0
   152d4:	e0900e13 	adds	r0, r0, r3, lsl lr
   152d8:	e0b11553 	adcs	r1, r1, r3, asr r5
   152dc:	ea000006 	b	152fc <__adddf3+0xd8>
   152e0:	e2455020 	sub	r5, r5, #32	; 0x20
   152e4:	e28ee020 	add	lr, lr, #32	; 0x20
   152e8:	e3520001 	cmp	r2, #1	; 0x1
   152ec:	e1a0ce13 	lsl	ip, r3, lr
   152f0:	238cc002 	orrcs	ip, ip, #2	; 0x2
   152f4:	e0900553 	adds	r0, r0, r3, asr r5
   152f8:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   152fc:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   15300:	5a000002 	bpl	15310 <__adddf3+0xec>
   15304:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   15308:	e2f00000 	rscs	r0, r0, #0	; 0x0
   1530c:	e2e11000 	rsc	r1, r1, #0	; 0x0
   15310:	e3510601 	cmp	r1, #1048576	; 0x100000
   15314:	3a00000e 	bcc	15354 <__adddf3+0x130>
   15318:	e3510602 	cmp	r1, #2097152	; 0x200000
   1531c:	3a000006 	bcc	1533c <__adddf3+0x118>
   15320:	e1b010a1 	lsrs	r1, r1, #1
   15324:	e1b00060 	rrxs	r0, r0
   15328:	e1a0c06c 	rrx	ip, ip
   1532c:	e2844001 	add	r4, r4, #1	; 0x1
   15330:	e1a02a84 	lsl	r2, r4, #21
   15334:	e3720501 	cmn	r2, #4194304	; 0x400000
   15338:	2a000055 	bcs	15494 <__adddf3+0x270>
   1533c:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   15340:	01b0c0a0 	lsrseq	ip, r0, #1
   15344:	e2b00000 	adcs	r0, r0, #0	; 0x0
   15348:	e0a11a04 	adc	r1, r1, r4, lsl #20
   1534c:	e1811005 	orr	r1, r1, r5
   15350:	e8bd8030 	pop	{r4, r5, pc}
   15354:	e1b0c08c 	lsls	ip, ip, #1
   15358:	e0b00000 	adcs	r0, r0, r0
   1535c:	e0a11001 	adc	r1, r1, r1
   15360:	e3110601 	tst	r1, #1048576	; 0x100000
   15364:	e2444001 	sub	r4, r4, #1	; 0x1
   15368:	1afffff3 	bne	1533c <__adddf3+0x118>
   1536c:	e3310000 	teq	r1, #0	; 0x0
   15370:	01a01000 	moveq	r1, r0
   15374:	03a00000 	moveq	r0, #0	; 0x0
   15378:	e16f3f11 	clz	r3, r1
   1537c:	02833020 	addeq	r3, r3, #32	; 0x20
   15380:	e243300b 	sub	r3, r3, #11	; 0xb
   15384:	e2532020 	subs	r2, r3, #32	; 0x20
   15388:	aa000007 	bge	153ac <__adddf3+0x188>
   1538c:	e292200c 	adds	r2, r2, #12	; 0xc
   15390:	da000004 	ble	153a8 <__adddf3+0x184>
   15394:	e282c014 	add	ip, r2, #20	; 0x14
   15398:	e262200c 	rsb	r2, r2, #12	; 0xc
   1539c:	e1a00c11 	lsl	r0, r1, ip
   153a0:	e1a01231 	lsr	r1, r1, r2
   153a4:	ea000004 	b	153bc <__adddf3+0x198>
   153a8:	e2822014 	add	r2, r2, #20	; 0x14
   153ac:	d262c020 	rsble	ip, r2, #32	; 0x20
   153b0:	e1a01211 	lsl	r1, r1, r2
   153b4:	d1811c30 	orrle	r1, r1, r0, lsr ip
   153b8:	d1a00210 	lslle	r0, r0, r2
   153bc:	e0544003 	subs	r4, r4, r3
   153c0:	a0811a04 	addge	r1, r1, r4, lsl #20
   153c4:	a1811005 	orrge	r1, r1, r5
   153c8:	a8bd8030 	popge	{r4, r5, pc}
   153cc:	e1e04004 	mvn	r4, r4
   153d0:	e254401f 	subs	r4, r4, #31	; 0x1f
   153d4:	aa00000d 	bge	15410 <__adddf3+0x1ec>
   153d8:	e294400c 	adds	r4, r4, #12	; 0xc
   153dc:	ca000005 	bgt	153f8 <__adddf3+0x1d4>
   153e0:	e2844014 	add	r4, r4, #20	; 0x14
   153e4:	e2642020 	rsb	r2, r4, #32	; 0x20
   153e8:	e1a00430 	lsr	r0, r0, r4
   153ec:	e1800211 	orr	r0, r0, r1, lsl r2
   153f0:	e1851431 	orr	r1, r5, r1, lsr r4
   153f4:	e8bd8030 	pop	{r4, r5, pc}
   153f8:	e264400c 	rsb	r4, r4, #12	; 0xc
   153fc:	e2642020 	rsb	r2, r4, #32	; 0x20
   15400:	e1a00230 	lsr	r0, r0, r2
   15404:	e1800411 	orr	r0, r0, r1, lsl r4
   15408:	e1a01005 	mov	r1, r5
   1540c:	e8bd8030 	pop	{r4, r5, pc}
   15410:	e1a00431 	lsr	r0, r1, r4
   15414:	e1a01005 	mov	r1, r5
   15418:	e8bd8030 	pop	{r4, r5, pc}
   1541c:	e3340000 	teq	r4, #0	; 0x0
   15420:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   15424:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   15428:	02844001 	addeq	r4, r4, #1	; 0x1
   1542c:	12455001 	subne	r5, r5, #1	; 0x1
   15430:	eaffffa1 	b	152bc <__adddf3+0x98>
   15434:	e1f0cac4 	mvns	ip, r4, asr #21
   15438:	11f0cac5 	mvnsne	ip, r5, asr #21
   1543c:	0a000018 	beq	154a4 <__adddf3+0x280>
   15440:	e1340005 	teq	r4, r5
   15444:	01300002 	teqeq	r0, r2
   15448:	0a000003 	beq	1545c <__adddf3+0x238>
   1544c:	e194c000 	orrs	ip, r4, r0
   15450:	01a01003 	moveq	r1, r3
   15454:	01a00002 	moveq	r0, r2
   15458:	e8bd8030 	pop	{r4, r5, pc}
   1545c:	e1310003 	teq	r1, r3
   15460:	13a01000 	movne	r1, #0	; 0x0
   15464:	13a00000 	movne	r0, #0	; 0x0
   15468:	18bd8030 	popne	{r4, r5, pc}
   1546c:	e1b0caa4 	lsrs	ip, r4, #21
   15470:	1a000003 	bne	15484 <__adddf3+0x260>
   15474:	e1b00080 	lsls	r0, r0, #1
   15478:	e0b11001 	adcs	r1, r1, r1
   1547c:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   15480:	e8bd8030 	pop	{r4, r5, pc}
   15484:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   15488:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   1548c:	38bd8030 	popcc	{r4, r5, pc}
   15490:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   15494:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   15498:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   1549c:	e3a00000 	mov	r0, #0	; 0x0
   154a0:	e8bd8030 	pop	{r4, r5, pc}
   154a4:	e1f0cac4 	mvns	ip, r4, asr #21
   154a8:	11a01003 	movne	r1, r3
   154ac:	11a00002 	movne	r0, r2
   154b0:	01f0cac5 	mvnseq	ip, r5, asr #21
   154b4:	11a03001 	movne	r3, r1
   154b8:	11a02000 	movne	r2, r0
   154bc:	e1904601 	orrs	r4, r0, r1, lsl #12
   154c0:	01925603 	orrseq	r5, r2, r3, lsl #12
   154c4:	01310003 	teqeq	r1, r3
   154c8:	13811702 	orrne	r1, r1, #524288	; 0x80000
   154cc:	e8bd8030 	pop	{r4, r5, pc}

000154d0 <__aeabi_ui2d>:
   154d0:	e3300000 	teq	r0, #0	; 0x0
   154d4:	03a01000 	moveq	r1, #0	; 0x0
   154d8:	012fff1e 	bxeq	lr
   154dc:	e92d4030 	push	{r4, r5, lr}
   154e0:	e3a04b01 	mov	r4, #1024	; 0x400
   154e4:	e2844032 	add	r4, r4, #50	; 0x32
   154e8:	e3a05000 	mov	r5, #0	; 0x0
   154ec:	e3a01000 	mov	r1, #0	; 0x0
   154f0:	eaffff9d 	b	1536c <__adddf3+0x148>

000154f4 <__aeabi_i2d>:
   154f4:	e3300000 	teq	r0, #0	; 0x0
   154f8:	03a01000 	moveq	r1, #0	; 0x0
   154fc:	012fff1e 	bxeq	lr
   15500:	e92d4030 	push	{r4, r5, lr}
   15504:	e3a04b01 	mov	r4, #1024	; 0x400
   15508:	e2844032 	add	r4, r4, #50	; 0x32
   1550c:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   15510:	42600000 	rsbmi	r0, r0, #0	; 0x0
   15514:	e3a01000 	mov	r1, #0	; 0x0
   15518:	eaffff93 	b	1536c <__adddf3+0x148>

0001551c <__aeabi_f2d>:
   1551c:	e1b02080 	lsls	r2, r0, #1
   15520:	e1a011c2 	asr	r1, r2, #3
   15524:	e1a01061 	rrx	r1, r1
   15528:	e1a00e02 	lsl	r0, r2, #28
   1552c:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   15530:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   15534:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   15538:	112fff1e 	bxne	lr
   1553c:	e3320000 	teq	r2, #0	; 0x0
   15540:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   15544:	012fff1e 	bxeq	lr
   15548:	e92d4030 	push	{r4, r5, lr}
   1554c:	e3a04d0e 	mov	r4, #896	; 0x380
   15550:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   15554:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   15558:	eaffff83 	b	1536c <__adddf3+0x148>

0001555c <__aeabi_ul2d>:
   1555c:	e1902001 	orrs	r2, r0, r1
   15560:	012fff1e 	bxeq	lr
   15564:	e92d4030 	push	{r4, r5, lr}
   15568:	e3a05000 	mov	r5, #0	; 0x0
   1556c:	ea000006 	b	1558c <__aeabi_l2d+0x1c>

00015570 <__aeabi_l2d>:
   15570:	e1902001 	orrs	r2, r0, r1
   15574:	012fff1e 	bxeq	lr
   15578:	e92d4030 	push	{r4, r5, lr}
   1557c:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   15580:	5a000001 	bpl	1558c <__aeabi_l2d+0x1c>
   15584:	e2700000 	rsbs	r0, r0, #0	; 0x0
   15588:	e2e11000 	rsc	r1, r1, #0	; 0x0
   1558c:	e3a04b01 	mov	r4, #1024	; 0x400
   15590:	e2844032 	add	r4, r4, #50	; 0x32
   15594:	e1b0cb21 	lsrs	ip, r1, #22
   15598:	0affff5c 	beq	15310 <__adddf3+0xec>
   1559c:	e3a02003 	mov	r2, #3	; 0x3
   155a0:	e1b0c1ac 	lsrs	ip, ip, #3
   155a4:	12822003 	addne	r2, r2, #3	; 0x3
   155a8:	e1b0c1ac 	lsrs	ip, ip, #3
   155ac:	12822003 	addne	r2, r2, #3	; 0x3
   155b0:	e08221ac 	add	r2, r2, ip, lsr #3
   155b4:	e2623020 	rsb	r3, r2, #32	; 0x20
   155b8:	e1a0c310 	lsl	ip, r0, r3
   155bc:	e1a00230 	lsr	r0, r0, r2
   155c0:	e1800311 	orr	r0, r0, r1, lsl r3
   155c4:	e1a01231 	lsr	r1, r1, r2
   155c8:	e0844002 	add	r4, r4, r2
   155cc:	eaffff4f 	b	15310 <__adddf3+0xec>

000155d0 <__aeabi_dmul>:
   155d0:	e92d4070 	push	{r4, r5, r6, lr}
   155d4:	e3a0c0ff 	mov	ip, #255	; 0xff
   155d8:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   155dc:	e01c4a21 	ands	r4, ip, r1, lsr #20
   155e0:	101c5a23 	andsne	r5, ip, r3, lsr #20
   155e4:	1134000c 	teqne	r4, ip
   155e8:	1135000c 	teqne	r5, ip
   155ec:	0b00006f 	bleq	157b0 <__aeabi_dmul+0x1e0>
   155f0:	e0844005 	add	r4, r4, r5
   155f4:	e0216003 	eor	r6, r1, r3
   155f8:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   155fc:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   15600:	e1905601 	orrs	r5, r0, r1, lsl #12
   15604:	11925603 	orrsne	r5, r2, r3, lsl #12
   15608:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   1560c:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   15610:	0a00001c 	beq	15688 <__aeabi_dmul+0xb8>
   15614:	e08ec290 	umull	ip, lr, r0, r2
   15618:	e3a05000 	mov	r5, #0	; 0x0
   1561c:	e0a5e291 	umlal	lr, r5, r1, r2
   15620:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   15624:	e0a5e390 	umlal	lr, r5, r0, r3
   15628:	e3a06000 	mov	r6, #0	; 0x0
   1562c:	e0a65391 	umlal	r5, r6, r1, r3
   15630:	e33c0000 	teq	ip, #0	; 0x0
   15634:	138ee001 	orrne	lr, lr, #1	; 0x1
   15638:	e24440ff 	sub	r4, r4, #255	; 0xff
   1563c:	e3560c02 	cmp	r6, #512	; 0x200
   15640:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   15644:	2a000002 	bcs	15654 <__aeabi_dmul+0x84>
   15648:	e1b0e08e 	lsls	lr, lr, #1
   1564c:	e0b55005 	adcs	r5, r5, r5
   15650:	e0a66006 	adc	r6, r6, r6
   15654:	e1821586 	orr	r1, r2, r6, lsl #11
   15658:	e1811aa5 	orr	r1, r1, r5, lsr #21
   1565c:	e1a00585 	lsl	r0, r5, #11
   15660:	e1800aae 	orr	r0, r0, lr, lsr #21
   15664:	e1a0e58e 	lsl	lr, lr, #11
   15668:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   1566c:	835c0c07 	cmphi	ip, #1792	; 0x700
   15670:	8a00000f 	bhi	156b4 <__aeabi_dmul+0xe4>
   15674:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   15678:	01b0e0a0 	lsrseq	lr, r0, #1
   1567c:	e2b00000 	adcs	r0, r0, #0	; 0x0
   15680:	e0a11a04 	adc	r1, r1, r4, lsl #20
   15684:	e8bd8070 	pop	{r4, r5, r6, pc}
   15688:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   1568c:	e1861001 	orr	r1, r6, r1
   15690:	e1800002 	orr	r0, r0, r2
   15694:	e0211003 	eor	r1, r1, r3
   15698:	e05440ac 	subs	r4, r4, ip, lsr #1
   1569c:	c074500c 	rsbsgt	r5, r4, ip
   156a0:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   156a4:	c8bd8070 	popgt	{r4, r5, r6, pc}
   156a8:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   156ac:	e3a0e000 	mov	lr, #0	; 0x0
   156b0:	e2544001 	subs	r4, r4, #1	; 0x1
   156b4:	ca000058 	bgt	1581c <__aeabi_dmul+0x24c>
   156b8:	e3740036 	cmn	r4, #54	; 0x36
   156bc:	d3a00000 	movle	r0, #0	; 0x0
   156c0:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   156c4:	d8bd8070 	pople	{r4, r5, r6, pc}
   156c8:	e2644000 	rsb	r4, r4, #0	; 0x0
   156cc:	e2544020 	subs	r4, r4, #32	; 0x20
   156d0:	aa000018 	bge	15738 <__aeabi_dmul+0x168>
   156d4:	e294400c 	adds	r4, r4, #12	; 0xc
   156d8:	ca00000b 	bgt	1570c <__aeabi_dmul+0x13c>
   156dc:	e2844014 	add	r4, r4, #20	; 0x14
   156e0:	e2645020 	rsb	r5, r4, #32	; 0x20
   156e4:	e1a03510 	lsl	r3, r0, r5
   156e8:	e1a00430 	lsr	r0, r0, r4
   156ec:	e1800511 	orr	r0, r0, r1, lsl r5
   156f0:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   156f4:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   156f8:	e0900fa3 	adds	r0, r0, r3, lsr #31
   156fc:	e0a21431 	adc	r1, r2, r1, lsr r4
   15700:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15704:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15708:	e8bd8070 	pop	{r4, r5, r6, pc}
   1570c:	e264400c 	rsb	r4, r4, #12	; 0xc
   15710:	e2645020 	rsb	r5, r4, #32	; 0x20
   15714:	e1a03410 	lsl	r3, r0, r4
   15718:	e1a00530 	lsr	r0, r0, r5
   1571c:	e1800411 	orr	r0, r0, r1, lsl r4
   15720:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15724:	e0900fa3 	adds	r0, r0, r3, lsr #31
   15728:	e2a11000 	adc	r1, r1, #0	; 0x0
   1572c:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15730:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15734:	e8bd8070 	pop	{r4, r5, r6, pc}
   15738:	e2645020 	rsb	r5, r4, #32	; 0x20
   1573c:	e18ee510 	orr	lr, lr, r0, lsl r5
   15740:	e1a03430 	lsr	r3, r0, r4
   15744:	e1833511 	orr	r3, r3, r1, lsl r5
   15748:	e1a00431 	lsr	r0, r1, r4
   1574c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15750:	e1c00431 	bic	r0, r0, r1, lsr r4
   15754:	e0800fa3 	add	r0, r0, r3, lsr #31
   15758:	e19ee083 	orrs	lr, lr, r3, lsl #1
   1575c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15760:	e8bd8070 	pop	{r4, r5, r6, pc}
   15764:	e3340000 	teq	r4, #0	; 0x0
   15768:	1a000008 	bne	15790 <__aeabi_dmul+0x1c0>
   1576c:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   15770:	e1b00080 	lsls	r0, r0, #1
   15774:	e0a11001 	adc	r1, r1, r1
   15778:	e3110601 	tst	r1, #1048576	; 0x100000
   1577c:	02444001 	subeq	r4, r4, #1	; 0x1
   15780:	0afffffa 	beq	15770 <__aeabi_dmul+0x1a0>
   15784:	e1811006 	orr	r1, r1, r6
   15788:	e3350000 	teq	r5, #0	; 0x0
   1578c:	11a0f00e 	movne	pc, lr
   15790:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   15794:	e1b02082 	lsls	r2, r2, #1
   15798:	e0a33003 	adc	r3, r3, r3
   1579c:	e3130601 	tst	r3, #1048576	; 0x100000
   157a0:	02455001 	subeq	r5, r5, #1	; 0x1
   157a4:	0afffffa 	beq	15794 <__aeabi_dmul+0x1c4>
   157a8:	e1833006 	orr	r3, r3, r6
   157ac:	e1a0f00e 	mov	pc, lr
   157b0:	e134000c 	teq	r4, ip
   157b4:	e00c5a23 	and	r5, ip, r3, lsr #20
   157b8:	1135000c 	teqne	r5, ip
   157bc:	0a000006 	beq	157dc <__aeabi_dmul+0x20c>
   157c0:	e1906081 	orrs	r6, r0, r1, lsl #1
   157c4:	11926083 	orrsne	r6, r2, r3, lsl #1
   157c8:	1affffe5 	bne	15764 <__aeabi_dmul+0x194>
   157cc:	e0211003 	eor	r1, r1, r3
   157d0:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   157d4:	e3a00000 	mov	r0, #0	; 0x0
   157d8:	e8bd8070 	pop	{r4, r5, r6, pc}
   157dc:	e1906081 	orrs	r6, r0, r1, lsl #1
   157e0:	01a00002 	moveq	r0, r2
   157e4:	01a01003 	moveq	r1, r3
   157e8:	11926083 	orrsne	r6, r2, r3, lsl #1
   157ec:	0a00000f 	beq	15830 <__aeabi_dmul+0x260>
   157f0:	e134000c 	teq	r4, ip
   157f4:	1a000001 	bne	15800 <__aeabi_dmul+0x230>
   157f8:	e1906601 	orrs	r6, r0, r1, lsl #12
   157fc:	1a00000b 	bne	15830 <__aeabi_dmul+0x260>
   15800:	e135000c 	teq	r5, ip
   15804:	1a000003 	bne	15818 <__aeabi_dmul+0x248>
   15808:	e1926603 	orrs	r6, r2, r3, lsl #12
   1580c:	11a00002 	movne	r0, r2
   15810:	11a01003 	movne	r1, r3
   15814:	1a000005 	bne	15830 <__aeabi_dmul+0x260>
   15818:	e0211003 	eor	r1, r1, r3
   1581c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15820:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   15824:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   15828:	e3a00000 	mov	r0, #0	; 0x0
   1582c:	e8bd8070 	pop	{r4, r5, r6, pc}
   15830:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   15834:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   15838:	e8bd8070 	pop	{r4, r5, r6, pc}

0001583c <__aeabi_ddiv>:
   1583c:	e92d4070 	push	{r4, r5, r6, lr}
   15840:	e3a0c0ff 	mov	ip, #255	; 0xff
   15844:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   15848:	e01c4a21 	ands	r4, ip, r1, lsr #20
   1584c:	101c5a23 	andsne	r5, ip, r3, lsr #20
   15850:	1134000c 	teqne	r4, ip
   15854:	1135000c 	teqne	r5, ip
   15858:	0b00005c 	bleq	159d0 <__aeabi_ddiv+0x194>
   1585c:	e0444005 	sub	r4, r4, r5
   15860:	e021e003 	eor	lr, r1, r3
   15864:	e1925603 	orrs	r5, r2, r3, lsl #12
   15868:	e1a01601 	lsl	r1, r1, #12
   1586c:	0a00004b 	beq	159a0 <__aeabi_ddiv+0x164>
   15870:	e1a03603 	lsl	r3, r3, #12
   15874:	e3a05201 	mov	r5, #268435456	; 0x10000000
   15878:	e1853223 	orr	r3, r5, r3, lsr #4
   1587c:	e1833c22 	orr	r3, r3, r2, lsr #24
   15880:	e1a02402 	lsl	r2, r2, #8
   15884:	e1855221 	orr	r5, r5, r1, lsr #4
   15888:	e1855c20 	orr	r5, r5, r0, lsr #24
   1588c:	e1a06400 	lsl	r6, r0, #8
   15890:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   15894:	e1550003 	cmp	r5, r3
   15898:	01560002 	cmpeq	r6, r2
   1589c:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   158a0:	e2844c03 	add	r4, r4, #768	; 0x300
   158a4:	2a000001 	bcs	158b0 <__aeabi_ddiv+0x74>
   158a8:	e1b030a3 	lsrs	r3, r3, #1
   158ac:	e1a02062 	rrx	r2, r2
   158b0:	e0566002 	subs	r6, r6, r2
   158b4:	e0c55003 	sbc	r5, r5, r3
   158b8:	e1b030a3 	lsrs	r3, r3, #1
   158bc:	e1a02062 	rrx	r2, r2
   158c0:	e3a00601 	mov	r0, #1048576	; 0x100000
   158c4:	e3a0c702 	mov	ip, #524288	; 0x80000
   158c8:	e056e002 	subs	lr, r6, r2
   158cc:	e0d5e003 	sbcs	lr, r5, r3
   158d0:	20466002 	subcs	r6, r6, r2
   158d4:	21a0500e 	movcs	r5, lr
   158d8:	2180000c 	orrcs	r0, r0, ip
   158dc:	e1b030a3 	lsrs	r3, r3, #1
   158e0:	e1a02062 	rrx	r2, r2
   158e4:	e056e002 	subs	lr, r6, r2
   158e8:	e0d5e003 	sbcs	lr, r5, r3
   158ec:	20466002 	subcs	r6, r6, r2
   158f0:	21a0500e 	movcs	r5, lr
   158f4:	218000ac 	orrcs	r0, r0, ip, lsr #1
   158f8:	e1b030a3 	lsrs	r3, r3, #1
   158fc:	e1a02062 	rrx	r2, r2
   15900:	e056e002 	subs	lr, r6, r2
   15904:	e0d5e003 	sbcs	lr, r5, r3
   15908:	20466002 	subcs	r6, r6, r2
   1590c:	21a0500e 	movcs	r5, lr
   15910:	2180012c 	orrcs	r0, r0, ip, lsr #2
   15914:	e1b030a3 	lsrs	r3, r3, #1
   15918:	e1a02062 	rrx	r2, r2
   1591c:	e056e002 	subs	lr, r6, r2
   15920:	e0d5e003 	sbcs	lr, r5, r3
   15924:	20466002 	subcs	r6, r6, r2
   15928:	21a0500e 	movcs	r5, lr
   1592c:	218001ac 	orrcs	r0, r0, ip, lsr #3
   15930:	e195e006 	orrs	lr, r5, r6
   15934:	0a00000d 	beq	15970 <__aeabi_ddiv+0x134>
   15938:	e1a05205 	lsl	r5, r5, #4
   1593c:	e1855e26 	orr	r5, r5, r6, lsr #28
   15940:	e1a06206 	lsl	r6, r6, #4
   15944:	e1a03183 	lsl	r3, r3, #3
   15948:	e1833ea2 	orr	r3, r3, r2, lsr #29
   1594c:	e1a02182 	lsl	r2, r2, #3
   15950:	e1b0c22c 	lsrs	ip, ip, #4
   15954:	1affffdb 	bne	158c8 <__aeabi_ddiv+0x8c>
   15958:	e3110601 	tst	r1, #1048576	; 0x100000
   1595c:	1a000006 	bne	1597c <__aeabi_ddiv+0x140>
   15960:	e1811000 	orr	r1, r1, r0
   15964:	e3a00000 	mov	r0, #0	; 0x0
   15968:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   1596c:	eaffffd5 	b	158c8 <__aeabi_ddiv+0x8c>
   15970:	e3110601 	tst	r1, #1048576	; 0x100000
   15974:	01811000 	orreq	r1, r1, r0
   15978:	03a00000 	moveq	r0, #0	; 0x0
   1597c:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   15980:	835c0c07 	cmphi	ip, #1792	; 0x700
   15984:	8affff4a 	bhi	156b4 <__aeabi_dmul+0xe4>
   15988:	e055c003 	subs	ip, r5, r3
   1598c:	0056c002 	subseq	ip, r6, r2
   15990:	01b0c0a0 	lsrseq	ip, r0, #1
   15994:	e2b00000 	adcs	r0, r0, #0	; 0x0
   15998:	e0a11a04 	adc	r1, r1, r4, lsl #20
   1599c:	e8bd8070 	pop	{r4, r5, r6, pc}
   159a0:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   159a4:	e18e1621 	orr	r1, lr, r1, lsr #12
   159a8:	e09440ac 	adds	r4, r4, ip, lsr #1
   159ac:	c074500c 	rsbsgt	r5, r4, ip
   159b0:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   159b4:	c8bd8070 	popgt	{r4, r5, r6, pc}
   159b8:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   159bc:	e3a0e000 	mov	lr, #0	; 0x0
   159c0:	e2544001 	subs	r4, r4, #1	; 0x1
   159c4:	eaffff3a 	b	156b4 <__aeabi_dmul+0xe4>
   159c8:	e185e006 	orr	lr, r5, r6
   159cc:	eaffff38 	b	156b4 <__aeabi_dmul+0xe4>
   159d0:	e00c5a23 	and	r5, ip, r3, lsr #20
   159d4:	e134000c 	teq	r4, ip
   159d8:	0135000c 	teqeq	r5, ip
   159dc:	0affff93 	beq	15830 <__aeabi_dmul+0x260>
   159e0:	e134000c 	teq	r4, ip
   159e4:	1a000006 	bne	15a04 <__aeabi_ddiv+0x1c8>
   159e8:	e1904601 	orrs	r4, r0, r1, lsl #12
   159ec:	1affff8f 	bne	15830 <__aeabi_dmul+0x260>
   159f0:	e135000c 	teq	r5, ip
   159f4:	1affff87 	bne	15818 <__aeabi_dmul+0x248>
   159f8:	e1a00002 	mov	r0, r2
   159fc:	e1a01003 	mov	r1, r3
   15a00:	eaffff8a 	b	15830 <__aeabi_dmul+0x260>
   15a04:	e135000c 	teq	r5, ip
   15a08:	1a000004 	bne	15a20 <__aeabi_ddiv+0x1e4>
   15a0c:	e1925603 	orrs	r5, r2, r3, lsl #12
   15a10:	0affff6d 	beq	157cc <__aeabi_dmul+0x1fc>
   15a14:	e1a00002 	mov	r0, r2
   15a18:	e1a01003 	mov	r1, r3
   15a1c:	eaffff83 	b	15830 <__aeabi_dmul+0x260>
   15a20:	e1906081 	orrs	r6, r0, r1, lsl #1
   15a24:	11926083 	orrsne	r6, r2, r3, lsl #1
   15a28:	1affff4d 	bne	15764 <__aeabi_dmul+0x194>
   15a2c:	e1904081 	orrs	r4, r0, r1, lsl #1
   15a30:	1affff78 	bne	15818 <__aeabi_dmul+0x248>
   15a34:	e1925083 	orrs	r5, r2, r3, lsl #1
   15a38:	1affff63 	bne	157cc <__aeabi_dmul+0x1fc>
   15a3c:	eaffff7b 	b	15830 <__aeabi_dmul+0x260>

00015a40 <__aeabi_d2iz>:
   15a40:	e1a02081 	lsl	r2, r1, #1
   15a44:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   15a48:	2a00000c 	bcs	15a80 <__aeabi_d2iz+0x40>
   15a4c:	5a000009 	bpl	15a78 <__aeabi_d2iz+0x38>
   15a50:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   15a54:	e0532ac2 	subs	r2, r3, r2, asr #21
   15a58:	9a00000a 	bls	15a88 <__aeabi_d2iz+0x48>
   15a5c:	e1a03581 	lsl	r3, r1, #11
   15a60:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   15a64:	e1833aa0 	orr	r3, r3, r0, lsr #21
   15a68:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   15a6c:	e1a00233 	lsr	r0, r3, r2
   15a70:	12600000 	rsbne	r0, r0, #0	; 0x0
   15a74:	e12fff1e 	bx	lr
   15a78:	e3a00000 	mov	r0, #0	; 0x0
   15a7c:	e12fff1e 	bx	lr
   15a80:	e1900601 	orrs	r0, r0, r1, lsl #12
   15a84:	1a000002 	bne	15a94 <__aeabi_d2iz+0x54>
   15a88:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   15a8c:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   15a90:	e12fff1e 	bx	lr
   15a94:	e3a00000 	mov	r0, #0	; 0x0
   15a98:	e12fff1e 	bx	lr

00015a9c <__div0>:
   15a9c:	e92d4002 	push	{r1, lr}
   15aa0:	e3a00008 	mov	r0, #8	; 0x8
   15aa4:	ebfffcaa 	bl	14d54 <raise>
   15aa8:	e8bd8002 	pop	{r1, pc}
