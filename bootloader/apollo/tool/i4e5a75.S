
/home/zhuangjianwei/Downloads/soft/job/projects/irom_bitbucket/infotm-irom/out/vmirom:     file format elf32-littlearm


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
      44:	3c00d130 	.word	0x3c00d130

00000048 <_bss_end>:
      48:	3c00dc2c 	.word	0x3c00dc2c

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
      78:	eb0003c8 	bl	fa0 <chip_test>
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
      9c:	00000504 	.word	0x00000504

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
     158:	e24dd01c 	sub	sp, sp, #28	; 0x1c
     15c:	e1a0b000 	mov	fp, r0
     160:	e3a01001 	mov	r1, #1	; 0x1
     164:	e1a00004 	mov	r0, r4
     168:	e1a0a002 	mov	sl, r2
     16c:	eb001241 	bl	4a78 <vs_assign_by_id>
     170:	e2505000 	subs	r5, r0, #0	; 0x0
     174:	0a000008 	beq	19c <bl_load_isi+0x4c>
     178:	e59f315c 	ldr	r3, [pc, #348]	; 2dc <bl_load_isi+0x18c>
     17c:	e1a00004 	mov	r0, r4
     180:	e5934000 	ldr	r4, [r3]
     184:	eb0012a0 	bl	4c0c <vs_device_name>
     188:	e1a02005 	mov	r2, r5
     18c:	e1a01000 	mov	r1, r0
     190:	e59f0148 	ldr	r0, [pc, #328]	; 2e0 <bl_load_isi+0x190>
     194:	e12fff34 	blx	r4
     198:	ea00004c 	b	2d0 <bl_load_isi+0x180>
     19c:	e59f2140 	ldr	r2, [pc, #320]	; 2e4 <bl_load_isi+0x194>
     1a0:	e89a0003 	ldm	sl, {r0, r1}
     1a4:	e3a03000 	mov	r3, #0	; 0x0
     1a8:	e0922000 	adds	r2, r2, r0
     1ac:	e59f0134 	ldr	r0, [pc, #308]	; 2e8 <bl_load_isi+0x198>
     1b0:	e0a33001 	adc	r3, r3, r1
     1b4:	e3e01000 	mvn	r1, #0	; 0x0
     1b8:	e0022000 	and	r2, r2, r0
     1bc:	e0033001 	and	r3, r3, r1
     1c0:	e88a000c 	stm	sl, {r2, r3}
     1c4:	e1a06005 	mov	r6, r5
     1c8:	e1a07005 	mov	r7, r5
     1cc:	e58d5008 	str	r5, [sp, #8]
     1d0:	e58d500c 	str	r5, [sp, #12]
     1d4:	ea00000f 	b	218 <bl_load_isi+0xc8>
     1d8:	e3570b01 	cmp	r7, #1024	; 0x400
     1dc:	1a00000d 	bne	218 <bl_load_isi+0xc8>
     1e0:	e1a0000b 	mov	r0, fp
     1e4:	eb00055f 	bl	1768 <isi_check_header>
     1e8:	e2502000 	subs	r2, r0, #0	; 0x0
     1ec:	e59f00f8 	ldr	r0, [pc, #248]	; 2ec <bl_load_isi+0x19c>
     1f0:	1a000006 	bne	210 <bl_load_isi+0xc0>
     1f4:	e59b300c 	ldr	r3, [fp, #12]
     1f8:	e2836c02 	add	r6, r3, #512	; 0x200
     1fc:	e1a01006 	mov	r1, r6
     200:	e59f30d4 	ldr	r3, [pc, #212]	; 2dc <bl_load_isi+0x18c>
     204:	e1a0e00f 	mov	lr, pc
     208:	e593f000 	ldr	pc, [r3]
     20c:	ea000001 	b	218 <bl_load_isi+0xc8>
     210:	e59f00d8 	ldr	r0, [pc, #216]	; 2f0 <bl_load_isi+0x1a0>
     214:	ea00000b 	b	248 <bl_load_isi+0xf8>
     218:	e2564000 	subs	r4, r6, #0	; 0x0
     21c:	13a04001 	movne	r4, #1	; 0x1
     220:	e1570006 	cmp	r7, r6
     224:	b3a04000 	movlt	r4, #0	; 0x0
     228:	e3540000 	cmp	r4, #0	; 0x0
     22c:	e08b0007 	add	r0, fp, r7
     230:	0a000006 	beq	250 <bl_load_isi+0x100>
     234:	e1a0000b 	mov	r0, fp
     238:	eb0005a3 	bl	18cc <isi_check_data>
     23c:	e3500000 	cmp	r0, #0	; 0x0
     240:	0a000022 	beq	2d0 <bl_load_isi+0x180>
     244:	e59f00a8 	ldr	r0, [pc, #168]	; 2f4 <bl_load_isi+0x1a4>
     248:	e3a01000 	mov	r1, #0	; 0x0
     24c:	ea000004 	b	264 <bl_load_isi+0x114>
     250:	e59d800c 	ldr	r8, [sp, #12]
     254:	e3580004 	cmp	r8, #4	; 0x4
     258:	da000005 	ble	274 <bl_load_isi+0x124>
     25c:	e59f0090 	ldr	r0, [pc, #144]	; 2f4 <bl_load_isi+0x1a4>
     260:	e1a01004 	mov	r1, r4
     264:	e59f3070 	ldr	r3, [pc, #112]	; 2dc <bl_load_isi+0x18c>
     268:	e1a0e00f 	mov	lr, pc
     26c:	e593f000 	ldr	pc, [r3]
     270:	ea000015 	b	2cc <bl_load_isi+0x17c>
     274:	e89a000c 	ldm	sl, {r2, r3}
     278:	e3a09b01 	mov	r9, #1024	; 0x400
     27c:	e58d9000 	str	r9, [sp]
     280:	e58d4004 	str	r4, [sp, #4]
     284:	eb0012d8 	bl	4dec <vs_read>
     288:	e89a000c 	ldm	sl, {r2, r3}
     28c:	e3a08b01 	mov	r8, #1024	; 0x400
     290:	e3a09000 	mov	r9, #0	; 0x0
     294:	e0922008 	adds	r2, r2, r8
     298:	e0a33009 	adc	r3, r3, r9
     29c:	e3500b01 	cmp	r0, #1024	; 0x400
     2a0:	159d900c 	ldrne	r9, [sp, #12]
     2a4:	e59d1008 	ldr	r1, [sp, #8]
     2a8:	12899001 	addne	r9, r9, #1	; 0x1
     2ac:	e2811001 	add	r1, r1, #1	; 0x1
     2b0:	02877b01 	addeq	r7, r7, #1024	; 0x400
     2b4:	058d400c 	streq	r4, [sp, #12]
     2b8:	158d900c 	strne	r9, [sp, #12]
     2bc:	e3510080 	cmp	r1, #128	; 0x80
     2c0:	e88a000c 	stm	sl, {r2, r3}
     2c4:	e58d1008 	str	r1, [sp, #8]
     2c8:	1affffc2 	bne	1d8 <bl_load_isi+0x88>
     2cc:	e3e05000 	mvn	r5, #0	; 0x0
     2d0:	e1a00005 	mov	r0, r5
     2d4:	e28dd01c 	add	sp, sp, #28	; 0x1c
     2d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     2dc:	3c00d68c 	.word	0x3c00d68c
     2e0:	0000fa71 	.word	0x0000fa71
     2e4:	0001ffff 	.word	0x0001ffff
     2e8:	fffe0000 	.word	0xfffe0000
     2ec:	0000fa95 	.word	0x0000fa95
     2f0:	0000faac 	.word	0x0000faac
     2f4:	0000fabe 	.word	0x0000fabe

000002f8 <try_get_bl>:
     2f8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     2fc:	e3a02602 	mov	r2, #2097152	; 0x200000
     300:	e24dd00c 	sub	sp, sp, #12	; 0xc
     304:	e3a03000 	mov	r3, #0	; 0x0
     308:	e88d000c 	stm	sp, {r2, r3}
     30c:	e1a07001 	mov	r7, r1
     310:	e1a06000 	mov	r6, r0
     314:	eb0000ab 	bl	5c8 <boot_device>
     318:	e1a04000 	mov	r4, r0
     31c:	eb0011f3 	bl	4af0 <vs_device_bootable>
     320:	e2505000 	subs	r5, r0, #0	; 0x0
     324:	1a000008 	bne	34c <try_get_bl+0x54>
     328:	e59f308c 	ldr	r3, [pc, #140]	; 3bc <try_get_bl+0xc4>
     32c:	e1a00004 	mov	r0, r4
     330:	e5934000 	ldr	r4, [r3]
     334:	eb001234 	bl	4c0c <vs_device_name>
     338:	e1a01000 	mov	r1, r0
     33c:	e59f007c 	ldr	r0, [pc, #124]	; 3c0 <try_get_bl+0xc8>
     340:	e12fff34 	blx	r4
     344:	e1a00005 	mov	r0, r5
     348:	ea000019 	b	3b4 <try_get_bl+0xbc>
     34c:	e1a00004 	mov	r0, r4
     350:	eb00121b 	bl	4bc4 <vs_device_boot0>
     354:	e3500000 	cmp	r0, #0	; 0x0
     358:	13a02000 	movne	r2, #0	; 0x0
     35c:	13a03000 	movne	r3, #0	; 0x0
     360:	188d000c 	stmne	sp, {r2, r3}
     364:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
     368:	e1a01004 	mov	r1, r4
     36c:	e1a0200d 	mov	r2, sp
     370:	e3a03000 	mov	r3, #0	; 0x0
     374:	ebffff75 	bl	150 <bl_load_isi>
     378:	e3500000 	cmp	r0, #0	; 0x0
     37c:	13a00000 	movne	r0, #0	; 0x0
     380:	1a00000b 	bne	3b4 <try_get_bl+0xbc>
     384:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     388:	e5932008 	ldr	r2, [r3, #8]
     38c:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     390:	e5862000 	str	r2, [r6]
     394:	e5874000 	str	r4, [r7]
     398:	e5933008 	ldr	r3, [r3, #8]
     39c:	e3530000 	cmp	r3, #0	; 0x0
     3a0:	aa000002 	bge	3b0 <try_get_bl+0xb8>
     3a4:	e3a00001 	mov	r0, #1	; 0x1
     3a8:	e59f1014 	ldr	r1, [pc, #20]	; 3c4 <try_get_bl+0xcc>
     3ac:	eb0016b3 	bl	5e80 <cdbg_verify_burn_enable>
     3b0:	e3a00001 	mov	r0, #1	; 0x1
     3b4:	e28dd00c 	add	sp, sp, #12	; 0xc
     3b8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     3bc:	3c00d68c 	.word	0x3c00d68c
     3c0:	0000fad0 	.word	0x0000fad0
     3c4:	3c000100 	.word	0x3c000100

000003c8 <try_boot>:
     3c8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     3cc:	e3a03000 	mov	r3, #0	; 0x0
     3d0:	e24dd008 	sub	sp, sp, #8	; 0x8
     3d4:	e1a07000 	mov	r7, r0
     3d8:	e1a00003 	mov	r0, r3
     3dc:	e58d3004 	str	r3, [sp, #4]
     3e0:	e58d3000 	str	r3, [sp]
     3e4:	e1a04003 	mov	r4, r3
     3e8:	eb00005c 	bl	560 <set_xom>
     3ec:	eb000062 	bl	57c <boot_info>
     3f0:	e28d8004 	add	r8, sp, #4	; 0x4
     3f4:	e1a05000 	mov	r5, r0
     3f8:	ea000008 	b	420 <try_boot+0x58>
     3fc:	eb000158 	bl	964 <gtc_time>
     400:	e1a00004 	mov	r0, r4
     404:	eb000055 	bl	560 <set_xom>
     408:	e1a00008 	mov	r0, r8
     40c:	e1a0100d 	mov	r1, sp
     410:	ebffffb8 	bl	2f8 <try_get_bl>
     414:	e3500000 	cmp	r0, #0	; 0x0
     418:	13a04001 	movne	r4, #1	; 0x1
     41c:	1a000003 	bne	430 <try_boot+0x68>
     420:	e1540005 	cmp	r4, r5
     424:	e2844001 	add	r4, r4, #1	; 0x1
     428:	bafffff3 	blt	3fc <try_boot+0x34>
     42c:	e3a04000 	mov	r4, #0	; 0x0
     430:	eb00014b 	bl	964 <gtc_time>
     434:	e3a00001 	mov	r0, #1	; 0x1
     438:	eb00015f 	bl	9bc <ecfg_check_flag>
     43c:	e3500000 	cmp	r0, #0	; 0x0
     440:	1a00000c 	bne	478 <try_boot+0xb0>
     444:	e3570000 	cmp	r7, #0	; 0x0
     448:	1a00000a 	bne	478 <try_boot+0xb0>
     44c:	eb0000c2 	bl	75c <get_reset_stats>
     450:	e3500000 	cmp	r0, #0	; 0x0
     454:	13500002 	cmpne	r0, #2	; 0x2
     458:	1a000006 	bne	478 <try_boot+0xb0>
     45c:	e3500002 	cmp	r0, #2	; 0x2
     460:	1a000001 	bne	46c <try_boot+0xa4>
     464:	e28d0004 	add	r0, sp, #4	; 0x4
     468:	eb0000c8 	bl	790 <update_mask_on_reset>
     46c:	e59d0004 	ldr	r0, [sp, #4]
     470:	eb001849 	bl	659c <burn>
     474:	eb00013a 	bl	964 <gtc_time>
     478:	e3540000 	cmp	r4, #0	; 0x0
     47c:	0a000004 	beq	494 <try_boot+0xcc>
     480:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     484:	e5930014 	ldr	r0, [r3, #20]
     488:	e59d1000 	ldr	r1, [sp]
     48c:	eb001686 	bl	5eac <cdbg_jump>
     490:	eafffffe 	b	490 <try_boot+0xc8>
     494:	eb0016b6 	bl	5f74 <cdbg_shell>
     498:	eafffffe 	b	498 <try_boot+0xd0>

0000049c <init_data_section>:
     49c:	e59f1010 	ldr	r1, [pc, #16]	; 4b4 <init_data_section+0x18>
     4a0:	e59f0010 	ldr	r0, [pc, #16]	; 4b8 <init_data_section+0x1c>
     4a4:	e1510000 	cmp	r1, r0
     4a8:	012fff1e 	bxeq	lr
     4ac:	e59f2008 	ldr	r2, [pc, #8]	; 4bc <init_data_section+0x20>
     4b0:	ea000344 	b	11c8 <memcpy>
     4b4:	00011cac 	.word	0x00011cac
     4b8:	3c00d000 	.word	0x3c00d000
     4bc:	00000130 	.word	0x00000130

000004c0 <initialization>:
     4c0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     4c4:	e24dd004 	sub	sp, sp, #4	; 0x4
     4c8:	ebfffff3 	bl	49c <init_data_section>
     4cc:	eb0001b1 	bl	b98 <init_efuse>
     4d0:	eb000027 	bl	574 <init_xom>
     4d4:	eb0000e0 	bl	85c <init_lowlevel>
     4d8:	eb000464 	bl	1670 <init_malloc>
     4dc:	eb0020c9 	bl	8808 <init_timer>
     4e0:	eb00012c 	bl	998 <init_gtc>
     4e4:	eb00205a 	bl	8654 <init_serial>
     4e8:	eb001986 	bl	6b08 <init_mmc>
     4ec:	eb000099 	bl	758 <init_crypto>
     4f0:	eb000265 	bl	e8c <init_alternate>
     4f4:	eb00001f 	bl	578 <init_boot_sequence>
     4f8:	e3a00000 	mov	r0, #0	; 0x0
     4fc:	e28dd004 	add	sp, sp, #4	; 0x4
     500:	e8bd8000 	pop	{pc}

00000504 <main>:
     504:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     508:	e3a00000 	mov	r0, #0	; 0x0
     50c:	e24dd004 	sub	sp, sp, #4	; 0x4
     510:	eb00163e 	bl	5e10 <cdbg_log_toggle>
     514:	ebffffe9 	bl	4c0 <initialization>
     518:	eb000111 	bl	964 <gtc_time>
     51c:	e3a00001 	mov	r0, #1	; 0x1
     520:	eb00163a 	bl	5e10 <cdbg_log_toggle>
     524:	e59f001c 	ldr	r0, [pc, #28]	; 548 <main+0x44>
     528:	e59f301c 	ldr	r3, [pc, #28]	; 54c <main+0x48>
     52c:	e1a0e00f 	mov	lr, pc
     530:	e593f000 	ldr	pc, [r3]
     534:	e3a00000 	mov	r0, #0	; 0x0
     538:	eb001634 	bl	5e10 <cdbg_log_toggle>
     53c:	e3a00000 	mov	r0, #0	; 0x0
     540:	ebffffa0 	bl	3c8 <try_boot>
     544:	eafffffe 	b	544 <main+0x40>
     548:	0000faea 	.word	0x0000faea
     54c:	3c00d68c 	.word	0x3c00d68c

00000550 <get_xom>:
     550:	e59f3004 	ldr	r3, [pc, #4]	; 55c <get_xom+0xc>
     554:	e1d300b0 	ldrh	r0, [r3]
     558:	e12fff1e 	bx	lr
     55c:	3c00d130 	.word	0x3c00d130

00000560 <set_xom>:
     560:	e59f3008 	ldr	r3, [pc, #8]	; 570 <set_xom+0x10>
     564:	e200001f 	and	r0, r0, #31	; 0x1f
     568:	e1c300b0 	strh	r0, [r3]
     56c:	e12fff1e 	bx	lr
     570:	3c00d130 	.word	0x3c00d130

00000574 <init_xom>:
     574:	e12fff1e 	bx	lr

00000578 <init_boot_sequence>:
     578:	e12fff1e 	bx	lr

0000057c <boot_info>:
     57c:	e92d4010 	push	{r4, lr}
     580:	eb000116 	bl	9e0 <ecfg_paras>
     584:	e1a04000 	mov	r4, r0
     588:	e3a00b01 	mov	r0, #1024	; 0x400
     58c:	eb00010a 	bl	9bc <ecfg_check_flag>
     590:	e59f3028 	ldr	r3, [pc, #40]	; 5c0 <boot_info+0x44>
     594:	e3500000 	cmp	r0, #0	; 0x0
     598:	11d330b0 	ldrhne	r3, [r3]
     59c:	01d330b0 	ldrheq	r3, [r3]
     5a0:	1203301f 	andne	r3, r3, #31	; 0x1f
     5a4:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5a8:	159f2014 	ldrne	r2, [pc, #20]	; 5c4 <boot_info+0x48>
     5ac:	11a03083 	lslne	r3, r3, #1
     5b0:	00843083 	addeq	r3, r4, r3, lsl #1
     5b4:	119200b3 	ldrhne	r0, [r2, r3]
     5b8:	01d302b0 	ldrheq	r0, [r3, #32]
     5bc:	e8bd8010 	pop	{r4, pc}
     5c0:	3c00d130 	.word	0x3c00d130
     5c4:	0000f6d4 	.word	0x0000f6d4

000005c8 <boot_device>:
     5c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     5cc:	e24dd004 	sub	sp, sp, #4	; 0x4
     5d0:	ebffffe9 	bl	57c <boot_info>
     5d4:	e1a00620 	lsr	r0, r0, #12
     5d8:	e6ff0070 	uxth	r0, r0
     5dc:	e28dd004 	add	sp, sp, #4	; 0x4
     5e0:	e8bd8000 	pop	{pc}

000005e4 <set_pll>:
     5e4:	e3500003 	cmp	r0, #3	; 0x3
     5e8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     5ec:	e1a02001 	mov	r2, r1
     5f0:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     5f4:	e3a03018 	mov	r3, #24	; 0x18
     5f8:	e0030093 	mul	r3, r3, r0
     5fc:	e59f808c 	ldr	r8, [pc, #140]	; 690 <set_pll+0xac>
     600:	e59f108c 	ldr	r1, [pc, #140]	; 694 <set_pll+0xb0>
     604:	e59f008c 	ldr	r0, [pc, #140]	; 698 <set_pll+0xb4>
     608:	e59fc08c 	ldr	ip, [pc, #140]	; 69c <set_pll+0xb8>
     60c:	e59f408c 	ldr	r4, [pc, #140]	; 6a0 <set_pll+0xbc>
     610:	e59fa08c 	ldr	sl, [pc, #140]	; 6a4 <set_pll+0xc0>
     614:	e0838008 	add	r8, r3, r8
     618:	e083c00c 	add	ip, r3, ip
     61c:	e0831001 	add	r1, r3, r1
     620:	e3a07001 	mov	r7, #1	; 0x1
     624:	e0830000 	add	r0, r3, r0
     628:	e1a06422 	lsr	r6, r2, #8
     62c:	e0834004 	add	r4, r3, r4
     630:	e3a05000 	mov	r5, #0	; 0x0
     634:	e6ef2072 	uxtb	r2, r2
     638:	e083a00a 	add	sl, r3, sl
     63c:	e3a03002 	mov	r3, #2	; 0x2
     640:	e5c87000 	strb	r7, [r8]
     644:	e5c12000 	strb	r2, [r1]
     648:	e5c06000 	strb	r6, [r0]
     64c:	e5cc5000 	strb	r5, [ip]
     650:	e5c43000 	strb	r3, [r4]
     654:	e0833007 	add	r3, r3, r7
     658:	e5c83000 	strb	r3, [r8]
     65c:	e1a02005 	mov	r2, r5
     660:	e5cc7000 	strb	r7, [ip]
     664:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     668:	ea000001 	b	674 <set_pll+0x90>
     66c:	e1520001 	cmp	r2, r1
     670:	0a000003 	beq	684 <set_pll+0xa0>
     674:	e5da3000 	ldrb	r3, [sl]
     678:	e2822001 	add	r2, r2, #1	; 0x1
     67c:	e3130001 	tst	r3, #1	; 0x1
     680:	0afffff9 	beq	66c <set_pll+0x88>
     684:	e3a03002 	mov	r3, #2	; 0x2
     688:	e5883000 	str	r3, [r8]
     68c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     690:	21e0a00c 	.word	0x21e0a00c
     694:	21e0a000 	.word	0x21e0a000
     698:	21e0a004 	.word	0x21e0a004
     69c:	21e0a010 	.word	0x21e0a010
     6a0:	21e0a008 	.word	0x21e0a008
     6a4:	21e0a014 	.word	0x21e0a014

000006a8 <set_clk_divider>:
     6a8:	e3500005 	cmp	r0, #5	; 0x5
     6ac:	812fff1e 	bxhi	lr
     6b0:	e59fc020 	ldr	ip, [pc, #32]	; 6d8 <set_clk_divider+0x30>
     6b4:	e1a03280 	lsl	r3, r0, #5
     6b8:	e083c00c 	add	ip, r3, ip
     6bc:	e3a02000 	mov	r2, #0	; 0x0
     6c0:	e7d13002 	ldrb	r3, [r1, r2]
     6c4:	e2822001 	add	r2, r2, #1	; 0x1
     6c8:	e3520008 	cmp	r2, #8	; 0x8
     6cc:	e4cc3004 	strb	r3, [ip], #4
     6d0:	1afffffa 	bne	6c0 <set_clk_divider+0x18>
     6d4:	e12fff1e 	bx	lr
     6d8:	21e0a070 	.word	0x21e0a070

000006dc <module_set_clock>:
     6dc:	e59f3020 	ldr	r3, [pc, #32]	; 704 <module_set_clock+0x28>
     6e0:	e1500003 	cmp	r0, r3
     6e4:	13a03001 	movne	r3, #1	; 0x1
     6e8:	1580300c 	strne	r3, [r0, #12]
     6ec:	e2013007 	and	r3, r1, #7	; 0x7
     6f0:	e5803000 	str	r3, [r0]
     6f4:	e3a03000 	mov	r3, #0	; 0x0
     6f8:	e5802004 	str	r2, [r0, #4]
     6fc:	e580300c 	str	r3, [r0, #12]
     700:	e12fff1e 	bx	lr
     704:	21e0a360 	.word	0x21e0a360

00000708 <module_enable>:
     708:	e3a020ff 	mov	r2, #255	; 0xff
     70c:	e3a01000 	mov	r1, #0	; 0x0
     710:	e3a03011 	mov	r3, #17	; 0x11
     714:	e580100c 	str	r1, [r0, #12]
     718:	e5802000 	str	r2, [r0]
     71c:	e5802004 	str	r2, [r0, #4]
     720:	e5803008 	str	r3, [r0, #8]
     724:	e5801014 	str	r1, [r0, #20]
     728:	e5903008 	ldr	r3, [r0, #8]
     72c:	e3c33010 	bic	r3, r3, #16	; 0x10
     730:	e5803008 	str	r3, [r0, #8]
     734:	e5802018 	str	r2, [r0, #24]
     738:	e5801000 	str	r1, [r0]
     73c:	e580200c 	str	r2, [r0, #12]
     740:	e12fff1e 	bx	lr

00000744 <init_igps_ram>:
     744:	e59f3008 	ldr	r3, [pc, #8]	; 754 <init_igps_ram+0x10>
     748:	e3a02001 	mov	r2, #1	; 0x1
     74c:	e5832c10 	str	r2, [r3, #3088]
     750:	e12fff1e 	bx	lr
     754:	21e0a000 	.word	0x21e0a000

00000758 <init_crypto>:
     758:	e12fff1e 	bx	lr

0000075c <get_reset_stats>:
     75c:	e59f2028 	ldr	r2, [pc, #40]	; 78c <get_reset_stats+0x30>
     760:	e5923c10 	ldr	r3, [r2, #3088]
     764:	e2130002 	ands	r0, r3, #2	; 0x2
     768:	012fff1e 	bxeq	lr
     76c:	e5920c2c 	ldr	r0, [r2, #3116]
     770:	e3100001 	tst	r0, #1	; 0x1
     774:	03a00001 	moveq	r0, #1	; 0x1
     778:	012fff1e 	bxeq	lr
     77c:	e3500037 	cmp	r0, #55	; 0x37
     780:	03a00002 	moveq	r0, #2	; 0x2
     784:	13a00003 	movne	r0, #3	; 0x3
     788:	e12fff1e 	bx	lr
     78c:	21e09000 	.word	0x21e09000

00000790 <update_mask_on_reset>:
     790:	e59f3018 	ldr	r3, [pc, #24]	; 7b0 <update_mask_on_reset+0x20>
     794:	e5932c30 	ldr	r2, [r3, #3120]
     798:	e3120080 	tst	r2, #128	; 0x80
     79c:	13a03001 	movne	r3, #1	; 0x1
     7a0:	1202200f 	andne	r2, r2, #15	; 0xf
     7a4:	11e03213 	mvnne	r3, r3, lsl r2
     7a8:	15803000 	strne	r3, [r0]
     7ac:	e12fff1e 	bx	lr
     7b0:	21e09000 	.word	0x21e09000

000007b4 <get_wakeup_entry>:
     7b4:	e59f301c 	ldr	r3, [pc, #28]	; 7d8 <get_wakeup_entry+0x24>
     7b8:	e5931c38 	ldr	r1, [r3, #3128]
     7bc:	e5932c34 	ldr	r2, [r3, #3124]
     7c0:	e5930c30 	ldr	r0, [r3, #3120]
     7c4:	e1822401 	orr	r2, r2, r1, lsl #8
     7c8:	e5933c2c 	ldr	r3, [r3, #3116]
     7cc:	e1800402 	orr	r0, r0, r2, lsl #8
     7d0:	e1830400 	orr	r0, r3, r0, lsl #8
     7d4:	e12fff1e 	bx	lr
     7d8:	21e09000 	.word	0x21e09000

000007dc <system_reset>:
     7dc:	e59f3010 	ldr	r3, [pc, #16]	; 7f4 <system_reset+0x18>
     7e0:	e3a02001 	mov	r2, #1	; 0x1
     7e4:	e5832c2c 	str	r2, [r3, #3116]
     7e8:	e2822002 	add	r2, r2, #2	; 0x2
     7ec:	e5832c00 	str	r2, [r3, #3072]
     7f0:	e12fff1e 	bx	lr
     7f4:	21e09000 	.word	0x21e09000

000007f8 <pads_init_module>:
     7f8:	e92d4030 	push	{r4, r5, lr}
     7fc:	e1a05000 	mov	r5, r0
     800:	e24dd004 	sub	sp, sp, #4	; 0x4
     804:	e59f404c 	ldr	r4, [pc, #76]	; 858 <pads_init_module+0x60>
     808:	ea00000d 	b	844 <pads_init_module+0x4c>
     80c:	e1500005 	cmp	r0, r5
     810:	1a00000a 	bne	840 <pads_init_module+0x48>
     814:	e3a01000 	mov	r1, #0	; 0x0
     818:	e1a02001 	mov	r2, r1
     81c:	e5140004 	ldr	r0, [r4, #-4]
     820:	eb0000f9 	bl	c0c <pads_chmod>
     824:	e3a00101 	mov	r0, #1073741824	; 0x40000000
     828:	eb000063 	bl	9bc <ecfg_check_flag>
     82c:	e3500000 	cmp	r0, #0	; 0x0
     830:	0a000002 	beq	840 <pads_init_module+0x48>
     834:	e8140005 	ldmda	r4, {r0, r2}
     838:	e3a01001 	mov	r1, #1	; 0x1
     83c:	eb00013e 	bl	d3c <pads_pull>
     840:	e284400c 	add	r4, r4, #12	; 0xc
     844:	e5140008 	ldr	r0, [r4, #-8]
     848:	e3500000 	cmp	r0, #0	; 0x0
     84c:	1affffee 	bne	80c <pads_init_module+0x14>
     850:	e28dd004 	add	sp, sp, #4	; 0x4
     854:	e8bd8030 	pop	{r4, r5, pc}
     858:	3c00d008 	.word	0x3c00d008

0000085c <init_lowlevel>:
     85c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     860:	e24dd044 	sub	sp, sp, #68	; 0x44
     864:	e3a01000 	mov	r1, #0	; 0x0
     868:	e3a02040 	mov	r2, #64	; 0x40
     86c:	e1a0000d 	mov	r0, sp
     870:	eb00024c 	bl	11a8 <memset>
     874:	e3a03063 	mov	r3, #99	; 0x63
     878:	e5cd3010 	strb	r3, [sp, #16]
     87c:	e3e03073 	mvn	r3, #115	; 0x73
     880:	e5cd3011 	strb	r3, [sp, #17]
     884:	e28330a5 	add	r3, r3, #165	; 0xa5
     888:	e5cd3012 	strb	r3, [sp, #18]
     88c:	e3a0301f 	mov	r3, #31	; 0x1f
     890:	e5cd3018 	strb	r3, [sp, #24]
     894:	e283305d 	add	r3, r3, #93	; 0x5d
     898:	e5cd3019 	strb	r3, [sp, #25]
     89c:	e3e0300f 	mvn	r3, #15	; 0xf
     8a0:	e5cd301a 	strb	r3, [sp, #26]
     8a4:	e2833020 	add	r3, r3, #32	; 0x20
     8a8:	e5cd3020 	strb	r3, [sp, #32]
     8ac:	e2833032 	add	r3, r3, #50	; 0x32
     8b0:	e5cd3021 	strb	r3, [sp, #33]
     8b4:	e3a03002 	mov	r3, #2	; 0x2
     8b8:	e5cd3029 	strb	r3, [sp, #41]
     8bc:	e3e0301f 	mvn	r3, #31	; 0x1f
     8c0:	e5cd3030 	strb	r3, [sp, #48]
     8c4:	e3e0307c 	mvn	r3, #124	; 0x7c
     8c8:	e5cd3031 	strb	r3, [sp, #49]
     8cc:	e283308c 	add	r3, r3, #140	; 0x8c
     8d0:	e3a02008 	mov	r2, #8	; 0x8
     8d4:	e5cd3032 	strb	r3, [sp, #50]
     8d8:	e3a03001 	mov	r3, #1	; 0x1
     8dc:	e5cd202a 	strb	r2, [sp, #42]
     8e0:	e58d3038 	str	r3, [sp, #56]
     8e4:	e5cd2022 	strb	r2, [sp, #34]
     8e8:	e3a01000 	mov	r1, #0	; 0x0
     8ec:	e3e00000 	mvn	r0, #0	; 0x0
     8f0:	e59f3044 	ldr	r3, [pc, #68]	; 93c <init_lowlevel+0xe0>
     8f4:	e1a02101 	lsl	r2, r1, #2
     8f8:	e2811001 	add	r1, r1, #1	; 0x1
     8fc:	e0823003 	add	r3, r2, r3
     900:	e3510008 	cmp	r1, #8	; 0x8
     904:	e5830000 	str	r0, [r3]
     908:	1afffff8 	bne	8f0 <init_lowlevel+0x94>
     90c:	e3012042 	movw	r2, #4162	; 0x1042
     910:	e28d0040 	add	r0, sp, #64	; 0x40
     914:	e3013031 	movw	r3, #4145	; 0x1031
     918:	e16024b0 	strh	r2, [r0, #-64]!
     91c:	e1cd30b4 	strh	r3, [sp, #4]
     920:	e1a0000d 	mov	r0, sp
     924:	e302304f 	movw	r3, #8271	; 0x204f
     928:	e1cd30b2 	strh	r3, [sp, #2]
     92c:	e1cd20b6 	strh	r2, [sp, #6]
     930:	eb00015d 	bl	eac <set_clk>
     934:	e28dd044 	add	sp, sp, #68	; 0x44
     938:	e8bd8000 	pop	{pc}
     93c:	21b01180 	.word	0x21b01180

00000940 <module_reset>:
     940:	e3a030ff 	mov	r3, #255	; 0xff
     944:	e5803000 	str	r3, [r0]
     948:	e92d4010 	push	{r4, lr}
     94c:	e1a04000 	mov	r4, r0
     950:	e3a0000a 	mov	r0, #10	; 0xa
     954:	eb001f67 	bl	86f8 <udelay>
     958:	e3a03000 	mov	r3, #0	; 0x0
     95c:	e5843000 	str	r3, [r4]
     960:	e8bd8010 	pop	{r4, pc}

00000964 <gtc_time>:
     964:	e59f3020 	ldr	r3, [pc, #32]	; 98c <gtc_time+0x28>
     968:	e92d4010 	push	{r4, lr}
     96c:	e5930000 	ldr	r0, [r3]
     970:	e59f3018 	ldr	r3, [pc, #24]	; 990 <gtc_time+0x2c>
     974:	e5934000 	ldr	r4, [r3]
     978:	eb001f82 	bl	8788 <get_timer>
     97c:	e1a01000 	mov	r1, r0
     980:	e59f000c 	ldr	r0, [pc, #12]	; 994 <gtc_time+0x30>
     984:	e12fff34 	blx	r4
     988:	e8bd8010 	pop	{r4, pc}
     98c:	3c00d134 	.word	0x3c00d134
     990:	3c00d68c 	.word	0x3c00d68c
     994:	0000faef 	.word	0x0000faef

00000998 <init_gtc>:
     998:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     99c:	e3a00000 	mov	r0, #0	; 0x0
     9a0:	e24dd004 	sub	sp, sp, #4	; 0x4
     9a4:	eb001f77 	bl	8788 <get_timer>
     9a8:	e59f3008 	ldr	r3, [pc, #8]	; 9b8 <init_gtc+0x20>
     9ac:	e5830000 	str	r0, [r3]
     9b0:	e28dd004 	add	sp, sp, #4	; 0x4
     9b4:	e8bd8000 	pop	{pc}
     9b8:	3c00d134 	.word	0x3c00d134

000009bc <ecfg_check_flag>:
     9bc:	e3500801 	cmp	r0, #65536	; 0x10000
     9c0:	0a000004 	beq	9d8 <ecfg_check_flag+0x1c>
     9c4:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     9c8:	0a000002 	beq	9d8 <ecfg_check_flag+0x1c>
     9cc:	e3500001 	cmp	r0, #1	; 0x1
     9d0:	13a00001 	movne	r0, #1	; 0x1
     9d4:	112fff1e 	bxne	lr
     9d8:	e3a00000 	mov	r0, #0	; 0x0
     9dc:	e12fff1e 	bx	lr

000009e0 <ecfg_paras>:
     9e0:	e59f0000 	ldr	r0, [pc, #0]	; 9e8 <ecfg_paras+0x8>
     9e4:	e12fff1e 	bx	lr
     9e8:	3c00d690 	.word	0x3c00d690

000009ec <efuse_mac>:
     9ec:	e3a00000 	mov	r0, #0	; 0x0
     9f0:	e3a01000 	mov	r1, #0	; 0x0
     9f4:	e12fff1e 	bx	lr

000009f8 <efuse_info>:
     9f8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     9fc:	e1a04000 	mov	r4, r0
     a00:	e24dd004 	sub	sp, sp, #4	; 0x4
     a04:	e59f6144 	ldr	r6, [pc, #324]	; b50 <efuse_info+0x158>
     a08:	e59f0144 	ldr	r0, [pc, #324]	; b54 <efuse_info+0x15c>
     a0c:	e1a0e00f 	mov	lr, pc
     a10:	e596f000 	ldr	pc, [r6]
     a14:	e59f013c 	ldr	r0, [pc, #316]	; b58 <efuse_info+0x160>
     a18:	e1a0e00f 	mov	lr, pc
     a1c:	e596f000 	ldr	pc, [r6]
     a20:	e3540000 	cmp	r4, #0	; 0x0
     a24:	0a00003c 	beq	b1c <efuse_info+0x124>
     a28:	e59f412c 	ldr	r4, [pc, #300]	; b5c <efuse_info+0x164>
     a2c:	e59f012c 	ldr	r0, [pc, #300]	; b60 <efuse_info+0x168>
     a30:	e5941000 	ldr	r1, [r4]
     a34:	e1a0e00f 	mov	lr, pc
     a38:	e596f000 	ldr	pc, [r6]
     a3c:	e5d41004 	ldrb	r1, [r4, #4]
     a40:	e59f011c 	ldr	r0, [pc, #284]	; b64 <efuse_info+0x16c>
     a44:	e1a0e00f 	mov	lr, pc
     a48:	e596f000 	ldr	pc, [r6]
     a4c:	e5d41005 	ldrb	r1, [r4, #5]
     a50:	e59f0110 	ldr	r0, [pc, #272]	; b68 <efuse_info+0x170>
     a54:	e1a0e00f 	mov	lr, pc
     a58:	e596f000 	ldr	pc, [r6]
     a5c:	e5d41006 	ldrb	r1, [r4, #6]
     a60:	e59f0104 	ldr	r0, [pc, #260]	; b6c <efuse_info+0x174>
     a64:	e1a0e00f 	mov	lr, pc
     a68:	e596f000 	ldr	pc, [r6]
     a6c:	e5d41007 	ldrb	r1, [r4, #7]
     a70:	e59f00f8 	ldr	r0, [pc, #248]	; b70 <efuse_info+0x178>
     a74:	e1a0e00f 	mov	lr, pc
     a78:	e596f000 	ldr	pc, [r6]
     a7c:	e1d410b8 	ldrh	r1, [r4, #8]
     a80:	e59f00ec 	ldr	r0, [pc, #236]	; b74 <efuse_info+0x17c>
     a84:	e1a0e00f 	mov	lr, pc
     a88:	e596f000 	ldr	pc, [r6]
     a8c:	e1d410ba 	ldrh	r1, [r4, #10]
     a90:	e59f00e0 	ldr	r0, [pc, #224]	; b78 <efuse_info+0x180>
     a94:	e1a0e00f 	mov	lr, pc
     a98:	e596f000 	ldr	pc, [r6]
     a9c:	e1d410bc 	ldrh	r1, [r4, #12]
     aa0:	e59f00d4 	ldr	r0, [pc, #212]	; b7c <efuse_info+0x184>
     aa4:	e1a0e00f 	mov	lr, pc
     aa8:	e596f000 	ldr	pc, [r6]
     aac:	e1d410be 	ldrh	r1, [r4, #14]
     ab0:	e59f00c8 	ldr	r0, [pc, #200]	; b80 <efuse_info+0x188>
     ab4:	e1a0e00f 	mov	lr, pc
     ab8:	e596f000 	ldr	pc, [r6]
     abc:	e1a05004 	mov	r5, r4
     ac0:	e3a04000 	mov	r4, #0	; 0x0
     ac4:	e1a01004 	mov	r1, r4
     ac8:	e5952010 	ldr	r2, [r5, #16]
     acc:	e2844001 	add	r4, r4, #1	; 0x1
     ad0:	e59f00ac 	ldr	r0, [pc, #172]	; b84 <efuse_info+0x18c>
     ad4:	e1a0e00f 	mov	lr, pc
     ad8:	e596f000 	ldr	pc, [r6]
     adc:	e3540004 	cmp	r4, #4	; 0x4
     ae0:	e2855004 	add	r5, r5, #4	; 0x4
     ae4:	1afffff6 	bne	ac4 <efuse_info+0xcc>
     ae8:	e59f6060 	ldr	r6, [pc, #96]	; b50 <efuse_info+0x158>
     aec:	e59f5068 	ldr	r5, [pc, #104]	; b5c <efuse_info+0x164>
     af0:	e284400c 	add	r4, r4, #12	; 0xc
     af4:	e1a03084 	lsl	r3, r4, #1
     af8:	e1a01004 	mov	r1, r4
     afc:	e19520b3 	ldrh	r2, [r5, r3]
     b00:	e2844001 	add	r4, r4, #1	; 0x1
     b04:	e59f007c 	ldr	r0, [pc, #124]	; b88 <efuse_info+0x190>
     b08:	e1a0e00f 	mov	lr, pc
     b0c:	e596f000 	ldr	pc, [r6]
     b10:	e354001c 	cmp	r4, #28	; 0x1c
     b14:	1afffff6 	bne	af4 <efuse_info+0xfc>
     b18:	ea00000a 	b	b48 <efuse_info+0x150>
     b1c:	e59f4068 	ldr	r4, [pc, #104]	; b8c <efuse_info+0x194>
     b20:	e59f7068 	ldr	r7, [pc, #104]	; b90 <efuse_info+0x198>
     b24:	e1a05006 	mov	r5, r6
     b28:	e5942000 	ldr	r2, [r4]
     b2c:	e1a01004 	mov	r1, r4
     b30:	e59f005c 	ldr	r0, [pc, #92]	; b94 <efuse_info+0x19c>
     b34:	e2844004 	add	r4, r4, #4	; 0x4
     b38:	e1a0e00f 	mov	lr, pc
     b3c:	e595f000 	ldr	pc, [r5]
     b40:	e1540007 	cmp	r4, r7
     b44:	1afffff7 	bne	b28 <efuse_info+0x130>
     b48:	e28dd004 	add	sp, sp, #4	; 0x4
     b4c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     b50:	3c00d68c 	.word	0x3c00d68c
     b54:	0000faf7 	.word	0x0000faf7
     b58:	0000fb0c 	.word	0x0000fb0c
     b5c:	3c00d690 	.word	0x3c00d690
     b60:	0000fb23 	.word	0x0000fb23
     b64:	0000fb32 	.word	0x0000fb32
     b68:	0000fb41 	.word	0x0000fb41
     b6c:	0000fb54 	.word	0x0000fb54
     b70:	0000fb63 	.word	0x0000fb63
     b74:	0000fb78 	.word	0x0000fb78
     b78:	0000fb85 	.word	0x0000fb85
     b7c:	0000fb95 	.word	0x0000fb95
     b80:	0000fba6 	.word	0x0000fba6
     b84:	0000fbb8 	.word	0x0000fbb8
     b88:	0000fbc8 	.word	0x0000fbc8
     b8c:	21e0d9b4 	.word	0x21e0d9b4
     b90:	21e0da94 	.word	0x21e0da94
     b94:	0000fbd9 	.word	0x0000fbd9

00000b98 <init_efuse>:
     b98:	e59f0008 	ldr	r0, [pc, #8]	; ba8 <init_efuse+0x10>
     b9c:	e3a01000 	mov	r1, #0	; 0x0
     ba0:	e3a02038 	mov	r2, #56	; 0x38
     ba4:	ea00017f 	b	11a8 <memset>
     ba8:	3c00d690 	.word	0x3c00d690

00000bac <assert_linkpc>:
     bac:	e3a00001 	mov	r0, #1	; 0x1
     bb0:	e12fff1e 	bx	lr

00000bb4 <pads_states>:
     bb4:	e3500000 	cmp	r0, #0	; 0x0
     bb8:	e2803007 	add	r3, r0, #7	; 0x7
     bbc:	a1a03000 	movge	r3, r0
     bc0:	e1a031c3 	asr	r3, r3, #3
     bc4:	e59f203c 	ldr	r2, [pc, #60]	; c08 <pads_states+0x54>
     bc8:	e1a03103 	lsl	r3, r3, #2
     bcc:	e0832002 	add	r2, r3, r2
     bd0:	e5921000 	ldr	r1, [r2]
     bd4:	e3a03001 	mov	r3, #1	; 0x1
     bd8:	e2002007 	and	r2, r0, #7	; 0x7
     bdc:	e0113213 	ands	r3, r1, r3, lsl r2
     be0:	01a00003 	moveq	r0, r3
     be4:	012fff1e 	bxeq	lr
     be8:	e1a03300 	lsl	r3, r0, #6
     bec:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     bf0:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     bf4:	e5933000 	ldr	r3, [r3]
     bf8:	e3530000 	cmp	r3, #0	; 0x0
     bfc:	13a00002 	movne	r0, #2	; 0x2
     c00:	03a00001 	moveq	r0, #1	; 0x1
     c04:	e12fff1e 	bx	lr
     c08:	21e0906c 	.word	0x21e0906c

00000c0c <pads_chmod>:
     c0c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     c10:	e1b07840 	asrs	r7, r0, #16
     c14:	e1a00800 	lsl	r0, r0, #16
     c18:	03a03000 	moveq	r3, #0	; 0x0
     c1c:	13a03001 	movne	r3, #1	; 0x1
     c20:	e1a00820 	lsr	r0, r0, #16
     c24:	e1570000 	cmp	r7, r0
     c28:	a3a0c000 	movge	ip, #0	; 0x0
     c2c:	b203c001 	andlt	ip, r3, #1	; 0x1
     c30:	e35c0000 	cmp	ip, #0	; 0x0
     c34:	e24dd004 	sub	sp, sp, #4	; 0x4
     c38:	e1a05001 	mov	r5, r1
     c3c:	e1a09002 	mov	r9, r2
     c40:	1a000038 	bne	d28 <pads_chmod+0x11c>
     c44:	e2803883 	add	r3, r0, #8585216	; 0x830000
     c48:	e2833903 	add	r3, r3, #49152	; 0xc000
     c4c:	e1a02303 	lsl	r2, r3, #6
     c50:	e3570000 	cmp	r7, #0	; 0x0
     c54:	01a07000 	moveq	r7, r0
     c58:	e2826008 	add	r6, r2, #8	; 0x8
     c5c:	e2828004 	add	r8, r2, #4	; 0x4
     c60:	e1a04000 	mov	r4, r0
     c64:	e3a0a001 	mov	sl, #1	; 0x1
     c68:	e59fb0c0 	ldr	fp, [pc, #192]	; d30 <pads_chmod+0x124>
     c6c:	ea00002a 	b	d1c <pads_chmod+0x110>
     c70:	e3540000 	cmp	r4, #0	; 0x0
     c74:	a1a03004 	movge	r3, r4
     c78:	e3550001 	cmp	r5, #1	; 0x1
     c7c:	e1a011c3 	asr	r1, r3, #3
     c80:	e204c007 	and	ip, r4, #7	; 0x7
     c84:	0a00000b 	beq	cb8 <pads_chmod+0xac>
     c88:	e3550002 	cmp	r5, #2	; 0x2
     c8c:	0a000012 	beq	cdc <pads_chmod+0xd0>
     c90:	e59f209c 	ldr	r2, [pc, #156]	; d34 <pads_chmod+0x128>
     c94:	e1a03101 	lsl	r3, r1, #2
     c98:	e2551000 	subs	r1, r5, #0	; 0x0
     c9c:	e0832002 	add	r2, r3, r2
     ca0:	e59f0090 	ldr	r0, [pc, #144]	; d38 <pads_chmod+0x12c>
     ca4:	1a000017 	bne	d08 <pads_chmod+0xfc>
     ca8:	e5923000 	ldr	r3, [r2]
     cac:	e1c33c1a 	bic	r3, r3, sl, lsl ip
     cb0:	e5823000 	str	r3, [r2]
     cb4:	ea000015 	b	d10 <pads_chmod+0x104>
     cb8:	e59f3074 	ldr	r3, [pc, #116]	; d34 <pads_chmod+0x128>
     cbc:	e1a02101 	lsl	r2, r1, #2
     cc0:	e0823003 	add	r3, r2, r3
     cc4:	e5932000 	ldr	r2, [r3]
     cc8:	e1822c15 	orr	r2, r2, r5, lsl ip
     ccc:	e5832000 	str	r2, [r3]
     cd0:	e3a03000 	mov	r3, #0	; 0x0
     cd4:	e5863000 	str	r3, [r6]
     cd8:	ea00000c 	b	d10 <pads_chmod+0x104>
     cdc:	e59f2050 	ldr	r2, [pc, #80]	; d34 <pads_chmod+0x128>
     ce0:	e1a03101 	lsl	r3, r1, #2
     ce4:	e0832002 	add	r2, r3, r2
     ce8:	e5923000 	ldr	r3, [r2]
     cec:	e2591000 	subs	r1, r9, #0	; 0x0
     cf0:	13a01001 	movne	r1, #1	; 0x1
     cf4:	e1833c1a 	orr	r3, r3, sl, lsl ip
     cf8:	e5823000 	str	r3, [r2]
     cfc:	e586a000 	str	sl, [r6]
     d00:	e5881000 	str	r1, [r8]
     d04:	ea000001 	b	d10 <pads_chmod+0x104>
     d08:	e1a0e00f 	mov	lr, pc
     d0c:	e59bf000 	ldr	pc, [fp]
     d10:	e2844001 	add	r4, r4, #1	; 0x1
     d14:	e2866040 	add	r6, r6, #64	; 0x40
     d18:	e2888040 	add	r8, r8, #64	; 0x40
     d1c:	e1540007 	cmp	r4, r7
     d20:	e2843007 	add	r3, r4, #7	; 0x7
     d24:	daffffd1 	ble	c70 <pads_chmod+0x64>
     d28:	e28dd004 	add	sp, sp, #4	; 0x4
     d2c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     d30:	3c00d68c 	.word	0x3c00d68c
     d34:	21e0906c 	.word	0x21e0906c
     d38:	0000fbe9 	.word	0x0000fbe9

00000d3c <pads_pull>:
     d3c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     d40:	e1b05840 	asrs	r5, r0, #16
     d44:	e1a00800 	lsl	r0, r0, #16
     d48:	03a03000 	moveq	r3, #0	; 0x0
     d4c:	13a03001 	movne	r3, #1	; 0x1
     d50:	e1a00820 	lsr	r0, r0, #16
     d54:	e1550000 	cmp	r5, r0
     d58:	a3a03000 	movge	r3, #0	; 0x0
     d5c:	b2033001 	andlt	r3, r3, #1	; 0x1
     d60:	e3530000 	cmp	r3, #0	; 0x0
     d64:	e1a0a001 	mov	sl, r1
     d68:	e1a08002 	mov	r8, r2
     d6c:	18bd85f0 	popne	{r4, r5, r6, r7, r8, sl, pc}
     d70:	e3550000 	cmp	r5, #0	; 0x0
     d74:	01a05000 	moveq	r5, r0
     d78:	e1a06003 	mov	r6, r3
     d7c:	e3a07001 	mov	r7, #1	; 0x1
     d80:	ea000018 	b	de8 <pads_pull+0xac>
     d84:	e3500000 	cmp	r0, #0	; 0x0
     d88:	a1a03000 	movge	r3, r0
     d8c:	e1a031c3 	asr	r3, r3, #3
     d90:	e1a01103 	lsl	r1, r3, #2
     d94:	e35a0000 	cmp	sl, #0	; 0x0
     d98:	e0812002 	add	r2, r1, r2
     d9c:	05923000 	ldreq	r3, [r2]
     da0:	e2004007 	and	r4, r0, #7	; 0x7
     da4:	e081c00c 	add	ip, r1, ip
     da8:	01c33417 	biceq	r3, r3, r7, lsl r4
     dac:	0a00000a 	beq	ddc <pads_pull+0xa0>
     db0:	e0183617 	ands	r3, r8, r7, lsl r6
     db4:	e1a02417 	lsl	r2, r7, r4
     db8:	159c3000 	ldrne	r3, [ip]
     dbc:	059c3000 	ldreq	r3, [ip]
     dc0:	11833002 	orrne	r3, r3, r2
     dc4:	01c33002 	biceq	r3, r3, r2
     dc8:	e59f2030 	ldr	r2, [pc, #48]	; e00 <pads_pull+0xc4>
     dcc:	e58c3000 	str	r3, [ip]
     dd0:	e0812002 	add	r2, r1, r2
     dd4:	e5923000 	ldr	r3, [r2]
     dd8:	e1833417 	orr	r3, r3, r7, lsl r4
     ddc:	e5823000 	str	r3, [r2]
     de0:	e2800001 	add	r0, r0, #1	; 0x1
     de4:	e2866001 	add	r6, r6, #1	; 0x1
     de8:	e1500005 	cmp	r0, r5
     dec:	e59f200c 	ldr	r2, [pc, #12]	; e00 <pads_pull+0xc4>
     df0:	e2803007 	add	r3, r0, #7	; 0x7
     df4:	e59fc008 	ldr	ip, [pc, #8]	; e04 <pads_pull+0xc8>
     df8:	daffffe1 	ble	d84 <pads_pull+0x48>
     dfc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     e00:	21e09014 	.word	0x21e09014
     e04:	21e091cc 	.word	0x21e091cc

00000e08 <pads_setpin>:
     e08:	e1b0c840 	asrs	ip, r0, #16
     e0c:	e1a00800 	lsl	r0, r0, #16
     e10:	03a03000 	moveq	r3, #0	; 0x0
     e14:	13a03001 	movne	r3, #1	; 0x1
     e18:	e1a00820 	lsr	r0, r0, #16
     e1c:	e15c0000 	cmp	ip, r0
     e20:	a3a03000 	movge	r3, #0	; 0x0
     e24:	b2033001 	andlt	r3, r3, #1	; 0x1
     e28:	e3530000 	cmp	r3, #0	; 0x0
     e2c:	112fff1e 	bxne	lr
     e30:	e1a03300 	lsl	r3, r0, #6
     e34:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     e38:	e35c0000 	cmp	ip, #0	; 0x0
     e3c:	01a0c000 	moveq	ip, r0
     e40:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     e44:	ea000002 	b	e54 <pads_setpin+0x4c>
     e48:	e2513000 	subs	r3, r1, #0	; 0x0
     e4c:	13a03001 	movne	r3, #1	; 0x1
     e50:	e4823040 	str	r3, [r2], #64
     e54:	e150000c 	cmp	r0, ip
     e58:	e2800001 	add	r0, r0, #1	; 0x1
     e5c:	dafffff9 	ble	e48 <pads_setpin+0x40>
     e60:	e12fff1e 	bx	lr

00000e64 <pads_getpin>:
     e64:	e59f3014 	ldr	r3, [pc, #20]	; e80 <pads_getpin+0x1c>
     e68:	e6f30070 	uxtah	r0, r3, r0
     e6c:	e1a00300 	lsl	r0, r0, #6
     e70:	e5900000 	ldr	r0, [r0]
     e74:	e2500000 	subs	r0, r0, #0	; 0x0
     e78:	13a00001 	movne	r0, #1	; 0x1
     e7c:	e12fff1e 	bx	lr
     e80:	0083c000 	.word	0x0083c000

00000e84 <get_boot_pin_value>:
     e84:	e3a00000 	mov	r0, #0	; 0x0
     e88:	e12fff1e 	bx	lr

00000e8c <init_alternate>:
     e8c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     e90:	e59f0010 	ldr	r0, [pc, #16]	; ea8 <init_alternate+0x1c>
     e94:	e24dd004 	sub	sp, sp, #4	; 0x4
     e98:	ebfffe1a 	bl	708 <module_enable>
     e9c:	e3a00000 	mov	r0, #0	; 0x0
     ea0:	e28dd004 	add	sp, sp, #4	; 0x4
     ea4:	e8bd8000 	pop	{pc}
     ea8:	21e13c00 	.word	0x21e13c00

00000eac <set_clk>:
     eac:	e92d4030 	push	{r4, r5, lr}
     eb0:	e1d010b2 	ldrh	r1, [r0, #2]
     eb4:	e24dd004 	sub	sp, sp, #4	; 0x4
     eb8:	e3510000 	cmp	r1, #0	; 0x0
     ebc:	e1a04000 	mov	r4, r0
     ec0:	0a000001 	beq	ecc <set_clk+0x20>
     ec4:	e3a00001 	mov	r0, #1	; 0x1
     ec8:	ebfffdc5 	bl	5e4 <set_pll>
     ecc:	e1d410b4 	ldrh	r1, [r4, #4]
     ed0:	e3510000 	cmp	r1, #0	; 0x0
     ed4:	0a000001 	beq	ee0 <set_clk+0x34>
     ed8:	e3a00002 	mov	r0, #2	; 0x2
     edc:	ebfffdc0 	bl	5e4 <set_pll>
     ee0:	e1d410b6 	ldrh	r1, [r4, #6]
     ee4:	e3510000 	cmp	r1, #0	; 0x0
     ee8:	0a000001 	beq	ef4 <set_clk+0x48>
     eec:	e3a00003 	mov	r0, #3	; 0x3
     ef0:	ebfffdbb 	bl	5e4 <set_pll>
     ef4:	e59f0084 	ldr	r0, [pc, #132]	; f80 <set_clk+0xd4>
     ef8:	e3a01001 	mov	r1, #1	; 0x1
     efc:	e3a0200b 	mov	r2, #11	; 0xb
     f00:	ebfffdf5 	bl	6dc <module_set_clock>
     f04:	e59f0078 	ldr	r0, [pc, #120]	; f84 <set_clk+0xd8>
     f08:	e3a01001 	mov	r1, #1	; 0x1
     f0c:	e3a02013 	mov	r2, #19	; 0x13
     f10:	ebfffdf1 	bl	6dc <module_set_clock>
     f14:	e59f006c 	ldr	r0, [pc, #108]	; f88 <set_clk+0xdc>
     f18:	e3a01002 	mov	r1, #2	; 0x2
     f1c:	e3a0200f 	mov	r2, #15	; 0xf
     f20:	ebfffded 	bl	6dc <module_set_clock>
     f24:	e59f0060 	ldr	r0, [pc, #96]	; f8c <set_clk+0xe0>
     f28:	e3a01002 	mov	r1, #2	; 0x2
     f2c:	e3a02019 	mov	r2, #25	; 0x19
     f30:	ebfffde9 	bl	6dc <module_set_clock>
     f34:	e59f4054 	ldr	r4, [pc, #84]	; f90 <set_clk+0xe4>
     f38:	e59f5054 	ldr	r5, [pc, #84]	; f94 <set_clk+0xe8>
     f3c:	e3a01003 	mov	r1, #3	; 0x3
     f40:	e1a00004 	mov	r0, r4
     f44:	e1a02001 	mov	r2, r1
     f48:	e2844010 	add	r4, r4, #16	; 0x10
     f4c:	ebfffde2 	bl	6dc <module_set_clock>
     f50:	e1540005 	cmp	r4, r5
     f54:	1afffff8 	bne	f3c <set_clk+0x90>
     f58:	e59f0038 	ldr	r0, [pc, #56]	; f98 <set_clk+0xec>
     f5c:	e3a01003 	mov	r1, #3	; 0x3
     f60:	e3a02013 	mov	r2, #19	; 0x13
     f64:	ebfffddc 	bl	6dc <module_set_clock>
     f68:	e59f002c 	ldr	r0, [pc, #44]	; f9c <set_clk+0xf0>
     f6c:	e3a01000 	mov	r1, #0	; 0x0
     f70:	e3a02005 	mov	r2, #5	; 0x5
     f74:	e28dd004 	add	sp, sp, #4	; 0x4
     f78:	e8bd4030 	pop	{r4, r5, lr}
     f7c:	eafffdd6 	b	6dc <module_set_clock>
     f80:	21e0a250 	.word	0x21e0a250
     f84:	21e0a2c0 	.word	0x21e0a2c0
     f88:	21e0a240 	.word	0x21e0a240
     f8c:	21e0a290 	.word	0x21e0a290
     f90:	21e0a130 	.word	0x21e0a130
     f94:	21e0a190 	.word	0x21e0a190
     f98:	21e0a260 	.word	0x21e0a260
     f9c:	21e0a360 	.word	0x21e0a360

00000fa0 <chip_test>:
     fa0:	e59f0044 	ldr	r0, [pc, #68]	; fec <end+0x4>
     fa4:	e5901000 	ldr	r1, [r0]
     fa8:	e59f2040 	ldr	r2, [pc, #64]	; ff0 <end+0x8>
     fac:	e1520001 	cmp	r2, r1
     fb0:	1a00000c 	bne	fe8 <end>
     fb4:	e5901004 	ldr	r1, [r0, #4]
     fb8:	e59f2034 	ldr	r2, [pc, #52]	; ff4 <end+0xc>
     fbc:	e1520001 	cmp	r2, r1
     fc0:	1a000008 	bne	fe8 <end>
     fc4:	e5901008 	ldr	r1, [r0, #8]
     fc8:	e59f2028 	ldr	r2, [pc, #40]	; ff8 <end+0x10>
     fcc:	e1520001 	cmp	r2, r1
     fd0:	1a000004 	bne	fe8 <end>
     fd4:	e590100c 	ldr	r1, [r0, #12]
     fd8:	e59f201c 	ldr	r2, [pc, #28]	; ffc <end+0x14>
     fdc:	e1520001 	cmp	r2, r1
     fe0:	1a000000 	bne	fe8 <end>
     fe4:	e59ff014 	ldr	pc, [pc, #20]	; 1000 <end+0x18>

00000fe8 <end>:
     fe8:	e1a0f00e 	.word	0xe1a0f00e
     fec:	3c000040 	.word	0x3c000040
     ff0:	6f666e69 	.word	0x6f666e69
     ff4:	612d6d74 	.word	0x612d6d74
     ff8:	2d6e696c 	.word	0x2d6e696c
     ffc:	796c6e6f 	.word	0x796c6e6f
    1000:	3c000200 	.word	0x3c000200

00001004 <strcpy>:
    1004:	e3a02000 	mov	r2, #0	; 0x0
    1008:	e7d13002 	ldrb	r3, [r1, r2]
    100c:	e3530000 	cmp	r3, #0	; 0x0
    1010:	e7c03002 	strb	r3, [r0, r2]
    1014:	e2822001 	add	r2, r2, #1	; 0x1
    1018:	1afffffa 	bne	1008 <strcpy+0x4>
    101c:	e12fff1e 	bx	lr

00001020 <strncpy>:
    1020:	e3a0c000 	mov	ip, #0	; 0x0
    1024:	e3520000 	cmp	r2, #0	; 0x0
    1028:	e2422001 	sub	r2, r2, #1	; 0x1
    102c:	012fff1e 	bxeq	lr
    1030:	e7d1300c 	ldrb	r3, [r1, ip]
    1034:	e3530000 	cmp	r3, #0	; 0x0
    1038:	e7c0300c 	strb	r3, [r0, ip]
    103c:	e28cc001 	add	ip, ip, #1	; 0x1
    1040:	1afffff7 	bne	1024 <strncpy+0x4>
    1044:	e12fff1e 	bx	lr

00001048 <strncat>:
    1048:	e92d4030 	push	{r4, r5, lr}
    104c:	e2525000 	subs	r5, r2, #0	; 0x0
    1050:	e1a04001 	mov	r4, r1
    1054:	11a0c000 	movne	ip, r0
    1058:	1a000001 	bne	1064 <strncat+0x1c>
    105c:	e8bd8030 	pop	{r4, r5, pc}
    1060:	e28cc001 	add	ip, ip, #1	; 0x1
    1064:	e5dc3000 	ldrb	r3, [ip]
    1068:	e3530000 	cmp	r3, #0	; 0x0
    106c:	1afffffb 	bne	1060 <strncat+0x18>
    1070:	ea000004 	b	1088 <strncat+0x40>
    1074:	e1550002 	cmp	r5, r2
    1078:	1a000004 	bne	1090 <strncat+0x48>
    107c:	e3a03000 	mov	r3, #0	; 0x0
    1080:	e5cc3000 	strb	r3, [ip]
    1084:	e8bd8030 	pop	{r4, r5, pc}
    1088:	e1a02003 	mov	r2, r3
    108c:	e1a0100c 	mov	r1, ip
    1090:	e7d43002 	ldrb	r3, [r4, r2]
    1094:	e28cc001 	add	ip, ip, #1	; 0x1
    1098:	e3530000 	cmp	r3, #0	; 0x0
    109c:	e7c13002 	strb	r3, [r1, r2]
    10a0:	e2822001 	add	r2, r2, #1	; 0x1
    10a4:	1afffff2 	bne	1074 <strncat+0x2c>
    10a8:	e8bd8030 	pop	{r4, r5, pc}

000010ac <strcmp>:
    10ac:	e92d4010 	push	{r4, lr}
    10b0:	e3a02000 	mov	r2, #0	; 0x0
    10b4:	e1a04000 	mov	r4, r0
    10b8:	e7d4c002 	ldrb	ip, [r4, r2]
    10bc:	e7d13002 	ldrb	r3, [r1, r2]
    10c0:	e2822001 	add	r2, r2, #1	; 0x1
    10c4:	e063300c 	rsb	r3, r3, ip
    10c8:	e6ef0073 	uxtb	r0, r3
    10cc:	e3500000 	cmp	r0, #0	; 0x0
    10d0:	1a000001 	bne	10dc <strcmp+0x30>
    10d4:	e35c0000 	cmp	ip, #0	; 0x0
    10d8:	1afffff6 	bne	10b8 <strcmp+0xc>
    10dc:	e6af0070 	sxtb	r0, r0
    10e0:	e8bd8010 	pop	{r4, pc}

000010e4 <strncmp>:
    10e4:	e92d4030 	push	{r4, r5, lr}
    10e8:	e3a0c000 	mov	ip, #0	; 0x0
    10ec:	e1a05000 	mov	r5, r0
    10f0:	e1a04001 	mov	r4, r1
    10f4:	ea000009 	b	1120 <strncmp+0x3c>
    10f8:	e7d5100c 	ldrb	r1, [r5, ip]
    10fc:	e7d4300c 	ldrb	r3, [r4, ip]
    1100:	e2422001 	sub	r2, r2, #1	; 0x1
    1104:	e0633001 	rsb	r3, r3, r1
    1108:	e6ef0073 	uxtb	r0, r3
    110c:	e3500000 	cmp	r0, #0	; 0x0
    1110:	e28cc001 	add	ip, ip, #1	; 0x1
    1114:	1a000004 	bne	112c <strncmp+0x48>
    1118:	e3510000 	cmp	r1, #0	; 0x0
    111c:	0a000002 	beq	112c <strncmp+0x48>
    1120:	e3520000 	cmp	r2, #0	; 0x0
    1124:	1afffff3 	bne	10f8 <strncmp+0x14>
    1128:	e1a00002 	mov	r0, r2
    112c:	e6af0070 	sxtb	r0, r0
    1130:	e8bd8030 	pop	{r4, r5, pc}

00001134 <strchr>:
    1134:	e1a03000 	mov	r3, r0
    1138:	e6ef1071 	uxtb	r1, r1
    113c:	ea000002 	b	114c <strchr+0x18>
    1140:	e3500000 	cmp	r0, #0	; 0x0
    1144:	012fff1e 	bxeq	lr
    1148:	e2833001 	add	r3, r3, #1	; 0x1
    114c:	e5d30000 	ldrb	r0, [r3]
    1150:	e1500001 	cmp	r0, r1
    1154:	1afffff9 	bne	1140 <strchr+0xc>
    1158:	e1a00003 	mov	r0, r3
    115c:	e12fff1e 	bx	lr

00001160 <strlen>:
    1160:	e1a02000 	mov	r2, r0
    1164:	ea000000 	b	116c <strlen+0xc>
    1168:	e2800001 	add	r0, r0, #1	; 0x1
    116c:	e5d03000 	ldrb	r3, [r0]
    1170:	e3530000 	cmp	r3, #0	; 0x0
    1174:	1afffffb 	bne	1168 <strlen+0x8>
    1178:	e0620000 	rsb	r0, r2, r0
    117c:	e12fff1e 	bx	lr

00001180 <strnlen>:
    1180:	e3a02000 	mov	r2, #0	; 0x0
    1184:	e1510002 	cmp	r1, r2
    1188:	e080c002 	add	ip, r0, r2
    118c:	0a000003 	beq	11a0 <strnlen+0x20>
    1190:	e7d03002 	ldrb	r3, [r0, r2]
    1194:	e2822001 	add	r2, r2, #1	; 0x1
    1198:	e3530000 	cmp	r3, #0	; 0x0
    119c:	1afffff8 	bne	1184 <strnlen+0x4>
    11a0:	e060000c 	rsb	r0, r0, ip
    11a4:	e12fff1e 	bx	lr

000011a8 <memset>:
    11a8:	e3a03000 	mov	r3, #0	; 0x0
    11ac:	ea000001 	b	11b8 <memset+0x10>
    11b0:	e7c01003 	strb	r1, [r0, r3]
    11b4:	e2833001 	add	r3, r3, #1	; 0x1
    11b8:	e3520000 	cmp	r2, #0	; 0x0
    11bc:	e2422001 	sub	r2, r2, #1	; 0x1
    11c0:	1afffffa 	bne	11b0 <memset+0x8>
    11c4:	e12fff1e 	bx	lr

000011c8 <memcpy>:
    11c8:	e3a0c000 	mov	ip, #0	; 0x0
    11cc:	ea000002 	b	11dc <memcpy+0x14>
    11d0:	e7d1300c 	ldrb	r3, [r1, ip]
    11d4:	e7c0300c 	strb	r3, [r0, ip]
    11d8:	e28cc001 	add	ip, ip, #1	; 0x1
    11dc:	e3520000 	cmp	r2, #0	; 0x0
    11e0:	e2422001 	sub	r2, r2, #1	; 0x1
    11e4:	1afffff9 	bne	11d0 <memcpy+0x8>
    11e8:	e12fff1e 	bx	lr

000011ec <memmove>:
    11ec:	e1500001 	cmp	r0, r1
    11f0:	e92d4010 	push	{r4, lr}
    11f4:	08bd8010 	popeq	{r4, pc}
    11f8:	93a0c000 	movls	ip, #0	; 0x0
    11fc:	9a000003 	bls	1210 <memmove+0x24>
    1200:	ea000006 	b	1220 <memmove+0x34>
    1204:	e7d1300c 	ldrb	r3, [r1, ip]
    1208:	e7c0300c 	strb	r3, [r0, ip]
    120c:	e28cc001 	add	ip, ip, #1	; 0x1
    1210:	e3520000 	cmp	r2, #0	; 0x0
    1214:	e2422001 	sub	r2, r2, #1	; 0x1
    1218:	1afffff9 	bne	1204 <memmove+0x18>
    121c:	e8bd8010 	pop	{r4, pc}
    1220:	e0814002 	add	r4, r1, r2
    1224:	e3a0c000 	mov	ip, #0	; 0x0
    1228:	e0801002 	add	r1, r0, r2
    122c:	ea000001 	b	1238 <memmove+0x4c>
    1230:	e7d4300c 	ldrb	r3, [r4, ip]
    1234:	e7c1300c 	strb	r3, [r1, ip]
    1238:	e3520000 	cmp	r2, #0	; 0x0
    123c:	e24cc001 	sub	ip, ip, #1	; 0x1
    1240:	e2422001 	sub	r2, r2, #1	; 0x1
    1244:	1afffff9 	bne	1230 <memmove+0x44>
    1248:	e8bd8010 	pop	{r4, pc}

0000124c <memcmp>:
    124c:	e92d4030 	push	{r4, r5, lr}
    1250:	e3a0c000 	mov	ip, #0	; 0x0
    1254:	e1a04001 	mov	r4, r1
    1258:	e1a05000 	mov	r5, r0
    125c:	e1a01002 	mov	r1, r2
    1260:	ea000005 	b	127c <memcmp+0x30>
    1264:	e7d5200c 	ldrb	r2, [r5, ip]
    1268:	e7d4300c 	ldrb	r3, [r4, ip]
    126c:	e28cc001 	add	ip, ip, #1	; 0x1
    1270:	e0520003 	subs	r0, r2, r3
    1274:	18bd8030 	popne	{r4, r5, pc}
    1278:	e2411001 	sub	r1, r1, #1	; 0x1
    127c:	e3510000 	cmp	r1, #0	; 0x0
    1280:	1afffff7 	bne	1264 <memcmp+0x18>
    1284:	e1a00001 	mov	r0, r1
    1288:	e8bd8030 	pop	{r4, r5, pc}

0000128c <so_m_isused>:
    128c:	e5903004 	ldr	r3, [r0, #4]
    1290:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1294:	03a00000 	moveq	r0, #0	; 0x0
    1298:	13a00001 	movne	r0, #1	; 0x1
    129c:	e12fff1e 	bx	lr

000012a0 <so_m_set_used>:
    12a0:	e5903004 	ldr	r3, [r0, #4]
    12a4:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    12a8:	e5803004 	str	r3, [r0, #4]
    12ac:	e12fff1e 	bx	lr

000012b0 <so_m_unset_used>:
    12b0:	e5903004 	ldr	r3, [r0, #4]
    12b4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    12b8:	e5803004 	str	r3, [r0, #4]
    12bc:	e12fff1e 	bx	lr

000012c0 <so_m_get_size>:
    12c0:	e5900004 	ldr	r0, [r0, #4]
    12c4:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    12c8:	e12fff1e 	bx	lr

000012cc <so_m_set_size>:
    12cc:	e5903004 	ldr	r3, [r0, #4]
    12d0:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    12d4:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    12d8:	e1811003 	orr	r1, r1, r3
    12dc:	e5801004 	str	r1, [r0, #4]
    12e0:	e12fff1e 	bx	lr

000012e4 <so_m_alloc>:
    12e4:	e59f3048 	ldr	r3, [pc, #72]	; 1334 <so_m_alloc+0x50>
    12e8:	e3a02000 	mov	r2, #0	; 0x0
    12ec:	e593c000 	ldr	ip, [r3]
    12f0:	e5931008 	ldr	r1, [r3, #8]
    12f4:	ea000007 	b	1318 <so_m_alloc+0x34>
    12f8:	e5903004 	ldr	r3, [r0, #4]
    12fc:	e3530000 	cmp	r3, #0	; 0x0
    1300:	1a000004 	bne	1318 <so_m_alloc+0x34>
    1304:	e59f2028 	ldr	r2, [pc, #40]	; 1334 <so_m_alloc+0x50>
    1308:	e5923004 	ldr	r3, [r2, #4]
    130c:	e2433001 	sub	r3, r3, #1	; 0x1
    1310:	e5823004 	str	r3, [r2, #4]
    1314:	e12fff1e 	bx	lr
    1318:	e152000c 	cmp	r2, ip
    131c:	e1a00001 	mov	r0, r1
    1320:	e2822001 	add	r2, r2, #1	; 0x1
    1324:	e2811010 	add	r1, r1, #16	; 0x10
    1328:	3afffff2 	bcc	12f8 <so_m_alloc+0x14>
    132c:	e3a00000 	mov	r0, #0	; 0x0
    1330:	e12fff1e 	bx	lr
    1334:	3c00d6cc 	.word	0x3c00d6cc

00001338 <so_m_remove>:
    1338:	e3a03000 	mov	r3, #0	; 0x0
    133c:	e59f2014 	ldr	r2, [pc, #20]	; 1358 <so_m_remove+0x20>
    1340:	e5803000 	str	r3, [r0]
    1344:	e5803004 	str	r3, [r0, #4]
    1348:	e5923004 	ldr	r3, [r2, #4]
    134c:	e2833001 	add	r3, r3, #1	; 0x1
    1350:	e5823004 	str	r3, [r2, #4]
    1354:	e12fff1e 	bx	lr
    1358:	3c00d6cc 	.word	0x3c00d6cc

0000135c <malloc>:
    135c:	e3500000 	cmp	r0, #0	; 0x0
    1360:	e92d4070 	push	{r4, r5, r6, lr}
    1364:	0a000047 	beq	1488 <malloc+0x12c>
    1368:	e59f1120 	ldr	r1, [pc, #288]	; 1490 <malloc+0x134>
    136c:	e280301f 	add	r3, r0, #31	; 0x1f
    1370:	e5b12014 	ldr	r2, [r1, #20]!
    1374:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    1378:	e2425008 	sub	r5, r2, #8	; 0x8
    137c:	ea000009 	b	13a8 <malloc+0x4c>
    1380:	e5953004 	ldr	r3, [r5, #4]
    1384:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    1388:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    138c:	1a000003 	bne	13a0 <malloc+0x44>
    1390:	e1520004 	cmp	r2, r4
    1394:	3a000001 	bcc	13a0 <malloc+0x44>
    1398:	9a000030 	bls	1460 <malloc+0x104>
    139c:	ea000005 	b	13b8 <malloc+0x5c>
    13a0:	e5953008 	ldr	r3, [r5, #8]
    13a4:	e2435008 	sub	r5, r3, #8	; 0x8
    13a8:	e2853008 	add	r3, r5, #8	; 0x8
    13ac:	e1530001 	cmp	r3, r1
    13b0:	1afffff2 	bne	1380 <malloc+0x24>
    13b4:	ea000033 	b	1488 <malloc+0x12c>
    13b8:	e59f30d0 	ldr	r3, [pc, #208]	; 1490 <malloc+0x134>
    13bc:	e1a01000 	mov	r1, r0
    13c0:	e5932008 	ldr	r2, [r3, #8]
    13c4:	e5930000 	ldr	r0, [r3]
    13c8:	ea00000a 	b	13f8 <malloc+0x9c>
    13cc:	e5923004 	ldr	r3, [r2, #4]
    13d0:	e2822010 	add	r2, r2, #16	; 0x10
    13d4:	e3530000 	cmp	r3, #0	; 0x0
    13d8:	1a000006 	bne	13f8 <malloc+0x9c>
    13dc:	e59f20ac 	ldr	r2, [pc, #172]	; 1490 <malloc+0x134>
    13e0:	e3560000 	cmp	r6, #0	; 0x0
    13e4:	e5923004 	ldr	r3, [r2, #4]
    13e8:	e2433001 	sub	r3, r3, #1	; 0x1
    13ec:	e5823004 	str	r3, [r2, #4]
    13f0:	1a000005 	bne	140c <malloc+0xb0>
    13f4:	ea000023 	b	1488 <malloc+0x12c>
    13f8:	e1510000 	cmp	r1, r0
    13fc:	e1a06002 	mov	r6, r2
    1400:	e2811001 	add	r1, r1, #1	; 0x1
    1404:	3afffff0 	bcc	13cc <malloc+0x70>
    1408:	ea00001e 	b	1488 <malloc+0x12c>
    140c:	e5953004 	ldr	r3, [r5, #4]
    1410:	e5962004 	ldr	r2, [r6, #4]
    1414:	e0643003 	rsb	r3, r4, r3
    1418:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    141c:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1420:	e1833002 	orr	r3, r3, r2
    1424:	e5863004 	str	r3, [r6, #4]
    1428:	e5951000 	ldr	r1, [r5]
    142c:	e5950004 	ldr	r0, [r5, #4]
    1430:	e1a0c005 	mov	ip, r5
    1434:	e1a03006 	mov	r3, r6
    1438:	e0841001 	add	r1, r4, r1
    143c:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    1440:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    1444:	e5bc4008 	ldr	r4, [ip, #8]!
    1448:	e4831008 	str	r1, [r3], #8
    144c:	e1822000 	orr	r2, r2, r0
    1450:	e5864008 	str	r4, [r6, #8]
    1454:	e5843004 	str	r3, [r4, #4]
    1458:	e985000c 	stmib	r5, {r2, r3}
    145c:	e586c00c 	str	ip, [r6, #12]
    1460:	e5953004 	ldr	r3, [r5, #4]
    1464:	e59f1024 	ldr	r1, [pc, #36]	; 1490 <malloc+0x134>
    1468:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    146c:	e5853004 	str	r3, [r5, #4]
    1470:	e5912010 	ldr	r2, [r1, #16]
    1474:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1478:	e0833002 	add	r3, r3, r2
    147c:	e5813010 	str	r3, [r1, #16]
    1480:	e5950000 	ldr	r0, [r5]
    1484:	e8bd8070 	pop	{r4, r5, r6, pc}
    1488:	e3a00000 	mov	r0, #0	; 0x0
    148c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1490:	3c00d6cc 	.word	0x3c00d6cc

00001494 <__sOMERGe>:
    1494:	e5902004 	ldr	r2, [r0, #4]
    1498:	e92d4030 	push	{r4, r5, lr}
    149c:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    14a0:	e1a05000 	mov	r5, r0
    14a4:	e1a0c001 	mov	ip, r1
    14a8:	18bd8030 	popne	{r4, r5, pc}
    14ac:	e5913004 	ldr	r3, [r1, #4]
    14b0:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    14b4:	18bd8030 	popne	{r4, r5, pc}
    14b8:	e0823003 	add	r3, r2, r3
    14bc:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    14c0:	e59f0030 	ldr	r0, [pc, #48]	; 14f8 <__sOMERGe+0x64>
    14c4:	e5853004 	str	r3, [r5, #4]
    14c8:	e5814000 	str	r4, [r1]
    14cc:	e5814004 	str	r4, [r1, #4]
    14d0:	e5903004 	ldr	r3, [r0, #4]
    14d4:	e591100c 	ldr	r1, [r1, #12]
    14d8:	e59c2008 	ldr	r2, [ip, #8]
    14dc:	e2833001 	add	r3, r3, #1	; 0x1
    14e0:	e5812000 	str	r2, [r1]
    14e4:	e5821004 	str	r1, [r2, #4]
    14e8:	e5803004 	str	r3, [r0, #4]
    14ec:	e58c4008 	str	r4, [ip, #8]
    14f0:	e58c400c 	str	r4, [ip, #12]
    14f4:	e8bd8030 	pop	{r4, r5, pc}
    14f8:	3c00d6cc 	.word	0x3c00d6cc

000014fc <free>:
    14fc:	e3500000 	cmp	r0, #0	; 0x0
    1500:	159f2090 	ldrne	r2, [pc, #144]	; 1598 <free+0x9c>
    1504:	e92d4010 	push	{r4, lr}
    1508:	15b23014 	ldrne	r3, [r2, #20]!
    150c:	1a000004 	bne	1524 <free+0x28>
    1510:	e8bd8010 	pop	{r4, pc}
    1514:	e5943000 	ldr	r3, [r4]
    1518:	e1500003 	cmp	r0, r3
    151c:	0a000005 	beq	1538 <free+0x3c>
    1520:	e5943008 	ldr	r3, [r4, #8]
    1524:	e2434008 	sub	r4, r3, #8	; 0x8
    1528:	e2843008 	add	r3, r4, #8	; 0x8
    152c:	e1530002 	cmp	r3, r2
    1530:	1afffff7 	bne	1514 <free+0x18>
    1534:	e8bd8010 	pop	{r4, pc}
    1538:	e5943004 	ldr	r3, [r4, #4]
    153c:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1540:	08bd8010 	popeq	{r4, pc}
    1544:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1548:	e59f1048 	ldr	r1, [pc, #72]	; 1598 <free+0x9c>
    154c:	e5843004 	str	r3, [r4, #4]
    1550:	e5912010 	ldr	r2, [r1, #16]
    1554:	e5940008 	ldr	r0, [r4, #8]
    1558:	e0632002 	rsb	r2, r3, r2
    155c:	e59f3038 	ldr	r3, [pc, #56]	; 159c <free+0xa0>
    1560:	e5812010 	str	r2, [r1, #16]
    1564:	e1500003 	cmp	r0, r3
    1568:	0a000002 	beq	1578 <free+0x7c>
    156c:	e2401008 	sub	r1, r0, #8	; 0x8
    1570:	e1a00004 	mov	r0, r4
    1574:	ebffffc6 	bl	1494 <__sOMERGe>
    1578:	e594000c 	ldr	r0, [r4, #12]
    157c:	e59f3018 	ldr	r3, [pc, #24]	; 159c <free+0xa0>
    1580:	e1500003 	cmp	r0, r3
    1584:	08bd8010 	popeq	{r4, pc}
    1588:	e2400008 	sub	r0, r0, #8	; 0x8
    158c:	e1a01004 	mov	r1, r4
    1590:	e8bd4010 	pop	{r4, lr}
    1594:	eaffffbe 	b	1494 <__sOMERGe>
    1598:	3c00d6cc 	.word	0x3c00d6cc
    159c:	3c00d6e0 	.word	0x3c00d6e0

000015a0 <__sOINFo>:
    15a0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    15a4:	e59f50a8 	ldr	r5, [pc, #168]	; 1654 <__sOINFo+0xb4>
    15a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    15ac:	e59f60a4 	ldr	r6, [pc, #164]	; 1658 <__sOINFo+0xb8>
    15b0:	e8950006 	ldm	r5, {r1, r2}
    15b4:	e59f00a0 	ldr	r0, [pc, #160]	; 165c <__sOINFo+0xbc>
    15b8:	e1a0e00f 	mov	lr, pc
    15bc:	e596f000 	ldr	pc, [r6]
    15c0:	e2853008 	add	r3, r5, #8	; 0x8
    15c4:	e8930098 	ldm	r3, {r3, r4, r7}
    15c8:	e2444b01 	sub	r4, r4, #1024	; 0x400
    15cc:	e0634004 	rsb	r4, r3, r4
    15d0:	e3a00064 	mov	r0, #100	; 0x64
    15d4:	e1a01004 	mov	r1, r4
    15d8:	e0000790 	mul	r0, r0, r7
    15dc:	eb003305 	bl	e1f8 <__aeabi_uidiv>
    15e0:	e1a01004 	mov	r1, r4
    15e4:	e1a03000 	mov	r3, r0
    15e8:	e1a02007 	mov	r2, r7
    15ec:	e59f006c 	ldr	r0, [pc, #108]	; 1660 <__sOINFo+0xc0>
    15f0:	e1a0e00f 	mov	lr, pc
    15f4:	e596f000 	ldr	pc, [r6]
    15f8:	e59f0064 	ldr	r0, [pc, #100]	; 1664 <__sOINFo+0xc4>
    15fc:	e1a0e00f 	mov	lr, pc
    1600:	e596f000 	ldr	pc, [r6]
    1604:	e59f005c 	ldr	r0, [pc, #92]	; 1668 <__sOINFo+0xc8>
    1608:	e1a0e00f 	mov	lr, pc
    160c:	e596f000 	ldr	pc, [r6]
    1610:	e5b53014 	ldr	r3, [r5, #20]!
    1614:	ea000007 	b	1638 <__sOINFo+0x98>
    1618:	e8940006 	ldm	r4, {r1, r2}
    161c:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1620:	03a03000 	moveq	r3, #0	; 0x0
    1624:	13a03001 	movne	r3, #1	; 0x1
    1628:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    162c:	e1a0e00f 	mov	lr, pc
    1630:	e596f000 	ldr	pc, [r6]
    1634:	e5943008 	ldr	r3, [r4, #8]
    1638:	e2434008 	sub	r4, r3, #8	; 0x8
    163c:	e2843008 	add	r3, r4, #8	; 0x8
    1640:	e1530005 	cmp	r3, r5
    1644:	e59f0020 	ldr	r0, [pc, #32]	; 166c <__sOINFo+0xcc>
    1648:	1afffff2 	bne	1618 <__sOINFo+0x78>
    164c:	e28dd004 	add	sp, sp, #4	; 0x4
    1650:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1654:	3c00d6cc 	.word	0x3c00d6cc
    1658:	3c00d68c 	.word	0x3c00d68c
    165c:	0000fc00 	.word	0x0000fc00
    1660:	0000fc1a 	.word	0x0000fc1a
    1664:	0000fc3f 	.word	0x0000fc3f
    1668:	0000fb0e 	.word	0x0000fb0e
    166c:	0000fc50 	.word	0x0000fc50

00001670 <init_malloc>:
    1670:	e59f20e4 	ldr	r2, [pc, #228]	; 175c <init_malloc+0xec>
    1674:	e59f30e4 	ldr	r3, [pc, #228]	; 1760 <init_malloc+0xf0>
    1678:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    167c:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1680:	e0602003 	rsb	r2, r0, r3
    1684:	e92d4010 	push	{r4, lr}
    1688:	e59f40d4 	ldr	r4, [pc, #212]	; 1764 <init_malloc+0xf4>
    168c:	e3520b01 	cmp	r2, #1024	; 0x400
    1690:	e5840008 	str	r0, [r4, #8]
    1694:	e584300c 	str	r3, [r4, #12]
    1698:	93e00000 	mvnls	r0, #0	; 0x0
    169c:	98bd8010 	popls	{r4, pc}
    16a0:	e284c014 	add	ip, r4, #20	; 0x14
    16a4:	e3a03040 	mov	r3, #64	; 0x40
    16a8:	e3a01000 	mov	r1, #0	; 0x0
    16ac:	e3a02b01 	mov	r2, #1024	; 0x400
    16b0:	e584c018 	str	ip, [r4, #24]
    16b4:	e584c014 	str	ip, [r4, #20]
    16b8:	e5843004 	str	r3, [r4, #4]
    16bc:	e5843000 	str	r3, [r4]
    16c0:	ebfffeb8 	bl	11a8 <memset>
    16c4:	e5940008 	ldr	r0, [r4, #8]
    16c8:	e594c000 	ldr	ip, [r4]
    16cc:	e1a01000 	mov	r1, r0
    16d0:	e3a02000 	mov	r2, #0	; 0x0
    16d4:	ea000006 	b	16f4 <init_malloc+0x84>
    16d8:	e59e3004 	ldr	r3, [lr, #4]
    16dc:	e3530000 	cmp	r3, #0	; 0x0
    16e0:	059f207c 	ldreq	r2, [pc, #124]	; 1764 <init_malloc+0xf4>
    16e4:	05923004 	ldreq	r3, [r2, #4]
    16e8:	02433001 	subeq	r3, r3, #1	; 0x1
    16ec:	05823004 	streq	r3, [r2, #4]
    16f0:	0a000005 	beq	170c <init_malloc+0x9c>
    16f4:	e152000c 	cmp	r2, ip
    16f8:	e1a0e001 	mov	lr, r1
    16fc:	e2822001 	add	r2, r2, #1	; 0x1
    1700:	e2811010 	add	r1, r1, #16	; 0x10
    1704:	3afffff3 	bcc	16d8 <init_malloc+0x68>
    1708:	e3a0e000 	mov	lr, #0	; 0x0
    170c:	e59fc050 	ldr	ip, [pc, #80]	; 1764 <init_malloc+0xf4>
    1710:	e59e2004 	ldr	r2, [lr, #4]
    1714:	e59c300c 	ldr	r3, [ip, #12]
    1718:	e1a0100e 	mov	r1, lr
    171c:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1720:	e0603003 	rsb	r3, r0, r3
    1724:	e59c4018 	ldr	r4, [ip, #24]
    1728:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    172c:	e2800b01 	add	r0, r0, #1024	; 0x400
    1730:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1734:	e4810008 	str	r0, [r1], #8
    1738:	e1833002 	orr	r3, r3, r2
    173c:	e28c2014 	add	r2, ip, #20	; 0x14
    1740:	e58e2008 	str	r2, [lr, #8]
    1744:	e58c1018 	str	r1, [ip, #24]
    1748:	e5841000 	str	r1, [r4]
    174c:	e58e3004 	str	r3, [lr, #4]
    1750:	e58e400c 	str	r4, [lr, #12]
    1754:	e3a00000 	mov	r0, #0	; 0x0
    1758:	e8bd8010 	pop	{r4, pc}
    175c:	3c00e01f 	.word	0x3c00e01f
    1760:	3c020000 	.word	0x3c020000
    1764:	3c00d6cc 	.word	0x3c00d6cc

00001768 <isi_check_header>:
    1768:	e92d4070 	push	{r4, r5, r6, lr}
    176c:	e5902000 	ldr	r2, [r0]
    1770:	e59f3088 	ldr	r3, [pc, #136]	; 1800 <isi_check_header+0x98>
    1774:	e1a05000 	mov	r5, r0
    1778:	e1520003 	cmp	r2, r3
    177c:	e24dd008 	sub	sp, sp, #8	; 0x8
    1780:	13e0001e 	mvnne	r0, #30	; 0x1e
    1784:	1a00001b 	bne	17f8 <isi_check_header+0x90>
    1788:	e3a06000 	mov	r6, #0	; 0x0
    178c:	e595401c 	ldr	r4, [r5, #28]
    1790:	e3a01c02 	mov	r1, #512	; 0x200
    1794:	e585601c 	str	r6, [r5, #28]
    1798:	e1a00006 	mov	r0, r6
    179c:	eb000f62 	bl	552c <hash_init>
    17a0:	e3a01c02 	mov	r1, #512	; 0x200
    17a4:	e1a00005 	mov	r0, r5
    17a8:	eb000f3a 	bl	5498 <hash_data>
    17ac:	e28d0004 	add	r0, sp, #4	; 0x4
    17b0:	eb000f1c 	bl	5428 <hash_value>
    17b4:	e585401c 	str	r4, [r5, #28]
    17b8:	e59d3004 	ldr	r3, [sp, #4]
    17bc:	e1540003 	cmp	r4, r3
    17c0:	13e0001f 	mvnne	r0, #31	; 0x1f
    17c4:	1a00000b 	bne	17f8 <isi_check_header+0x90>
    17c8:	e5951004 	ldr	r1, [r5, #4]
    17cc:	e59f4030 	ldr	r4, [pc, #48]	; 1804 <isi_check_header+0x9c>
    17d0:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    17d4:	e59f002c 	ldr	r0, [pc, #44]	; 1808 <isi_check_header+0xa0>
    17d8:	e1a0e00f 	mov	lr, pc
    17dc:	e594f000 	ldr	pc, [r4]
    17e0:	e5d51007 	ldrb	r1, [r5, #7]
    17e4:	e59f0020 	ldr	r0, [pc, #32]	; 180c <isi_check_header+0xa4>
    17e8:	e201100f 	and	r1, r1, #15	; 0xf
    17ec:	e1a0e00f 	mov	lr, pc
    17f0:	e594f000 	ldr	pc, [r4]
    17f4:	e1a00006 	mov	r0, r6
    17f8:	e28dd008 	add	sp, sp, #8	; 0x8
    17fc:	e8bd8070 	pop	{r4, r5, r6, pc}
    1800:	6973692b 	.word	0x6973692b
    1804:	3c00d68c 	.word	0x3c00d68c
    1808:	0000fc78 	.word	0x0000fc78
    180c:	0000fc86 	.word	0x0000fc86

00001810 <isi_check_data_separate>:
    1810:	e92d4070 	push	{r4, r5, r6, lr}
    1814:	e1a05000 	mov	r5, r0
    1818:	e5d00007 	ldrb	r0, [r0, #7]
    181c:	e24dd028 	sub	sp, sp, #40	; 0x28
    1820:	e1a04001 	mov	r4, r1
    1824:	e200000f 	and	r0, r0, #15	; 0xf
    1828:	e595100c 	ldr	r1, [r5, #12]
    182c:	eb000f3e 	bl	552c <hash_init>
    1830:	e28d6008 	add	r6, sp, #8	; 0x8
    1834:	e595100c 	ldr	r1, [r5, #12]
    1838:	e1a00004 	mov	r0, r4
    183c:	eb000f15 	bl	5498 <hash_data>
    1840:	e1a00006 	mov	r0, r6
    1844:	eb000ef7 	bl	5428 <hash_value>
    1848:	eb000eef 	bl	540c <hash_len>
    184c:	e2854020 	add	r4, r5, #32	; 0x20
    1850:	e1a02000 	mov	r2, r0
    1854:	e1a01004 	mov	r1, r4
    1858:	e1a00006 	mov	r0, r6
    185c:	ebfffe7a 	bl	124c <memcmp>
    1860:	e3500000 	cmp	r0, #0	; 0x0
    1864:	0a000014 	beq	18bc <isi_check_data_separate+0xac>
    1868:	e595100c 	ldr	r1, [r5, #12]
    186c:	e59f3050 	ldr	r3, [pc, #80]	; 18c4 <isi_check_data_separate+0xb4>
    1870:	e59f0050 	ldr	r0, [pc, #80]	; 18c8 <isi_check_data_separate+0xb8>
    1874:	e1a0e00f 	mov	lr, pc
    1878:	e593f000 	ldr	pc, [r3]
    187c:	eb000ee2 	bl	540c <hash_len>
    1880:	e1a01004 	mov	r1, r4
    1884:	e1a03000 	mov	r3, r0
    1888:	e3a02001 	mov	r2, #1	; 0x1
    188c:	e1a00004 	mov	r0, r4
    1890:	e3a04020 	mov	r4, #32	; 0x20
    1894:	e58d4000 	str	r4, [sp]
    1898:	eb0025ba 	bl	af88 <print_buffer>
    189c:	eb000eda 	bl	540c <hash_len>
    18a0:	e1a01006 	mov	r1, r6
    18a4:	e1a03000 	mov	r3, r0
    18a8:	e3a02001 	mov	r2, #1	; 0x1
    18ac:	e1a00006 	mov	r0, r6
    18b0:	e58d4000 	str	r4, [sp]
    18b4:	eb0025b3 	bl	af88 <print_buffer>
    18b8:	e3e00020 	mvn	r0, #32	; 0x20
    18bc:	e28dd028 	add	sp, sp, #40	; 0x28
    18c0:	e8bd8070 	pop	{r4, r5, r6, pc}
    18c4:	3c00d68c 	.word	0x3c00d68c
    18c8:	0000fc99 	.word	0x0000fc99

000018cc <isi_check_data>:
    18cc:	e2801c02 	add	r1, r0, #512	; 0x200
    18d0:	eaffffce 	b	1810 <isi_check_data_separate>

000018d4 <isi_check>:
    18d4:	e92d4010 	push	{r4, lr}
    18d8:	e1a04000 	mov	r4, r0
    18dc:	ebffffa1 	bl	1768 <isi_check_header>
    18e0:	e3500000 	cmp	r0, #0	; 0x0
    18e4:	18bd8010 	popne	{r4, pc}
    18e8:	e1a00004 	mov	r0, r4
    18ec:	e8bd4010 	pop	{r4, lr}
    18f0:	eafffff5 	b	18cc <isi_check_data>

000018f4 <load_serial_ymodem>:
    18f4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    18f8:	e24dde41 	sub	sp, sp, #1040	; 0x410
    18fc:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1900:	e284400c 	add	r4, r4, #12	; 0xc
    1904:	e3a03002 	mov	r3, #2	; 0x2
    1908:	e1a07000 	mov	r7, r0
    190c:	e1a01004 	mov	r1, r4
    1910:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1914:	e58d3404 	str	r3, [sp, #1028]
    1918:	eb0031bc 	bl	e010 <xyzModem_stream_open>
    191c:	e3500000 	cmp	r0, #0	; 0x0
    1920:	1a000012 	bne	1970 <load_serial_ymodem+0x7c>
    1924:	e1a08004 	mov	r8, r4
    1928:	e1a05000 	mov	r5, r0
    192c:	e1a04000 	mov	r4, r0
    1930:	e1a0600d 	mov	r6, sp
    1934:	ea000001 	b	1940 <load_serial_ymodem+0x4c>
    1938:	e085500c 	add	r5, r5, ip
    193c:	ebfffe21 	bl	11c8 <memcpy>
    1940:	e3a01b01 	mov	r1, #1024	; 0x400
    1944:	e1a02008 	mov	r2, r8
    1948:	e1a0000d 	mov	r0, sp
    194c:	eb00311b 	bl	ddc0 <xyzModem_stream_read>
    1950:	e0843007 	add	r3, r4, r7
    1954:	e250c000 	subs	ip, r0, #0	; 0x0
    1958:	e1a0100d 	mov	r1, sp
    195c:	e1a0200c 	mov	r2, ip
    1960:	e1a00003 	mov	r0, r3
    1964:	e084400c 	add	r4, r4, ip
    1968:	cafffff2 	bgt	1938 <load_serial_ymodem+0x44>
    196c:	ea000007 	b	1990 <load_serial_ymodem+0x9c>
    1970:	e59f3054 	ldr	r3, [pc, #84]	; 19cc <load_serial_ymodem+0xd8>
    1974:	e59d040c 	ldr	r0, [sp, #1036]
    1978:	e5934000 	ldr	r4, [r3]
    197c:	eb002d96 	bl	cfdc <xyzModem_error>
    1980:	e1a01000 	mov	r1, r0
    1984:	e59f0044 	ldr	r0, [pc, #68]	; 19d0 <load_serial_ymodem+0xdc>
    1988:	e12fff34 	blx	r4
    198c:	e3a05000 	mov	r5, #0	; 0x0
    1990:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1994:	e280000c 	add	r0, r0, #12	; 0xc
    1998:	eb002d77 	bl	cf7c <xyzModem_stream_close>
    199c:	e3a00000 	mov	r0, #0	; 0x0
    19a0:	e59f102c 	ldr	r1, [pc, #44]	; 19d4 <load_serial_ymodem+0xe0>
    19a4:	eb003025 	bl	da40 <xyzModem_stream_terminate>
    19a8:	e1a01005 	mov	r1, r5
    19ac:	e59f0024 	ldr	r0, [pc, #36]	; 19d8 <load_serial_ymodem+0xe4>
    19b0:	e1a02005 	mov	r2, r5
    19b4:	e59f3010 	ldr	r3, [pc, #16]	; 19cc <load_serial_ymodem+0xd8>
    19b8:	e1a0e00f 	mov	lr, pc
    19bc:	e593f000 	ldr	pc, [r3]
    19c0:	e1a00007 	mov	r0, r7
    19c4:	e28dde41 	add	sp, sp, #1040	; 0x410
    19c8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    19cc:	3c00d68c 	.word	0x3c00d68c
    19d0:	0000fd95 	.word	0x0000fd95
    19d4:	000019dc 	.word	0x000019dc
    19d8:	0000fcbd 	.word	0x0000fcbd

000019dc <getcxmodem>:
    19dc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    19e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    19e4:	eb002a16 	bl	c244 <tstc>
    19e8:	e3500000 	cmp	r0, #0	; 0x0
    19ec:	1a000002 	bne	19fc <getcxmodem+0x20>
    19f0:	e3e00000 	mvn	r0, #0	; 0x0
    19f4:	e28dd004 	add	sp, sp, #4	; 0x4
    19f8:	e8bd8000 	pop	{pc}
    19fc:	e28dd004 	add	sp, sp, #4	; 0x4
    1a00:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1a04:	ea002a0f 	b	c248 <getc>

00001a08 <do_load_serial_bin>:
    1a08:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1a0c:	e3520001 	cmp	r2, #1	; 0x1
    1a10:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1a14:	e1a06002 	mov	r6, r2
    1a18:	e1a07000 	mov	r7, r0
    1a1c:	e1a05003 	mov	r5, r3
    1a20:	ca000006 	bgt	1a40 <do_load_serial_bin+0x38>
    1a24:	e59f0120 	ldr	r0, [pc, #288]	; 1b4c <do_load_serial_bin+0x144>
    1a28:	e3a06000 	mov	r6, #0	; 0x0
    1a2c:	e59f311c 	ldr	r3, [pc, #284]	; 1b50 <do_load_serial_bin+0x148>
    1a30:	e1a0e00f 	mov	lr, pc
    1a34:	e593f000 	ldr	pc, [r3]
    1a38:	e1a04006 	mov	r4, r6
    1a3c:	ea00000c 	b	1a74 <do_load_serial_bin+0x6c>
    1a40:	e5930004 	ldr	r0, [r3, #4]
    1a44:	e3a01000 	mov	r1, #0	; 0x0
    1a48:	e3a02010 	mov	r2, #16	; 0x10
    1a4c:	eb0025e5 	bl	b1e8 <simple_strtoul>
    1a50:	e3560003 	cmp	r6, #3	; 0x3
    1a54:	e1a04000 	mov	r4, r0
    1a58:	13a06000 	movne	r6, #0	; 0x0
    1a5c:	1a000004 	bne	1a74 <do_load_serial_bin+0x6c>
    1a60:	e5950008 	ldr	r0, [r5, #8]
    1a64:	e3a01000 	mov	r1, #0	; 0x0
    1a68:	e3a02010 	mov	r2, #16	; 0x10
    1a6c:	eb0025dd 	bl	b1e8 <simple_strtoul>
    1a70:	e1a06000 	mov	r6, r0
    1a74:	e5950000 	ldr	r0, [r5]
    1a78:	e59f10d4 	ldr	r1, [pc, #212]	; 1b54 <do_load_serial_bin+0x14c>
    1a7c:	ebfffd8a 	bl	10ac <strcmp>
    1a80:	e3500000 	cmp	r0, #0	; 0x0
    1a84:	1a000007 	bne	1aa8 <do_load_serial_bin+0xa0>
    1a88:	e1a01004 	mov	r1, r4
    1a8c:	e59f00c4 	ldr	r0, [pc, #196]	; 1b58 <do_load_serial_bin+0x150>
    1a90:	e59f30b8 	ldr	r3, [pc, #184]	; 1b50 <do_load_serial_bin+0x148>
    1a94:	e1a0e00f 	mov	lr, pc
    1a98:	e593f000 	ldr	pc, [r3]
    1a9c:	e1a00004 	mov	r0, r4
    1aa0:	ebffff93 	bl	18f4 <load_serial_ymodem>
    1aa4:	ea000025 	b	1b40 <do_load_serial_bin+0x138>
    1aa8:	e5950000 	ldr	r0, [r5]
    1aac:	e59f10a8 	ldr	r1, [pc, #168]	; 1b5c <do_load_serial_bin+0x154>
    1ab0:	ebfffd7d 	bl	10ac <strcmp>
    1ab4:	e3500000 	cmp	r0, #0	; 0x0
    1ab8:	e59f5090 	ldr	r5, [pc, #144]	; 1b50 <do_load_serial_bin+0x148>
    1abc:	1a00001b 	bne	1b30 <do_load_serial_bin+0x128>
    1ac0:	e1a02006 	mov	r2, r6
    1ac4:	e59f0094 	ldr	r0, [pc, #148]	; 1b60 <do_load_serial_bin+0x158>
    1ac8:	e1a01004 	mov	r1, r4
    1acc:	e1a0e00f 	mov	lr, pc
    1ad0:	e595f000 	ldr	pc, [r5]
    1ad4:	e1a00004 	mov	r0, r4
    1ad8:	e1a01006 	mov	r1, r6
    1adc:	eb002f3a 	bl	d7cc <control_send>
    1ae0:	e3500000 	cmp	r0, #0	; 0x0
    1ae4:	aa000005 	bge	1b00 <do_load_serial_bin+0xf8>
    1ae8:	e59d0004 	ldr	r0, [sp, #4]
    1aec:	e5954000 	ldr	r4, [r5]
    1af0:	eb002d39 	bl	cfdc <xyzModem_error>
    1af4:	e1a01000 	mov	r1, r0
    1af8:	e59f0064 	ldr	r0, [pc, #100]	; 1b64 <do_load_serial_bin+0x15c>
    1afc:	e12fff34 	blx	r4
    1b00:	e28d0004 	add	r0, sp, #4	; 0x4
    1b04:	eb002d1c 	bl	cf7c <xyzModem_stream_close>
    1b08:	e3a00000 	mov	r0, #0	; 0x0
    1b0c:	e59f1054 	ldr	r1, [pc, #84]	; 1b68 <do_load_serial_bin+0x160>
    1b10:	eb002fca 	bl	da40 <xyzModem_stream_terminate>
    1b14:	e1a01006 	mov	r1, r6
    1b18:	e59f004c 	ldr	r0, [pc, #76]	; 1b6c <do_load_serial_bin+0x164>
    1b1c:	e1a02006 	mov	r2, r6
    1b20:	e59f3028 	ldr	r3, [pc, #40]	; 1b50 <do_load_serial_bin+0x148>
    1b24:	e1a0e00f 	mov	lr, pc
    1b28:	e593f000 	ldr	pc, [r3]
    1b2c:	ea000003 	b	1b40 <do_load_serial_bin+0x138>
    1b30:	e5971014 	ldr	r1, [r7, #20]
    1b34:	e59f0034 	ldr	r0, [pc, #52]	; 1b70 <do_load_serial_bin+0x168>
    1b38:	e1a0e00f 	mov	lr, pc
    1b3c:	e595f000 	ldr	pc, [r5]
    1b40:	e3a00000 	mov	r0, #0	; 0x0
    1b44:	e28dd00c 	add	sp, sp, #12	; 0xc
    1b48:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1b4c:	0000fce5 	.word	0x0000fce5
    1b50:	3c00d68c 	.word	0x3c00d68c
    1b54:	0000fcff 	.word	0x0000fcff
    1b58:	0000fd02 	.word	0x0000fd02
    1b5c:	0000fd45 	.word	0x0000fd45
    1b60:	0000fd48 	.word	0x0000fd48
    1b64:	0000fd95 	.word	0x0000fd95
    1b68:	000019dc 	.word	0x000019dc
    1b6c:	0000fcbd 	.word	0x0000fcbd
    1b70:	0000fd94 	.word	0x0000fd94

00001b74 <do_mem_cp>:
    1b74:	e3520004 	cmp	r2, #4	; 0x4
    1b78:	e92d4070 	push	{r4, r5, r6, lr}
    1b7c:	e1a04003 	mov	r4, r3
    1b80:	0a000002 	beq	1b90 <do_mem_cp+0x1c>
    1b84:	eb0003ad 	bl	2a40 <cmd_usage>
    1b88:	e3a00001 	mov	r0, #1	; 0x1
    1b8c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1b90:	e5930004 	ldr	r0, [r3, #4]
    1b94:	e3a01000 	mov	r1, #0	; 0x0
    1b98:	e3a02010 	mov	r2, #16	; 0x10
    1b9c:	eb002591 	bl	b1e8 <simple_strtoul>
    1ba0:	e3a01000 	mov	r1, #0	; 0x0
    1ba4:	e3a02010 	mov	r2, #16	; 0x10
    1ba8:	e1a06000 	mov	r6, r0
    1bac:	e5940008 	ldr	r0, [r4, #8]
    1bb0:	eb00258c 	bl	b1e8 <simple_strtoul>
    1bb4:	e3a01000 	mov	r1, #0	; 0x0
    1bb8:	e1a05000 	mov	r5, r0
    1bbc:	e3a02010 	mov	r2, #16	; 0x10
    1bc0:	e594000c 	ldr	r0, [r4, #12]
    1bc4:	eb002587 	bl	b1e8 <simple_strtoul>
    1bc8:	e2800003 	add	r0, r0, #3	; 0x3
    1bcc:	e1b00120 	lsrs	r0, r0, #2
    1bd0:	1a000002 	bne	1be0 <do_mem_cp+0x6c>
    1bd4:	ea000006 	b	1bf4 <do_mem_cp+0x80>
    1bd8:	e5163004 	ldr	r3, [r6, #-4]
    1bdc:	e5053004 	str	r3, [r5, #-4]
    1be0:	e3500000 	cmp	r0, #0	; 0x0
    1be4:	e2866004 	add	r6, r6, #4	; 0x4
    1be8:	e2400001 	sub	r0, r0, #1	; 0x1
    1bec:	e2855004 	add	r5, r5, #4	; 0x4
    1bf0:	1afffff8 	bne	1bd8 <do_mem_cp+0x64>
    1bf4:	e3a00000 	mov	r0, #0	; 0x0
    1bf8:	e8bd8070 	pop	{r4, r5, r6, pc}

00001bfc <do_mem_cmp>:
    1bfc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1c00:	e3520004 	cmp	r2, #4	; 0x4
    1c04:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1c08:	e1a04003 	mov	r4, r3
    1c0c:	0a000002 	beq	1c1c <do_mem_cmp+0x20>
    1c10:	eb00038a 	bl	2a40 <cmd_usage>
    1c14:	e3a04001 	mov	r4, #1	; 0x1
    1c18:	ea000029 	b	1cc4 <do_mem_cmp+0xc8>
    1c1c:	e5930004 	ldr	r0, [r3, #4]
    1c20:	e3a01000 	mov	r1, #0	; 0x0
    1c24:	e3a02010 	mov	r2, #16	; 0x10
    1c28:	eb00256e 	bl	b1e8 <simple_strtoul>
    1c2c:	e3a01000 	mov	r1, #0	; 0x0
    1c30:	e3a02010 	mov	r2, #16	; 0x10
    1c34:	e1a07000 	mov	r7, r0
    1c38:	e5940008 	ldr	r0, [r4, #8]
    1c3c:	eb002569 	bl	b1e8 <simple_strtoul>
    1c40:	e3a01000 	mov	r1, #0	; 0x0
    1c44:	e1a06000 	mov	r6, r0
    1c48:	e3a02010 	mov	r2, #16	; 0x10
    1c4c:	e594000c 	ldr	r0, [r4, #12]
    1c50:	eb002564 	bl	b1e8 <simple_strtoul>
    1c54:	e2800003 	add	r0, r0, #3	; 0x3
    1c58:	e1a00120 	lsr	r0, r0, #2
    1c5c:	e3a05000 	mov	r5, #0	; 0x0
    1c60:	ea00000f 	b	1ca4 <do_mem_cmp+0xa8>
    1c64:	e5972000 	ldr	r2, [r7]
    1c68:	e5963000 	ldr	r3, [r6]
    1c6c:	e1520003 	cmp	r2, r3
    1c70:	0a000008 	beq	1c98 <do_mem_cmp+0x9c>
    1c74:	e58d3000 	str	r3, [sp]
    1c78:	e1a01007 	mov	r1, r7
    1c7c:	e1a03006 	mov	r3, r6
    1c80:	e59f0048 	ldr	r0, [pc, #72]	; 1cd0 <do_mem_cmp+0xd4>
    1c84:	e59fc048 	ldr	ip, [pc, #72]	; 1cd4 <do_mem_cmp+0xd8>
    1c88:	e1a0e00f 	mov	lr, pc
    1c8c:	e59cf000 	ldr	pc, [ip]
    1c90:	e3a04001 	mov	r4, #1	; 0x1
    1c94:	ea000005 	b	1cb0 <do_mem_cmp+0xb4>
    1c98:	e2855001 	add	r5, r5, #1	; 0x1
    1c9c:	e2877004 	add	r7, r7, #4	; 0x4
    1ca0:	e2866004 	add	r6, r6, #4	; 0x4
    1ca4:	e1500005 	cmp	r0, r5
    1ca8:	1affffed 	bne	1c64 <do_mem_cmp+0x68>
    1cac:	e3a04000 	mov	r4, #0	; 0x0
    1cb0:	e1a01005 	mov	r1, r5
    1cb4:	e59f001c 	ldr	r0, [pc, #28]	; 1cd8 <do_mem_cmp+0xdc>
    1cb8:	e59f3014 	ldr	r3, [pc, #20]	; 1cd4 <do_mem_cmp+0xd8>
    1cbc:	e1a0e00f 	mov	lr, pc
    1cc0:	e593f000 	ldr	pc, [r3]
    1cc4:	e1a00004 	mov	r0, r4
    1cc8:	e28dd00c 	add	sp, sp, #12	; 0xc
    1ccc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1cd0:	0000fe9a 	.word	0x0000fe9a
    1cd4:	3c00d68c 	.word	0x3c00d68c
    1cd8:	0000fed2 	.word	0x0000fed2

00001cdc <do_mem_mw>:
    1cdc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1ce0:	e1a06002 	mov	r6, r2
    1ce4:	e2422003 	sub	r2, r2, #3	; 0x3
    1ce8:	e3520001 	cmp	r2, #1	; 0x1
    1cec:	e24dd004 	sub	sp, sp, #4	; 0x4
    1cf0:	e1a04003 	mov	r4, r3
    1cf4:	9a000002 	bls	1d04 <do_mem_mw+0x28>
    1cf8:	eb000350 	bl	2a40 <cmd_usage>
    1cfc:	e3a00001 	mov	r0, #1	; 0x1
    1d00:	ea000016 	b	1d60 <do_mem_mw+0x84>
    1d04:	e3a01000 	mov	r1, #0	; 0x0
    1d08:	e3a02010 	mov	r2, #16	; 0x10
    1d0c:	e5930004 	ldr	r0, [r3, #4]
    1d10:	eb002534 	bl	b1e8 <simple_strtoul>
    1d14:	e3a01000 	mov	r1, #0	; 0x0
    1d18:	e1a05000 	mov	r5, r0
    1d1c:	e3a02010 	mov	r2, #16	; 0x10
    1d20:	e5940008 	ldr	r0, [r4, #8]
    1d24:	eb00252f 	bl	b1e8 <simple_strtoul>
    1d28:	e3560004 	cmp	r6, #4	; 0x4
    1d2c:	e1a07000 	mov	r7, r0
    1d30:	13a00001 	movne	r0, #1	; 0x1
    1d34:	1a000006 	bne	1d54 <do_mem_mw+0x78>
    1d38:	e594000c 	ldr	r0, [r4, #12]
    1d3c:	e3a01000 	mov	r1, #0	; 0x0
    1d40:	e3a02010 	mov	r2, #16	; 0x10
    1d44:	eb002527 	bl	b1e8 <simple_strtoul>
    1d48:	ea000001 	b	1d54 <do_mem_mw+0x78>
    1d4c:	e5057004 	str	r7, [r5, #-4]
    1d50:	e2400001 	sub	r0, r0, #1	; 0x1
    1d54:	e3500000 	cmp	r0, #0	; 0x0
    1d58:	e2855004 	add	r5, r5, #4	; 0x4
    1d5c:	1afffffa 	bne	1d4c <do_mem_mw+0x70>
    1d60:	e28dd004 	add	sp, sp, #4	; 0x4
    1d64:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00001d68 <do_mem_md>:
    1d68:	e92d4070 	push	{r4, r5, r6, lr}
    1d6c:	e3520001 	cmp	r2, #1	; 0x1
    1d70:	e24dd008 	sub	sp, sp, #8	; 0x8
    1d74:	e1a04002 	mov	r4, r2
    1d78:	e1a05003 	mov	r5, r3
    1d7c:	ca000002 	bgt	1d8c <do_mem_md+0x24>
    1d80:	eb00032e 	bl	2a40 <cmd_usage>
    1d84:	e3a00001 	mov	r0, #1	; 0x1
    1d88:	ea000014 	b	1de0 <do_mem_md+0x78>
    1d8c:	e5930004 	ldr	r0, [r3, #4]
    1d90:	e3a01000 	mov	r1, #0	; 0x0
    1d94:	e3a02010 	mov	r2, #16	; 0x10
    1d98:	eb002512 	bl	b1e8 <simple_strtoul>
    1d9c:	e3540002 	cmp	r4, #2	; 0x2
    1da0:	e1a06000 	mov	r6, r0
    1da4:	03a030a0 	moveq	r3, #160	; 0xa0
    1da8:	0a000004 	beq	1dc0 <do_mem_md+0x58>
    1dac:	e5950008 	ldr	r0, [r5, #8]
    1db0:	e3a01000 	mov	r1, #0	; 0x0
    1db4:	e3a02010 	mov	r2, #16	; 0x10
    1db8:	eb00250a 	bl	b1e8 <simple_strtoul>
    1dbc:	e1a03000 	mov	r3, r0
    1dc0:	e3a0c004 	mov	ip, #4	; 0x4
    1dc4:	e1a00006 	mov	r0, r6
    1dc8:	e1a0200c 	mov	r2, ip
    1dcc:	e1a03123 	lsr	r3, r3, #2
    1dd0:	e1a01006 	mov	r1, r6
    1dd4:	e58dc000 	str	ip, [sp]
    1dd8:	eb00246a 	bl	af88 <print_buffer>
    1ddc:	e3a00000 	mov	r0, #0	; 0x0
    1de0:	e28dd008 	add	sp, sp, #8	; 0x8
    1de4:	e8bd8070 	pop	{r4, r5, r6, pc}

00001de8 <do_sc>:
    1de8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1dec:	e3520002 	cmp	r2, #2	; 0x2
    1df0:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    1df4:	0a000002 	beq	1e04 <do_sc+0x1c>
    1df8:	eb000310 	bl	2a40 <cmd_usage>
    1dfc:	e3a00001 	mov	r0, #1	; 0x1
    1e00:	ea000060 	b	1f88 <do_sc+0x1a0>
    1e04:	e5930004 	ldr	r0, [r3, #4]
    1e08:	e3a01000 	mov	r1, #0	; 0x0
    1e0c:	e3a02010 	mov	r2, #16	; 0x10
    1e10:	eb0024f4 	bl	b1e8 <simple_strtoul>
    1e14:	e28d4f41 	add	r4, sp, #260	; 0x104
    1e18:	e2844002 	add	r4, r4, #2	; 0x2
    1e1c:	e59f116c 	ldr	r1, [pc, #364]	; 1f90 <do_sc+0x1a8>
    1e20:	e1a06000 	mov	r6, r0
    1e24:	e3a02012 	mov	r2, #18	; 0x12
    1e28:	e1a00004 	mov	r0, r4
    1e2c:	ebfffce5 	bl	11c8 <memcpy>
    1e30:	e3a01000 	mov	r1, #0	; 0x0
    1e34:	ea000004 	b	1e4c <do_sc+0x64>
    1e38:	e7d12006 	ldrb	r2, [r1, r6]
    1e3c:	e7d43001 	ldrb	r3, [r4, r1]
    1e40:	e2811001 	add	r1, r1, #1	; 0x1
    1e44:	e1520003 	cmp	r2, r3
    1e48:	1a000014 	bne	1ea0 <do_sc+0xb8>
    1e4c:	e3510012 	cmp	r1, #18	; 0x12
    1e50:	e0815006 	add	r5, r1, r6
    1e54:	1afffff7 	bne	1e38 <do_sc+0x50>
    1e58:	e2851eff 	add	r1, r5, #4080	; 0xff0
    1e5c:	e281100f 	add	r1, r1, #15	; 0xf
    1e60:	e1a02005 	mov	r2, r5
    1e64:	e5d23000 	ldrb	r3, [r2]
    1e68:	e353000a 	cmp	r3, #10	; 0xa
    1e6c:	1a000005 	bne	1e88 <do_sc+0xa0>
    1e70:	e5d23001 	ldrb	r3, [r2, #1]
    1e74:	e353005d 	cmp	r3, #93	; 0x5d
    1e78:	1a000002 	bne	1e88 <do_sc+0xa0>
    1e7c:	e5d23002 	ldrb	r3, [r2, #2]
    1e80:	e353005d 	cmp	r3, #93	; 0x5d
    1e84:	0a000003 	beq	1e98 <do_sc+0xb0>
    1e88:	e1520001 	cmp	r2, r1
    1e8c:	e2822001 	add	r2, r2, #1	; 0x1
    1e90:	1afffff3 	bne	1e64 <do_sc+0x7c>
    1e94:	ea000001 	b	1ea0 <do_sc+0xb8>
    1e98:	e3550000 	cmp	r5, #0	; 0x0
    1e9c:	1a000006 	bne	1ebc <do_sc+0xd4>
    1ea0:	e59f00ec 	ldr	r0, [pc, #236]	; 1f94 <do_sc+0x1ac>
    1ea4:	e1a01006 	mov	r1, r6
    1ea8:	e59f30e8 	ldr	r3, [pc, #232]	; 1f98 <do_sc+0x1b0>
    1eac:	e1a0e00f 	mov	lr, pc
    1eb0:	e593f000 	ldr	pc, [r3]
    1eb4:	e3e00000 	mvn	r0, #0	; 0x0
    1eb8:	ea000032 	b	1f88 <do_sc+0x1a0>
    1ebc:	e59f40d4 	ldr	r4, [pc, #212]	; 1f98 <do_sc+0x1b0>
    1ec0:	e1a01006 	mov	r1, r6
    1ec4:	e59f00d0 	ldr	r0, [pc, #208]	; 1f9c <do_sc+0x1b4>
    1ec8:	e28d6008 	add	r6, sp, #8	; 0x8
    1ecc:	e1a0e00f 	mov	lr, pc
    1ed0:	e594f000 	ldr	pc, [r4]
    1ed4:	e2466002 	sub	r6, r6, #2	; 0x2
    1ed8:	e1a07004 	mov	r7, r4
    1edc:	ea000000 	b	1ee4 <do_sc+0xfc>
    1ee0:	e2855001 	add	r5, r5, #1	; 0x1
    1ee4:	e5d53000 	ldrb	r3, [r5]
    1ee8:	e353002d 	cmp	r3, #45	; 0x2d
    1eec:	1353000a 	cmpne	r3, #10	; 0xa
    1ef0:	0afffffa 	beq	1ee0 <do_sc+0xf8>
    1ef4:	e353005b 	cmp	r3, #91	; 0x5b
    1ef8:	0afffff8 	beq	1ee0 <do_sc+0xf8>
    1efc:	e353005d 	cmp	r3, #93	; 0x5d
    1f00:	0a000001 	beq	1f0c <do_sc+0x124>
    1f04:	e1a02006 	mov	r2, r6
    1f08:	ea000005 	b	1f24 <do_sc+0x13c>
    1f0c:	e5d53001 	ldrb	r3, [r5, #1]
    1f10:	e353005d 	cmp	r3, #93	; 0x5d
    1f14:	1afffffa 	bne	1f04 <do_sc+0x11c>
    1f18:	ea00000c 	b	1f50 <do_sc+0x168>
    1f1c:	e5423001 	strb	r3, [r2, #-1]
    1f20:	e2855001 	add	r5, r5, #1	; 0x1
    1f24:	e5d53000 	ldrb	r3, [r5]
    1f28:	e1a01002 	mov	r1, r2
    1f2c:	e353000a 	cmp	r3, #10	; 0xa
    1f30:	1353005d 	cmpne	r3, #93	; 0x5d
    1f34:	03a04000 	moveq	r4, #0	; 0x0
    1f38:	13a04001 	movne	r4, #1	; 0x1
    1f3c:	e2822001 	add	r2, r2, #1	; 0x1
    1f40:	1afffff5 	bne	1f1c <do_sc+0x134>
    1f44:	e3550000 	cmp	r5, #0	; 0x0
    1f48:	e5c14000 	strb	r4, [r1]
    1f4c:	1a000005 	bne	1f68 <do_sc+0x180>
    1f50:	e59f0048 	ldr	r0, [pc, #72]	; 1fa0 <do_sc+0x1b8>
    1f54:	e59f303c 	ldr	r3, [pc, #60]	; 1f98 <do_sc+0x1b0>
    1f58:	e1a0e00f 	mov	lr, pc
    1f5c:	e593f000 	ldr	pc, [r3]
    1f60:	e3a00000 	mov	r0, #0	; 0x0
    1f64:	ea000007 	b	1f88 <do_sc+0x1a0>
    1f68:	e1a01006 	mov	r1, r6
    1f6c:	e59f0030 	ldr	r0, [pc, #48]	; 1fa4 <do_sc+0x1bc>
    1f70:	e1a0e00f 	mov	lr, pc
    1f74:	e597f000 	ldr	pc, [r7]
    1f78:	e1a00006 	mov	r0, r6
    1f7c:	e1a01004 	mov	r1, r4
    1f80:	eb0028ec 	bl	c338 <run_command>
    1f84:	eaffffd6 	b	1ee4 <do_sc+0xfc>
    1f88:	e28ddf47 	add	sp, sp, #284	; 0x11c
    1f8c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1f90:	0000f8da 	.word	0x0000f8da
    1f94:	0000ff9c 	.word	0x0000ff9c
    1f98:	3c00d68c 	.word	0x3c00d68c
    1f9c:	0000ffb9 	.word	0x0000ffb9
    1fa0:	0000ffd2 	.word	0x0000ffd2
    1fa4:	0000ffdf 	.word	0x0000ffdf

00001fa8 <do_ls>:
    1fa8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1fac:	e59f001c 	ldr	r0, [pc, #28]	; 1fd0 <do_ls+0x28>
    1fb0:	e24dd004 	sub	sp, sp, #4	; 0x4
    1fb4:	e59f1018 	ldr	r1, [pc, #24]	; 1fd4 <do_ls+0x2c>
    1fb8:	e59f3018 	ldr	r3, [pc, #24]	; 1fd8 <do_ls+0x30>
    1fbc:	e1a0e00f 	mov	lr, pc
    1fc0:	e593f000 	ldr	pc, [r3]
    1fc4:	e3a00000 	mov	r0, #0	; 0x0
    1fc8:	e28dd004 	add	sp, sp, #4	; 0x4
    1fcc:	e8bd8000 	pop	{pc}
    1fd0:	0000fd95 	.word	0x0000fd95
    1fd4:	3c00d09c 	.word	0x3c00d09c
    1fd8:	3c00d68c 	.word	0x3c00d68c

00001fdc <do_linkpc>:
    1fdc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1fe0:	e24dd004 	sub	sp, sp, #4	; 0x4
    1fe4:	eb0021a7 	bl	a688 <linkpc_loop>
    1fe8:	e3a00000 	mov	r0, #0	; 0x0
    1fec:	e28dd004 	add	sp, sp, #4	; 0x4
    1ff0:	e8bd8000 	pop	{pc}

00001ff4 <do_pads>:
    1ff4:	e3520002 	cmp	r2, #2	; 0x2
    1ff8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    1ffc:	d59f0194 	ldrle	r0, [pc, #404]	; 2198 <do_pads+0x1a4>
    2000:	e1a05002 	mov	r5, r2
    2004:	e1a06003 	mov	r6, r3
    2008:	da000015 	ble	2064 <do_pads+0x70>
    200c:	e5930008 	ldr	r0, [r3, #8]
    2010:	e3a01000 	mov	r1, #0	; 0x0
    2014:	e3a0200a 	mov	r2, #10	; 0xa
    2018:	eb002472 	bl	b1e8 <simple_strtoul>
    201c:	e3550003 	cmp	r5, #3	; 0x3
    2020:	e1a04000 	mov	r4, r0
    2024:	01a07000 	moveq	r7, r0
    2028:	0a000004 	beq	2040 <do_pads+0x4c>
    202c:	e596000c 	ldr	r0, [r6, #12]
    2030:	e3a01000 	mov	r1, #0	; 0x0
    2034:	e3a0200a 	mov	r2, #10	; 0xa
    2038:	eb00246a 	bl	b1e8 <simple_strtoul>
    203c:	e1a07000 	mov	r7, r0
    2040:	e1943007 	orrs	r3, r4, r7
    2044:	4a000005 	bmi	2060 <do_pads+0x6c>
    2048:	e1a05004 	mov	r5, r4
    204c:	e2866004 	add	r6, r6, #4	; 0x4
    2050:	e59f9144 	ldr	r9, [pc, #324]	; 219c <do_pads+0x1a8>
    2054:	e59f8144 	ldr	r8, [pc, #324]	; 21a0 <do_pads+0x1ac>
    2058:	e59fa144 	ldr	sl, [pc, #324]	; 21a4 <do_pads+0x1b0>
    205c:	ea000049 	b	2188 <do_pads+0x194>
    2060:	e59f0140 	ldr	r0, [pc, #320]	; 21a8 <do_pads+0x1b4>
    2064:	e59f3134 	ldr	r3, [pc, #308]	; 21a0 <do_pads+0x1ac>
    2068:	e1a0e00f 	mov	lr, pc
    206c:	e593f000 	ldr	pc, [r3]
    2070:	ea000046 	b	2190 <do_pads+0x19c>
    2074:	e5960000 	ldr	r0, [r6]
    2078:	e59f111c 	ldr	r1, [pc, #284]	; 219c <do_pads+0x1a8>
    207c:	e3a02002 	mov	r2, #2	; 0x2
    2080:	ebfffc17 	bl	10e4 <strncmp>
    2084:	e3500000 	cmp	r0, #0	; 0x0
    2088:	1a00000b 	bne	20bc <do_pads+0xc8>
    208c:	e1a02000 	mov	r2, r0
    2090:	e3a01001 	mov	r1, #1	; 0x1
    2094:	e1a00005 	mov	r0, r5
    2098:	ebfffadb 	bl	c0c <pads_chmod>
    209c:	e1a00005 	mov	r0, r5
    20a0:	e5984000 	ldr	r4, [r8]
    20a4:	ebfffb6e 	bl	e64 <pads_getpin>
    20a8:	e1a01005 	mov	r1, r5
    20ac:	e1a02000 	mov	r2, r0
    20b0:	e59f00f4 	ldr	r0, [pc, #244]	; 21ac <do_pads+0x1b8>
    20b4:	e12fff34 	blx	r4
    20b8:	ea000031 	b	2184 <do_pads+0x190>
    20bc:	e59f10ec 	ldr	r1, [pc, #236]	; 21b0 <do_pads+0x1bc>
    20c0:	e3a02004 	mov	r2, #4	; 0x4
    20c4:	e5960000 	ldr	r0, [r6]
    20c8:	ebfffc05 	bl	10e4 <strncmp>
    20cc:	e3500000 	cmp	r0, #0	; 0x0
    20d0:	01a00005 	moveq	r0, r5
    20d4:	03a01002 	moveq	r1, #2	; 0x2
    20d8:	03a02001 	moveq	r2, #1	; 0x1
    20dc:	0a000011 	beq	2128 <do_pads+0x134>
    20e0:	e59f10cc 	ldr	r1, [pc, #204]	; 21b4 <do_pads+0x1c0>
    20e4:	e3a02003 	mov	r2, #3	; 0x3
    20e8:	e5960000 	ldr	r0, [r6]
    20ec:	ebfffbfc 	bl	10e4 <strncmp>
    20f0:	e3500000 	cmp	r0, #0	; 0x0
    20f4:	01a02000 	moveq	r2, r0
    20f8:	03a01002 	moveq	r1, #2	; 0x2
    20fc:	01a00005 	moveq	r0, r5
    2100:	0a000008 	beq	2128 <do_pads+0x134>
    2104:	e5960000 	ldr	r0, [r6]
    2108:	e59f10a8 	ldr	r1, [pc, #168]	; 21b8 <do_pads+0x1c4>
    210c:	e3a02004 	mov	r2, #4	; 0x4
    2110:	ebfffbf3 	bl	10e4 <strncmp>
    2114:	e3500000 	cmp	r0, #0	; 0x0
    2118:	1a000004 	bne	2130 <do_pads+0x13c>
    211c:	e1a01000 	mov	r1, r0
    2120:	e1a02001 	mov	r2, r1
    2124:	e1a00005 	mov	r0, r5
    2128:	ebfffab7 	bl	c0c <pads_chmod>
    212c:	ea000014 	b	2184 <do_pads+0x190>
    2130:	e5960000 	ldr	r0, [r6]
    2134:	e59f1080 	ldr	r1, [pc, #128]	; 21bc <do_pads+0x1c8>
    2138:	e3a02002 	mov	r2, #2	; 0x2
    213c:	ebfffbe8 	bl	10e4 <strncmp>
    2140:	e3500000 	cmp	r0, #0	; 0x0
    2144:	1a00000c 	bne	217c <do_pads+0x188>
    2148:	e1a00005 	mov	r0, r5
    214c:	ebfffa98 	bl	bb4 <pads_states>
    2150:	e3500000 	cmp	r0, #0	; 0x0
    2154:	e5983000 	ldr	r3, [r8]
    2158:	059f2060 	ldreq	r2, [pc, #96]	; 21c0 <do_pads+0x1cc>
    215c:	0a000002 	beq	216c <do_pads+0x178>
    2160:	e3500001 	cmp	r0, #1	; 0x1
    2164:	01a02009 	moveq	r2, r9
    2168:	11a0200a 	movne	r2, sl
    216c:	e59f0050 	ldr	r0, [pc, #80]	; 21c4 <do_pads+0x1d0>
    2170:	e1a01005 	mov	r1, r5
    2174:	e12fff33 	blx	r3
    2178:	ea000001 	b	2184 <do_pads+0x190>
    217c:	e59f0044 	ldr	r0, [pc, #68]	; 21c8 <do_pads+0x1d4>
    2180:	eaffffb7 	b	2064 <do_pads+0x70>
    2184:	e2855001 	add	r5, r5, #1	; 0x1
    2188:	e1550007 	cmp	r5, r7
    218c:	daffffb8 	ble	2074 <do_pads+0x80>
    2190:	e3a00000 	mov	r0, #0	; 0x0
    2194:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    2198:	00010003 	.word	0x00010003
    219c:	00010040 	.word	0x00010040
    21a0:	3c00d68c 	.word	0x3c00d68c
    21a4:	0001189a 	.word	0x0001189a
    21a8:	0001001b 	.word	0x0001001b
    21ac:	00010043 	.word	0x00010043
    21b0:	00010057 	.word	0x00010057
    21b4:	0001005c 	.word	0x0001005c
    21b8:	00010060 	.word	0x00010060
    21bc:	00010065 	.word	0x00010065
    21c0:	00010068 	.word	0x00010068
    21c4:	00010071 	.word	0x00010071
    21c8:	00010086 	.word	0x00010086

000021cc <do_mux>:
    21cc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    21d0:	e3520001 	cmp	r2, #1	; 0x1
    21d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    21d8:	da000005 	ble	21f4 <do_mux+0x28>
    21dc:	e5930004 	ldr	r0, [r3, #4]
    21e0:	e3a01000 	mov	r1, #0	; 0x0
    21e4:	e3a0200a 	mov	r2, #10	; 0xa
    21e8:	eb0023fe 	bl	b1e8 <simple_strtoul>
    21ec:	e6ff0070 	uxth	r0, r0
    21f0:	eb002328 	bl	ae98 <set_mux>
    21f4:	e59f301c 	ldr	r3, [pc, #28]	; 2218 <do_mux+0x4c>
    21f8:	e59f001c 	ldr	r0, [pc, #28]	; 221c <do_mux+0x50>
    21fc:	e5931000 	ldr	r1, [r3]
    2200:	e59f3018 	ldr	r3, [pc, #24]	; 2220 <do_mux+0x54>
    2204:	e1a0e00f 	mov	lr, pc
    2208:	e593f000 	ldr	pc, [r3]
    220c:	e3a00000 	mov	r0, #0	; 0x0
    2210:	e28dd004 	add	sp, sp, #4	; 0x4
    2214:	e8bd8000 	pop	{pc}
    2218:	21e09000 	.word	0x21e09000
    221c:	0001009f 	.word	0x0001009f
    2220:	3c00d68c 	.word	0x3c00d68c

00002224 <do_mac>:
    2224:	e59f3020 	ldr	r3, [pc, #32]	; 224c <do_mac+0x28>
    2228:	e92d4010 	push	{r4, lr}
    222c:	e5934000 	ldr	r4, [r3]
    2230:	ebfff9ed 	bl	9ec <efuse_mac>
    2234:	e1a02000 	mov	r2, r0
    2238:	e1a03001 	mov	r3, r1
    223c:	e59f000c 	ldr	r0, [pc, #12]	; 2250 <do_mac+0x2c>
    2240:	e12fff34 	blx	r4
    2244:	e3a00000 	mov	r0, #0	; 0x0
    2248:	e8bd8010 	pop	{r4, pc}
    224c:	3c00d68c 	.word	0x3c00d68c
    2250:	000100b1 	.word	0x000100b1

00002254 <do_boot>:
    2254:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2258:	e3a00000 	mov	r0, #0	; 0x0
    225c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2260:	ebfff858 	bl	3c8 <try_boot>
    2264:	e3a00000 	mov	r0, #0	; 0x0
    2268:	e28dd004 	add	sp, sp, #4	; 0x4
    226c:	e8bd8000 	pop	{pc}

00002270 <do_efuse>:
    2270:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2274:	e3520001 	cmp	r2, #1	; 0x1
    2278:	e24dd004 	sub	sp, sp, #4	; 0x4
    227c:	da000006 	ble	229c <do_efuse+0x2c>
    2280:	e5930004 	ldr	r0, [r3, #4]
    2284:	e59f1024 	ldr	r1, [pc, #36]	; 22b0 <do_efuse+0x40>
    2288:	e3a02002 	mov	r2, #2	; 0x2
    228c:	ebfffb94 	bl	10e4 <strncmp>
    2290:	e3500000 	cmp	r0, #0	; 0x0
    2294:	02800001 	addeq	r0, r0, #1	; 0x1
    2298:	0a000000 	beq	22a0 <do_efuse+0x30>
    229c:	e3a00000 	mov	r0, #0	; 0x0
    22a0:	ebfff9d4 	bl	9f8 <efuse_info>
    22a4:	e3a00000 	mov	r0, #0	; 0x0
    22a8:	e28dd004 	add	sp, sp, #4	; 0x4
    22ac:	e8bd8000 	pop	{pc}
    22b0:	000100c9 	.word	0x000100c9

000022b4 <do_pi>:
    22b4:	e92d4030 	push	{r4, r5, lr}
    22b8:	e3520001 	cmp	r2, #1	; 0x1
    22bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    22c0:	e1a04002 	mov	r4, r2
    22c4:	e1a05003 	mov	r5, r3
    22c8:	da000015 	ble	2324 <do_pi+0x70>
    22cc:	e59f10ac 	ldr	r1, [pc, #172]	; 2380 <do_pi+0xcc>
    22d0:	e5930004 	ldr	r0, [r3, #4]
    22d4:	e3a02004 	mov	r2, #4	; 0x4
    22d8:	ebfffb81 	bl	10e4 <strncmp>
    22dc:	e2501000 	subs	r1, r0, #0	; 0x0
    22e0:	1a00000d 	bne	231c <do_pi+0x68>
    22e4:	e3540002 	cmp	r4, #2	; 0x2
    22e8:	01a04001 	moveq	r4, r1
    22ec:	0a000003 	beq	2300 <do_pi+0x4c>
    22f0:	e5950008 	ldr	r0, [r5, #8]
    22f4:	e3a02010 	mov	r2, #16	; 0x10
    22f8:	eb0023ba 	bl	b1e8 <simple_strtoul>
    22fc:	e1a04000 	mov	r4, r0
    2300:	e59f007c 	ldr	r0, [pc, #124]	; 2384 <do_pi+0xd0>
    2304:	e1a01004 	mov	r1, r4
    2308:	e59f3078 	ldr	r3, [pc, #120]	; 2388 <do_pi+0xd4>
    230c:	e1a0e00f 	mov	lr, pc
    2310:	e593f000 	ldr	pc, [r3]
    2314:	e1a00004 	mov	r0, r4
    2318:	eb00109f 	bl	659c <burn>
    231c:	e3a04000 	mov	r4, #0	; 0x0
    2320:	ea000013 	b	2374 <do_pi+0xc0>
    2324:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    2328:	eb002742 	bl	c038 <ius_check_header>
    232c:	e2504000 	subs	r4, r0, #0	; 0x0
    2330:	159f0054 	ldrne	r0, [pc, #84]	; 238c <do_pi+0xd8>
    2334:	1a000005 	bne	2350 <do_pi+0x9c>
    2338:	e3a0000d 	mov	r0, #13	; 0xd
    233c:	e3a01001 	mov	r1, #1	; 0x1
    2340:	eb0009cc 	bl	4a78 <vs_assign_by_id>
    2344:	e2504000 	subs	r4, r0, #0	; 0x0
    2348:	0a000005 	beq	2364 <do_pi+0xb0>
    234c:	e59f003c 	ldr	r0, [pc, #60]	; 2390 <do_pi+0xdc>
    2350:	e1a01004 	mov	r1, r4
    2354:	e59f302c 	ldr	r3, [pc, #44]	; 2388 <do_pi+0xd4>
    2358:	e1a0e00f 	mov	lr, pc
    235c:	e593f000 	ldr	pc, [r3]
    2360:	ea000003 	b	2374 <do_pi+0xc0>
    2364:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    2368:	e3a0100d 	mov	r1, #13	; 0xd
    236c:	e1a02004 	mov	r2, r4
    2370:	eb001010 	bl	63b8 <burn_images>
    2374:	e1a00004 	mov	r0, r4
    2378:	e28dd004 	add	sp, sp, #4	; 0x4
    237c:	e8bd8030 	pop	{r4, r5, pc}
    2380:	000100cc 	.word	0x000100cc
    2384:	000100d1 	.word	0x000100d1
    2388:	3c00d68c 	.word	0x3c00d68c
    238c:	000100fb 	.word	0x000100fb
    2390:	0001011b 	.word	0x0001011b

00002394 <do_spbyte>:
    2394:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2398:	e5930004 	ldr	r0, [r3, #4]
    239c:	e24dd004 	sub	sp, sp, #4	; 0x4
    23a0:	e3a01000 	mov	r1, #0	; 0x0
    23a4:	e3a0200a 	mov	r2, #10	; 0xa
    23a8:	eb00238e 	bl	b1e8 <simple_strtoul>
    23ac:	eb001b41 	bl	90b8 <spi_set_bytes>
    23b0:	e3a00000 	mov	r0, #0	; 0x0
    23b4:	e28dd004 	add	sp, sp, #4	; 0x4
    23b8:	e8bd8000 	pop	{pc}

000023bc <do_cfg>:
    23bc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23c0:	e5930004 	ldr	r0, [r3, #4]
    23c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    23c8:	e3a01000 	mov	r1, #0	; 0x0
    23cc:	e3a02010 	mov	r2, #16	; 0x10
    23d0:	eb002384 	bl	b1e8 <simple_strtoul>
    23d4:	eb000f29 	bl	6080 <cdbg_config_isi>
    23d8:	e3a00000 	mov	r0, #0	; 0x0
    23dc:	e28dd004 	add	sp, sp, #4	; 0x4
    23e0:	e8bd8000 	pop	{pc}

000023e4 <do_sleep>:
    23e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23e8:	e3a01000 	mov	r1, #0	; 0x0
    23ec:	e24dd004 	sub	sp, sp, #4	; 0x4
    23f0:	e3a0200a 	mov	r2, #10	; 0xa
    23f4:	e5930004 	ldr	r0, [r3, #4]
    23f8:	eb00237a 	bl	b1e8 <simple_strtoul>
    23fc:	e59f3020 	ldr	r3, [pc, #32]	; 2424 <do_sleep+0x40>
    2400:	e0000093 	mul	r0, r3, r0
    2404:	eb0018bb 	bl	86f8 <udelay>
    2408:	e59f0018 	ldr	r0, [pc, #24]	; 2428 <do_sleep+0x44>
    240c:	e59f3018 	ldr	r3, [pc, #24]	; 242c <do_sleep+0x48>
    2410:	e1a0e00f 	mov	lr, pc
    2414:	e593f000 	ldr	pc, [r3]
    2418:	e3a00000 	mov	r0, #0	; 0x0
    241c:	e28dd004 	add	sp, sp, #4	; 0x4
    2420:	e8bd8000 	pop	{pc}
    2424:	000f4240 	.word	0x000f4240
    2428:	00010136 	.word	0x00010136
    242c:	3c00d68c 	.word	0x3c00d68c

00002430 <do_go>:
    2430:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2434:	e5930004 	ldr	r0, [r3, #4]
    2438:	e24dd004 	sub	sp, sp, #4	; 0x4
    243c:	e3a02010 	mov	r2, #16	; 0x10
    2440:	e3a01000 	mov	r1, #0	; 0x0
    2444:	eb002367 	bl	b1e8 <simple_strtoul>
    2448:	e3a01000 	mov	r1, #0	; 0x0
    244c:	eb000e96 	bl	5eac <cdbg_jump>
    2450:	e3a00000 	mov	r0, #0	; 0x0
    2454:	e28dd004 	add	sp, sp, #4	; 0x4
    2458:	e8bd8000 	pop	{pc}

0000245c <do_ml>:
    245c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2460:	e24dd004 	sub	sp, sp, #4	; 0x4
    2464:	ebfffc4d 	bl	15a0 <__sOINFo>
    2468:	e3a00000 	mov	r0, #0	; 0x0
    246c:	e28dd004 	add	sp, sp, #4	; 0x4
    2470:	e8bd8000 	pop	{pc}

00002474 <do_bp>:
    2474:	e92d4030 	push	{r4, r5, lr}
    2478:	e3520001 	cmp	r2, #1	; 0x1
    247c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2480:	da000005 	ble	249c <do_bp+0x28>
    2484:	e5930004 	ldr	r0, [r3, #4]
    2488:	e3a01000 	mov	r1, #0	; 0x0
    248c:	e3a02010 	mov	r2, #16	; 0x10
    2490:	eb002354 	bl	b1e8 <simple_strtoul>
    2494:	e6ff0070 	uxth	r0, r0
    2498:	ebfff830 	bl	560 <set_xom>
    249c:	e59f5034 	ldr	r5, [pc, #52]	; 24d8 <do_bp+0x64>
    24a0:	e3a00001 	mov	r0, #1	; 0x1
    24a4:	e5954000 	ldr	r4, [r5]
    24a8:	ebfff828 	bl	550 <get_xom>
    24ac:	e1a01000 	mov	r1, r0
    24b0:	e59f0024 	ldr	r0, [pc, #36]	; 24dc <do_bp+0x68>
    24b4:	e12fff34 	blx	r4
    24b8:	e5954000 	ldr	r4, [r5]
    24bc:	ebfff82e 	bl	57c <boot_info>
    24c0:	e1a01000 	mov	r1, r0
    24c4:	e59f0014 	ldr	r0, [pc, #20]	; 24e0 <do_bp+0x6c>
    24c8:	e12fff34 	blx	r4
    24cc:	e3a00000 	mov	r0, #0	; 0x0
    24d0:	e28dd004 	add	sp, sp, #4	; 0x4
    24d4:	e8bd8030 	pop	{r4, r5, pc}
    24d8:	3c00d68c 	.word	0x3c00d68c
    24dc:	00010141 	.word	0x00010141
    24e0:	00010157 	.word	0x00010157

000024e4 <do_reset>:
    24e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    24e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    24ec:	ebfff8ba 	bl	7dc <system_reset>
    24f0:	e3a00000 	mov	r0, #0	; 0x0
    24f4:	e28dd004 	add	sp, sp, #4	; 0x4
    24f8:	e8bd8000 	pop	{pc}

000024fc <output_debug>:
    24fc:	e24dd010 	sub	sp, sp, #16	; 0x10
    2500:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2504:	e24dd004 	sub	sp, sp, #4	; 0x4
    2508:	e28dc008 	add	ip, sp, #8	; 0x8
    250c:	e3700002 	cmn	r0, #2	; 0x2
    2510:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    2514:	e1a01000 	mov	r1, r0
    2518:	0a000012 	beq	2568 <output_debug+0x6c>
    251c:	e3700001 	cmn	r0, #1	; 0x1
    2520:	0a000008 	beq	2548 <output_debug+0x4c>
    2524:	e3700003 	cmn	r0, #3	; 0x3
    2528:	1a000010 	bne	2570 <output_debug+0x74>
    252c:	e59f30a4 	ldr	r3, [pc, #164]	; 25d8 <output_debug+0xdc>
    2530:	e59f00a4 	ldr	r0, [pc, #164]	; 25dc <output_debug+0xe0>
    2534:	e5931000 	ldr	r1, [r3]
    2538:	e59f30a0 	ldr	r3, [pc, #160]	; 25e0 <output_debug+0xe4>
    253c:	e1a0e00f 	mov	lr, pc
    2540:	e593f000 	ldr	pc, [r3]
    2544:	ea00001e 	b	25c4 <output_debug+0xc8>
    2548:	e59f3088 	ldr	r3, [pc, #136]	; 25d8 <output_debug+0xdc>
    254c:	e59f0090 	ldr	r0, [pc, #144]	; 25e4 <output_debug+0xe8>
    2550:	e5932000 	ldr	r2, [r3]
    2554:	e28d1010 	add	r1, sp, #16	; 0x10
    2558:	e59f3080 	ldr	r3, [pc, #128]	; 25e0 <output_debug+0xe4>
    255c:	e1a0e00f 	mov	lr, pc
    2560:	e593f000 	ldr	pc, [r3]
    2564:	ea000016 	b	25c4 <output_debug+0xc8>
    2568:	e59f0078 	ldr	r0, [pc, #120]	; 25e8 <output_debug+0xec>
    256c:	ea000003 	b	2580 <output_debug+0x84>
    2570:	e59d300c 	ldr	r3, [sp, #12]
    2574:	e3530001 	cmp	r3, #1	; 0x1
    2578:	1a000004 	bne	2590 <output_debug+0x94>
    257c:	e59f0068 	ldr	r0, [pc, #104]	; 25ec <output_debug+0xf0>
    2580:	e59f3058 	ldr	r3, [pc, #88]	; 25e0 <output_debug+0xe4>
    2584:	e1a0e00f 	mov	lr, pc
    2588:	e593f000 	ldr	pc, [r3]
    258c:	ea00000c 	b	25c4 <output_debug+0xc8>
    2590:	e3530002 	cmp	r3, #2	; 0x2
    2594:	e59fc044 	ldr	ip, [pc, #68]	; 25e0 <output_debug+0xe4>
    2598:	1a000003 	bne	25ac <output_debug+0xb0>
    259c:	e59f004c 	ldr	r0, [pc, #76]	; 25f0 <output_debug+0xf4>
    25a0:	e1a0e00f 	mov	lr, pc
    25a4:	e59cf000 	ldr	pc, [ip]
    25a8:	ea000005 	b	25c4 <output_debug+0xc8>
    25ac:	e59f3024 	ldr	r3, [pc, #36]	; 25d8 <output_debug+0xdc>
    25b0:	e59f003c 	ldr	r0, [pc, #60]	; 25f4 <output_debug+0xf8>
    25b4:	e5933000 	ldr	r3, [r3]
    25b8:	e28d2010 	add	r2, sp, #16	; 0x10
    25bc:	e1a0e00f 	mov	lr, pc
    25c0:	e59cf000 	ldr	pc, [ip]
    25c4:	e3a00000 	mov	r0, #0	; 0x0
    25c8:	e28dd004 	add	sp, sp, #4	; 0x4
    25cc:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    25d0:	e28dd010 	add	sp, sp, #16	; 0x10
    25d4:	e12fff1e 	bx	lr
    25d8:	3c00d138 	.word	0x3c00d138
    25dc:	000102e2 	.word	0x000102e2
    25e0:	3c00d68c 	.word	0x3c00d68c
    25e4:	0001030a 	.word	0x0001030a
    25e8:	00010319 	.word	0x00010319
    25ec:	00010333 	.word	0x00010333
    25f0:	0001033e 	.word	0x0001033e
    25f4:	0001034c 	.word	0x0001034c

000025f8 <do_vs>:
    25f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    25fc:	e3520006 	cmp	r2, #6	; 0x6
    2600:	e24dd024 	sub	sp, sp, #36	; 0x24
    2604:	e1a06000 	mov	r6, r0
    2608:	e1a04003 	mov	r4, r3
    260c:	1a000005 	bne	2628 <do_vs+0x30>
    2610:	e5930014 	ldr	r0, [r3, #20]
    2614:	e3a01000 	mov	r1, #0	; 0x0
    2618:	e282200a 	add	r2, r2, #10	; 0xa
    261c:	eb0022f1 	bl	b1e8 <simple_strtoul>
    2620:	e1a05000 	mov	r5, r0
    2624:	ea00008f 	b	2868 <do_vs+0x270>
    2628:	e2423001 	sub	r3, r2, #1	; 0x1
    262c:	e3530005 	cmp	r3, #5	; 0x5
    2630:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    2634:	ea0000c4 	b	294c <do_vs+0x354>
    2638:	0000294c 	.word	0x0000294c
    263c:	00002650 	.word	0x00002650
    2640:	00002748 	.word	0x00002748
    2644:	000027b8 	.word	0x000027b8
    2648:	00002864 	.word	0x00002864
    264c:	00002864 	.word	0x00002864
    2650:	e5940004 	ldr	r0, [r4, #4]
    2654:	e59f1310 	ldr	r1, [pc, #784]	; 296c <do_vs+0x374>
    2658:	ebfffa93 	bl	10ac <strcmp>
    265c:	e2505000 	subs	r5, r0, #0	; 0x0
    2660:	1a000025 	bne	26fc <do_vs+0x104>
    2664:	e59f4304 	ldr	r4, [pc, #772]	; 2970 <do_vs+0x378>
    2668:	e59f0304 	ldr	r0, [pc, #772]	; 2974 <do_vs+0x37c>
    266c:	e1a0e00f 	mov	lr, pc
    2670:	e594f000 	ldr	pc, [r4]
    2674:	e3a0802c 	mov	r8, #44	; 0x2c
    2678:	e59f72f8 	ldr	r7, [pc, #760]	; 2978 <do_vs+0x380>
    267c:	ea000003 	b	2690 <do_vs+0x98>
    2680:	e0227598 	mla	r2, r8, r5, r7
    2684:	e1a0e00f 	mov	lr, pc
    2688:	e594f000 	ldr	pc, [r4]
    268c:	e2855001 	add	r5, r5, #1	; 0x1
    2690:	eb00097e 	bl	4c90 <vs_device_count>
    2694:	e1550000 	cmp	r5, r0
    2698:	e1a01005 	mov	r1, r5
    269c:	e59f02d8 	ldr	r0, [pc, #728]	; 297c <do_vs+0x384>
    26a0:	e59f62c8 	ldr	r6, [pc, #712]	; 2970 <do_vs+0x378>
    26a4:	bafffff5 	blt	2680 <do_vs+0x88>
    26a8:	e59f02d0 	ldr	r0, [pc, #720]	; 2980 <do_vs+0x388>
    26ac:	e1a0e00f 	mov	lr, pc
    26b0:	e596f000 	ldr	pc, [r6]
    26b4:	eb00084e 	bl	47f4 <vs_is_assigned>
    26b8:	e2504000 	subs	r4, r0, #0	; 0x0
    26bc:	0a000009 	beq	26e8 <do_vs+0xf0>
    26c0:	e59f32bc 	ldr	r3, [pc, #700]	; 2984 <do_vs+0x38c>
    26c4:	e5965000 	ldr	r5, [r6]
    26c8:	e5934000 	ldr	r4, [r3]
    26cc:	e1a00004 	mov	r0, r4
    26d0:	eb0008d1 	bl	4a1c <vs_align>
    26d4:	e1a01004 	mov	r1, r4
    26d8:	e1a02000 	mov	r2, r0
    26dc:	e59f02a4 	ldr	r0, [pc, #676]	; 2988 <do_vs+0x390>
    26e0:	e12fff35 	blx	r5
    26e4:	ea00009d 	b	2960 <do_vs+0x368>
    26e8:	e59f029c 	ldr	r0, [pc, #668]	; 298c <do_vs+0x394>
    26ec:	e1a0e00f 	mov	lr, pc
    26f0:	e596f000 	ldr	pc, [r6]
    26f4:	e1a00004 	mov	r0, r4
    26f8:	ea000099 	b	2964 <do_vs+0x36c>
    26fc:	e5940004 	ldr	r0, [r4, #4]
    2700:	e59f1288 	ldr	r1, [pc, #648]	; 2990 <do_vs+0x398>
    2704:	ebfffa68 	bl	10ac <strcmp>
    2708:	e2505000 	subs	r5, r0, #0	; 0x0
    270c:	1a000087 	bne	2930 <do_vs+0x338>
    2710:	eb000860 	bl	4898 <vs_reset>
    2714:	e28d4020 	add	r4, sp, #32	; 0x20
    2718:	e5240018 	str	r0, [r4, #-24]!
    271c:	e59f126c 	ldr	r1, [pc, #620]	; 2990 <do_vs+0x398>
    2720:	e2840008 	add	r0, r4, #8	; 0x8
    2724:	ebfffa36 	bl	1004 <strcpy>
    2728:	e3a03001 	mov	r3, #1	; 0x1
    272c:	e58d300c 	str	r3, [sp, #12]
    2730:	e28d3018 	add	r3, sp, #24	; 0x18
    2734:	e8930003 	ldm	r3, {r0, r1}
    2738:	e88d0003 	stm	sp, {r0, r1}
    273c:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2740:	ebffff6d 	bl	24fc <output_debug>
    2744:	ea000019 	b	27b0 <do_vs+0x1b8>
    2748:	e5940004 	ldr	r0, [r4, #4]
    274c:	e59f1240 	ldr	r1, [pc, #576]	; 2994 <do_vs+0x39c>
    2750:	ebfffa55 	bl	10ac <strcmp>
    2754:	e2505000 	subs	r5, r0, #0	; 0x0
    2758:	1a000074 	bne	2930 <do_vs+0x338>
    275c:	e5940008 	ldr	r0, [r4, #8]
    2760:	e3a01001 	mov	r1, #1	; 0x1
    2764:	eb00095f 	bl	4ce8 <vs_assign_by_name>
    2768:	e2507000 	subs	r7, r0, #0	; 0x0
    276c:	e59f61fc 	ldr	r6, [pc, #508]	; 2970 <do_vs+0x378>
    2770:	1a00000b 	bne	27a4 <do_vs+0x1ac>
    2774:	e5943008 	ldr	r3, [r4, #8]
    2778:	e59f2218 	ldr	r2, [pc, #536]	; 2998 <do_vs+0x3a0>
    277c:	e1a00003 	mov	r0, r3
    2780:	e5823000 	str	r3, [r2]
    2784:	eb000943 	bl	4c98 <vs_device_id>
    2788:	e59f31f4 	ldr	r3, [pc, #500]	; 2984 <do_vs+0x38c>
    278c:	e5830000 	str	r0, [r3]
    2790:	e59f0204 	ldr	r0, [pc, #516]	; 299c <do_vs+0x3a4>
    2794:	e1a0e00f 	mov	lr, pc
    2798:	e596f000 	ldr	pc, [r6]
    279c:	e1a00007 	mov	r0, r7
    27a0:	ea00006f 	b	2964 <do_vs+0x36c>
    27a4:	e59f01f4 	ldr	r0, [pc, #500]	; 29a0 <do_vs+0x3a8>
    27a8:	e1a0e00f 	mov	lr, pc
    27ac:	e596f000 	ldr	pc, [r6]
    27b0:	e1a00005 	mov	r0, r5
    27b4:	ea00006a 	b	2964 <do_vs+0x36c>
    27b8:	e3a02010 	mov	r2, #16	; 0x10
    27bc:	e5940008 	ldr	r0, [r4, #8]
    27c0:	e3a01000 	mov	r1, #0	; 0x0
    27c4:	eb0022df 	bl	b348 <simple_strtoull>
    27c8:	e3a02010 	mov	r2, #16	; 0x10
    27cc:	e1a07000 	mov	r7, r0
    27d0:	e1a08001 	mov	r8, r1
    27d4:	e594000c 	ldr	r0, [r4, #12]
    27d8:	e3a01000 	mov	r1, #0	; 0x0
    27dc:	eb0022d9 	bl	b348 <simple_strtoull>
    27e0:	e59f11bc 	ldr	r1, [pc, #444]	; 29a4 <do_vs+0x3ac>
    27e4:	e1a05000 	mov	r5, r0
    27e8:	e28d0010 	add	r0, sp, #16	; 0x10
    27ec:	ebfffa04 	bl	1004 <strcpy>
    27f0:	e3a03002 	mov	r3, #2	; 0x2
    27f4:	e5940004 	ldr	r0, [r4, #4]
    27f8:	e59f11a4 	ldr	r1, [pc, #420]	; 29a4 <do_vs+0x3ac>
    27fc:	e58d300c 	str	r3, [sp, #12]
    2800:	ebfffa29 	bl	10ac <strcmp>
    2804:	e3500000 	cmp	r0, #0	; 0x0
    2808:	1a000004 	bne	2820 <do_vs+0x228>
    280c:	e1a00007 	mov	r0, r7
    2810:	e1a01008 	mov	r1, r8
    2814:	e1a02005 	mov	r2, r5
    2818:	eb000843 	bl	492c <vs_erase>
    281c:	ea000008 	b	2844 <do_vs+0x24c>
    2820:	e5940004 	ldr	r0, [r4, #4]
    2824:	e59f117c 	ldr	r1, [pc, #380]	; 29a8 <do_vs+0x3b0>
    2828:	ebfffa1f 	bl	10ac <strcmp>
    282c:	e3500000 	cmp	r0, #0	; 0x0
    2830:	1a00003e 	bne	2930 <do_vs+0x338>
    2834:	e1a00007 	mov	r0, r7
    2838:	e1a01008 	mov	r1, r8
    283c:	e1a02005 	mov	r2, r5
    2840:	eb000857 	bl	49a4 <vs_scrub>
    2844:	e58d0008 	str	r0, [sp, #8]
    2848:	e28d2018 	add	r2, sp, #24	; 0x18
    284c:	e8920003 	ldm	r2, {r0, r1}
    2850:	e88d0003 	stm	sp, {r0, r1}
    2854:	e28d3008 	add	r3, sp, #8	; 0x8
    2858:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    285c:	ebffff26 	bl	24fc <output_debug>
    2860:	ea00003e 	b	2960 <do_vs+0x368>
    2864:	e3a05000 	mov	r5, #0	; 0x0
    2868:	e3a01000 	mov	r1, #0	; 0x0
    286c:	e3a02010 	mov	r2, #16	; 0x10
    2870:	e5940008 	ldr	r0, [r4, #8]
    2874:	eb00225b 	bl	b1e8 <simple_strtoul>
    2878:	e3a02010 	mov	r2, #16	; 0x10
    287c:	e1a09000 	mov	r9, r0
    2880:	e3a01000 	mov	r1, #0	; 0x0
    2884:	e594000c 	ldr	r0, [r4, #12]
    2888:	eb0022ae 	bl	b348 <simple_strtoull>
    288c:	e3a02010 	mov	r2, #16	; 0x10
    2890:	e1a08000 	mov	r8, r0
    2894:	e1a0a001 	mov	sl, r1
    2898:	e5940010 	ldr	r0, [r4, #16]
    289c:	e3a01000 	mov	r1, #0	; 0x0
    28a0:	e08db002 	add	fp, sp, r2
    28a4:	eb0022a7 	bl	b348 <simple_strtoull>
    28a8:	e3a03000 	mov	r3, #0	; 0x0
    28ac:	e59f10f8 	ldr	r1, [pc, #248]	; 29ac <do_vs+0x3b4>
    28b0:	e1a07000 	mov	r7, r0
    28b4:	e1a0000b 	mov	r0, fp
    28b8:	e58d300c 	str	r3, [sp, #12]
    28bc:	ebfff9d0 	bl	1004 <strcpy>
    28c0:	e5940004 	ldr	r0, [r4, #4]
    28c4:	e59f10e4 	ldr	r1, [pc, #228]	; 29b0 <do_vs+0x3b8>
    28c8:	ebfff9f7 	bl	10ac <strcmp>
    28cc:	e3500000 	cmp	r0, #0	; 0x0
    28d0:	1a000006 	bne	28f0 <do_vs+0x2f8>
    28d4:	e1a00009 	mov	r0, r9
    28d8:	e1a02008 	mov	r2, r8
    28dc:	e1a0300a 	mov	r3, sl
    28e0:	e58d7000 	str	r7, [sp]
    28e4:	e58d5004 	str	r5, [sp, #4]
    28e8:	eb0007f9 	bl	48d4 <vs_write>
    28ec:	eaffffd4 	b	2844 <do_vs+0x24c>
    28f0:	e5940004 	ldr	r0, [r4, #4]
    28f4:	e59f10b8 	ldr	r1, [pc, #184]	; 29b4 <do_vs+0x3bc>
    28f8:	ebfff9eb 	bl	10ac <strcmp>
    28fc:	e3500000 	cmp	r0, #0	; 0x0
    2900:	1a00000a 	bne	2930 <do_vs+0x338>
    2904:	e1a00009 	mov	r0, r9
    2908:	e1a02008 	mov	r2, r8
    290c:	e1a0300a 	mov	r3, sl
    2910:	e58d7000 	str	r7, [sp]
    2914:	e58d5004 	str	r5, [sp, #4]
    2918:	eb000933 	bl	4dec <vs_read>
    291c:	e59f1094 	ldr	r1, [pc, #148]	; 29b8 <do_vs+0x3c0>
    2920:	e58d0008 	str	r0, [sp, #8]
    2924:	e1a0000b 	mov	r0, fp
    2928:	ebfff9b5 	bl	1004 <strcpy>
    292c:	eaffffc5 	b	2848 <do_vs+0x250>
    2930:	e59f0084 	ldr	r0, [pc, #132]	; 29bc <do_vs+0x3c4>
    2934:	e5961014 	ldr	r1, [r6, #20]
    2938:	e59f3030 	ldr	r3, [pc, #48]	; 2970 <do_vs+0x378>
    293c:	e1a0e00f 	mov	lr, pc
    2940:	e593f000 	ldr	pc, [r3]
    2944:	e3e00000 	mvn	r0, #0	; 0x0
    2948:	ea000005 	b	2964 <do_vs+0x36c>
    294c:	e5901014 	ldr	r1, [r0, #20]
    2950:	e59f3018 	ldr	r3, [pc, #24]	; 2970 <do_vs+0x378>
    2954:	e59f0060 	ldr	r0, [pc, #96]	; 29bc <do_vs+0x3c4>
    2958:	e1a0e00f 	mov	lr, pc
    295c:	e593f000 	ldr	pc, [r3]
    2960:	e3a00000 	mov	r0, #0	; 0x0
    2964:	e28dd024 	add	sp, sp, #36	; 0x24
    2968:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    296c:	00010186 	.word	0x00010186
    2970:	3c00d68c 	.word	0x3c00d68c
    2974:	00010363 	.word	0x00010363
    2978:	0000efdc 	.word	0x0000efdc
    297c:	00010371 	.word	0x00010371
    2980:	00010379 	.word	0x00010379
    2984:	3c00d138 	.word	0x3c00d138
    2988:	0001038b 	.word	0x0001038b
    298c:	00010395 	.word	0x00010395
    2990:	00010174 	.word	0x00010174
    2994:	0001166e 	.word	0x0001166e
    2998:	3c00d13c 	.word	0x3c00d13c
    299c:	000103a8 	.word	0x000103a8
    29a0:	000103b4 	.word	0x000103b4
    29a4:	000103e1 	.word	0x000103e1
    29a8:	000103e7 	.word	0x000103e7
    29ac:	000103ed 	.word	0x000103ed
    29b0:	000103f6 	.word	0x000103f6
    29b4:	000103fc 	.word	0x000103fc
    29b8:	00010401 	.word	0x00010401
    29bc:	0000fd94 	.word	0x0000fd94

000029c0 <cmd_get_data_size>:
    29c0:	e92d4030 	push	{r4, r5, lr}
    29c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    29c8:	e1a04001 	mov	r4, r1
    29cc:	e1a05000 	mov	r5, r0
    29d0:	ebfff9e2 	bl	1160 <strlen>
    29d4:	e3500002 	cmp	r0, #2	; 0x2
    29d8:	da000015 	ble	2a34 <cmd_get_data_size+0x74>
    29dc:	e0850000 	add	r0, r5, r0
    29e0:	e5503002 	ldrb	r3, [r0, #-2]
    29e4:	e353002e 	cmp	r3, #46	; 0x2e
    29e8:	1a000011 	bne	2a34 <cmd_get_data_size+0x74>
    29ec:	e5500001 	ldrb	r0, [r0, #-1]
    29f0:	e350006c 	cmp	r0, #108	; 0x6c
    29f4:	03a04004 	moveq	r4, #4	; 0x4
    29f8:	0a00000d 	beq	2a34 <cmd_get_data_size+0x74>
    29fc:	8a000002 	bhi	2a0c <cmd_get_data_size+0x4c>
    2a00:	e3500062 	cmp	r0, #98	; 0x62
    2a04:	1a000009 	bne	2a30 <cmd_get_data_size+0x70>
    2a08:	ea000006 	b	2a28 <cmd_get_data_size+0x68>
    2a0c:	e3500073 	cmp	r0, #115	; 0x73
    2a10:	03e04001 	mvneq	r4, #1	; 0x1
    2a14:	0a000006 	beq	2a34 <cmd_get_data_size+0x74>
    2a18:	e3500077 	cmp	r0, #119	; 0x77
    2a1c:	03a04002 	moveq	r4, #2	; 0x2
    2a20:	1a000002 	bne	2a30 <cmd_get_data_size+0x70>
    2a24:	ea000002 	b	2a34 <cmd_get_data_size+0x74>
    2a28:	e3a04001 	mov	r4, #1	; 0x1
    2a2c:	ea000000 	b	2a34 <cmd_get_data_size+0x74>
    2a30:	e3e04000 	mvn	r4, #0	; 0x0
    2a34:	e1a00004 	mov	r0, r4
    2a38:	e28dd004 	add	sp, sp, #4	; 0x4
    2a3c:	e8bd8030 	pop	{r4, r5, pc}

00002a40 <cmd_usage>:
    2a40:	e92d4030 	push	{r4, r5, lr}
    2a44:	e1a04000 	mov	r4, r0
    2a48:	e24dd004 	sub	sp, sp, #4	; 0x4
    2a4c:	e59f5054 	ldr	r5, [pc, #84]	; 2aa8 <cmd_usage+0x68>
    2a50:	e59f0054 	ldr	r0, [pc, #84]	; 2aac <cmd_usage+0x6c>
    2a54:	e5941000 	ldr	r1, [r4]
    2a58:	e5942010 	ldr	r2, [r4, #16]
    2a5c:	e1a0e00f 	mov	lr, pc
    2a60:	e595f000 	ldr	pc, [r5]
    2a64:	e59f0044 	ldr	r0, [pc, #68]	; 2ab0 <cmd_usage+0x70>
    2a68:	e5941000 	ldr	r1, [r4]
    2a6c:	e1a0e00f 	mov	lr, pc
    2a70:	e595f000 	ldr	pc, [r5]
    2a74:	e5940014 	ldr	r0, [r4, #20]
    2a78:	e3500000 	cmp	r0, #0	; 0x0
    2a7c:	1a000003 	bne	2a90 <cmd_usage+0x50>
    2a80:	e59f002c 	ldr	r0, [pc, #44]	; 2ab4 <cmd_usage+0x74>
    2a84:	eb0025c5 	bl	c1a0 <puts>
    2a88:	e3a00001 	mov	r0, #1	; 0x1
    2a8c:	ea000003 	b	2aa0 <cmd_usage+0x60>
    2a90:	eb0025c2 	bl	c1a0 <puts>
    2a94:	e3a0000a 	mov	r0, #10	; 0xa
    2a98:	eb0025e8 	bl	c240 <putc>
    2a9c:	e3a00000 	mov	r0, #0	; 0x0
    2aa0:	e28dd004 	add	sp, sp, #4	; 0x4
    2aa4:	e8bd8030 	pop	{r4, r5, pc}
    2aa8:	3c00d68c 	.word	0x3c00d68c
    2aac:	000104f6 	.word	0x000104f6
    2ab0:	00010500 	.word	0x00010500
    2ab4:	0001050b 	.word	0x0001050b

00002ab8 <find_cmd_tbl>:
    2ab8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2abc:	e1a05001 	mov	r5, r1
    2ac0:	e24dd004 	sub	sp, sp, #4	; 0x4
    2ac4:	e3a0102e 	mov	r1, #46	; 0x2e
    2ac8:	e1a07000 	mov	r7, r0
    2acc:	e1a08002 	mov	r8, r2
    2ad0:	ebfff997 	bl	1134 <strchr>
    2ad4:	e3500000 	cmp	r0, #0	; 0x0
    2ad8:	10676000 	rsbne	r6, r7, r0
    2adc:	1a000002 	bne	2aec <find_cmd_tbl+0x34>
    2ae0:	e1a00007 	mov	r0, r7
    2ae4:	ebfff99d 	bl	1160 <strlen>
    2ae8:	e1a06000 	mov	r6, r0
    2aec:	e3a03018 	mov	r3, #24	; 0x18
    2af0:	e02a5893 	mla	sl, r3, r8, r5
    2af4:	e1a04005 	mov	r4, r5
    2af8:	e1a08005 	mov	r8, r5
    2afc:	e3a05000 	mov	r5, #0	; 0x0
    2b00:	ea00000a 	b	2b30 <find_cmd_tbl+0x78>
    2b04:	e5941000 	ldr	r1, [r4]
    2b08:	ebfff975 	bl	10e4 <strncmp>
    2b0c:	e3500000 	cmp	r0, #0	; 0x0
    2b10:	1a000005 	bne	2b2c <find_cmd_tbl+0x74>
    2b14:	e5940000 	ldr	r0, [r4]
    2b18:	ebfff990 	bl	1160 <strlen>
    2b1c:	e1560000 	cmp	r6, r0
    2b20:	0a000009 	beq	2b4c <find_cmd_tbl+0x94>
    2b24:	e2855001 	add	r5, r5, #1	; 0x1
    2b28:	e1a08004 	mov	r8, r4
    2b2c:	e2844018 	add	r4, r4, #24	; 0x18
    2b30:	e154000a 	cmp	r4, sl
    2b34:	e1a02006 	mov	r2, r6
    2b38:	e1a00007 	mov	r0, r7
    2b3c:	1afffff0 	bne	2b04 <find_cmd_tbl+0x4c>
    2b40:	e3550001 	cmp	r5, #1	; 0x1
    2b44:	01a04008 	moveq	r4, r8
    2b48:	13a04000 	movne	r4, #0	; 0x0
    2b4c:	e1a00004 	mov	r0, r4
    2b50:	e28dd004 	add	sp, sp, #4	; 0x4
    2b54:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002b58 <find_cmd>:
    2b58:	e59f1014 	ldr	r1, [pc, #20]	; 2b74 <find_cmd+0x1c>
    2b5c:	e59f2014 	ldr	r2, [pc, #20]	; 2b78 <find_cmd+0x20>
    2b60:	e59f3014 	ldr	r3, [pc, #20]	; 2b7c <find_cmd+0x24>
    2b64:	e0612002 	rsb	r2, r1, r2
    2b68:	e1a021c2 	asr	r2, r2, #3
    2b6c:	e0020293 	mul	r2, r3, r2
    2b70:	eaffffd0 	b	2ab8 <find_cmd_tbl>
    2b74:	000119dc 	.word	0x000119dc
    2b78:	00011cac 	.word	0x00011cac
    2b7c:	aaaaaaab 	.word	0xaaaaaaab

00002b80 <_do_help>:
    2b80:	e1a0c00d 	mov	ip, sp
    2b84:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2b88:	e24cb004 	sub	fp, ip, #4	; 0x4
    2b8c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2b90:	e59b8004 	ldr	r8, [fp, #4]
    2b94:	e1a07000 	mov	r7, r0
    2b98:	e3580001 	cmp	r8, #1	; 0x1
    2b9c:	e1a0a001 	mov	sl, r1
    2ba0:	159b5008 	ldrne	r5, [fp, #8]
    2ba4:	13a06001 	movne	r6, #1	; 0x1
    2ba8:	13a04000 	movne	r4, #0	; 0x0
    2bac:	159f9158 	ldrne	r9, [pc, #344]	; 2d0c <_do_help+0x18c>
    2bb0:	1a00004e 	bne	2cf0 <_do_help+0x170>
    2bb4:	e1a03101 	lsl	r3, r1, #2
    2bb8:	e283300e 	add	r3, r3, #14	; 0xe
    2bbc:	e3c33007 	bic	r3, r3, #7	; 0x7
    2bc0:	e1a0900d 	mov	r9, sp
    2bc4:	e063d00d 	rsb	sp, r3, sp
    2bc8:	e1a0800d 	mov	r8, sp
    2bcc:	e3a03000 	mov	r3, #0	; 0x0
    2bd0:	ea000002 	b	2be0 <_do_help+0x60>
    2bd4:	e7880103 	str	r0, [r8, r3, lsl #2]
    2bd8:	e2800018 	add	r0, r0, #24	; 0x18
    2bdc:	e2833001 	add	r3, r3, #1	; 0x1
    2be0:	e153000a 	cmp	r3, sl
    2be4:	bafffffa 	blt	2bd4 <_do_help+0x54>
    2be8:	e24a7001 	sub	r7, sl, #1	; 0x1
    2bec:	ea000015 	b	2c48 <_do_help+0xc8>
    2bf0:	e5943000 	ldr	r3, [r4]
    2bf4:	e5942004 	ldr	r2, [r4, #4]
    2bf8:	e5930000 	ldr	r0, [r3]
    2bfc:	e5921000 	ldr	r1, [r2]
    2c00:	ebfff929 	bl	10ac <strcmp>
    2c04:	e3500000 	cmp	r0, #0	; 0x0
    2c08:	c5942000 	ldrgt	r2, [r4]
    2c0c:	c5943004 	ldrgt	r3, [r4, #4]
    2c10:	e2855001 	add	r5, r5, #1	; 0x1
    2c14:	c5843000 	strgt	r3, [r4]
    2c18:	c5842004 	strgt	r2, [r4, #4]
    2c1c:	c2866001 	addgt	r6, r6, #1	; 0x1
    2c20:	e1550007 	cmp	r5, r7
    2c24:	e2844004 	add	r4, r4, #4	; 0x4
    2c28:	bafffff0 	blt	2bf0 <_do_help+0x70>
    2c2c:	e3560000 	cmp	r6, #0	; 0x0
    2c30:	1a000003 	bne	2c44 <_do_help+0xc4>
    2c34:	e3a05000 	mov	r5, #0	; 0x0
    2c38:	e1a04005 	mov	r4, r5
    2c3c:	e59f70c8 	ldr	r7, [pc, #200]	; 2d0c <_do_help+0x18c>
    2c40:	ea000016 	b	2ca0 <_do_help+0x120>
    2c44:	e2477001 	sub	r7, r7, #1	; 0x1
    2c48:	e3570000 	cmp	r7, #0	; 0x0
    2c4c:	dafffff8 	ble	2c34 <_do_help+0xb4>
    2c50:	e3a05000 	mov	r5, #0	; 0x0
    2c54:	e1a04008 	mov	r4, r8
    2c58:	e1a06005 	mov	r6, r5
    2c5c:	eaffffe3 	b	2bf0 <_do_help+0x70>
    2c60:	e7983004 	ldr	r3, [r8, r4]
    2c64:	e5936010 	ldr	r6, [r3, #16]
    2c68:	eb002577 	bl	c24c <ctrlc>
    2c6c:	e3500000 	cmp	r0, #0	; 0x0
    2c70:	e1a03006 	mov	r3, r6
    2c74:	e3a01008 	mov	r1, #8	; 0x8
    2c78:	e59f0090 	ldr	r0, [pc, #144]	; 2d10 <_do_help+0x190>
    2c7c:	13a04001 	movne	r4, #1	; 0x1
    2c80:	1a00000a 	bne	2cb0 <_do_help+0x130>
    2c84:	e3560000 	cmp	r6, #0	; 0x0
    2c88:	0a000003 	beq	2c9c <_do_help+0x11c>
    2c8c:	e7982004 	ldr	r2, [r8, r4]
    2c90:	e5922000 	ldr	r2, [r2]
    2c94:	e1a0e00f 	mov	lr, pc
    2c98:	e597f000 	ldr	pc, [r7]
    2c9c:	e2844004 	add	r4, r4, #4	; 0x4
    2ca0:	e155000a 	cmp	r5, sl
    2ca4:	e2855001 	add	r5, r5, #1	; 0x1
    2ca8:	baffffec 	blt	2c60 <_do_help+0xe0>
    2cac:	e3a04000 	mov	r4, #0	; 0x0
    2cb0:	e1a0d009 	mov	sp, r9
    2cb4:	ea000011 	b	2d00 <_do_help+0x180>
    2cb8:	e5950004 	ldr	r0, [r5, #4]
    2cbc:	ebffff7d 	bl	2ab8 <find_cmd_tbl>
    2cc0:	e3500000 	cmp	r0, #0	; 0x0
    2cc4:	0a000002 	beq	2cd4 <_do_help+0x154>
    2cc8:	ebffff5c 	bl	2a40 <cmd_usage>
    2ccc:	e1844000 	orr	r4, r4, r0
    2cd0:	ea000004 	b	2ce8 <_do_help+0x168>
    2cd4:	e5951004 	ldr	r1, [r5, #4]
    2cd8:	e59f0034 	ldr	r0, [pc, #52]	; 2d14 <_do_help+0x194>
    2cdc:	e1a0e00f 	mov	lr, pc
    2ce0:	e599f000 	ldr	pc, [r9]
    2ce4:	e3a04001 	mov	r4, #1	; 0x1
    2ce8:	e2866001 	add	r6, r6, #1	; 0x1
    2cec:	e2855004 	add	r5, r5, #4	; 0x4
    2cf0:	e1560008 	cmp	r6, r8
    2cf4:	e1a01007 	mov	r1, r7
    2cf8:	e1a0200a 	mov	r2, sl
    2cfc:	baffffed 	blt	2cb8 <_do_help+0x138>
    2d00:	e1a00004 	mov	r0, r4
    2d04:	e24bd028 	sub	sp, fp, #40	; 0x28
    2d08:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    2d0c:	3c00d68c 	.word	0x3c00d68c
    2d10:	0001052c 	.word	0x0001052c
    2d14:	00010536 	.word	0x00010536

00002d18 <do_help>:
    2d18:	e92d4030 	push	{r4, r5, lr}
    2d1c:	e59fc038 	ldr	ip, [pc, #56]	; 2d5c <do_help+0x44>
    2d20:	e59fe038 	ldr	lr, [pc, #56]	; 2d60 <do_help+0x48>
    2d24:	e1a05001 	mov	r5, r1
    2d28:	e06ec00c 	rsb	ip, lr, ip
    2d2c:	e59f1030 	ldr	r1, [pc, #48]	; 2d64 <do_help+0x4c>
    2d30:	e24dd00c 	sub	sp, sp, #12	; 0xc
    2d34:	e1a04000 	mov	r4, r0
    2d38:	e1a0c1cc 	asr	ip, ip, #3
    2d3c:	e88d000c 	stm	sp, {r2, r3}
    2d40:	e0010c91 	mul	r1, r1, ip
    2d44:	e1a0000e 	mov	r0, lr
    2d48:	e1a02004 	mov	r2, r4
    2d4c:	e1a03005 	mov	r3, r5
    2d50:	ebffff8a 	bl	2b80 <_do_help>
    2d54:	e28dd00c 	add	sp, sp, #12	; 0xc
    2d58:	e8bd8030 	pop	{r4, r5, pc}
    2d5c:	00011cac 	.word	0x00011cac
    2d60:	000119dc 	.word	0x000119dc
    2d64:	aaaaaaab 	.word	0xaaaaaaab

00002d68 <tt_blk_trolling>:
    2d68:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2d6c:	e3a01001 	mov	r1, #1	; 0x1
    2d70:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    2d74:	e1a04002 	mov	r4, r2
    2d78:	e1a05003 	mov	r5, r3
    2d7c:	e58d0010 	str	r0, [sp, #16]
    2d80:	e59da050 	ldr	sl, [sp, #80]
    2d84:	eb0007d7 	bl	4ce8 <vs_assign_by_name>
    2d88:	e3500000 	cmp	r0, #0	; 0x0
    2d8c:	0a000005 	beq	2da8 <tt_blk_trolling+0x40>
    2d90:	e59d1010 	ldr	r1, [sp, #16]
    2d94:	e59f03d4 	ldr	r0, [pc, #980]	; 3170 <tt_blk_trolling+0x408>
    2d98:	e59f33d4 	ldr	r3, [pc, #980]	; 3174 <tt_blk_trolling+0x40c>
    2d9c:	e1a0e00f 	mov	lr, pc
    2da0:	e593f000 	ldr	pc, [r3]
    2da4:	ea000026 	b	2e44 <tt_blk_trolling+0xdc>
    2da8:	e1940005 	orrs	r0, r4, r5
    2dac:	03a04502 	moveq	r4, #8388608	; 0x800000
    2db0:	03a06502 	moveq	r6, #8388608	; 0x800000
    2db4:	03a07000 	moveq	r7, #0	; 0x0
    2db8:	0a00000b 	beq	2dec <tt_blk_trolling+0x84>
    2dbc:	e3550000 	cmp	r5, #0	; 0x0
    2dc0:	1a000006 	bne	2de0 <tt_blk_trolling+0x78>
    2dc4:	e3540502 	cmp	r4, #8388608	; 0x800000
    2dc8:	2a000004 	bcs	2de0 <tt_blk_trolling+0x78>
    2dcc:	e3a0000e 	mov	r0, #14	; 0xe
    2dd0:	eb000711 	bl	4a1c <vs_align>
    2dd4:	e1a06004 	mov	r6, r4
    2dd8:	e1a07005 	mov	r7, r5
    2ddc:	ea000002 	b	2dec <tt_blk_trolling+0x84>
    2de0:	e1a06004 	mov	r6, r4
    2de4:	e1a07005 	mov	r7, r5
    2de8:	e3a04502 	mov	r4, #8388608	; 0x800000
    2dec:	e3a0000e 	mov	r0, #14	; 0xe
    2df0:	eb000709 	bl	4a1c <vs_align>
    2df4:	e1a01000 	mov	r1, r0
    2df8:	e1a00004 	mov	r0, r4
    2dfc:	eb002cfd 	bl	e1f8 <__aeabi_uidiv>
    2e00:	e1a01000 	mov	r1, r0
    2e04:	e3a00000 	mov	r0, #0	; 0x0
    2e08:	eb000514 	bl	4260 <tt_rnd_get_int>
    2e0c:	e1a04000 	mov	r4, r0
    2e10:	e3a0000e 	mov	r0, #14	; 0xe
    2e14:	eb000700 	bl	4a1c <vs_align>
    2e18:	e0040094 	mul	r4, r4, r0
    2e1c:	e28d0024 	add	r0, sp, #36	; 0x24
    2e20:	e1a01004 	mov	r1, r4
    2e24:	e1a02004 	mov	r2, r4
    2e28:	eb000535 	bl	4304 <tt_rnd_get_data>
    2e2c:	e250b000 	subs	fp, r0, #0	; 0x0
    2e30:	1a000005 	bne	2e4c <tt_blk_trolling+0xe4>
    2e34:	e59f033c 	ldr	r0, [pc, #828]	; 3178 <tt_blk_trolling+0x410>
    2e38:	e59f3334 	ldr	r3, [pc, #820]	; 3174 <tt_blk_trolling+0x40c>
    2e3c:	e1a0e00f 	mov	lr, pc
    2e40:	e593f000 	ldr	pc, [r3]
    2e44:	e3e00000 	mvn	r0, #0	; 0x0
    2e48:	ea0000c6 	b	3168 <tt_blk_trolling+0x400>
    2e4c:	e59d3024 	ldr	r3, [sp, #36]
    2e50:	e3a0000e 	mov	r0, #14	; 0xe
    2e54:	e0564003 	subs	r4, r6, r3
    2e58:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    2e5c:	eb0006ee 	bl	4a1c <vs_align>
    2e60:	e3550000 	cmp	r5, #0	; 0x0
    2e64:	e1cd41f8 	strd	r4, [sp, #24]
    2e68:	1a000005 	bne	2e84 <tt_blk_trolling+0x11c>
    2e6c:	e1a01000 	mov	r1, r0
    2e70:	e1a00004 	mov	r0, r4
    2e74:	eb002cdf 	bl	e1f8 <__aeabi_uidiv>
    2e78:	e58d501c 	str	r5, [sp, #28]
    2e7c:	e58d0018 	str	r0, [sp, #24]
    2e80:	ea000002 	b	2e90 <tt_blk_trolling+0x128>
    2e84:	e1a01000 	mov	r1, r0
    2e88:	e28d0018 	add	r0, sp, #24	; 0x18
    2e8c:	eb002098 	bl	b0f4 <__div64_32>
    2e90:	e59d1018 	ldr	r1, [sp, #24]
    2e94:	e3a00000 	mov	r0, #0	; 0x0
    2e98:	eb0004f0 	bl	4260 <tt_rnd_get_int>
    2e9c:	e1a04000 	mov	r4, r0
    2ea0:	e3a0000e 	mov	r0, #14	; 0xe
    2ea4:	eb0006dc 	bl	4a1c <vs_align>
    2ea8:	e0c98490 	smull	r8, r9, r0, r4
    2eac:	e59d1010 	ldr	r1, [sp, #16]
    2eb0:	e1a0200b 	mov	r2, fp
    2eb4:	e59d3024 	ldr	r3, [sp, #36]
    2eb8:	e59f02bc 	ldr	r0, [pc, #700]	; 317c <tt_blk_trolling+0x414>
    2ebc:	e88d0300 	stm	sp, {r8, r9}
    2ec0:	e59f42ac 	ldr	r4, [pc, #684]	; 3174 <tt_blk_trolling+0x40c>
    2ec4:	e1a0e00f 	mov	lr, pc
    2ec8:	e594f000 	ldr	pc, [r4]
    2ecc:	e59d0010 	ldr	r0, [sp, #16]
    2ed0:	eb000770 	bl	4c98 <vs_device_id>
    2ed4:	eb00075b 	bl	4c48 <vs_device_erasable>
    2ed8:	e3500000 	cmp	r0, #0	; 0x0
    2edc:	0a000006 	beq	2efc <tt_blk_trolling+0x194>
    2ee0:	e59f0298 	ldr	r0, [pc, #664]	; 3180 <tt_blk_trolling+0x418>
    2ee4:	e1a0e00f 	mov	lr, pc
    2ee8:	e594f000 	ldr	pc, [r4]
    2eec:	e1a00008 	mov	r0, r8
    2ef0:	e1a01009 	mov	r1, r9
    2ef4:	e59d2024 	ldr	r2, [sp, #36]
    2ef8:	eb00068b 	bl	492c <vs_erase>
    2efc:	e59f0280 	ldr	r0, [pc, #640]	; 3184 <tt_blk_trolling+0x41c>
    2f00:	e59f326c 	ldr	r3, [pc, #620]	; 3174 <tt_blk_trolling+0x40c>
    2f04:	e1a0e00f 	mov	lr, pc
    2f08:	e593f000 	ldr	pc, [r3]
    2f0c:	e35a0000 	cmp	sl, #0	; 0x0
    2f10:	13a07000 	movne	r7, #0	; 0x0
    2f14:	159d6024 	ldrne	r6, [sp, #36]
    2f18:	1a00001c 	bne	2f90 <tt_blk_trolling+0x228>
    2f1c:	ea00001f 	b	2fa0 <tt_blk_trolling+0x238>
    2f20:	e3560902 	cmp	r6, #32768	; 0x8000
    2f24:	b1a01006 	movlt	r1, r6
    2f28:	a3a01902 	movge	r1, #32768	; 0x8000
    2f2c:	e58d100c 	str	r1, [sp, #12]
    2f30:	e59d1024 	ldr	r1, [sp, #36]
    2f34:	e59d200c 	ldr	r2, [sp, #12]
    2f38:	e0661001 	rsb	r1, r6, r1
    2f3c:	e08b1001 	add	r1, fp, r1
    2f40:	ebfff8a0 	bl	11c8 <memcpy>
    2f44:	e3a00000 	mov	r0, #0	; 0x0
    2f48:	eb00160e 	bl	8788 <get_timer>
    2f4c:	e59d3024 	ldr	r3, [sp, #36]
    2f50:	e3a01000 	mov	r1, #0	; 0x0
    2f54:	e0663003 	rsb	r3, r6, r3
    2f58:	e0984003 	adds	r4, r8, r3
    2f5c:	e0a95fc3 	adc	r5, r9, r3, asr #31
    2f60:	e1a03005 	mov	r3, r5
    2f64:	e59d500c 	ldr	r5, [sp, #12]
    2f68:	e58d0008 	str	r0, [sp, #8]
    2f6c:	e1a02004 	mov	r2, r4
    2f70:	e1a0000a 	mov	r0, sl
    2f74:	e58d1004 	str	r1, [sp, #4]
    2f78:	e58d5000 	str	r5, [sp]
    2f7c:	eb000654 	bl	48d4 <vs_write>
    2f80:	e59d0008 	ldr	r0, [sp, #8]
    2f84:	eb0015ff 	bl	8788 <get_timer>
    2f88:	e0656006 	rsb	r6, r5, r6
    2f8c:	e0877000 	add	r7, r7, r0
    2f90:	e3560000 	cmp	r6, #0	; 0x0
    2f94:	e1a0000a 	mov	r0, sl
    2f98:	1affffe0 	bne	2f20 <tt_blk_trolling+0x1b8>
    2f9c:	ea00000b 	b	2fd0 <tt_blk_trolling+0x268>
    2fa0:	e1a0000a 	mov	r0, sl
    2fa4:	eb0015f7 	bl	8788 <get_timer>
    2fa8:	e59d1024 	ldr	r1, [sp, #36]
    2fac:	e1a04000 	mov	r4, r0
    2fb0:	e1a02008 	mov	r2, r8
    2fb4:	e1a03009 	mov	r3, r9
    2fb8:	e1a0000b 	mov	r0, fp
    2fbc:	e88d0402 	stm	sp, {r1, sl}
    2fc0:	eb000643 	bl	48d4 <vs_write>
    2fc4:	e1a00004 	mov	r0, r4
    2fc8:	eb0015ee 	bl	8788 <get_timer>
    2fcc:	e1a07000 	mov	r7, r0
    2fd0:	e59d0024 	ldr	r0, [sp, #36]
    2fd4:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    2fd8:	e1a00540 	asr	r0, r0, #10
    2fdc:	e1a01007 	mov	r1, r7
    2fe0:	e0000093 	mul	r0, r3, r0
    2fe4:	eb002c83 	bl	e1f8 <__aeabi_uidiv>
    2fe8:	e59f3184 	ldr	r3, [pc, #388]	; 3174 <tt_blk_trolling+0x40c>
    2fec:	e58d0014 	str	r0, [sp, #20]
    2ff0:	e59f0190 	ldr	r0, [pc, #400]	; 3188 <tt_blk_trolling+0x420>
    2ff4:	e1a0e00f 	mov	lr, pc
    2ff8:	e593f000 	ldr	pc, [r3]
    2ffc:	e35a0000 	cmp	sl, #0	; 0x0
    3000:	13a07000 	movne	r7, #0	; 0x0
    3004:	159d6024 	ldrne	r6, [sp, #36]
    3008:	1a00001b 	bne	307c <tt_blk_trolling+0x314>
    300c:	ea00001e 	b	308c <tt_blk_trolling+0x324>
    3010:	eb0015dc 	bl	8788 <get_timer>
    3014:	e59d3024 	ldr	r3, [sp, #36]
    3018:	e3560902 	cmp	r6, #32768	; 0x8000
    301c:	b1a04006 	movlt	r4, r6
    3020:	a3a04902 	movge	r4, #32768	; 0x8000
    3024:	e0663003 	rsb	r3, r6, r3
    3028:	e1a05000 	mov	r5, r0
    302c:	e0980003 	adds	r0, r8, r3
    3030:	e0a91fc3 	adc	r1, r9, r3, asr #31
    3034:	e1a02000 	mov	r2, r0
    3038:	e1a03001 	mov	r3, r1
    303c:	e1a0000a 	mov	r0, sl
    3040:	e3a01000 	mov	r1, #0	; 0x0
    3044:	e58d1004 	str	r1, [sp, #4]
    3048:	e58d4000 	str	r4, [sp]
    304c:	eb000766 	bl	4dec <vs_read>
    3050:	e1a00005 	mov	r0, r5
    3054:	eb0015cb 	bl	8788 <get_timer>
    3058:	e1a05000 	mov	r5, r0
    305c:	e59d0024 	ldr	r0, [sp, #36]
    3060:	e1a0100a 	mov	r1, sl
    3064:	e0660000 	rsb	r0, r6, r0
    3068:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    306c:	e1a02004 	mov	r2, r4
    3070:	ebfff854 	bl	11c8 <memcpy>
    3074:	e0877005 	add	r7, r7, r5
    3078:	e0646006 	rsb	r6, r4, r6
    307c:	e3a00000 	mov	r0, #0	; 0x0
    3080:	e1560000 	cmp	r6, r0
    3084:	1affffe1 	bne	3010 <tt_blk_trolling+0x2a8>
    3088:	ea00000b 	b	30bc <tt_blk_trolling+0x354>
    308c:	e1a0000a 	mov	r0, sl
    3090:	eb0015bc 	bl	8788 <get_timer>
    3094:	e59d1024 	ldr	r1, [sp, #36]
    3098:	e1a04000 	mov	r4, r0
    309c:	e1a02008 	mov	r2, r8
    30a0:	e1a03009 	mov	r3, r9
    30a4:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    30a8:	e88d0402 	stm	sp, {r1, sl}
    30ac:	eb00074e 	bl	4dec <vs_read>
    30b0:	e1a00004 	mov	r0, r4
    30b4:	eb0015b3 	bl	8788 <get_timer>
    30b8:	e1a07000 	mov	r7, r0
    30bc:	e59d0024 	ldr	r0, [sp, #36]
    30c0:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    30c4:	e1a00540 	asr	r0, r0, #10
    30c8:	e0000093 	mul	r0, r3, r0
    30cc:	e1a01007 	mov	r1, r7
    30d0:	eb002c48 	bl	e1f8 <__aeabi_uidiv>
    30d4:	e59f4098 	ldr	r4, [pc, #152]	; 3174 <tt_blk_trolling+0x40c>
    30d8:	e1a05000 	mov	r5, r0
    30dc:	e59f00a8 	ldr	r0, [pc, #168]	; 318c <tt_blk_trolling+0x424>
    30e0:	e1a0e00f 	mov	lr, pc
    30e4:	e594f000 	ldr	pc, [r4]
    30e8:	e1a0000b 	mov	r0, fp
    30ec:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    30f0:	e59d2024 	ldr	r2, [sp, #36]
    30f4:	e3a0300c 	mov	r3, #12	; 0xc
    30f8:	eb000404 	bl	4110 <tt_cmp_buffer>
    30fc:	e3500000 	cmp	r0, #0	; 0x0
    3100:	0a000007 	beq	3124 <tt_blk_trolling+0x3bc>
    3104:	e58d0000 	str	r0, [sp]
    3108:	e59d1010 	ldr	r1, [sp, #16]
    310c:	e59d2014 	ldr	r2, [sp, #20]
    3110:	e1a03005 	mov	r3, r5
    3114:	e59f0074 	ldr	r0, [pc, #116]	; 3190 <tt_blk_trolling+0x428>
    3118:	e1a0e00f 	mov	lr, pc
    311c:	e594f000 	ldr	pc, [r4]
    3120:	ea000005 	b	313c <tt_blk_trolling+0x3d4>
    3124:	e59d1010 	ldr	r1, [sp, #16]
    3128:	e59d2014 	ldr	r2, [sp, #20]
    312c:	e1a03005 	mov	r3, r5
    3130:	e59f005c 	ldr	r0, [pc, #92]	; 3194 <tt_blk_trolling+0x42c>
    3134:	e1a0e00f 	mov	lr, pc
    3138:	e594f000 	ldr	pc, [r4]
    313c:	e59f0054 	ldr	r0, [pc, #84]	; 3198 <tt_blk_trolling+0x430>
    3140:	e59d1024 	ldr	r1, [sp, #36]
    3144:	e890000c 	ldm	r0, {r2, r3}
    3148:	e0922001 	adds	r2, r2, r1
    314c:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3150:	e880000c 	stm	r0, {r2, r3}
    3154:	e59f0040 	ldr	r0, [pc, #64]	; 319c <tt_blk_trolling+0x434>
    3158:	e59f3014 	ldr	r3, [pc, #20]	; 3174 <tt_blk_trolling+0x40c>
    315c:	e1a0e00f 	mov	lr, pc
    3160:	e593f000 	ldr	pc, [r3]
    3164:	e3a00000 	mov	r0, #0	; 0x0
    3168:	e28dd02c 	add	sp, sp, #44	; 0x2c
    316c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3170:	000106dd 	.word	0x000106dd
    3174:	3c00d68c 	.word	0x3c00d68c
    3178:	000106fb 	.word	0x000106fb
    317c:	0001071b 	.word	0x0001071b
    3180:	00011563 	.word	0x00011563
    3184:	0000ff1a 	.word	0x0000ff1a
    3188:	000118ea 	.word	0x000118ea
    318c:	0001074f 	.word	0x0001074f
    3190:	00010752 	.word	0x00010752
    3194:	0001078c 	.word	0x0001078c
    3198:	3c00d140 	.word	0x3c00d140
    319c:	000104fe 	.word	0x000104fe

000031a0 <do_tt_blk>:
    31a0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    31a4:	e3520001 	cmp	r2, #1	; 0x1
    31a8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    31ac:	e1a05002 	mov	r5, r2
    31b0:	e1a07003 	mov	r7, r3
    31b4:	ca000005 	bgt	31d0 <do_tt_blk+0x30>
    31b8:	e59f01bc 	ldr	r0, [pc, #444]	; 337c <do_tt_blk+0x1dc>
    31bc:	e59f31bc 	ldr	r3, [pc, #444]	; 3380 <do_tt_blk+0x1e0>
    31c0:	e1a0e00f 	mov	lr, pc
    31c4:	e593f000 	ldr	pc, [r3]
    31c8:	e3e00000 	mvn	r0, #0	; 0x0
    31cc:	ea000068 	b	3374 <do_tt_blk+0x1d4>
    31d0:	e3520002 	cmp	r2, #2	; 0x2
    31d4:	03a04000 	moveq	r4, #0	; 0x0
    31d8:	01a06004 	moveq	r6, r4
    31dc:	0a000025 	beq	3278 <do_tt_blk+0xd8>
    31e0:	e3a01000 	mov	r1, #0	; 0x0
    31e4:	e3a0200a 	mov	r2, #10	; 0xa
    31e8:	e5930008 	ldr	r0, [r3, #8]
    31ec:	eb001ffd 	bl	b1e8 <simple_strtoul>
    31f0:	e59f118c 	ldr	r1, [pc, #396]	; 3384 <do_tt_blk+0x1e4>
    31f4:	e1a04000 	mov	r4, r0
    31f8:	e3a02005 	mov	r2, #5	; 0x5
    31fc:	e5970004 	ldr	r0, [r7, #4]
    3200:	ebfff7b7 	bl	10e4 <strncmp>
    3204:	e3500000 	cmp	r0, #0	; 0x0
    3208:	059f0178 	ldreq	r0, [pc, #376]	; 3388 <do_tt_blk+0x1e8>
    320c:	01a01004 	moveq	r1, r4
    3210:	059f3168 	ldreq	r3, [pc, #360]	; 3380 <do_tt_blk+0x1e0>
    3214:	0a000008 	beq	323c <do_tt_blk+0x9c>
    3218:	e59f116c 	ldr	r1, [pc, #364]	; 338c <do_tt_blk+0x1ec>
    321c:	e5970004 	ldr	r0, [r7, #4]
    3220:	e3a02003 	mov	r2, #3	; 0x3
    3224:	ebfff7ae 	bl	10e4 <strncmp>
    3228:	e3500000 	cmp	r0, #0	; 0x0
    322c:	e59f314c 	ldr	r3, [pc, #332]	; 3380 <do_tt_blk+0x1e0>
    3230:	059f0150 	ldreq	r0, [pc, #336]	; 3388 <do_tt_blk+0x1e8>
    3234:	159f0154 	ldrne	r0, [pc, #340]	; 3390 <do_tt_blk+0x1f0>
    3238:	e1a01004 	mov	r1, r4
    323c:	e1a0e00f 	mov	lr, pc
    3240:	e593f000 	ldr	pc, [r3]
    3244:	e3550003 	cmp	r5, #3	; 0x3
    3248:	03a06000 	moveq	r6, #0	; 0x0
    324c:	0a000009 	beq	3278 <do_tt_blk+0xd8>
    3250:	e3a01000 	mov	r1, #0	; 0x0
    3254:	e597000c 	ldr	r0, [r7, #12]
    3258:	e3a02010 	mov	r2, #16	; 0x10
    325c:	eb001fe1 	bl	b1e8 <simple_strtoul>
    3260:	e1a06000 	mov	r6, r0
    3264:	e1a01006 	mov	r1, r6
    3268:	e59f0124 	ldr	r0, [pc, #292]	; 3394 <do_tt_blk+0x1f4>
    326c:	e59f310c 	ldr	r3, [pc, #268]	; 3380 <do_tt_blk+0x1e0>
    3270:	e1a0e00f 	mov	lr, pc
    3274:	e593f000 	ldr	pc, [r3]
    3278:	e3a00000 	mov	r0, #0	; 0x0
    327c:	eb001541 	bl	8788 <get_timer>
    3280:	e59f1110 	ldr	r1, [pc, #272]	; 3398 <do_tt_blk+0x1f8>
    3284:	e3a02000 	mov	r2, #0	; 0x0
    3288:	e3a03000 	mov	r3, #0	; 0x0
    328c:	e881000c 	stm	r1, {r2, r3}
    3290:	e1a09a04 	lsl	r9, r4, #20
    3294:	e2875004 	add	r5, r7, #4	; 0x4
    3298:	e1a08b24 	lsr	r8, r4, #22
    329c:	e1a07504 	lsl	r7, r4, #10
    32a0:	e1a0a624 	lsr	sl, r4, #12
    32a4:	e1a0b000 	mov	fp, r0
    32a8:	e3a04000 	mov	r4, #0	; 0x0
    32ac:	e59f30cc 	ldr	r3, [pc, #204]	; 3380 <do_tt_blk+0x1e0>
    32b0:	e1a01004 	mov	r1, r4
    32b4:	e59f00e0 	ldr	r0, [pc, #224]	; 339c <do_tt_blk+0x1fc>
    32b8:	e1a0e00f 	mov	lr, pc
    32bc:	e593f000 	ldr	pc, [r3]
    32c0:	e59f10bc 	ldr	r1, [pc, #188]	; 3384 <do_tt_blk+0x1e4>
    32c4:	e3a02005 	mov	r2, #5	; 0x5
    32c8:	e5950000 	ldr	r0, [r5]
    32cc:	ebfff784 	bl	10e4 <strncmp>
    32d0:	e3500000 	cmp	r0, #0	; 0x0
    32d4:	e3a02003 	mov	r2, #3	; 0x3
    32d8:	e59f10ac 	ldr	r1, [pc, #172]	; 338c <do_tt_blk+0x1ec>
    32dc:	e1a03008 	mov	r3, r8
    32e0:	05950000 	ldreq	r0, [r5]
    32e4:	01a02007 	moveq	r2, r7
    32e8:	0a000008 	beq	3310 <do_tt_blk+0x170>
    32ec:	e5950000 	ldr	r0, [r5]
    32f0:	ebfff77b 	bl	10e4 <strncmp>
    32f4:	e3500000 	cmp	r0, #0	; 0x0
    32f8:	05950000 	ldreq	r0, [r5]
    32fc:	15950000 	ldrne	r0, [r5]
    3300:	e1a02009 	mov	r2, r9
    3304:	e1a0300a 	mov	r3, sl
    3308:	01a02007 	moveq	r2, r7
    330c:	01a03008 	moveq	r3, r8
    3310:	e58d6000 	str	r6, [sp]
    3314:	ebfffe93 	bl	2d68 <tt_blk_trolling>
    3318:	eb0023c9 	bl	c244 <tstc>
    331c:	e3500000 	cmp	r0, #0	; 0x0
    3320:	e2844001 	add	r4, r4, #1	; 0x1
    3324:	0affffe0 	beq	32ac <do_tt_blk+0x10c>
    3328:	eb0023c6 	bl	c248 <getc>
    332c:	e3500003 	cmp	r0, #3	; 0x3
    3330:	1affffdd 	bne	32ac <do_tt_blk+0x10c>
    3334:	e1a0000b 	mov	r0, fp
    3338:	eb001512 	bl	8788 <get_timer>
    333c:	e59f3054 	ldr	r3, [pc, #84]	; 3398 <do_tt_blk+0x1f8>
    3340:	e30e1a60 	movw	r1, #60000	; 0xea60
    3344:	e5934000 	ldr	r4, [r3]
    3348:	e5933004 	ldr	r3, [r3, #4]
    334c:	e1a04a24 	lsr	r4, r4, #20
    3350:	e1844603 	orr	r4, r4, r3, lsl #12
    3354:	eb002ba7 	bl	e1f8 <__aeabi_uidiv>
    3358:	e1a01004 	mov	r1, r4
    335c:	e1a02000 	mov	r2, r0
    3360:	e59f3018 	ldr	r3, [pc, #24]	; 3380 <do_tt_blk+0x1e0>
    3364:	e59f0034 	ldr	r0, [pc, #52]	; 33a0 <do_tt_blk+0x200>
    3368:	e1a0e00f 	mov	lr, pc
    336c:	e593f000 	ldr	pc, [r3]
    3370:	e3a00000 	mov	r0, #0	; 0x0
    3374:	e28dd00c 	add	sp, sp, #12	; 0xc
    3378:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    337c:	000107b9 	.word	0x000107b9
    3380:	3c00d68c 	.word	0x3c00d68c
    3384:	00010800 	.word	0x00010800
    3388:	00010806 	.word	0x00010806
    338c:	000101bb 	.word	0x000101bb
    3390:	0001082a 	.word	0x0001082a
    3394:	0001084e 	.word	0x0001084e
    3398:	3c00d140 	.word	0x3c00d140
    339c:	00010871 	.word	0x00010871
    33a0:	0001087b 	.word	0x0001087b

000033a4 <__show_strbuf>:
    33a4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    33a8:	e59f4058 	ldr	r4, [pc, #88]	; 3408 <__show_strbuf+0x64>
    33ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    33b0:	e1a06001 	mov	r6, r1
    33b4:	e1a01000 	mov	r1, r0
    33b8:	e59f004c 	ldr	r0, [pc, #76]	; 340c <__show_strbuf+0x68>
    33bc:	e1a05002 	mov	r5, r2
    33c0:	e1a07004 	mov	r7, r4
    33c4:	e1a0e00f 	mov	lr, pc
    33c8:	e594f000 	ldr	pc, [r4]
    33cc:	e3a04000 	mov	r4, #0	; 0x0
    33d0:	ea000003 	b	33e4 <__show_strbuf+0x40>
    33d4:	e7d61004 	ldrb	r1, [r6, r4]
    33d8:	e1a0e00f 	mov	lr, pc
    33dc:	e597f000 	ldr	pc, [r7]
    33e0:	e2844001 	add	r4, r4, #1	; 0x1
    33e4:	e1540005 	cmp	r4, r5
    33e8:	e59f0020 	ldr	r0, [pc, #32]	; 3410 <__show_strbuf+0x6c>
    33ec:	bafffff8 	blt	33d4 <__show_strbuf+0x30>
    33f0:	e59f001c 	ldr	r0, [pc, #28]	; 3414 <__show_strbuf+0x70>
    33f4:	e59f300c 	ldr	r3, [pc, #12]	; 3408 <__show_strbuf+0x64>
    33f8:	e1a0e00f 	mov	lr, pc
    33fc:	e593f000 	ldr	pc, [r3]
    3400:	e28dd004 	add	sp, sp, #4	; 0x4
    3404:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3408:	3c00d68c 	.word	0x3c00d68c
    340c:	000108c9 	.word	0x000108c9
    3410:	0001171c 	.word	0x0001171c
    3414:	000104fe 	.word	0x000104fe

00003418 <do_crypto_quick>:
    3418:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    341c:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    3420:	e28da0e0 	add	sl, sp, #224	; 0xe0
    3424:	e3a04010 	mov	r4, #16	; 0x10
    3428:	e28d70c0 	add	r7, sp, #192	; 0xc0
    342c:	e1a02004 	mov	r2, r4
    3430:	e3a09020 	mov	r9, #32	; 0x20
    3434:	e59f11f0 	ldr	r1, [pc, #496]	; 362c <do_crypto_quick+0x214>
    3438:	e1a0000a 	mov	r0, sl
    343c:	e3a05040 	mov	r5, #64	; 0x40
    3440:	e28d6080 	add	r6, sp, #128	; 0x80
    3444:	ebfff75f 	bl	11c8 <memcpy>
    3448:	e1a02009 	mov	r2, r9
    344c:	e59f11dc 	ldr	r1, [pc, #476]	; 3630 <do_crypto_quick+0x218>
    3450:	e1a00007 	mov	r0, r7
    3454:	ebfff75b 	bl	11c8 <memcpy>
    3458:	e08d8005 	add	r8, sp, r5
    345c:	e1a02005 	mov	r2, r5
    3460:	e59f11cc 	ldr	r1, [pc, #460]	; 3634 <do_crypto_quick+0x21c>
    3464:	e1a00006 	mov	r0, r6
    3468:	ebfff756 	bl	11c8 <memcpy>
    346c:	e1a02005 	mov	r2, r5
    3470:	e59f11c0 	ldr	r1, [pc, #448]	; 3638 <do_crypto_quick+0x220>
    3474:	e1a00008 	mov	r0, r8
    3478:	ebfff752 	bl	11c8 <memcpy>
    347c:	e1a02004 	mov	r2, r4
    3480:	e1a0100a 	mov	r1, sl
    3484:	e59f01b0 	ldr	r0, [pc, #432]	; 363c <do_crypto_quick+0x224>
    3488:	ebffffc5 	bl	33a4 <__show_strbuf>
    348c:	e1a02009 	mov	r2, r9
    3490:	e1a01007 	mov	r1, r7
    3494:	e59f01a4 	ldr	r0, [pc, #420]	; 3640 <do_crypto_quick+0x228>
    3498:	ebffffc1 	bl	33a4 <__show_strbuf>
    349c:	e1a01006 	mov	r1, r6
    34a0:	e1a02005 	mov	r2, r5
    34a4:	e59f0198 	ldr	r0, [pc, #408]	; 3644 <do_crypto_quick+0x22c>
    34a8:	ebffffbd 	bl	33a4 <__show_strbuf>
    34ac:	e1a01008 	mov	r1, r8
    34b0:	e1a02005 	mov	r2, r5
    34b4:	e59f018c 	ldr	r0, [pc, #396]	; 3648 <do_crypto_quick+0x230>
    34b8:	ebffffb9 	bl	33a4 <__show_strbuf>
    34bc:	e1a01007 	mov	r1, r7
    34c0:	e1a0200a 	mov	r2, sl
    34c4:	e3a00001 	mov	r0, #1	; 0x1
    34c8:	eb0006f9 	bl	50b4 <ss_aes_init>
    34cc:	e1a0100d 	mov	r1, sp
    34d0:	e1a02005 	mov	r2, r5
    34d4:	e1a00006 	mov	r0, r6
    34d8:	eb0006e4 	bl	5070 <ss_aes_data>
    34dc:	e1a0100d 	mov	r1, sp
    34e0:	e1a02005 	mov	r2, r5
    34e4:	e59f0160 	ldr	r0, [pc, #352]	; 364c <do_crypto_quick+0x234>
    34e8:	ebffffad 	bl	33a4 <__show_strbuf>
    34ec:	e1a01007 	mov	r1, r7
    34f0:	e1a0200a 	mov	r2, sl
    34f4:	e3a00000 	mov	r0, #0	; 0x0
    34f8:	eb0006ed 	bl	50b4 <ss_aes_init>
    34fc:	e59f714c 	ldr	r7, [pc, #332]	; 3650 <do_crypto_quick+0x238>
    3500:	e1a0100d 	mov	r1, sp
    3504:	e1a02005 	mov	r2, r5
    3508:	e1a00008 	mov	r0, r8
    350c:	eb0006d7 	bl	5070 <ss_aes_data>
    3510:	e1a0100d 	mov	r1, sp
    3514:	e1a02005 	mov	r2, r5
    3518:	e59f0134 	ldr	r0, [pc, #308]	; 3654 <do_crypto_quick+0x23c>
    351c:	ebffffa0 	bl	33a4 <__show_strbuf>
    3520:	e1a02007 	mov	r2, r7
    3524:	e3a01000 	mov	r1, #0	; 0x0
    3528:	e3a00001 	mov	r0, #1	; 0x1
    352c:	eb0006e0 	bl	50b4 <ss_aes_init>
    3530:	e1a0100d 	mov	r1, sp
    3534:	e1a02005 	mov	r2, r5
    3538:	e1a00006 	mov	r0, r6
    353c:	eb0006cb 	bl	5070 <ss_aes_data>
    3540:	e1a0100d 	mov	r1, sp
    3544:	e1a02005 	mov	r2, r5
    3548:	e59f0108 	ldr	r0, [pc, #264]	; 3658 <do_crypto_quick+0x240>
    354c:	ebffff94 	bl	33a4 <__show_strbuf>
    3550:	e3a00000 	mov	r0, #0	; 0x0
    3554:	e1a02007 	mov	r2, r7
    3558:	e1a01000 	mov	r1, r0
    355c:	eb0006d4 	bl	50b4 <ss_aes_init>
    3560:	e1a0100d 	mov	r1, sp
    3564:	e1a02005 	mov	r2, r5
    3568:	e1a0000d 	mov	r0, sp
    356c:	eb0006bf 	bl	5070 <ss_aes_data>
    3570:	e1a02005 	mov	r2, r5
    3574:	e1a0100d 	mov	r1, sp
    3578:	e59f00dc 	ldr	r0, [pc, #220]	; 365c <do_crypto_quick+0x244>
    357c:	ebffff88 	bl	33a4 <__show_strbuf>
    3580:	e1a01005 	mov	r1, r5
    3584:	e3a00001 	mov	r0, #1	; 0x1
    3588:	eb0007e7 	bl	552c <hash_init>
    358c:	e1a01005 	mov	r1, r5
    3590:	e1a00006 	mov	r0, r6
    3594:	eb0007bf 	bl	5498 <hash_data>
    3598:	e1a0000d 	mov	r0, sp
    359c:	eb0007a1 	bl	5428 <hash_value>
    35a0:	eb000799 	bl	540c <hash_len>
    35a4:	e1a0100d 	mov	r1, sp
    35a8:	e1a02000 	mov	r2, r0
    35ac:	e59f00ac 	ldr	r0, [pc, #172]	; 3660 <do_crypto_quick+0x248>
    35b0:	ebffff7b 	bl	33a4 <__show_strbuf>
    35b4:	e1a01005 	mov	r1, r5
    35b8:	e3a00002 	mov	r0, #2	; 0x2
    35bc:	eb0007da 	bl	552c <hash_init>
    35c0:	e1a01005 	mov	r1, r5
    35c4:	e1a00006 	mov	r0, r6
    35c8:	eb0007b2 	bl	5498 <hash_data>
    35cc:	e1a0000d 	mov	r0, sp
    35d0:	eb000794 	bl	5428 <hash_value>
    35d4:	eb00078c 	bl	540c <hash_len>
    35d8:	e1a0100d 	mov	r1, sp
    35dc:	e1a02000 	mov	r2, r0
    35e0:	e59f007c 	ldr	r0, [pc, #124]	; 3664 <do_crypto_quick+0x24c>
    35e4:	ebffff6e 	bl	33a4 <__show_strbuf>
    35e8:	e1a01005 	mov	r1, r5
    35ec:	e3a00003 	mov	r0, #3	; 0x3
    35f0:	eb0007cd 	bl	552c <hash_init>
    35f4:	e1a01005 	mov	r1, r5
    35f8:	e1a00006 	mov	r0, r6
    35fc:	eb0007a5 	bl	5498 <hash_data>
    3600:	e1a0000d 	mov	r0, sp
    3604:	eb000787 	bl	5428 <hash_value>
    3608:	eb00077f 	bl	540c <hash_len>
    360c:	e1a0100d 	mov	r1, sp
    3610:	e1a02000 	mov	r2, r0
    3614:	e59f004c 	ldr	r0, [pc, #76]	; 3668 <do_crypto_quick+0x250>
    3618:	ebffff61 	bl	33a4 <__show_strbuf>
    361c:	e1a0400d 	mov	r4, sp
    3620:	e3a00000 	mov	r0, #0	; 0x0
    3624:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    3628:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    362c:	0000efa4 	.word	0x0000efa4
    3630:	0000ef84 	.word	0x0000ef84
    3634:	0000ef44 	.word	0x0000ef44
    3638:	0000ef04 	.word	0x0000ef04
    363c:	000108ce 	.word	0x000108ce
    3640:	000108d1 	.word	0x000108d1
    3644:	000108d5 	.word	0x000108d5
    3648:	000108e0 	.word	0x000108e0
    364c:	000108f1 	.word	0x000108f1
    3650:	3c00d09c 	.word	0x3c00d09c
    3654:	00010904 	.word	0x00010904
    3658:	000108ed 	.word	0x000108ed
    365c:	00010900 	.word	0x00010900
    3660:	00010913 	.word	0x00010913
    3664:	00010917 	.word	0x00010917
    3668:	0001091c 	.word	0x0001091c

0000366c <tt_crypto_hash>:
    366c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3670:	e3a0544e 	mov	r5, #1308622848	; 0x4e000000
    3674:	e5951000 	ldr	r1, [r5]
    3678:	e59f2220 	ldr	r2, [pc, #544]	; 38a0 <tt_crypto_hash+0x234>
    367c:	e24dd038 	sub	sp, sp, #56	; 0x38
    3680:	e59f321c 	ldr	r3, [pc, #540]	; 38a4 <tt_crypto_hash+0x238>
    3684:	e1510002 	cmp	r1, r2
    3688:	e28dc028 	add	ip, sp, #40	; 0x28
    368c:	e1a06000 	mov	r6, r0
    3690:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3694:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    3698:	0a000006 	beq	36b8 <tt_crypto_hash+0x4c>
    369c:	e59f0204 	ldr	r0, [pc, #516]	; 38a8 <tt_crypto_hash+0x23c>
    36a0:	e59f3204 	ldr	r3, [pc, #516]	; 38ac <tt_crypto_hash+0x240>
    36a4:	e1a0e00f 	mov	lr, pc
    36a8:	e593f000 	ldr	pc, [r3]
    36ac:	e59f01fc 	ldr	r0, [pc, #508]	; 38b0 <tt_crypto_hash+0x244>
    36b0:	eb001410 	bl	86f8 <udelay>
    36b4:	ea000008 	b	36dc <tt_crypto_hash+0x70>
    36b8:	e5951004 	ldr	r1, [r5, #4]
    36bc:	e3a04000 	mov	r4, #0	; 0x0
    36c0:	e3510003 	cmp	r1, #3	; 0x3
    36c4:	e5854000 	str	r4, [r5]
    36c8:	e59fc1dc 	ldr	ip, [pc, #476]	; 38ac <tt_crypto_hash+0x240>
    36cc:	9a000004 	bls	36e4 <tt_crypto_hash+0x78>
    36d0:	e59f01dc 	ldr	r0, [pc, #476]	; 38b4 <tt_crypto_hash+0x248>
    36d4:	e1a0e00f 	mov	lr, pc
    36d8:	e59cf000 	ldr	pc, [ip]
    36dc:	e3e0c000 	mvn	ip, #0	; 0x0
    36e0:	ea00006b 	b	3894 <tt_crypto_hash+0x228>
    36e4:	e28d2038 	add	r2, sp, #56	; 0x38
    36e8:	e0823101 	add	r3, r2, r1, lsl #2
    36ec:	e5131010 	ldr	r1, [r3, #-16]
    36f0:	e5952008 	ldr	r2, [r5, #8]
    36f4:	e59f01bc 	ldr	r0, [pc, #444]	; 38b8 <tt_crypto_hash+0x24c>
    36f8:	e1a0e00f 	mov	lr, pc
    36fc:	e59cf000 	ldr	pc, [ip]
    3700:	e9950003 	ldmib	r5, {r0, r1}
    3704:	eb000788 	bl	552c <hash_init>
    3708:	eb00073f 	bl	540c <hash_len>
    370c:	e1a03000 	mov	r3, r0
    3710:	e59f01a4 	ldr	r0, [pc, #420]	; 38bc <tt_crypto_hash+0x250>
    3714:	e3a0c020 	mov	ip, #32	; 0x20
    3718:	e1a01000 	mov	r1, r0
    371c:	e3a02001 	mov	r2, #1	; 0x1
    3720:	e58dc000 	str	ip, [sp]
    3724:	eb001e17 	bl	af88 <print_buffer>
    3728:	e3560000 	cmp	r6, #0	; 0x0
    372c:	0a000022 	beq	37bc <tt_crypto_hash+0x150>
    3730:	e1a09005 	mov	r9, r5
    3734:	e1a0a004 	mov	sl, r4
    3738:	e5955008 	ldr	r5, [r5, #8]
    373c:	e2868901 	add	r8, r6, #16384	; 0x4000
    3740:	ea000019 	b	37ac <tt_crypto_hash+0x140>
    3744:	e5991008 	ldr	r1, [r9, #8]
    3748:	e3550902 	cmp	r5, #32768	; 0x8000
    374c:	b1a04005 	movlt	r4, r5
    3750:	a3a04902 	movge	r4, #32768	; 0x8000
    3754:	e0651001 	rsb	r1, r5, r1
    3758:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    375c:	e2811030 	add	r1, r1, #48	; 0x30
    3760:	e1a02004 	mov	r2, r4
    3764:	ebfff697 	bl	11c8 <memcpy>
    3768:	e3a00000 	mov	r0, #0	; 0x0
    376c:	eb001405 	bl	8788 <get_timer>
    3770:	e3550901 	cmp	r5, #16384	; 0x4000
    3774:	b1a01005 	movlt	r1, r5
    3778:	a3a01901 	movge	r1, #16384	; 0x4000
    377c:	e1a07000 	mov	r7, r0
    3780:	e1a00006 	mov	r0, r6
    3784:	eb000743 	bl	5498 <hash_data>
    3788:	e3540902 	cmp	r4, #32768	; 0x8000
    378c:	e0645005 	rsb	r5, r4, r5
    3790:	1a000002 	bne	37a0 <tt_crypto_hash+0x134>
    3794:	e3a01901 	mov	r1, #16384	; 0x4000
    3798:	e1a00008 	mov	r0, r8
    379c:	eb00073d 	bl	5498 <hash_data>
    37a0:	e1a00007 	mov	r0, r7
    37a4:	eb0013f7 	bl	8788 <get_timer>
    37a8:	e08aa000 	add	sl, sl, r0
    37ac:	e3550000 	cmp	r5, #0	; 0x0
    37b0:	e1a00006 	mov	r0, r6
    37b4:	1affffe2 	bne	3744 <tt_crypto_hash+0xd8>
    37b8:	ea000008 	b	37e0 <tt_crypto_hash+0x174>
    37bc:	e1a00006 	mov	r0, r6
    37c0:	eb0013f0 	bl	8788 <get_timer>
    37c4:	e5951008 	ldr	r1, [r5, #8]
    37c8:	e1a04000 	mov	r4, r0
    37cc:	e59f00ec 	ldr	r0, [pc, #236]	; 38c0 <tt_crypto_hash+0x254>
    37d0:	eb000730 	bl	5498 <hash_data>
    37d4:	e1a00004 	mov	r0, r4
    37d8:	eb0013ea 	bl	8788 <get_timer>
    37dc:	e1a0a000 	mov	sl, r0
    37e0:	e28d4008 	add	r4, sp, #8	; 0x8
    37e4:	e3a0644e 	mov	r6, #1308622848	; 0x4e000000
    37e8:	e1a00004 	mov	r0, r4
    37ec:	eb00070d 	bl	5428 <hash_value>
    37f0:	e5965008 	ldr	r5, [r6, #8]
    37f4:	eb000704 	bl	540c <hash_len>
    37f8:	e3a0c020 	mov	ip, #32	; 0x20
    37fc:	e1a03000 	mov	r3, r0
    3800:	e1a01004 	mov	r1, r4
    3804:	e3a02001 	mov	r2, #1	; 0x1
    3808:	e1a00004 	mov	r0, r4
    380c:	e58dc000 	str	ip, [sp]
    3810:	eb001ddc 	bl	af88 <print_buffer>
    3814:	e5963004 	ldr	r3, [r6, #4]
    3818:	e28d1038 	add	r1, sp, #56	; 0x38
    381c:	e0812103 	add	r2, r1, r3, lsl #2
    3820:	e59f3084 	ldr	r3, [pc, #132]	; 38ac <tt_crypto_hash+0x240>
    3824:	e5127010 	ldr	r7, [r2, #-16]
    3828:	e5938000 	ldr	r8, [r3]
    382c:	eb0006f6 	bl	540c <hash_len>
    3830:	e1a01004 	mov	r1, r4
    3834:	e1a02000 	mov	r2, r0
    3838:	e59f007c 	ldr	r0, [pc, #124]	; 38bc <tt_crypto_hash+0x250>
    383c:	ebfff682 	bl	124c <memcmp>
    3840:	e1a05525 	lsr	r5, r5, #10
    3844:	e3500000 	cmp	r0, #0	; 0x0
    3848:	e59f3074 	ldr	r3, [pc, #116]	; 38c4 <tt_crypto_hash+0x258>
    384c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3850:	e1a0100a 	mov	r1, sl
    3854:	e0000590 	mul	r0, r0, r5
    3858:	e59f4068 	ldr	r4, [pc, #104]	; 38c8 <tt_crypto_hash+0x25c>
    385c:	01a04003 	moveq	r4, r3
    3860:	eb002a64 	bl	e1f8 <__aeabi_uidiv>
    3864:	e1a01007 	mov	r1, r7
    3868:	e1a03000 	mov	r3, r0
    386c:	e1a02004 	mov	r2, r4
    3870:	e59f0054 	ldr	r0, [pc, #84]	; 38cc <tt_crypto_hash+0x260>
    3874:	e12fff38 	blx	r8
    3878:	e59f1050 	ldr	r1, [pc, #80]	; 38d0 <tt_crypto_hash+0x264>
    387c:	e5960008 	ldr	r0, [r6, #8]
    3880:	e891000c 	ldm	r1, {r2, r3}
    3884:	e0922000 	adds	r2, r2, r0
    3888:	e2a33000 	adc	r3, r3, #0	; 0x0
    388c:	e881000c 	stm	r1, {r2, r3}
    3890:	e3a0c000 	mov	ip, #0	; 0x0
    3894:	e1a0000c 	mov	r0, ip
    3898:	e28dd038 	add	sp, sp, #56	; 0x38
    389c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    38a0:	99997777 	.word	0x99997777
    38a4:	0000efb4 	.word	0x0000efb4
    38a8:	00010923 	.word	0x00010923
    38ac:	3c00d68c 	.word	0x3c00d68c
    38b0:	004c4b40 	.word	0x004c4b40
    38b4:	00010942 	.word	0x00010942
    38b8:	0001095d 	.word	0x0001095d
    38bc:	4e000010 	.word	0x4e000010
    38c0:	4e000030 	.word	0x4e000030
    38c4:	00010978 	.word	0x00010978
    38c8:	0001097d 	.word	0x0001097d
    38cc:	00010984 	.word	0x00010984
    38d0:	3c00d150 	.word	0x3c00d150

000038d4 <tt_crypto_aes>:
    38d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    38d8:	e24dd014 	sub	sp, sp, #20	; 0x14
    38dc:	e1a06000 	mov	r6, r0
    38e0:	e3a01000 	mov	r1, #0	; 0x0
    38e4:	e28d000c 	add	r0, sp, #12	; 0xc
    38e8:	e3a02502 	mov	r2, #8388608	; 0x800000
    38ec:	eb000284 	bl	4304 <tt_rnd_get_data>
    38f0:	e250b000 	subs	fp, r0, #0	; 0x0
    38f4:	e59f4340 	ldr	r4, [pc, #832]	; 3c3c <tt_crypto_aes+0x368>
    38f8:	059f0340 	ldreq	r0, [pc, #832]	; 3c40 <tt_crypto_aes+0x36c>
    38fc:	0a00005c 	beq	3a74 <tt_crypto_aes+0x1a0>
    3900:	e59d200c 	ldr	r2, [sp, #12]
    3904:	e1a0100b 	mov	r1, fp
    3908:	e3c2200f 	bic	r2, r2, #15	; 0xf
    390c:	e1a03002 	mov	r3, r2
    3910:	e58d200c 	str	r2, [sp, #12]
    3914:	e59f0328 	ldr	r0, [pc, #808]	; 3c44 <tt_crypto_aes+0x370>
    3918:	e1a02006 	mov	r2, r6
    391c:	e1a0e00f 	mov	lr, pc
    3920:	e594f000 	ldr	pc, [r4]
    3924:	e59f031c 	ldr	r0, [pc, #796]	; 3c48 <tt_crypto_aes+0x374>
    3928:	e1a0e00f 	mov	lr, pc
    392c:	e594f000 	ldr	pc, [r4]
    3930:	e1a0100b 	mov	r1, fp
    3934:	e59d200c 	ldr	r2, [sp, #12]
    3938:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    393c:	ebfff621 	bl	11c8 <memcpy>
    3940:	e59f0304 	ldr	r0, [pc, #772]	; 3c4c <tt_crypto_aes+0x378>
    3944:	e1a0e00f 	mov	lr, pc
    3948:	e594f000 	ldr	pc, [r4]
    394c:	e3a01000 	mov	r1, #0	; 0x0
    3950:	e59f22f8 	ldr	r2, [pc, #760]	; 3c50 <tt_crypto_aes+0x37c>
    3954:	e3a00001 	mov	r0, #1	; 0x1
    3958:	eb0005d5 	bl	50b4 <ss_aes_init>
    395c:	e59f02f0 	ldr	r0, [pc, #752]	; 3c54 <tt_crypto_aes+0x380>
    3960:	e1a0e00f 	mov	lr, pc
    3964:	e594f000 	ldr	pc, [r4]
    3968:	e3560000 	cmp	r6, #0	; 0x0
    396c:	159d500c 	ldrne	r5, [sp, #12]
    3970:	13a0a000 	movne	sl, #0	; 0x0
    3974:	12868901 	addne	r8, r6, #16384	; 0x4000
    3978:	1a000022 	bne	3a08 <tt_crypto_aes+0x134>
    397c:	ea000025 	b	3a18 <tt_crypto_aes+0x144>
    3980:	e59d100c 	ldr	r1, [sp, #12]
    3984:	e3550902 	cmp	r5, #32768	; 0x8000
    3988:	b1a07005 	movlt	r7, r5
    398c:	a3a07902 	movge	r7, #32768	; 0x8000
    3990:	e0651001 	rsb	r1, r5, r1
    3994:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3998:	e1a02007 	mov	r2, r7
    399c:	ebfff609 	bl	11c8 <memcpy>
    39a0:	e3a00000 	mov	r0, #0	; 0x0
    39a4:	eb001377 	bl	8788 <get_timer>
    39a8:	e3550901 	cmp	r5, #16384	; 0x4000
    39ac:	b1a02005 	movlt	r2, r5
    39b0:	a3a02901 	movge	r2, #16384	; 0x4000
    39b4:	e1a04000 	mov	r4, r0
    39b8:	e1a01006 	mov	r1, r6
    39bc:	e1a00006 	mov	r0, r6
    39c0:	eb0005aa 	bl	5070 <ss_aes_data>
    39c4:	e3570902 	cmp	r7, #32768	; 0x8000
    39c8:	1a000003 	bne	39dc <tt_crypto_aes+0x108>
    39cc:	e1a01008 	mov	r1, r8
    39d0:	e3a02901 	mov	r2, #16384	; 0x4000
    39d4:	e1a00008 	mov	r0, r8
    39d8:	eb0005a4 	bl	5070 <ss_aes_data>
    39dc:	e1a00004 	mov	r0, r4
    39e0:	eb001368 	bl	8788 <get_timer>
    39e4:	e1a04000 	mov	r4, r0
    39e8:	e59d000c 	ldr	r0, [sp, #12]
    39ec:	e1a01006 	mov	r1, r6
    39f0:	e0650000 	rsb	r0, r5, r0
    39f4:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    39f8:	e1a02007 	mov	r2, r7
    39fc:	ebfff5f1 	bl	11c8 <memcpy>
    3a00:	e08aa004 	add	sl, sl, r4
    3a04:	e0675005 	rsb	r5, r7, r5
    3a08:	e3550000 	cmp	r5, #0	; 0x0
    3a0c:	e1a00006 	mov	r0, r6
    3a10:	1affffda 	bne	3980 <tt_crypto_aes+0xac>
    3a14:	ea000009 	b	3a40 <tt_crypto_aes+0x16c>
    3a18:	e1a00006 	mov	r0, r6
    3a1c:	eb001359 	bl	8788 <get_timer>
    3a20:	e1a04000 	mov	r4, r0
    3a24:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3a28:	e1a01000 	mov	r1, r0
    3a2c:	e59d200c 	ldr	r2, [sp, #12]
    3a30:	eb00058e 	bl	5070 <ss_aes_data>
    3a34:	e1a00004 	mov	r0, r4
    3a38:	eb001352 	bl	8788 <get_timer>
    3a3c:	e1a0a000 	mov	sl, r0
    3a40:	e59d300c 	ldr	r3, [sp, #12]
    3a44:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3a48:	e1a0100b 	mov	r1, fp
    3a4c:	e3a02010 	mov	r2, #16	; 0x10
    3a50:	e58d3004 	str	r3, [sp, #4]
    3a54:	ebfff5fc 	bl	124c <memcmp>
    3a58:	e3500000 	cmp	r0, #0	; 0x0
    3a5c:	e59f41d8 	ldr	r4, [pc, #472]	; 3c3c <tt_crypto_aes+0x368>
    3a60:	1a000007 	bne	3a84 <tt_crypto_aes+0x1b0>
    3a64:	e59f01ec 	ldr	r0, [pc, #492]	; 3c58 <tt_crypto_aes+0x384>
    3a68:	e1a0e00f 	mov	lr, pc
    3a6c:	e594f000 	ldr	pc, [r4]
    3a70:	e59f01e4 	ldr	r0, [pc, #484]	; 3c5c <tt_crypto_aes+0x388>
    3a74:	e1a0e00f 	mov	lr, pc
    3a78:	e594f000 	ldr	pc, [r4]
    3a7c:	e3e00000 	mvn	r0, #0	; 0x0
    3a80:	ea00006b 	b	3c34 <tt_crypto_aes+0x360>
    3a84:	e3a00000 	mov	r0, #0	; 0x0
    3a88:	e1a01000 	mov	r1, r0
    3a8c:	e59f21bc 	ldr	r2, [pc, #444]	; 3c50 <tt_crypto_aes+0x37c>
    3a90:	eb000587 	bl	50b4 <ss_aes_init>
    3a94:	e59f01c4 	ldr	r0, [pc, #452]	; 3c60 <tt_crypto_aes+0x38c>
    3a98:	e1a0e00f 	mov	lr, pc
    3a9c:	e594f000 	ldr	pc, [r4]
    3aa0:	e3560000 	cmp	r6, #0	; 0x0
    3aa4:	159d500c 	ldrne	r5, [sp, #12]
    3aa8:	13a08000 	movne	r8, #0	; 0x0
    3aac:	12869901 	addne	r9, r6, #16384	; 0x4000
    3ab0:	1a000022 	bne	3b40 <tt_crypto_aes+0x26c>
    3ab4:	ea000025 	b	3b50 <tt_crypto_aes+0x27c>
    3ab8:	eb001332 	bl	8788 <get_timer>
    3abc:	e59d100c 	ldr	r1, [sp, #12]
    3ac0:	e3550902 	cmp	r5, #32768	; 0x8000
    3ac4:	b1a07005 	movlt	r7, r5
    3ac8:	a3a07902 	movge	r7, #32768	; 0x8000
    3acc:	e0651001 	rsb	r1, r5, r1
    3ad0:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3ad4:	e1a02007 	mov	r2, r7
    3ad8:	e1a04000 	mov	r4, r0
    3adc:	e1a00006 	mov	r0, r6
    3ae0:	ebfff5b8 	bl	11c8 <memcpy>
    3ae4:	e3550901 	cmp	r5, #16384	; 0x4000
    3ae8:	b1a02005 	movlt	r2, r5
    3aec:	a3a02901 	movge	r2, #16384	; 0x4000
    3af0:	e1a00006 	mov	r0, r6
    3af4:	e1a01006 	mov	r1, r6
    3af8:	eb00055c 	bl	5070 <ss_aes_data>
    3afc:	e3570902 	cmp	r7, #32768	; 0x8000
    3b00:	1a000003 	bne	3b14 <tt_crypto_aes+0x240>
    3b04:	e1a01009 	mov	r1, r9
    3b08:	e3a02901 	mov	r2, #16384	; 0x4000
    3b0c:	e1a00009 	mov	r0, r9
    3b10:	eb000556 	bl	5070 <ss_aes_data>
    3b14:	e1a00004 	mov	r0, r4
    3b18:	eb00131a 	bl	8788 <get_timer>
    3b1c:	e1a04000 	mov	r4, r0
    3b20:	e59d000c 	ldr	r0, [sp, #12]
    3b24:	e1a01006 	mov	r1, r6
    3b28:	e0650000 	rsb	r0, r5, r0
    3b2c:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3b30:	e1a02007 	mov	r2, r7
    3b34:	ebfff5a3 	bl	11c8 <memcpy>
    3b38:	e0888004 	add	r8, r8, r4
    3b3c:	e0675005 	rsb	r5, r7, r5
    3b40:	e3a00000 	mov	r0, #0	; 0x0
    3b44:	e1550000 	cmp	r5, r0
    3b48:	1affffda 	bne	3ab8 <tt_crypto_aes+0x1e4>
    3b4c:	ea000009 	b	3b78 <tt_crypto_aes+0x2a4>
    3b50:	e1a00006 	mov	r0, r6
    3b54:	eb00130b 	bl	8788 <get_timer>
    3b58:	e1a04000 	mov	r4, r0
    3b5c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3b60:	e1a01000 	mov	r1, r0
    3b64:	e59d200c 	ldr	r2, [sp, #12]
    3b68:	eb000540 	bl	5070 <ss_aes_data>
    3b6c:	e1a00004 	mov	r0, r4
    3b70:	eb001304 	bl	8788 <get_timer>
    3b74:	e1a08000 	mov	r8, r0
    3b78:	e59d3004 	ldr	r3, [sp, #4]
    3b7c:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3b80:	e1a00543 	asr	r0, r3, #10
    3b84:	e1a0100a 	mov	r1, sl
    3b88:	e0000094 	mul	r0, r4, r0
    3b8c:	eb002a1b 	bl	e400 <__aeabi_idiv>
    3b90:	e1a06000 	mov	r6, r0
    3b94:	e59d000c 	ldr	r0, [sp, #12]
    3b98:	e1a01008 	mov	r1, r8
    3b9c:	e1a00540 	asr	r0, r0, #10
    3ba0:	e0000094 	mul	r0, r4, r0
    3ba4:	eb002a15 	bl	e400 <__aeabi_idiv>
    3ba8:	e59f408c 	ldr	r4, [pc, #140]	; 3c3c <tt_crypto_aes+0x368>
    3bac:	e1a05000 	mov	r5, r0
    3bb0:	e59f00ac 	ldr	r0, [pc, #172]	; 3c64 <tt_crypto_aes+0x390>
    3bb4:	e1a0e00f 	mov	lr, pc
    3bb8:	e594f000 	ldr	pc, [r4]
    3bbc:	e1a0000b 	mov	r0, fp
    3bc0:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3bc4:	e59d200c 	ldr	r2, [sp, #12]
    3bc8:	e3a0300c 	mov	r3, #12	; 0xc
    3bcc:	eb00014f 	bl	4110 <tt_cmp_buffer>
    3bd0:	e3500000 	cmp	r0, #0	; 0x0
    3bd4:	0a000006 	beq	3bf4 <tt_crypto_aes+0x320>
    3bd8:	e1a03000 	mov	r3, r0
    3bdc:	e1a01006 	mov	r1, r6
    3be0:	e1a02005 	mov	r2, r5
    3be4:	e59f007c 	ldr	r0, [pc, #124]	; 3c68 <tt_crypto_aes+0x394>
    3be8:	e1a0e00f 	mov	lr, pc
    3bec:	e594f000 	ldr	pc, [r4]
    3bf0:	ea000004 	b	3c08 <tt_crypto_aes+0x334>
    3bf4:	e1a01006 	mov	r1, r6
    3bf8:	e1a02005 	mov	r2, r5
    3bfc:	e59f0068 	ldr	r0, [pc, #104]	; 3c6c <tt_crypto_aes+0x398>
    3c00:	e1a0e00f 	mov	lr, pc
    3c04:	e594f000 	ldr	pc, [r4]
    3c08:	e59f0060 	ldr	r0, [pc, #96]	; 3c70 <tt_crypto_aes+0x39c>
    3c0c:	e59d100c 	ldr	r1, [sp, #12]
    3c10:	e890000c 	ldm	r0, {r2, r3}
    3c14:	e0922001 	adds	r2, r2, r1
    3c18:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3c1c:	e880000c 	stm	r0, {r2, r3}
    3c20:	e59f0030 	ldr	r0, [pc, #48]	; 3c58 <tt_crypto_aes+0x384>
    3c24:	e59f3010 	ldr	r3, [pc, #16]	; 3c3c <tt_crypto_aes+0x368>
    3c28:	e1a0e00f 	mov	lr, pc
    3c2c:	e593f000 	ldr	pc, [r3]
    3c30:	e3a00000 	mov	r0, #0	; 0x0
    3c34:	e28dd014 	add	sp, sp, #20	; 0x14
    3c38:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3c3c:	3c00d68c 	.word	0x3c00d68c
    3c40:	000109a4 	.word	0x000109a4
    3c44:	000109c7 	.word	0x000109c7
    3c48:	00010409 	.word	0x00010409
    3c4c:	0001020b 	.word	0x0001020b
    3c50:	3c00d09c 	.word	0x3c00d09c
    3c54:	00011563 	.word	0x00011563
    3c58:	000104fe 	.word	0x000104fe
    3c5c:	000109f3 	.word	0x000109f3
    3c60:	000112b6 	.word	0x000112b6
    3c64:	0001074f 	.word	0x0001074f
    3c68:	00010a14 	.word	0x00010a14
    3c6c:	00010a52 	.word	0x00010a52
    3c70:	3c00d148 	.word	0x3c00d148

00003c74 <do_tt_crypto>:
    3c74:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3c78:	e3520001 	cmp	r2, #1	; 0x1
    3c7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    3c80:	d3a07000 	movle	r7, #0	; 0x0
    3c84:	da000009 	ble	3cb0 <do_tt_crypto+0x3c>
    3c88:	e5930004 	ldr	r0, [r3, #4]
    3c8c:	e3a01000 	mov	r1, #0	; 0x0
    3c90:	e3a02010 	mov	r2, #16	; 0x10
    3c94:	eb001d53 	bl	b1e8 <simple_strtoul>
    3c98:	e1a07000 	mov	r7, r0
    3c9c:	e1a01007 	mov	r1, r7
    3ca0:	e59f0114 	ldr	r0, [pc, #276]	; 3dbc <do_tt_crypto+0x148>
    3ca4:	e59f3114 	ldr	r3, [pc, #276]	; 3dc0 <do_tt_crypto+0x14c>
    3ca8:	e1a0e00f 	mov	lr, pc
    3cac:	e593f000 	ldr	pc, [r3]
    3cb0:	e3a00000 	mov	r0, #0	; 0x0
    3cb4:	eb0012b3 	bl	8788 <get_timer>
    3cb8:	e59f1104 	ldr	r1, [pc, #260]	; 3dc4 <do_tt_crypto+0x150>
    3cbc:	e3a02000 	mov	r2, #0	; 0x0
    3cc0:	e3a03000 	mov	r3, #0	; 0x0
    3cc4:	e881000c 	stm	r1, {r2, r3}
    3cc8:	e59f10f8 	ldr	r1, [pc, #248]	; 3dc8 <do_tt_crypto+0x154>
    3ccc:	e59fa0ec 	ldr	sl, [pc, #236]	; 3dc0 <do_tt_crypto+0x14c>
    3cd0:	e881000c 	stm	r1, {r2, r3}
    3cd4:	e1a08000 	mov	r8, r0
    3cd8:	e3a06000 	mov	r6, #0	; 0x0
    3cdc:	e3a01006 	mov	r1, #6	; 0x6
    3ce0:	e3a00000 	mov	r0, #0	; 0x0
    3ce4:	eb00015d 	bl	4260 <tt_rnd_get_int>
    3ce8:	e3a01002 	mov	r1, #2	; 0x2
    3cec:	e1a04000 	mov	r4, r0
    3cf0:	e3a00000 	mov	r0, #0	; 0x0
    3cf4:	eb000159 	bl	4260 <tt_rnd_get_int>
    3cf8:	e1a01006 	mov	r1, r6
    3cfc:	e1a03000 	mov	r3, r0
    3d00:	e1a05000 	mov	r5, r0
    3d04:	e1a02004 	mov	r2, r4
    3d08:	e59f00bc 	ldr	r0, [pc, #188]	; 3dcc <do_tt_crypto+0x158>
    3d0c:	e1a0e00f 	mov	lr, pc
    3d10:	e59af000 	ldr	pc, [sl]
    3d14:	e3540003 	cmp	r4, #3	; 0x3
    3d18:	9a000004 	bls	3d30 <do_tt_crypto+0xbc>
    3d1c:	e3550000 	cmp	r5, #0	; 0x0
    3d20:	11a00007 	movne	r0, r7
    3d24:	03a00000 	moveq	r0, #0	; 0x0
    3d28:	ebfffee9 	bl	38d4 <tt_crypto_aes>
    3d2c:	ea000003 	b	3d40 <do_tt_crypto+0xcc>
    3d30:	e3550000 	cmp	r5, #0	; 0x0
    3d34:	11a00007 	movne	r0, r7
    3d38:	03a00000 	moveq	r0, #0	; 0x0
    3d3c:	ebfffe4a 	bl	366c <tt_crypto_hash>
    3d40:	eb00213f 	bl	c244 <tstc>
    3d44:	e3500000 	cmp	r0, #0	; 0x0
    3d48:	e2866001 	add	r6, r6, #1	; 0x1
    3d4c:	0affffe2 	beq	3cdc <do_tt_crypto+0x68>
    3d50:	eb00213c 	bl	c248 <getc>
    3d54:	e3500003 	cmp	r0, #3	; 0x3
    3d58:	1affffdf 	bne	3cdc <do_tt_crypto+0x68>
    3d5c:	e1a00008 	mov	r0, r8
    3d60:	eb001288 	bl	8788 <get_timer>
    3d64:	e59f305c 	ldr	r3, [pc, #92]	; 3dc8 <do_tt_crypto+0x154>
    3d68:	e59f2054 	ldr	r2, [pc, #84]	; 3dc4 <do_tt_crypto+0x150>
    3d6c:	e5934000 	ldr	r4, [r3]
    3d70:	e5925000 	ldr	r5, [r2]
    3d74:	e5933004 	ldr	r3, [r3, #4]
    3d78:	e5922004 	ldr	r2, [r2, #4]
    3d7c:	e30e1a60 	movw	r1, #60000	; 0xea60
    3d80:	e1a04a24 	lsr	r4, r4, #20
    3d84:	e1a05a25 	lsr	r5, r5, #20
    3d88:	e1844603 	orr	r4, r4, r3, lsl #12
    3d8c:	e1855602 	orr	r5, r5, r2, lsl #12
    3d90:	eb002918 	bl	e1f8 <__aeabi_uidiv>
    3d94:	e1a01004 	mov	r1, r4
    3d98:	e1a03000 	mov	r3, r0
    3d9c:	e1a02005 	mov	r2, r5
    3da0:	e59f0028 	ldr	r0, [pc, #40]	; 3dd0 <do_tt_crypto+0x15c>
    3da4:	e59fc014 	ldr	ip, [pc, #20]	; 3dc0 <do_tt_crypto+0x14c>
    3da8:	e1a0e00f 	mov	lr, pc
    3dac:	e59cf000 	ldr	pc, [ip]
    3db0:	e3a00000 	mov	r0, #0	; 0x0
    3db4:	e28dd004 	add	sp, sp, #4	; 0x4
    3db8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    3dbc:	00010a83 	.word	0x00010a83
    3dc0:	3c00d68c 	.word	0x3c00d68c
    3dc4:	3c00d150 	.word	0x3c00d150
    3dc8:	3c00d148 	.word	0x3c00d148
    3dcc:	00010aa9 	.word	0x00010aa9
    3dd0:	00010ac8 	.word	0x00010ac8

00003dd4 <tt_ff_trolling>:
    3dd4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3dd8:	e3a01001 	mov	r1, #1	; 0x1
    3ddc:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    3de0:	e58d200c 	str	r2, [sp, #12]
    3de4:	e58d3008 	str	r3, [sp, #8]
    3de8:	e1a05000 	mov	r5, r0
    3dec:	eb0003bd 	bl	4ce8 <vs_assign_by_name>
    3df0:	e2506000 	subs	r6, r0, #0	; 0x0
    3df4:	0a000005 	beq	3e10 <tt_ff_trolling+0x3c>
    3df8:	e1a01005 	mov	r1, r5
    3dfc:	e59f0258 	ldr	r0, [pc, #600]	; 405c <tt_ff_trolling+0x288>
    3e00:	e59f3258 	ldr	r3, [pc, #600]	; 4060 <tt_ff_trolling+0x28c>
    3e04:	e1a0e00f 	mov	lr, pc
    3e08:	e593f000 	ldr	pc, [r3]
    3e0c:	ea00000c 	b	3e44 <tt_ff_trolling+0x70>
    3e10:	e3a0000e 	mov	r0, #14	; 0xe
    3e14:	eb000300 	bl	4a1c <vs_align>
    3e18:	e1a01000 	mov	r1, r0
    3e1c:	e28d0018 	add	r0, sp, #24	; 0x18
    3e20:	e5201004 	str	r1, [r0, #-4]!
    3e24:	e1a02001 	mov	r2, r1
    3e28:	eb000135 	bl	4304 <tt_rnd_get_data>
    3e2c:	e250a000 	subs	sl, r0, #0	; 0x0
    3e30:	e59f4228 	ldr	r4, [pc, #552]	; 4060 <tt_ff_trolling+0x28c>
    3e34:	1a000004 	bne	3e4c <tt_ff_trolling+0x78>
    3e38:	e59f0224 	ldr	r0, [pc, #548]	; 4064 <tt_ff_trolling+0x290>
    3e3c:	e1a0e00f 	mov	lr, pc
    3e40:	e594f000 	ldr	pc, [r4]
    3e44:	e3e00000 	mvn	r0, #0	; 0x0
    3e48:	ea000073 	b	401c <tt_ff_trolling+0x248>
    3e4c:	e59d2014 	ldr	r2, [sp, #20]
    3e50:	e1a0100a 	mov	r1, sl
    3e54:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3e58:	ebfff4da 	bl	11c8 <memcpy>
    3e5c:	e3a01501 	mov	r1, #4194304	; 0x400000
    3e60:	e59f0200 	ldr	r0, [pc, #512]	; 4068 <tt_ff_trolling+0x294>
    3e64:	e1a0e00f 	mov	lr, pc
    3e68:	e594f000 	ldr	pc, [r4]
    3e6c:	e1a00005 	mov	r0, r5
    3e70:	eb000388 	bl	4c98 <vs_device_id>
    3e74:	e3a04000 	mov	r4, #0	; 0x0
    3e78:	e3a05000 	mov	r5, #0	; 0x0
    3e7c:	e1a0b006 	mov	fp, r6
    3e80:	e3a08501 	mov	r8, #4194304	; 0x400000
    3e84:	e3a09000 	mov	r9, #0	; 0x0
    3e88:	e1a07000 	mov	r7, r0
    3e8c:	ea000021 	b	3f18 <tt_ff_trolling+0x144>
    3e90:	eb00036c 	bl	4c48 <vs_device_erasable>
    3e94:	e3500000 	cmp	r0, #0	; 0x0
    3e98:	0a000003 	beq	3eac <tt_ff_trolling+0xd8>
    3e9c:	e1a00004 	mov	r0, r4
    3ea0:	e1a01005 	mov	r1, r5
    3ea4:	e3a02501 	mov	r2, #4194304	; 0x400000
    3ea8:	eb00029f 	bl	492c <vs_erase>
    3eac:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3eb0:	e5816000 	str	r6, [r1]
    3eb4:	e1a00001 	mov	r0, r1
    3eb8:	e59d1014 	ldr	r1, [sp, #20]
    3ebc:	e1a02004 	mov	r2, r4
    3ec0:	e1a03005 	mov	r3, r5
    3ec4:	e88d0802 	stm	sp, {r1, fp}
    3ec8:	eb000281 	bl	48d4 <vs_write>
    3ecc:	e3500000 	cmp	r0, #0	; 0x0
    3ed0:	aa000006 	bge	3ef0 <tt_ff_trolling+0x11c>
    3ed4:	e1a02004 	mov	r2, r4
    3ed8:	e1a03005 	mov	r3, r5
    3edc:	e59f0188 	ldr	r0, [pc, #392]	; 406c <tt_ff_trolling+0x298>
    3ee0:	e59f1178 	ldr	r1, [pc, #376]	; 4060 <tt_ff_trolling+0x28c>
    3ee4:	e1a0e00f 	mov	lr, pc
    3ee8:	e591f000 	ldr	pc, [r1]
    3eec:	ea000012 	b	3f3c <tt_ff_trolling+0x168>
    3ef0:	e316001f 	tst	r6, #31	; 0x1f
    3ef4:	1a000004 	bne	3f0c <tt_ff_trolling+0x138>
    3ef8:	e1a01106 	lsl	r1, r6, #2
    3efc:	e59f016c 	ldr	r0, [pc, #364]	; 4070 <tt_ff_trolling+0x29c>
    3f00:	e59f3158 	ldr	r3, [pc, #344]	; 4060 <tt_ff_trolling+0x28c>
    3f04:	e1a0e00f 	mov	lr, pc
    3f08:	e593f000 	ldr	pc, [r3]
    3f0c:	e0944008 	adds	r4, r4, r8
    3f10:	e0a55009 	adc	r5, r5, r9
    3f14:	e2866001 	add	r6, r6, #1	; 0x1
    3f18:	e59d1008 	ldr	r1, [sp, #8]
    3f1c:	e1a00007 	mov	r0, r7
    3f20:	e1510005 	cmp	r1, r5
    3f24:	8affffd9 	bhi	3e90 <tt_ff_trolling+0xbc>
    3f28:	1a00003d 	bne	4024 <tt_ff_trolling+0x250>
    3f2c:	e59d300c 	ldr	r3, [sp, #12]
    3f30:	e1530004 	cmp	r3, r4
    3f34:	8affffd5 	bhi	3e90 <tt_ff_trolling+0xbc>
    3f38:	ea000039 	b	4024 <tt_ff_trolling+0x250>
    3f3c:	e59f411c 	ldr	r4, [pc, #284]	; 4060 <tt_ff_trolling+0x28c>
    3f40:	e59f012c 	ldr	r0, [pc, #300]	; 4074 <tt_ff_trolling+0x2a0>
    3f44:	e3a01501 	mov	r1, #4194304	; 0x400000
    3f48:	e3a06000 	mov	r6, #0	; 0x0
    3f4c:	e1a0e00f 	mov	lr, pc
    3f50:	e594f000 	ldr	pc, [r4]
    3f54:	e28a7004 	add	r7, sl, #4	; 0x4
    3f58:	e3a04000 	mov	r4, #0	; 0x0
    3f5c:	e3a05000 	mov	r5, #0	; 0x0
    3f60:	e1a0b006 	mov	fp, r6
    3f64:	e3a0a44e 	mov	sl, #1308622848	; 0x4e000000
    3f68:	e3a08501 	mov	r8, #4194304	; 0x400000
    3f6c:	e3a09000 	mov	r9, #0	; 0x0
    3f70:	ea00001e 	b	3ff0 <tt_ff_trolling+0x21c>
    3f74:	e59d1014 	ldr	r1, [sp, #20]
    3f78:	e88d0802 	stm	sp, {r1, fp}
    3f7c:	eb00039a 	bl	4dec <vs_read>
    3f80:	e316001f 	tst	r6, #31	; 0x1f
    3f84:	1a000004 	bne	3f9c <tt_ff_trolling+0x1c8>
    3f88:	e1a01106 	lsl	r1, r6, #2
    3f8c:	e59f00dc 	ldr	r0, [pc, #220]	; 4070 <tt_ff_trolling+0x29c>
    3f90:	e59f30c8 	ldr	r3, [pc, #200]	; 4060 <tt_ff_trolling+0x28c>
    3f94:	e1a0e00f 	mov	lr, pc
    3f98:	e593f000 	ldr	pc, [r3]
    3f9c:	e59a2000 	ldr	r2, [sl]
    3fa0:	e1a00007 	mov	r0, r7
    3fa4:	e1520006 	cmp	r2, r6
    3fa8:	e59f10c8 	ldr	r1, [pc, #200]	; 4078 <tt_ff_trolling+0x2a4>
    3fac:	e3a0300a 	mov	r3, #10	; 0xa
    3fb0:	e2866001 	add	r6, r6, #1	; 0x1
    3fb4:	1a000004 	bne	3fcc <tt_ff_trolling+0x1f8>
    3fb8:	e59d2014 	ldr	r2, [sp, #20]
    3fbc:	e2422004 	sub	r2, r2, #4	; 0x4
    3fc0:	eb000052 	bl	4110 <tt_cmp_buffer>
    3fc4:	e3500000 	cmp	r0, #0	; 0x0
    3fc8:	0a000006 	beq	3fe8 <tt_ff_trolling+0x214>
    3fcc:	e1a02004 	mov	r2, r4
    3fd0:	e1a03005 	mov	r3, r5
    3fd4:	e59f00a0 	ldr	r0, [pc, #160]	; 407c <tt_ff_trolling+0x2a8>
    3fd8:	e59f1080 	ldr	r1, [pc, #128]	; 4060 <tt_ff_trolling+0x28c>
    3fdc:	e1a0e00f 	mov	lr, pc
    3fe0:	e591f000 	ldr	pc, [r1]
    3fe4:	ea00001a 	b	4054 <tt_ff_trolling+0x280>
    3fe8:	e0944008 	adds	r4, r4, r8
    3fec:	e0a55009 	adc	r5, r5, r9
    3ff0:	e59d1008 	ldr	r1, [sp, #8]
    3ff4:	e1a02004 	mov	r2, r4
    3ff8:	e1510005 	cmp	r1, r5
    3ffc:	e1a03005 	mov	r3, r5
    4000:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4004:	8affffda 	bhi	3f74 <tt_ff_trolling+0x1a0>
    4008:	1a00000a 	bne	4038 <tt_ff_trolling+0x264>
    400c:	e59d100c 	ldr	r1, [sp, #12]
    4010:	e1510004 	cmp	r1, r4
    4014:	8affffd6 	bhi	3f74 <tt_ff_trolling+0x1a0>
    4018:	ea000006 	b	4038 <tt_ff_trolling+0x264>
    401c:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4020:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4024:	e59f0054 	ldr	r0, [pc, #84]	; 4080 <tt_ff_trolling+0x2ac>
    4028:	e59f3030 	ldr	r3, [pc, #48]	; 4060 <tt_ff_trolling+0x28c>
    402c:	e1a0e00f 	mov	lr, pc
    4030:	e593f000 	ldr	pc, [r3]
    4034:	eaffffc0 	b	3f3c <tt_ff_trolling+0x168>
    4038:	e59f0040 	ldr	r0, [pc, #64]	; 4080 <tt_ff_trolling+0x2ac>
    403c:	e59f401c 	ldr	r4, [pc, #28]	; 4060 <tt_ff_trolling+0x28c>
    4040:	e1a0e00f 	mov	lr, pc
    4044:	e594f000 	ldr	pc, [r4]
    4048:	e59f0034 	ldr	r0, [pc, #52]	; 4084 <tt_ff_trolling+0x2b0>
    404c:	e1a0e00f 	mov	lr, pc
    4050:	e594f000 	ldr	pc, [r4]
    4054:	e3a00000 	mov	r0, #0	; 0x0
    4058:	eaffffef 	b	401c <tt_ff_trolling+0x248>
    405c:	00010b4c 	.word	0x00010b4c
    4060:	3c00d68c 	.word	0x3c00d68c
    4064:	00010b69 	.word	0x00010b69
    4068:	00010b88 	.word	0x00010b88
    406c:	00010ba6 	.word	0x00010ba6
    4070:	00010bbe 	.word	0x00010bbe
    4074:	00010bc5 	.word	0x00010bc5
    4078:	4e000004 	.word	0x4e000004
    407c:	00010be5 	.word	0x00010be5
    4080:	00010bf5 	.word	0x00010bf5
    4084:	00010bfc 	.word	0x00010bfc

00004088 <do_tt_ff>:
    4088:	e92d4030 	push	{r4, r5, lr}
    408c:	e3520001 	cmp	r2, #1	; 0x1
    4090:	e24dd004 	sub	sp, sp, #4	; 0x4
    4094:	e1a05003 	mov	r5, r3
    4098:	ca000005 	bgt	40b4 <do_tt_ff+0x2c>
    409c:	e59f0060 	ldr	r0, [pc, #96]	; 4104 <do_tt_ff+0x7c>
    40a0:	e59f3060 	ldr	r3, [pc, #96]	; 4108 <do_tt_ff+0x80>
    40a4:	e1a0e00f 	mov	lr, pc
    40a8:	e593f000 	ldr	pc, [r3]
    40ac:	e3e00000 	mvn	r0, #0	; 0x0
    40b0:	ea000011 	b	40fc <do_tt_ff+0x74>
    40b4:	e3520002 	cmp	r2, #2	; 0x2
    40b8:	03a04000 	moveq	r4, #0	; 0x0
    40bc:	0a000009 	beq	40e8 <do_tt_ff+0x60>
    40c0:	e3a01000 	mov	r1, #0	; 0x0
    40c4:	e5930008 	ldr	r0, [r3, #8]
    40c8:	e3a0200a 	mov	r2, #10	; 0xa
    40cc:	eb001c45 	bl	b1e8 <simple_strtoul>
    40d0:	e1a04000 	mov	r4, r0
    40d4:	e1a01004 	mov	r1, r4
    40d8:	e59f002c 	ldr	r0, [pc, #44]	; 410c <do_tt_ff+0x84>
    40dc:	e59f3024 	ldr	r3, [pc, #36]	; 4108 <do_tt_ff+0x80>
    40e0:	e1a0e00f 	mov	lr, pc
    40e4:	e593f000 	ldr	pc, [r3]
    40e8:	e5950004 	ldr	r0, [r5, #4]
    40ec:	e1a03624 	lsr	r3, r4, #12
    40f0:	e1a02a04 	lsl	r2, r4, #20
    40f4:	ebffff36 	bl	3dd4 <tt_ff_trolling>
    40f8:	e3a00000 	mov	r0, #0	; 0x0
    40fc:	e28dd004 	add	sp, sp, #4	; 0x4
    4100:	e8bd8030 	pop	{r4, r5, pc}
    4104:	00010c14 	.word	0x00010c14
    4108:	3c00d68c 	.word	0x3c00d68c
    410c:	00010c5a 	.word	0x00010c5a

00004110 <tt_cmp_buffer>:
    4110:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4114:	e3a04000 	mov	r4, #0	; 0x0
    4118:	e24dd004 	sub	sp, sp, #4	; 0x4
    411c:	e1a0a002 	mov	sl, r2
    4120:	e1a07000 	mov	r7, r0
    4124:	e1a06001 	mov	r6, r1
    4128:	e1a08003 	mov	r8, r3
    412c:	e1a09142 	asr	r9, r2, #2
    4130:	e1a05004 	mov	r5, r4
    4134:	e59fb0e4 	ldr	fp, [pc, #228]	; 4220 <tt_cmp_buffer+0x110>
    4138:	ea00001f 	b	41bc <tt_cmp_buffer+0xac>
    413c:	e5970000 	ldr	r0, [r7]
    4140:	e5961000 	ldr	r1, [r6]
    4144:	e1500001 	cmp	r0, r1
    4148:	0a000018 	beq	41b0 <tt_cmp_buffer+0xa0>
    414c:	e5d72000 	ldrb	r2, [r7]
    4150:	e5d63000 	ldrb	r3, [r6]
    4154:	e1520003 	cmp	r2, r3
    4158:	e5d72001 	ldrb	r2, [r7, #1]
    415c:	e5d63001 	ldrb	r3, [r6, #1]
    4160:	12855001 	addne	r5, r5, #1	; 0x1
    4164:	e1520003 	cmp	r2, r3
    4168:	e5d72002 	ldrb	r2, [r7, #2]
    416c:	e5d63002 	ldrb	r3, [r6, #2]
    4170:	12855001 	addne	r5, r5, #1	; 0x1
    4174:	e1520003 	cmp	r2, r3
    4178:	e5d72003 	ldrb	r2, [r7, #3]
    417c:	e5d63003 	ldrb	r3, [r6, #3]
    4180:	12855001 	addne	r5, r5, #1	; 0x1
    4184:	e1520003 	cmp	r2, r3
    4188:	12855001 	addne	r5, r5, #1	; 0x1
    418c:	e3580000 	cmp	r8, #0	; 0x0
    4190:	0a000006 	beq	41b0 <tt_cmp_buffer+0xa0>
    4194:	e1a02000 	mov	r2, r0
    4198:	e1a03001 	mov	r3, r1
    419c:	e59f0080 	ldr	r0, [pc, #128]	; 4224 <tt_cmp_buffer+0x114>
    41a0:	e1a01104 	lsl	r1, r4, #2
    41a4:	e1a0e00f 	mov	lr, pc
    41a8:	e59bf000 	ldr	pc, [fp]
    41ac:	e2488001 	sub	r8, r8, #1	; 0x1
    41b0:	e2844001 	add	r4, r4, #1	; 0x1
    41b4:	e2877004 	add	r7, r7, #4	; 0x4
    41b8:	e2866004 	add	r6, r6, #4	; 0x4
    41bc:	e1540009 	cmp	r4, r9
    41c0:	baffffdd 	blt	413c <tt_cmp_buffer+0x2c>
    41c4:	e3ca9003 	bic	r9, sl, #3	; 0x3
    41c8:	e3a04000 	mov	r4, #0	; 0x0
    41cc:	e20aa003 	and	sl, sl, #3	; 0x3
    41d0:	e59fb048 	ldr	fp, [pc, #72]	; 4220 <tt_cmp_buffer+0x110>
    41d4:	ea00000c 	b	420c <tt_cmp_buffer+0xfc>
    41d8:	e7d72004 	ldrb	r2, [r7, r4]
    41dc:	e7d63004 	ldrb	r3, [r6, r4]
    41e0:	e1520003 	cmp	r2, r3
    41e4:	0a000007 	beq	4208 <tt_cmp_buffer+0xf8>
    41e8:	e3580000 	cmp	r8, #0	; 0x0
    41ec:	e2855001 	add	r5, r5, #1	; 0x1
    41f0:	0a000004 	beq	4208 <tt_cmp_buffer+0xf8>
    41f4:	e0841009 	add	r1, r4, r9
    41f8:	e59f0028 	ldr	r0, [pc, #40]	; 4228 <tt_cmp_buffer+0x118>
    41fc:	e1a0e00f 	mov	lr, pc
    4200:	e59bf000 	ldr	pc, [fp]
    4204:	e2488001 	sub	r8, r8, #1	; 0x1
    4208:	e2844001 	add	r4, r4, #1	; 0x1
    420c:	e154000a 	cmp	r4, sl
    4210:	bafffff0 	blt	41d8 <tt_cmp_buffer+0xc8>
    4214:	e1a00005 	mov	r0, r5
    4218:	e28dd004 	add	sp, sp, #4	; 0x4
    421c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4220:	3c00d68c 	.word	0x3c00d68c
    4224:	00010c81 	.word	0x00010c81
    4228:	00010cac 	.word	0x00010cac

0000422c <tt_rnd_seed>:
    422c:	e92d4030 	push	{r4, r5, lr}
    4230:	e59f5024 	ldr	r5, [pc, #36]	; 425c <tt_rnd_seed+0x30>
    4234:	e24dd004 	sub	sp, sp, #4	; 0x4
    4238:	e3a00000 	mov	r0, #0	; 0x0
    423c:	e5954000 	ldr	r4, [r5]
    4240:	eb001150 	bl	8788 <get_timer>
    4244:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    4248:	e6e40070 	uxtab	r0, r4, r0
    424c:	e0003003 	and	r3, r0, r3
    4250:	e5853000 	str	r3, [r5]
    4254:	e28dd004 	add	sp, sp, #4	; 0x4
    4258:	e8bd8030 	pop	{r4, r5, pc}
    425c:	3c00d158 	.word	0x3c00d158

00004260 <tt_rnd_get_int>:
    4260:	e59fc094 	ldr	ip, [pc, #148]	; 42fc <tt_rnd_get_int+0x9c>
    4264:	e3a02101 	mov	r2, #1073741824	; 0x40000000
    4268:	e59c3000 	ldr	r3, [ip]
    426c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4270:	e19330b2 	ldrh	r3, [r3, r2]
    4274:	e24dd004 	sub	sp, sp, #4	; 0x4
    4278:	e3c33003 	bic	r3, r3, #3	; 0x3
    427c:	e6ff3073 	uxth	r3, r3
    4280:	e19340b2 	ldrh	r4, [r3, r2]
    4284:	e58c3000 	str	r3, [ip]
    4288:	e1a07000 	mov	r7, r0
    428c:	e1a06001 	mov	r6, r1
    4290:	ebffffe5 	bl	422c <tt_rnd_seed>
    4294:	e1a00004 	mov	r0, r4
    4298:	eb0029a2 	bl	e928 <__aeabi_ui2d>
    429c:	e3a02000 	mov	r2, #0	; 0x0
    42a0:	e59f3058 	ldr	r3, [pc, #88]	; 4300 <tt_rnd_get_int+0xa0>
    42a4:	eb002a7a 	bl	ec94 <__aeabi_ddiv>
    42a8:	e1a04000 	mov	r4, r0
    42ac:	e0670006 	rsb	r0, r7, r6
    42b0:	e1a05001 	mov	r5, r1
    42b4:	eb0029a4 	bl	e94c <__aeabi_i2d>
    42b8:	e1a02000 	mov	r2, r0
    42bc:	e1a03001 	mov	r3, r1
    42c0:	e1a00004 	mov	r0, r4
    42c4:	e1a01005 	mov	r1, r5
    42c8:	eb0029d6 	bl	ea28 <__aeabi_dmul>
    42cc:	e1a04000 	mov	r4, r0
    42d0:	e1a00007 	mov	r0, r7
    42d4:	e1a05001 	mov	r5, r1
    42d8:	eb00299b 	bl	e94c <__aeabi_i2d>
    42dc:	e1a02000 	mov	r2, r0
    42e0:	e1a03001 	mov	r3, r1
    42e4:	e1a00004 	mov	r0, r4
    42e8:	e1a01005 	mov	r1, r5
    42ec:	eb0028e2 	bl	e67c <__adddf3>
    42f0:	eb002ae8 	bl	ee98 <__aeabi_d2iz>
    42f4:	e28dd004 	add	sp, sp, #4	; 0x4
    42f8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    42fc:	3c00d158 	.word	0x3c00d158
    4300:	40efffe0 	.word	0x40efffe0

00004304 <tt_rnd_get_data>:
    4304:	e92d4010 	push	{r4, lr}
    4308:	e1a04000 	mov	r4, r0
    430c:	e1a00001 	mov	r0, r1
    4310:	e1a01002 	mov	r1, r2
    4314:	ebffffd1 	bl	4260 <tt_rnd_get_int>
    4318:	e59f3010 	ldr	r3, [pc, #16]	; 4330 <tt_rnd_get_data+0x2c>
    431c:	e5840000 	str	r0, [r4]
    4320:	e5930000 	ldr	r0, [r3]
    4324:	e3c00003 	bic	r0, r0, #3	; 0x3
    4328:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    432c:	e8bd8010 	pop	{r4, pc}
    4330:	3c00d158 	.word	0x3c00d158

00004334 <tt_wire_get_opt>:
    4334:	e5900008 	ldr	r0, [r0, #8]
    4338:	e12fff1e 	bx	lr

0000433c <tt_wire_get_len>:
    433c:	e590000c 	ldr	r0, [r0, #12]
    4340:	e12fff1e 	bx	lr

00004344 <tt_wire_make_data>:
    4344:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4348:	e24dd004 	sub	sp, sp, #4	; 0x4
    434c:	e1a04000 	mov	r4, r0
    4350:	e1a07002 	mov	r7, r2
    4354:	e1a06003 	mov	r6, r3
    4358:	ebfff39a 	bl	11c8 <memcpy>
    435c:	e59f3040 	ldr	r3, [pc, #64]	; 43a4 <tt_wire_make_data+0x60>
    4360:	e5846008 	str	r6, [r4, #8]
    4364:	e5843000 	str	r3, [r4]
    4368:	e59d3018 	ldr	r3, [sp, #24]
    436c:	e3a05000 	mov	r5, #0	; 0x0
    4370:	e584300c 	str	r3, [r4, #12]
    4374:	e1a01007 	mov	r1, r7
    4378:	e5845004 	str	r5, [r4, #4]
    437c:	e1a00005 	mov	r0, r5
    4380:	eb000469 	bl	552c <hash_init>
    4384:	e1a01007 	mov	r1, r7
    4388:	e1a00004 	mov	r0, r4
    438c:	eb000441 	bl	5498 <hash_data>
    4390:	e2840004 	add	r0, r4, #4	; 0x4
    4394:	eb000423 	bl	5428 <hash_value>
    4398:	e1a00005 	mov	r0, r5
    439c:	e28dd004 	add	sp, sp, #4	; 0x4
    43a0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    43a4:	7766dd80 	.word	0x7766dd80

000043a8 <tt_wire_check_valid>:
    43a8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    43ac:	e5903000 	ldr	r3, [r0]
    43b0:	e59f2084 	ldr	r2, [pc, #132]	; 443c <tt_wire_check_valid+0x94>
    43b4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    43b8:	e1530002 	cmp	r3, r2
    43bc:	e1a04000 	mov	r4, r0
    43c0:	e1a06001 	mov	r6, r1
    43c4:	e5907004 	ldr	r7, [r0, #4]
    43c8:	0a000006 	beq	43e8 <tt_wire_check_valid+0x40>
    43cc:	e1a01003 	mov	r1, r3
    43d0:	e59f0068 	ldr	r0, [pc, #104]	; 4440 <tt_wire_check_valid+0x98>
    43d4:	e59f3068 	ldr	r3, [pc, #104]	; 4444 <tt_wire_check_valid+0x9c>
    43d8:	e1a0e00f 	mov	lr, pc
    43dc:	e593f000 	ldr	pc, [r3]
    43e0:	e3a00000 	mov	r0, #0	; 0x0
    43e4:	ea000012 	b	4434 <tt_wire_check_valid+0x8c>
    43e8:	e3a05000 	mov	r5, #0	; 0x0
    43ec:	e5805004 	str	r5, [r0, #4]
    43f0:	e1a00005 	mov	r0, r5
    43f4:	eb00044c 	bl	552c <hash_init>
    43f8:	e1a01006 	mov	r1, r6
    43fc:	e1a00004 	mov	r0, r4
    4400:	eb000424 	bl	5498 <hash_data>
    4404:	e28d0004 	add	r0, sp, #4	; 0x4
    4408:	eb000406 	bl	5428 <hash_value>
    440c:	e59d1004 	ldr	r1, [sp, #4]
    4410:	e1510007 	cmp	r1, r7
    4414:	03a00001 	moveq	r0, #1	; 0x1
    4418:	0a000005 	beq	4434 <tt_wire_check_valid+0x8c>
    441c:	e59f0024 	ldr	r0, [pc, #36]	; 4448 <tt_wire_check_valid+0xa0>
    4420:	e1a02007 	mov	r2, r7
    4424:	e59f3018 	ldr	r3, [pc, #24]	; 4444 <tt_wire_check_valid+0x9c>
    4428:	e1a0e00f 	mov	lr, pc
    442c:	e593f000 	ldr	pc, [r3]
    4430:	e1a00005 	mov	r0, r5
    4434:	e28dd00c 	add	sp, sp, #12	; 0xc
    4438:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    443c:	7766dd80 	.word	0x7766dd80
    4440:	00010cd7 	.word	0x00010cd7
    4444:	3c00d68c 	.word	0x3c00d68c
    4448:	00010d00 	.word	0x00010d00

0000444c <do_tt_wire>:
    444c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4450:	e3520001 	cmp	r2, #1	; 0x1
    4454:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    4458:	e1a04003 	mov	r4, r3
    445c:	d59f032c 	ldrle	r0, [pc, #812]	; 4790 <do_tt_wire+0x344>
    4460:	da00002b 	ble	4514 <do_tt_wire+0xc8>
    4464:	e3a02003 	mov	r2, #3	; 0x3
    4468:	e5930004 	ldr	r0, [r3, #4]
    446c:	e59f1320 	ldr	r1, [pc, #800]	; 4794 <do_tt_wire+0x348>
    4470:	ebfff31b 	bl	10e4 <strncmp>
    4474:	e3500000 	cmp	r0, #0	; 0x0
    4478:	059f2318 	ldreq	r2, [pc, #792]	; 4798 <do_tt_wire+0x34c>
    447c:	0a000006 	beq	449c <do_tt_wire+0x50>
    4480:	e5940004 	ldr	r0, [r4, #4]
    4484:	e59f1310 	ldr	r1, [pc, #784]	; 479c <do_tt_wire+0x350>
    4488:	e3a02003 	mov	r2, #3	; 0x3
    448c:	ebfff314 	bl	10e4 <strncmp>
    4490:	e3500000 	cmp	r0, #0	; 0x0
    4494:	1a000009 	bne	44c0 <do_tt_wire+0x74>
    4498:	e59f2300 	ldr	r2, [pc, #768]	; 47a0 <do_tt_wire+0x354>
    449c:	e59f3300 	ldr	r3, [pc, #768]	; 47a4 <do_tt_wire+0x358>
    44a0:	e3a00000 	mov	r0, #0	; 0x0
    44a4:	e5832000 	str	r2, [r3]
    44a8:	eb0010b6 	bl	8788 <get_timer>
    44ac:	e59f82f4 	ldr	r8, [pc, #756]	; 47a8 <do_tt_wire+0x35c>
    44b0:	e59f92ec 	ldr	r9, [pc, #748]	; 47a4 <do_tt_wire+0x358>
    44b4:	e1a0a008 	mov	sl, r8
    44b8:	e58d000c 	str	r0, [sp, #12]
    44bc:	ea000006 	b	44dc <do_tt_wire+0x90>
    44c0:	e5941004 	ldr	r1, [r4, #4]
    44c4:	e59f02e0 	ldr	r0, [pc, #736]	; 47ac <do_tt_wire+0x360>
    44c8:	e59f32d8 	ldr	r3, [pc, #728]	; 47a8 <do_tt_wire+0x35c>
    44cc:	e1a0e00f 	mov	lr, pc
    44d0:	e593f000 	ldr	pc, [r3]
    44d4:	e3e00000 	mvn	r0, #0	; 0x0
    44d8:	ea0000aa 	b	4788 <do_tt_wire+0x33c>
    44dc:	e59f02cc 	ldr	r0, [pc, #716]	; 47b0 <do_tt_wire+0x364>
    44e0:	eb001084 	bl	86f8 <udelay>
    44e4:	e59f02c8 	ldr	r0, [pc, #712]	; 47b4 <do_tt_wire+0x368>
    44e8:	e1a0e00f 	mov	lr, pc
    44ec:	e598f000 	ldr	pc, [r8]
    44f0:	e5993000 	ldr	r3, [r9]
    44f4:	e1a0e00f 	mov	lr, pc
    44f8:	e593f000 	ldr	pc, [r3]
    44fc:	e3500000 	cmp	r0, #0	; 0x0
    4500:	a3a07000 	movge	r7, #0	; 0x0
    4504:	a3a06020 	movge	r6, #32	; 0x20
    4508:	a59fb2a8 	ldrge	fp, [pc, #680]	; 47b8 <do_tt_wire+0x36c>
    450c:	aa000004 	bge	4524 <do_tt_wire+0xd8>
    4510:	e59f02a4 	ldr	r0, [pc, #676]	; 47bc <do_tt_wire+0x370>
    4514:	e59f328c 	ldr	r3, [pc, #652]	; 47a8 <do_tt_wire+0x35c>
    4518:	e1a0e00f 	mov	lr, pc
    451c:	e593f000 	ldr	pc, [r3]
    4520:	eaffffeb 	b	44d4 <do_tt_wire+0x88>
    4524:	e3570000 	cmp	r7, #0	; 0x0
    4528:	e1a01006 	mov	r1, r6
    452c:	e59f028c 	ldr	r0, [pc, #652]	; 47c0 <do_tt_wire+0x374>
    4530:	1a00002f 	bne	45f4 <do_tt_wire+0x1a8>
    4534:	e59f0288 	ldr	r0, [pc, #648]	; 47c4 <do_tt_wire+0x378>
    4538:	e1a0e00f 	mov	lr, pc
    453c:	e598f000 	ldr	pc, [r8]
    4540:	e1a00007 	mov	r0, r7
    4544:	eb00108f 	bl	8788 <get_timer>
    4548:	e1a01006 	mov	r1, r6
    454c:	e1a04000 	mov	r4, r0
    4550:	e5993000 	ldr	r3, [r9]
    4554:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4558:	e1a0e00f 	mov	lr, pc
    455c:	e593f004 	ldr	pc, [r3, #4]
    4560:	e1a05000 	mov	r5, r0
    4564:	e1a00004 	mov	r0, r4
    4568:	eb001086 	bl	8788 <get_timer>
    456c:	e3550000 	cmp	r5, #0	; 0x0
    4570:	e1a04000 	mov	r4, r0
    4574:	e1a01006 	mov	r1, r6
    4578:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    457c:	ba00003d 	blt	4678 <do_tt_wire+0x22c>
    4580:	ebffff88 	bl	43a8 <tt_wire_check_valid>
    4584:	e3500000 	cmp	r0, #0	; 0x0
    4588:	e1a01004 	mov	r1, r4
    458c:	e1a03546 	asr	r3, r6, #10
    4590:	1a000007 	bne	45b4 <do_tt_wire+0x168>
    4594:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    4598:	e0000390 	mul	r0, r0, r3
    459c:	eb002797 	bl	e400 <__aeabi_idiv>
    45a0:	e1a01000 	mov	r1, r0
    45a4:	e59f021c 	ldr	r0, [pc, #540]	; 47c8 <do_tt_wire+0x37c>
    45a8:	e1a0e00f 	mov	lr, pc
    45ac:	e59af000 	ldr	pc, [sl]
    45b0:	eaffffc9 	b	44dc <do_tt_wire+0x90>
    45b4:	e59fc210 	ldr	ip, [pc, #528]	; 47cc <do_tt_wire+0x380>
    45b8:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    45bc:	e0000392 	mul	r0, r2, r3
    45c0:	e89c000c 	ldm	ip, {r2, r3}
    45c4:	e0922006 	adds	r2, r2, r6
    45c8:	e0a33fc6 	adc	r3, r3, r6, asr #31
    45cc:	e88c000c 	stm	ip, {r2, r3}
    45d0:	eb00278a 	bl	e400 <__aeabi_idiv>
    45d4:	e1a01000 	mov	r1, r0
    45d8:	e59f01f0 	ldr	r0, [pc, #496]	; 47d0 <do_tt_wire+0x384>
    45dc:	e1a0e00f 	mov	lr, pc
    45e0:	e598f000 	ldr	pc, [r8]
    45e4:	e3a0344e 	mov	r3, #1308622848	; 0x4e000000
    45e8:	e5937008 	ldr	r7, [r3, #8]
    45ec:	e593600c 	ldr	r6, [r3, #12]
    45f0:	ea000044 	b	4708 <do_tt_wire+0x2bc>
    45f4:	e3570001 	cmp	r7, #1	; 0x1
    45f8:	1a000030 	bne	46c0 <do_tt_wire+0x274>
    45fc:	e1a0e00f 	mov	lr, pc
    4600:	e598f000 	ldr	pc, [r8]
    4604:	e1a01007 	mov	r1, r7
    4608:	e3a00000 	mov	r0, #0	; 0x0
    460c:	ebffff13 	bl	4260 <tt_rnd_get_int>
    4610:	e3a01020 	mov	r1, #32	; 0x20
    4614:	e1a07000 	mov	r7, r0
    4618:	e3a02502 	mov	r2, #8388608	; 0x800000
    461c:	e28d0014 	add	r0, sp, #20	; 0x14
    4620:	ebffff37 	bl	4304 <tt_rnd_get_data>
    4624:	e59dc014 	ldr	ip, [sp, #20]
    4628:	e1a01000 	mov	r1, r0
    462c:	e1a02006 	mov	r2, r6
    4630:	e1a03007 	mov	r3, r7
    4634:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4638:	e58dc000 	str	ip, [sp]
    463c:	ebffff40 	bl	4344 <tt_wire_make_data>
    4640:	e3a00000 	mov	r0, #0	; 0x0
    4644:	eb00104f 	bl	8788 <get_timer>
    4648:	e1a01006 	mov	r1, r6
    464c:	e1a05000 	mov	r5, r0
    4650:	e5993000 	ldr	r3, [r9]
    4654:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4658:	e1a0e00f 	mov	lr, pc
    465c:	e593f008 	ldr	pc, [r3, #8]
    4660:	e1a04000 	mov	r4, r0
    4664:	e1a00005 	mov	r0, r5
    4668:	eb001046 	bl	8788 <get_timer>
    466c:	e3540000 	cmp	r4, #0	; 0x0
    4670:	e1a01000 	mov	r1, r0
    4674:	aa000003 	bge	4688 <do_tt_wire+0x23c>
    4678:	e59f0154 	ldr	r0, [pc, #340]	; 47d4 <do_tt_wire+0x388>
    467c:	e1a0e00f 	mov	lr, pc
    4680:	e59af000 	ldr	pc, [sl]
    4684:	eaffff94 	b	44dc <do_tt_wire+0x90>
    4688:	e89b000c 	ldm	fp, {r2, r3}
    468c:	e0922006 	adds	r2, r2, r6
    4690:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4694:	e59d6014 	ldr	r6, [sp, #20]
    4698:	e88b000c 	stm	fp, {r2, r3}
    469c:	e1a00546 	asr	r0, r6, #10
    46a0:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    46a4:	e0000092 	mul	r0, r2, r0
    46a8:	eb002754 	bl	e400 <__aeabi_idiv>
    46ac:	e1a01000 	mov	r1, r0
    46b0:	e59f0120 	ldr	r0, [pc, #288]	; 47d8 <do_tt_wire+0x38c>
    46b4:	e1a0e00f 	mov	lr, pc
    46b8:	e598f000 	ldr	pc, [r8]
    46bc:	ea000011 	b	4708 <do_tt_wire+0x2bc>
    46c0:	e3570003 	cmp	r7, #3	; 0x3
    46c4:	1a000004 	bne	46dc <do_tt_wire+0x290>
    46c8:	e59f010c 	ldr	r0, [pc, #268]	; 47dc <do_tt_wire+0x390>
    46cc:	e59f30d4 	ldr	r3, [pc, #212]	; 47a8 <do_tt_wire+0x35c>
    46d0:	e1a0e00f 	mov	lr, pc
    46d4:	e593f000 	ldr	pc, [r3]
    46d8:	ea000010 	b	4720 <do_tt_wire+0x2d4>
    46dc:	e3570002 	cmp	r7, #2	; 0x2
    46e0:	1a000003 	bne	46f4 <do_tt_wire+0x2a8>
    46e4:	e59f00f4 	ldr	r0, [pc, #244]	; 47e0 <do_tt_wire+0x394>
    46e8:	e1a0e00f 	mov	lr, pc
    46ec:	e598f000 	ldr	pc, [r8]
    46f0:	eaffff79 	b	44dc <do_tt_wire+0x90>
    46f4:	e59f00e8 	ldr	r0, [pc, #232]	; 47e4 <do_tt_wire+0x398>
    46f8:	e59f30a8 	ldr	r3, [pc, #168]	; 47a8 <do_tt_wire+0x35c>
    46fc:	e1a0e00f 	mov	lr, pc
    4700:	e593f000 	ldr	pc, [r3]
    4704:	eafffffe 	b	4704 <do_tt_wire+0x2b8>
    4708:	eb001ecd 	bl	c244 <tstc>
    470c:	e3500000 	cmp	r0, #0	; 0x0
    4710:	0affff83 	beq	4524 <do_tt_wire+0xd8>
    4714:	eb001ecb 	bl	c248 <getc>
    4718:	e3500003 	cmp	r0, #3	; 0x3
    471c:	1affff80 	bne	4524 <do_tt_wire+0xd8>
    4720:	e59d000c 	ldr	r0, [sp, #12]
    4724:	eb001017 	bl	8788 <get_timer>
    4728:	e59f3088 	ldr	r3, [pc, #136]	; 47b8 <do_tt_wire+0x36c>
    472c:	e59f4074 	ldr	r4, [pc, #116]	; 47a8 <do_tt_wire+0x35c>
    4730:	e893000a 	ldm	r3, {r1, r3}
    4734:	e1a01a21 	lsr	r1, r1, #20
    4738:	e1811603 	orr	r1, r1, r3, lsl #12
    473c:	e1a05000 	mov	r5, r0
    4740:	e59f00a0 	ldr	r0, [pc, #160]	; 47e8 <do_tt_wire+0x39c>
    4744:	e1a0e00f 	mov	lr, pc
    4748:	e594f000 	ldr	pc, [r4]
    474c:	e59f3078 	ldr	r3, [pc, #120]	; 47cc <do_tt_wire+0x380>
    4750:	e59f0094 	ldr	r0, [pc, #148]	; 47ec <do_tt_wire+0x3a0>
    4754:	e893000a 	ldm	r3, {r1, r3}
    4758:	e1a01a21 	lsr	r1, r1, #20
    475c:	e1811603 	orr	r1, r1, r3, lsl #12
    4760:	e1a0e00f 	mov	lr, pc
    4764:	e594f000 	ldr	pc, [r4]
    4768:	e30e1a60 	movw	r1, #60000	; 0xea60
    476c:	e1a00005 	mov	r0, r5
    4770:	eb002722 	bl	e400 <__aeabi_idiv>
    4774:	e1a01000 	mov	r1, r0
    4778:	e59f0070 	ldr	r0, [pc, #112]	; 47f0 <do_tt_wire+0x3a4>
    477c:	e1a0e00f 	mov	lr, pc
    4780:	e594f000 	ldr	pc, [r4]
    4784:	e3a00000 	mov	r0, #0	; 0x0
    4788:	e28dd01c 	add	sp, sp, #28	; 0x1c
    478c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4790:	00010d29 	.word	0x00010d29
    4794:	00010d72 	.word	0x00010d72
    4798:	0000efc4 	.word	0x0000efc4
    479c:	00010d76 	.word	0x00010d76
    47a0:	0000efdc 	.word	0x0000efdc
    47a4:	3c00d170 	.word	0x3c00d170
    47a8:	3c00d68c 	.word	0x3c00d68c
    47ac:	00010d7a 	.word	0x00010d7a
    47b0:	001e8480 	.word	0x001e8480
    47b4:	00010d9a 	.word	0x00010d9a
    47b8:	3c00d168 	.word	0x3c00d168
    47bc:	00010dbc 	.word	0x00010dbc
    47c0:	00010e29 	.word	0x00010e29
    47c4:	00010ddf 	.word	0x00010ddf
    47c8:	00010e0b 	.word	0x00010e0b
    47cc:	3c00d160 	.word	0x3c00d160
    47d0:	00010e1a 	.word	0x00010e1a
    47d4:	00010dfb 	.word	0x00010dfb
    47d8:	00010e45 	.word	0x00010e45
    47dc:	00010e54 	.word	0x00010e54
    47e0:	00010e66 	.word	0x00010e66
    47e4:	00010e7f 	.word	0x00010e7f
    47e8:	00010eac 	.word	0x00010eac
    47ec:	00010ec6 	.word	0x00010ec6
    47f0:	00010ee4 	.word	0x00010ee4

000047f4 <vs_is_assigned>:
    47f4:	e59f300c 	ldr	r3, [pc, #12]	; 4808 <vs_is_assigned+0x14>
    47f8:	e5930000 	ldr	r0, [r3]
    47fc:	e2500000 	subs	r0, r0, #0	; 0x0
    4800:	13a00001 	movne	r0, #1	; 0x1
    4804:	e12fff1e 	bx	lr
    4808:	3c00d178 	.word	0x3c00d178

0000480c <vs_is_opt_available>:
    480c:	e350000d 	cmp	r0, #13	; 0xd
    4810:	959f2078 	ldrls	r2, [pc, #120]	; 4890 <vs_is_opt_available+0x84>
    4814:	93a0302c 	movls	r3, #44	; 0x2c
    4818:	859f3074 	ldrhi	r3, [pc, #116]	; 4894 <vs_is_opt_available+0x88>
    481c:	90202093 	mlals	r0, r3, r0, r2
    4820:	85930000 	ldrhi	r0, [r3]
    4824:	e3500000 	cmp	r0, #0	; 0x0
    4828:	0a000016 	beq	4888 <vs_is_opt_available+0x7c>
    482c:	e3510005 	cmp	r1, #5	; 0x5
    4830:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    4834:	ea000013 	b	4888 <vs_is_opt_available+0x7c>
    4838:	00004850 	.word	0x00004850
    483c:	00004858 	.word	0x00004858
    4840:	00004860 	.word	0x00004860
    4844:	00004868 	.word	0x00004868
    4848:	00004870 	.word	0x00004870
    484c:	00004878 	.word	0x00004878
    4850:	e590300c 	ldr	r3, [r0, #12]
    4854:	ea000008 	b	487c <vs_is_opt_available+0x70>
    4858:	e5903010 	ldr	r3, [r0, #16]
    485c:	ea000006 	b	487c <vs_is_opt_available+0x70>
    4860:	e5903014 	ldr	r3, [r0, #20]
    4864:	ea000004 	b	487c <vs_is_opt_available+0x70>
    4868:	e5903018 	ldr	r3, [r0, #24]
    486c:	ea000002 	b	487c <vs_is_opt_available+0x70>
    4870:	e590301c 	ldr	r3, [r0, #28]
    4874:	ea000000 	b	487c <vs_is_opt_available+0x70>
    4878:	e5903020 	ldr	r3, [r0, #32]
    487c:	e2530000 	subs	r0, r3, #0	; 0x0
    4880:	13a00001 	movne	r0, #1	; 0x1
    4884:	e12fff1e 	bx	lr
    4888:	e3a00000 	mov	r0, #0	; 0x0
    488c:	e12fff1e 	bx	lr
    4890:	0000efdc 	.word	0x0000efdc
    4894:	3c00d178 	.word	0x3c00d178

00004898 <vs_reset>:
    4898:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    489c:	e59f302c 	ldr	r3, [pc, #44]	; 48d0 <vs_reset+0x38>
    48a0:	e24dd004 	sub	sp, sp, #4	; 0x4
    48a4:	e5933000 	ldr	r3, [r3]
    48a8:	e3530000 	cmp	r3, #0	; 0x0
    48ac:	03e00001 	mvneq	r0, #1	; 0x1
    48b0:	0a000004 	beq	48c8 <vs_reset+0x30>
    48b4:	e5933018 	ldr	r3, [r3, #24]
    48b8:	e3530000 	cmp	r3, #0	; 0x0
    48bc:	03e00002 	mvneq	r0, #2	; 0x2
    48c0:	0a000000 	beq	48c8 <vs_reset+0x30>
    48c4:	e12fff33 	blx	r3
    48c8:	e28dd004 	add	sp, sp, #4	; 0x4
    48cc:	e8bd8000 	pop	{pc}
    48d0:	3c00d178 	.word	0x3c00d178

000048d4 <vs_write>:
    48d4:	e92d4010 	push	{r4, lr}
    48d8:	e24dd008 	sub	sp, sp, #8	; 0x8
    48dc:	e59d4010 	ldr	r4, [sp, #16]
    48e0:	e3540000 	cmp	r4, #0	; 0x0
    48e4:	01a00004 	moveq	r0, r4
    48e8:	0a00000c 	beq	4920 <vs_write+0x4c>
    48ec:	e59f1034 	ldr	r1, [pc, #52]	; 4928 <vs_write+0x54>
    48f0:	e5911000 	ldr	r1, [r1]
    48f4:	e3510000 	cmp	r1, #0	; 0x0
    48f8:	03e00001 	mvneq	r0, #1	; 0x1
    48fc:	0a000007 	beq	4920 <vs_write+0x4c>
    4900:	e591c014 	ldr	ip, [r1, #20]
    4904:	e35c0000 	cmp	ip, #0	; 0x0
    4908:	03e00002 	mvneq	r0, #2	; 0x2
    490c:	0a000003 	beq	4920 <vs_write+0x4c>
    4910:	e59d1014 	ldr	r1, [sp, #20]
    4914:	e58d4000 	str	r4, [sp]
    4918:	e58d1004 	str	r1, [sp, #4]
    491c:	e12fff3c 	blx	ip
    4920:	e28dd008 	add	sp, sp, #8	; 0x8
    4924:	e8bd8010 	pop	{r4, pc}
    4928:	3c00d178 	.word	0x3c00d178

0000492c <vs_erase>:
    492c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4930:	e59f302c 	ldr	r3, [pc, #44]	; 4964 <vs_erase+0x38>
    4934:	e24dd004 	sub	sp, sp, #4	; 0x4
    4938:	e5933000 	ldr	r3, [r3]
    493c:	e3530000 	cmp	r3, #0	; 0x0
    4940:	03e00001 	mvneq	r0, #1	; 0x1
    4944:	0a000004 	beq	495c <vs_erase+0x30>
    4948:	e593301c 	ldr	r3, [r3, #28]
    494c:	e3530000 	cmp	r3, #0	; 0x0
    4950:	03e00002 	mvneq	r0, #2	; 0x2
    4954:	0a000000 	beq	495c <vs_erase+0x30>
    4958:	e12fff33 	blx	r3
    495c:	e28dd004 	add	sp, sp, #4	; 0x4
    4960:	e8bd8000 	pop	{pc}
    4964:	3c00d178 	.word	0x3c00d178

00004968 <vs_isbad>:
    4968:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    496c:	e59f302c 	ldr	r3, [pc, #44]	; 49a0 <vs_isbad+0x38>
    4970:	e24dd004 	sub	sp, sp, #4	; 0x4
    4974:	e5933000 	ldr	r3, [r3]
    4978:	e3530000 	cmp	r3, #0	; 0x0
    497c:	03e00001 	mvneq	r0, #1	; 0x1
    4980:	0a000004 	beq	4998 <vs_isbad+0x30>
    4984:	e5933024 	ldr	r3, [r3, #36]
    4988:	e3530000 	cmp	r3, #0	; 0x0
    498c:	03e00002 	mvneq	r0, #2	; 0x2
    4990:	0a000000 	beq	4998 <vs_isbad+0x30>
    4994:	e12fff33 	blx	r3
    4998:	e28dd004 	add	sp, sp, #4	; 0x4
    499c:	e8bd8000 	pop	{pc}
    49a0:	3c00d178 	.word	0x3c00d178

000049a4 <vs_scrub>:
    49a4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    49a8:	e59f302c 	ldr	r3, [pc, #44]	; 49dc <vs_scrub+0x38>
    49ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    49b0:	e5933000 	ldr	r3, [r3]
    49b4:	e3530000 	cmp	r3, #0	; 0x0
    49b8:	03e00001 	mvneq	r0, #1	; 0x1
    49bc:	0a000004 	beq	49d4 <vs_scrub+0x30>
    49c0:	e5933020 	ldr	r3, [r3, #32]
    49c4:	e3530000 	cmp	r3, #0	; 0x0
    49c8:	03e00002 	mvneq	r0, #2	; 0x2
    49cc:	0a000000 	beq	49d4 <vs_scrub+0x30>
    49d0:	e12fff33 	blx	r3
    49d4:	e28dd004 	add	sp, sp, #4	; 0x4
    49d8:	e8bd8000 	pop	{pc}
    49dc:	3c00d178 	.word	0x3c00d178

000049e0 <vs_special>:
    49e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    49e4:	e59f302c 	ldr	r3, [pc, #44]	; 4a18 <vs_special+0x38>
    49e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    49ec:	e5933000 	ldr	r3, [r3]
    49f0:	e3530000 	cmp	r3, #0	; 0x0
    49f4:	03e00001 	mvneq	r0, #1	; 0x1
    49f8:	0a000004 	beq	4a10 <vs_special+0x30>
    49fc:	e5933028 	ldr	r3, [r3, #40]
    4a00:	e3530000 	cmp	r3, #0	; 0x0
    4a04:	03e00002 	mvneq	r0, #2	; 0x2
    4a08:	0a000000 	beq	4a10 <vs_special+0x30>
    4a0c:	e12fff33 	blx	r3
    4a10:	e28dd004 	add	sp, sp, #4	; 0x4
    4a14:	e8bd8000 	pop	{pc}
    4a18:	3c00d178 	.word	0x3c00d178

00004a1c <vs_align>:
    4a1c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4a20:	e350000e 	cmp	r0, #14	; 0xe
    4a24:	059f3044 	ldreq	r3, [pc, #68]	; 4a70 <vs_align+0x54>
    4a28:	e24dd004 	sub	sp, sp, #4	; 0x4
    4a2c:	05930000 	ldreq	r0, [r3]
    4a30:	0a000004 	beq	4a48 <vs_align+0x2c>
    4a34:	e350000d 	cmp	r0, #13	; 0xd
    4a38:	8a000009 	bhi	4a64 <vs_align+0x48>
    4a3c:	e59f2030 	ldr	r2, [pc, #48]	; 4a74 <vs_align+0x58>
    4a40:	e3a0302c 	mov	r3, #44	; 0x2c
    4a44:	e0202093 	mla	r0, r3, r0, r2
    4a48:	e3500000 	cmp	r0, #0	; 0x0
    4a4c:	0a000004 	beq	4a64 <vs_align+0x48>
    4a50:	e590000c 	ldr	r0, [r0, #12]
    4a54:	e3500000 	cmp	r0, #0	; 0x0
    4a58:	0a000001 	beq	4a64 <vs_align+0x48>
    4a5c:	e12fff30 	blx	r0
    4a60:	ea000000 	b	4a68 <vs_align+0x4c>
    4a64:	e3a00000 	mov	r0, #0	; 0x0
    4a68:	e28dd004 	add	sp, sp, #4	; 0x4
    4a6c:	e8bd8000 	pop	{pc}
    4a70:	3c00d178 	.word	0x3c00d178
    4a74:	0000efdc 	.word	0x0000efdc

00004a78 <vs_assign_by_id>:
    4a78:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4a7c:	e59fc05c 	ldr	ip, [pc, #92]	; 4ae0 <vs_assign_by_id+0x68>
    4a80:	e3a03000 	mov	r3, #0	; 0x0
    4a84:	e350000d 	cmp	r0, #13	; 0xd
    4a88:	e24dd004 	sub	sp, sp, #4	; 0x4
    4a8c:	e58c3000 	str	r3, [ip]
    4a90:	8a00000b 	bhi	4ac4 <vs_assign_by_id+0x4c>
    4a94:	e59f2048 	ldr	r2, [pc, #72]	; 4ae4 <vs_assign_by_id+0x6c>
    4a98:	e283302c 	add	r3, r3, #44	; 0x2c
    4a9c:	e0232093 	mla	r3, r3, r0, r2
    4aa0:	e3510000 	cmp	r1, #0	; 0x0
    4aa4:	e58c3000 	str	r3, [ip]
    4aa8:	01a00001 	moveq	r0, r1
    4aac:	0a000002 	beq	4abc <vs_assign_by_id+0x44>
    4ab0:	e28dd004 	add	sp, sp, #4	; 0x4
    4ab4:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    4ab8:	eaffff76 	b	4898 <vs_reset>
    4abc:	e28dd004 	add	sp, sp, #4	; 0x4
    4ac0:	e8bd8000 	pop	{pc}
    4ac4:	e1a01000 	mov	r1, r0
    4ac8:	e59f3018 	ldr	r3, [pc, #24]	; 4ae8 <vs_assign_by_id+0x70>
    4acc:	e59f0018 	ldr	r0, [pc, #24]	; 4aec <vs_assign_by_id+0x74>
    4ad0:	e1a0e00f 	mov	lr, pc
    4ad4:	e593f000 	ldr	pc, [r3]
    4ad8:	e3e00000 	mvn	r0, #0	; 0x0
    4adc:	eafffff6 	b	4abc <vs_assign_by_id+0x44>
    4ae0:	3c00d178 	.word	0x3c00d178
    4ae4:	0000efdc 	.word	0x0000efdc
    4ae8:	3c00d68c 	.word	0x3c00d68c
    4aec:	00010f0d 	.word	0x00010f0d

00004af0 <vs_device_bootable>:
    4af0:	e350000e 	cmp	r0, #14	; 0xe
    4af4:	059f3030 	ldreq	r3, [pc, #48]	; 4b2c <vs_device_bootable+0x3c>
    4af8:	05930000 	ldreq	r0, [r3]
    4afc:	0a000004 	beq	4b14 <vs_device_bootable+0x24>
    4b00:	e350000d 	cmp	r0, #13	; 0xd
    4b04:	8a000006 	bhi	4b24 <vs_device_bootable+0x34>
    4b08:	e59f2020 	ldr	r2, [pc, #32]	; 4b30 <vs_device_bootable+0x40>
    4b0c:	e3a0302c 	mov	r3, #44	; 0x2c
    4b10:	e0202093 	mla	r0, r3, r0, r2
    4b14:	e3500000 	cmp	r0, #0	; 0x0
    4b18:	15903008 	ldrne	r3, [r0, #8]
    4b1c:	12030001 	andne	r0, r3, #1	; 0x1
    4b20:	112fff1e 	bxne	lr
    4b24:	e3a00000 	mov	r0, #0	; 0x0
    4b28:	e12fff1e 	bx	lr
    4b2c:	3c00d178 	.word	0x3c00d178
    4b30:	0000efdc 	.word	0x0000efdc

00004b34 <vs_device_burnsrc>:
    4b34:	e350000e 	cmp	r0, #14	; 0xe
    4b38:	059f3034 	ldreq	r3, [pc, #52]	; 4b74 <vs_device_burnsrc+0x40>
    4b3c:	05930000 	ldreq	r0, [r3]
    4b40:	0a000004 	beq	4b58 <vs_device_burnsrc+0x24>
    4b44:	e350000d 	cmp	r0, #13	; 0xd
    4b48:	8a000007 	bhi	4b6c <vs_device_burnsrc+0x38>
    4b4c:	e59f2024 	ldr	r2, [pc, #36]	; 4b78 <vs_device_burnsrc+0x44>
    4b50:	e3a0302c 	mov	r3, #44	; 0x2c
    4b54:	e0202093 	mla	r0, r3, r0, r2
    4b58:	e3500000 	cmp	r0, #0	; 0x0
    4b5c:	15903008 	ldrne	r3, [r0, #8]
    4b60:	11a030a3 	lsrne	r3, r3, #1
    4b64:	12030001 	andne	r0, r3, #1	; 0x1
    4b68:	112fff1e 	bxne	lr
    4b6c:	e3a00000 	mov	r0, #0	; 0x0
    4b70:	e12fff1e 	bx	lr
    4b74:	3c00d178 	.word	0x3c00d178
    4b78:	0000efdc 	.word	0x0000efdc

00004b7c <vs_device_burndst>:
    4b7c:	e350000e 	cmp	r0, #14	; 0xe
    4b80:	059f3034 	ldreq	r3, [pc, #52]	; 4bbc <vs_device_burndst+0x40>
    4b84:	05930000 	ldreq	r0, [r3]
    4b88:	0a000004 	beq	4ba0 <vs_device_burndst+0x24>
    4b8c:	e350000d 	cmp	r0, #13	; 0xd
    4b90:	8a000007 	bhi	4bb4 <vs_device_burndst+0x38>
    4b94:	e59f2024 	ldr	r2, [pc, #36]	; 4bc0 <vs_device_burndst+0x44>
    4b98:	e3a0302c 	mov	r3, #44	; 0x2c
    4b9c:	e0202093 	mla	r0, r3, r0, r2
    4ba0:	e3500000 	cmp	r0, #0	; 0x0
    4ba4:	15903008 	ldrne	r3, [r0, #8]
    4ba8:	11a03123 	lsrne	r3, r3, #2
    4bac:	12030001 	andne	r0, r3, #1	; 0x1
    4bb0:	112fff1e 	bxne	lr
    4bb4:	e3a00000 	mov	r0, #0	; 0x0
    4bb8:	e12fff1e 	bx	lr
    4bbc:	3c00d178 	.word	0x3c00d178
    4bc0:	0000efdc 	.word	0x0000efdc

00004bc4 <vs_device_boot0>:
    4bc4:	e350000e 	cmp	r0, #14	; 0xe
    4bc8:	059f3034 	ldreq	r3, [pc, #52]	; 4c04 <vs_device_boot0+0x40>
    4bcc:	05930000 	ldreq	r0, [r3]
    4bd0:	0a000004 	beq	4be8 <vs_device_boot0+0x24>
    4bd4:	e350000d 	cmp	r0, #13	; 0xd
    4bd8:	8a000007 	bhi	4bfc <vs_device_boot0+0x38>
    4bdc:	e59f2024 	ldr	r2, [pc, #36]	; 4c08 <vs_device_boot0+0x44>
    4be0:	e3a0302c 	mov	r3, #44	; 0x2c
    4be4:	e0202093 	mla	r0, r3, r0, r2
    4be8:	e3500000 	cmp	r0, #0	; 0x0
    4bec:	15903008 	ldrne	r3, [r0, #8]
    4bf0:	11a031a3 	lsrne	r3, r3, #3
    4bf4:	12030001 	andne	r0, r3, #1	; 0x1
    4bf8:	112fff1e 	bxne	lr
    4bfc:	e3a00000 	mov	r0, #0	; 0x0
    4c00:	e12fff1e 	bx	lr
    4c04:	3c00d178 	.word	0x3c00d178
    4c08:	0000efdc 	.word	0x0000efdc

00004c0c <vs_device_name>:
    4c0c:	e350000e 	cmp	r0, #14	; 0xe
    4c10:	059f3028 	ldreq	r3, [pc, #40]	; 4c40 <vs_device_name+0x34>
    4c14:	05930000 	ldreq	r0, [r3]
    4c18:	0a000004 	beq	4c30 <vs_device_name+0x24>
    4c1c:	e350000d 	cmp	r0, #13	; 0xd
    4c20:	8a000004 	bhi	4c38 <vs_device_name+0x2c>
    4c24:	e59f2018 	ldr	r2, [pc, #24]	; 4c44 <vs_device_name+0x38>
    4c28:	e3a0302c 	mov	r3, #44	; 0x2c
    4c2c:	e0202093 	mla	r0, r3, r0, r2
    4c30:	e3500000 	cmp	r0, #0	; 0x0
    4c34:	112fff1e 	bxne	lr
    4c38:	e3a00000 	mov	r0, #0	; 0x0
    4c3c:	e12fff1e 	bx	lr
    4c40:	3c00d178 	.word	0x3c00d178
    4c44:	0000efdc 	.word	0x0000efdc

00004c48 <vs_device_erasable>:
    4c48:	e350000e 	cmp	r0, #14	; 0xe
    4c4c:	059f3034 	ldreq	r3, [pc, #52]	; 4c88 <vs_device_erasable+0x40>
    4c50:	05930000 	ldreq	r0, [r3]
    4c54:	0a000004 	beq	4c6c <vs_device_erasable+0x24>
    4c58:	e350000d 	cmp	r0, #13	; 0xd
    4c5c:	8a000007 	bhi	4c80 <vs_device_erasable+0x38>
    4c60:	e59f2024 	ldr	r2, [pc, #36]	; 4c8c <vs_device_erasable+0x44>
    4c64:	e3a0302c 	mov	r3, #44	; 0x2c
    4c68:	e0202093 	mla	r0, r3, r0, r2
    4c6c:	e3500000 	cmp	r0, #0	; 0x0
    4c70:	15903008 	ldrne	r3, [r0, #8]
    4c74:	11a03223 	lsrne	r3, r3, #4
    4c78:	12030001 	andne	r0, r3, #1	; 0x1
    4c7c:	112fff1e 	bxne	lr
    4c80:	e3a00000 	mov	r0, #0	; 0x0
    4c84:	e12fff1e 	bx	lr
    4c88:	3c00d178 	.word	0x3c00d178
    4c8c:	0000efdc 	.word	0x0000efdc

00004c90 <vs_device_count>:
    4c90:	e3a0000e 	mov	r0, #14	; 0xe
    4c94:	e12fff1e 	bx	lr

00004c98 <vs_device_id>:
    4c98:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4c9c:	e59f7040 	ldr	r7, [pc, #64]	; 4ce4 <vs_device_id+0x4c>
    4ca0:	e24dd004 	sub	sp, sp, #4	; 0x4
    4ca4:	e1a05000 	mov	r5, r0
    4ca8:	e3a04000 	mov	r4, #0	; 0x0
    4cac:	e3a0602c 	mov	r6, #44	; 0x2c
    4cb0:	e0217496 	mla	r1, r6, r4, r7
    4cb4:	e1a00005 	mov	r0, r5
    4cb8:	e3a02010 	mov	r2, #16	; 0x10
    4cbc:	ebfff108 	bl	10e4 <strncmp>
    4cc0:	e3500000 	cmp	r0, #0	; 0x0
    4cc4:	0a000003 	beq	4cd8 <vs_device_id+0x40>
    4cc8:	e2844001 	add	r4, r4, #1	; 0x1
    4ccc:	e354000e 	cmp	r4, #14	; 0xe
    4cd0:	1afffff6 	bne	4cb0 <vs_device_id+0x18>
    4cd4:	e2844001 	add	r4, r4, #1	; 0x1
    4cd8:	e1a00004 	mov	r0, r4
    4cdc:	e28dd004 	add	sp, sp, #4	; 0x4
    4ce0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4ce4:	0000efdc 	.word	0x0000efdc

00004ce8 <vs_assign_by_name>:
    4ce8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4cec:	e59f3084 	ldr	r3, [pc, #132]	; 4d78 <vs_assign_by_name+0x90>
    4cf0:	e3a02000 	mov	r2, #0	; 0x0
    4cf4:	e24dd004 	sub	sp, sp, #4	; 0x4
    4cf8:	e5832000 	str	r2, [r3]
    4cfc:	e1a06000 	mov	r6, r0
    4d00:	e1a07001 	mov	r7, r1
    4d04:	e1a04002 	mov	r4, r2
    4d08:	e3a0a02c 	mov	sl, #44	; 0x2c
    4d0c:	e59f8068 	ldr	r8, [pc, #104]	; 4d7c <vs_assign_by_name+0x94>
    4d10:	ea00000d 	b	4d4c <vs_assign_by_name+0x64>
    4d14:	e025849a 	mla	r5, sl, r4, r8
    4d18:	e1a00005 	mov	r0, r5
    4d1c:	ebfff0f0 	bl	10e4 <strncmp>
    4d20:	e3500000 	cmp	r0, #0	; 0x0
    4d24:	1a000007 	bne	4d48 <vs_assign_by_name+0x60>
    4d28:	e59f3048 	ldr	r3, [pc, #72]	; 4d78 <vs_assign_by_name+0x90>
    4d2c:	e3570000 	cmp	r7, #0	; 0x0
    4d30:	e5835000 	str	r5, [r3]
    4d34:	01a00007 	moveq	r0, r7
    4d38:	0a00000c 	beq	4d70 <vs_assign_by_name+0x88>
    4d3c:	e28dd004 	add	sp, sp, #4	; 0x4
    4d40:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    4d44:	eafffed3 	b	4898 <vs_reset>
    4d48:	e2844001 	add	r4, r4, #1	; 0x1
    4d4c:	e354000e 	cmp	r4, #14	; 0xe
    4d50:	e1a01006 	mov	r1, r6
    4d54:	e3a02010 	mov	r2, #16	; 0x10
    4d58:	1affffed 	bne	4d14 <vs_assign_by_name+0x2c>
    4d5c:	e59f001c 	ldr	r0, [pc, #28]	; 4d80 <vs_assign_by_name+0x98>
    4d60:	e59f301c 	ldr	r3, [pc, #28]	; 4d84 <vs_assign_by_name+0x9c>
    4d64:	e1a0e00f 	mov	lr, pc
    4d68:	e593f000 	ldr	pc, [r3]
    4d6c:	e3e00000 	mvn	r0, #0	; 0x0
    4d70:	e28dd004 	add	sp, sp, #4	; 0x4
    4d74:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4d78:	3c00d178 	.word	0x3c00d178
    4d7c:	0000efdc 	.word	0x0000efdc
    4d80:	00010f26 	.word	0x00010f26
    4d84:	3c00d68c 	.word	0x3c00d68c

00004d88 <vs_is_device>:
    4d88:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4d8c:	e350000e 	cmp	r0, #14	; 0xe
    4d90:	059f304c 	ldreq	r3, [pc, #76]	; 4de4 <vs_is_device+0x5c>
    4d94:	e1a0c001 	mov	ip, r1
    4d98:	e24dd004 	sub	sp, sp, #4	; 0x4
    4d9c:	05931000 	ldreq	r1, [r3]
    4da0:	0a000004 	beq	4db8 <vs_is_device+0x30>
    4da4:	e350000d 	cmp	r0, #13	; 0xd
    4da8:	8a00000a 	bhi	4dd8 <vs_is_device+0x50>
    4dac:	e59f2034 	ldr	r2, [pc, #52]	; 4de8 <vs_is_device+0x60>
    4db0:	e3a0302c 	mov	r3, #44	; 0x2c
    4db4:	e0212093 	mla	r1, r3, r0, r2
    4db8:	e3510000 	cmp	r1, #0	; 0x0
    4dbc:	0a000005 	beq	4dd8 <vs_is_device+0x50>
    4dc0:	e1a0000c 	mov	r0, ip
    4dc4:	e3a02010 	mov	r2, #16	; 0x10
    4dc8:	ebfff0c5 	bl	10e4 <strncmp>
    4dcc:	e2700001 	rsbs	r0, r0, #1	; 0x1
    4dd0:	33a00000 	movcc	r0, #0	; 0x0
    4dd4:	ea000000 	b	4ddc <vs_is_device+0x54>
    4dd8:	e3a00000 	mov	r0, #0	; 0x0
    4ddc:	e28dd004 	add	sp, sp, #4	; 0x4
    4de0:	e8bd8000 	pop	{pc}
    4de4:	3c00d178 	.word	0x3c00d178
    4de8:	0000efdc 	.word	0x0000efdc

00004dec <vs_read>:
    4dec:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4df0:	e59f4234 	ldr	r4, [pc, #564]	; 502c <vs_read+0x240>
    4df4:	e1a09000 	mov	r9, r0
    4df8:	e5940000 	ldr	r0, [r4]
    4dfc:	e24dd024 	sub	sp, sp, #36	; 0x24
    4e00:	e3500000 	cmp	r0, #0	; 0x0
    4e04:	e1a06002 	mov	r6, r2
    4e08:	e1a07003 	mov	r7, r3
    4e0c:	e59d5048 	ldr	r5, [sp, #72]
    4e10:	03e04001 	mvneq	r4, #1	; 0x1
    4e14:	0a000081 	beq	5020 <vs_read+0x234>
    4e18:	e5903010 	ldr	r3, [r0, #16]
    4e1c:	e3530000 	cmp	r3, #0	; 0x0
    4e20:	03e04002 	mvneq	r4, #2	; 0x2
    4e24:	0a00007d 	beq	5020 <vs_read+0x234>
    4e28:	e3550000 	cmp	r5, #0	; 0x0
    4e2c:	01a04005 	moveq	r4, r5
    4e30:	0a00007a 	beq	5020 <vs_read+0x234>
    4e34:	e59f11f4 	ldr	r1, [pc, #500]	; 5030 <vs_read+0x244>
    4e38:	e3a02003 	mov	r2, #3	; 0x3
    4e3c:	ebfff0a8 	bl	10e4 <strncmp>
    4e40:	e3500000 	cmp	r0, #0	; 0x0
    4e44:	1a00000a 	bne	4e74 <vs_read+0x88>
    4e48:	e59d204c 	ldr	r2, [sp, #76]
    4e4c:	e58d5000 	str	r5, [sp]
    4e50:	e58d2004 	str	r2, [sp, #4]
    4e54:	e5941000 	ldr	r1, [r4]
    4e58:	e1a00009 	mov	r0, r9
    4e5c:	e1a02006 	mov	r2, r6
    4e60:	e1a03007 	mov	r3, r7
    4e64:	e1a0e00f 	mov	lr, pc
    4e68:	e591f010 	ldr	pc, [r1, #16]
    4e6c:	e1a04000 	mov	r4, r0
    4e70:	ea00006a 	b	5020 <vs_read+0x234>
    4e74:	e3a0000e 	mov	r0, #14	; 0xe
    4e78:	ebfffee7 	bl	4a1c <vs_align>
    4e7c:	e2402001 	sub	r2, r0, #1	; 0x1
    4e80:	e0124006 	ands	r4, r2, r6
    4e84:	10644000 	rsbne	r4, r4, r0
    4e88:	e0643005 	rsb	r3, r4, r5
    4e8c:	e0032002 	and	r2, r3, r2
    4e90:	e192a004 	orrs	sl, r2, r4
    4e94:	e1a08000 	mov	r8, r0
    4e98:	e58d3014 	str	r3, [sp, #20]
    4e9c:	e58d200c 	str	r2, [sp, #12]
    4ea0:	e58da018 	str	sl, [sp, #24]
    4ea4:	01a0400a 	moveq	r4, sl
    4ea8:	058da01c 	streq	sl, [sp, #28]
    4eac:	0a00002c 	beq	4f64 <vs_read+0x178>
    4eb0:	ebfff129 	bl	135c <malloc>
    4eb4:	e250b000 	subs	fp, r0, #0	; 0x0
    4eb8:	e58db01c 	str	fp, [sp, #28]
    4ebc:	1a00000c 	bne	4ef4 <vs_read+0x108>
    4ec0:	e59f416c 	ldr	r4, [pc, #364]	; 5034 <vs_read+0x248>
    4ec4:	e59f016c 	ldr	r0, [pc, #364]	; 5038 <vs_read+0x24c>
    4ec8:	e300110e 	movw	r1, #270	; 0x10e
    4ecc:	e1a0e00f 	mov	lr, pc
    4ed0:	e594f000 	ldr	pc, [r4]
    4ed4:	e59f0160 	ldr	r0, [pc, #352]	; 503c <vs_read+0x250>
    4ed8:	e59f1160 	ldr	r1, [pc, #352]	; 5040 <vs_read+0x254>
    4edc:	e1a0e00f 	mov	lr, pc
    4ee0:	e594f000 	ldr	pc, [r4]
    4ee4:	e59f0158 	ldr	r0, [pc, #344]	; 5044 <vs_read+0x258>
    4ee8:	e1a0e00f 	mov	lr, pc
    4eec:	e594f000 	ldr	pc, [r4]
    4ef0:	eafffffe 	b	4ef0 <vs_read+0x104>
    4ef4:	e3540000 	cmp	r4, #0	; 0x0
    4ef8:	0a000019 	beq	4f64 <vs_read+0x178>
    4efc:	e59f2128 	ldr	r2, [pc, #296]	; 502c <vs_read+0x240>
    4f00:	e2683000 	rsb	r3, r8, #0	; 0x0
    4f04:	e5921000 	ldr	r1, [r2]
    4f08:	e59d204c 	ldr	r2, [sp, #76]
    4f0c:	e006a003 	and	sl, r6, r3
    4f10:	e3a0b000 	mov	fp, #0	; 0x0
    4f14:	e58d2004 	str	r2, [sp, #4]
    4f18:	e1a0300b 	mov	r3, fp
    4f1c:	e58d8000 	str	r8, [sp]
    4f20:	e1a0200a 	mov	r2, sl
    4f24:	e1a0e00f 	mov	lr, pc
    4f28:	e591f010 	ldr	pc, [r1, #16]
    4f2c:	e250b000 	subs	fp, r0, #0	; 0x0
    4f30:	b1a0400b 	movlt	r4, fp
    4f34:	ba000037 	blt	5018 <vs_read+0x22c>
    4f38:	e59d201c 	ldr	r2, [sp, #28]
    4f3c:	e0641008 	rsb	r1, r4, r8
    4f40:	e0821001 	add	r1, r2, r1
    4f44:	e1a00009 	mov	r0, r9
    4f48:	e1a02004 	mov	r2, r4
    4f4c:	ebfff09d 	bl	11c8 <memcpy>
    4f50:	e0966004 	adds	r6, r6, r4
    4f54:	e59d5014 	ldr	r5, [sp, #20]
    4f58:	e0899004 	add	r9, r9, r4
    4f5c:	e2a77000 	adc	r7, r7, #0	; 0x0
    4f60:	e1a0400b 	mov	r4, fp
    4f64:	e59d300c 	ldr	r3, [sp, #12]
    4f68:	e0555003 	subs	r5, r5, r3
    4f6c:	0a00000f 	beq	4fb0 <vs_read+0x1c4>
    4f70:	e59f30b4 	ldr	r3, [pc, #180]	; 502c <vs_read+0x240>
    4f74:	e59da04c 	ldr	sl, [sp, #76]
    4f78:	e58d5000 	str	r5, [sp]
    4f7c:	e5931000 	ldr	r1, [r3]
    4f80:	e1a00009 	mov	r0, r9
    4f84:	e58da004 	str	sl, [sp, #4]
    4f88:	e1a02006 	mov	r2, r6
    4f8c:	e1a03007 	mov	r3, r7
    4f90:	e1a0e00f 	mov	lr, pc
    4f94:	e591f010 	ldr	pc, [r1, #16]
    4f98:	e3500000 	cmp	r0, #0	; 0x0
    4f9c:	ba000012 	blt	4fec <vs_read+0x200>
    4fa0:	e0966005 	adds	r6, r6, r5
    4fa4:	e0a77fc5 	adc	r7, r7, r5, asr #31
    4fa8:	e0844000 	add	r4, r4, r0
    4fac:	e0899005 	add	r9, r9, r5
    4fb0:	e59db00c 	ldr	fp, [sp, #12]
    4fb4:	e35b0000 	cmp	fp, #0	; 0x0
    4fb8:	0a000013 	beq	500c <vs_read+0x220>
    4fbc:	e59d204c 	ldr	r2, [sp, #76]
    4fc0:	e59f3064 	ldr	r3, [pc, #100]	; 502c <vs_read+0x240>
    4fc4:	e58d2004 	str	r2, [sp, #4]
    4fc8:	e58d8000 	str	r8, [sp]
    4fcc:	e5931000 	ldr	r1, [r3]
    4fd0:	e1a02006 	mov	r2, r6
    4fd4:	e1a03007 	mov	r3, r7
    4fd8:	e59d001c 	ldr	r0, [sp, #28]
    4fdc:	e1a0e00f 	mov	lr, pc
    4fe0:	e591f010 	ldr	pc, [r1, #16]
    4fe4:	e3500000 	cmp	r0, #0	; 0x0
    4fe8:	aa000001 	bge	4ff4 <vs_read+0x208>
    4fec:	e1a04000 	mov	r4, r0
    4ff0:	ea000005 	b	500c <vs_read+0x220>
    4ff4:	e1a00009 	mov	r0, r9
    4ff8:	e59d101c 	ldr	r1, [sp, #28]
    4ffc:	e59d200c 	ldr	r2, [sp, #12]
    5000:	ebfff070 	bl	11c8 <memcpy>
    5004:	e59d300c 	ldr	r3, [sp, #12]
    5008:	e0844003 	add	r4, r4, r3
    500c:	e59da018 	ldr	sl, [sp, #24]
    5010:	e35a0000 	cmp	sl, #0	; 0x0
    5014:	0a000001 	beq	5020 <vs_read+0x234>
    5018:	e59d001c 	ldr	r0, [sp, #28]
    501c:	ebfff136 	bl	14fc <free>
    5020:	e1a00004 	mov	r0, r4
    5024:	e28dd024 	add	sp, sp, #36	; 0x24
    5028:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    502c:	3c00d178 	.word	0x3c00d178
    5030:	00010f41 	.word	0x00010f41
    5034:	3c00d68c 	.word	0x3c00d68c
    5038:	00010f45 	.word	0x00010f45
    503c:	0000fd95 	.word	0x0000fd95
    5040:	00010f57 	.word	0x00010f57
    5044:	00010f62 	.word	0x00010f62

00005048 <ss_lock_isk>:
    5048:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    504c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5050:	eb0001d5 	bl	57ac <security_enabled>
    5054:	e3500000 	cmp	r0, #0	; 0x0
    5058:	0a000002 	beq	5068 <ss_lock_isk+0x20>
    505c:	e28dd004 	add	sp, sp, #4	; 0x4
    5060:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    5064:	ea00002e 	b	5124 <block_cipher_lock_isk>
    5068:	e28dd004 	add	sp, sp, #4	; 0x4
    506c:	e8bd8000 	pop	{pc}

00005070 <ss_aes_data>:
    5070:	e92d4070 	push	{r4, r5, r6, lr}
    5074:	e1a05000 	mov	r5, r0
    5078:	e1a06001 	mov	r6, r1
    507c:	e1a04002 	mov	r4, r2
    5080:	eb0001c9 	bl	57ac <security_enabled>
    5084:	e3500000 	cmp	r0, #0	; 0x0
    5088:	0a000007 	beq	50ac <ss_aes_data+0x3c>
    508c:	e284200f 	add	r2, r4, #15	; 0xf
    5090:	e1a00005 	mov	r0, r5
    5094:	e1a01006 	mov	r1, r6
    5098:	e3c2200f 	bic	r2, r2, #15	; 0xf
    509c:	eb0000a0 	bl	5324 <block_cipher_operation>
    50a0:	e3500000 	cmp	r0, #0	; 0x0
    50a4:	13e00000 	mvnne	r0, #0	; 0x0
    50a8:	18bd8070 	popne	{r4, r5, r6, pc}
    50ac:	e3a00000 	mov	r0, #0	; 0x0
    50b0:	e8bd8070 	pop	{r4, r5, r6, pc}

000050b4 <ss_aes_init>:
    50b4:	e92d4070 	push	{r4, r5, r6, lr}
    50b8:	e1a05000 	mov	r5, r0
    50bc:	e1a04001 	mov	r4, r1
    50c0:	e1a06002 	mov	r6, r2
    50c4:	eb0001b8 	bl	57ac <security_enabled>
    50c8:	e3500000 	cmp	r0, #0	; 0x0
    50cc:	0a00000b 	beq	5100 <ss_aes_init+0x4c>
    50d0:	e3540000 	cmp	r4, #0	; 0x0
    50d4:	01a00005 	moveq	r0, r5
    50d8:	01a02004 	moveq	r2, r4
    50dc:	01a03006 	moveq	r3, r6
    50e0:	03a01001 	moveq	r1, #1	; 0x1
    50e4:	11a00005 	movne	r0, r5
    50e8:	11a02004 	movne	r2, r4
    50ec:	11a03006 	movne	r3, r6
    50f0:	13a01000 	movne	r1, #0	; 0x0
    50f4:	eb00002c 	bl	51ac <block_cipher_aes_init>
    50f8:	e3500000 	cmp	r0, #0	; 0x0
    50fc:	1a000001 	bne	5108 <ss_aes_init+0x54>
    5100:	e3a00000 	mov	r0, #0	; 0x0
    5104:	e8bd8070 	pop	{r4, r5, r6, pc}
    5108:	e3e00000 	mvn	r0, #0	; 0x0
    510c:	e8bd8070 	pop	{r4, r5, r6, pc}

00005110 <check_register_bit>:
    5110:	e3a03001 	mov	r3, #1	; 0x1
    5114:	e0103113 	ands	r3, r0, r3, lsl r1
    5118:	03a00000 	moveq	r0, #0	; 0x0
    511c:	13a00001 	movne	r0, #1	; 0x1
    5120:	e12fff1e 	bx	lr

00005124 <block_cipher_lock_isk>:
    5124:	e59f200c 	ldr	r2, [pc, #12]	; 5138 <block_cipher_lock_isk+0x14>
    5128:	e5923400 	ldr	r3, [r2, #1024]
    512c:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    5130:	e5823400 	str	r3, [r2, #1024]
    5134:	e12fff1e 	bx	lr
    5138:	24100000 	.word	0x24100000

0000513c <block_cipher_reset>:
    513c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5140:	e59f4060 	ldr	r4, [pc, #96]	; 51a8 <block_cipher_reset+0x6c>
    5144:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    5148:	e5843400 	str	r3, [r4, #1024]
    514c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5150:	eb000d57 	bl	86b4 <get_ticks>
    5154:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5158:	e3a07000 	mov	r7, #0	; 0x0
    515c:	e0966000 	adds	r6, r6, r0
    5160:	e1a05004 	mov	r5, r4
    5164:	e0a77001 	adc	r7, r7, r1
    5168:	e5944400 	ldr	r4, [r4, #1024]
    516c:	ea000008 	b	5194 <block_cipher_reset+0x58>
    5170:	e5954400 	ldr	r4, [r5, #1024]
    5174:	eb000d4e 	bl	86b4 <get_ticks>
    5178:	e1510007 	cmp	r1, r7
    517c:	8a000002 	bhi	518c <block_cipher_reset+0x50>
    5180:	1a000003 	bne	5194 <block_cipher_reset+0x58>
    5184:	e1500006 	cmp	r0, r6
    5188:	9a000001 	bls	5194 <block_cipher_reset+0x58>
    518c:	e3a00001 	mov	r0, #1	; 0x1
    5190:	ea000002 	b	51a0 <block_cipher_reset+0x64>
    5194:	e3540000 	cmp	r4, #0	; 0x0
    5198:	bafffff4 	blt	5170 <block_cipher_reset+0x34>
    519c:	e3a00000 	mov	r0, #0	; 0x0
    51a0:	e28dd004 	add	sp, sp, #4	; 0x4
    51a4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    51a8:	24100000 	.word	0x24100000

000051ac <block_cipher_aes_init>:
    51ac:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    51b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    51b4:	e1a05000 	mov	r5, r0
    51b8:	e1a04001 	mov	r4, r1
    51bc:	e1a06002 	mov	r6, r2
    51c0:	e1a07003 	mov	r7, r3
    51c4:	ebffffdc 	bl	513c <block_cipher_reset>
    51c8:	e3500000 	cmp	r0, #0	; 0x0
    51cc:	1a00003d 	bne	52c8 <block_cipher_aes_init+0x11c>
    51d0:	e59f3134 	ldr	r3, [pc, #308]	; 530c <block_cipher_aes_init+0x160>
    51d4:	e3a02001 	mov	r2, #1	; 0x1
    51d8:	e3550001 	cmp	r5, #1	; 0x1
    51dc:	e5832404 	str	r2, [r3, #1028]
    51e0:	8a000038 	bhi	52c8 <block_cipher_aes_init+0x11c>
    51e4:	e3540003 	cmp	r4, #3	; 0x3
    51e8:	13540000 	cmpne	r4, #0	; 0x0
    51ec:	0a000001 	beq	51f8 <block_cipher_aes_init+0x4c>
    51f0:	e1540002 	cmp	r4, r2
    51f4:	1a000033 	bne	52c8 <block_cipher_aes_init+0x11c>
    51f8:	e1a03305 	lsl	r3, r5, #6
    51fc:	e3833901 	orr	r3, r3, #16384	; 0x4000
    5200:	e3833004 	orr	r3, r3, #4	; 0x4
    5204:	e59f2100 	ldr	r2, [pc, #256]	; 530c <block_cipher_aes_init+0x160>
    5208:	e1833984 	orr	r3, r3, r4, lsl #19
    520c:	e3540000 	cmp	r4, #0	; 0x0
    5210:	e5823400 	str	r3, [r2, #1024]
    5214:	1a000008 	bne	523c <block_cipher_aes_init+0x90>
    5218:	e59f10f0 	ldr	r1, [pc, #240]	; 5310 <block_cipher_aes_init+0x164>
    521c:	e59f00f0 	ldr	r0, [pc, #240]	; 5314 <block_cipher_aes_init+0x168>
    5220:	e59f30f0 	ldr	r3, [pc, #240]	; 5318 <block_cipher_aes_init+0x16c>
    5224:	e0862001 	add	r2, r6, r1
    5228:	e0823003 	add	r3, r2, r3
    522c:	e5933000 	ldr	r3, [r3]
    5230:	e4813004 	str	r3, [r1], #4
    5234:	e1510000 	cmp	r1, r0
    5238:	1afffff8 	bne	5220 <block_cipher_aes_init+0x74>
    523c:	e59f10d0 	ldr	r1, [pc, #208]	; 5314 <block_cipher_aes_init+0x168>
    5240:	e59f00d4 	ldr	r0, [pc, #212]	; 531c <block_cipher_aes_init+0x170>
    5244:	e59f30d4 	ldr	r3, [pc, #212]	; 5320 <block_cipher_aes_init+0x174>
    5248:	e0872001 	add	r2, r7, r1
    524c:	e0823003 	add	r3, r2, r3
    5250:	e5933000 	ldr	r3, [r3]
    5254:	e4813004 	str	r3, [r1], #4
    5258:	e1510000 	cmp	r1, r0
    525c:	1afffff8 	bne	5244 <block_cipher_aes_init+0x98>
    5260:	e3540000 	cmp	r4, #0	; 0x0
    5264:	1a000008 	bne	528c <block_cipher_aes_init+0xe0>
    5268:	eb000d11 	bl	86b4 <get_ticks>
    526c:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    5270:	e3a05000 	mov	r5, #0	; 0x0
    5274:	e0944000 	adds	r4, r4, r0
    5278:	e59f608c 	ldr	r6, [pc, #140]	; 530c <block_cipher_aes_init+0x160>
    527c:	e0a55001 	adc	r5, r5, r1
    5280:	e596340c 	ldr	r3, [r6, #1036]
    5284:	e3130008 	tst	r3, #8	; 0x8
    5288:	0a000011 	beq	52d4 <block_cipher_aes_init+0x128>
    528c:	eb000d08 	bl	86b4 <get_ticks>
    5290:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    5294:	e3a05000 	mov	r5, #0	; 0x0
    5298:	e0944000 	adds	r4, r4, r0
    529c:	e59f6068 	ldr	r6, [pc, #104]	; 530c <block_cipher_aes_init+0x160>
    52a0:	e0a55001 	adc	r5, r5, r1
    52a4:	e596340c 	ldr	r3, [r6, #1036]
    52a8:	e59f205c 	ldr	r2, [pc, #92]	; 530c <block_cipher_aes_init+0x160>
    52ac:	e3130010 	tst	r3, #16	; 0x10
    52b0:	0a00000e 	beq	52f0 <block_cipher_aes_init+0x144>
    52b4:	e5923400 	ldr	r3, [r2, #1024]
    52b8:	e3a00000 	mov	r0, #0	; 0x0
    52bc:	e3833001 	orr	r3, r3, #1	; 0x1
    52c0:	e5823400 	str	r3, [r2, #1024]
    52c4:	ea000000 	b	52cc <block_cipher_aes_init+0x120>
    52c8:	e3a00001 	mov	r0, #1	; 0x1
    52cc:	e28dd004 	add	sp, sp, #4	; 0x4
    52d0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    52d4:	eb000cf6 	bl	86b4 <get_ticks>
    52d8:	e1510005 	cmp	r1, r5
    52dc:	8afffff9 	bhi	52c8 <block_cipher_aes_init+0x11c>
    52e0:	1affffe6 	bne	5280 <block_cipher_aes_init+0xd4>
    52e4:	e1500004 	cmp	r0, r4
    52e8:	8afffff6 	bhi	52c8 <block_cipher_aes_init+0x11c>
    52ec:	eaffffe3 	b	5280 <block_cipher_aes_init+0xd4>
    52f0:	eb000cef 	bl	86b4 <get_ticks>
    52f4:	e1510005 	cmp	r1, r5
    52f8:	8afffff2 	bhi	52c8 <block_cipher_aes_init+0x11c>
    52fc:	1affffe8 	bne	52a4 <block_cipher_aes_init+0xf8>
    5300:	e1500004 	cmp	r0, r4
    5304:	8affffef 	bhi	52c8 <block_cipher_aes_init+0x11c>
    5308:	eaffffe5 	b	52a4 <block_cipher_aes_init+0xf8>
    530c:	24100000 	.word	0x24100000
    5310:	24100420 	.word	0x24100420
    5314:	24100440 	.word	0x24100440
    5318:	dbeffbe0 	.word	0xdbeffbe0
    531c:	24100450 	.word	0x24100450
    5320:	dbeffbc0 	.word	0xdbeffbc0

00005324 <block_cipher_operation>:
    5324:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5328:	e24dd00c 	sub	sp, sp, #12	; 0xc
    532c:	e1a08002 	mov	r8, r2
    5330:	e58d0004 	str	r0, [sp, #4]
    5334:	e1a09001 	mov	r9, r1
    5338:	e1a06002 	mov	r6, r2
    533c:	e59fa0c4 	ldr	sl, [pc, #196]	; 5408 <block_cipher_operation+0xe4>
    5340:	e3a0b000 	mov	fp, #0	; 0x0
    5344:	ea000028 	b	53ec <block_cipher_operation+0xc8>
    5348:	e59d3004 	ldr	r3, [sp, #4]
    534c:	e3560902 	cmp	r6, #32768	; 0x8000
    5350:	b1a07006 	movlt	r7, r6
    5354:	a3a07902 	movge	r7, #32768	; 0x8000
    5358:	e0664008 	rsb	r4, r6, r8
    535c:	e3a00000 	mov	r0, #0	; 0x0
    5360:	e0831004 	add	r1, r3, r4
    5364:	e1a02007 	mov	r2, r7
    5368:	eb0000ed 	bl	5724 <dma_sdma_init>
    536c:	e3500000 	cmp	r0, #0	; 0x0
    5370:	1a000021 	bne	53fc <block_cipher_operation+0xd8>
    5374:	e0891004 	add	r1, r9, r4
    5378:	e2800001 	add	r0, r0, #1	; 0x1
    537c:	e1a02007 	mov	r2, r7
    5380:	eb0000e7 	bl	5724 <dma_sdma_init>
    5384:	e3500000 	cmp	r0, #0	; 0x0
    5388:	1a00001b 	bne	53fc <block_cipher_operation+0xd8>
    538c:	eb0000d4 	bl	56e4 <dma_start>
    5390:	e3a00001 	mov	r0, #1	; 0x1
    5394:	eb0000d2 	bl	56e4 <dma_start>
    5398:	eb000cc5 	bl	86b4 <get_ticks>
    539c:	e090400a 	adds	r4, r0, sl
    53a0:	e0a1500b 	adc	r5, r1, fp
    53a4:	ea000005 	b	53c0 <block_cipher_operation+0x9c>
    53a8:	eb000cc1 	bl	86b4 <get_ticks>
    53ac:	e1510005 	cmp	r1, r5
    53b0:	8a000011 	bhi	53fc <block_cipher_operation+0xd8>
    53b4:	1a000001 	bne	53c0 <block_cipher_operation+0x9c>
    53b8:	e1500004 	cmp	r0, r4
    53bc:	8a00000e 	bhi	53fc <block_cipher_operation+0xd8>
    53c0:	e3a01003 	mov	r1, #3	; 0x3
    53c4:	e3a00001 	mov	r0, #1	; 0x1
    53c8:	eb0000cb 	bl	56fc <dma_get_status>
    53cc:	e3500000 	cmp	r0, #0	; 0x0
    53d0:	0afffff4 	beq	53a8 <block_cipher_operation+0x84>
    53d4:	e3a00000 	mov	r0, #0	; 0x0
    53d8:	e3a01003 	mov	r1, #3	; 0x3
    53dc:	eb0000c6 	bl	56fc <dma_get_status>
    53e0:	e3500000 	cmp	r0, #0	; 0x0
    53e4:	0a000004 	beq	53fc <block_cipher_operation+0xd8>
    53e8:	e0676006 	rsb	r6, r7, r6
    53ec:	e3560000 	cmp	r6, #0	; 0x0
    53f0:	1affffd4 	bne	5348 <block_cipher_operation+0x24>
    53f4:	e1a00006 	mov	r0, r6
    53f8:	ea000000 	b	5400 <block_cipher_operation+0xdc>
    53fc:	e3a00001 	mov	r0, #1	; 0x1
    5400:	e28dd00c 	add	sp, sp, #12	; 0xc
    5404:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5408:	000f4240 	.word	0x000f4240

0000540c <hash_len>:
    540c:	e59f300c 	ldr	r3, [pc, #12]	; 5420 <hash_len+0x14>
    5410:	e5932000 	ldr	r2, [r3]
    5414:	e59f3008 	ldr	r3, [pc, #8]	; 5424 <hash_len+0x18>
    5418:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    541c:	e12fff1e 	bx	lr
    5420:	3c00d17c 	.word	0x3c00d17c
    5424:	0000f244 	.word	0x0000f244

00005428 <hash_value>:
    5428:	e92d4070 	push	{r4, r5, r6, lr}
    542c:	e59f6054 	ldr	r6, [pc, #84]	; 5488 <hash_value+0x60>
    5430:	e1a05000 	mov	r5, r0
    5434:	e5963000 	ldr	r3, [r6]
    5438:	e3530000 	cmp	r3, #0	; 0x0
    543c:	0a000009 	beq	5468 <hash_value+0x40>
    5440:	eb0000d9 	bl	57ac <security_enabled>
    5444:	e2504000 	subs	r4, r0, #0	; 0x0
    5448:	1a000006 	bne	5468 <hash_value+0x40>
    544c:	e59f0038 	ldr	r0, [pc, #56]	; 548c <hash_value+0x64>
    5450:	e5961000 	ldr	r1, [r6]
    5454:	e59f3034 	ldr	r3, [pc, #52]	; 5490 <hash_value+0x68>
    5458:	e1a0e00f 	mov	lr, pc
    545c:	e593f000 	ldr	pc, [r3]
    5460:	e1a00004 	mov	r0, r4
    5464:	e8bd8070 	pop	{r4, r5, r6, pc}
    5468:	e59f3018 	ldr	r3, [pc, #24]	; 5488 <hash_value+0x60>
    546c:	e1a00005 	mov	r0, r5
    5470:	e5932000 	ldr	r2, [r3]
    5474:	e59f3018 	ldr	r3, [pc, #24]	; 5494 <hash_value+0x6c>
    5478:	e0833202 	add	r3, r3, r2, lsl #4
    547c:	e1a0e00f 	mov	lr, pc
    5480:	e593f00c 	ldr	pc, [r3, #12]
    5484:	e8bd8070 	pop	{r4, r5, r6, pc}
    5488:	3c00d17c 	.word	0x3c00d17c
    548c:	00010f93 	.word	0x00010f93
    5490:	3c00d68c 	.word	0x3c00d68c
    5494:	0000f244 	.word	0x0000f244

00005498 <hash_data>:
    5498:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    549c:	e59f5074 	ldr	r5, [pc, #116]	; 5518 <hash_data+0x80>
    54a0:	e24dd004 	sub	sp, sp, #4	; 0x4
    54a4:	e5953000 	ldr	r3, [r5]
    54a8:	e1a06000 	mov	r6, r0
    54ac:	e3530000 	cmp	r3, #0	; 0x0
    54b0:	e1a07001 	mov	r7, r1
    54b4:	0a000009 	beq	54e0 <hash_data+0x48>
    54b8:	eb0000bb 	bl	57ac <security_enabled>
    54bc:	e2504000 	subs	r4, r0, #0	; 0x0
    54c0:	1a000006 	bne	54e0 <hash_data+0x48>
    54c4:	e59f0050 	ldr	r0, [pc, #80]	; 551c <hash_data+0x84>
    54c8:	e5951000 	ldr	r1, [r5]
    54cc:	e59f304c 	ldr	r3, [pc, #76]	; 5520 <hash_data+0x88>
    54d0:	e1a0e00f 	mov	lr, pc
    54d4:	e593f000 	ldr	pc, [r3]
    54d8:	e1a00004 	mov	r0, r4
    54dc:	ea00000b 	b	5510 <hash_data+0x78>
    54e0:	e59f003c 	ldr	r0, [pc, #60]	; 5524 <hash_data+0x8c>
    54e4:	e59f3034 	ldr	r3, [pc, #52]	; 5520 <hash_data+0x88>
    54e8:	e1a0e00f 	mov	lr, pc
    54ec:	e593f000 	ldr	pc, [r3]
    54f0:	e59f3020 	ldr	r3, [pc, #32]	; 5518 <hash_data+0x80>
    54f4:	e1a00006 	mov	r0, r6
    54f8:	e5932000 	ldr	r2, [r3]
    54fc:	e59f3024 	ldr	r3, [pc, #36]	; 5528 <hash_data+0x90>
    5500:	e1a01007 	mov	r1, r7
    5504:	e0833202 	add	r3, r3, r2, lsl #4
    5508:	e1a0e00f 	mov	lr, pc
    550c:	e593f008 	ldr	pc, [r3, #8]
    5510:	e28dd004 	add	sp, sp, #4	; 0x4
    5514:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5518:	3c00d17c 	.word	0x3c00d17c
    551c:	00010f93 	.word	0x00010f93
    5520:	3c00d68c 	.word	0x3c00d68c
    5524:	00010fb4 	.word	0x00010fb4
    5528:	0000f244 	.word	0x0000f244

0000552c <hash_init>:
    552c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5530:	e3500003 	cmp	r0, #3	; 0x3
    5534:	e24dd004 	sub	sp, sp, #4	; 0x4
    5538:	e1a04000 	mov	r4, r0
    553c:	e1a07001 	mov	r7, r1
    5540:	9a000005 	bls	555c <hash_init+0x30>
    5544:	e59f0064 	ldr	r0, [pc, #100]	; 55b0 <hash_init+0x84>
    5548:	e59f3064 	ldr	r3, [pc, #100]	; 55b4 <hash_init+0x88>
    554c:	e1a0e00f 	mov	lr, pc
    5550:	e593f000 	ldr	pc, [r3]
    5554:	e3e00000 	mvn	r0, #0	; 0x0
    5558:	ea000012 	b	55a8 <hash_init+0x7c>
    555c:	e59f6054 	ldr	r6, [pc, #84]	; 55b8 <hash_init+0x8c>
    5560:	e3500000 	cmp	r0, #0	; 0x0
    5564:	e5860000 	str	r0, [r6]
    5568:	0a000009 	beq	5594 <hash_init+0x68>
    556c:	eb00008e 	bl	57ac <security_enabled>
    5570:	e2505000 	subs	r5, r0, #0	; 0x0
    5574:	1a000006 	bne	5594 <hash_init+0x68>
    5578:	e59f003c 	ldr	r0, [pc, #60]	; 55bc <hash_init+0x90>
    557c:	e5961000 	ldr	r1, [r6]
    5580:	e59f302c 	ldr	r3, [pc, #44]	; 55b4 <hash_init+0x88>
    5584:	e1a0e00f 	mov	lr, pc
    5588:	e593f000 	ldr	pc, [r3]
    558c:	e1a00005 	mov	r0, r5
    5590:	ea000004 	b	55a8 <hash_init+0x7c>
    5594:	e59f3024 	ldr	r3, [pc, #36]	; 55c0 <hash_init+0x94>
    5598:	e1a00007 	mov	r0, r7
    559c:	e0833204 	add	r3, r3, r4, lsl #4
    55a0:	e1a0e00f 	mov	lr, pc
    55a4:	e593f004 	ldr	pc, [r3, #4]
    55a8:	e28dd004 	add	sp, sp, #4	; 0x4
    55ac:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    55b0:	00010fbf 	.word	0x00010fbf
    55b4:	3c00d68c 	.word	0x3c00d68c
    55b8:	3c00d17c 	.word	0x3c00d17c
    55bc:	00010f93 	.word	0x00010f93
    55c0:	0000f244 	.word	0x0000f244

000055c4 <md5_value>:
    55c4:	e92d4030 	push	{r4, r5, lr}
    55c8:	e59f3054 	ldr	r3, [pc, #84]	; 5624 <md5_value+0x60>
    55cc:	e1a05000 	mov	r5, r0
    55d0:	e893000c 	ldm	r3, {r2, r3}
    55d4:	e1823003 	orr	r3, r2, r3
    55d8:	e24dd024 	sub	sp, sp, #36	; 0x24
    55dc:	e3530000 	cmp	r3, #0	; 0x0
    55e0:	13e00000 	mvnne	r0, #0	; 0x0
    55e4:	1a00000c 	bne	561c <md5_value+0x58>
    55e8:	e1a0000d 	mov	r0, sp
    55ec:	eb000178 	bl	5bd4 <hash_module_get_result>
    55f0:	e3500000 	cmp	r0, #0	; 0x0
    55f4:	e1a0400d 	mov	r4, sp
    55f8:	13e00001 	mvnne	r0, #1	; 0x1
    55fc:	1a000006 	bne	561c <md5_value+0x58>
    5600:	e1a0200d 	mov	r2, sp
    5604:	e7d23000 	ldrb	r3, [r2, r0]
    5608:	e7c53000 	strb	r3, [r5, r0]
    560c:	e2800001 	add	r0, r0, #1	; 0x1
    5610:	e3500010 	cmp	r0, #16	; 0x10
    5614:	1afffffa 	bne	5604 <md5_value+0x40>
    5618:	e3a00000 	mov	r0, #0	; 0x0
    561c:	e28dd024 	add	sp, sp, #36	; 0x24
    5620:	e8bd8030 	pop	{r4, r5, pc}
    5624:	3c00d180 	.word	0x3c00d180

00005628 <md5_data>:
    5628:	e59f305c 	ldr	r3, [pc, #92]	; 568c <md5_data+0x64>
    562c:	e92d4070 	push	{r4, r5, r6, lr}
    5630:	e1a06001 	mov	r6, r1
    5634:	e5931004 	ldr	r1, [r3, #4]
    5638:	e1a04006 	mov	r4, r6
    563c:	e3a05000 	mov	r5, #0	; 0x0
    5640:	e1550001 	cmp	r5, r1
    5644:	e593c000 	ldr	ip, [r3]
    5648:	8a000002 	bhi	5658 <md5_data+0x30>
    564c:	1a000003 	bne	5660 <md5_data+0x38>
    5650:	e154000c 	cmp	r4, ip
    5654:	9a000001 	bls	5660 <md5_data+0x38>
    5658:	e3e00000 	mvn	r0, #0	; 0x0
    565c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5660:	e1a0200c 	mov	r2, ip
    5664:	e1a03001 	mov	r3, r1
    5668:	e59fc01c 	ldr	ip, [pc, #28]	; 568c <md5_data+0x64>
    566c:	e0522004 	subs	r2, r2, r4
    5670:	e0c33005 	sbc	r3, r3, r5
    5674:	e1a01006 	mov	r1, r6
    5678:	e88c000c 	stm	ip, {r2, r3}
    567c:	eb0001b7 	bl	5d60 <hash_dma_transfer>
    5680:	e3500000 	cmp	r0, #0	; 0x0
    5684:	13e00001 	mvnne	r0, #1	; 0x1
    5688:	e8bd8070 	pop	{r4, r5, r6, pc}
    568c:	3c00d180 	.word	0x3c00d180

00005690 <md5_init>:
    5690:	e1a03ea0 	lsr	r3, r0, #29
    5694:	e1a0c180 	lsl	ip, r0, #3
    5698:	e92d4010 	push	{r4, lr}
    569c:	e59f403c 	ldr	r4, [pc, #60]	; 56e0 <md5_init+0x50>
    56a0:	e1a0200c 	mov	r2, ip
    56a4:	e1a01003 	mov	r1, r3
    56a8:	e3a00000 	mov	r0, #0	; 0x0
    56ac:	e5843004 	str	r3, [r4, #4]
    56b0:	e584c000 	str	ip, [r4]
    56b4:	eb00018d 	bl	5cf0 <hash_module_init>
    56b8:	e3500000 	cmp	r0, #0	; 0x0
    56bc:	05942000 	ldreq	r2, [r4]
    56c0:	05943004 	ldreq	r3, [r4, #4]
    56c4:	01a021a2 	lsreq	r2, r2, #3
    56c8:	01822e83 	orreq	r2, r2, r3, lsl #29
    56cc:	01a031a3 	lsreq	r3, r3, #3
    56d0:	13e00000 	mvnne	r0, #0	; 0x0
    56d4:	05843004 	streq	r3, [r4, #4]
    56d8:	05842000 	streq	r2, [r4]
    56dc:	e8bd8010 	pop	{r4, pc}
    56e0:	3c00d180 	.word	0x3c00d180

000056e4 <dma_start>:
    56e4:	e2800709 	add	r0, r0, #2359296	; 0x240000
    56e8:	e2800a01 	add	r0, r0, #4096	; 0x1000
    56ec:	e1a00400 	lsl	r0, r0, #8
    56f0:	e3a03001 	mov	r3, #1	; 0x1
    56f4:	e5803000 	str	r3, [r0]
    56f8:	e12fff1e 	bx	lr

000056fc <dma_get_status>:
    56fc:	e3a0c001 	mov	ip, #1	; 0x1
    5700:	e1a0211c 	lsl	r2, ip, r1
    5704:	e2800709 	add	r0, r0, #2359296	; 0x240000
    5708:	e2800a01 	add	r0, r0, #4096	; 0x1000
    570c:	e1a01400 	lsl	r1, r0, #8
    5710:	e5913024 	ldr	r3, [r1, #36]
    5714:	e0130002 	ands	r0, r3, r2
    5718:	15812024 	strne	r2, [r1, #36]
    571c:	11a0000c 	movne	r0, ip
    5720:	e12fff1e 	bx	lr

00005724 <dma_sdma_init>:
    5724:	e280c709 	add	ip, r0, #2359296	; 0x240000
    5728:	e28cca01 	add	ip, ip, #4096	; 0x1000
    572c:	e1a0040c 	lsl	r0, ip, #8
    5730:	e2800004 	add	r0, r0, #4	; 0x4
    5734:	e5903000 	ldr	r3, [r0]
    5738:	e3520801 	cmp	r2, #65536	; 0x10000
    573c:	e3833001 	orr	r3, r3, #1	; 0x1
    5740:	e5803000 	str	r3, [r0]
    5744:	e92d4010 	push	{r4, lr}
    5748:	e3a03341 	mov	r3, #67108865	; 0x4000001
    574c:	e1a0440c 	lsl	r4, ip, #8
    5750:	e584300c 	str	r3, [r4, #12]
    5754:	31a02802 	lslcc	r2, r2, #16
    5758:	31a0340c 	lslcc	r3, ip, #8
    575c:	31a02822 	lsrcc	r2, r2, #16
    5760:	32833010 	addcc	r3, r3, #16	; 0x10
    5764:	35832000 	strcc	r2, [r3]
    5768:	23a00001 	movcs	r0, #1	; 0x1
    576c:	35841014 	strcc	r1, [r4, #20]
    5770:	33a00000 	movcc	r0, #0	; 0x0
    5774:	e8bd8010 	pop	{r4, pc}

00005778 <security_cke_control>:
    5778:	e59fc014 	ldr	ip, [pc, #20]	; 5794 <security_cke_control+0x1c>
    577c:	e3a02001 	mov	r2, #1	; 0x1
    5780:	e59c3000 	ldr	r3, [ip]
    5784:	e1c33012 	bic	r3, r3, r2, lsl r0
    5788:	e1833011 	orr	r3, r3, r1, lsl r0
    578c:	e58c3000 	str	r3, [ip]
    5790:	e12fff1e 	bx	lr
    5794:	24102000 	.word	0x24102000

00005798 <security_enable>:
    5798:	e59f3008 	ldr	r3, [pc, #8]	; 57a8 <security_enable+0x10>
    579c:	e3a02001 	mov	r2, #1	; 0x1
    57a0:	e5832000 	str	r2, [r3]
    57a4:	e12fff1e 	bx	lr
    57a8:	3c00d188 	.word	0x3c00d188

000057ac <security_enabled>:
    57ac:	e59f300c 	ldr	r3, [pc, #12]	; 57c0 <security_enabled+0x14>
    57b0:	e5930000 	ldr	r0, [r3]
    57b4:	e2500000 	subs	r0, r0, #0	; 0x0
    57b8:	13a00001 	movne	r0, #1	; 0x1
    57bc:	e12fff1e 	bx	lr
    57c0:	3c00d188 	.word	0x3c00d188

000057c4 <dma_reset>:
    57c4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    57c8:	e2800709 	add	r0, r0, #2359296	; 0x240000
    57cc:	e2800a01 	add	r0, r0, #4096	; 0x1000
    57d0:	e1a07400 	lsl	r7, r0, #8
    57d4:	e3a03002 	mov	r3, #2	; 0x2
    57d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    57dc:	e5873000 	str	r3, [r7]
    57e0:	eb000bb3 	bl	86b4 <get_ticks>
    57e4:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    57e8:	e3a05000 	mov	r5, #0	; 0x0
    57ec:	e0944000 	adds	r4, r4, r0
    57f0:	e0a55001 	adc	r5, r5, r1
    57f4:	e5976000 	ldr	r6, [r7]
    57f8:	ea000008 	b	5820 <dma_reset+0x5c>
    57fc:	e5976000 	ldr	r6, [r7]
    5800:	eb000bab 	bl	86b4 <get_ticks>
    5804:	e1510005 	cmp	r1, r5
    5808:	8a000002 	bhi	5818 <dma_reset+0x54>
    580c:	1a000003 	bne	5820 <dma_reset+0x5c>
    5810:	e1500004 	cmp	r0, r4
    5814:	9a000001 	bls	5820 <dma_reset+0x5c>
    5818:	e3a00001 	mov	r0, #1	; 0x1
    581c:	ea000002 	b	582c <dma_reset+0x68>
    5820:	e3560000 	cmp	r6, #0	; 0x0
    5824:	1afffff4 	bne	57fc <dma_reset+0x38>
    5828:	e1a00006 	mov	r0, r6
    582c:	e28dd004 	add	sp, sp, #4	; 0x4
    5830:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00005834 <sha1_value>:
    5834:	e92d4030 	push	{r4, r5, lr}
    5838:	e59f3054 	ldr	r3, [pc, #84]	; 5894 <sha1_value+0x60>
    583c:	e1a05000 	mov	r5, r0
    5840:	e893000c 	ldm	r3, {r2, r3}
    5844:	e1823003 	orr	r3, r2, r3
    5848:	e24dd024 	sub	sp, sp, #36	; 0x24
    584c:	e3530000 	cmp	r3, #0	; 0x0
    5850:	13e00000 	mvnne	r0, #0	; 0x0
    5854:	1a00000c 	bne	588c <sha1_value+0x58>
    5858:	e1a0000d 	mov	r0, sp
    585c:	eb0000dc 	bl	5bd4 <hash_module_get_result>
    5860:	e3500000 	cmp	r0, #0	; 0x0
    5864:	e1a0400d 	mov	r4, sp
    5868:	13e00001 	mvnne	r0, #1	; 0x1
    586c:	1a000006 	bne	588c <sha1_value+0x58>
    5870:	e1a0200d 	mov	r2, sp
    5874:	e7d23000 	ldrb	r3, [r2, r0]
    5878:	e7c53000 	strb	r3, [r5, r0]
    587c:	e2800001 	add	r0, r0, #1	; 0x1
    5880:	e3500014 	cmp	r0, #20	; 0x14
    5884:	1afffffa 	bne	5874 <sha1_value+0x40>
    5888:	e3a00000 	mov	r0, #0	; 0x0
    588c:	e28dd024 	add	sp, sp, #36	; 0x24
    5890:	e8bd8030 	pop	{r4, r5, pc}
    5894:	3c00d190 	.word	0x3c00d190

00005898 <sha1_data>:
    5898:	e59f305c 	ldr	r3, [pc, #92]	; 58fc <sha1_data+0x64>
    589c:	e92d4070 	push	{r4, r5, r6, lr}
    58a0:	e1a06001 	mov	r6, r1
    58a4:	e5931004 	ldr	r1, [r3, #4]
    58a8:	e1a04006 	mov	r4, r6
    58ac:	e3a05000 	mov	r5, #0	; 0x0
    58b0:	e1550001 	cmp	r5, r1
    58b4:	e593c000 	ldr	ip, [r3]
    58b8:	8a000002 	bhi	58c8 <sha1_data+0x30>
    58bc:	1a000003 	bne	58d0 <sha1_data+0x38>
    58c0:	e154000c 	cmp	r4, ip
    58c4:	9a000001 	bls	58d0 <sha1_data+0x38>
    58c8:	e3e00000 	mvn	r0, #0	; 0x0
    58cc:	e8bd8070 	pop	{r4, r5, r6, pc}
    58d0:	e1a0200c 	mov	r2, ip
    58d4:	e1a03001 	mov	r3, r1
    58d8:	e59fc01c 	ldr	ip, [pc, #28]	; 58fc <sha1_data+0x64>
    58dc:	e0522004 	subs	r2, r2, r4
    58e0:	e0c33005 	sbc	r3, r3, r5
    58e4:	e1a01006 	mov	r1, r6
    58e8:	e88c000c 	stm	ip, {r2, r3}
    58ec:	eb00011b 	bl	5d60 <hash_dma_transfer>
    58f0:	e3500000 	cmp	r0, #0	; 0x0
    58f4:	13e00001 	mvnne	r0, #1	; 0x1
    58f8:	e8bd8070 	pop	{r4, r5, r6, pc}
    58fc:	3c00d190 	.word	0x3c00d190

00005900 <sha1_init>:
    5900:	e1a03ea0 	lsr	r3, r0, #29
    5904:	e1a0c180 	lsl	ip, r0, #3
    5908:	e92d4010 	push	{r4, lr}
    590c:	e59f403c 	ldr	r4, [pc, #60]	; 5950 <sha1_init+0x50>
    5910:	e1a0200c 	mov	r2, ip
    5914:	e1a01003 	mov	r1, r3
    5918:	e3a00001 	mov	r0, #1	; 0x1
    591c:	e5843004 	str	r3, [r4, #4]
    5920:	e584c000 	str	ip, [r4]
    5924:	eb0000f1 	bl	5cf0 <hash_module_init>
    5928:	e3500000 	cmp	r0, #0	; 0x0
    592c:	05942000 	ldreq	r2, [r4]
    5930:	05943004 	ldreq	r3, [r4, #4]
    5934:	01a021a2 	lsreq	r2, r2, #3
    5938:	01822e83 	orreq	r2, r2, r3, lsl #29
    593c:	01a031a3 	lsreq	r3, r3, #3
    5940:	13e00000 	mvnne	r0, #0	; 0x0
    5944:	05843004 	streq	r3, [r4, #4]
    5948:	05842000 	streq	r2, [r4]
    594c:	e8bd8010 	pop	{r4, pc}
    5950:	3c00d190 	.word	0x3c00d190

00005954 <sha256_value>:
    5954:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5958:	e59f3028 	ldr	r3, [pc, #40]	; 5988 <sha256_value+0x34>
    595c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5960:	e893000c 	ldm	r3, {r2, r3}
    5964:	e1823003 	orr	r3, r2, r3
    5968:	e3530000 	cmp	r3, #0	; 0x0
    596c:	13e00000 	mvnne	r0, #0	; 0x0
    5970:	1a000002 	bne	5980 <sha256_value+0x2c>
    5974:	eb000096 	bl	5bd4 <hash_module_get_result>
    5978:	e3500000 	cmp	r0, #0	; 0x0
    597c:	13e00001 	mvnne	r0, #1	; 0x1
    5980:	e28dd004 	add	sp, sp, #4	; 0x4
    5984:	e8bd8000 	pop	{pc}
    5988:	3c00d198 	.word	0x3c00d198

0000598c <sha256_data>:
    598c:	e59f305c 	ldr	r3, [pc, #92]	; 59f0 <sha256_data+0x64>
    5990:	e92d4070 	push	{r4, r5, r6, lr}
    5994:	e1a06001 	mov	r6, r1
    5998:	e5931004 	ldr	r1, [r3, #4]
    599c:	e1a04006 	mov	r4, r6
    59a0:	e3a05000 	mov	r5, #0	; 0x0
    59a4:	e1550001 	cmp	r5, r1
    59a8:	e593c000 	ldr	ip, [r3]
    59ac:	8a000002 	bhi	59bc <sha256_data+0x30>
    59b0:	1a000003 	bne	59c4 <sha256_data+0x38>
    59b4:	e154000c 	cmp	r4, ip
    59b8:	9a000001 	bls	59c4 <sha256_data+0x38>
    59bc:	e3e00000 	mvn	r0, #0	; 0x0
    59c0:	e8bd8070 	pop	{r4, r5, r6, pc}
    59c4:	e1a0200c 	mov	r2, ip
    59c8:	e1a03001 	mov	r3, r1
    59cc:	e59fc01c 	ldr	ip, [pc, #28]	; 59f0 <sha256_data+0x64>
    59d0:	e0522004 	subs	r2, r2, r4
    59d4:	e0c33005 	sbc	r3, r3, r5
    59d8:	e1a01006 	mov	r1, r6
    59dc:	e88c000c 	stm	ip, {r2, r3}
    59e0:	eb0000de 	bl	5d60 <hash_dma_transfer>
    59e4:	e3500000 	cmp	r0, #0	; 0x0
    59e8:	13e00001 	mvnne	r0, #1	; 0x1
    59ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    59f0:	3c00d198 	.word	0x3c00d198

000059f4 <sha256_init>:
    59f4:	e1a03ea0 	lsr	r3, r0, #29
    59f8:	e1a0c180 	lsl	ip, r0, #3
    59fc:	e92d4010 	push	{r4, lr}
    5a00:	e59f403c 	ldr	r4, [pc, #60]	; 5a44 <sha256_init+0x50>
    5a04:	e1a0200c 	mov	r2, ip
    5a08:	e1a01003 	mov	r1, r3
    5a0c:	e3a00003 	mov	r0, #3	; 0x3
    5a10:	e5843004 	str	r3, [r4, #4]
    5a14:	e584c000 	str	ip, [r4]
    5a18:	eb0000b4 	bl	5cf0 <hash_module_init>
    5a1c:	e3500000 	cmp	r0, #0	; 0x0
    5a20:	05942000 	ldreq	r2, [r4]
    5a24:	05943004 	ldreq	r3, [r4, #4]
    5a28:	01a021a2 	lsreq	r2, r2, #3
    5a2c:	01822e83 	orreq	r2, r2, r3, lsl #29
    5a30:	01a031a3 	lsreq	r3, r3, #3
    5a34:	13e00000 	mvnne	r0, #0	; 0x0
    5a38:	05843004 	streq	r3, [r4, #4]
    5a3c:	05842000 	streq	r2, [r4]
    5a40:	e8bd8010 	pop	{r4, pc}
    5a44:	3c00d198 	.word	0x3c00d198

00005a48 <crc32_init>:
    5a48:	e59f300c 	ldr	r3, [pc, #12]	; 5a5c <crc32_init+0x14>
    5a4c:	e3e02000 	mvn	r2, #0	; 0x0
    5a50:	e5832000 	str	r2, [r3]
    5a54:	e3a00000 	mov	r0, #0	; 0x0
    5a58:	e12fff1e 	bx	lr
    5a5c:	3c00d1a0 	.word	0x3c00d1a0

00005a60 <crc32_data>:
    5a60:	e59f311c 	ldr	r3, [pc, #284]	; 5b84 <crc32_data+0x124>
    5a64:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5a68:	e593c000 	ldr	ip, [r3]
    5a6c:	e1a08000 	mov	r8, r0
    5a70:	e1a06001 	mov	r6, r1
    5a74:	e1a04000 	mov	r4, r0
    5a78:	e1a07001 	mov	r7, r1
    5a7c:	e59f5104 	ldr	r5, [pc, #260]	; 5b88 <crc32_data+0x128>
    5a80:	ea000027 	b	5b24 <crc32_data+0xc4>
    5a84:	e5543008 	ldrb	r3, [r4, #-8]
    5a88:	e5542007 	ldrb	r2, [r4, #-7]
    5a8c:	e02c3003 	eor	r3, ip, r3
    5a90:	e20330ff 	and	r3, r3, #255	; 0xff
    5a94:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    5a98:	e5541006 	ldrb	r1, [r4, #-6]
    5a9c:	e023342c 	eor	r3, r3, ip, lsr #8
    5aa0:	e0232002 	eor	r2, r3, r2
    5aa4:	e20220ff 	and	r2, r2, #255	; 0xff
    5aa8:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    5aac:	e5540005 	ldrb	r0, [r4, #-5]
    5ab0:	e0222423 	eor	r2, r2, r3, lsr #8
    5ab4:	e0221001 	eor	r1, r2, r1
    5ab8:	e20110ff 	and	r1, r1, #255	; 0xff
    5abc:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5ac0:	e5541004 	ldrb	r1, [r4, #-4]
    5ac4:	e0233422 	eor	r3, r3, r2, lsr #8
    5ac8:	e0230000 	eor	r0, r3, r0
    5acc:	e20000ff 	and	r0, r0, #255	; 0xff
    5ad0:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5ad4:	e5540003 	ldrb	r0, [r4, #-3]
    5ad8:	e0222423 	eor	r2, r2, r3, lsr #8
    5adc:	e0221001 	eor	r1, r2, r1
    5ae0:	e20110ff 	and	r1, r1, #255	; 0xff
    5ae4:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5ae8:	e554c002 	ldrb	ip, [r4, #-2]
    5aec:	e0233422 	eor	r3, r3, r2, lsr #8
    5af0:	e0230000 	eor	r0, r3, r0
    5af4:	e20000ff 	and	r0, r0, #255	; 0xff
    5af8:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5afc:	e5541001 	ldrb	r1, [r4, #-1]
    5b00:	e0222423 	eor	r2, r2, r3, lsr #8
    5b04:	e022c00c 	eor	ip, r2, ip
    5b08:	e20cc0ff 	and	ip, ip, #255	; 0xff
    5b0c:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    5b10:	e0233422 	eor	r3, r3, r2, lsr #8
    5b14:	e0231001 	eor	r1, r3, r1
    5b18:	e20110ff 	and	r1, r1, #255	; 0xff
    5b1c:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    5b20:	e022c423 	eor	ip, r2, r3, lsr #8
    5b24:	e3570007 	cmp	r7, #7	; 0x7
    5b28:	e2844008 	add	r4, r4, #8	; 0x8
    5b2c:	e2477008 	sub	r7, r7, #8	; 0x8
    5b30:	8affffd3 	bhi	5a84 <crc32_data+0x24>
    5b34:	e3c63007 	bic	r3, r6, #7	; 0x7
    5b38:	e0880003 	add	r0, r8, r3
    5b3c:	e59f3040 	ldr	r3, [pc, #64]	; 5b84 <crc32_data+0x124>
    5b40:	e2161007 	ands	r1, r6, #7	; 0x7
    5b44:	e583c000 	str	ip, [r3]
    5b48:	0a00000b 	beq	5b7c <crc32_data+0x11c>
    5b4c:	e59f4034 	ldr	r4, [pc, #52]	; 5b88 <crc32_data+0x128>
    5b50:	e3a02000 	mov	r2, #0	; 0x0
    5b54:	e7d03002 	ldrb	r3, [r0, r2]
    5b58:	e2822001 	add	r2, r2, #1	; 0x1
    5b5c:	e02c3003 	eor	r3, ip, r3
    5b60:	e20330ff 	and	r3, r3, #255	; 0xff
    5b64:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    5b68:	e1510002 	cmp	r1, r2
    5b6c:	e023c42c 	eor	ip, r3, ip, lsr #8
    5b70:	1afffff7 	bne	5b54 <crc32_data+0xf4>
    5b74:	e59f3008 	ldr	r3, [pc, #8]	; 5b84 <crc32_data+0x124>
    5b78:	e583c000 	str	ip, [r3]
    5b7c:	e3a00000 	mov	r0, #0	; 0x0
    5b80:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5b84:	3c00d1a0 	.word	0x3c00d1a0
    5b88:	0000f284 	.word	0x0000f284

00005b8c <crc32_value>:
    5b8c:	e59f3010 	ldr	r3, [pc, #16]	; 5ba4 <crc32_value+0x18>
    5b90:	e5933000 	ldr	r3, [r3]
    5b94:	e1e03003 	mvn	r3, r3
    5b98:	e5803000 	str	r3, [r0]
    5b9c:	e3a00000 	mov	r0, #0	; 0x0
    5ba0:	e12fff1e 	bx	lr
    5ba4:	3c00d1a0 	.word	0x3c00d1a0

00005ba8 <crc32_le>:
    5ba8:	e92d4010 	push	{r4, lr}
    5bac:	e59f401c 	ldr	r4, [pc, #28]	; 5bd0 <crc32_le+0x28>
    5bb0:	e3e03000 	mvn	r3, #0	; 0x0
    5bb4:	e1a00001 	mov	r0, r1
    5bb8:	e1a01002 	mov	r1, r2
    5bbc:	e5843000 	str	r3, [r4]
    5bc0:	ebffffa6 	bl	5a60 <crc32_data>
    5bc4:	e5940000 	ldr	r0, [r4]
    5bc8:	e1e00000 	mvn	r0, r0
    5bcc:	e8bd8010 	pop	{r4, pc}
    5bd0:	3c00d1a0 	.word	0x3c00d1a0

00005bd4 <hash_module_get_result>:
    5bd4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5bd8:	e1a08000 	mov	r8, r0
    5bdc:	eb000ab4 	bl	86b4 <get_ticks>
    5be0:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5be4:	e59f3088 	ldr	r3, [pc, #136]	; 5c74 <hash_module_get_result+0xa0>
    5be8:	e3a05000 	mov	r5, #0	; 0x0
    5bec:	e0944000 	adds	r4, r4, r0
    5bf0:	e0a55001 	adc	r5, r5, r1
    5bf4:	e1a07003 	mov	r7, r3
    5bf8:	e593680c 	ldr	r6, [r3, #2060]
    5bfc:	ea000008 	b	5c24 <hash_module_get_result+0x50>
    5c00:	e597680c 	ldr	r6, [r7, #2060]
    5c04:	eb000aaa 	bl	86b4 <get_ticks>
    5c08:	e1510005 	cmp	r1, r5
    5c0c:	8a000002 	bhi	5c1c <hash_module_get_result+0x48>
    5c10:	1a000003 	bne	5c24 <hash_module_get_result+0x50>
    5c14:	e1500004 	cmp	r0, r4
    5c18:	9a000001 	bls	5c24 <hash_module_get_result+0x50>
    5c1c:	e3a00001 	mov	r0, #1	; 0x1
    5c20:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5c24:	e3160001 	tst	r6, #1	; 0x1
    5c28:	0afffff4 	beq	5c00 <hash_module_get_result+0x2c>
    5c2c:	e59f0044 	ldr	r0, [pc, #68]	; 5c78 <hash_module_get_result+0xa4>
    5c30:	e59fc044 	ldr	ip, [pc, #68]	; 5c7c <hash_module_get_result+0xa8>
    5c34:	e1a03000 	mov	r3, r0
    5c38:	e6931008 	ldr	r1, [r3], r8
    5c3c:	e59f203c 	ldr	r2, [pc, #60]	; 5c80 <hash_module_get_result+0xac>
    5c40:	e2800004 	add	r0, r0, #4	; 0x4
    5c44:	e0832002 	add	r2, r3, r2
    5c48:	e150000c 	cmp	r0, ip
    5c4c:	e5821000 	str	r1, [r2]
    5c50:	1afffff7 	bne	5c34 <hash_module_get_result+0x60>
    5c54:	e59f2018 	ldr	r2, [pc, #24]	; 5c74 <hash_module_get_result+0xa0>
    5c58:	e3a03001 	mov	r3, #1	; 0x1
    5c5c:	e582380c 	str	r3, [r2, #2060]
    5c60:	e5923800 	ldr	r3, [r2, #2048]
    5c64:	e3a00000 	mov	r0, #0	; 0x0
    5c68:	e3c33080 	bic	r3, r3, #128	; 0x80
    5c6c:	e5823800 	str	r3, [r2, #2048]
    5c70:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5c74:	24100000 	.word	0x24100000
    5c78:	24100858 	.word	0x24100858
    5c7c:	24100878 	.word	0x24100878
    5c80:	dbeff7a8 	.word	0xdbeff7a8

00005c84 <hash_module_clear>:
    5c84:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5c88:	e59f405c 	ldr	r4, [pc, #92]	; 5cec <hash_module_clear+0x68>
    5c8c:	e3a03002 	mov	r3, #2	; 0x2
    5c90:	e5843800 	str	r3, [r4, #2048]
    5c94:	e24dd004 	sub	sp, sp, #4	; 0x4
    5c98:	eb000a85 	bl	86b4 <get_ticks>
    5c9c:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5ca0:	e3a07000 	mov	r7, #0	; 0x0
    5ca4:	e0966000 	adds	r6, r6, r0
    5ca8:	e1a05004 	mov	r5, r4
    5cac:	e0a77001 	adc	r7, r7, r1
    5cb0:	e5944800 	ldr	r4, [r4, #2048]
    5cb4:	ea000008 	b	5cdc <hash_module_clear+0x58>
    5cb8:	e5954800 	ldr	r4, [r5, #2048]
    5cbc:	eb000a7c 	bl	86b4 <get_ticks>
    5cc0:	e1510007 	cmp	r1, r7
    5cc4:	8a000002 	bhi	5cd4 <hash_module_clear+0x50>
    5cc8:	1a000003 	bne	5cdc <hash_module_clear+0x58>
    5ccc:	e1500006 	cmp	r0, r6
    5cd0:	9a000001 	bls	5cdc <hash_module_clear+0x58>
    5cd4:	e3a00001 	mov	r0, #1	; 0x1
    5cd8:	ea000001 	b	5ce4 <hash_module_clear+0x60>
    5cdc:	e2140002 	ands	r0, r4, #2	; 0x2
    5ce0:	1afffff4 	bne	5cb8 <hash_module_clear+0x34>
    5ce4:	e28dd004 	add	sp, sp, #4	; 0x4
    5ce8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5cec:	24100000 	.word	0x24100000

00005cf0 <hash_module_init>:
    5cf0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5cf4:	e59f4060 	ldr	r4, [pc, #96]	; 5d5c <hash_module_init+0x6c>
    5cf8:	e24dd004 	sub	sp, sp, #4	; 0x4
    5cfc:	e5943800 	ldr	r3, [r4, #2048]
    5d00:	e1a05000 	mov	r5, r0
    5d04:	e3c33080 	bic	r3, r3, #128	; 0x80
    5d08:	e5843800 	str	r3, [r4, #2048]
    5d0c:	e1a07001 	mov	r7, r1
    5d10:	e1a06002 	mov	r6, r2
    5d14:	ebffffda 	bl	5c84 <hash_module_clear>
    5d18:	e3500000 	cmp	r0, #0	; 0x0
    5d1c:	13a00001 	movne	r0, #1	; 0x1
    5d20:	1a00000b 	bne	5d54 <hash_module_init+0x64>
    5d24:	e3a03001 	mov	r3, #1	; 0x1
    5d28:	e1a02185 	lsl	r2, r5, #3
    5d2c:	e5843804 	str	r3, [r4, #2052]
    5d30:	e5842800 	str	r2, [r4, #2048]
    5d34:	e5847814 	str	r7, [r4, #2068]
    5d38:	e5846810 	str	r6, [r4, #2064]
    5d3c:	e5943800 	ldr	r3, [r4, #2048]
    5d40:	e3833080 	orr	r3, r3, #128	; 0x80
    5d44:	e5843800 	str	r3, [r4, #2048]
    5d48:	e5943800 	ldr	r3, [r4, #2048]
    5d4c:	e3833004 	orr	r3, r3, #4	; 0x4
    5d50:	e5843800 	str	r3, [r4, #2048]
    5d54:	e28dd004 	add	sp, sp, #4	; 0x4
    5d58:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5d5c:	24100000 	.word	0x24100000

00005d60 <hash_dma_transfer>:
    5d60:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5d64:	e1a08001 	mov	r8, r1
    5d68:	e24dd004 	sub	sp, sp, #4	; 0x4
    5d6c:	e1a09000 	mov	r9, r0
    5d70:	e1a06001 	mov	r6, r1
    5d74:	e59fa090 	ldr	sl, [pc, #144]	; 5e0c <hash_dma_transfer+0xac>
    5d78:	e3a0b000 	mov	fp, #0	; 0x0
    5d7c:	ea00001b 	b	5df0 <hash_dma_transfer+0x90>
    5d80:	e3560902 	cmp	r6, #32768	; 0x8000
    5d84:	b1a07006 	movlt	r7, r6
    5d88:	a3a07902 	movge	r7, #32768	; 0x8000
    5d8c:	e0661008 	rsb	r1, r6, r8
    5d90:	e0891001 	add	r1, r9, r1
    5d94:	e3a00002 	mov	r0, #2	; 0x2
    5d98:	e1a02007 	mov	r2, r7
    5d9c:	ebfffe60 	bl	5724 <dma_sdma_init>
    5da0:	e3500000 	cmp	r0, #0	; 0x0
    5da4:	1a000015 	bne	5e00 <hash_dma_transfer+0xa0>
    5da8:	e2800002 	add	r0, r0, #2	; 0x2
    5dac:	ebfffe4c 	bl	56e4 <dma_start>
    5db0:	eb000a3f 	bl	86b4 <get_ticks>
    5db4:	e090400a 	adds	r4, r0, sl
    5db8:	e0a1500b 	adc	r5, r1, fp
    5dbc:	ea000005 	b	5dd8 <hash_dma_transfer+0x78>
    5dc0:	eb000a3b 	bl	86b4 <get_ticks>
    5dc4:	e1510005 	cmp	r1, r5
    5dc8:	8a00000c 	bhi	5e00 <hash_dma_transfer+0xa0>
    5dcc:	1a000001 	bne	5dd8 <hash_dma_transfer+0x78>
    5dd0:	e1500004 	cmp	r0, r4
    5dd4:	8a000009 	bhi	5e00 <hash_dma_transfer+0xa0>
    5dd8:	e3a01003 	mov	r1, #3	; 0x3
    5ddc:	e3a00002 	mov	r0, #2	; 0x2
    5de0:	ebfffe45 	bl	56fc <dma_get_status>
    5de4:	e3500000 	cmp	r0, #0	; 0x0
    5de8:	0afffff4 	beq	5dc0 <hash_dma_transfer+0x60>
    5dec:	e0676006 	rsb	r6, r7, r6
    5df0:	e3560000 	cmp	r6, #0	; 0x0
    5df4:	1affffe1 	bne	5d80 <hash_dma_transfer+0x20>
    5df8:	e1a00006 	mov	r0, r6
    5dfc:	ea000000 	b	5e04 <hash_dma_transfer+0xa4>
    5e00:	e3a00001 	mov	r0, #1	; 0x1
    5e04:	e28dd004 	add	sp, sp, #4	; 0x4
    5e08:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5e0c:	000f4240 	.word	0x000f4240

00005e10 <cdbg_log_toggle>:
    5e10:	e59f3014 	ldr	r3, [pc, #20]	; 5e2c <cdbg_log_toggle+0x1c>
    5e14:	e3500000 	cmp	r0, #0	; 0x0
    5e18:	e59f2010 	ldr	r2, [pc, #16]	; 5e30 <cdbg_log_toggle+0x20>
    5e1c:	01a02003 	moveq	r2, r3
    5e20:	e59f300c 	ldr	r3, [pc, #12]	; 5e34 <cdbg_log_toggle+0x24>
    5e24:	e5832000 	str	r2, [r3]
    5e28:	e12fff1e 	bx	lr
    5e2c:	0000c14c 	.word	0x0000c14c
    5e30:	0000c1d0 	.word	0x0000c1d0
    5e34:	3c00d68c 	.word	0x3c00d68c

00005e38 <cdbg_wipe_bootarea>:
    5e38:	e59f3018 	ldr	r3, [pc, #24]	; 5e58 <cdbg_wipe_bootarea+0x20>
    5e3c:	e59f1018 	ldr	r1, [pc, #24]	; 5e5c <cdbg_wipe_bootarea+0x24>
    5e40:	e59f2018 	ldr	r2, [pc, #24]	; 5e60 <cdbg_wipe_bootarea+0x28>
    5e44:	e5031004 	str	r1, [r3, #-4]
    5e48:	e2833004 	add	r3, r3, #4	; 0x4
    5e4c:	e1530002 	cmp	r3, r2
    5e50:	1afffffb 	bne	5e44 <cdbg_wipe_bootarea+0xc>
    5e54:	e12fff1e 	bx	lr
    5e58:	3c000004 	.word	0x3c000004
    5e5c:	eafffffe 	.word	0xeafffffe
    5e60:	3c008004 	.word	0x3c008004

00005e64 <cdbg_verify_burn>:
    5e64:	e59f3004 	ldr	r3, [pc, #4]	; 5e70 <cdbg_verify_burn+0xc>
    5e68:	e5930000 	ldr	r0, [r3]
    5e6c:	e12fff1e 	bx	lr
    5e70:	3c00d1a4 	.word	0x3c00d1a4

00005e74 <cdbg_verify_burn_key>:
    5e74:	e59f0000 	ldr	r0, [pc, #0]	; 5e7c <cdbg_verify_burn_key+0x8>
    5e78:	e12fff1e 	bx	lr
    5e7c:	3c00d1a8 	.word	0x3c00d1a8

00005e80 <cdbg_verify_burn_enable>:
    5e80:	e3500000 	cmp	r0, #0	; 0x0
    5e84:	e59f3018 	ldr	r3, [pc, #24]	; 5ea4 <cdbg_verify_burn_enable+0x24>
    5e88:	03a02000 	moveq	r2, #0	; 0x0
    5e8c:	13a02001 	movne	r2, #1	; 0x1
    5e90:	e5832000 	str	r2, [r3]
    5e94:	012fff1e 	bxeq	lr
    5e98:	e59f0008 	ldr	r0, [pc, #8]	; 5ea8 <cdbg_verify_burn_enable+0x28>
    5e9c:	e3a02020 	mov	r2, #32	; 0x20
    5ea0:	eaffecc8 	b	11c8 <memcpy>
    5ea4:	3c00d1a4 	.word	0x3c00d1a4
    5ea8:	3c00d1a8 	.word	0x3c00d1a8

00005eac <cdbg_jump>:
    5eac:	e92d4030 	push	{r4, r5, lr}
    5eb0:	e1a04000 	mov	r4, r0
    5eb4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5eb8:	e1a05001 	mov	r5, r1
    5ebc:	ebfffc61 	bl	5048 <ss_lock_isk>
    5ec0:	e59f001c 	ldr	r0, [pc, #28]	; 5ee4 <cdbg_jump+0x38>
    5ec4:	e1a01004 	mov	r1, r4
    5ec8:	e59f3018 	ldr	r3, [pc, #24]	; 5ee8 <cdbg_jump+0x3c>
    5ecc:	e1a0e00f 	mov	lr, pc
    5ed0:	e593f000 	ldr	pc, [r3]
    5ed4:	e3000bb8 	movw	r0, #3000	; 0xbb8
    5ed8:	eb000a06 	bl	86f8 <udelay>
    5edc:	e1a00005 	mov	r0, r5
    5ee0:	e12fff34 	blx	r4
    5ee4:	00011000 	.word	0x00011000
    5ee8:	3c00d68c 	.word	0x3c00d68c

00005eec <cdbg_restore_os>:
    5eec:	e92d4010 	push	{r4, lr}
    5ef0:	ebffea2f 	bl	7b4 <get_wakeup_entry>
    5ef4:	e59f301c 	ldr	r3, [pc, #28]	; 5f18 <cdbg_restore_os+0x2c>
    5ef8:	e1a04000 	mov	r4, r0
    5efc:	e59f0018 	ldr	r0, [pc, #24]	; 5f1c <cdbg_restore_os+0x30>
    5f00:	e1a0e00f 	mov	lr, pc
    5f04:	e593f000 	ldr	pc, [r3]
    5f08:	e1a00004 	mov	r0, r4
    5f0c:	e3a01000 	mov	r1, #0	; 0x0
    5f10:	e8bd4010 	pop	{r4, lr}
    5f14:	eaffffe4 	b	5eac <cdbg_jump>
    5f18:	3c00d68c 	.word	0x3c00d68c
    5f1c:	00011010 	.word	0x00011010

00005f20 <cdbg_get_pp>:
    5f20:	e92d4070 	push	{r4, r5, r6, lr}
    5f24:	e24dd008 	sub	sp, sp, #8	; 0x8
    5f28:	e1a0000d 	mov	r0, sp
    5f2c:	e59f103c 	ldr	r1, [pc, #60]	; 5f70 <cdbg_get_pp+0x50>
    5f30:	e3a02008 	mov	r2, #8	; 0x8
    5f34:	ebffeca3 	bl	11c8 <memcpy>
    5f38:	e1a0400d 	mov	r4, sp
    5f3c:	e1a0600d 	mov	r6, sp
    5f40:	ea000004 	b	5f58 <cdbg_get_pp+0x38>
    5f44:	eb0018bf 	bl	c248 <getc>
    5f48:	e1500004 	cmp	r0, r4
    5f4c:	e2854001 	add	r4, r5, #1	; 0x1
    5f50:	e1a05006 	mov	r5, r6
    5f54:	1a000000 	bne	5f5c <cdbg_get_pp+0x3c>
    5f58:	e1a05004 	mov	r5, r4
    5f5c:	e5d54000 	ldrb	r4, [r5]
    5f60:	e3540000 	cmp	r4, #0	; 0x0
    5f64:	1afffff6 	bne	5f44 <cdbg_get_pp+0x24>
    5f68:	e28dd008 	add	sp, sp, #8	; 0x8
    5f6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5f70:	0000f8ec 	.word	0x0000f8ec

00005f74 <cdbg_shell>:
    5f74:	e92d4010 	push	{r4, lr}
    5f78:	ebfffc32 	bl	5048 <ss_lock_isk>
    5f7c:	e59f3048 	ldr	r3, [pc, #72]	; 5fcc <cdbg_shell+0x58>
    5f80:	e59f1048 	ldr	r1, [pc, #72]	; 5fd0 <cdbg_shell+0x5c>
    5f84:	e59f2048 	ldr	r2, [pc, #72]	; 5fd4 <cdbg_shell+0x60>
    5f88:	e5031004 	str	r1, [r3, #-4]
    5f8c:	e2833004 	add	r3, r3, #4	; 0x4
    5f90:	e1530002 	cmp	r3, r2
    5f94:	1afffffb 	bne	5f88 <cdbg_shell+0x14>
    5f98:	e3a00001 	mov	r0, #1	; 0x1
    5f9c:	eb002083 	bl	e1b0 <ss_jtag_en>
    5fa0:	e59f4030 	ldr	r4, [pc, #48]	; 5fd8 <cdbg_shell+0x64>
    5fa4:	e59f3030 	ldr	r3, [pc, #48]	; 5fdc <cdbg_shell+0x68>
    5fa8:	e59f0030 	ldr	r0, [pc, #48]	; 5fe0 <cdbg_shell+0x6c>
    5fac:	e5843000 	str	r3, [r4]
    5fb0:	eb001886 	bl	c1d0 <printf_on>
    5fb4:	ebffffd9 	bl	5f20 <cdbg_get_pp>
    5fb8:	e59f0024 	ldr	r0, [pc, #36]	; 5fe4 <cdbg_shell+0x70>
    5fbc:	e1a0e00f 	mov	lr, pc
    5fc0:	e594f000 	ldr	pc, [r4]
    5fc4:	eb001b8a 	bl	cdf4 <main_loop>
    5fc8:	eafffffd 	b	5fc4 <cdbg_shell+0x50>
    5fcc:	3c000004 	.word	0x3c000004
    5fd0:	eafffffe 	.word	0xeafffffe
    5fd4:	3c008004 	.word	0x3c008004
    5fd8:	3c00d68c 	.word	0x3c00d68c
    5fdc:	0000c1d0 	.word	0x0000c1d0
    5fe0:	0001101f 	.word	0x0001101f
    5fe4:	00011024 	.word	0x00011024

00005fe8 <cdbg_do_config>:
    5fe8:	e92d4030 	push	{r4, r5, lr}
    5fec:	e59f307c 	ldr	r3, [pc, #124]	; 6070 <cdbg_do_config+0x88>
    5ff0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ff4:	e1a04000 	mov	r4, r0
    5ff8:	e1a05001 	mov	r5, r1
    5ffc:	e59f0070 	ldr	r0, [pc, #112]	; 6074 <cdbg_do_config+0x8c>
    6000:	e59f1070 	ldr	r1, [pc, #112]	; 6078 <cdbg_do_config+0x90>
    6004:	e1a0e00f 	mov	lr, pc
    6008:	e593f000 	ldr	pc, [r3]
    600c:	e3a02000 	mov	r2, #0	; 0x0
    6010:	e59f0060 	ldr	r0, [pc, #96]	; 6078 <cdbg_do_config+0x90>
    6014:	ea000006 	b	6034 <cdbg_do_config+0x4c>
    6018:	e7943002 	ldr	r3, [r4, r2]
    601c:	e2822004 	add	r2, r2, #4	; 0x4
    6020:	e1530000 	cmp	r3, r0
    6024:	1a000002 	bne	6034 <cdbg_do_config+0x4c>
    6028:	e3510000 	cmp	r1, #0	; 0x0
    602c:	1a000004 	bne	6044 <cdbg_do_config+0x5c>
    6030:	ea000007 	b	6054 <cdbg_do_config+0x6c>
    6034:	e1520005 	cmp	r2, r5
    6038:	e0841002 	add	r1, r4, r2
    603c:	bafffff5 	blt	6018 <cdbg_do_config+0x30>
    6040:	ea000003 	b	6054 <cdbg_do_config+0x6c>
    6044:	e5910004 	ldr	r0, [r1, #4]
    6048:	eb000c1a 	bl	90b8 <spi_set_bytes>
    604c:	e3a00000 	mov	r0, #0	; 0x0
    6050:	ea000004 	b	6068 <cdbg_do_config+0x80>
    6054:	e59f0020 	ldr	r0, [pc, #32]	; 607c <cdbg_do_config+0x94>
    6058:	e59f3010 	ldr	r3, [pc, #16]	; 6070 <cdbg_do_config+0x88>
    605c:	e1a0e00f 	mov	lr, pc
    6060:	e593f000 	ldr	pc, [r3]
    6064:	e3e00000 	mvn	r0, #0	; 0x0
    6068:	e28dd004 	add	sp, sp, #4	; 0x4
    606c:	e8bd8030 	pop	{r4, r5, pc}
    6070:	3c00d68c 	.word	0x3c00d68c
    6074:	00011035 	.word	0x00011035
    6078:	4a82467c 	.word	0x4a82467c
    607c:	00011052 	.word	0x00011052

00006080 <cdbg_config_isi>:
    6080:	e92d4030 	push	{r4, r5, lr}
    6084:	e24dd004 	sub	sp, sp, #4	; 0x4
    6088:	e1a04000 	mov	r4, r0
    608c:	ebffee10 	bl	18d4 <isi_check>
    6090:	e2505000 	subs	r5, r0, #0	; 0x0
    6094:	0a000007 	beq	60b8 <cdbg_config_isi+0x38>
    6098:	e59f002c 	ldr	r0, [pc, #44]	; 60cc <cdbg_config_isi+0x4c>
    609c:	e1a01005 	mov	r1, r5
    60a0:	e59f3028 	ldr	r3, [pc, #40]	; 60d0 <cdbg_config_isi+0x50>
    60a4:	e1a0e00f 	mov	lr, pc
    60a8:	e593f000 	ldr	pc, [r3]
    60ac:	e1a00005 	mov	r0, r5
    60b0:	e28dd004 	add	sp, sp, #4	; 0x4
    60b4:	e8bd8030 	pop	{r4, r5, pc}
    60b8:	e594100c 	ldr	r1, [r4, #12]
    60bc:	e2840c02 	add	r0, r4, #512	; 0x200
    60c0:	e28dd004 	add	sp, sp, #4	; 0x4
    60c4:	e8bd4030 	pop	{r4, r5, lr}
    60c8:	eaffffc6 	b	5fe8 <cdbg_do_config>
    60cc:	00011071 	.word	0x00011071
    60d0:	3c00d68c 	.word	0x3c00d68c

000060d4 <burn_image_loop>:
    60d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    60d8:	e24dd024 	sub	sp, sp, #36	; 0x24
    60dc:	e59d704c 	ldr	r7, [sp, #76]
    60e0:	e1a06000 	mov	r6, r0
    60e4:	e3570000 	cmp	r7, #0	; 0x0
    60e8:	e58d1010 	str	r1, [sp, #16]
    60ec:	e58d200c 	str	r2, [sp, #12]
    60f0:	e58d3008 	str	r3, [sp, #8]
    60f4:	1a00000c 	bne	612c <burn_image_loop+0x58>
    60f8:	e59f4290 	ldr	r4, [pc, #656]	; 6390 <burn_image_loop+0x2bc>
    60fc:	e59f0290 	ldr	r0, [pc, #656]	; 6394 <burn_image_loop+0x2c0>
    6100:	e3a0102e 	mov	r1, #46	; 0x2e
    6104:	e1a0e00f 	mov	lr, pc
    6108:	e594f000 	ldr	pc, [r4]
    610c:	e59f0284 	ldr	r0, [pc, #644]	; 6398 <burn_image_loop+0x2c4>
    6110:	e59f1284 	ldr	r1, [pc, #644]	; 639c <burn_image_loop+0x2c8>
    6114:	e1a0e00f 	mov	lr, pc
    6118:	e594f000 	ldr	pc, [r4]
    611c:	e59f027c 	ldr	r0, [pc, #636]	; 63a0 <burn_image_loop+0x2cc>
    6120:	e1a0e00f 	mov	lr, pc
    6124:	e594f000 	ldr	pc, [r4]
    6128:	eafffffe 	b	6128 <burn_image_loop+0x54>
    612c:	e59d0010 	ldr	r0, [sp, #16]
    6130:	e59d1008 	ldr	r1, [sp, #8]
    6134:	e1500001 	cmp	r0, r1
    6138:	1a000005 	bne	6154 <burn_image_loop+0x80>
    613c:	e59f0260 	ldr	r0, [pc, #608]	; 63a4 <burn_image_loop+0x2d0>
    6140:	e59f3248 	ldr	r3, [pc, #584]	; 6390 <burn_image_loop+0x2bc>
    6144:	e1a0e00f 	mov	lr, pc
    6148:	e593f000 	ldr	pc, [r3]
    614c:	e3e00001 	mvn	r0, #1	; 0x1
    6150:	ea00008c 	b	6388 <burn_image_loop+0x2b4>
    6154:	e596c000 	ldr	ip, [r6]
    6158:	e3a03000 	mov	r3, #0	; 0x0
    615c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    6160:	e002200c 	and	r2, r2, ip
    6164:	e1a01483 	lsl	r1, r3, #9
    6168:	e1d600b8 	ldrh	r0, [r6, #8]
    616c:	e1811ba2 	orr	r1, r1, r2, lsr #23
    6170:	e1a0e482 	lsl	lr, r2, #9
    6174:	e1a03001 	mov	r3, r1
    6178:	e3a02401 	mov	r2, #16777216	; 0x1000000
    617c:	e5961004 	ldr	r1, [r6, #4]
    6180:	e1a0cc2c 	lsr	ip, ip, #24
    6184:	e1a05000 	mov	r5, r0
    6188:	e09e8002 	adds	r8, lr, r2
    618c:	e1a09003 	mov	r9, r3
    6190:	e3a04000 	mov	r4, #0	; 0x0
    6194:	e3a03000 	mov	r3, #0	; 0x0
    6198:	e20cc03f 	and	ip, ip, #63	; 0x3f
    619c:	e59d0008 	ldr	r0, [sp, #8]
    61a0:	e0a99003 	adc	r9, r9, r3
    61a4:	e184a001 	orr	sl, r4, r1
    61a8:	e1a0b005 	mov	fp, r5
    61ac:	e58dc018 	str	ip, [sp, #24]
    61b0:	e596500c 	ldr	r5, [r6, #12]
    61b4:	ebfffaa3 	bl	4c48 <vs_device_erasable>
    61b8:	e1500004 	cmp	r0, r4
    61bc:	0a000006 	beq	61dc <burn_image_loop+0x108>
    61c0:	e1a01004 	mov	r1, r4
    61c4:	e59d0008 	ldr	r0, [sp, #8]
    61c8:	ebfffa2a 	bl	4a78 <vs_assign_by_id>
    61cc:	e1a0000a 	mov	r0, sl
    61d0:	e1a0100b 	mov	r1, fp
    61d4:	e59d2048 	ldr	r2, [sp, #72]
    61d8:	ebfff9d3 	bl	492c <vs_erase>
    61dc:	e59d0010 	ldr	r0, [sp, #16]
    61e0:	e3a01001 	mov	r1, #1	; 0x1
    61e4:	ebfffa23 	bl	4a78 <vs_assign_by_id>
    61e8:	e59d2018 	ldr	r2, [sp, #24]
    61ec:	e3520002 	cmp	r2, #2	; 0x2
    61f0:	1a000019 	bne	625c <burn_image_loop+0x188>
    61f4:	e3a01c02 	mov	r1, #512	; 0x200
    61f8:	e58d1000 	str	r1, [sp]
    61fc:	e59d100c 	ldr	r1, [sp, #12]
    6200:	e1a02008 	mov	r2, r8
    6204:	e1a03009 	mov	r3, r9
    6208:	e1a00007 	mov	r0, r7
    620c:	e58d1004 	str	r1, [sp, #4]
    6210:	ebfffaf5 	bl	4dec <vs_read>
    6214:	e1a00007 	mov	r0, r7
    6218:	ebffed52 	bl	1768 <isi_check_header>
    621c:	e2504000 	subs	r4, r0, #0	; 0x0
    6220:	1a00000d 	bne	625c <burn_image_loop+0x188>
    6224:	e5973004 	ldr	r3, [r7, #4]
    6228:	e1b03e23 	lsrs	r3, r3, #28
    622c:	0a00000a 	beq	625c <burn_image_loop+0x188>
    6230:	e59f0170 	ldr	r0, [pc, #368]	; 63a8 <burn_image_loop+0x2d4>
    6234:	e59f3154 	ldr	r3, [pc, #340]	; 6390 <burn_image_loop+0x2bc>
    6238:	e1a0e00f 	mov	lr, pc
    623c:	e593f000 	ldr	pc, [r3]
    6240:	e59f2164 	ldr	r2, [pc, #356]	; 63ac <burn_image_loop+0x2d8>
    6244:	e1a01004 	mov	r1, r4
    6248:	e3a00001 	mov	r0, #1	; 0x1
    624c:	ebfffb98 	bl	50b4 <ss_aes_init>
    6250:	e3a02001 	mov	r2, #1	; 0x1
    6254:	e58d201c 	str	r2, [sp, #28]
    6258:	ea000001 	b	6264 <burn_image_loop+0x190>
    625c:	e3a03000 	mov	r3, #0	; 0x0
    6260:	e58d301c 	str	r3, [sp, #28]
    6264:	e59f4124 	ldr	r4, [pc, #292]	; 6390 <burn_image_loop+0x2bc>
    6268:	e59d1048 	ldr	r1, [sp, #72]
    626c:	e59f013c 	ldr	r0, [pc, #316]	; 63b0 <burn_image_loop+0x2dc>
    6270:	e3a02901 	mov	r2, #16384	; 0x4000
    6274:	e1a0e00f 	mov	lr, pc
    6278:	e594f000 	ldr	pc, [r4]
    627c:	e3a06000 	mov	r6, #0	; 0x0
    6280:	e58d5014 	str	r5, [sp, #20]
    6284:	e3a04901 	mov	r4, #16384	; 0x4000
    6288:	ea000039 	b	6374 <burn_image_loop+0x2a0>
    628c:	e1b03a23 	lsrs	r3, r3, #20
    6290:	0a000005 	beq	62ac <burn_image_loop+0x1d8>
    6294:	e1a01a25 	lsr	r1, r5, #20
    6298:	e59f0114 	ldr	r0, [pc, #276]	; 63b4 <burn_image_loop+0x2e0>
    629c:	e59f20ec 	ldr	r2, [pc, #236]	; 6390 <burn_image_loop+0x2bc>
    62a0:	e1a0e00f 	mov	lr, pc
    62a4:	e592f000 	ldr	pc, [r2]
    62a8:	e58d5014 	str	r5, [sp, #20]
    62ac:	e3560000 	cmp	r6, #0	; 0x0
    62b0:	d3a00000 	movle	r0, #0	; 0x0
    62b4:	c3a00001 	movgt	r0, #1	; 0x1
    62b8:	e2866001 	add	r6, r6, #1	; 0x1
    62bc:	eb000de3 	bl	9a50 <led_ctrl>
    62c0:	e3560032 	cmp	r6, #50	; 0x32
    62c4:	e3a01000 	mov	r1, #0	; 0x0
    62c8:	e59d0010 	ldr	r0, [sp, #16]
    62cc:	c2466064 	subgt	r6, r6, #100	; 0x64
    62d0:	ebfff9e8 	bl	4a78 <vs_assign_by_id>
    62d4:	e59d100c 	ldr	r1, [sp, #12]
    62d8:	e1a02008 	mov	r2, r8
    62dc:	e1a03009 	mov	r3, r9
    62e0:	e1a00007 	mov	r0, r7
    62e4:	e58d4000 	str	r4, [sp]
    62e8:	e58d1004 	str	r1, [sp, #4]
    62ec:	ebfffabe 	bl	4dec <vs_read>
    62f0:	e59d201c 	ldr	r2, [sp, #28]
    62f4:	e3520000 	cmp	r2, #0	; 0x0
    62f8:	0a000003 	beq	630c <burn_image_loop+0x238>
    62fc:	e1a01007 	mov	r1, r7
    6300:	e1a02004 	mov	r2, r4
    6304:	e1a00007 	mov	r0, r7
    6308:	ebfffb58 	bl	5070 <ss_aes_data>
    630c:	e3a01000 	mov	r1, #0	; 0x0
    6310:	e59d0008 	ldr	r0, [sp, #8]
    6314:	ebfff9d7 	bl	4a78 <vs_assign_by_id>
    6318:	e2501000 	subs	r1, r0, #0	; 0x0
    631c:	1a000005 	bne	6338 <burn_image_loop+0x264>
    6320:	e1a00007 	mov	r0, r7
    6324:	e1a0200a 	mov	r2, sl
    6328:	e1a0300b 	mov	r3, fp
    632c:	e58d1004 	str	r1, [sp, #4]
    6330:	e58d4000 	str	r4, [sp]
    6334:	ebfff966 	bl	48d4 <vs_write>
    6338:	e59d0018 	ldr	r0, [sp, #24]
    633c:	e3a01000 	mov	r1, #0	; 0x0
    6340:	e3500001 	cmp	r0, #1	; 0x1
    6344:	e3a00901 	mov	r0, #16384	; 0x4000
    6348:	93a03901 	movls	r3, #16384	; 0x4000
    634c:	83a03000 	movhi	r3, #0	; 0x0
    6350:	e3550901 	cmp	r5, #16384	; 0x4000
    6354:	31a02005 	movcc	r2, r5
    6358:	23a02901 	movcs	r2, #16384	; 0x4000
    635c:	e09aa000 	adds	sl, sl, r0
    6360:	e0abb001 	adc	fp, fp, r1
    6364:	e0988000 	adds	r8, r8, r0
    6368:	e0877003 	add	r7, r7, r3
    636c:	e0a99001 	adc	r9, r9, r1
    6370:	e0625005 	rsb	r5, r2, r5
    6374:	e59d1014 	ldr	r1, [sp, #20]
    6378:	e3550000 	cmp	r5, #0	; 0x0
    637c:	e0653001 	rsb	r3, r5, r1
    6380:	1affffc1 	bne	628c <burn_image_loop+0x1b8>
    6384:	e1a00005 	mov	r0, r5
    6388:	e28dd024 	add	sp, sp, #36	; 0x24
    638c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6390:	3c00d68c 	.word	0x3c00d68c
    6394:	00010f45 	.word	0x00010f45
    6398:	0000fd95 	.word	0x0000fd95
    639c:	0001108c 	.word	0x0001108c
    63a0:	00011093 	.word	0x00011093
    63a4:	000110a9 	.word	0x000110a9
    63a8:	000110d6 	.word	0x000110d6
    63ac:	3c00d09c 	.word	0x3c00d09c
    63b0:	000110ef 	.word	0x000110ef
    63b4:	000110fe 	.word	0x000110fe

000063b8 <burn_images>:
    63b8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    63bc:	e590901c 	ldr	r9, [r0, #28]
    63c0:	e24dd014 	sub	sp, sp, #20	; 0x14
    63c4:	e59f41ac 	ldr	r4, [pc, #428]	; 6578 <burn_images+0x1c0>
    63c8:	e1a05000 	mov	r5, r0
    63cc:	e58d100c 	str	r1, [sp, #12]
    63d0:	e59f01a4 	ldr	r0, [pc, #420]	; 657c <burn_images+0x1c4>
    63d4:	e1a01009 	mov	r1, r9
    63d8:	e1a0b002 	mov	fp, r2
    63dc:	e2857020 	add	r7, r5, #32	; 0x20
    63e0:	e1a0e00f 	mov	lr, pc
    63e4:	e594f000 	ldr	pc, [r4]
    63e8:	e1a0a004 	mov	sl, r4
    63ec:	e3a08000 	mov	r8, #0	; 0x0
    63f0:	ea000055 	b	654c <burn_images+0x194>
    63f4:	e59f0184 	ldr	r0, [pc, #388]	; 6580 <burn_images+0x1c8>
    63f8:	e1a01008 	mov	r1, r8
    63fc:	e1a0e00f 	mov	lr, pc
    6400:	e59af000 	ldr	pc, [sl]
    6404:	e5d73003 	ldrb	r3, [r7, #3]
    6408:	e5976008 	ldr	r6, [r7, #8]
    640c:	e203503f 	and	r5, r3, #63	; 0x3f
    6410:	e3550002 	cmp	r5, #2	; 0x2
    6414:	0a00000a 	beq	6444 <burn_images+0x8c>
    6418:	ca000004 	bgt	6430 <burn_images+0x78>
    641c:	e3550000 	cmp	r5, #0	; 0x0
    6420:	0a000051 	beq	656c <burn_images+0x1b4>
    6424:	e3550001 	cmp	r5, #1	; 0x1
    6428:	1a00001a 	bne	6498 <burn_images+0xe0>
    642c:	ea000008 	b	6454 <burn_images+0x9c>
    6430:	e3550030 	cmp	r5, #48	; 0x30
    6434:	0a00000c 	beq	646c <burn_images+0xb4>
    6438:	e3550031 	cmp	r5, #49	; 0x31
    643c:	1a000015 	bne	6498 <burn_images+0xe0>
    6440:	ea00000e 	b	6480 <burn_images+0xc8>
    6444:	e1a04826 	lsr	r4, r6, #16
    6448:	e354000f 	cmp	r4, #15	; 0xf
    644c:	1a000015 	bne	64a8 <burn_images+0xf0>
    6450:	ea00001f 	b	64d4 <burn_images+0x11c>
    6454:	e59f0128 	ldr	r0, [pc, #296]	; 6584 <burn_images+0x1cc>
    6458:	e1a01006 	mov	r1, r6
    645c:	e1a0e00f 	mov	lr, pc
    6460:	e59af000 	ldr	pc, [sl]
    6464:	e1a0e006 	mov	lr, r6
    6468:	ea000040 	b	6570 <burn_images+0x1b8>
    646c:	e59f0114 	ldr	r0, [pc, #276]	; 6588 <burn_images+0x1d0>
    6470:	e1a0e00f 	mov	lr, pc
    6474:	e59af000 	ldr	pc, [sl]
    6478:	ebffe8d7 	bl	7dc <system_reset>
    647c:	ea00003a 	b	656c <burn_images+0x1b4>
    6480:	e59f0104 	ldr	r0, [pc, #260]	; 658c <burn_images+0x1d4>
    6484:	e1a0e00f 	mov	lr, pc
    6488:	e59af000 	ldr	pc, [sl]
    648c:	e3a00001 	mov	r0, #1	; 0x1
    6490:	ebffe7cc 	bl	3c8 <try_boot>
    6494:	ea000034 	b	656c <burn_images+0x1b4>
    6498:	e59f00f0 	ldr	r0, [pc, #240]	; 6590 <burn_images+0x1d8>
    649c:	e1a0e00f 	mov	lr, pc
    64a0:	e59af000 	ldr	pc, [sl]
    64a4:	ea000026 	b	6544 <burn_images+0x18c>
    64a8:	e1a00004 	mov	r0, r4
    64ac:	e3a01001 	mov	r1, #1	; 0x1
    64b0:	ebfff970 	bl	4a78 <vs_assign_by_id>
    64b4:	e3500000 	cmp	r0, #0	; 0x0
    64b8:	0a000005 	beq	64d4 <burn_images+0x11c>
    64bc:	e59f00d0 	ldr	r0, [pc, #208]	; 6594 <burn_images+0x1dc>
    64c0:	e59f30b0 	ldr	r3, [pc, #176]	; 6578 <burn_images+0x1c0>
    64c4:	e1a0e00f 	mov	lr, pc
    64c8:	e593f000 	ldr	pc, [r3]
    64cc:	e3e04002 	mvn	r4, #2	; 0x2
    64d0:	ea000020 	b	6558 <burn_images+0x1a0>
    64d4:	e3a0e30f 	mov	lr, #1006632960	; 0x3c000000
    64d8:	e1a0c826 	lsr	ip, r6, #16
    64dc:	e1a0c80c 	lsl	ip, ip, #16
    64e0:	e1a03004 	mov	r3, r4
    64e4:	e1a00007 	mov	r0, r7
    64e8:	e59d100c 	ldr	r1, [sp, #12]
    64ec:	e1a0200b 	mov	r2, fp
    64f0:	e88d5000 	stm	sp, {ip, lr}
    64f4:	ebfffef6 	bl	60d4 <burn_image_loop>
    64f8:	e3500000 	cmp	r0, #0	; 0x0
    64fc:	aa000006 	bge	651c <burn_images+0x164>
    6500:	e1a01008 	mov	r1, r8
    6504:	e59f008c 	ldr	r0, [pc, #140]	; 6598 <burn_images+0x1e0>
    6508:	e59f3068 	ldr	r3, [pc, #104]	; 6578 <burn_images+0x1c0>
    650c:	e1a0e00f 	mov	lr, pc
    6510:	e593f000 	ldr	pc, [r3]
    6514:	e3e04000 	mvn	r4, #0	; 0x0
    6518:	ea00000e 	b	6558 <burn_images+0x1a0>
    651c:	e3550001 	cmp	r5, #1	; 0x1
    6520:	1a000003 	bne	6534 <burn_images+0x17c>
    6524:	e5970004 	ldr	r0, [r7, #4]
    6528:	e3a01000 	mov	r1, #0	; 0x0
    652c:	ebfffe5e 	bl	5eac <cdbg_jump>
    6530:	ea000003 	b	6544 <burn_images+0x18c>
    6534:	e3550000 	cmp	r5, #0	; 0x0
    6538:	1a000001 	bne	6544 <burn_images+0x18c>
    653c:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    6540:	ebfffece 	bl	6080 <cdbg_config_isi>
    6544:	e2888001 	add	r8, r8, #1	; 0x1
    6548:	e2877010 	add	r7, r7, #16	; 0x10
    654c:	e1580009 	cmp	r8, r9
    6550:	baffffa7 	blt	63f4 <burn_images+0x3c>
    6554:	e3a04000 	mov	r4, #0	; 0x0
    6558:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    655c:	ebffebe6 	bl	14fc <free>
    6560:	e1a00004 	mov	r0, r4
    6564:	e28dd014 	add	sp, sp, #20	; 0x14
    6568:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    656c:	e3a0e30f 	mov	lr, #1006632960	; 0x3c000000
    6570:	e3a0400f 	mov	r4, #15	; 0xf
    6574:	eaffffd7 	b	64d8 <burn_images+0x120>
    6578:	3c00d68c 	.word	0x3c00d68c
    657c:	0001110b 	.word	0x0001110b
    6580:	00011128 	.word	0x00011128
    6584:	0001113e 	.word	0x0001113e
    6588:	0001115b 	.word	0x0001115b
    658c:	00011170 	.word	0x00011170
    6590:	00011183 	.word	0x00011183
    6594:	00011197 	.word	0x00011197
    6598:	000111b4 	.word	0x000111b4

0000659c <burn>:
    659c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    65a0:	e1a09000 	mov	r9, r0
    65a4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    65a8:	e3a00c02 	mov	r0, #512	; 0x200
    65ac:	ebffeb6a 	bl	135c <malloc>
    65b0:	e2507000 	subs	r7, r0, #0	; 0x0
    65b4:	1a000004 	bne	65cc <burn+0x30>
    65b8:	e59f019c 	ldr	r0, [pc, #412]	; 675c <burn+0x1c0>
    65bc:	e59f319c 	ldr	r3, [pc, #412]	; 6760 <burn+0x1c4>
    65c0:	e1a0e00f 	mov	lr, pc
    65c4:	e593f000 	ldr	pc, [r3]
    65c8:	ea000060 	b	6750 <burn+0x1b4>
    65cc:	ebffe7fd 	bl	5c8 <boot_device>
    65d0:	e3a05000 	mov	r5, #0	; 0x0
    65d4:	e59f8184 	ldr	r8, [pc, #388]	; 6760 <burn+0x1c4>
    65d8:	e3a0bc02 	mov	fp, #512	; 0x200
    65dc:	e1a0a000 	mov	sl, r0
    65e0:	ea00004c 	b	6718 <burn+0x17c>
    65e4:	e1a00005 	mov	r0, r5
    65e8:	ebfff951 	bl	4b34 <vs_device_burnsrc>
    65ec:	e3500000 	cmp	r0, #0	; 0x0
    65f0:	0a000047 	beq	6714 <burn+0x178>
    65f4:	e155000a 	cmp	r5, sl
    65f8:	0a000045 	beq	6714 <burn+0x178>
    65fc:	e3a03001 	mov	r3, #1	; 0x1
    6600:	e0193513 	ands	r3, r9, r3, lsl r5
    6604:	1a000042 	bne	6714 <burn+0x178>
    6608:	ebffe8d5 	bl	964 <gtc_time>
    660c:	e1a00005 	mov	r0, r5
    6610:	e59f114c 	ldr	r1, [pc, #332]	; 6764 <burn+0x1c8>
    6614:	ebfff9db 	bl	4d88 <vs_is_device>
    6618:	e3500000 	cmp	r0, #0	; 0x0
    661c:	1a000004 	bne	6634 <burn+0x98>
    6620:	e1a00005 	mov	r0, r5
    6624:	e59f113c 	ldr	r1, [pc, #316]	; 6768 <burn+0x1cc>
    6628:	ebfff9d6 	bl	4d88 <vs_is_device>
    662c:	e3500000 	cmp	r0, #0	; 0x0
    6630:	0a00000c 	beq	6668 <burn+0xcc>
    6634:	ebffe95c 	bl	bac <assert_linkpc>
    6638:	e3500000 	cmp	r0, #0	; 0x0
    663c:	0a000009 	beq	6668 <burn+0xcc>
    6640:	eb001010 	bl	a688 <linkpc_loop>
    6644:	e2506000 	subs	r6, r0, #0	; 0x0
    6648:	0a000038 	beq	6730 <burn+0x194>
    664c:	e1a00005 	mov	r0, r5
    6650:	e5984000 	ldr	r4, [r8]
    6654:	ebfff96c 	bl	4c0c <vs_device_name>
    6658:	e1a02006 	mov	r2, r6
    665c:	e1a01000 	mov	r1, r0
    6660:	e59f0104 	ldr	r0, [pc, #260]	; 676c <burn+0x1d0>
    6664:	ea00000a 	b	6694 <burn+0xf8>
    6668:	e1a00005 	mov	r0, r5
    666c:	e3a01001 	mov	r1, #1	; 0x1
    6670:	ebfff900 	bl	4a78 <vs_assign_by_id>
    6674:	e2506000 	subs	r6, r0, #0	; 0x0
    6678:	0a000007 	beq	669c <burn+0x100>
    667c:	e1a00005 	mov	r0, r5
    6680:	e5984000 	ldr	r4, [r8]
    6684:	ebfff960 	bl	4c0c <vs_device_name>
    6688:	e1a01000 	mov	r1, r0
    668c:	e59f00dc 	ldr	r0, [pc, #220]	; 6770 <burn+0x1d4>
    6690:	e1a02006 	mov	r2, r6
    6694:	e12fff34 	blx	r4
    6698:	ea00001d 	b	6714 <burn+0x178>
    669c:	e1a01007 	mov	r1, r7
    66a0:	e59f00cc 	ldr	r0, [pc, #204]	; 6774 <burn+0x1d8>
    66a4:	e3a02401 	mov	r2, #16777216	; 0x1000000
    66a8:	e3a03c02 	mov	r3, #512	; 0x200
    66ac:	e58d6000 	str	r6, [sp]
    66b0:	e1a0e00f 	mov	lr, pc
    66b4:	e598f000 	ldr	pc, [r8]
    66b8:	e3a02401 	mov	r2, #16777216	; 0x1000000
    66bc:	e3a03000 	mov	r3, #0	; 0x0
    66c0:	e1a00007 	mov	r0, r7
    66c4:	e58d6004 	str	r6, [sp, #4]
    66c8:	e58db000 	str	fp, [sp]
    66cc:	ebfff9c6 	bl	4dec <vs_read>
    66d0:	e1a00007 	mov	r0, r7
    66d4:	eb001657 	bl	c038 <ius_check_header>
    66d8:	e2504000 	subs	r4, r0, #0	; 0x0
    66dc:	1a00000c 	bne	6714 <burn+0x178>
    66e0:	e1a01005 	mov	r1, r5
    66e4:	e59f3074 	ldr	r3, [pc, #116]	; 6760 <burn+0x1c4>
    66e8:	e59f0088 	ldr	r0, [pc, #136]	; 6778 <burn+0x1dc>
    66ec:	e1a0e00f 	mov	lr, pc
    66f0:	e593f000 	ldr	pc, [r3]
    66f4:	e1a00004 	mov	r0, r4
    66f8:	e1a01004 	mov	r1, r4
    66fc:	ebfffddf 	bl	5e80 <cdbg_verify_burn_enable>
    6700:	e1a00007 	mov	r0, r7
    6704:	e1a01005 	mov	r1, r5
    6708:	e1a02004 	mov	r2, r4
    670c:	ebffff29 	bl	63b8 <burn_images>
    6710:	ea000006 	b	6730 <burn+0x194>
    6714:	e2855001 	add	r5, r5, #1	; 0x1
    6718:	ebfff95c 	bl	4c90 <vs_device_count>
    671c:	e1550000 	cmp	r5, r0
    6720:	baffffaf 	blt	65e4 <burn+0x48>
    6724:	e1a00007 	mov	r0, r7
    6728:	ebffeb73 	bl	14fc <free>
    672c:	ea000007 	b	6750 <burn+0x1b4>
    6730:	e59f0044 	ldr	r0, [pc, #68]	; 677c <burn+0x1e0>
    6734:	e59f3024 	ldr	r3, [pc, #36]	; 6760 <burn+0x1c4>
    6738:	e1a0e00f 	mov	lr, pc
    673c:	e593f000 	ldr	pc, [r3]
    6740:	e1a00007 	mov	r0, r7
    6744:	ebffeb6c 	bl	14fc <free>
    6748:	ebfffe09 	bl	5f74 <cdbg_shell>
    674c:	eafffffe 	b	674c <burn+0x1b0>
    6750:	e3e00000 	mvn	r0, #0	; 0x0
    6754:	e28dd00c 	add	sp, sp, #12	; 0xc
    6758:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    675c:	000111cc 	.word	0x000111cc
    6760:	3c00d68c 	.word	0x3c00d68c
    6764:	00010d76 	.word	0x00010d76
    6768:	00010d72 	.word	0x00010d72
    676c:	000111ec 	.word	0x000111ec
    6770:	0001120e 	.word	0x0001120e
    6774:	0001122d 	.word	0x0001122d
    6778:	0001125d 	.word	0x0001125d
    677c:	00011275 	.word	0x00011275

00006780 <mmc_send_cmd>:
    6780:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6784:	e24dd004 	sub	sp, sp, #4	; 0x4
    6788:	e1a0e00f 	mov	lr, pc
    678c:	e590f104 	ldr	pc, [r0, #260]
    6790:	e28dd004 	add	sp, sp, #4	; 0x4
    6794:	e8bd8000 	pop	{pc}

00006798 <mmc_set_blocklen>:
    6798:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    679c:	e3a03015 	mov	r3, #21	; 0x15
    67a0:	e24dd024 	sub	sp, sp, #36	; 0x24
    67a4:	e3a02000 	mov	r2, #0	; 0x0
    67a8:	e58d3004 	str	r3, [sp, #4]
    67ac:	e3a03010 	mov	r3, #16	; 0x10
    67b0:	e58d1008 	str	r1, [sp, #8]
    67b4:	e1cd30b0 	strh	r3, [sp]
    67b8:	e58d201c 	str	r2, [sp, #28]
    67bc:	e1a0100d 	mov	r1, sp
    67c0:	e1a0e00f 	mov	lr, pc
    67c4:	e590f104 	ldr	pc, [r0, #260]
    67c8:	e28dd024 	add	sp, sp, #36	; 0x24
    67cc:	e8bd8000 	pop	{pc}

000067d0 <find_mmc_device>:
    67d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    67d4:	e59f3048 	ldr	r3, [pc, #72]	; 6824 <find_mmc_device+0x54>
    67d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    67dc:	e1a01000 	mov	r1, r0
    67e0:	e1a0c003 	mov	ip, r3
    67e4:	e5932000 	ldr	r2, [r3]
    67e8:	ea000004 	b	6800 <find_mmc_device+0x30>
    67ec:	e592309c 	ldr	r3, [r2, #156]
    67f0:	e1a00002 	mov	r0, r2
    67f4:	e1530001 	cmp	r3, r1
    67f8:	0a000007 	beq	681c <find_mmc_device+0x4c>
    67fc:	e5922000 	ldr	r2, [r2]
    6800:	e152000c 	cmp	r2, ip
    6804:	1afffff8 	bne	67ec <find_mmc_device+0x1c>
    6808:	e59f0018 	ldr	r0, [pc, #24]	; 6828 <find_mmc_device+0x58>
    680c:	e59f3018 	ldr	r3, [pc, #24]	; 682c <find_mmc_device+0x5c>
    6810:	e1a0e00f 	mov	lr, pc
    6814:	e593f000 	ldr	pc, [r3]
    6818:	e3a00000 	mov	r0, #0	; 0x0
    681c:	e28dd004 	add	sp, sp, #4	; 0x4
    6820:	e8bd8000 	pop	{pc}
    6824:	3c00d1c8 	.word	0x3c00d1c8
    6828:	00011298 	.word	0x00011298
    682c:	3c00d68c 	.word	0x3c00d68c

00006830 <mmc_send_ext_csd>:
    6830:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6834:	e3a03015 	mov	r3, #21	; 0x15
    6838:	e24dd034 	sub	sp, sp, #52	; 0x34
    683c:	e58d3004 	str	r3, [sp, #4]
    6840:	e3a03c02 	mov	r3, #512	; 0x200
    6844:	e3a02000 	mov	r2, #0	; 0x0
    6848:	e3a0c001 	mov	ip, #1	; 0x1
    684c:	e58d302c 	str	r3, [sp, #44]
    6850:	e3a03008 	mov	r3, #8	; 0x8
    6854:	e58d1020 	str	r1, [sp, #32]
    6858:	e58d201c 	str	r2, [sp, #28]
    685c:	e58d2008 	str	r2, [sp, #8]
    6860:	e58dc024 	str	ip, [sp, #36]
    6864:	e1cd30b0 	strh	r3, [sp]
    6868:	e58dc028 	str	ip, [sp, #40]
    686c:	e1a0100d 	mov	r1, sp
    6870:	e28d2020 	add	r2, sp, #32	; 0x20
    6874:	e1a0e00f 	mov	lr, pc
    6878:	e590f104 	ldr	pc, [r0, #260]
    687c:	e28dd034 	add	sp, sp, #52	; 0x34
    6880:	e8bd8000 	pop	{pc}

00006884 <sd_switch>:
    6884:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    6888:	e1a02102 	lsl	r2, r2, #2
    688c:	e18ccf81 	orr	ip, ip, r1, lsl #31
    6890:	e3a0100f 	mov	r1, #15	; 0xf
    6894:	e1ccc211 	bic	ip, ip, r1, lsl r2
    6898:	e18cc213 	orr	ip, ip, r3, lsl r2
    689c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    68a0:	e3a03015 	mov	r3, #21	; 0x15
    68a4:	e24dd034 	sub	sp, sp, #52	; 0x34
    68a8:	e58d3004 	str	r3, [sp, #4]
    68ac:	e3a03000 	mov	r3, #0	; 0x0
    68b0:	e58d301c 	str	r3, [sp, #28]
    68b4:	e59d3038 	ldr	r3, [sp, #56]
    68b8:	e3a02001 	mov	r2, #1	; 0x1
    68bc:	e58d3020 	str	r3, [sp, #32]
    68c0:	e3a03040 	mov	r3, #64	; 0x40
    68c4:	e58d302c 	str	r3, [sp, #44]
    68c8:	e3a03006 	mov	r3, #6	; 0x6
    68cc:	e58d2024 	str	r2, [sp, #36]
    68d0:	e58d2028 	str	r2, [sp, #40]
    68d4:	e58dc008 	str	ip, [sp, #8]
    68d8:	e1cd30b0 	strh	r3, [sp]
    68dc:	e1a0100d 	mov	r1, sp
    68e0:	e28d2020 	add	r2, sp, #32	; 0x20
    68e4:	e1a0e00f 	mov	lr, pc
    68e8:	e590f104 	ldr	pc, [r0, #260]
    68ec:	e28dd034 	add	sp, sp, #52	; 0x34
    68f0:	e8bd8000 	pop	{pc}

000068f4 <mmc_set_ios>:
    68f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    68f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    68fc:	e1a0e00f 	mov	lr, pc
    6900:	e590f108 	ldr	pc, [r0, #264]
    6904:	e28dd004 	add	sp, sp, #4	; 0x4
    6908:	e8bd8000 	pop	{pc}

0000690c <mmc_set_clock>:
    690c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6910:	e590203c 	ldr	r2, [r0, #60]
    6914:	e590c038 	ldr	ip, [r0, #56]
    6918:	e1510002 	cmp	r1, r2
    691c:	21a01002 	movcs	r1, r2
    6920:	e24dd004 	sub	sp, sp, #4	; 0x4
    6924:	e151000c 	cmp	r1, ip
    6928:	25801048 	strcs	r1, [r0, #72]
    692c:	3580c048 	strcc	ip, [r0, #72]
    6930:	e1a0e00f 	mov	lr, pc
    6934:	e590f108 	ldr	pc, [r0, #264]
    6938:	e28dd004 	add	sp, sp, #4	; 0x4
    693c:	e8bd8000 	pop	{pc}

00006940 <mmc_set_bus_width>:
    6940:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6944:	e5801044 	str	r1, [r0, #68]
    6948:	e24dd004 	sub	sp, sp, #4	; 0x4
    694c:	e1a0e00f 	mov	lr, pc
    6950:	e590f108 	ldr	pc, [r0, #264]
    6954:	e28dd004 	add	sp, sp, #4	; 0x4
    6958:	e8bd8000 	pop	{pc}

0000695c <mmc_send_if_cond>:
    695c:	e92d4010 	push	{r4, lr}
    6960:	e590202c 	ldr	r2, [r0, #44]
    6964:	e59f3060 	ldr	r3, [pc, #96]	; 69cc <mmc_send_if_cond+0x70>
    6968:	e24dd020 	sub	sp, sp, #32	; 0x20
    696c:	e0023003 	and	r3, r2, r3
    6970:	e3530000 	cmp	r3, #0	; 0x0
    6974:	e28d1020 	add	r1, sp, #32	; 0x20
    6978:	e3a03008 	mov	r3, #8	; 0x8
    697c:	e300c1aa 	movw	ip, #426	; 0x1aa
    6980:	e16132b0 	strh	r3, [r1, #-32]!
    6984:	03a0c0aa 	moveq	ip, #170	; 0xaa
    6988:	e3a02000 	mov	r2, #0	; 0x0
    698c:	e3a03015 	mov	r3, #21	; 0x15
    6990:	e98d1008 	stmib	sp, {r3, ip}
    6994:	e58d201c 	str	r2, [sp, #28]
    6998:	e1a0100d 	mov	r1, sp
    699c:	e5903104 	ldr	r3, [r0, #260]
    69a0:	e1a04000 	mov	r4, r0
    69a4:	e12fff33 	blx	r3
    69a8:	e3500000 	cmp	r0, #0	; 0x0
    69ac:	1a000004 	bne	69c4 <mmc_send_if_cond+0x68>
    69b0:	e5dd300c 	ldrb	r3, [sp, #12]
    69b4:	e35300aa 	cmp	r3, #170	; 0xaa
    69b8:	059f3010 	ldreq	r3, [pc, #16]	; 69d0 <mmc_send_if_cond+0x74>
    69bc:	13e00010 	mvnne	r0, #16	; 0x10
    69c0:	05843030 	streq	r3, [r4, #48]
    69c4:	e28dd020 	add	sp, sp, #32	; 0x20
    69c8:	e8bd8010 	pop	{r4, pc}
    69cc:	00ff8000 	.word	0x00ff8000
    69d0:	00020020 	.word	0x00020020

000069d4 <mmc_register>:
    69d4:	e5903110 	ldr	r3, [r0, #272]
    69d8:	e59fc05c 	ldr	ip, [pc, #92]	; 6a3c <mmc_register+0x68>
    69dc:	e3530000 	cmp	r3, #0	; 0x0
    69e0:	e3a03006 	mov	r3, #6	; 0x6
    69e4:	e59c2000 	ldr	r2, [ip]
    69e8:	e5803098 	str	r3, [r0, #152]
    69ec:	e59f304c 	ldr	r3, [pc, #76]	; 6a40 <mmc_register+0x6c>
    69f0:	e2821001 	add	r1, r2, #1	; 0x1
    69f4:	e58030f8 	str	r3, [r0, #248]
    69f8:	e59f3044 	ldr	r3, [pc, #68]	; 6a44 <mmc_register+0x70>
    69fc:	e580209c 	str	r2, [r0, #156]
    6a00:	e58030fc 	str	r3, [r0, #252]
    6a04:	e3a03001 	mov	r3, #1	; 0x1
    6a08:	e58c1000 	str	r1, [ip]
    6a0c:	e5c030a4 	strb	r3, [r0, #164]
    6a10:	03a03c02 	moveq	r3, #512	; 0x200
    6a14:	05803110 	streq	r3, [r0, #272]
    6a18:	e59f3028 	ldr	r3, [pc, #40]	; 6a48 <mmc_register+0x74>
    6a1c:	e5800004 	str	r0, [r0, #4]
    6a20:	e5932004 	ldr	r2, [r3, #4]
    6a24:	e5803000 	str	r3, [r0]
    6a28:	e5830004 	str	r0, [r3, #4]
    6a2c:	e5820000 	str	r0, [r2]
    6a30:	e5802004 	str	r2, [r0, #4]
    6a34:	e3a00000 	mov	r0, #0	; 0x0
    6a38:	e12fff1e 	bx	lr
    6a3c:	3c00d0bc 	.word	0x3c00d0bc
    6a40:	00007c10 	.word	0x00007c10
    6a44:	0000712c 	.word	0x0000712c
    6a48:	3c00d1c8 	.word	0x3c00d1c8

00006a4c <mmc_get_dev>:
    6a4c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a50:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a54:	ebffff5d 	bl	67d0 <find_mmc_device>
    6a58:	e3500000 	cmp	r0, #0	; 0x0
    6a5c:	12800098 	addne	r0, r0, #152	; 0x98
    6a60:	e28dd004 	add	sp, sp, #4	; 0x4
    6a64:	e8bd8000 	pop	{pc}

00006a68 <print_mmc_devices>:
    6a68:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6a6c:	e59f3068 	ldr	r3, [pc, #104]	; 6adc <print_mmc_devices+0x74>
    6a70:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a74:	e1a05000 	mov	r5, r0
    6a78:	e1a07003 	mov	r7, r3
    6a7c:	e5934000 	ldr	r4, [r3]
    6a80:	e59f6058 	ldr	r6, [pc, #88]	; 6ae0 <print_mmc_devices+0x78>
    6a84:	ea00000a 	b	6ab4 <print_mmc_devices+0x4c>
    6a88:	e594209c 	ldr	r2, [r4, #156]
    6a8c:	e1a0e00f 	mov	lr, pc
    6a90:	e596f000 	ldr	pc, [r6]
    6a94:	e5943000 	ldr	r3, [r4]
    6a98:	e59f0044 	ldr	r0, [pc, #68]	; 6ae4 <print_mmc_devices+0x7c>
    6a9c:	e1530007 	cmp	r3, r7
    6aa0:	e1a01005 	mov	r1, r5
    6aa4:	0a000001 	beq	6ab0 <print_mmc_devices+0x48>
    6aa8:	e1a0e00f 	mov	lr, pc
    6aac:	e596f000 	ldr	pc, [r6]
    6ab0:	e5944000 	ldr	r4, [r4]
    6ab4:	e1540007 	cmp	r4, r7
    6ab8:	e2841008 	add	r1, r4, #8	; 0x8
    6abc:	e59f0024 	ldr	r0, [pc, #36]	; 6ae8 <print_mmc_devices+0x80>
    6ac0:	1afffff0 	bne	6a88 <print_mmc_devices+0x20>
    6ac4:	e59f0020 	ldr	r0, [pc, #32]	; 6aec <print_mmc_devices+0x84>
    6ac8:	e59f3010 	ldr	r3, [pc, #16]	; 6ae0 <print_mmc_devices+0x78>
    6acc:	e1a0e00f 	mov	lr, pc
    6ad0:	e593f000 	ldr	pc, [r3]
    6ad4:	e28dd004 	add	sp, sp, #4	; 0x4
    6ad8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6adc:	3c00d1c8 	.word	0x3c00d1c8
    6ae0:	3c00d68c 	.word	0x3c00d68c
    6ae4:	000112b8 	.word	0x000112b8
    6ae8:	000112b1 	.word	0x000112b1
    6aec:	000104fe 	.word	0x000104fe

00006af0 <get_mmc_num>:
    6af0:	e59f3004 	ldr	r3, [pc, #4]	; 6afc <get_mmc_num+0xc>
    6af4:	e5930000 	ldr	r0, [r3]
    6af8:	e12fff1e 	bx	lr
    6afc:	3c00d0bc 	.word	0x3c00d0bc

00006b00 <mmc_vs_align>:
    6b00:	e3a00c02 	mov	r0, #512	; 0x200
    6b04:	e12fff1e 	bx	lr

00006b08 <init_mmc>:
    6b08:	e59f3060 	ldr	r3, [pc, #96]	; 6b70 <init_mmc+0x68>
    6b0c:	e59f2060 	ldr	r2, [pc, #96]	; 6b74 <init_mmc+0x6c>
    6b10:	e92d4070 	push	{r4, r5, r6, lr}
    6b14:	e3a04000 	mov	r4, #0	; 0x0
    6b18:	e3a00b02 	mov	r0, #2048	; 0x800
    6b1c:	e5833004 	str	r3, [r3, #4]
    6b20:	e5824000 	str	r4, [r2]
    6b24:	e5833000 	str	r3, [r3]
    6b28:	ebffea0b 	bl	135c <malloc>
    6b2c:	e1a05000 	mov	r5, r0
    6b30:	e2806b01 	add	r6, r0, #1024	; 0x400
    6b34:	e1a00005 	mov	r0, r5
    6b38:	e3a01000 	mov	r1, #0	; 0x0
    6b3c:	e3a02f4e 	mov	r2, #312	; 0x138
    6b40:	ebffe998 	bl	11a8 <memset>
    6b44:	e5856130 	str	r6, [r5, #304]
    6b48:	e1a00004 	mov	r0, r4
    6b4c:	e1a01005 	mov	r1, r5
    6b50:	eb000490 	bl	7d98 <imap_sdhc_initialize>
    6b54:	e3500000 	cmp	r0, #0	; 0x0
    6b58:	e2844001 	add	r4, r4, #1	; 0x1
    6b5c:	e2855f4e 	add	r5, r5, #312	; 0x138
    6b60:	18bd8070 	popne	{r4, r5, r6, pc}
    6b64:	e3540003 	cmp	r4, #3	; 0x3
    6b68:	1afffff1 	bne	6b34 <init_mmc+0x2c>
    6b6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    6b70:	3c00d1c8 	.word	0x3c00d1c8
    6b74:	3c00d0bc 	.word	0x3c00d0bc

00006b78 <sd_change_freq>:
    6b78:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6b7c:	e5903050 	ldr	r3, [r0, #80]
    6b80:	e3a01000 	mov	r1, #0	; 0x0
    6b84:	e2132b01 	ands	r2, r3, #1024	; 0x400
    6b88:	e1a04000 	mov	r4, r0
    6b8c:	e580104c 	str	r1, [r0, #76]
    6b90:	e24dd084 	sub	sp, sp, #132	; 0x84
    6b94:	11a00001 	movne	r0, r1
    6b98:	1a000070 	bne	6d60 <sd_change_freq+0x1e8>
    6b9c:	e1d438b0 	ldrh	r3, [r4, #128]
    6ba0:	e3a05015 	mov	r5, #21	; 0x15
    6ba4:	e1a03803 	lsl	r3, r3, #16
    6ba8:	e58d3050 	str	r3, [sp, #80]
    6bac:	e28d6048 	add	r6, sp, #72	; 0x48
    6bb0:	e3a03037 	mov	r3, #55	; 0x37
    6bb4:	e1cd34b8 	strh	r3, [sp, #72]
    6bb8:	e58d504c 	str	r5, [sp, #76]
    6bbc:	e58d2064 	str	r2, [sp, #100]
    6bc0:	e1a01006 	mov	r1, r6
    6bc4:	e1a0e00f 	mov	lr, pc
    6bc8:	e594f104 	ldr	pc, [r4, #260]
    6bcc:	e3500000 	cmp	r0, #0	; 0x0
    6bd0:	1a000062 	bne	6d60 <sd_change_freq+0x1e8>
    6bd4:	e3a03033 	mov	r3, #51	; 0x33
    6bd8:	e58d504c 	str	r5, [sp, #76]
    6bdc:	e1a09006 	mov	r9, r6
    6be0:	e58d0064 	str	r0, [sp, #100]
    6be4:	e1cd34b8 	strh	r3, [sp, #72]
    6be8:	e58d0050 	str	r0, [sp, #80]
    6bec:	e3a05003 	mov	r5, #3	; 0x3
    6bf0:	e28da078 	add	sl, sp, #120	; 0x78
    6bf4:	e3a08008 	mov	r8, #8	; 0x8
    6bf8:	e3a06001 	mov	r6, #1	; 0x1
    6bfc:	e28d7068 	add	r7, sp, #104	; 0x68
    6c00:	e58da068 	str	sl, [sp, #104]
    6c04:	e58d8074 	str	r8, [sp, #116]
    6c08:	e58d6070 	str	r6, [sp, #112]
    6c0c:	e58d606c 	str	r6, [sp, #108]
    6c10:	e1a00004 	mov	r0, r4
    6c14:	e1a01009 	mov	r1, r9
    6c18:	e1a02007 	mov	r2, r7
    6c1c:	e1a0e00f 	mov	lr, pc
    6c20:	e594f104 	ldr	pc, [r4, #260]
    6c24:	e250b000 	subs	fp, r0, #0	; 0x0
    6c28:	0a000003 	beq	6c3c <sd_change_freq+0xc4>
    6c2c:	e3550000 	cmp	r5, #0	; 0x0
    6c30:	e2455001 	sub	r5, r5, #1	; 0x1
    6c34:	1afffff1 	bne	6c00 <sd_change_freq+0x88>
    6c38:	ea000047 	b	6d5c <sd_change_freq+0x1e4>
    6c3c:	e59d3078 	ldr	r3, [sp, #120]
    6c40:	e59d007c 	ldr	r0, [sp, #124]
    6c44:	e1a01c03 	lsl	r1, r3, #24
    6c48:	e2032cff 	and	r2, r3, #65280	; 0xff00
    6c4c:	e1811c23 	orr	r1, r1, r3, lsr #24
    6c50:	e1811402 	orr	r1, r1, r2, lsl #8
    6c54:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    6c58:	e1811423 	orr	r1, r1, r3, lsr #8
    6c5c:	e1a03c00 	lsl	r3, r0, #24
    6c60:	e1833c20 	orr	r3, r3, r0, lsr #24
    6c64:	e2002cff 	and	r2, r0, #65280	; 0xff00
    6c68:	e1a0cc21 	lsr	ip, r1, #24
    6c6c:	e1833402 	orr	r3, r3, r2, lsl #8
    6c70:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    6c74:	e20cc00f 	and	ip, ip, #15	; 0xf
    6c78:	e1833420 	orr	r3, r3, r0, lsr #8
    6c7c:	e35c0001 	cmp	ip, #1	; 0x1
    6c80:	e584305c 	str	r3, [r4, #92]
    6c84:	e5841058 	str	r1, [r4, #88]
    6c88:	059f30d8 	ldreq	r3, [pc, #216]	; 6d68 <sd_change_freq+0x1f0>
    6c8c:	0a000004 	beq	6ca4 <sd_change_freq+0x12c>
    6c90:	3a000002 	bcc	6ca0 <sd_change_freq+0x128>
    6c94:	e35c0002 	cmp	ip, #2	; 0x2
    6c98:	059f30cc 	ldreq	r3, [pc, #204]	; 6d6c <sd_change_freq+0x1f4>
    6c9c:	0a000000 	beq	6ca4 <sd_change_freq+0x12c>
    6ca0:	e59f30c8 	ldr	r3, [pc, #200]	; 6d70 <sd_change_freq+0x1f8>
    6ca4:	e5843030 	str	r3, [r4, #48]
    6ca8:	e5943058 	ldr	r3, [r4, #88]
    6cac:	e5942030 	ldr	r2, [r4, #48]
    6cb0:	e3130701 	tst	r3, #262144	; 0x40000
    6cb4:	1594304c 	ldrne	r3, [r4, #76]
    6cb8:	13833c01 	orrne	r3, r3, #256	; 0x100
    6cbc:	1584304c 	strne	r3, [r4, #76]
    6cc0:	e59f30a8 	ldr	r3, [pc, #168]	; 6d70 <sd_change_freq+0x1f8>
    6cc4:	e1520003 	cmp	r2, r3
    6cc8:	13a05004 	movne	r5, #4	; 0x4
    6ccc:	128d6008 	addne	r6, sp, #8	; 0x8
    6cd0:	1a000007 	bne	6cf4 <sd_change_freq+0x17c>
    6cd4:	ea000020 	b	6d5c <sd_change_freq+0x1e4>
    6cd8:	e58d6000 	str	r6, [sp]
    6cdc:	ebfffee8 	bl	6884 <sd_switch>
    6ce0:	e3500000 	cmp	r0, #0	; 0x0
    6ce4:	1a00001d 	bne	6d60 <sd_change_freq+0x1e8>
    6ce8:	e59d3024 	ldr	r3, [sp, #36]
    6cec:	e3130c02 	tst	r3, #512	; 0x200
    6cf0:	0a000005 	beq	6d0c <sd_change_freq+0x194>
    6cf4:	e3a01000 	mov	r1, #0	; 0x0
    6cf8:	e2555001 	subs	r5, r5, #1	; 0x1
    6cfc:	e1a00004 	mov	r0, r4
    6d00:	e1a02001 	mov	r2, r1
    6d04:	e3a03001 	mov	r3, #1	; 0x1
    6d08:	2afffff2 	bcs	6cd8 <sd_change_freq+0x160>
    6d0c:	e59d3014 	ldr	r3, [sp, #20]
    6d10:	e3130c02 	tst	r3, #512	; 0x200
    6d14:	0a000010 	beq	6d5c <sd_change_freq+0x1e4>
    6d18:	e3a01001 	mov	r1, #1	; 0x1
    6d1c:	e28dc008 	add	ip, sp, #8	; 0x8
    6d20:	e1a00004 	mov	r0, r4
    6d24:	e3a02000 	mov	r2, #0	; 0x0
    6d28:	e1a03001 	mov	r3, r1
    6d2c:	e58dc000 	str	ip, [sp]
    6d30:	ebfffed3 	bl	6884 <sd_switch>
    6d34:	e3500000 	cmp	r0, #0	; 0x0
    6d38:	1a000008 	bne	6d60 <sd_change_freq+0x1e8>
    6d3c:	e59d3018 	ldr	r3, [sp, #24]
    6d40:	e1a03c03 	lsl	r3, r3, #24
    6d44:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    6d48:	e3530401 	cmp	r3, #16777216	; 0x1000000
    6d4c:	0594304c 	ldreq	r3, [r4, #76]
    6d50:	03833001 	orreq	r3, r3, #1	; 0x1
    6d54:	0584304c 	streq	r3, [r4, #76]
    6d58:	ea000000 	b	6d60 <sd_change_freq+0x1e8>
    6d5c:	e1a0000b 	mov	r0, fp
    6d60:	e28dd084 	add	sp, sp, #132	; 0x84
    6d64:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6d68:	0002001a 	.word	0x0002001a
    6d6c:	00020020 	.word	0x00020020
    6d70:	00020010 	.word	0x00020010

00006d74 <sd_send_op_cond>:
    6d74:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    6d78:	e59f9140 	ldr	r9, [pc, #320]	; 6ec0 <sd_send_op_cond+0x14c>
    6d7c:	e24dd020 	sub	sp, sp, #32	; 0x20
    6d80:	e1a04000 	mov	r4, r0
    6d84:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    6d88:	e3a0a015 	mov	sl, #21	; 0x15
    6d8c:	e3a06000 	mov	r6, #0	; 0x0
    6d90:	e1a0700d 	mov	r7, sp
    6d94:	e3a08001 	mov	r8, #1	; 0x1
    6d98:	e3a03037 	mov	r3, #55	; 0x37
    6d9c:	e1cd30b0 	strh	r3, [sp]
    6da0:	e1a0100d 	mov	r1, sp
    6da4:	e1a02006 	mov	r2, r6
    6da8:	e58da004 	str	sl, [sp, #4]
    6dac:	e58d6008 	str	r6, [sp, #8]
    6db0:	e58d601c 	str	r6, [sp, #28]
    6db4:	e1a00004 	mov	r0, r4
    6db8:	e1a0e00f 	mov	lr, pc
    6dbc:	e594f104 	ldr	pc, [r4, #260]
    6dc0:	e2503000 	subs	r3, r0, #0	; 0x0
    6dc4:	e1a0100d 	mov	r1, sp
    6dc8:	e3a02000 	mov	r2, #0	; 0x0
    6dcc:	e59fe0f0 	ldr	lr, [pc, #240]	; 6ec4 <sd_send_op_cond+0x150>
    6dd0:	e1a00004 	mov	r0, r4
    6dd4:	1a000036 	bne	6eb4 <sd_send_op_cond+0x140>
    6dd8:	e594302c 	ldr	r3, [r4, #44]
    6ddc:	e594c030 	ldr	ip, [r4, #48]
    6de0:	e003e00e 	and	lr, r3, lr
    6de4:	e15c0009 	cmp	ip, r9
    6de8:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    6dec:	e58de008 	str	lr, [sp, #8]
    6df0:	058d3008 	streq	r3, [sp, #8]
    6df4:	e3a03029 	mov	r3, #41	; 0x29
    6df8:	e1cd30b0 	strh	r3, [sp]
    6dfc:	e58d8004 	str	r8, [sp, #4]
    6e00:	e5943104 	ldr	r3, [r4, #260]
    6e04:	e12fff33 	blx	r3
    6e08:	e2503000 	subs	r3, r0, #0	; 0x0
    6e0c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    6e10:	1a000027 	bne	6eb4 <sd_send_op_cond+0x140>
    6e14:	eb000637 	bl	86f8 <udelay>
    6e18:	e59d300c 	ldr	r3, [sp, #12]
    6e1c:	e3530000 	cmp	r3, #0	; 0x0
    6e20:	ba000002 	blt	6e30 <sd_send_op_cond+0xbc>
    6e24:	e2555001 	subs	r5, r5, #1	; 0x1
    6e28:	2affffda 	bcs	6d98 <sd_send_op_cond+0x24>
    6e2c:	ea00001f 	b	6eb0 <sd_send_op_cond+0x13c>
    6e30:	e3550000 	cmp	r5, #0	; 0x0
    6e34:	da00001d 	ble	6eb0 <sd_send_op_cond+0x13c>
    6e38:	e59f3080 	ldr	r3, [pc, #128]	; 6ec0 <sd_send_op_cond+0x14c>
    6e3c:	e5942030 	ldr	r2, [r4, #48]
    6e40:	e1520003 	cmp	r2, r3
    6e44:	12433010 	subne	r3, r3, #16	; 0x10
    6e48:	15843030 	strne	r3, [r4, #48]
    6e4c:	e5943050 	ldr	r3, [r4, #80]
    6e50:	e3130b01 	tst	r3, #1024	; 0x400
    6e54:	0a00000c 	beq	6e8c <sd_send_op_cond+0x118>
    6e58:	e3a03001 	mov	r3, #1	; 0x1
    6e5c:	e3a02000 	mov	r2, #0	; 0x0
    6e60:	e58d3004 	str	r3, [sp, #4]
    6e64:	e3a0303a 	mov	r3, #58	; 0x3a
    6e68:	e1cd30b0 	strh	r3, [sp]
    6e6c:	e58d2008 	str	r2, [sp, #8]
    6e70:	e58d201c 	str	r2, [sp, #28]
    6e74:	e1a00004 	mov	r0, r4
    6e78:	e1a0100d 	mov	r1, sp
    6e7c:	e1a0e00f 	mov	lr, pc
    6e80:	e594f104 	ldr	pc, [r4, #260]
    6e84:	e2503000 	subs	r3, r0, #0	; 0x0
    6e88:	1a000009 	bne	6eb4 <sd_send_op_cond+0x140>
    6e8c:	e59d200c 	ldr	r2, [sp, #12]
    6e90:	e3a03000 	mov	r3, #0	; 0x0
    6e94:	e1c438b0 	strh	r3, [r4, #128]
    6e98:	e1a03f22 	lsr	r3, r2, #30
    6e9c:	e2033001 	and	r3, r3, #1	; 0x1
    6ea0:	e5843040 	str	r3, [r4, #64]
    6ea4:	e5842054 	str	r2, [r4, #84]
    6ea8:	e3a03000 	mov	r3, #0	; 0x0
    6eac:	ea000000 	b	6eb4 <sd_send_op_cond+0x140>
    6eb0:	e3e03010 	mvn	r3, #16	; 0x10
    6eb4:	e1a00003 	mov	r0, r3
    6eb8:	e28dd020 	add	sp, sp, #32	; 0x20
    6ebc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    6ec0:	00020020 	.word	0x00020020
    6ec4:	00ff8000 	.word	0x00ff8000

00006ec8 <mmc_go_idle>:
    6ec8:	e92d4010 	push	{r4, lr}
    6ecc:	e1a04000 	mov	r4, r0
    6ed0:	e24dd020 	sub	sp, sp, #32	; 0x20
    6ed4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    6ed8:	eb000606 	bl	86f8 <udelay>
    6edc:	e3a02000 	mov	r2, #0	; 0x0
    6ee0:	e1a00004 	mov	r0, r4
    6ee4:	e1cd20b0 	strh	r2, [sp]
    6ee8:	e58d2008 	str	r2, [sp, #8]
    6eec:	e58d2004 	str	r2, [sp, #4]
    6ef0:	e58d201c 	str	r2, [sp, #28]
    6ef4:	e1a0100d 	mov	r1, sp
    6ef8:	e1a0e00f 	mov	lr, pc
    6efc:	e594f104 	ldr	pc, [r4, #260]
    6f00:	e2504000 	subs	r4, r0, #0	; 0x0
    6f04:	1a000001 	bne	6f10 <mmc_go_idle+0x48>
    6f08:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    6f0c:	eb0005f9 	bl	86f8 <udelay>
    6f10:	e1a00004 	mov	r0, r4
    6f14:	e28dd020 	add	sp, sp, #32	; 0x20
    6f18:	e8bd8010 	pop	{r4, pc}

00006f1c <mmc_send_op_cond>:
    6f1c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    6f20:	e24dd020 	sub	sp, sp, #32	; 0x20
    6f24:	e1a05000 	mov	r5, r0
    6f28:	e3a06001 	mov	r6, #1	; 0x1
    6f2c:	ebffffe5 	bl	6ec8 <mmc_go_idle>
    6f30:	e3a02000 	mov	r2, #0	; 0x0
    6f34:	e1cd60b0 	strh	r6, [sp]
    6f38:	e58d6004 	str	r6, [sp, #4]
    6f3c:	e58d2008 	str	r2, [sp, #8]
    6f40:	e58d201c 	str	r2, [sp, #28]
    6f44:	e1a00005 	mov	r0, r5
    6f48:	e1a0100d 	mov	r1, sp
    6f4c:	e1a0e00f 	mov	lr, pc
    6f50:	e595f104 	ldr	pc, [r5, #260]
    6f54:	e2504000 	subs	r4, r0, #0	; 0x0
    6f58:	e1a0700d 	mov	r7, sp
    6f5c:	1a000039 	bne	7048 <mmc_send_op_cond+0x12c>
    6f60:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    6f64:	eb0005e3 	bl	86f8 <udelay>
    6f68:	e1a08006 	mov	r8, r6
    6f6c:	e1a0700d 	mov	r7, sp
    6f70:	e1a06004 	mov	r6, r4
    6f74:	e3024710 	movw	r4, #10000	; 0x2710
    6f78:	e595302c 	ldr	r3, [r5, #44]
    6f7c:	e3a02001 	mov	r2, #1	; 0x1
    6f80:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    6f84:	e1cd20b0 	strh	r2, [sp]
    6f88:	e58d3008 	str	r3, [sp, #8]
    6f8c:	e1a02006 	mov	r2, r6
    6f90:	e58d8004 	str	r8, [sp, #4]
    6f94:	e58d601c 	str	r6, [sp, #28]
    6f98:	e1a0100d 	mov	r1, sp
    6f9c:	e1a00005 	mov	r0, r5
    6fa0:	e1a0e00f 	mov	lr, pc
    6fa4:	e595f104 	ldr	pc, [r5, #260]
    6fa8:	e2502000 	subs	r2, r0, #0	; 0x0
    6fac:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    6fb0:	11a04002 	movne	r4, r2
    6fb4:	1a000023 	bne	7048 <mmc_send_op_cond+0x12c>
    6fb8:	eb0005ce 	bl	86f8 <udelay>
    6fbc:	e59d300c 	ldr	r3, [sp, #12]
    6fc0:	e3530000 	cmp	r3, #0	; 0x0
    6fc4:	ba000002 	blt	6fd4 <mmc_send_op_cond+0xb8>
    6fc8:	e2544001 	subs	r4, r4, #1	; 0x1
    6fcc:	2affffe9 	bcs	6f78 <mmc_send_op_cond+0x5c>
    6fd0:	ea00001b 	b	7044 <mmc_send_op_cond+0x128>
    6fd4:	e3540000 	cmp	r4, #0	; 0x0
    6fd8:	da000019 	ble	7044 <mmc_send_op_cond+0x128>
    6fdc:	e5953050 	ldr	r3, [r5, #80]
    6fe0:	e3130b01 	tst	r3, #1024	; 0x400
    6fe4:	0a00000b 	beq	7018 <mmc_send_op_cond+0xfc>
    6fe8:	e3a0303a 	mov	r3, #58	; 0x3a
    6fec:	e58d8004 	str	r8, [sp, #4]
    6ff0:	e1cd30b0 	strh	r3, [sp]
    6ff4:	e58d6008 	str	r6, [sp, #8]
    6ff8:	e58d601c 	str	r6, [sp, #28]
    6ffc:	e1a0100d 	mov	r1, sp
    7000:	e1a02006 	mov	r2, r6
    7004:	e1a00005 	mov	r0, r5
    7008:	e1a0e00f 	mov	lr, pc
    700c:	e595f104 	ldr	pc, [r5, #260]
    7010:	e2504000 	subs	r4, r0, #0	; 0x0
    7014:	1a00000b 	bne	7048 <mmc_send_op_cond+0x12c>
    7018:	e59d300c 	ldr	r3, [sp, #12]
    701c:	e3a01801 	mov	r1, #65536	; 0x10000
    7020:	e1a02f23 	lsr	r2, r3, #30
    7024:	e2022001 	and	r2, r2, #1	; 0x1
    7028:	e5852040 	str	r2, [r5, #64]
    702c:	e3a02000 	mov	r2, #0	; 0x0
    7030:	e5851030 	str	r1, [r5, #48]
    7034:	e1c528b0 	strh	r2, [r5, #128]
    7038:	e5853054 	str	r3, [r5, #84]
    703c:	e3a04000 	mov	r4, #0	; 0x0
    7040:	ea000000 	b	7048 <mmc_send_op_cond+0x12c>
    7044:	e3e04010 	mvn	r4, #16	; 0x10
    7048:	e1a00004 	mov	r0, r4
    704c:	e28dd020 	add	sp, sp, #32	; 0x20
    7050:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00007054 <mmc_send_status>:
    7054:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7058:	e1d038b0 	ldrh	r3, [r0, #128]
    705c:	e24dd024 	sub	sp, sp, #36	; 0x24
    7060:	e1a03803 	lsl	r3, r3, #16
    7064:	e58d3008 	str	r3, [sp, #8]
    7068:	e3a03000 	mov	r3, #0	; 0x0
    706c:	e3a02015 	mov	r2, #21	; 0x15
    7070:	e58d301c 	str	r3, [sp, #28]
    7074:	e3a0300d 	mov	r3, #13	; 0xd
    7078:	e1a06000 	mov	r6, r0
    707c:	e58d2004 	str	r2, [sp, #4]
    7080:	e1a04001 	mov	r4, r1
    7084:	e1cd30b0 	strh	r3, [sp]
    7088:	e1a0700d 	mov	r7, sp
    708c:	e1a0100d 	mov	r1, sp
    7090:	e3a02000 	mov	r2, #0	; 0x0
    7094:	e1a00006 	mov	r0, r6
    7098:	e1a0e00f 	mov	lr, pc
    709c:	e596f104 	ldr	pc, [r6, #260]
    70a0:	e2505000 	subs	r5, r0, #0	; 0x0
    70a4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    70a8:	1a000018 	bne	7110 <mmc_send_status+0xbc>
    70ac:	e59d300c 	ldr	r3, [sp, #12]
    70b0:	e3130c01 	tst	r3, #256	; 0x100
    70b4:	1a00000e 	bne	70f4 <mmc_send_status+0xa0>
    70b8:	eb00058e 	bl	86f8 <udelay>
    70bc:	e59d100c 	ldr	r1, [sp, #12]
    70c0:	e59f3054 	ldr	r3, [pc, #84]	; 711c <mmc_send_status+0xc8>
    70c4:	e2444001 	sub	r4, r4, #1	; 0x1
    70c8:	e0013003 	and	r3, r1, r3
    70cc:	e3530000 	cmp	r3, #0	; 0x0
    70d0:	0a000005 	beq	70ec <mmc_send_status+0x98>
    70d4:	e59f0044 	ldr	r0, [pc, #68]	; 7120 <mmc_send_status+0xcc>
    70d8:	e59f3044 	ldr	r3, [pc, #68]	; 7124 <mmc_send_status+0xd0>
    70dc:	e1a0e00f 	mov	lr, pc
    70e0:	e593f000 	ldr	pc, [r3]
    70e4:	e3e05011 	mvn	r5, #17	; 0x11
    70e8:	ea000008 	b	7110 <mmc_send_status+0xbc>
    70ec:	e3740001 	cmn	r4, #1	; 0x1
    70f0:	1affffe5 	bne	708c <mmc_send_status+0x38>
    70f4:	e3540000 	cmp	r4, #0	; 0x0
    70f8:	1a000004 	bne	7110 <mmc_send_status+0xbc>
    70fc:	e59f0024 	ldr	r0, [pc, #36]	; 7128 <mmc_send_status+0xd4>
    7100:	e59f301c 	ldr	r3, [pc, #28]	; 7124 <mmc_send_status+0xd0>
    7104:	e1a0e00f 	mov	lr, pc
    7108:	e593f000 	ldr	pc, [r3]
    710c:	e3e05012 	mvn	r5, #18	; 0x12
    7110:	e1a00005 	mov	r0, r5
    7114:	e28dd024 	add	sp, sp, #36	; 0x24
    7118:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    711c:	fdf94080 	.word	0xfdf94080
    7120:	000112bc 	.word	0x000112bc
    7124:	3c00d68c 	.word	0x3c00d68c
    7128:	000112d2 	.word	0x000112d2

0000712c <mmc_bwrite>:
    712c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7130:	e24dd034 	sub	sp, sp, #52	; 0x34
    7134:	e1a06001 	mov	r6, r1
    7138:	e1a0b002 	mov	fp, r2
    713c:	e1a0a003 	mov	sl, r3
    7140:	ebfffda2 	bl	67d0 <find_mmc_device>
    7144:	e2504000 	subs	r4, r0, #0	; 0x0
    7148:	0a000060 	beq	72d0 <mmc_bwrite+0x1a4>
    714c:	e594108c 	ldr	r1, [r4, #140]
    7150:	ebfffd90 	bl	6798 <mmc_set_blocklen>
    7154:	e3500000 	cmp	r0, #0	; 0x0
    7158:	01a09000 	moveq	r9, r0
    715c:	01a0700b 	moveq	r7, fp
    7160:	0a000005 	beq	717c <mmc_bwrite+0x50>
    7164:	e594108c 	ldr	r1, [r4, #140]
    7168:	e59f016c 	ldr	r0, [pc, #364]	; 72dc <mmc_bwrite+0x1b0>
    716c:	e59f316c 	ldr	r3, [pc, #364]	; 72e0 <mmc_bwrite+0x1b4>
    7170:	e1a0e00f 	mov	lr, pc
    7174:	e593f000 	ldr	pc, [r3]
    7178:	ea000054 	b	72d0 <mmc_bwrite+0x1a4>
    717c:	e5943110 	ldr	r3, [r4, #272]
    7180:	e59420a8 	ldr	r2, [r4, #168]
    7184:	e1570003 	cmp	r7, r3
    7188:	31a05007 	movcc	r5, r7
    718c:	21a05003 	movcs	r5, r3
    7190:	e0858006 	add	r8, r5, r6
    7194:	e1580002 	cmp	r8, r2
    7198:	9a000005 	bls	71b4 <mmc_bwrite+0x88>
    719c:	e59f0140 	ldr	r0, [pc, #320]	; 72e4 <mmc_bwrite+0x1b8>
    71a0:	e1a01008 	mov	r1, r8
    71a4:	e59f3134 	ldr	r3, [pc, #308]	; 72e0 <mmc_bwrite+0x1b4>
    71a8:	e1a0e00f 	mov	lr, pc
    71ac:	e593f000 	ldr	pc, [r3]
    71b0:	ea00003d 	b	72ac <mmc_bwrite+0x180>
    71b4:	e3550001 	cmp	r5, #1	; 0x1
    71b8:	93a03018 	movls	r3, #24	; 0x18
    71bc:	91cd30b0 	strhls	r3, [sp]
    71c0:	e5943040 	ldr	r3, [r4, #64]
    71c4:	83a02019 	movhi	r2, #25	; 0x19
    71c8:	81cd20b0 	strhhi	r2, [sp]
    71cc:	e3530000 	cmp	r3, #0	; 0x0
    71d0:	0594308c 	ldreq	r3, [r4, #140]
    71d4:	e3a02015 	mov	r2, #21	; 0x15
    71d8:	00030693 	muleq	r3, r3, r6
    71dc:	158d6008 	strne	r6, [sp, #8]
    71e0:	058d3008 	streq	r3, [sp, #8]
    71e4:	e58d2004 	str	r2, [sp, #4]
    71e8:	e58d901c 	str	r9, [sp, #28]
    71ec:	e58da020 	str	sl, [sp, #32]
    71f0:	e58d5028 	str	r5, [sp, #40]
    71f4:	e594308c 	ldr	r3, [r4, #140]
    71f8:	e3a02002 	mov	r2, #2	; 0x2
    71fc:	e58d2024 	str	r2, [sp, #36]
    7200:	e58d302c 	str	r3, [sp, #44]
    7204:	e1a00004 	mov	r0, r4
    7208:	e1a0100d 	mov	r1, sp
    720c:	e28d2020 	add	r2, sp, #32	; 0x20
    7210:	e1a0e00f 	mov	lr, pc
    7214:	e594f104 	ldr	pc, [r4, #260]
    7218:	e3500000 	cmp	r0, #0	; 0x0
    721c:	0a000004 	beq	7234 <mmc_bwrite+0x108>
    7220:	e59f00c0 	ldr	r0, [pc, #192]	; 72e8 <mmc_bwrite+0x1bc>
    7224:	e59f30b4 	ldr	r3, [pc, #180]	; 72e0 <mmc_bwrite+0x1b4>
    7228:	e1a0e00f 	mov	lr, pc
    722c:	e593f000 	ldr	pc, [r3]
    7230:	ea00001d 	b	72ac <mmc_bwrite+0x180>
    7234:	e5943050 	ldr	r3, [r4, #80]
    7238:	e1a03523 	lsr	r3, r3, #10
    723c:	e2233001 	eor	r3, r3, #1	; 0x1
    7240:	e3550001 	cmp	r5, #1	; 0x1
    7244:	93a03000 	movls	r3, #0	; 0x0
    7248:	82033001 	andhi	r3, r3, #1	; 0x1
    724c:	e3530000 	cmp	r3, #0	; 0x0
    7250:	0a000011 	beq	729c <mmc_bwrite+0x170>
    7254:	e3a0200c 	mov	r2, #12	; 0xc
    7258:	e3a0301d 	mov	r3, #29	; 0x1d
    725c:	e1cd20b0 	strh	r2, [sp]
    7260:	e98d0208 	stmib	sp, {r3, r9}
    7264:	e58d901c 	str	r9, [sp, #28]
    7268:	e1a00004 	mov	r0, r4
    726c:	e1a0100d 	mov	r1, sp
    7270:	e1a02009 	mov	r2, r9
    7274:	e1a0e00f 	mov	lr, pc
    7278:	e594f104 	ldr	pc, [r4, #260]
    727c:	e3500000 	cmp	r0, #0	; 0x0
    7280:	0a000005 	beq	729c <mmc_bwrite+0x170>
    7284:	e59f0060 	ldr	r0, [pc, #96]	; 72ec <mmc_bwrite+0x1c0>
    7288:	e59f3050 	ldr	r3, [pc, #80]	; 72e0 <mmc_bwrite+0x1b4>
    728c:	e1a0e00f 	mov	lr, pc
    7290:	e593f000 	ldr	pc, [r3]
    7294:	e1a00009 	mov	r0, r9
    7298:	ea00000d 	b	72d4 <mmc_bwrite+0x1a8>
    729c:	e1a00004 	mov	r0, r4
    72a0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    72a4:	ebffff6a 	bl	7054 <mmc_send_status>
    72a8:	ea000001 	b	72b4 <mmc_bwrite+0x188>
    72ac:	e3550000 	cmp	r5, #0	; 0x0
    72b0:	1a000006 	bne	72d0 <mmc_bwrite+0x1a4>
    72b4:	e0577005 	subs	r7, r7, r5
    72b8:	e594008c 	ldr	r0, [r4, #140]
    72bc:	102aa590 	mlane	sl, r0, r5, sl
    72c0:	11a06008 	movne	r6, r8
    72c4:	1affffac 	bne	717c <mmc_bwrite+0x50>
    72c8:	e000009b 	mul	r0, fp, r0
    72cc:	ea000000 	b	72d4 <mmc_bwrite+0x1a8>
    72d0:	e3a00000 	mov	r0, #0	; 0x0
    72d4:	e28dd034 	add	sp, sp, #52	; 0x34
    72d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    72dc:	000112ee 	.word	0x000112ee
    72e0:	3c00d68c 	.word	0x3c00d68c
    72e4:	000112f8 	.word	0x000112f8
    72e8:	00011324 	.word	0x00011324
    72ec:	00011336 	.word	0x00011336

000072f0 <mmc2_vs_write>:
    72f0:	e1a024a2 	lsr	r2, r2, #9
    72f4:	e1821b83 	orr	r1, r2, r3, lsl #23
    72f8:	e59d2000 	ldr	r2, [sp]
    72fc:	e1a03000 	mov	r3, r0
    7300:	e1a024c2 	asr	r2, r2, #9
    7304:	e3a00002 	mov	r0, #2	; 0x2
    7308:	eaffff87 	b	712c <mmc_bwrite>

0000730c <mmc1_vs_write>:
    730c:	e1a024a2 	lsr	r2, r2, #9
    7310:	e1821b83 	orr	r1, r2, r3, lsl #23
    7314:	e59d2000 	ldr	r2, [sp]
    7318:	e1a03000 	mov	r3, r0
    731c:	e1a024c2 	asr	r2, r2, #9
    7320:	e3a00001 	mov	r0, #1	; 0x1
    7324:	eaffff80 	b	712c <mmc_bwrite>

00007328 <mmc0_vs_write>:
    7328:	e1a024a2 	lsr	r2, r2, #9
    732c:	e1821b83 	orr	r1, r2, r3, lsl #23
    7330:	e59d2000 	ldr	r2, [sp]
    7334:	e1a03000 	mov	r3, r0
    7338:	e1a024c2 	asr	r2, r2, #9
    733c:	e3a00000 	mov	r0, #0	; 0x0
    7340:	eaffff79 	b	712c <mmc_bwrite>

00007344 <mmc_switch>:
    7344:	e92d4030 	push	{r4, r5, lr}
    7348:	e1a02802 	lsl	r2, r2, #16
    734c:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    7350:	e24dd024 	sub	sp, sp, #36	; 0x24
    7354:	e1822403 	orr	r2, r2, r3, lsl #8
    7358:	e3a0301d 	mov	r3, #29	; 0x1d
    735c:	e3a01000 	mov	r1, #0	; 0x0
    7360:	e58d3004 	str	r3, [sp, #4]
    7364:	e3a03006 	mov	r3, #6	; 0x6
    7368:	e58d2008 	str	r2, [sp, #8]
    736c:	e1cd30b0 	strh	r3, [sp]
    7370:	e1a02001 	mov	r2, r1
    7374:	e58d101c 	str	r1, [sp, #28]
    7378:	e1a0100d 	mov	r1, sp
    737c:	e1a04000 	mov	r4, r0
    7380:	e1a0e00f 	mov	lr, pc
    7384:	e590f104 	ldr	pc, [r0, #260]
    7388:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    738c:	e1a05000 	mov	r5, r0
    7390:	e1a00004 	mov	r0, r4
    7394:	ebffff2e 	bl	7054 <mmc_send_status>
    7398:	e1a00005 	mov	r0, r5
    739c:	e28dd024 	add	sp, sp, #36	; 0x24
    73a0:	e8bd8030 	pop	{r4, r5, pc}

000073a4 <mmc_change_freq>:
    73a4:	e92d4070 	push	{r4, r5, r6, lr}
    73a8:	e5903050 	ldr	r3, [r0, #80]
    73ac:	e24ddc02 	sub	sp, sp, #512	; 0x200
    73b0:	e3130b01 	tst	r3, #1024	; 0x400
    73b4:	e3a03000 	mov	r3, #0	; 0x0
    73b8:	e1a04000 	mov	r4, r0
    73bc:	e580304c 	str	r3, [r0, #76]
    73c0:	1a000020 	bne	7448 <mmc_change_freq+0xa4>
    73c4:	e5902030 	ldr	r2, [r0, #48]
    73c8:	e59f3084 	ldr	r3, [pc, #132]	; 7454 <mmc_change_freq+0xb0>
    73cc:	e1520003 	cmp	r2, r3
    73d0:	9a00001c 	bls	7448 <mmc_change_freq+0xa4>
    73d4:	e3a03c01 	mov	r3, #256	; 0x100
    73d8:	e580304c 	str	r3, [r0, #76]
    73dc:	e1a0100d 	mov	r1, sp
    73e0:	ebfffd12 	bl	6830 <mmc_send_ext_csd>
    73e4:	e3500000 	cmp	r0, #0	; 0x0
    73e8:	e1a0600d 	mov	r6, sp
    73ec:	1a000016 	bne	744c <mmc_change_freq+0xa8>
    73f0:	e3a01001 	mov	r1, #1	; 0x1
    73f4:	e1a00004 	mov	r0, r4
    73f8:	e3a020b9 	mov	r2, #185	; 0xb9
    73fc:	e1a03001 	mov	r3, r1
    7400:	e5dd50c4 	ldrb	r5, [sp, #196]
    7404:	ebffffce 	bl	7344 <mmc_switch>
    7408:	e3500000 	cmp	r0, #0	; 0x0
    740c:	1a00000e 	bne	744c <mmc_change_freq+0xa8>
    7410:	e1a0100d 	mov	r1, sp
    7414:	e1a00004 	mov	r0, r4
    7418:	ebfffd04 	bl	6830 <mmc_send_ext_csd>
    741c:	e3500000 	cmp	r0, #0	; 0x0
    7420:	1a000009 	bne	744c <mmc_change_freq+0xa8>
    7424:	e5dd30b9 	ldrb	r3, [sp, #185]
    7428:	e3530000 	cmp	r3, #0	; 0x0
    742c:	0a000006 	beq	744c <mmc_change_freq+0xa8>
    7430:	e594304c 	ldr	r3, [r4, #76]
    7434:	e3150002 	tst	r5, #2	; 0x2
    7438:	13833011 	orrne	r3, r3, #17	; 0x11
    743c:	03833001 	orreq	r3, r3, #1	; 0x1
    7440:	e584304c 	str	r3, [r4, #76]
    7444:	ea000000 	b	744c <mmc_change_freq+0xa8>
    7448:	e3a00000 	mov	r0, #0	; 0x0
    744c:	e28ddc02 	add	sp, sp, #512	; 0x200
    7450:	e8bd8070 	pop	{r4, r5, r6, pc}
    7454:	0001003f 	.word	0x0001003f

00007458 <mmc_startup>:
    7458:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    745c:	e5903050 	ldr	r3, [r0, #80]
    7460:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    7464:	e3130b01 	tst	r3, #1024	; 0x400
    7468:	13a0300a 	movne	r3, #10	; 0xa
    746c:	03a03002 	moveq	r3, #2	; 0x2
    7470:	e28d1c02 	add	r1, sp, #512	; 0x200
    7474:	e3a02000 	mov	r2, #0	; 0x0
    7478:	e1c131b0 	strh	r3, [r1, #16]
    747c:	e28d5e21 	add	r5, sp, #528	; 0x210
    7480:	e3a03007 	mov	r3, #7	; 0x7
    7484:	e58d3214 	str	r3, [sp, #532]
    7488:	e58d2218 	str	r2, [sp, #536]
    748c:	e58d222c 	str	r2, [sp, #556]
    7490:	e1a01005 	mov	r1, r5
    7494:	e1a06000 	mov	r6, r0
    7498:	e1a0e00f 	mov	lr, pc
    749c:	e590f104 	ldr	pc, [r0, #260]
    74a0:	e2504000 	subs	r4, r0, #0	; 0x0
    74a4:	1a000159 	bne	7a10 <mmc_startup+0x5b8>
    74a8:	e3a02010 	mov	r2, #16	; 0x10
    74ac:	e2860070 	add	r0, r6, #112	; 0x70
    74b0:	e285100c 	add	r1, r5, #12	; 0xc
    74b4:	ebffe743 	bl	11c8 <memcpy>
    74b8:	e5963050 	ldr	r3, [r6, #80]
    74bc:	e2132b01 	ands	r2, r3, #1024	; 0x400
    74c0:	1a000013 	bne	7514 <mmc_startup+0xbc>
    74c4:	e1d638b0 	ldrh	r3, [r6, #128]
    74c8:	e3a00003 	mov	r0, #3	; 0x3
    74cc:	e1a03803 	lsl	r3, r3, #16
    74d0:	e58d3218 	str	r3, [sp, #536]
    74d4:	e3a03015 	mov	r3, #21	; 0x15
    74d8:	e58d3214 	str	r3, [sp, #532]
    74dc:	e28d3c02 	add	r3, sp, #512	; 0x200
    74e0:	e1c301b0 	strh	r0, [r3, #16]
    74e4:	e58d222c 	str	r2, [sp, #556]
    74e8:	e1a01005 	mov	r1, r5
    74ec:	e1a00006 	mov	r0, r6
    74f0:	e1a0e00f 	mov	lr, pc
    74f4:	e596f104 	ldr	pc, [r6, #260]
    74f8:	e2504000 	subs	r4, r0, #0	; 0x0
    74fc:	1a000143 	bne	7a10 <mmc_startup+0x5b8>
    7500:	e5963030 	ldr	r3, [r6, #48]
    7504:	e3130802 	tst	r3, #131072	; 0x20000
    7508:	128d1c02 	addne	r1, sp, #512	; 0x200
    750c:	11d111be 	ldrhne	r1, [r1, #30]
    7510:	11c618b0 	strhne	r1, [r6, #128]
    7514:	e1d638b0 	ldrh	r3, [r6, #128]
    7518:	e3a01000 	mov	r1, #0	; 0x0
    751c:	e1a03803 	lsl	r3, r3, #16
    7520:	e3a02007 	mov	r2, #7	; 0x7
    7524:	e58d3218 	str	r3, [sp, #536]
    7528:	e3a00009 	mov	r0, #9	; 0x9
    752c:	e28d3c02 	add	r3, sp, #512	; 0x200
    7530:	e1c301b0 	strh	r0, [r3, #16]
    7534:	e58d2214 	str	r2, [sp, #532]
    7538:	e58d122c 	str	r1, [sp, #556]
    753c:	e1a02001 	mov	r2, r1
    7540:	e1a00006 	mov	r0, r6
    7544:	e28d1e21 	add	r1, sp, #528	; 0x210
    7548:	e1a0e00f 	mov	lr, pc
    754c:	e596f104 	ldr	pc, [r6, #260]
    7550:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7554:	e1a04000 	mov	r4, r0
    7558:	e1a00006 	mov	r0, r6
    755c:	ebfffebc 	bl	7054 <mmc_send_status>
    7560:	e3540000 	cmp	r4, #0	; 0x0
    7564:	1a000129 	bne	7a10 <mmc_startup+0x5b8>
    7568:	e5963030 	ldr	r3, [r6, #48]
    756c:	e59d021c 	ldr	r0, [sp, #540]
    7570:	e59d2224 	ldr	r2, [sp, #548]
    7574:	e59d1228 	ldr	r1, [sp, #552]
    7578:	e3530801 	cmp	r3, #65536	; 0x10000
    757c:	e59d3220 	ldr	r3, [sp, #544]
    7580:	e5862068 	str	r2, [r6, #104]
    7584:	e5863064 	str	r3, [r6, #100]
    7588:	e586106c 	str	r1, [r6, #108]
    758c:	e5860060 	str	r0, [r6, #96]
    7590:	1a000013 	bne	75e4 <mmc_startup+0x18c>
    7594:	e1a03d20 	lsr	r3, r0, #26
    7598:	e203300f 	and	r3, r3, #15	; 0xf
    759c:	e3530004 	cmp	r3, #4	; 0x4
    75a0:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    75a4:	ea00000c 	b	75dc <mmc_startup+0x184>
    75a8:	000075dc 	.word	0x000075dc
    75ac:	000075bc 	.word	0x000075bc
    75b0:	000075c4 	.word	0x000075c4
    75b4:	000075cc 	.word	0x000075cc
    75b8:	000075d4 	.word	0x000075d4
    75bc:	e59f3458 	ldr	r3, [pc, #1112]	; 7a1c <mmc_startup+0x5c4>
    75c0:	ea000006 	b	75e0 <mmc_startup+0x188>
    75c4:	e59f3454 	ldr	r3, [pc, #1108]	; 7a20 <mmc_startup+0x5c8>
    75c8:	ea000004 	b	75e0 <mmc_startup+0x188>
    75cc:	e59f3450 	ldr	r3, [pc, #1104]	; 7a24 <mmc_startup+0x5cc>
    75d0:	ea000002 	b	75e0 <mmc_startup+0x188>
    75d4:	e59f344c 	ldr	r3, [pc, #1100]	; 7a28 <mmc_startup+0x5d0>
    75d8:	ea000000 	b	75e0 <mmc_startup+0x188>
    75dc:	e59f3448 	ldr	r3, [pc, #1096]	; 7a2c <mmc_startup+0x5d4>
    75e0:	e5863030 	str	r3, [r6, #48]
    75e4:	e28d1c02 	add	r1, sp, #512	; 0x200
    75e8:	e1d132b2 	ldrh	r3, [r1, #34]
    75ec:	e3a0e001 	mov	lr, #1	; 0x1
    75f0:	e203300f 	and	r3, r3, #15	; 0xf
    75f4:	e1a0c31e 	lsl	ip, lr, r3
    75f8:	e59d121c 	ldr	r1, [sp, #540]
    75fc:	e59f242c 	ldr	r2, [pc, #1068]	; 7a30 <mmc_startup+0x5d8>
    7600:	e1a031a1 	lsr	r3, r1, #3
    7604:	e203300f 	and	r3, r3, #15	; 0xf
    7608:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    760c:	e59f3420 	ldr	r3, [pc, #1056]	; 7a34 <mmc_startup+0x5dc>
    7610:	e2011007 	and	r1, r1, #7	; 0x7
    7614:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    7618:	e5962030 	ldr	r2, [r6, #48]
    761c:	e0030093 	mul	r3, r3, r0
    7620:	e3120802 	tst	r2, #131072	; 0x20000
    7624:	e5863084 	str	r3, [r6, #132]
    7628:	e586c088 	str	ip, [r6, #136]
    762c:	059d3228 	ldreq	r3, [sp, #552]
    7630:	1586c08c 	strne	ip, [r6, #140]
    7634:	01a03b23 	lsreq	r3, r3, #22
    7638:	0203300f 	andeq	r3, r3, #15	; 0xf
    763c:	01a0331e 	lsleq	r3, lr, r3
    7640:	0586308c 	streq	r3, [r6, #140]
    7644:	e5963040 	ldr	r3, [r6, #64]
    7648:	e5962064 	ldr	r2, [r6, #100]
    764c:	e3530000 	cmp	r3, #0	; 0x0
    7650:	e5960068 	ldr	r0, [r6, #104]
    7654:	0a000007 	beq	7678 <mmc_startup+0x220>
    7658:	e202203f 	and	r2, r2, #63	; 0x3f
    765c:	e1a03820 	lsr	r3, r0, #16
    7660:	e1833802 	orr	r3, r3, r2, lsl #16
    7664:	e1a08003 	mov	r8, r3
    7668:	e3a09000 	mov	r9, #0	; 0x0
    766c:	e3a04008 	mov	r4, #8	; 0x8
    7670:	e3a05000 	mov	r5, #0	; 0x0
    7674:	ea000009 	b	76a0 <mmc_startup+0x248>
    7678:	e1a03b02 	lsl	r3, r2, #22
    767c:	e1a03b23 	lsr	r3, r3, #22
    7680:	e200190e 	and	r1, r0, #229376	; 0x38000
    7684:	e1a02f20 	lsr	r2, r0, #30
    7688:	e1822103 	orr	r2, r2, r3, lsl #2
    768c:	e1a017a1 	lsr	r1, r1, #15
    7690:	e1a08002 	mov	r8, r2
    7694:	e3a09000 	mov	r9, #0	; 0x0
    7698:	e1a04001 	mov	r4, r1
    769c:	e3a05000 	mov	r5, #0	; 0x0
    76a0:	e3a02001 	mov	r2, #1	; 0x1
    76a4:	e0980002 	adds	r0, r8, r2
    76a8:	e3a03000 	mov	r3, #0	; 0x0
    76ac:	e0a91003 	adc	r1, r9, r3
    76b0:	e2842002 	add	r2, r4, #2	; 0x2
    76b4:	eb001be6 	bl	e654 <__aeabi_llsl>
    76b8:	e596c088 	ldr	ip, [r6, #136]
    76bc:	e0832c90 	umull	r2, r3, r0, ip
    76c0:	e023319c 	mla	r3, ip, r1, r3
    76c4:	e35c0c02 	cmp	ip, #512	; 0x200
    76c8:	e1c629f0 	strd	r2, [r6, #144]
    76cc:	83a03c02 	movhi	r3, #512	; 0x200
    76d0:	85863088 	strhi	r3, [r6, #136]
    76d4:	e596308c 	ldr	r3, [r6, #140]
    76d8:	e3530c02 	cmp	r3, #512	; 0x200
    76dc:	83a03c02 	movhi	r3, #512	; 0x200
    76e0:	8586308c 	strhi	r3, [r6, #140]
    76e4:	e5963050 	ldr	r3, [r6, #80]
    76e8:	e2131b01 	ands	r1, r3, #1024	; 0x400
    76ec:	1a00000f 	bne	7730 <mmc_startup+0x2d8>
    76f0:	e1d638b0 	ldrh	r3, [r6, #128]
    76f4:	e3a0201d 	mov	r2, #29	; 0x1d
    76f8:	e1a03803 	lsl	r3, r3, #16
    76fc:	e58d2214 	str	r2, [sp, #532]
    7700:	e58d3218 	str	r3, [sp, #536]
    7704:	e28d2c02 	add	r2, sp, #512	; 0x200
    7708:	e3a03007 	mov	r3, #7	; 0x7
    770c:	e1c231b0 	strh	r3, [r2, #16]
    7710:	e58d122c 	str	r1, [sp, #556]
    7714:	e1a02001 	mov	r2, r1
    7718:	e1a00006 	mov	r0, r6
    771c:	e28d1e21 	add	r1, sp, #528	; 0x210
    7720:	e1a0e00f 	mov	lr, pc
    7724:	e596f104 	ldr	pc, [r6, #260]
    7728:	e2504000 	subs	r4, r0, #0	; 0x0
    772c:	1a0000b7 	bne	7a10 <mmc_startup+0x5b8>
    7730:	e5962030 	ldr	r2, [r6, #48]
    7734:	e3e03000 	mvn	r3, #0	; 0x0
    7738:	e3120802 	tst	r2, #131072	; 0x20000
    773c:	e5c63082 	strb	r3, [r6, #130]
    7740:	1a00001e 	bne	77c0 <mmc_startup+0x368>
    7744:	e59f32ec 	ldr	r3, [pc, #748]	; 7a38 <mmc_startup+0x5e0>
    7748:	e1520003 	cmp	r2, r3
    774c:	9a00001b 	bls	77c0 <mmc_startup+0x368>
    7750:	e28d1010 	add	r1, sp, #16	; 0x10
    7754:	e1a00006 	mov	r0, r6
    7758:	ebfffc34 	bl	6830 <mmc_send_ext_csd>
    775c:	e5dd30d0 	ldrb	r3, [sp, #208]
    7760:	e2701001 	rsbs	r1, r0, #1	; 0x1
    7764:	33a01000 	movcc	r1, #0	; 0x0
    7768:	e3530001 	cmp	r3, #1	; 0x1
    776c:	93a01000 	movls	r1, #0	; 0x0
    7770:	e3510000 	cmp	r1, #0	; 0x0
    7774:	0a00000d 	beq	77b0 <mmc_startup+0x358>
    7778:	e5dd20e6 	ldrb	r2, [sp, #230]
    777c:	e5dd10e5 	ldrb	r1, [sp, #229]
    7780:	e5dd00e4 	ldrb	r0, [sp, #228]
    7784:	e1a02802 	lsl	r2, r2, #16
    7788:	e1822401 	orr	r2, r2, r1, lsl #8
    778c:	e5dd10e7 	ldrb	r1, [sp, #231]
    7790:	e1822000 	orr	r2, r2, r0
    7794:	e1822c01 	orr	r2, r2, r1, lsl #24
    7798:	e1a03fc2 	asr	r3, r2, #31
    779c:	e1a01483 	lsl	r1, r3, #9
    77a0:	e1811ba2 	orr	r1, r1, r2, lsr #23
    77a4:	e1a02482 	lsl	r2, r2, #9
    77a8:	e5861094 	str	r1, [r6, #148]
    77ac:	e5862090 	str	r2, [r6, #144]
    77b0:	e5dd30b0 	ldrb	r3, [sp, #176]
    77b4:	e3130001 	tst	r3, #1	; 0x1
    77b8:	15dd30c3 	ldrbne	r3, [sp, #195]
    77bc:	15c63082 	strbne	r3, [r6, #130]
    77c0:	e5963030 	ldr	r3, [r6, #48]
    77c4:	e3130802 	tst	r3, #131072	; 0x20000
    77c8:	0a000002 	beq	77d8 <mmc_startup+0x380>
    77cc:	e1a00006 	mov	r0, r6
    77d0:	ebfffce8 	bl	6b78 <sd_change_freq>
    77d4:	ea000001 	b	77e0 <mmc_startup+0x388>
    77d8:	e1a00006 	mov	r0, r6
    77dc:	ebfffef0 	bl	73a4 <mmc_change_freq>
    77e0:	e3500000 	cmp	r0, #0	; 0x0
    77e4:	e1a05000 	mov	r5, r0
    77e8:	11a04000 	movne	r4, r0
    77ec:	1a000087 	bne	7a10 <mmc_startup+0x5b8>
    77f0:	e596304c 	ldr	r3, [r6, #76]
    77f4:	e5962050 	ldr	r2, [r6, #80]
    77f8:	e5961030 	ldr	r1, [r6, #48]
    77fc:	e0022003 	and	r2, r2, r3
    7800:	e3110802 	tst	r1, #131072	; 0x20000
    7804:	e586204c 	str	r2, [r6, #76]
    7808:	e2023c01 	and	r3, r2, #256	; 0x100
    780c:	0a00002c 	beq	78c4 <mmc_startup+0x46c>
    7810:	e3530000 	cmp	r3, #0	; 0x0
    7814:	0a000023 	beq	78a8 <mmc_startup+0x450>
    7818:	e1d638b0 	ldrh	r3, [r6, #128]
    781c:	e28d2c02 	add	r2, sp, #512	; 0x200
    7820:	e1a03803 	lsl	r3, r3, #16
    7824:	e58d3218 	str	r3, [sp, #536]
    7828:	e3a08015 	mov	r8, #21	; 0x15
    782c:	e3a03037 	mov	r3, #55	; 0x37
    7830:	e28d9e21 	add	r9, sp, #528	; 0x210
    7834:	e1c231b0 	strh	r3, [r2, #16]
    7838:	e58d022c 	str	r0, [sp, #556]
    783c:	e58d8214 	str	r8, [sp, #532]
    7840:	e1a00006 	mov	r0, r6
    7844:	e1a01009 	mov	r1, r9
    7848:	e1a02005 	mov	r2, r5
    784c:	e1a0e00f 	mov	lr, pc
    7850:	e596f104 	ldr	pc, [r6, #260]
    7854:	e2504000 	subs	r4, r0, #0	; 0x0
    7858:	1a00006c 	bne	7a10 <mmc_startup+0x5b8>
    785c:	e3a03002 	mov	r3, #2	; 0x2
    7860:	e3a01006 	mov	r1, #6	; 0x6
    7864:	e28d0c02 	add	r0, sp, #512	; 0x200
    7868:	e1c011b0 	strh	r1, [r0, #16]
    786c:	e58d8214 	str	r8, [sp, #532]
    7870:	e58d3218 	str	r3, [sp, #536]
    7874:	e58d522c 	str	r5, [sp, #556]
    7878:	e1a01009 	mov	r1, r9
    787c:	e1a02005 	mov	r2, r5
    7880:	e1a00006 	mov	r0, r6
    7884:	e1a0e00f 	mov	lr, pc
    7888:	e596f104 	ldr	pc, [r6, #260]
    788c:	e2504000 	subs	r4, r0, #0	; 0x0
    7890:	1a00005e 	bne	7a10 <mmc_startup+0x5b8>
    7894:	e3a03004 	mov	r3, #4	; 0x4
    7898:	e5863044 	str	r3, [r6, #68]
    789c:	e1a00006 	mov	r0, r6
    78a0:	e1a0e00f 	mov	lr, pc
    78a4:	e596f108 	ldr	pc, [r6, #264]
    78a8:	e596304c 	ldr	r3, [r6, #76]
    78ac:	e3130001 	tst	r3, #1	; 0x1
    78b0:	11a00006 	movne	r0, r6
    78b4:	159f1180 	ldrne	r1, [pc, #384]	; 7a3c <mmc_startup+0x5e4>
    78b8:	01a00006 	moveq	r0, r6
    78bc:	059f117c 	ldreq	r1, [pc, #380]	; 7a40 <mmc_startup+0x5e8>
    78c0:	ea000022 	b	7950 <mmc_startup+0x4f8>
    78c4:	e3530000 	cmp	r3, #0	; 0x0
    78c8:	0a000008 	beq	78f0 <mmc_startup+0x498>
    78cc:	e3a01001 	mov	r1, #1	; 0x1
    78d0:	e1a03001 	mov	r3, r1
    78d4:	e1a00006 	mov	r0, r6
    78d8:	e3a020b7 	mov	r2, #183	; 0xb7
    78dc:	ebfffe98 	bl	7344 <mmc_switch>
    78e0:	e2504000 	subs	r4, r0, #0	; 0x0
    78e4:	03a03004 	moveq	r3, #4	; 0x4
    78e8:	1a000048 	bne	7a10 <mmc_startup+0x5b8>
    78ec:	ea000009 	b	7918 <mmc_startup+0x4c0>
    78f0:	e3120c02 	tst	r2, #512	; 0x200
    78f4:	0a00000b 	beq	7928 <mmc_startup+0x4d0>
    78f8:	e1a00006 	mov	r0, r6
    78fc:	e3a01001 	mov	r1, #1	; 0x1
    7900:	e3a020b7 	mov	r2, #183	; 0xb7
    7904:	e3a03002 	mov	r3, #2	; 0x2
    7908:	ebfffe8d 	bl	7344 <mmc_switch>
    790c:	e2504000 	subs	r4, r0, #0	; 0x0
    7910:	1a00003e 	bne	7a10 <mmc_startup+0x5b8>
    7914:	e3a03008 	mov	r3, #8	; 0x8
    7918:	e5863044 	str	r3, [r6, #68]
    791c:	e1a00006 	mov	r0, r6
    7920:	e1a0e00f 	mov	lr, pc
    7924:	e596f108 	ldr	pc, [r6, #264]
    7928:	e596304c 	ldr	r3, [r6, #76]
    792c:	e3130001 	tst	r3, #1	; 0x1
    7930:	01a00006 	moveq	r0, r6
    7934:	059f1108 	ldreq	r1, [pc, #264]	; 7a44 <mmc_startup+0x5ec>
    7938:	0a000004 	beq	7950 <mmc_startup+0x4f8>
    793c:	e3130010 	tst	r3, #16	; 0x10
    7940:	159f1100 	ldrne	r1, [pc, #256]	; 7a48 <mmc_startup+0x5f0>
    7944:	059f1100 	ldreq	r1, [pc, #256]	; 7a4c <mmc_startup+0x5f4>
    7948:	11a00006 	movne	r0, r6
    794c:	01a00006 	moveq	r0, r6
    7950:	ebfffbed 	bl	690c <mmc_set_clock>
    7954:	e5964094 	ldr	r4, [r6, #148]
    7958:	e5960090 	ldr	r0, [r6, #144]
    795c:	e5961088 	ldr	r1, [r6, #136]
    7960:	e3a03000 	mov	r3, #0	; 0x0
    7964:	e3540000 	cmp	r4, #0	; 0x0
    7968:	e5c630a3 	strb	r3, [r6, #163]
    796c:	e5c630a2 	strb	r3, [r6, #162]
    7970:	e58610ac 	str	r1, [r6, #172]
    7974:	e58d0230 	str	r0, [sp, #560]
    7978:	e58d4234 	str	r4, [sp, #564]
    797c:	1a000003 	bne	7990 <mmc_startup+0x538>
    7980:	eb001a1c 	bl	e1f8 <__aeabi_uidiv>
    7984:	e58d4234 	str	r4, [sp, #564]
    7988:	e58d0230 	str	r0, [sp, #560]
    798c:	ea000001 	b	7998 <mmc_startup+0x540>
    7990:	e28d0e23 	add	r0, sp, #560	; 0x230
    7994:	eb000dd6 	bl	b0f4 <__div64_32>
    7998:	e5d6007f 	ldrb	r0, [r6, #127]
    799c:	e59d1230 	ldr	r1, [sp, #560]
    79a0:	e5962070 	ldr	r2, [r6, #112]
    79a4:	e5963078 	ldr	r3, [r6, #120]
    79a8:	e58610a8 	str	r1, [r6, #168]
    79ac:	e1803403 	orr	r3, r0, r3, lsl #8
    79b0:	e1a02422 	lsr	r2, r2, #8
    79b4:	e59f1094 	ldr	r1, [pc, #148]	; 7a50 <mmc_startup+0x5f8>
    79b8:	e28600b0 	add	r0, r6, #176	; 0xb0
    79bc:	eb001195 	bl	c018 <sprintf>
    79c0:	e596c074 	ldr	ip, [r6, #116]
    79c4:	e5d62070 	ldrb	r2, [r6, #112]
    79c8:	e1a0482c 	lsr	r4, ip, #16
    79cc:	e1a0542c 	lsr	r5, ip, #8
    79d0:	e1a03c2c 	lsr	r3, ip, #24
    79d4:	e20440ff 	and	r4, r4, #255	; 0xff
    79d8:	e20cc0ff 	and	ip, ip, #255	; 0xff
    79dc:	e59f1070 	ldr	r1, [pc, #112]	; 7a54 <mmc_startup+0x5fc>
    79e0:	e20550ff 	and	r5, r5, #255	; 0xff
    79e4:	e28600d9 	add	r0, r6, #217	; 0xd9
    79e8:	e88d1030 	stm	sp, {r4, r5, ip}
    79ec:	eb001189 	bl	c018 <sprintf>
    79f0:	e5962078 	ldr	r2, [r6, #120]
    79f4:	e28600ee 	add	r0, r6, #238	; 0xee
    79f8:	e1a03c22 	lsr	r3, r2, #24
    79fc:	e203300f 	and	r3, r3, #15	; 0xf
    7a00:	e1a02e22 	lsr	r2, r2, #28
    7a04:	e59f104c 	ldr	r1, [pc, #76]	; 7a58 <mmc_startup+0x600>
    7a08:	eb001182 	bl	c018 <sprintf>
    7a0c:	e3a04000 	mov	r4, #0	; 0x0
    7a10:	e1a00004 	mov	r0, r4
    7a14:	e28ddf8e 	add	sp, sp, #568	; 0x238
    7a18:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    7a1c:	00010014 	.word	0x00010014
    7a20:	00010022 	.word	0x00010022
    7a24:	00010030 	.word	0x00010030
    7a28:	00010040 	.word	0x00010040
    7a2c:	00010012 	.word	0x00010012
    7a30:	0000f694 	.word	0x0000f694
    7a34:	0000f684 	.word	0x0000f684
    7a38:	0001003f 	.word	0x0001003f
    7a3c:	02faf080 	.word	0x02faf080
    7a40:	017d7840 	.word	0x017d7840
    7a44:	01312d00 	.word	0x01312d00
    7a48:	03197500 	.word	0x03197500
    7a4c:	018cba80 	.word	0x018cba80
    7a50:	00011351 	.word	0x00011351
    7a54:	00011363 	.word	0x00011363
    7a58:	0001136e 	.word	0x0001136e

00007a5c <mmc_init>:
    7a5c:	e92d4010 	push	{r4, lr}
    7a60:	ebfffb5a 	bl	67d0 <find_mmc_device>
    7a64:	e1a04000 	mov	r4, r0
    7a68:	e1a0e00f 	mov	lr, pc
    7a6c:	e590f10c 	ldr	pc, [r0, #268]
    7a70:	e3500000 	cmp	r0, #0	; 0x0
    7a74:	18bd8010 	popne	{r4, pc}
    7a78:	e3a03001 	mov	r3, #1	; 0x1
    7a7c:	e5843044 	str	r3, [r4, #68]
    7a80:	e1a00004 	mov	r0, r4
    7a84:	e1a0e00f 	mov	lr, pc
    7a88:	e594f108 	ldr	pc, [r4, #264]
    7a8c:	e1a00004 	mov	r0, r4
    7a90:	e59f1060 	ldr	r1, [pc, #96]	; 7af8 <mmc_init+0x9c>
    7a94:	ebfffb9c 	bl	690c <mmc_set_clock>
    7a98:	e1a00004 	mov	r0, r4
    7a9c:	ebfffd09 	bl	6ec8 <mmc_go_idle>
    7aa0:	e3500000 	cmp	r0, #0	; 0x0
    7aa4:	18bd8010 	popne	{r4, pc}
    7aa8:	e5c40083 	strb	r0, [r4, #131]
    7aac:	e1a00004 	mov	r0, r4
    7ab0:	ebfffba9 	bl	695c <mmc_send_if_cond>
    7ab4:	e1a00004 	mov	r0, r4
    7ab8:	ebfffcad 	bl	6d74 <sd_send_op_cond>
    7abc:	e3700013 	cmn	r0, #19	; 0x13
    7ac0:	1a000009 	bne	7aec <mmc_init+0x90>
    7ac4:	e1a00004 	mov	r0, r4
    7ac8:	ebfffd13 	bl	6f1c <mmc_send_op_cond>
    7acc:	e3500000 	cmp	r0, #0	; 0x0
    7ad0:	0a000005 	beq	7aec <mmc_init+0x90>
    7ad4:	e59f0020 	ldr	r0, [pc, #32]	; 7afc <mmc_init+0xa0>
    7ad8:	e59f3020 	ldr	r3, [pc, #32]	; 7b00 <mmc_init+0xa4>
    7adc:	e1a0e00f 	mov	lr, pc
    7ae0:	e593f000 	ldr	pc, [r3]
    7ae4:	e3e00010 	mvn	r0, #16	; 0x10
    7ae8:	e8bd8010 	pop	{r4, pc}
    7aec:	e1a00004 	mov	r0, r4
    7af0:	e8bd4010 	pop	{r4, lr}
    7af4:	eafffe57 	b	7458 <mmc_startup>
    7af8:	00061a80 	.word	0x00061a80
    7afc:	00011374 	.word	0x00011374
    7b00:	3c00d68c 	.word	0x3c00d68c

00007b04 <mmc2_vs_reset>:
    7b04:	e3a00002 	mov	r0, #2	; 0x2
    7b08:	eaffffd3 	b	7a5c <mmc_init>

00007b0c <mmc1_vs_reset>:
    7b0c:	e3a00001 	mov	r0, #1	; 0x1
    7b10:	eaffffd1 	b	7a5c <mmc_init>

00007b14 <mmc0_vs_reset>:
    7b14:	e3a00000 	mov	r0, #0	; 0x0
    7b18:	eaffffcf 	b	7a5c <mmc_init>

00007b1c <mmc_read_blocks>:
    7b1c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7b20:	e3530001 	cmp	r3, #1	; 0x1
    7b24:	e24dd030 	sub	sp, sp, #48	; 0x30
    7b28:	e1a06003 	mov	r6, r3
    7b2c:	83a03012 	movhi	r3, #18	; 0x12
    7b30:	93a03011 	movls	r3, #17	; 0x11
    7b34:	e1cd30b0 	strh	r3, [sp]
    7b38:	e5903040 	ldr	r3, [r0, #64]
    7b3c:	e3a07000 	mov	r7, #0	; 0x0
    7b40:	e3530000 	cmp	r3, #0	; 0x0
    7b44:	05903088 	ldreq	r3, [r0, #136]
    7b48:	158d2008 	strne	r2, [sp, #8]
    7b4c:	00030293 	muleq	r3, r3, r2
    7b50:	058d3008 	streq	r3, [sp, #8]
    7b54:	e5903088 	ldr	r3, [r0, #136]
    7b58:	e3a02015 	mov	r2, #21	; 0x15
    7b5c:	e58d302c 	str	r3, [sp, #44]
    7b60:	e3a03001 	mov	r3, #1	; 0x1
    7b64:	e58d2004 	str	r2, [sp, #4]
    7b68:	e58d1020 	str	r1, [sp, #32]
    7b6c:	e1a04000 	mov	r4, r0
    7b70:	e58d3024 	str	r3, [sp, #36]
    7b74:	e58d701c 	str	r7, [sp, #28]
    7b78:	e58d6028 	str	r6, [sp, #40]
    7b7c:	e1a0100d 	mov	r1, sp
    7b80:	e28d2020 	add	r2, sp, #32	; 0x20
    7b84:	e1a0e00f 	mov	lr, pc
    7b88:	e594f104 	ldr	pc, [r4, #260]
    7b8c:	e2505000 	subs	r5, r0, #0	; 0x0
    7b90:	e1a0800d 	mov	r8, sp
    7b94:	11a00007 	movne	r0, r7
    7b98:	1a000018 	bne	7c00 <mmc_read_blocks+0xe4>
    7b9c:	e3560001 	cmp	r6, #1	; 0x1
    7ba0:	9a000012 	bls	7bf0 <mmc_read_blocks+0xd4>
    7ba4:	e3a0301d 	mov	r3, #29	; 0x1d
    7ba8:	e58d3004 	str	r3, [sp, #4]
    7bac:	e3a0300c 	mov	r3, #12	; 0xc
    7bb0:	e1cd30b0 	strh	r3, [sp]
    7bb4:	e58d5008 	str	r5, [sp, #8]
    7bb8:	e58d501c 	str	r5, [sp, #28]
    7bbc:	e1a0100d 	mov	r1, sp
    7bc0:	e1a00004 	mov	r0, r4
    7bc4:	e1a02005 	mov	r2, r5
    7bc8:	e1a0e00f 	mov	lr, pc
    7bcc:	e594f104 	ldr	pc, [r4, #260]
    7bd0:	e3500000 	cmp	r0, #0	; 0x0
    7bd4:	0a000005 	beq	7bf0 <mmc_read_blocks+0xd4>
    7bd8:	e59f0028 	ldr	r0, [pc, #40]	; 7c08 <mmc_read_blocks+0xec>
    7bdc:	e59f3028 	ldr	r3, [pc, #40]	; 7c0c <mmc_read_blocks+0xf0>
    7be0:	e1a0e00f 	mov	lr, pc
    7be4:	e593f000 	ldr	pc, [r3]
    7be8:	e1a00005 	mov	r0, r5
    7bec:	ea000003 	b	7c00 <mmc_read_blocks+0xe4>
    7bf0:	e1a00004 	mov	r0, r4
    7bf4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7bf8:	ebfffd15 	bl	7054 <mmc_send_status>
    7bfc:	e1a00006 	mov	r0, r6
    7c00:	e28dd030 	add	sp, sp, #48	; 0x30
    7c04:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7c08:	00011336 	.word	0x00011336
    7c0c:	3c00d68c 	.word	0x3c00d68c

00007c10 <mmc_bread>:
    7c10:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    7c14:	e252a000 	subs	sl, r2, #0	; 0x0
    7c18:	e24dd004 	sub	sp, sp, #4	; 0x4
    7c1c:	e1a07001 	mov	r7, r1
    7c20:	e1a08003 	mov	r8, r3
    7c24:	0a000027 	beq	7cc8 <mmc_bread+0xb8>
    7c28:	ebfffae8 	bl	67d0 <find_mmc_device>
    7c2c:	e2506000 	subs	r6, r0, #0	; 0x0
    7c30:	0a000024 	beq	7cc8 <mmc_bread+0xb8>
    7c34:	e59620a8 	ldr	r2, [r6, #168]
    7c38:	e08a1007 	add	r1, sl, r7
    7c3c:	e1510002 	cmp	r1, r2
    7c40:	9a000004 	bls	7c58 <mmc_bread+0x48>
    7c44:	e59f0088 	ldr	r0, [pc, #136]	; 7cd4 <mmc_bread+0xc4>
    7c48:	e59f3088 	ldr	r3, [pc, #136]	; 7cd8 <mmc_bread+0xc8>
    7c4c:	e1a0e00f 	mov	lr, pc
    7c50:	e593f000 	ldr	pc, [r3]
    7c54:	ea00001b 	b	7cc8 <mmc_bread+0xb8>
    7c58:	e5961088 	ldr	r1, [r6, #136]
    7c5c:	ebfffacd 	bl	6798 <mmc_set_blocklen>
    7c60:	e3500000 	cmp	r0, #0	; 0x0
    7c64:	01a0500a 	moveq	r5, sl
    7c68:	0a000005 	beq	7c84 <mmc_bread+0x74>
    7c6c:	e5961088 	ldr	r1, [r6, #136]
    7c70:	e59f0064 	ldr	r0, [pc, #100]	; 7cdc <mmc_bread+0xcc>
    7c74:	e59f305c 	ldr	r3, [pc, #92]	; 7cd8 <mmc_bread+0xc8>
    7c78:	e1a0e00f 	mov	lr, pc
    7c7c:	e593f000 	ldr	pc, [r3]
    7c80:	ea000010 	b	7cc8 <mmc_bread+0xb8>
    7c84:	e5964110 	ldr	r4, [r6, #272]
    7c88:	e1a02007 	mov	r2, r7
    7c8c:	e1550004 	cmp	r5, r4
    7c90:	31a04005 	movcc	r4, r5
    7c94:	e1a01008 	mov	r1, r8
    7c98:	e1a00006 	mov	r0, r6
    7c9c:	e1a03004 	mov	r3, r4
    7ca0:	ebffff9d 	bl	7b1c <mmc_read_blocks>
    7ca4:	e1500004 	cmp	r0, r4
    7ca8:	e0877000 	add	r7, r7, r0
    7cac:	1a000005 	bne	7cc8 <mmc_bread+0xb8>
    7cb0:	e0555000 	subs	r5, r5, r0
    7cb4:	e5962088 	ldr	r2, [r6, #136]
    7cb8:	10288092 	mlane	r8, r2, r0, r8
    7cbc:	1afffff0 	bne	7c84 <mmc_bread+0x74>
    7cc0:	e000029a 	mul	r0, sl, r2
    7cc4:	ea000000 	b	7ccc <mmc_bread+0xbc>
    7cc8:	e3a00000 	mov	r0, #0	; 0x0
    7ccc:	e28dd004 	add	sp, sp, #4	; 0x4
    7cd0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    7cd4:	000112f8 	.word	0x000112f8
    7cd8:	3c00d68c 	.word	0x3c00d68c
    7cdc:	0001139d 	.word	0x0001139d

00007ce0 <mmc2_vs_read>:
    7ce0:	e1a024a2 	lsr	r2, r2, #9
    7ce4:	e1821b83 	orr	r1, r2, r3, lsl #23
    7ce8:	e59d2000 	ldr	r2, [sp]
    7cec:	e1a03000 	mov	r3, r0
    7cf0:	e1a024c2 	asr	r2, r2, #9
    7cf4:	e3a00002 	mov	r0, #2	; 0x2
    7cf8:	eaffffc4 	b	7c10 <mmc_bread>

00007cfc <mmc1_vs_read>:
    7cfc:	e1a024a2 	lsr	r2, r2, #9
    7d00:	e1821b83 	orr	r1, r2, r3, lsl #23
    7d04:	e59d2000 	ldr	r2, [sp]
    7d08:	e1a03000 	mov	r3, r0
    7d0c:	e1a024c2 	asr	r2, r2, #9
    7d10:	e3a00001 	mov	r0, #1	; 0x1
    7d14:	eaffffbd 	b	7c10 <mmc_bread>

00007d18 <mmc0_vs_read>:
    7d18:	e1a024a2 	lsr	r2, r2, #9
    7d1c:	e1821b83 	orr	r1, r2, r3, lsl #23
    7d20:	e59d2000 	ldr	r2, [sp]
    7d24:	e1a03000 	mov	r3, r0
    7d28:	e1a024c2 	asr	r2, r2, #9
    7d2c:	e3a00000 	mov	r0, #0	; 0x0
    7d30:	eaffffb6 	b	7c10 <mmc_bread>

00007d34 <mci_pull_data32>:
    7d34:	e1a02142 	asr	r2, r2, #2
    7d38:	e3a0c000 	mov	ip, #0	; 0x0
    7d3c:	ea000003 	b	7d50 <mci_pull_data32+0x1c>
    7d40:	e5903118 	ldr	r3, [r0, #280]
    7d44:	e5933200 	ldr	r3, [r3, #512]
    7d48:	e781300c 	str	r3, [r1, ip]
    7d4c:	e28cc004 	add	ip, ip, #4	; 0x4
    7d50:	e3520000 	cmp	r2, #0	; 0x0
    7d54:	e2422001 	sub	r2, r2, #1	; 0x1
    7d58:	cafffff8 	bgt	7d40 <mci_pull_data32+0xc>
    7d5c:	e12fff1e 	bx	lr

00007d60 <mci_push_data32>:
    7d60:	e92d4010 	push	{r4, lr}
    7d64:	e3a0c000 	mov	ip, #0	; 0x0
    7d68:	e1a04000 	mov	r4, r0
    7d6c:	e1a00001 	mov	r0, r1
    7d70:	e1a01142 	asr	r1, r2, #2
    7d74:	ea000003 	b	7d88 <mci_push_data32+0x28>
    7d78:	e790200c 	ldr	r2, [r0, ip]
    7d7c:	e5943118 	ldr	r3, [r4, #280]
    7d80:	e28cc004 	add	ip, ip, #4	; 0x4
    7d84:	e5832200 	str	r2, [r3, #512]
    7d88:	e3510000 	cmp	r1, #0	; 0x0
    7d8c:	e2411001 	sub	r1, r1, #1	; 0x1
    7d90:	cafffff8 	bgt	7d78 <mci_push_data32+0x18>
    7d94:	e8bd8010 	pop	{r4, pc}

00007d98 <imap_sdhc_initialize>:
    7d98:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7d9c:	e3510000 	cmp	r1, #0	; 0x0
    7da0:	e24dd004 	sub	sp, sp, #4	; 0x4
    7da4:	1a000005 	bne	7dc0 <imap_sdhc_initialize+0x28>
    7da8:	e59f00ec 	ldr	r0, [pc, #236]	; 7e9c <imap_sdhc_initialize+0x104>
    7dac:	e59f30ec 	ldr	r3, [pc, #236]	; 7ea0 <imap_sdhc_initialize+0x108>
    7db0:	e1a0e00f 	mov	lr, pc
    7db4:	e593f000 	ldr	pc, [r3]
    7db8:	e3e00004 	mvn	r0, #4	; 0x4
    7dbc:	ea000034 	b	7e94 <imap_sdhc_initialize+0xfc>
    7dc0:	e3500000 	cmp	r0, #0	; 0x0
    7dc4:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    7dc8:	0a000005 	beq	7de4 <imap_sdhc_initialize+0x4c>
    7dcc:	e3500001 	cmp	r0, #1	; 0x1
    7dd0:	059f30cc 	ldreq	r3, [pc, #204]	; 7ea4 <imap_sdhc_initialize+0x10c>
    7dd4:	0a000002 	beq	7de4 <imap_sdhc_initialize+0x4c>
    7dd8:	e3500002 	cmp	r0, #2	; 0x2
    7ddc:	1a000018 	bne	7e44 <imap_sdhc_initialize+0xac>
    7de0:	e59f30c0 	ldr	r3, [pc, #192]	; 7ea8 <imap_sdhc_initialize+0x110>
    7de4:	e5813118 	str	r3, [r1, #280]
    7de8:	e59f30bc 	ldr	r3, [pc, #188]	; 7eac <imap_sdhc_initialize+0x114>
    7dec:	e591c130 	ldr	ip, [r1, #304]
    7df0:	e5813104 	str	r3, [r1, #260]
    7df4:	e59f30b4 	ldr	r3, [pc, #180]	; 7eb0 <imap_sdhc_initialize+0x118>
    7df8:	e3a02000 	mov	r2, #0	; 0x0
    7dfc:	e5813108 	str	r3, [r1, #264]
    7e00:	e59f30ac 	ldr	r3, [pc, #172]	; 7eb4 <imap_sdhc_initialize+0x11c>
    7e04:	e5810114 	str	r0, [r1, #276]
    7e08:	e581310c 	str	r3, [r1, #268]
    7e0c:	e59f30a4 	ldr	r3, [pc, #164]	; 7eb8 <imap_sdhc_initialize+0x120>
    7e10:	e5812034 	str	r2, [r1, #52]
    7e14:	e581302c 	str	r3, [r1, #44]
    7e18:	e3003101 	movw	r3, #257	; 0x101
    7e1c:	e5813050 	str	r3, [r1, #80]
    7e20:	e59f3094 	ldr	r3, [pc, #148]	; 7ebc <imap_sdhc_initialize+0x124>
    7e24:	e5812028 	str	r2, [r1, #40]
    7e28:	e5813038 	str	r3, [r1, #56]
    7e2c:	e59f308c 	ldr	r3, [pc, #140]	; 7ec0 <imap_sdhc_initialize+0x128>
    7e30:	e5812110 	str	r2, [r1, #272]
    7e34:	e581303c 	str	r3, [r1, #60]
    7e38:	e1a0000c 	mov	r0, ip
    7e3c:	e2822010 	add	r2, r2, #16	; 0x10
    7e40:	ea000005 	b	7e5c <imap_sdhc_initialize+0xc4>
    7e44:	e59f0078 	ldr	r0, [pc, #120]	; 7ec4 <imap_sdhc_initialize+0x12c>
    7e48:	e59f3050 	ldr	r3, [pc, #80]	; 7ea0 <imap_sdhc_initialize+0x108>
    7e4c:	e1a0e00f 	mov	lr, pc
    7e50:	e593f000 	ldr	pc, [r3]
    7e54:	e3e00000 	mvn	r0, #0	; 0x0
    7e58:	ea00000d 	b	7e94 <imap_sdhc_initialize+0xfc>
    7e5c:	e5913130 	ldr	r3, [r1, #304]
    7e60:	e0823003 	add	r3, r2, r3
    7e64:	e2822010 	add	r2, r2, #16	; 0x10
    7e68:	e3520b01 	cmp	r2, #1024	; 0x400
    7e6c:	e580300c 	str	r3, [r0, #12]
    7e70:	e2800010 	add	r0, r0, #16	; 0x10
    7e74:	1afffff8 	bne	7e5c <imap_sdhc_initialize+0xc4>
    7e78:	e5912130 	ldr	r2, [r1, #304]
    7e7c:	e3a03020 	mov	r3, #32	; 0x20
    7e80:	e58c33f0 	str	r3, [ip, #1008]
    7e84:	e58c23fc 	str	r2, [ip, #1020]
    7e88:	e1a00001 	mov	r0, r1
    7e8c:	ebfffad0 	bl	69d4 <mmc_register>
    7e90:	e3a00000 	mov	r0, #0	; 0x0
    7e94:	e28dd004 	add	sp, sp, #4	; 0x4
    7e98:	e8bd8000 	pop	{pc}
    7e9c:	000113a7 	.word	0x000113a7
    7ea0:	3c00d68c 	.word	0x3c00d68c
    7ea4:	27200000 	.word	0x27200000
    7ea8:	27300000 	.word	0x27300000
    7eac:	00007f38 	.word	0x00007f38
    7eb0:	00008410 	.word	0x00008410
    7eb4:	00008258 	.word	0x00008258
    7eb8:	00ff8000 	.word	0x00ff8000
    7ebc:	00061a80 	.word	0x00061a80
    7ec0:	018cba80 	.word	0x018cba80
    7ec4:	000113bd 	.word	0x000113bd

00007ec8 <mci_send_cmd>:
    7ec8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7ecc:	e5903118 	ldr	r3, [r0, #280]
    7ed0:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    7ed4:	e5832028 	str	r2, [r3, #40]
    7ed8:	e5903118 	ldr	r3, [r0, #280]
    7edc:	e24dd004 	sub	sp, sp, #4	; 0x4
    7ee0:	e583102c 	str	r1, [r3, #44]
    7ee4:	e1a04000 	mov	r4, r0
    7ee8:	eb0001f1 	bl	86b4 <get_ticks>
    7eec:	e59f5040 	ldr	r5, [pc, #64]	; 7f34 <mci_send_cmd+0x6c>
    7ef0:	e1a06000 	mov	r6, r0
    7ef4:	e1a07001 	mov	r7, r1
    7ef8:	e5943118 	ldr	r3, [r4, #280]
    7efc:	e593302c 	ldr	r3, [r3, #44]
    7f00:	e3530000 	cmp	r3, #0	; 0x0
    7f04:	aa000008 	bge	7f2c <mci_send_cmd+0x64>
    7f08:	eb0001e9 	bl	86b4 <get_ticks>
    7f0c:	e1a02000 	mov	r2, r0
    7f10:	e1a03001 	mov	r3, r1
    7f14:	e0522006 	subs	r2, r2, r6
    7f18:	e0c33007 	sbc	r3, r3, r7
    7f1c:	e3530000 	cmp	r3, #0	; 0x0
    7f20:	1a000001 	bne	7f2c <mci_send_cmd+0x64>
    7f24:	e1520005 	cmp	r2, r5
    7f28:	9afffff2 	bls	7ef8 <mci_send_cmd+0x30>
    7f2c:	e28dd004 	add	sp, sp, #4	; 0x4
    7f30:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7f34:	0007a11f 	.word	0x0007a11f

00007f38 <imap_sdhc_send_cmd>:
    7f38:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    7f3c:	e252a000 	subs	sl, r2, #0	; 0x0
    7f40:	e1a04000 	mov	r4, r0
    7f44:	e1a08001 	mov	r8, r1
    7f48:	0a000039 	beq	8034 <imap_sdhc_send_cmd+0xfc>
    7f4c:	e99a000c 	ldmib	sl, {r2, r3}
    7f50:	e59a100c 	ldr	r1, [sl, #12]
    7f54:	e3120001 	tst	r2, #1	; 0x1
    7f58:	e0000193 	mul	r0, r3, r1
    7f5c:	15942118 	ldrne	r2, [r4, #280]
    7f60:	11a03801 	lslne	r3, r1, #16
    7f64:	13833001 	orrne	r3, r3, #1	; 0x1
    7f68:	15823100 	strne	r3, [r2, #256]
    7f6c:	e59a200c 	ldr	r2, [sl, #12]
    7f70:	e5943118 	ldr	r3, [r4, #280]
    7f74:	e1a0e000 	mov	lr, r0
    7f78:	e583201c 	str	r2, [r3, #28]
    7f7c:	e5943118 	ldr	r3, [r4, #280]
    7f80:	e3a05000 	mov	r5, #0	; 0x0
    7f84:	e5830020 	str	r0, [r3, #32]
    7f88:	e5942118 	ldr	r2, [r4, #280]
    7f8c:	e3019ff0 	movw	r9, #8176	; 0x1ff0
    7f90:	e5923000 	ldr	r3, [r2]
    7f94:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    7f98:	e3833020 	orr	r3, r3, #32	; 0x20
    7f9c:	e5823000 	str	r3, [r2]
    7fa0:	e59a6000 	ldr	r6, [sl]
    7fa4:	e594c130 	ldr	ip, [r4, #304]
    7fa8:	e15e0009 	cmp	lr, r9
    7fac:	31a0100e 	movcc	r1, lr
    7fb0:	21a01009 	movcs	r1, r9
    7fb4:	e59c0004 	ldr	r0, [ip, #4]
    7fb8:	e59f3288 	ldr	r3, [pc, #648]	; 8248 <imap_sdhc_send_cmd+0x310>
    7fbc:	e1a02981 	lsl	r2, r1, #19
    7fc0:	e0003003 	and	r3, r0, r3
    7fc4:	e1a029a2 	lsr	r2, r2, #19
    7fc8:	e1822003 	orr	r2, r2, r3
    7fcc:	e05ee001 	subs	lr, lr, r1
    7fd0:	e0853006 	add	r3, r5, r6
    7fd4:	e98c000c 	stmib	ip, {r2, r3}
    7fd8:	e58c7000 	str	r7, [ip]
    7fdc:	e1a0200c 	mov	r2, ip
    7fe0:	e0855001 	add	r5, r5, r1
    7fe4:	e28cc010 	add	ip, ip, #16	; 0x10
    7fe8:	1affffee 	bne	7fa8 <imap_sdhc_send_cmd+0x70>
    7fec:	e3c73002 	bic	r3, r7, #2	; 0x2
    7ff0:	e3833004 	orr	r3, r3, #4	; 0x4
    7ff4:	e5823000 	str	r3, [r2]
    7ff8:	e5942130 	ldr	r2, [r4, #304]
    7ffc:	e5923000 	ldr	r3, [r2]
    8000:	e3833008 	orr	r3, r3, #8	; 0x8
    8004:	e5823000 	str	r3, [r2]
    8008:	e5942118 	ldr	r2, [r4, #280]
    800c:	e5923000 	ldr	r3, [r2]
    8010:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    8014:	e5823000 	str	r3, [r2]
    8018:	e5942118 	ldr	r2, [r4, #280]
    801c:	e5923080 	ldr	r3, [r2, #128]
    8020:	e3833080 	orr	r3, r3, #128	; 0x80
    8024:	e5823080 	str	r3, [r2, #128]
    8028:	e5942118 	ldr	r2, [r4, #280]
    802c:	e3a03001 	mov	r3, #1	; 0x1
    8030:	e5823084 	str	r3, [r2, #132]
    8034:	e1d800b0 	ldrh	r0, [r8]
    8038:	e5982004 	ldr	r2, [r8, #4]
    803c:	e350000c 	cmp	r0, #12	; 0xc
    8040:	03a03901 	moveq	r3, #16384	; 0x4000
    8044:	13a03a02 	movne	r3, #8192	; 0x2000
    8048:	e3120004 	tst	r2, #4	; 0x4
    804c:	13833c01 	orrne	r3, r3, #256	; 0x100
    8050:	e3120001 	tst	r2, #1	; 0x1
    8054:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    8058:	0a000002 	beq	8068 <imap_sdhc_send_cmd+0x130>
    805c:	e3120002 	tst	r2, #2	; 0x2
    8060:	e3811040 	orr	r1, r1, #64	; 0x40
    8064:	13811080 	orrne	r1, r1, #128	; 0x80
    8068:	e35a0000 	cmp	sl, #0	; 0x0
    806c:	0a000003 	beq	8080 <imap_sdhc_send_cmd+0x148>
    8070:	e59a3004 	ldr	r3, [sl, #4]
    8074:	e3811c02 	orr	r1, r1, #512	; 0x200
    8078:	e3130002 	tst	r3, #2	; 0x2
    807c:	13811b01 	orrne	r1, r1, #1024	; 0x400
    8080:	e594312c 	ldr	r3, [r4, #300]
    8084:	e1811000 	orr	r1, r1, r0
    8088:	e3530000 	cmp	r3, #0	; 0x0
    808c:	13a03000 	movne	r3, #0	; 0x0
    8090:	1584312c 	strne	r3, [r4, #300]
    8094:	e5982008 	ldr	r2, [r8, #8]
    8098:	e5943118 	ldr	r3, [r4, #280]
    809c:	13811902 	orrne	r1, r1, #32768	; 0x8000
    80a0:	e5832028 	str	r2, [r3, #40]
    80a4:	e5943118 	ldr	r3, [r4, #280]
    80a8:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    80ac:	e583102c 	str	r1, [r3, #44]
    80b0:	eb00017f 	bl	86b4 <get_ticks>
    80b4:	e3005bb8 	movw	r5, #3000	; 0xbb8
    80b8:	e1a06000 	mov	r6, r0
    80bc:	e1a07001 	mov	r7, r1
    80c0:	eb00017b 	bl	86b4 <get_ticks>
    80c4:	e1a02000 	mov	r2, r0
    80c8:	e1a03001 	mov	r3, r1
    80cc:	e0522006 	subs	r2, r2, r6
    80d0:	e0c33007 	sbc	r3, r3, r7
    80d4:	e3530000 	cmp	r3, #0	; 0x0
    80d8:	1a000007 	bne	80fc <imap_sdhc_send_cmd+0x1c4>
    80dc:	e1520005 	cmp	r2, r5
    80e0:	8a000005 	bhi	80fc <imap_sdhc_send_cmd+0x1c4>
    80e4:	e5943118 	ldr	r3, [r4, #280]
    80e8:	e5932044 	ldr	r2, [r3, #68]
    80ec:	e2831044 	add	r1, r3, #68	; 0x44
    80f0:	e3120004 	tst	r2, #4	; 0x4
    80f4:	0afffff1 	beq	80c0 <imap_sdhc_send_cmd+0x188>
    80f8:	ea000005 	b	8114 <imap_sdhc_send_cmd+0x1dc>
    80fc:	e59f0148 	ldr	r0, [pc, #328]	; 824c <imap_sdhc_send_cmd+0x314>
    8100:	e59f3148 	ldr	r3, [pc, #328]	; 8250 <imap_sdhc_send_cmd+0x318>
    8104:	e1a0e00f 	mov	lr, pc
    8108:	e593f000 	ldr	pc, [r3]
    810c:	e3e00011 	mvn	r0, #17	; 0x11
    8110:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    8114:	e5915000 	ldr	r5, [r1]
    8118:	e3003142 	movw	r3, #322	; 0x142
    811c:	e0053003 	and	r3, r5, r3
    8120:	e3530000 	cmp	r3, #0	; 0x0
    8124:	e3a03004 	mov	r3, #4	; 0x4
    8128:	e5813000 	str	r3, [r1]
    812c:	0a00000e 	beq	816c <imap_sdhc_send_cmd+0x234>
    8130:	e1d810b0 	ldrh	r1, [r8]
    8134:	e5943118 	ldr	r3, [r4, #280]
    8138:	e3510037 	cmp	r1, #55	; 0x37
    813c:	13510008 	cmpne	r1, #8	; 0x8
    8140:	e5835044 	str	r5, [r3, #68]
    8144:	0a000004 	beq	815c <imap_sdhc_send_cmd+0x224>
    8148:	e59f0104 	ldr	r0, [pc, #260]	; 8254 <imap_sdhc_send_cmd+0x31c>
    814c:	e1a02005 	mov	r2, r5
    8150:	e59f30f8 	ldr	r3, [pc, #248]	; 8250 <imap_sdhc_send_cmd+0x318>
    8154:	e1a0e00f 	mov	lr, pc
    8158:	e593f000 	ldr	pc, [r3]
    815c:	e3150c01 	tst	r5, #256	; 0x100
    8160:	13e00012 	mvnne	r0, #18	; 0x12
    8164:	03e00011 	mvneq	r0, #17	; 0x11
    8168:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    816c:	e5983004 	ldr	r3, [r8, #4]
    8170:	e5942118 	ldr	r2, [r4, #280]
    8174:	e3130002 	tst	r3, #2	; 0x2
    8178:	05923030 	ldreq	r3, [r2, #48]
    817c:	0588300c 	streq	r3, [r8, #12]
    8180:	0a00000a 	beq	81b0 <imap_sdhc_send_cmd+0x278>
    8184:	e592303c 	ldr	r3, [r2, #60]
    8188:	e588300c 	str	r3, [r8, #12]
    818c:	e5943118 	ldr	r3, [r4, #280]
    8190:	e5933038 	ldr	r3, [r3, #56]
    8194:	e5883010 	str	r3, [r8, #16]
    8198:	e5943118 	ldr	r3, [r4, #280]
    819c:	e5933034 	ldr	r3, [r3, #52]
    81a0:	e5883014 	str	r3, [r8, #20]
    81a4:	e5943118 	ldr	r3, [r4, #280]
    81a8:	e5933030 	ldr	r3, [r3, #48]
    81ac:	e5883018 	str	r3, [r8, #24]
    81b0:	e35a0000 	cmp	sl, #0	; 0x0
    81b4:	01a0000a 	moveq	r0, sl
    81b8:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
    81bc:	e594c118 	ldr	ip, [r4, #280]
    81c0:	e59a3004 	ldr	r3, [sl, #4]
    81c4:	e28c7044 	add	r7, ip, #68	; 0x44
    81c8:	e2030001 	and	r0, r3, #1	; 0x1
    81cc:	e28c608c 	add	r6, ip, #140	; 0x8c
    81d0:	e5975000 	ldr	r5, [r7]
    81d4:	e3500000 	cmp	r0, #0	; 0x0
    81d8:	e596e000 	ldr	lr, [r6]
    81dc:	e2051008 	and	r1, r5, #8	; 0x8
    81e0:	0a000005 	beq	81fc <imap_sdhc_send_cmd+0x2c4>
    81e4:	e3510000 	cmp	r1, #0	; 0x0
    81e8:	0afffff8 	beq	81d0 <imap_sdhc_send_cmd+0x298>
    81ec:	e31e0002 	tst	lr, #2	; 0x2
    81f0:	0afffff6 	beq	81d0 <imap_sdhc_send_cmd+0x298>
    81f4:	e3a03002 	mov	r3, #2	; 0x2
    81f8:	ea000004 	b	8210 <imap_sdhc_send_cmd+0x2d8>
    81fc:	e3510000 	cmp	r1, #0	; 0x0
    8200:	0afffff2 	beq	81d0 <imap_sdhc_send_cmd+0x298>
    8204:	e31e0001 	tst	lr, #1	; 0x1
    8208:	0afffff0 	beq	81d0 <imap_sdhc_send_cmd+0x298>
    820c:	e3a03001 	mov	r3, #1	; 0x1
    8210:	e58c308c 	str	r3, [ip, #140]
    8214:	e5943118 	ldr	r3, [r4, #280]
    8218:	e3a00000 	mov	r0, #0	; 0x0
    821c:	e5835044 	str	r5, [r3, #68]
    8220:	e5942118 	ldr	r2, [r4, #280]
    8224:	e5923000 	ldr	r3, [r2]
    8228:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    822c:	e3833004 	orr	r3, r3, #4	; 0x4
    8230:	e5823000 	str	r3, [r2]
    8234:	e5942118 	ldr	r2, [r4, #280]
    8238:	e5923080 	ldr	r3, [r2, #128]
    823c:	e3c33080 	bic	r3, r3, #128	; 0x80
    8240:	e5823080 	str	r3, [r2, #128]
    8244:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    8248:	03ffe000 	.word	0x03ffe000
    824c:	000113cf 	.word	0x000113cf
    8250:	3c00d68c 	.word	0x3c00d68c
    8254:	000113dd 	.word	0x000113dd

00008258 <imap_sdhc_init>:
    8258:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    825c:	e5901114 	ldr	r1, [r0, #276]
    8260:	e3a02000 	mov	r2, #0	; 0x0
    8264:	e3a03001 	mov	r3, #1	; 0x1
    8268:	e3510001 	cmp	r1, #1	; 0x1
    826c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8270:	e1a04000 	mov	r4, r0
    8274:	e5802048 	str	r2, [r0, #72]
    8278:	e580312c 	str	r3, [r0, #300]
    827c:	e580211c 	str	r2, [r0, #284]
    8280:	0a000007 	beq	82a4 <imap_sdhc_init+0x4c>
    8284:	3a000002 	bcc	8294 <imap_sdhc_init+0x3c>
    8288:	e3510002 	cmp	r1, #2	; 0x2
    828c:	1a00000c 	bne	82c4 <imap_sdhc_init+0x6c>
    8290:	ea000007 	b	82b4 <imap_sdhc_init+0x5c>
    8294:	e3a0000c 	mov	r0, #12	; 0xc
    8298:	ebffe156 	bl	7f8 <pads_init_module>
    829c:	e59f0148 	ldr	r0, [pc, #328]	; 83ec <imap_sdhc_init+0x194>
    82a0:	ea000006 	b	82c0 <imap_sdhc_init+0x68>
    82a4:	e3a0000d 	mov	r0, #13	; 0xd
    82a8:	ebffe152 	bl	7f8 <pads_init_module>
    82ac:	e59f013c 	ldr	r0, [pc, #316]	; 83f0 <imap_sdhc_init+0x198>
    82b0:	ea000002 	b	82c0 <imap_sdhc_init+0x68>
    82b4:	e3a0000e 	mov	r0, #14	; 0xe
    82b8:	ebffe14e 	bl	7f8 <pads_init_module>
    82bc:	e59f0130 	ldr	r0, [pc, #304]	; 83f4 <imap_sdhc_init+0x19c>
    82c0:	ebffe110 	bl	708 <module_enable>
    82c4:	e5942118 	ldr	r2, [r4, #280]
    82c8:	e59f5128 	ldr	r5, [pc, #296]	; 83f8 <imap_sdhc_init+0x1a0>
    82cc:	e5923070 	ldr	r3, [r2, #112]
    82d0:	e1a03943 	asr	r3, r3, #18
    82d4:	e2033007 	and	r3, r3, #7	; 0x7
    82d8:	e3530002 	cmp	r3, #2	; 0x2
    82dc:	03a03003 	moveq	r3, #3	; 0x3
    82e0:	13a03002 	movne	r3, #2	; 0x2
    82e4:	e5843120 	str	r3, [r4, #288]
    82e8:	e3a03007 	mov	r3, #7	; 0x7
    82ec:	e5823000 	str	r3, [r2]
    82f0:	e59f3104 	ldr	r3, [pc, #260]	; 83fc <imap_sdhc_init+0x1a4>
    82f4:	e5843124 	str	r3, [r4, #292]
    82f8:	e59f3100 	ldr	r3, [pc, #256]	; 8400 <imap_sdhc_init+0x1a8>
    82fc:	e5843128 	str	r3, [r4, #296]
    8300:	eb0000eb 	bl	86b4 <get_ticks>
    8304:	e1a06000 	mov	r6, r0
    8308:	e1a07001 	mov	r7, r1
    830c:	e5943118 	ldr	r3, [r4, #280]
    8310:	e3a00001 	mov	r0, #1	; 0x1
    8314:	e5933000 	ldr	r3, [r3]
    8318:	e3130007 	tst	r3, #7	; 0x7
    831c:	0a00000d 	beq	8358 <imap_sdhc_init+0x100>
    8320:	eb0000f4 	bl	86f8 <udelay>
    8324:	eb0000e2 	bl	86b4 <get_ticks>
    8328:	e1a02000 	mov	r2, r0
    832c:	e1a03001 	mov	r3, r1
    8330:	e0522006 	subs	r2, r2, r6
    8334:	e0c33007 	sbc	r3, r3, r7
    8338:	e3530000 	cmp	r3, #0	; 0x0
    833c:	1a000001 	bne	8348 <imap_sdhc_init+0xf0>
    8340:	e1520005 	cmp	r2, r5
    8344:	9afffff0 	bls	830c <imap_sdhc_init+0xb4>
    8348:	e59f00b4 	ldr	r0, [pc, #180]	; 8404 <imap_sdhc_init+0x1ac>
    834c:	e59f30b4 	ldr	r3, [pc, #180]	; 8408 <imap_sdhc_init+0x1b0>
    8350:	e1a0e00f 	mov	lr, pc
    8354:	e593f000 	ldr	pc, [r3]
    8358:	e5943118 	ldr	r3, [r4, #280]
    835c:	e3a01000 	mov	r1, #0	; 0x0
    8360:	e5831024 	str	r1, [r3, #36]
    8364:	e5943118 	ldr	r3, [r4, #280]
    8368:	e3e02000 	mvn	r2, #0	; 0x0
    836c:	e5832044 	str	r2, [r3, #68]
    8370:	e5943118 	ldr	r3, [r4, #280]
    8374:	e1a00001 	mov	r0, r1
    8378:	e5832014 	str	r2, [r3, #20]
    837c:	e5942118 	ldr	r2, [r4, #280]
    8380:	e592304c 	ldr	r3, [r2, #76]
    8384:	e1a03283 	lsl	r3, r3, #5
    8388:	e1a03b23 	lsr	r3, r3, #22
    838c:	e1833803 	orr	r3, r3, r3, lsl #16
    8390:	e582304c 	str	r3, [r2, #76]
    8394:	e5943118 	ldr	r3, [r4, #280]
    8398:	e5831010 	str	r1, [r3, #16]
    839c:	e5942118 	ldr	r2, [r4, #280]
    83a0:	e5923004 	ldr	r3, [r2, #4]
    83a4:	e3833001 	orr	r3, r3, #1	; 0x1
    83a8:	e5823004 	str	r3, [r2, #4]
    83ac:	e5942118 	ldr	r2, [r4, #280]
    83b0:	e59f3054 	ldr	r3, [pc, #84]	; 840c <imap_sdhc_init+0x1b4>
    83b4:	e5823058 	str	r3, [r2, #88]
    83b8:	e5942118 	ldr	r2, [r4, #280]
    83bc:	e3003103 	movw	r3, #259	; 0x103
    83c0:	e5823090 	str	r3, [r2, #144]
    83c4:	e5942130 	ldr	r2, [r4, #304]
    83c8:	e5943118 	ldr	r3, [r4, #280]
    83cc:	e5832088 	str	r2, [r3, #136]
    83d0:	e5942118 	ldr	r2, [r4, #280]
    83d4:	e5923080 	ldr	r3, [r2, #128]
    83d8:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    83dc:	e3833008 	orr	r3, r3, #8	; 0x8
    83e0:	e5823080 	str	r3, [r2, #128]
    83e4:	e28dd004 	add	sp, sp, #4	; 0x4
    83e8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    83ec:	21e39000 	.word	0x21e39000
    83f0:	21e38800 	.word	0x21e38800
    83f4:	21e38c00 	.word	0x21e38c00
    83f8:	0007a11f 	.word	0x0007a11f
    83fc:	00007d34 	.word	0x00007d34
    8400:	00007d60 	.word	0x00007d60
    8404:	000113f5 	.word	0x000113f5
    8408:	3c00d68c 	.word	0x3c00d68c
    840c:	008c2800 	.word	0x008c2800

00008410 <imap_sdhc_set_ios>:
    8410:	e5902048 	ldr	r2, [r0, #72]
    8414:	e590311c 	ldr	r3, [r0, #284]
    8418:	e92d4070 	push	{r4, r5, r6, lr}
    841c:	e1520003 	cmp	r2, r3
    8420:	e1a05000 	mov	r5, r0
    8424:	0a00002a 	beq	84d4 <imap_sdhc_set_ios+0xc4>
    8428:	e5903114 	ldr	r3, [r0, #276]
    842c:	e59f00d0 	ldr	r0, [pc, #208]	; 8504 <imap_sdhc_set_ios+0xf4>
    8430:	e0830000 	add	r0, r3, r0
    8434:	e1a00200 	lsl	r0, r0, #4
    8438:	eb000a40 	bl	ad40 <module_get_clock>
    843c:	e5954048 	ldr	r4, [r5, #72]
    8440:	e1a06000 	mov	r6, r0
    8444:	e1a01004 	mov	r1, r4
    8448:	eb0017e6 	bl	e3e8 <__aeabi_uidivmod>
    844c:	e3510000 	cmp	r1, #0	; 0x0
    8450:	0a000005 	beq	846c <imap_sdhc_set_ios+0x5c>
    8454:	e1a00006 	mov	r0, r6
    8458:	e1a01004 	mov	r1, r4
    845c:	eb001765 	bl	e1f8 <__aeabi_uidiv>
    8460:	e1a000a0 	lsr	r0, r0, #1
    8464:	e2806001 	add	r6, r0, #1	; 0x1
    8468:	ea000003 	b	847c <imap_sdhc_set_ios+0x6c>
    846c:	e1a00006 	mov	r0, r6
    8470:	e1a01004 	mov	r1, r4
    8474:	eb00175f 	bl	e1f8 <__aeabi_uidiv>
    8478:	e1a060a0 	lsr	r6, r0, #1
    847c:	e5953118 	ldr	r3, [r5, #280]
    8480:	e3a04000 	mov	r4, #0	; 0x0
    8484:	e5834010 	str	r4, [r3, #16]
    8488:	e5953118 	ldr	r3, [r5, #280]
    848c:	e1a00005 	mov	r0, r5
    8490:	e583400c 	str	r4, [r3, #12]
    8494:	e59f106c 	ldr	r1, [pc, #108]	; 8508 <imap_sdhc_set_ios+0xf8>
    8498:	e1a02004 	mov	r2, r4
    849c:	ebfffe89 	bl	7ec8 <mci_send_cmd>
    84a0:	e5953118 	ldr	r3, [r5, #280]
    84a4:	e1a00005 	mov	r0, r5
    84a8:	e5836008 	str	r6, [r3, #8]
    84ac:	e59f1054 	ldr	r1, [pc, #84]	; 8508 <imap_sdhc_set_ios+0xf8>
    84b0:	e1a02004 	mov	r2, r4
    84b4:	ebfffe83 	bl	7ec8 <mci_send_cmd>
    84b8:	e5952118 	ldr	r2, [r5, #280]
    84bc:	e3a03001 	mov	r3, #1	; 0x1
    84c0:	e5823010 	str	r3, [r2, #16]
    84c4:	e1a00005 	mov	r0, r5
    84c8:	e1a02004 	mov	r2, r4
    84cc:	e59f1034 	ldr	r1, [pc, #52]	; 8508 <imap_sdhc_set_ios+0xf8>
    84d0:	ebfffe7c 	bl	7ec8 <mci_send_cmd>
    84d4:	e5953044 	ldr	r3, [r5, #68]
    84d8:	e3530004 	cmp	r3, #4	; 0x4
    84dc:	05952118 	ldreq	r2, [r5, #280]
    84e0:	03a03001 	moveq	r3, #1	; 0x1
    84e4:	05823018 	streq	r3, [r2, #24]
    84e8:	08bd8070 	popeq	{r4, r5, r6, pc}
    84ec:	e3530008 	cmp	r3, #8	; 0x8
    84f0:	e5950118 	ldr	r0, [r5, #280]
    84f4:	059f3010 	ldreq	r3, [pc, #16]	; 850c <imap_sdhc_set_ios+0xfc>
    84f8:	13a03000 	movne	r3, #0	; 0x0
    84fc:	e5803018 	str	r3, [r0, #24]
    8500:	e8bd8070 	pop	{r4, r5, r6, pc}
    8504:	021e0a24 	.word	0x021e0a24
    8508:	00202000 	.word	0x00202000
    850c:	00010001 	.word	0x00010001

00008510 <serial_tstc>:
    8510:	e59f3010 	ldr	r3, [pc, #16]	; 8528 <serial_tstc+0x18>
    8514:	e5930018 	ldr	r0, [r3, #24]
    8518:	e1a00220 	lsr	r0, r0, #4
    851c:	e2200001 	eor	r0, r0, #1	; 0x1
    8520:	e2000001 	and	r0, r0, #1	; 0x1
    8524:	e12fff1e 	bx	lr
    8528:	20803000 	.word	0x20803000

0000852c <serial_putc>:
    852c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8530:	e24dd004 	sub	sp, sp, #4	; 0x4
    8534:	e1a06000 	mov	r6, r0
    8538:	eb00005d 	bl	86b4 <get_ticks>
    853c:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    8540:	e3a05000 	mov	r5, #0	; 0x0
    8544:	e0944000 	adds	r4, r4, r0
    8548:	e59f7034 	ldr	r7, [pc, #52]	; 8584 <serial_putc+0x58>
    854c:	e0a55001 	adc	r5, r5, r1
    8550:	e5973018 	ldr	r3, [r7, #24]
    8554:	e3130020 	tst	r3, #32	; 0x20
    8558:	0a000005 	beq	8574 <serial_putc+0x48>
    855c:	eb000054 	bl	86b4 <get_ticks>
    8560:	e1510005 	cmp	r1, r5
    8564:	8a000002 	bhi	8574 <serial_putc+0x48>
    8568:	1afffff8 	bne	8550 <serial_putc+0x24>
    856c:	e1500004 	cmp	r0, r4
    8570:	9afffff6 	bls	8550 <serial_putc+0x24>
    8574:	e59f3008 	ldr	r3, [pc, #8]	; 8584 <serial_putc+0x58>
    8578:	e5836000 	str	r6, [r3]
    857c:	e28dd004 	add	sp, sp, #4	; 0x4
    8580:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8584:	20803000 	.word	0x20803000

00008588 <serial_puts>:
    8588:	e92d4030 	push	{r4, r5, lr}
    858c:	e1a05000 	mov	r5, r0
    8590:	e24dd004 	sub	sp, sp, #4	; 0x4
    8594:	e3a04000 	mov	r4, #0	; 0x0
    8598:	ea000006 	b	85b8 <serial_puts+0x30>
    859c:	e353000a 	cmp	r3, #10	; 0xa
    85a0:	1a000001 	bne	85ac <serial_puts+0x24>
    85a4:	e3a0000d 	mov	r0, #13	; 0xd
    85a8:	ebffffdf 	bl	852c <serial_putc>
    85ac:	e7d50004 	ldrb	r0, [r5, r4]
    85b0:	ebffffdd 	bl	852c <serial_putc>
    85b4:	e2844001 	add	r4, r4, #1	; 0x1
    85b8:	e7d53004 	ldrb	r3, [r5, r4]
    85bc:	e3530000 	cmp	r3, #0	; 0x0
    85c0:	1afffff5 	bne	859c <serial_puts+0x14>
    85c4:	e28dd004 	add	sp, sp, #4	; 0x4
    85c8:	e8bd8030 	pop	{r4, r5, pc}

000085cc <serial_getc>:
    85cc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    85d0:	e59f2024 	ldr	r2, [pc, #36]	; 85fc <serial_getc+0x30>
    85d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    85d8:	e5923018 	ldr	r3, [r2, #24]
    85dc:	e3130010 	tst	r3, #16	; 0x10
    85e0:	1afffffc 	bne	85d8 <serial_getc+0xc>
    85e4:	ebffef10 	bl	422c <tt_rnd_seed>
    85e8:	e59f300c 	ldr	r3, [pc, #12]	; 85fc <serial_getc+0x30>
    85ec:	e5930000 	ldr	r0, [r3]
    85f0:	e6ef0070 	uxtb	r0, r0
    85f4:	e28dd004 	add	sp, sp, #4	; 0x4
    85f8:	e8bd8000 	pop	{pc}
    85fc:	20803000 	.word	0x20803000

00008600 <serial_setbrg>:
    8600:	e92d4010 	push	{r4, lr}
    8604:	e59f0038 	ldr	r0, [pc, #56]	; 8644 <serial_setbrg+0x44>
    8608:	eb0009cc 	bl	ad40 <module_get_clock>
    860c:	e59f1034 	ldr	r1, [pc, #52]	; 8648 <serial_setbrg+0x48>
    8610:	e1a04000 	mov	r4, r0
    8614:	eb001779 	bl	e400 <__aeabi_idiv>
    8618:	e6bf3070 	sxth	r3, r0
    861c:	e59f0028 	ldr	r0, [pc, #40]	; 864c <serial_setbrg+0x4c>
    8620:	e59f1020 	ldr	r1, [pc, #32]	; 8648 <serial_setbrg+0x48>
    8624:	e0204390 	mla	r0, r0, r3, r4
    8628:	e59f4020 	ldr	r4, [pc, #32]	; 8650 <serial_setbrg+0x50>
    862c:	e1a00300 	lsl	r0, r0, #6
    8630:	e5843024 	str	r3, [r4, #36]
    8634:	eb001771 	bl	e400 <__aeabi_idiv>
    8638:	e200003f 	and	r0, r0, #63	; 0x3f
    863c:	e5840028 	str	r0, [r4, #40]
    8640:	e8bd8010 	pop	{r4, pc}
    8644:	21e0a290 	.word	0x21e0a290
    8648:	001c2000 	.word	0x001c2000
    864c:	ffe3e000 	.word	0xffe3e000
    8650:	20803000 	.word	0x20803000

00008654 <init_serial>:
    8654:	e92d4010 	push	{r4, lr}
    8658:	e59f004c 	ldr	r0, [pc, #76]	; 86ac <init_serial+0x58>
    865c:	e59f404c 	ldr	r4, [pc, #76]	; 86b0 <init_serial+0x5c>
    8660:	ebffe028 	bl	708 <module_enable>
    8664:	e3a0000f 	mov	r0, #15	; 0xf
    8668:	ebffe062 	bl	7f8 <pads_init_module>
    866c:	e594302c 	ldr	r3, [r4, #44]
    8670:	e3c330ff 	bic	r3, r3, #255	; 0xff
    8674:	e3833060 	orr	r3, r3, #96	; 0x60
    8678:	e584302c 	str	r3, [r4, #44]
    867c:	ebffffdf 	bl	8600 <serial_setbrg>
    8680:	e5943030 	ldr	r3, [r4, #48]
    8684:	e3a00000 	mov	r0, #0	; 0x0
    8688:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    868c:	e3c33087 	bic	r3, r3, #135	; 0x87
    8690:	e3833c03 	orr	r3, r3, #768	; 0x300
    8694:	e3833001 	orr	r3, r3, #1	; 0x1
    8698:	e5843030 	str	r3, [r4, #48]
    869c:	e594302c 	ldr	r3, [r4, #44]
    86a0:	e3833010 	orr	r3, r3, #16	; 0x10
    86a4:	e584302c 	str	r3, [r4, #44]
    86a8:	e8bd8010 	pop	{r4, pc}
    86ac:	21e12000 	.word	0x21e12000
    86b0:	20803000 	.word	0x20803000

000086b4 <get_ticks>:
    86b4:	e59f1038 	ldr	r1, [pc, #56]	; 86f4 <get_ticks+0x40>
    86b8:	e5913204 	ldr	r3, [r1, #516]
    86bc:	e5912200 	ldr	r2, [r1, #512]
    86c0:	e591c204 	ldr	ip, [r1, #516]
    86c4:	e15c0003 	cmp	ip, r3
    86c8:	15913200 	ldrne	r3, [r1, #512]
    86cc:	01a00002 	moveq	r0, r2
    86d0:	03a01000 	moveq	r1, #0	; 0x0
    86d4:	11a00003 	movne	r0, r3
    86d8:	13a01000 	movne	r1, #0	; 0x0
    86dc:	e3a02000 	mov	r2, #0	; 0x0
    86e0:	e1822000 	orr	r2, r2, r0
    86e4:	e18c3001 	orr	r3, ip, r1
    86e8:	e1a01003 	mov	r1, r3
    86ec:	e1a00002 	mov	r0, r2
    86f0:	e12fff1e 	bx	lr
    86f4:	21b00000 	.word	0x21b00000

000086f8 <udelay>:
    86f8:	e59f1084 	ldr	r1, [pc, #132]	; 8784 <udelay+0x8c>
    86fc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8700:	e5913204 	ldr	r3, [r1, #516]
    8704:	e5912200 	ldr	r2, [r1, #512]
    8708:	e591c204 	ldr	ip, [r1, #516]
    870c:	e15c0003 	cmp	ip, r3
    8710:	15913200 	ldrne	r3, [r1, #512]
    8714:	01a04002 	moveq	r4, r2
    8718:	03a05000 	moveq	r5, #0	; 0x0
    871c:	11a04003 	movne	r4, r3
    8720:	13a05000 	movne	r5, #0	; 0x0
    8724:	e3a02000 	mov	r2, #0	; 0x0
    8728:	e1822004 	orr	r2, r2, r4
    872c:	e18c3005 	orr	r3, ip, r5
    8730:	e0926000 	adds	r6, r2, r0
    8734:	e59fc048 	ldr	ip, [pc, #72]	; 8784 <udelay+0x8c>
    8738:	e2a37000 	adc	r7, r3, #0	; 0x0
    873c:	e59c3204 	ldr	r3, [ip, #516]
    8740:	e59c2200 	ldr	r2, [ip, #512]
    8744:	e59c1204 	ldr	r1, [ip, #516]
    8748:	e1510003 	cmp	r1, r3
    874c:	159c3200 	ldrne	r3, [ip, #512]
    8750:	01a04002 	moveq	r4, r2
    8754:	03a05000 	moveq	r5, #0	; 0x0
    8758:	11a04003 	movne	r4, r3
    875c:	13a05000 	movne	r5, #0	; 0x0
    8760:	e1851001 	orr	r1, r5, r1
    8764:	e3a02000 	mov	r2, #0	; 0x0
    8768:	e1570001 	cmp	r7, r1
    876c:	e1840002 	orr	r0, r4, r2
    8770:	8afffff1 	bhi	873c <udelay+0x44>
    8774:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    8778:	e1560000 	cmp	r6, r0
    877c:	8affffee 	bhi	873c <udelay+0x44>
    8780:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8784:	21b00000 	.word	0x21b00000

00008788 <get_timer>:
    8788:	e59f2074 	ldr	r2, [pc, #116]	; 8804 <get_timer+0x7c>
    878c:	e92d4070 	push	{r4, r5, r6, lr}
    8790:	e5923204 	ldr	r3, [r2, #516]
    8794:	e1a06000 	mov	r6, r0
    8798:	e5921200 	ldr	r1, [r2, #512]
    879c:	e5920204 	ldr	r0, [r2, #516]
    87a0:	e24dd008 	sub	sp, sp, #8	; 0x8
    87a4:	e1500003 	cmp	r0, r3
    87a8:	15923200 	ldrne	r3, [r2, #512]
    87ac:	01a04001 	moveq	r4, r1
    87b0:	03a05000 	moveq	r5, #0	; 0x0
    87b4:	11a04003 	movne	r4, r3
    87b8:	13a05000 	movne	r5, #0	; 0x0
    87bc:	e1a03000 	mov	r3, r0
    87c0:	e3a02000 	mov	r2, #0	; 0x0
    87c4:	e1851003 	orr	r1, r5, r3
    87c8:	e1840002 	orr	r0, r4, r2
    87cc:	e1510002 	cmp	r1, r2
    87d0:	e88d0003 	stm	sp, {r0, r1}
    87d4:	1a000003 	bne	87e8 <get_timer+0x60>
    87d8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    87dc:	eb001685 	bl	e1f8 <__aeabi_uidiv>
    87e0:	e58d0000 	str	r0, [sp]
    87e4:	ea000002 	b	87f4 <get_timer+0x6c>
    87e8:	e1a0000d 	mov	r0, sp
    87ec:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    87f0:	eb000a3f 	bl	b0f4 <__div64_32>
    87f4:	e59d0000 	ldr	r0, [sp]
    87f8:	e0660000 	rsb	r0, r6, r0
    87fc:	e28dd008 	add	sp, sp, #8	; 0x8
    8800:	e8bd8070 	pop	{r4, r5, r6, pc}
    8804:	21b00000 	.word	0x21b00000

00008808 <init_timer>:
    8808:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    880c:	e3a00003 	mov	r0, #3	; 0x3
    8810:	e24dd004 	sub	sp, sp, #4	; 0x4
    8814:	eb000949 	bl	ad40 <module_get_clock>
    8818:	e59f1030 	ldr	r1, [pc, #48]	; 8850 <init_timer+0x48>
    881c:	eb0016f7 	bl	e400 <__aeabi_idiv>
    8820:	e2401001 	sub	r1, r0, #1	; 0x1
    8824:	e59f2028 	ldr	r2, [pc, #40]	; 8854 <init_timer+0x4c>
    8828:	e1a01401 	lsl	r1, r1, #8
    882c:	e3a03000 	mov	r3, #0	; 0x0
    8830:	e3811001 	orr	r1, r1, #1	; 0x1
    8834:	e5823208 	str	r3, [r2, #520]
    8838:	e1a00003 	mov	r0, r3
    883c:	e5823200 	str	r3, [r2, #512]
    8840:	e5823204 	str	r3, [r2, #516]
    8844:	e5821208 	str	r1, [r2, #520]
    8848:	e28dd004 	add	sp, sp, #4	; 0x4
    884c:	e8bd8000 	pop	{pc}
    8850:	000f4240 	.word	0x000f4240
    8854:	21b00000 	.word	0x21b00000

00008858 <e2p_vs_align>:
    8858:	e3a00010 	mov	r0, #16	; 0x10
    885c:	e12fff1e 	bx	lr

00008860 <e2p_vs_reset>:
    8860:	e3a00000 	mov	r0, #0	; 0x0
    8864:	e12fff1e 	bx	lr

00008868 <iic_address_num>:
    8868:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    886c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8870:	ebffdf41 	bl	57c <boot_info>
    8874:	e59f3014 	ldr	r3, [pc, #20]	; 8890 <iic_address_num+0x28>
    8878:	e1a00080 	lsl	r0, r0, #1
    887c:	e20000ff 	and	r0, r0, #255	; 0xff
    8880:	e5830000 	str	r0, [r3]
    8884:	e3a00001 	mov	r0, #1	; 0x1
    8888:	e28dd004 	add	sp, sp, #4	; 0x4
    888c:	e8bd8000 	pop	{pc}
    8890:	3c00d1d0 	.word	0x3c00d1d0

00008894 <iic_read_get_address>:
    8894:	e1d020b2 	ldrh	r2, [r0, #2]
    8898:	e590300c 	ldr	r3, [r0, #12]
    889c:	e92d4010 	push	{r4, lr}
    88a0:	e0833002 	add	r3, r3, r2
    88a4:	e1c030b2 	strh	r3, [r0, #2]
    88a8:	e1a04000 	mov	r4, r0
    88ac:	ebffffed 	bl	8868 <iic_address_num>
    88b0:	e3500000 	cmp	r0, #0	; 0x0
    88b4:	1a000008 	bne	88dc <iic_read_get_address+0x48>
    88b8:	e1d430b2 	ldrh	r3, [r4, #2]
    88bc:	e3530c01 	cmp	r3, #256	; 0x100
    88c0:	1a000005 	bne	88dc <iic_read_get_address+0x48>
    88c4:	e1d430b0 	ldrh	r3, [r4]
    88c8:	e1c400b2 	strh	r0, [r4, #2]
    88cc:	e2833002 	add	r3, r3, #2	; 0x2
    88d0:	e1c430b0 	strh	r3, [r4]
    88d4:	e3a03001 	mov	r3, #1	; 0x1
    88d8:	e1c431b0 	strh	r3, [r4, #16]
    88dc:	ebffffe1 	bl	8868 <iic_address_num>
    88e0:	e3500000 	cmp	r0, #0	; 0x0
    88e4:	e5941008 	ldr	r1, [r4, #8]
    88e8:	0a000004 	beq	8900 <iic_read_get_address+0x6c>
    88ec:	e3510000 	cmp	r1, #0	; 0x0
    88f0:	13a03000 	movne	r3, #0	; 0x0
    88f4:	15843008 	strne	r3, [r4, #8]
    88f8:	1a00000e 	bne	8938 <iic_read_get_address+0xa4>
    88fc:	ea000010 	b	8944 <iic_read_get_address+0xb0>
    8900:	e1d430b2 	ldrh	r3, [r4, #2]
    8904:	e2632c01 	rsb	r2, r3, #256	; 0x100
    8908:	e1520001 	cmp	r2, r1
    890c:	e5842004 	str	r2, [r4, #4]
    8910:	2a000005 	bcs	892c <iic_read_get_address+0x98>
    8914:	e0623001 	rsb	r3, r2, r1
    8918:	e5843008 	str	r3, [r4, #8]
    891c:	e3a03001 	mov	r3, #1	; 0x1
    8920:	e1c431b0 	strh	r3, [r4, #16]
    8924:	e584200c 	str	r2, [r4, #12]
    8928:	ea000003 	b	893c <iic_read_get_address+0xa8>
    892c:	e3510000 	cmp	r1, #0	; 0x0
    8930:	0a000003 	beq	8944 <iic_read_get_address+0xb0>
    8934:	e5840008 	str	r0, [r4, #8]
    8938:	e584100c 	str	r1, [r4, #12]
    893c:	e3a00001 	mov	r0, #1	; 0x1
    8940:	e8bd8010 	pop	{r4, pc}
    8944:	e3a00000 	mov	r0, #0	; 0x0
    8948:	e8bd8010 	pop	{r4, pc}

0000894c <iic_write_get_address>:
    894c:	e1d020b2 	ldrh	r2, [r0, #2]
    8950:	e590300c 	ldr	r3, [r0, #12]
    8954:	e92d4010 	push	{r4, lr}
    8958:	e0833002 	add	r3, r3, r2
    895c:	e1c030b2 	strh	r3, [r0, #2]
    8960:	e1a04000 	mov	r4, r0
    8964:	ebffffbf 	bl	8868 <iic_address_num>
    8968:	e3500000 	cmp	r0, #0	; 0x0
    896c:	1a000008 	bne	8994 <iic_write_get_address+0x48>
    8970:	e1d430b2 	ldrh	r3, [r4, #2]
    8974:	e3530c01 	cmp	r3, #256	; 0x100
    8978:	1a000005 	bne	8994 <iic_write_get_address+0x48>
    897c:	e1d430b0 	ldrh	r3, [r4]
    8980:	e1c400b2 	strh	r0, [r4, #2]
    8984:	e2833002 	add	r3, r3, #2	; 0x2
    8988:	e1c430b0 	strh	r3, [r4]
    898c:	e3a03001 	mov	r3, #1	; 0x1
    8990:	e1c431b0 	strh	r3, [r4, #16]
    8994:	e1d430b2 	ldrh	r3, [r4, #2]
    8998:	e5940008 	ldr	r0, [r4, #8]
    899c:	e203300f 	and	r3, r3, #15	; 0xf
    89a0:	e2632010 	rsb	r2, r3, #16	; 0x10
    89a4:	e1520000 	cmp	r2, r0
    89a8:	e5842004 	str	r2, [r4, #4]
    89ac:	2a000004 	bcs	89c4 <iic_write_get_address+0x78>
    89b0:	e0623000 	rsb	r3, r2, r0
    89b4:	e5843008 	str	r3, [r4, #8]
    89b8:	e584200c 	str	r2, [r4, #12]
    89bc:	e3a00001 	mov	r0, #1	; 0x1
    89c0:	e8bd8010 	pop	{r4, pc}
    89c4:	e3500000 	cmp	r0, #0	; 0x0
    89c8:	13a03000 	movne	r3, #0	; 0x0
    89cc:	1584000c 	strne	r0, [r4, #12]
    89d0:	15843008 	strne	r3, [r4, #8]
    89d4:	13a00001 	movne	r0, #1	; 0x1
    89d8:	e8bd8010 	pop	{r4, pc}

000089dc <iic_word_addr_func>:
    89dc:	e92d4030 	push	{r4, r5, lr}
    89e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    89e4:	e1a04000 	mov	r4, r0
    89e8:	e1a05001 	mov	r5, r1
    89ec:	ebffff9d 	bl	8868 <iic_address_num>
    89f0:	e3500000 	cmp	r0, #0	; 0x0
    89f4:	16ff0074 	uxthne	r0, r4
    89f8:	1a00000c 	bne	8a30 <iic_word_addr_func+0x54>
    89fc:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    8a00:	e0051003 	and	r1, r5, r3
    8a04:	e3a020ff 	mov	r2, #255	; 0xff
    8a08:	e3510000 	cmp	r1, #0	; 0x0
    8a0c:	e0040002 	and	r0, r4, r2
    8a10:	aa000005 	bge	8a2c <iic_word_addr_func+0x50>
    8a14:	e3e02000 	mvn	r2, #0	; 0x0
    8a18:	e0900002 	adds	r0, r0, r2
    8a1c:	e3e020ff 	mvn	r2, #255	; 0xff
    8a20:	e1800002 	orr	r0, r0, r2
    8a24:	e3a02001 	mov	r2, #1	; 0x1
    8a28:	e0900002 	adds	r0, r0, r2
    8a2c:	e6ff0070 	uxth	r0, r0
    8a30:	e28dd004 	add	sp, sp, #4	; 0x4
    8a34:	e8bd8030 	pop	{r4, r5, pc}

00008a38 <iic_addr_func>:
    8a38:	e92d4030 	push	{r4, r5, lr}
    8a3c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8a40:	e1a04000 	mov	r4, r0
    8a44:	e1a05001 	mov	r5, r1
    8a48:	ebffff86 	bl	8868 <iic_address_num>
    8a4c:	e3500000 	cmp	r0, #0	; 0x0
    8a50:	e59f003c 	ldr	r0, [pc, #60]	; 8a94 <iic_addr_func+0x5c>
    8a54:	11d000b0 	ldrhne	r0, [r0]
    8a58:	1a00000b 	bne	8a8c <iic_addr_func+0x54>
    8a5c:	e1a02005 	mov	r2, r5
    8a60:	e1a03fc2 	asr	r3, r2, #31
    8a64:	e1a02fc3 	asr	r2, r3, #31
    8a68:	e20220ff 	and	r2, r2, #255	; 0xff
    8a6c:	e3a03000 	mov	r3, #0	; 0x0
    8a70:	e0922004 	adds	r2, r2, r4
    8a74:	e0a33005 	adc	r3, r3, r5
    8a78:	e5900000 	ldr	r0, [r0]
    8a7c:	e1a01422 	lsr	r1, r2, #8
    8a80:	e1811c03 	orr	r1, r1, r3, lsl #24
    8a84:	e1800081 	orr	r0, r0, r1, lsl #1
    8a88:	e6ff0070 	uxth	r0, r0
    8a8c:	e28dd004 	add	sp, sp, #4	; 0x4
    8a90:	e8bd8030 	pop	{r4, r5, pc}
    8a94:	3c00d1d0 	.word	0x3c00d1d0

00008a98 <e2p_vs_write>:
    8a98:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8a9c:	e1a01003 	mov	r1, r3
    8aa0:	e24dd024 	sub	sp, sp, #36	; 0x24
    8aa4:	e1a06000 	mov	r6, r0
    8aa8:	e1a00002 	mov	r0, r2
    8aac:	e1a05003 	mov	r5, r3
    8ab0:	e1a04002 	mov	r4, r2
    8ab4:	ebffffdf 	bl	8a38 <iic_addr_func>
    8ab8:	e1a01005 	mov	r1, r5
    8abc:	e1cd00bc 	strh	r0, [sp, #12]
    8ac0:	e1a00004 	mov	r0, r4
    8ac4:	ebffffc4 	bl	89dc <iic_word_addr_func>
    8ac8:	e3a03000 	mov	r3, #0	; 0x0
    8acc:	e59d2048 	ldr	r2, [sp, #72]
    8ad0:	e1a05003 	mov	r5, r3
    8ad4:	e58d3018 	str	r3, [sp, #24]
    8ad8:	e28dc00c 	add	ip, sp, #12	; 0xc
    8adc:	e3a03001 	mov	r3, #1	; 0x1
    8ae0:	e1cd00be 	strh	r0, [sp, #14]
    8ae4:	e58d2014 	str	r2, [sp, #20]
    8ae8:	e1cd31bc 	strh	r3, [sp, #28]
    8aec:	e59fb10c 	ldr	fp, [pc, #268]	; 8c00 <e2p_vs_write+0x168>
    8af0:	e3a0a001 	mov	sl, #1	; 0x1
    8af4:	e1a08005 	mov	r8, r5
    8af8:	e28c9012 	add	r9, ip, #18	; 0x12
    8afc:	ea000037 	b	8be0 <e2p_vs_write+0x148>
    8b00:	ebffff58 	bl	8868 <iic_address_num>
    8b04:	e3500000 	cmp	r0, #0	; 0x0
    8b08:	e1dd10be 	ldrh	r1, [sp, #14]
    8b0c:	11a03421 	lsrne	r3, r1, #8
    8b10:	15cd301e 	strbne	r3, [sp, #30]
    8b14:	15cd101f 	strbne	r1, [sp, #31]
    8b18:	13a07002 	movne	r7, #2	; 0x2
    8b1c:	1a000005 	bne	8b38 <e2p_vs_write+0xa0>
    8b20:	e6ef1071 	uxtb	r1, r1
    8b24:	e5cd101e 	strb	r1, [sp, #30]
    8b28:	e59f00d4 	ldr	r0, [pc, #212]	; 8c04 <e2p_vs_write+0x16c>
    8b2c:	e1a0e00f 	mov	lr, pc
    8b30:	e59bf000 	ldr	pc, [fp]
    8b34:	e3a07001 	mov	r7, #1	; 0x1
    8b38:	ebfffedd 	bl	86b4 <get_ticks>
    8b3c:	e1dd31bc 	ldrh	r3, [sp, #28]
    8b40:	e1a04000 	mov	r4, r0
    8b44:	e3530000 	cmp	r3, #0	; 0x0
    8b48:	0a000016 	beq	8ba8 <e2p_vs_write+0x110>
    8b4c:	e3a0c000 	mov	ip, #0	; 0x0
    8b50:	e5dd200c 	ldrb	r2, [sp, #12]
    8b54:	e1a00008 	mov	r0, r8
    8b58:	e1a0100a 	mov	r1, sl
    8b5c:	e1a03008 	mov	r3, r8
    8b60:	e1cdc1bc 	strh	ip, [sp, #28]
    8b64:	e58da000 	str	sl, [sp]
    8b68:	e58d8004 	str	r8, [sp, #4]
    8b6c:	eb000137 	bl	9050 <iic_init>
    8b70:	e3500000 	cmp	r0, #0	; 0x0
    8b74:	1a00000b 	bne	8ba8 <e2p_vs_write+0x110>
    8b78:	e1a00008 	mov	r0, r8
    8b7c:	ea00001d 	b	8bf8 <e2p_vs_write+0x160>
    8b80:	ebfffecb 	bl	86b4 <get_ticks>
    8b84:	e1540000 	cmp	r4, r0
    8b88:	2a000008 	bcs	8bb0 <e2p_vs_write+0x118>
    8b8c:	e59f0074 	ldr	r0, [pc, #116]	; 8c08 <e2p_vs_write+0x170>
    8b90:	e1a0e00f 	mov	lr, pc
    8b94:	e59bf000 	ldr	pc, [fp]
    8b98:	e59d3018 	ldr	r3, [sp, #24]
    8b9c:	e0853003 	add	r3, r5, r3
    8ba0:	e6ff5073 	uxth	r5, r3
    8ba4:	ea00000b 	b	8bd8 <e2p_vs_write+0x140>
    8ba8:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8bac:	e2844e12 	add	r4, r4, #288	; 0x120
    8bb0:	e59dc018 	ldr	ip, [sp, #24]
    8bb4:	e1a01009 	mov	r1, r9
    8bb8:	e1a02007 	mov	r2, r7
    8bbc:	e1a03006 	mov	r3, r6
    8bc0:	e3a00000 	mov	r0, #0	; 0x0
    8bc4:	e58dc000 	str	ip, [sp]
    8bc8:	e58da004 	str	sl, [sp, #4]
    8bcc:	eb000078 	bl	8db4 <iicreg_write>
    8bd0:	e3500000 	cmp	r0, #0	; 0x0
    8bd4:	0affffe9 	beq	8b80 <e2p_vs_write+0xe8>
    8bd8:	e59d3018 	ldr	r3, [sp, #24]
    8bdc:	e0866003 	add	r6, r6, r3
    8be0:	e28d000c 	add	r0, sp, #12	; 0xc
    8be4:	ebffff58 	bl	894c <iic_write_get_address>
    8be8:	e3500000 	cmp	r0, #0	; 0x0
    8bec:	1affffc3 	bne	8b00 <e2p_vs_write+0x68>
    8bf0:	e59d2048 	ldr	r2, [sp, #72]
    8bf4:	e0650002 	rsb	r0, r5, r2
    8bf8:	e28dd024 	add	sp, sp, #36	; 0x24
    8bfc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8c00:	3c00d68c 	.word	0x3c00d68c
    8c04:	00011408 	.word	0x00011408
    8c08:	00011412 	.word	0x00011412

00008c0c <e2p_vs_read>:
    8c0c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8c10:	e1a01003 	mov	r1, r3
    8c14:	e24dd024 	sub	sp, sp, #36	; 0x24
    8c18:	e1a06000 	mov	r6, r0
    8c1c:	e1a00002 	mov	r0, r2
    8c20:	e1a05003 	mov	r5, r3
    8c24:	e1a04002 	mov	r4, r2
    8c28:	e59d7048 	ldr	r7, [sp, #72]
    8c2c:	ebffff81 	bl	8a38 <iic_addr_func>
    8c30:	e1a01005 	mov	r1, r5
    8c34:	e1cd00bc 	strh	r0, [sp, #12]
    8c38:	e1a00004 	mov	r0, r4
    8c3c:	ebffff66 	bl	89dc <iic_word_addr_func>
    8c40:	e28db00c 	add	fp, sp, #12	; 0xc
    8c44:	e3a03000 	mov	r3, #0	; 0x0
    8c48:	e3a02001 	mov	r2, #1	; 0x1
    8c4c:	e1cd00be 	strh	r0, [sp, #14]
    8c50:	e1cd21bc 	strh	r2, [sp, #28]
    8c54:	e58d7014 	str	r7, [sp, #20]
    8c58:	e58d3018 	str	r3, [sp, #24]
    8c5c:	e1a05003 	mov	r5, r3
    8c60:	e3a09001 	mov	r9, #1	; 0x1
    8c64:	e28ba012 	add	sl, fp, #18	; 0x12
    8c68:	ea00002b 	b	8d1c <e2p_vs_read+0x110>
    8c6c:	ebfffefd 	bl	8868 <iic_address_num>
    8c70:	e1dd20be 	ldrh	r2, [sp, #14]
    8c74:	e3500000 	cmp	r0, #0	; 0x0
    8c78:	11a03422 	lsrne	r3, r2, #8
    8c7c:	15cd301e 	strbne	r3, [sp, #30]
    8c80:	15cd201f 	strbne	r2, [sp, #31]
    8c84:	13a08002 	movne	r8, #2	; 0x2
    8c88:	05cd201e 	strbeq	r2, [sp, #30]
    8c8c:	03a08001 	moveq	r8, #1	; 0x1
    8c90:	ebfffe87 	bl	86b4 <get_ticks>
    8c94:	e1dd31bc 	ldrh	r3, [sp, #28]
    8c98:	e1a04000 	mov	r4, r0
    8c9c:	e3530000 	cmp	r3, #0	; 0x0
    8ca0:	0a000010 	beq	8ce8 <e2p_vs_read+0xdc>
    8ca4:	e3a0c000 	mov	ip, #0	; 0x0
    8ca8:	e5dd200c 	ldrb	r2, [sp, #12]
    8cac:	e1a00005 	mov	r0, r5
    8cb0:	e1a01009 	mov	r1, r9
    8cb4:	e1a03005 	mov	r3, r5
    8cb8:	e1cdc1bc 	strh	ip, [sp, #28]
    8cbc:	e58d9000 	str	r9, [sp]
    8cc0:	e58d5004 	str	r5, [sp, #4]
    8cc4:	eb0000e1 	bl	9050 <iic_init>
    8cc8:	e3500000 	cmp	r0, #0	; 0x0
    8ccc:	1a000005 	bne	8ce8 <e2p_vs_read+0xdc>
    8cd0:	e1a07005 	mov	r7, r5
    8cd4:	ea000014 	b	8d2c <e2p_vs_read+0x120>
    8cd8:	ebfffe75 	bl	86b4 <get_ticks>
    8cdc:	e1540000 	cmp	r4, r0
    8ce0:	2a000002 	bcs	8cf0 <e2p_vs_read+0xe4>
    8ce4:	ea00000a 	b	8d14 <e2p_vs_read+0x108>
    8ce8:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8cec:	e2844e12 	add	r4, r4, #288	; 0x120
    8cf0:	e59dc018 	ldr	ip, [sp, #24]
    8cf4:	e1a0100a 	mov	r1, sl
    8cf8:	e1a02008 	mov	r2, r8
    8cfc:	e1a03006 	mov	r3, r6
    8d00:	e3a00000 	mov	r0, #0	; 0x0
    8d04:	e58dc000 	str	ip, [sp]
    8d08:	eb000061 	bl	8e94 <iic_read_func>
    8d0c:	e3500000 	cmp	r0, #0	; 0x0
    8d10:	0afffff0 	beq	8cd8 <e2p_vs_read+0xcc>
    8d14:	e59d3018 	ldr	r3, [sp, #24]
    8d18:	e0866003 	add	r6, r6, r3
    8d1c:	e1a0000b 	mov	r0, fp
    8d20:	ebfffedb 	bl	8894 <iic_read_get_address>
    8d24:	e3500000 	cmp	r0, #0	; 0x0
    8d28:	1affffcf 	bne	8c6c <e2p_vs_read+0x60>
    8d2c:	e1a00007 	mov	r0, r7
    8d30:	e28dd024 	add	sp, sp, #36	; 0x24
    8d34:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00008d38 <iicreg_read>:
    8d38:	e3500001 	cmp	r0, #1	; 0x1
    8d3c:	e92d4070 	push	{r4, r5, r6, lr}
    8d40:	83a00000 	movhi	r0, #0	; 0x0
    8d44:	e1a05001 	mov	r5, r1
    8d48:	e1a04002 	mov	r4, r2
    8d4c:	88bd8070 	pophi	{r4, r5, r6, pc}
    8d50:	e59f3058 	ldr	r3, [pc, #88]	; 8db0 <iicreg_read+0x78>
    8d54:	e5932070 	ldr	r2, [r3, #112]
    8d58:	e2022008 	and	r2, r2, #8	; 0x8
    8d5c:	ea000000 	b	8d64 <iicreg_read+0x2c>
    8d60:	e5931010 	ldr	r1, [r3, #16]
    8d64:	e3520000 	cmp	r2, #0	; 0x0
    8d68:	1afffffc 	bne	8d60 <iicreg_read+0x28>
    8d6c:	e59f103c 	ldr	r1, [pc, #60]	; 8db0 <iicreg_read+0x78>
    8d70:	e1a00004 	mov	r0, r4
    8d74:	e3a06c01 	mov	r6, #256	; 0x100
    8d78:	e591c070 	ldr	ip, [r1, #112]
    8d7c:	e2503000 	subs	r3, r0, #0	; 0x0
    8d80:	13a03001 	movne	r3, #1	; 0x1
    8d84:	e01330ac 	ands	r3, r3, ip, lsr #1
    8d88:	15816010 	strne	r6, [r1, #16]
    8d8c:	12400001 	subne	r0, r0, #1	; 0x1
    8d90:	e31c0008 	tst	ip, #8	; 0x8
    8d94:	15913010 	ldrne	r3, [r1, #16]
    8d98:	12822001 	addne	r2, r2, #1	; 0x1
    8d9c:	14c53001 	strbne	r3, [r5], #1
    8da0:	e1520004 	cmp	r2, r4
    8da4:	1afffff3 	bne	8d78 <iicreg_read+0x40>
    8da8:	e3a00001 	mov	r0, #1	; 0x1
    8dac:	e8bd8070 	pop	{r4, r5, r6, pc}
    8db0:	20300000 	.word	0x20300000

00008db4 <iicreg_write>:
    8db4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8db8:	e59d4018 	ldr	r4, [sp, #24]
    8dbc:	e3500001 	cmp	r0, #1	; 0x1
    8dc0:	e1a05001 	mov	r5, r1
    8dc4:	e5dd801c 	ldrb	r8, [sp, #28]
    8dc8:	e1a01003 	mov	r1, r3
    8dcc:	e0847002 	add	r7, r4, r2
    8dd0:	8a00002c 	bhi	8e88 <iicreg_write+0xd4>
    8dd4:	e59f00b4 	ldr	r0, [pc, #180]	; 8e90 <iicreg_write+0xdc>
    8dd8:	e5903034 	ldr	r3, [r0, #52]
    8ddc:	e213c040 	ands	ip, r3, #64	; 0x40
    8de0:	15903080 	ldrne	r3, [r0, #128]
    8de4:	15903054 	ldrne	r3, [r0, #84]
    8de8:	13a00000 	movne	r0, #0	; 0x0
    8dec:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    8df0:	e5903070 	ldr	r3, [r0, #112]
    8df4:	e3130004 	tst	r3, #4	; 0x4
    8df8:	11a06000 	movne	r6, r0
    8dfc:	11a0000c 	movne	r0, ip
    8e00:	11a0c006 	movne	ip, r6
    8e04:	1a00000d 	bne	8e40 <iicreg_write+0x8c>
    8e08:	ea00001e 	b	8e88 <iicreg_write+0xd4>
    8e0c:	e5963070 	ldr	r3, [r6, #112]
    8e10:	e3130002 	tst	r3, #2	; 0x2
    8e14:	0afffffc 	beq	8e0c <iicreg_write+0x58>
    8e18:	e3520000 	cmp	r2, #0	; 0x0
    8e1c:	14d53001 	ldrbne	r3, [r5], #1
    8e20:	12422001 	subne	r2, r2, #1	; 0x1
    8e24:	1a000003 	bne	8e38 <iicreg_write+0x84>
    8e28:	e3540000 	cmp	r4, #0	; 0x0
    8e2c:	0a000002 	beq	8e3c <iicreg_write+0x88>
    8e30:	e4d13001 	ldrb	r3, [r1], #1
    8e34:	e2444001 	sub	r4, r4, #1	; 0x1
    8e38:	e58c3010 	str	r3, [ip, #16]
    8e3c:	e2800001 	add	r0, r0, #1	; 0x1
    8e40:	e1500007 	cmp	r0, r7
    8e44:	3afffff0 	bcc	8e0c <iicreg_write+0x58>
    8e48:	e59f2040 	ldr	r2, [pc, #64]	; 8e90 <iicreg_write+0xdc>
    8e4c:	e5923070 	ldr	r3, [r2, #112]
    8e50:	e3130004 	tst	r3, #4	; 0x4
    8e54:	0afffffc 	beq	8e4c <iicreg_write+0x98>
    8e58:	e3580000 	cmp	r8, #0	; 0x0
    8e5c:	159f202c 	ldrne	r2, [pc, #44]	; 8e90 <iicreg_write+0xdc>
    8e60:	0a000006 	beq	8e80 <iicreg_write+0xcc>
    8e64:	e5923034 	ldr	r3, [r2, #52]
    8e68:	e3130c02 	tst	r3, #512	; 0x200
    8e6c:	0afffffc 	beq	8e64 <iicreg_write+0xb0>
    8e70:	e59f3018 	ldr	r3, [pc, #24]	; 8e90 <iicreg_write+0xdc>
    8e74:	e3010964 	movw	r0, #6500	; 0x1964
    8e78:	e5933060 	ldr	r3, [r3, #96]
    8e7c:	ebfffe1d 	bl	86f8 <udelay>
    8e80:	e3a00001 	mov	r0, #1	; 0x1
    8e84:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8e88:	e3a00000 	mov	r0, #0	; 0x0
    8e8c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8e90:	20300000 	.word	0x20300000

00008e94 <iic_read_func>:
    8e94:	e92d4070 	push	{r4, r5, r6, lr}
    8e98:	e59f4080 	ldr	r4, [pc, #128]	; 8f20 <iic_read_func+0x8c>
    8e9c:	e1a06003 	mov	r6, r3
    8ea0:	e5d43000 	ldrb	r3, [r4]
    8ea4:	e24dd008 	sub	sp, sp, #8	; 0x8
    8ea8:	e243300a 	sub	r3, r3, #10	; 0xa
    8eac:	e6ef3073 	uxtb	r3, r3
    8eb0:	e3530001 	cmp	r3, #1	; 0x1
    8eb4:	e1a05000 	mov	r5, r0
    8eb8:	9a00000b 	bls	8eec <iic_read_func+0x58>
    8ebc:	e3a0e000 	mov	lr, #0	; 0x0
    8ec0:	e1a0300e 	mov	r3, lr
    8ec4:	e3a0c001 	mov	ip, #1	; 0x1
    8ec8:	e58dc004 	str	ip, [sp, #4]
    8ecc:	e58de000 	str	lr, [sp]
    8ed0:	ebffffb7 	bl	8db4 <iicreg_write>
    8ed4:	e3500000 	cmp	r0, #0	; 0x0
    8ed8:	03a0300c 	moveq	r3, #12	; 0xc
    8edc:	05c43000 	strbeq	r3, [r4]
    8ee0:	0a00000c 	beq	8f18 <iic_read_func+0x84>
    8ee4:	e3a0300a 	mov	r3, #10	; 0xa
    8ee8:	e5c43000 	strb	r3, [r4]
    8eec:	e59d2018 	ldr	r2, [sp, #24]
    8ef0:	e1a00005 	mov	r0, r5
    8ef4:	e1a01006 	mov	r1, r6
    8ef8:	ebffff8e 	bl	8d38 <iicreg_read>
    8efc:	e59f201c 	ldr	r2, [pc, #28]	; 8f20 <iic_read_func+0x8c>
    8f00:	e3500000 	cmp	r0, #0	; 0x0
    8f04:	13a0300d 	movne	r3, #13	; 0xd
    8f08:	03a0300b 	moveq	r3, #11	; 0xb
    8f0c:	15c23000 	strbne	r3, [r2]
    8f10:	13a00001 	movne	r0, #1	; 0x1
    8f14:	05c23000 	strbeq	r3, [r2]
    8f18:	e28dd008 	add	sp, sp, #8	; 0x8
    8f1c:	e8bd8070 	pop	{r4, r5, r6, pc}
    8f20:	3c00d1d4 	.word	0x3c00d1d4

00008f24 <iic_writeread>:
    8f24:	e92d4070 	push	{r4, r5, r6, lr}
    8f28:	e59f4088 	ldr	r4, [pc, #136]	; 8fb8 <iic_writeread+0x94>
    8f2c:	e24dd010 	sub	sp, sp, #16	; 0x10
    8f30:	e5d43000 	ldrb	r3, [r4]
    8f34:	e5cd100f 	strb	r1, [sp, #15]
    8f38:	e243300a 	sub	r3, r3, #10	; 0xa
    8f3c:	e6ef3073 	uxtb	r3, r3
    8f40:	e3530001 	cmp	r3, #1	; 0x1
    8f44:	e1a06002 	mov	r6, r2
    8f48:	e1a05000 	mov	r5, r0
    8f4c:	9a00000c 	bls	8f84 <iic_writeread+0x60>
    8f50:	e3a0c000 	mov	ip, #0	; 0x0
    8f54:	e1a0300c 	mov	r3, ip
    8f58:	e28d100f 	add	r1, sp, #15	; 0xf
    8f5c:	e3a02001 	mov	r2, #1	; 0x1
    8f60:	e58dc000 	str	ip, [sp]
    8f64:	e58dc004 	str	ip, [sp, #4]
    8f68:	ebffff91 	bl	8db4 <iicreg_write>
    8f6c:	e3500000 	cmp	r0, #0	; 0x0
    8f70:	03a0300c 	moveq	r3, #12	; 0xc
    8f74:	05c43000 	strbeq	r3, [r4]
    8f78:	0a00000c 	beq	8fb0 <iic_writeread+0x8c>
    8f7c:	e3a0300a 	mov	r3, #10	; 0xa
    8f80:	e5c43000 	strb	r3, [r4]
    8f84:	e3a02001 	mov	r2, #1	; 0x1
    8f88:	e1a00005 	mov	r0, r5
    8f8c:	e1a01006 	mov	r1, r6
    8f90:	ebffff68 	bl	8d38 <iicreg_read>
    8f94:	e59f201c 	ldr	r2, [pc, #28]	; 8fb8 <iic_writeread+0x94>
    8f98:	e3500000 	cmp	r0, #0	; 0x0
    8f9c:	13a0300d 	movne	r3, #13	; 0xd
    8fa0:	03a0300b 	moveq	r3, #11	; 0xb
    8fa4:	15c23000 	strbne	r3, [r2]
    8fa8:	13a00001 	movne	r0, #1	; 0x1
    8fac:	05c23000 	strbeq	r3, [r2]
    8fb0:	e28dd010 	add	sp, sp, #16	; 0x10
    8fb4:	e8bd8070 	pop	{r4, r5, r6, pc}
    8fb8:	3c00d1d4 	.word	0x3c00d1d4

00008fbc <iicreg_init>:
    8fbc:	e92d4030 	push	{r4, r5, lr}
    8fc0:	e1a04000 	mov	r4, r0
    8fc4:	e24dd004 	sub	sp, sp, #4	; 0x4
    8fc8:	e59f0074 	ldr	r0, [pc, #116]	; 9044 <iicreg_init+0x88>
    8fcc:	e1a05002 	mov	r5, r2
    8fd0:	eb00075a 	bl	ad40 <module_get_clock>
    8fd4:	e3540001 	cmp	r4, #1	; 0x1
    8fd8:	83a00000 	movhi	r0, #0	; 0x0
    8fdc:	8a000016 	bhi	903c <iicreg_init+0x80>
    8fe0:	e59f1060 	ldr	r1, [pc, #96]	; 9048 <iicreg_init+0x8c>
    8fe4:	eb001483 	bl	e1f8 <__aeabi_uidiv>
    8fe8:	e59f205c 	ldr	r2, [pc, #92]	; 904c <iicreg_init+0x90>
    8fec:	e1a010a0 	lsr	r1, r0, #1
    8ff0:	e3a0c000 	mov	ip, #0	; 0x0
    8ff4:	e3a03043 	mov	r3, #67	; 0x43
    8ff8:	e582c06c 	str	ip, [r2, #108]
    8ffc:	e5823000 	str	r3, [r2]
    9000:	e5820014 	str	r0, [r2, #20]
    9004:	e5820018 	str	r0, [r2, #24]
    9008:	e58210a0 	str	r1, [r2, #160]
    900c:	e59230a0 	ldr	r3, [r2, #160]
    9010:	e1a000a5 	lsr	r0, r5, #1
    9014:	e3c33c01 	bic	r3, r3, #256	; 0x100
    9018:	e58230a0 	str	r3, [r2, #160]
    901c:	e5923000 	ldr	r3, [r2]
    9020:	e3a01001 	mov	r1, #1	; 0x1
    9024:	e3833020 	orr	r3, r3, #32	; 0x20
    9028:	e5823000 	str	r3, [r2]
    902c:	e582c030 	str	ip, [r2, #48]
    9030:	e5820004 	str	r0, [r2, #4]
    9034:	e582106c 	str	r1, [r2, #108]
    9038:	e1a00001 	mov	r0, r1
    903c:	e28dd004 	add	sp, sp, #4	; 0x4
    9040:	e8bd8030 	pop	{r4, r5, pc}
    9044:	21e0a360 	.word	0x21e0a360
    9048:	0009c400 	.word	0x0009c400
    904c:	20300000 	.word	0x20300000

00009050 <iic_init>:
    9050:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9054:	e59fc054 	ldr	ip, [pc, #84]	; 90b0 <iic_init+0x60>
    9058:	e24dd004 	sub	sp, sp, #4	; 0x4
    905c:	e3a0e00d 	mov	lr, #13	; 0xd
    9060:	e1a0a000 	mov	sl, r0
    9064:	e59f0048 	ldr	r0, [pc, #72]	; 90b4 <iic_init+0x64>
    9068:	e1a04001 	mov	r4, r1
    906c:	e1a05002 	mov	r5, r2
    9070:	e1a06003 	mov	r6, r3
    9074:	e5cce000 	strb	lr, [ip]
    9078:	e5dd7020 	ldrb	r7, [sp, #32]
    907c:	e5dd8024 	ldrb	r8, [sp, #36]
    9080:	ebffdda0 	bl	708 <module_enable>
    9084:	e3a0000a 	mov	r0, #10	; 0xa
    9088:	ebffddda 	bl	7f8 <pads_init_module>
    908c:	e1a0000a 	mov	r0, sl
    9090:	e1a01004 	mov	r1, r4
    9094:	e1a02005 	mov	r2, r5
    9098:	e1a03006 	mov	r3, r6
    909c:	e58d7020 	str	r7, [sp, #32]
    90a0:	e58d8024 	str	r8, [sp, #36]
    90a4:	e28dd004 	add	sp, sp, #4	; 0x4
    90a8:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    90ac:	eaffffc2 	b	8fbc <iicreg_init>
    90b0:	3c00d1d4 	.word	0x3c00d1d4
    90b4:	21e10c00 	.word	0x21e10c00

000090b8 <spi_set_bytes>:
    90b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    90bc:	e59f3020 	ldr	r3, [pc, #32]	; 90e4 <spi_set_bytes+0x2c>
    90c0:	e1a01000 	mov	r1, r0
    90c4:	e5830000 	str	r0, [r3]
    90c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    90cc:	e59f0014 	ldr	r0, [pc, #20]	; 90e8 <spi_set_bytes+0x30>
    90d0:	e59f3014 	ldr	r3, [pc, #20]	; 90ec <spi_set_bytes+0x34>
    90d4:	e1a0e00f 	mov	lr, pc
    90d8:	e593f000 	ldr	pc, [r3]
    90dc:	e28dd004 	add	sp, sp, #4	; 0x4
    90e0:	e8bd8000 	pop	{pc}
    90e4:	3c00d0c0 	.word	0x3c00d0c0
    90e8:	00011419 	.word	0x00011419
    90ec:	3c00d68c 	.word	0x3c00d68c

000090f0 <spi_fifo_clear>:
    90f0:	e59f2028 	ldr	r2, [pc, #40]	; 9120 <spi_fifo_clear+0x30>
    90f4:	e592300c 	ldr	r3, [r2, #12]
    90f8:	e3130010 	tst	r3, #16	; 0x10
    90fc:	1afffffc 	bne	90f4 <spi_fifo_clear+0x4>
    9100:	ea000001 	b	910c <spi_fifo_clear+0x1c>
    9104:	e5923008 	ldr	r3, [r2, #8]
    9108:	ea000000 	b	9110 <spi_fifo_clear+0x20>
    910c:	e59f200c 	ldr	r2, [pc, #12]	; 9120 <spi_fifo_clear+0x30>
    9110:	e592300c 	ldr	r3, [r2, #12]
    9114:	e3130004 	tst	r3, #4	; 0x4
    9118:	1afffff9 	bne	9104 <spi_fifo_clear+0x14>
    911c:	e12fff1e 	bx	lr
    9120:	20500000 	.word	0x20500000

00009124 <flash_vs_align>:
    9124:	e3a00c01 	mov	r0, #256	; 0x100
    9128:	e12fff1e 	bx	lr

0000912c <spi_exchange>:
    912c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9130:	e5905008 	ldr	r5, [r0, #8]
    9134:	e3510001 	cmp	r1, #1	; 0x1
    9138:	e24dd00c 	sub	sp, sp, #12	; 0xc
    913c:	e1a0a000 	mov	sl, r0
    9140:	e5908004 	ldr	r8, [r0, #4]
    9144:	d1a04005 	movle	r4, r5
    9148:	d3a07000 	movle	r7, #0	; 0x0
    914c:	da000020 	ble	91d4 <spi_exchange+0xa8>
    9150:	e280a00c 	add	sl, r0, #12	; 0xc
    9154:	e1a01008 	mov	r1, r8
    9158:	e1a0000d 	mov	r0, sp
    915c:	e1a02005 	mov	r2, r5
    9160:	ebffe018 	bl	11c8 <memcpy>
    9164:	e99a0012 	ldmib	sl, {r1, r4}
    9168:	e0843005 	add	r3, r4, r5
    916c:	e3530008 	cmp	r3, #8	; 0x8
    9170:	a3a03008 	movge	r3, #8	; 0x8
    9174:	e6ef6073 	uxtb	r6, r3
    9178:	e3510000 	cmp	r1, #0	; 0x0
    917c:	e1a0700d 	mov	r7, sp
    9180:	059a8000 	ldreq	r8, [sl]
    9184:	00667084 	rsbeq	r7, r6, r4, lsl #1
    9188:	0a000008 	beq	91b0 <spi_exchange+0x84>
    918c:	e0654006 	rsb	r4, r5, r6
    9190:	e0870005 	add	r0, r7, r5
    9194:	e1a02004 	mov	r2, r4
    9198:	ebffe00a 	bl	11c8 <memcpy>
    919c:	e99a000c 	ldmib	sl, {r2, r3}
    91a0:	e0855003 	add	r5, r5, r3
    91a4:	e0828004 	add	r8, r2, r4
    91a8:	e3a07000 	mov	r7, #0	; 0x0
    91ac:	e0664005 	rsb	r4, r6, r5
    91b0:	e3a02000 	mov	r2, #0	; 0x0
    91b4:	e1a0000d 	mov	r0, sp
    91b8:	e59f126c 	ldr	r1, [pc, #620]	; 942c <spi_exchange+0x300>
    91bc:	ea000002 	b	91cc <spi_exchange+0xa0>
    91c0:	e7d03002 	ldrb	r3, [r0, r2]
    91c4:	e2822001 	add	r2, r2, #1	; 0x1
    91c8:	e5813008 	str	r3, [r1, #8]
    91cc:	e1520006 	cmp	r2, r6
    91d0:	bafffffa 	blt	91c0 <spi_exchange+0x94>
    91d4:	e59f6250 	ldr	r6, [pc, #592]	; 942c <spi_exchange+0x300>
    91d8:	e3a0b005 	mov	fp, #5	; 0x5
    91dc:	e5969018 	ldr	r9, [r6, #24]
    91e0:	e3190004 	tst	r9, #4	; 0x4
    91e4:	0a000030 	beq	92ac <spi_exchange+0x180>
    91e8:	e3550003 	cmp	r5, #3	; 0x3
    91ec:	da000005 	ble	9208 <spi_exchange+0xdc>
    91f0:	e5963008 	ldr	r3, [r6, #8]
    91f4:	e2455004 	sub	r5, r5, #4	; 0x4
    91f8:	e5963008 	ldr	r3, [r6, #8]
    91fc:	e5963008 	ldr	r3, [r6, #8]
    9200:	e5963008 	ldr	r3, [r6, #8]
    9204:	ea000028 	b	92ac <spi_exchange+0x180>
    9208:	e3550000 	cmp	r5, #0	; 0x0
    920c:	0a000003 	beq	9220 <spi_exchange+0xf4>
    9210:	e2555001 	subs	r5, r5, #1	; 0x1
    9214:	e5963008 	ldr	r3, [r6, #8]
    9218:	1afffffc 	bne	9210 <spi_exchange+0xe4>
    921c:	ea000022 	b	92ac <spi_exchange+0x180>
    9220:	e59a3000 	ldr	r3, [sl]
    9224:	e3530000 	cmp	r3, #0	; 0x0
    9228:	1a000004 	bne	9240 <spi_exchange+0x114>
    922c:	e59f01fc 	ldr	r0, [pc, #508]	; 9430 <spi_exchange+0x304>
    9230:	e59f31fc 	ldr	r3, [pc, #508]	; 9434 <spi_exchange+0x308>
    9234:	e1a0e00f 	mov	lr, pc
    9238:	e593f000 	ldr	pc, [r3]
    923c:	ea00001a 	b	92ac <spi_exchange+0x180>
    9240:	e3540003 	cmp	r4, #3	; 0x3
    9244:	da00000e 	ble	9284 <spi_exchange+0x158>
    9248:	e5962008 	ldr	r2, [r6, #8]
    924c:	e1a03008 	mov	r3, r8
    9250:	e4c32001 	strb	r2, [r3], #1
    9254:	e5962008 	ldr	r2, [r6, #8]
    9258:	e2831001 	add	r1, r3, #1	; 0x1
    925c:	e5c82001 	strb	r2, [r8, #1]
    9260:	e5962008 	ldr	r2, [r6, #8]
    9264:	e2544004 	subs	r4, r4, #4	; 0x4
    9268:	e5c32001 	strb	r2, [r3, #1]
    926c:	e5963008 	ldr	r3, [r6, #8]
    9270:	e2812001 	add	r2, r1, #1	; 0x1
    9274:	e5c13001 	strb	r3, [r1, #1]
    9278:	0a00005e 	beq	93f8 <spi_exchange+0x2cc>
    927c:	e2828001 	add	r8, r2, #1	; 0x1
    9280:	ea000009 	b	92ac <spi_exchange+0x180>
    9284:	e3540000 	cmp	r4, #0	; 0x0
    9288:	0a00005a 	beq	93f8 <spi_exchange+0x2cc>
    928c:	e59f2198 	ldr	r2, [pc, #408]	; 942c <spi_exchange+0x300>
    9290:	e1a01005 	mov	r1, r5
    9294:	e5923008 	ldr	r3, [r2, #8]
    9298:	e7c83001 	strb	r3, [r8, r1]
    929c:	e2811001 	add	r1, r1, #1	; 0x1
    92a0:	e1510004 	cmp	r1, r4
    92a4:	1afffffa 	bne	9294 <spi_exchange+0x168>
    92a8:	ea000052 	b	93f8 <spi_exchange+0x2cc>
    92ac:	e3190008 	tst	r9, #8	; 0x8
    92b0:	0a00002a 	beq	9360 <spi_exchange+0x234>
    92b4:	e59a3004 	ldr	r3, [sl, #4]
    92b8:	e3530000 	cmp	r3, #0	; 0x0
    92bc:	0a000019 	beq	9328 <spi_exchange+0x1fc>
    92c0:	e3540003 	cmp	r4, #3	; 0x3
    92c4:	da00000d 	ble	9300 <spi_exchange+0x1d4>
    92c8:	e1a03008 	mov	r3, r8
    92cc:	e4d32001 	ldrb	r2, [r3], #1
    92d0:	e2544004 	subs	r4, r4, #4	; 0x4
    92d4:	e5862008 	str	r2, [r6, #8]
    92d8:	e5d82001 	ldrb	r2, [r8, #1]
    92dc:	e2831001 	add	r1, r3, #1	; 0x1
    92e0:	e5862008 	str	r2, [r6, #8]
    92e4:	e5d33001 	ldrb	r3, [r3, #1]
    92e8:	e2818002 	add	r8, r1, #2	; 0x2
    92ec:	e5863008 	str	r3, [r6, #8]
    92f0:	e5d13001 	ldrb	r3, [r1, #1]
    92f4:	e5863008 	str	r3, [r6, #8]
    92f8:	1a000018 	bne	9360 <spi_exchange+0x234>
    92fc:	ea00003d 	b	93f8 <spi_exchange+0x2cc>
    9300:	e3540000 	cmp	r4, #0	; 0x0
    9304:	0a00003b 	beq	93f8 <spi_exchange+0x2cc>
    9308:	e59f111c 	ldr	r1, [pc, #284]	; 942c <spi_exchange+0x300>
    930c:	e3a02000 	mov	r2, #0	; 0x0
    9310:	e7d83002 	ldrb	r3, [r8, r2]
    9314:	e2822001 	add	r2, r2, #1	; 0x1
    9318:	e1520004 	cmp	r2, r4
    931c:	e5813008 	str	r3, [r1, #8]
    9320:	1afffffa 	bne	9310 <spi_exchange+0x1e4>
    9324:	ea000033 	b	93f8 <spi_exchange+0x2cc>
    9328:	e3570003 	cmp	r7, #3	; 0x3
    932c:	da000005 	ble	9348 <spi_exchange+0x21c>
    9330:	e586b008 	str	fp, [r6, #8]
    9334:	e2477004 	sub	r7, r7, #4	; 0x4
    9338:	e586b008 	str	fp, [r6, #8]
    933c:	e586b008 	str	fp, [r6, #8]
    9340:	e586b008 	str	fp, [r6, #8]
    9344:	ea000005 	b	9360 <spi_exchange+0x234>
    9348:	e3570000 	cmp	r7, #0	; 0x0
    934c:	0a000003 	beq	9360 <spi_exchange+0x234>
    9350:	e3a03005 	mov	r3, #5	; 0x5
    9354:	e2577001 	subs	r7, r7, #1	; 0x1
    9358:	e5863008 	str	r3, [r6, #8]
    935c:	1afffffb 	bne	9350 <spi_exchange+0x224>
    9360:	e3190002 	tst	r9, #2	; 0x2
    9364:	0affff9c 	beq	91dc <spi_exchange+0xb0>
    9368:	ea000009 	b	9394 <spi_exchange+0x268>
    936c:	e596300c 	ldr	r3, [r6, #12]
    9370:	e2455001 	sub	r5, r5, #1	; 0x1
    9374:	e3130004 	tst	r3, #4	; 0x4
    9378:	15963008 	ldrne	r3, [r6, #8]
    937c:	1a000006 	bne	939c <spi_exchange+0x270>
    9380:	e59f00b0 	ldr	r0, [pc, #176]	; 9438 <spi_exchange+0x30c>
    9384:	e1a01005 	mov	r1, r5
    9388:	e1a0e00f 	mov	lr, pc
    938c:	e597f000 	ldr	pc, [r7]
    9390:	ea000001 	b	939c <spi_exchange+0x270>
    9394:	e59f6090 	ldr	r6, [pc, #144]	; 942c <spi_exchange+0x300>
    9398:	e59f7094 	ldr	r7, [pc, #148]	; 9434 <spi_exchange+0x308>
    939c:	e3550000 	cmp	r5, #0	; 0x0
    93a0:	1afffff1 	bne	936c <spi_exchange+0x240>
    93a4:	ea00000a 	b	93d4 <spi_exchange+0x2a8>
    93a8:	e595300c 	ldr	r3, [r5, #12]
    93ac:	e2444001 	sub	r4, r4, #1	; 0x1
    93b0:	e3130004 	tst	r3, #4	; 0x4
    93b4:	15953008 	ldrne	r3, [r5, #8]
    93b8:	14c83001 	strbne	r3, [r8], #1
    93bc:	1a000006 	bne	93dc <spi_exchange+0x2b0>
    93c0:	e59f0074 	ldr	r0, [pc, #116]	; 943c <spi_exchange+0x310>
    93c4:	e1a01004 	mov	r1, r4
    93c8:	e1a0e00f 	mov	lr, pc
    93cc:	e596f000 	ldr	pc, [r6]
    93d0:	ea000001 	b	93dc <spi_exchange+0x2b0>
    93d4:	e59f5050 	ldr	r5, [pc, #80]	; 942c <spi_exchange+0x300>
    93d8:	e59f6054 	ldr	r6, [pc, #84]	; 9434 <spi_exchange+0x308>
    93dc:	e59a3000 	ldr	r3, [sl]
    93e0:	e3530000 	cmp	r3, #0	; 0x0
    93e4:	13540000 	cmpne	r4, #0	; 0x0
    93e8:	1affffee 	bne	93a8 <spi_exchange+0x27c>
    93ec:	e59f3038 	ldr	r3, [pc, #56]	; 942c <spi_exchange+0x300>
    93f0:	e3a02003 	mov	r2, #3	; 0x3
    93f4:	e5832020 	str	r2, [r3, #32]
    93f8:	e59f202c 	ldr	r2, [pc, #44]	; 942c <spi_exchange+0x300>
    93fc:	e592300c 	ldr	r3, [r2, #12]
    9400:	e3130010 	tst	r3, #16	; 0x10
    9404:	1afffffc 	bne	93fc <spi_exchange+0x2d0>
    9408:	ea000001 	b	9414 <spi_exchange+0x2e8>
    940c:	e5923008 	ldr	r3, [r2, #8]
    9410:	ea000000 	b	9418 <spi_exchange+0x2ec>
    9414:	e59f2010 	ldr	r2, [pc, #16]	; 942c <spi_exchange+0x300>
    9418:	e592300c 	ldr	r3, [r2, #12]
    941c:	e3130004 	tst	r3, #4	; 0x4
    9420:	1afffff9 	bne	940c <spi_exchange+0x2e0>
    9424:	e28dd00c 	add	sp, sp, #12	; 0xc
    9428:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    942c:	20500000 	.word	0x20500000
    9430:	00011430 	.word	0x00011430
    9434:	3c00d68c 	.word	0x3c00d68c
    9438:	00011447 	.word	0x00011447
    943c:	0001145f 	.word	0x0001145f

00009440 <flash_vs_read>:
    9440:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9444:	e24dd024 	sub	sp, sp, #36	; 0x24
    9448:	e28d4018 	add	r4, sp, #24	; 0x18
    944c:	e2841003 	add	r1, r4, #3	; 0x3
    9450:	e59da040 	ldr	sl, [sp, #64]
    9454:	e58d1004 	str	r1, [sp, #4]
    9458:	e3a01005 	mov	r1, #5	; 0x5
    945c:	e3a0c000 	mov	ip, #0	; 0x0
    9460:	e58d1008 	str	r1, [sp, #8]
    9464:	e2811006 	add	r1, r1, #6	; 0x6
    9468:	e58dc018 	str	ip, [sp, #24]
    946c:	e2845004 	add	r5, r4, #4	; 0x4
    9470:	e58d000c 	str	r0, [sp, #12]
    9474:	e58dc010 	str	ip, [sp, #16]
    9478:	e5cd101b 	strb	r1, [sp, #27]
    947c:	e1a06002 	mov	r6, r2
    9480:	e1a07003 	mov	r7, r3
    9484:	e58dc000 	str	ip, [sp]
    9488:	e1a0400a 	mov	r4, sl
    948c:	e1a0800d 	mov	r8, sp
    9490:	ea00000c 	b	94c8 <flash_vs_read+0x88>
    9494:	e3540a01 	cmp	r4, #4096	; 0x1000
    9498:	b1a03004 	movlt	r3, r4
    949c:	a3a03a01 	movge	r3, #4096	; 0x1000
    94a0:	e5852000 	str	r2, [r5]
    94a4:	e58d3014 	str	r3, [sp, #20]
    94a8:	ebffff1f 	bl	912c <spi_exchange>
    94ac:	e59d2014 	ldr	r2, [sp, #20]
    94b0:	e59d300c 	ldr	r3, [sp, #12]
    94b4:	e0966002 	adds	r6, r6, r2
    94b8:	e0833002 	add	r3, r3, r2
    94bc:	e0a77fc2 	adc	r7, r7, r2, asr #31
    94c0:	e58d300c 	str	r3, [sp, #12]
    94c4:	e0624004 	rsb	r4, r2, r4
    94c8:	e1a02c06 	lsl	r2, r6, #24
    94cc:	e2061cff 	and	r1, r6, #65280	; 0xff00
    94d0:	e1822c26 	orr	r2, r2, r6, lsr #24
    94d4:	e1822401 	orr	r2, r2, r1, lsl #8
    94d8:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    94dc:	e1822423 	orr	r2, r2, r3, lsr #8
    94e0:	e3540000 	cmp	r4, #0	; 0x0
    94e4:	e1a02442 	asr	r2, r2, #8
    94e8:	e1a0000d 	mov	r0, sp
    94ec:	e3a01002 	mov	r1, #2	; 0x2
    94f0:	1affffe7 	bne	9494 <flash_vs_read+0x54>
    94f4:	e1a0000a 	mov	r0, sl
    94f8:	e28dd024 	add	sp, sp, #36	; 0x24
    94fc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00009500 <flash_poll>:
    9500:	e92d4030 	push	{r4, r5, lr}
    9504:	e24dd024 	sub	sp, sp, #36	; 0x24
    9508:	e28dc018 	add	ip, sp, #24	; 0x18
    950c:	e28c3001 	add	r3, ip, #1	; 0x1
    9510:	e3a02000 	mov	r2, #0	; 0x0
    9514:	e3a01001 	mov	r1, #1	; 0x1
    9518:	e58d300c 	str	r3, [sp, #12]
    951c:	e3a03005 	mov	r3, #5	; 0x5
    9520:	e58d2018 	str	r2, [sp, #24]
    9524:	e58d2010 	str	r2, [sp, #16]
    9528:	e58d1014 	str	r1, [sp, #20]
    952c:	e5cd3018 	strb	r3, [sp, #24]
    9530:	e1a04000 	mov	r4, r0
    9534:	e58d201c 	str	r2, [sp, #28]
    9538:	e88d1004 	stm	sp, {r2, ip}
    953c:	e58d1008 	str	r1, [sp, #8]
    9540:	e1a0500d 	mov	r5, sp
    9544:	e1a0000d 	mov	r0, sp
    9548:	e3a01002 	mov	r1, #2	; 0x2
    954c:	ebfffef6 	bl	912c <spi_exchange>
    9550:	e5dd3019 	ldrb	r3, [sp, #25]
    9554:	e3540000 	cmp	r4, #0	; 0x0
    9558:	11a030a3 	lsrne	r3, r3, #1
    955c:	12233001 	eorne	r3, r3, #1	; 0x1
    9560:	e2033001 	and	r3, r3, #1	; 0x1
    9564:	e3530000 	cmp	r3, #0	; 0x0
    9568:	1afffff5 	bne	9544 <flash_poll+0x44>
    956c:	e28dd024 	add	sp, sp, #36	; 0x24
    9570:	e8bd8030 	pop	{r4, r5, pc}

00009574 <flash_wren>:
    9574:	e92d4010 	push	{r4, lr}
    9578:	e24dd020 	sub	sp, sp, #32	; 0x20
    957c:	e3a04001 	mov	r4, #1	; 0x1
    9580:	e28dc018 	add	ip, sp, #24	; 0x18
    9584:	e08c3004 	add	r3, ip, r4
    9588:	e3a02000 	mov	r2, #0	; 0x0
    958c:	e58d3010 	str	r3, [sp, #16]
    9590:	e3a03006 	mov	r3, #6	; 0x6
    9594:	e58d2018 	str	r2, [sp, #24]
    9598:	e1a0000d 	mov	r0, sp
    959c:	e5cd3018 	strb	r3, [sp, #24]
    95a0:	e1a01004 	mov	r1, r4
    95a4:	e3a03004 	mov	r3, #4	; 0x4
    95a8:	e58d200c 	str	r2, [sp, #12]
    95ac:	e5cd3019 	strb	r3, [sp, #25]
    95b0:	e58d201c 	str	r2, [sp, #28]
    95b4:	e88d1004 	stm	sp, {r2, ip}
    95b8:	e58d4008 	str	r4, [sp, #8]
    95bc:	e58d4014 	str	r4, [sp, #20]
    95c0:	ebfffed9 	bl	912c <spi_exchange>
    95c4:	e1a00004 	mov	r0, r4
    95c8:	ebffffcc 	bl	9500 <flash_poll>
    95cc:	e28dd020 	add	sp, sp, #32	; 0x20
    95d0:	e8bd8010 	pop	{r4, pc}

000095d4 <flash_vs_erase>:
    95d4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    95d8:	e24dd034 	sub	sp, sp, #52	; 0x34
    95dc:	e28d4028 	add	r4, sp, #40	; 0x28
    95e0:	e3a06001 	mov	r6, #1	; 0x1
    95e4:	e59f109c 	ldr	r1, [pc, #156]	; 9688 <flash_vs_erase+0xb4>
    95e8:	e3a02008 	mov	r2, #8	; 0x8
    95ec:	e1a00004 	mov	r0, r4
    95f0:	ebffdef4 	bl	11c8 <memcpy>
    95f4:	e0843006 	add	r3, r4, r6
    95f8:	e2842003 	add	r2, r4, #3	; 0x3
    95fc:	e3a05000 	mov	r5, #0	; 0x0
    9600:	e28d7004 	add	r7, sp, #4	; 0x4
    9604:	e58d3008 	str	r3, [sp, #8]
    9608:	e3a03002 	mov	r3, #2	; 0x2
    960c:	e58d300c 	str	r3, [sp, #12]
    9610:	e58d2014 	str	r2, [sp, #20]
    9614:	e58d4020 	str	r4, [sp, #32]
    9618:	e58d5004 	str	r5, [sp, #4]
    961c:	e58d5010 	str	r5, [sp, #16]
    9620:	e58d6018 	str	r6, [sp, #24]
    9624:	e58d501c 	str	r5, [sp, #28]
    9628:	e58d6024 	str	r6, [sp, #36]
    962c:	ebffffd0 	bl	9574 <flash_wren>
    9630:	e1a01006 	mov	r1, r6
    9634:	e1a00007 	mov	r0, r7
    9638:	ebfffebb 	bl	912c <spi_exchange>
    963c:	e1a00005 	mov	r0, r5
    9640:	ebffffae 	bl	9500 <flash_poll>
    9644:	e2870018 	add	r0, r7, #24	; 0x18
    9648:	e1a01006 	mov	r1, r6
    964c:	ebfffeb6 	bl	912c <spi_exchange>
    9650:	e1a01006 	mov	r1, r6
    9654:	e1a00007 	mov	r0, r7
    9658:	ebfffeb3 	bl	912c <spi_exchange>
    965c:	e1a00005 	mov	r0, r5
    9660:	ebffffa6 	bl	9500 <flash_poll>
    9664:	ebffffc2 	bl	9574 <flash_wren>
    9668:	e1a01006 	mov	r1, r6
    966c:	e287000c 	add	r0, r7, #12	; 0xc
    9670:	ebfffead 	bl	912c <spi_exchange>
    9674:	e1a00005 	mov	r0, r5
    9678:	ebffffa0 	bl	9500 <flash_poll>
    967c:	e1a00005 	mov	r0, r5
    9680:	e28dd034 	add	sp, sp, #52	; 0x34
    9684:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9688:	0000f8f4 	.word	0x0000f8f4

0000968c <flash_vs_write>:
    968c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9690:	e3a01004 	mov	r1, #4	; 0x4
    9694:	e24dd020 	sub	sp, sp, #32	; 0x20
    9698:	e3a0c000 	mov	ip, #0	; 0x0
    969c:	e59d9040 	ldr	r9, [sp, #64]
    96a0:	e58d1008 	str	r1, [sp, #8]
    96a4:	e2811002 	add	r1, r1, #2	; 0x2
    96a8:	e58dc018 	str	ip, [sp, #24]
    96ac:	e28d701c 	add	r7, sp, #28	; 0x1c
    96b0:	e5cd1018 	strb	r1, [sp, #24]
    96b4:	e3a01002 	mov	r1, #2	; 0x2
    96b8:	e58dc00c 	str	ip, [sp, #12]
    96bc:	e58d0010 	str	r0, [sp, #16]
    96c0:	e5cd101b 	strb	r1, [sp, #27]
    96c4:	e1a04002 	mov	r4, r2
    96c8:	e1a05003 	mov	r5, r3
    96cc:	e58dc000 	str	ip, [sp]
    96d0:	e58d7004 	str	r7, [sp, #4]
    96d4:	e1a06009 	mov	r6, r9
    96d8:	e59fa080 	ldr	sl, [pc, #128]	; 9760 <flash_vs_write+0xd4>
    96dc:	e1a0800d 	mov	r8, sp
    96e0:	ea000011 	b	972c <flash_vs_write+0xa0>
    96e4:	e59a3000 	ldr	r3, [sl]
    96e8:	e5872000 	str	r2, [r7]
    96ec:	e1560003 	cmp	r6, r3
    96f0:	d58d6014 	strle	r6, [sp, #20]
    96f4:	c58d3014 	strgt	r3, [sp, #20]
    96f8:	ebffff9d 	bl	9574 <flash_wren>
    96fc:	e1a0000d 	mov	r0, sp
    9700:	e3a01002 	mov	r1, #2	; 0x2
    9704:	ebfffe88 	bl	912c <spi_exchange>
    9708:	e3a00000 	mov	r0, #0	; 0x0
    970c:	ebffff7b 	bl	9500 <flash_poll>
    9710:	e59d2014 	ldr	r2, [sp, #20]
    9714:	e59d3010 	ldr	r3, [sp, #16]
    9718:	e0944002 	adds	r4, r4, r2
    971c:	e0833002 	add	r3, r3, r2
    9720:	e0a55fc2 	adc	r5, r5, r2, asr #31
    9724:	e58d3010 	str	r3, [sp, #16]
    9728:	e0626006 	rsb	r6, r2, r6
    972c:	e1a02c04 	lsl	r2, r4, #24
    9730:	e1822c24 	orr	r2, r2, r4, lsr #24
    9734:	e2041cff 	and	r1, r4, #65280	; 0xff00
    9738:	e1822401 	orr	r2, r2, r1, lsl #8
    973c:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    9740:	e1822423 	orr	r2, r2, r3, lsr #8
    9744:	e3c220ff 	bic	r2, r2, #255	; 0xff
    9748:	e3560000 	cmp	r6, #0	; 0x0
    974c:	e3822002 	orr	r2, r2, #2	; 0x2
    9750:	1affffe3 	bne	96e4 <flash_vs_write+0x58>
    9754:	e1a00009 	mov	r0, r9
    9758:	e28dd020 	add	sp, sp, #32	; 0x20
    975c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9760:	3c00d0c0 	.word	0x3c00d0c0

00009764 <spi_init_pads>:
    9764:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9768:	e3a0000b 	mov	r0, #11	; 0xb
    976c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9770:	ebffdc20 	bl	7f8 <pads_init_module>
    9774:	e3a00000 	mov	r0, #0	; 0x0
    9778:	e28dd004 	add	sp, sp, #4	; 0x4
    977c:	e8bd8000 	pop	{pc}

00009780 <spi_init_clk>:
    9780:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9784:	e59f00e8 	ldr	r0, [pc, #232]	; 9874 <spi_init_clk+0xf4>
    9788:	e24dd00c 	sub	sp, sp, #12	; 0xc
    978c:	eb00056b 	bl	ad40 <module_get_clock>
    9790:	e3a05002 	mov	r5, #2	; 0x2
    9794:	e59fa0dc 	ldr	sl, [pc, #220]	; 9878 <spi_init_clk+0xf8>
    9798:	e1a08000 	mov	r8, r0
    979c:	ea00000c 	b	97d4 <spi_init_clk+0x54>
    97a0:	e1a06004 	mov	r6, r4
    97a4:	e1a01007 	mov	r1, r7
    97a8:	e1a00008 	mov	r0, r8
    97ac:	eb001313 	bl	e400 <__aeabi_idiv>
    97b0:	e150000a 	cmp	r0, sl
    97b4:	e0877005 	add	r7, r7, r5
    97b8:	e2864001 	add	r4, r6, #1	; 0x1
    97bc:	da000007 	ble	97e0 <spi_init_clk+0x60>
    97c0:	e3540c01 	cmp	r4, #256	; 0x100
    97c4:	1afffff5 	bne	97a0 <spi_init_clk+0x20>
    97c8:	e2855002 	add	r5, r5, #2	; 0x2
    97cc:	e3550c01 	cmp	r5, #256	; 0x100
    97d0:	0a000003 	beq	97e4 <spi_init_clk+0x64>
    97d4:	e1a07005 	mov	r7, r5
    97d8:	e3a06000 	mov	r6, #0	; 0x0
    97dc:	eafffff0 	b	97a4 <spi_init_clk+0x24>
    97e0:	e1a04006 	mov	r4, r6
    97e4:	e35400ff 	cmp	r4, #255	; 0xff
    97e8:	d35500fe 	cmple	r5, #254	; 0xfe
    97ec:	da000007 	ble	9810 <spi_init_clk+0x90>
    97f0:	e1a01005 	mov	r1, r5
    97f4:	e1a02004 	mov	r2, r4
    97f8:	e59f007c 	ldr	r0, [pc, #124]	; 987c <spi_init_clk+0xfc>
    97fc:	e59f307c 	ldr	r3, [pc, #124]	; 9880 <spi_init_clk+0x100>
    9800:	e1a0e00f 	mov	lr, pc
    9804:	e593f000 	ldr	pc, [r3]
    9808:	e3a05002 	mov	r5, #2	; 0x2
    980c:	e3a04027 	mov	r4, #39	; 0x27
    9810:	e2841001 	add	r1, r4, #1	; 0x1
    9814:	e0010195 	mul	r1, r5, r1
    9818:	e1a00008 	mov	r0, r8
    981c:	eb0012f7 	bl	e400 <__aeabi_idiv>
    9820:	e1a01008 	mov	r1, r8
    9824:	e58d0000 	str	r0, [sp]
    9828:	e1a02005 	mov	r2, r5
    982c:	e1a03004 	mov	r3, r4
    9830:	e59f004c 	ldr	r0, [pc, #76]	; 9884 <spi_init_clk+0x104>
    9834:	e59fc044 	ldr	ip, [pc, #68]	; 9880 <spi_init_clk+0x100>
    9838:	e1a0e00f 	mov	lr, pc
    983c:	e59cf000 	ldr	pc, [ip]
    9840:	e59f2040 	ldr	r2, [pc, #64]	; 9888 <spi_init_clk+0x108>
    9844:	e3a03000 	mov	r3, #0	; 0x0
    9848:	e1a01404 	lsl	r1, r4, #8
    984c:	e5823004 	str	r3, [r2, #4]
    9850:	e38110c7 	orr	r1, r1, #199	; 0xc7
    9854:	e5823014 	str	r3, [r2, #20]
    9858:	e1a00003 	mov	r0, r3
    985c:	e283300a 	add	r3, r3, #10	; 0xa
    9860:	e5825010 	str	r5, [r2, #16]
    9864:	e5821000 	str	r1, [r2]
    9868:	e5823004 	str	r3, [r2, #4]
    986c:	e28dd00c 	add	sp, sp, #12	; 0xc
    9870:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    9874:	21e0a360 	.word	0x21e0a360
    9878:	00551928 	.word	0x00551928
    987c:	00011477 	.word	0x00011477
    9880:	3c00d68c 	.word	0x3c00d68c
    9884:	000114a5 	.word	0x000114a5
    9888:	20500000 	.word	0x20500000

0000988c <flash_vs_reset>:
    988c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9890:	e59f0044 	ldr	r0, [pc, #68]	; 98dc <flash_vs_reset+0x50>
    9894:	e24dd004 	sub	sp, sp, #4	; 0x4
    9898:	ebffdb9a 	bl	708 <module_enable>
    989c:	ebffffb7 	bl	9780 <spi_init_clk>
    98a0:	e3a0000b 	mov	r0, #11	; 0xb
    98a4:	ebffdbd3 	bl	7f8 <pads_init_module>
    98a8:	e59f2030 	ldr	r2, [pc, #48]	; 98e0 <flash_vs_reset+0x54>
    98ac:	e592300c 	ldr	r3, [r2, #12]
    98b0:	e3130010 	tst	r3, #16	; 0x10
    98b4:	1afffffc 	bne	98ac <flash_vs_reset+0x20>
    98b8:	ea000001 	b	98c4 <flash_vs_reset+0x38>
    98bc:	e5923008 	ldr	r3, [r2, #8]
    98c0:	ea000000 	b	98c8 <flash_vs_reset+0x3c>
    98c4:	e59f2014 	ldr	r2, [pc, #20]	; 98e0 <flash_vs_reset+0x54>
    98c8:	e592300c 	ldr	r3, [r2, #12]
    98cc:	e2130004 	ands	r0, r3, #4	; 0x4
    98d0:	1afffff9 	bne	98bc <flash_vs_reset+0x30>
    98d4:	e28dd004 	add	sp, sp, #4	; 0x4
    98d8:	e8bd8000 	pop	{pc}
    98dc:	21e11400 	.word	0x21e11400
    98e0:	20500000 	.word	0x20500000

000098e4 <ram_addr2offs>:
    98e4:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    98e8:	93a02101 	movls	r2, #1073741824	; 0x40000000
    98ec:	e3a01000 	mov	r1, #0	; 0x0
    98f0:	83a02103 	movhi	r2, #-1073741824	; 0xc0000000
    98f4:	93a03000 	movls	r3, #0	; 0x0
    98f8:	83e03000 	mvnhi	r3, #0	; 0x0
    98fc:	e0922000 	adds	r2, r2, r0
    9900:	e0a33001 	adc	r3, r3, r1
    9904:	e1a01003 	mov	r1, r3
    9908:	e1a00002 	mov	r0, r2
    990c:	e12fff1e 	bx	lr

00009910 <ram_offs2addr>:
    9910:	e3510000 	cmp	r1, #0	; 0x0
    9914:	ca000002 	bgt	9924 <ram_offs2addr+0x14>
    9918:	1a000003 	bne	992c <ram_offs2addr+0x1c>
    991c:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    9920:	9a000001 	bls	992c <ram_offs2addr+0x1c>
    9924:	e2800103 	add	r0, r0, #-1073741824	; 0xc0000000
    9928:	e12fff1e 	bx	lr
    992c:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    9930:	e12fff1e 	bx	lr

00009934 <ram_vs_align>:
    9934:	e3a00c02 	mov	r0, #512	; 0x200
    9938:	e12fff1e 	bx	lr

0000993c <ram_vs_reset>:
    993c:	e3a00000 	mov	r0, #0	; 0x0
    9940:	e12fff1e 	bx	lr

00009944 <ram_vs_write>:
    9944:	e92d4010 	push	{r4, lr}
    9948:	e3530000 	cmp	r3, #0	; 0x0
    994c:	e1a01000 	mov	r1, r0
    9950:	e59d4008 	ldr	r4, [sp, #8]
    9954:	ca000002 	bgt	9964 <ram_vs_write+0x20>
    9958:	1a000003 	bne	996c <ram_vs_write+0x28>
    995c:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    9960:	9a000001 	bls	996c <ram_vs_write+0x28>
    9964:	e2820103 	add	r0, r2, #-1073741824	; 0xc0000000
    9968:	ea000000 	b	9970 <ram_vs_write+0x2c>
    996c:	e2820101 	add	r0, r2, #1073741824	; 0x40000000
    9970:	e1a02004 	mov	r2, r4
    9974:	ebffde13 	bl	11c8 <memcpy>
    9978:	e1a00004 	mov	r0, r4
    997c:	e8bd8010 	pop	{r4, pc}

00009980 <ram_vs_read>:
    9980:	e92d4010 	push	{r4, lr}
    9984:	e3530000 	cmp	r3, #0	; 0x0
    9988:	e59d4008 	ldr	r4, [sp, #8]
    998c:	ca000002 	bgt	999c <ram_vs_read+0x1c>
    9990:	1a000003 	bne	99a4 <ram_vs_read+0x24>
    9994:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    9998:	9a000001 	bls	99a4 <ram_vs_read+0x24>
    999c:	e2821103 	add	r1, r2, #-1073741824	; 0xc0000000
    99a0:	ea000000 	b	99a8 <ram_vs_read+0x28>
    99a4:	e2821101 	add	r1, r2, #1073741824	; 0x40000000
    99a8:	e1a02004 	mov	r2, r4
    99ac:	ebffde05 	bl	11c8 <memcpy>
    99b0:	e1a00004 	mov	r0, r4
    99b4:	e8bd8010 	pop	{r4, pc}

000099b8 <init_led>:
    99b8:	e3a0c000 	mov	ip, #0	; 0x0
    99bc:	e59f3010 	ldr	r3, [pc, #16]	; 99d4 <init_led+0x1c>
    99c0:	e59f0010 	ldr	r0, [pc, #16]	; 99d8 <init_led+0x20>
    99c4:	e1a0200c 	mov	r2, ip
    99c8:	e3a01002 	mov	r1, #2	; 0x2
    99cc:	e583c000 	str	ip, [r3]
    99d0:	eaffdc8d 	b	c0c <pads_chmod>
    99d4:	3c00d6f4 	.word	0x3c00d6f4
    99d8:	002e002d 	.word	0x002e002d

000099dc <led_off>:
    99dc:	e3a02000 	mov	r2, #0	; 0x0
    99e0:	e59f300c 	ldr	r3, [pc, #12]	; 99f4 <led_off+0x18>
    99e4:	e1a01002 	mov	r1, r2
    99e8:	e3a0002e 	mov	r0, #46	; 0x2e
    99ec:	e5832000 	str	r2, [r3]
    99f0:	eaffdd04 	b	e08 <pads_setpin>
    99f4:	3c00d6f4 	.word	0x3c00d6f4

000099f8 <led_on>:
    99f8:	e3a02001 	mov	r2, #1	; 0x1
    99fc:	e59f300c 	ldr	r3, [pc, #12]	; 9a10 <led_on+0x18>
    9a00:	e1a01002 	mov	r1, r2
    9a04:	e3a0002e 	mov	r0, #46	; 0x2e
    9a08:	e5832000 	str	r2, [r3]
    9a0c:	eaffdcfd 	b	e08 <pads_setpin>
    9a10:	3c00d6f4 	.word	0x3c00d6f4

00009a14 <led_pulse>:
    9a14:	e92d4010 	push	{r4, lr}
    9a18:	e1a04000 	mov	r4, r0
    9a1c:	ebfffff5 	bl	99f8 <led_on>
    9a20:	e3a00005 	mov	r0, #5	; 0x5
    9a24:	ea000001 	b	9a30 <led_pulse+0x1c>
    9a28:	ebfffff2 	bl	99f8 <led_on>
    9a2c:	e3a00002 	mov	r0, #2	; 0x2
    9a30:	ebfffb30 	bl	86f8 <udelay>
    9a34:	ebffffe8 	bl	99dc <led_off>
    9a38:	e3a00002 	mov	r0, #2	; 0x2
    9a3c:	ebfffb2d 	bl	86f8 <udelay>
    9a40:	e3540000 	cmp	r4, #0	; 0x0
    9a44:	e2444001 	sub	r4, r4, #1	; 0x1
    9a48:	cafffff6 	bgt	9a28 <led_pulse+0x14>
    9a4c:	e8bd8010 	pop	{r4, pc}

00009a50 <led_ctrl>:
    9a50:	e3500000 	cmp	r0, #0	; 0x0
    9a54:	0a000000 	beq	9a5c <led_ctrl+0xc>
    9a58:	eaffffe6 	b	99f8 <led_on>
    9a5c:	eaffffde 	b	99dc <led_off>

00009a60 <led_toggle>:
    9a60:	e59f300c 	ldr	r3, [pc, #12]	; 9a74 <led_toggle+0x14>
    9a64:	e5930000 	ldr	r0, [r3]
    9a68:	e2700001 	rsbs	r0, r0, #1	; 0x1
    9a6c:	33a00000 	movcc	r0, #0	; 0x0
    9a70:	eafffff6 	b	9a50 <led_ctrl>
    9a74:	3c00d6f4 	.word	0x3c00d6f4

00009a78 <udc_get_cable_state>:
    9a78:	e3a00001 	mov	r0, #1	; 0x1
    9a7c:	e12fff1e 	bx	lr

00009a80 <udc_disconnect>:
    9a80:	e59f103c 	ldr	r1, [pc, #60]	; 9ac4 <udc_disconnect+0x44>
    9a84:	e59f303c 	ldr	r3, [pc, #60]	; 9ac8 <udc_disconnect+0x48>
    9a88:	e5912010 	ldr	r2, [r1, #16]
    9a8c:	e3a00000 	mov	r0, #0	; 0x0
    9a90:	e3822001 	orr	r2, r2, #1	; 0x1
    9a94:	e92d4010 	push	{r4, lr}
    9a98:	e5c30000 	strb	r0, [r3]
    9a9c:	e1a04001 	mov	r4, r1
    9aa0:	e5812010 	str	r2, [r1, #16]
    9aa4:	ea000000 	b	9aac <udc_disconnect+0x2c>
    9aa8:	ebfffb12 	bl	86f8 <udelay>
    9aac:	e5943010 	ldr	r3, [r4, #16]
    9ab0:	e3a0000a 	mov	r0, #10	; 0xa
    9ab4:	e2133001 	ands	r3, r3, #1	; 0x1
    9ab8:	1afffffa 	bne	9aa8 <udc_disconnect+0x28>
    9abc:	e1a00003 	mov	r0, r3
    9ac0:	e8bd8010 	pop	{r4, pc}
    9ac4:	27100000 	.word	0x27100000
    9ac8:	3c00d1e0 	.word	0x3c00d1e0

00009acc <udc_event_handle>:
    9acc:	e59f2768 	ldr	r2, [pc, #1896]	; a23c <udc_event_handle+0x770>
    9ad0:	e59f3768 	ldr	r3, [pc, #1896]	; a240 <udc_event_handle+0x774>
    9ad4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9ad8:	e5934014 	ldr	r4, [r3, #20]
    9adc:	e5923000 	ldr	r3, [r2]
    9ae0:	e59f0758 	ldr	r0, [pc, #1880]	; a240 <udc_event_handle+0x774>
    9ae4:	e1530004 	cmp	r3, r4
    9ae8:	e204ca03 	and	ip, r4, #12288	; 0x3000
    9aec:	15824000 	strne	r4, [r2]
    9af0:	e2043b03 	and	r3, r4, #3072	; 0xc00
    9af4:	e35c0a01 	cmp	ip, #4096	; 0x1000
    9af8:	e24dd008 	sub	sp, sp, #8	; 0x8
    9afc:	e5803014 	str	r3, [r0, #20]
    9b00:	0a000002 	beq	9b10 <udc_event_handle+0x44>
    9b04:	e35c0a02 	cmp	ip, #8192	; 0x2000
    9b08:	1a000034 	bne	9be0 <udc_event_handle+0x114>
    9b0c:	ea000025 	b	9ba8 <udc_event_handle+0xdc>
    9b10:	e3a03302 	mov	r3, #134217728	; 0x8000000
    9b14:	e5803b00 	str	r3, [r0, #2816]
    9b18:	e5803b40 	str	r3, [r0, #2880]
    9b1c:	e59f3720 	ldr	r3, [pc, #1824]	; a244 <udc_event_handle+0x778>
    9b20:	e3a02009 	mov	r2, #9	; 0x9
    9b24:	e580381c 	str	r3, [r0, #2076]
    9b28:	e3a03b02 	mov	r3, #2048	; 0x800
    9b2c:	e5802814 	str	r2, [r0, #2068]
    9b30:	e5802810 	str	r2, [r0, #2064]
    9b34:	e5803024 	str	r3, [r0, #36]
    9b38:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    9b3c:	e5803028 	str	r3, [r0, #40]
    9b40:	e2833b01 	add	r3, r3, #1024	; 0x400
    9b44:	e5803104 	str	r3, [r0, #260]
    9b48:	e59f36f8 	ldr	r3, [pc, #1784]	; a248 <udc_event_handle+0x77c>
    9b4c:	e1a05000 	mov	r5, r0
    9b50:	e580305c 	str	r3, [r0, #92]
    9b54:	e5903010 	ldr	r3, [r0, #16]
    9b58:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    9b5c:	e3833e43 	orr	r3, r3, #1072	; 0x430
    9b60:	e5803010 	str	r3, [r0, #16]
    9b64:	ea000000 	b	9b6c <udc_event_handle+0xa0>
    9b68:	ebfffae2 	bl	86f8 <udelay>
    9b6c:	e5953010 	ldr	r3, [r5, #16]
    9b70:	e3a00001 	mov	r0, #1	; 0x1
    9b74:	e3130030 	tst	r3, #48	; 0x30
    9b78:	e59f16c0 	ldr	r1, [pc, #1728]	; a240 <udc_event_handle+0x774>
    9b7c:	1afffff9 	bne	9b68 <udc_event_handle+0x9c>
    9b80:	e59f36c4 	ldr	r3, [pc, #1732]	; a24c <udc_event_handle+0x780>
    9b84:	e2800009 	add	r0, r0, #9	; 0x9
    9b88:	e5932024 	ldr	r2, [r3, #36]
    9b8c:	e59f36bc 	ldr	r3, [pc, #1724]	; a250 <udc_event_handle+0x784>
    9b90:	e5813b10 	str	r3, [r1, #2832]
    9b94:	e3a03a01 	mov	r3, #4096	; 0x1000
    9b98:	e5812b14 	str	r2, [r1, #2836]
    9b9c:	e5813014 	str	r3, [r1, #20]
    9ba0:	ebfffad4 	bl	86f8 <udelay>
    9ba4:	ea00000d 	b	9be0 <udc_event_handle+0x114>
    9ba8:	e5902808 	ldr	r2, [r0, #2056]
    9bac:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9bb0:	e5803b00 	str	r3, [r0, #2816]
    9bb4:	e1a020a2 	lsr	r2, r2, #1
    9bb8:	e59f368c 	ldr	r3, [pc, #1676]	; a24c <udc_event_handle+0x780>
    9bbc:	e5901018 	ldr	r1, [r0, #24]
    9bc0:	e2022003 	and	r2, r2, #3	; 0x3
    9bc4:	e5c3201c 	strb	r2, [r3, #28]
    9bc8:	e59f3684 	ldr	r3, [pc, #1668]	; a254 <udc_event_handle+0x788>
    9bcc:	e3811008 	orr	r1, r1, #8	; 0x8
    9bd0:	e3a02001 	mov	r2, #1	; 0x1
    9bd4:	e5801018 	str	r1, [r0, #24]
    9bd8:	e5832000 	str	r2, [r3]
    9bdc:	e580c014 	str	ip, [r0, #20]
    9be0:	e59f3658 	ldr	r3, [pc, #1624]	; a240 <udc_event_handle+0x774>
    9be4:	e3140701 	tst	r4, #262144	; 0x40000
    9be8:	e5939818 	ldr	r9, [r3, #2072]
    9bec:	0a000051 	beq	9d38 <udc_event_handle+0x26c>
    9bf0:	e3190001 	tst	r9, #1	; 0x1
    9bf4:	0a000042 	beq	9d04 <udc_event_handle+0x238>
    9bf8:	e5935908 	ldr	r5, [r3, #2312]
    9bfc:	e3150001 	tst	r5, #1	; 0x1
    9c00:	0a00003b 	beq	9cf4 <udc_event_handle+0x228>
    9c04:	e59f3640 	ldr	r3, [pc, #1600]	; a24c <udc_event_handle+0x780>
    9c08:	e5933020 	ldr	r3, [r3, #32]
    9c0c:	e2433002 	sub	r3, r3, #2	; 0x2
    9c10:	e3530003 	cmp	r3, #3	; 0x3
    9c14:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    9c18:	ea000035 	b	9cf4 <udc_event_handle+0x228>
    9c1c:	00009c5c 	.word	0x00009c5c
    9c20:	00009c2c 	.word	0x00009c2c
    9c24:	00009c90 	.word	0x00009c90
    9c28:	00009c90 	.word	0x00009c90
    9c2c:	e59f160c 	ldr	r1, [pc, #1548]	; a240 <udc_event_handle+0x774>
    9c30:	e59f0614 	ldr	r0, [pc, #1556]	; a24c <udc_event_handle+0x780>
    9c34:	e5912910 	ldr	r2, [r1, #2320]
    9c38:	e590c00c 	ldr	ip, [r0, #12]
    9c3c:	e3a03004 	mov	r3, #4	; 0x4
    9c40:	e3822702 	orr	r2, r2, #524288	; 0x80000
    9c44:	e5803020 	str	r3, [r0, #32]
    9c48:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9c4c:	e5812910 	str	r2, [r1, #2320]
    9c50:	e581c914 	str	ip, [r1, #2324]
    9c54:	e5813900 	str	r3, [r1, #2304]
    9c58:	ea000025 	b	9cf4 <udc_event_handle+0x228>
    9c5c:	e59f15dc 	ldr	r1, [pc, #1500]	; a240 <udc_event_handle+0x774>
    9c60:	e59f05e4 	ldr	r0, [pc, #1508]	; a24c <udc_event_handle+0x780>
    9c64:	e5913b10 	ldr	r3, [r1, #2832]
    9c68:	e590c024 	ldr	ip, [r0, #36]
    9c6c:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    9c70:	e3833702 	orr	r3, r3, #524288	; 0x80000
    9c74:	e5813b10 	str	r3, [r1, #2832]
    9c78:	e3a02005 	mov	r2, #5	; 0x5
    9c7c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9c80:	e581cb14 	str	ip, [r1, #2836]
    9c84:	e5802020 	str	r2, [r0, #32]
    9c88:	e5813b00 	str	r3, [r1, #2816]
    9c8c:	ea000018 	b	9cf4 <udc_event_handle+0x228>
    9c90:	e59f05b4 	ldr	r0, [pc, #1460]	; a24c <udc_event_handle+0x780>
    9c94:	e5d03015 	ldrb	r3, [r0, #21]
    9c98:	e3530009 	cmp	r3, #9	; 0x9
    9c9c:	1a00000a 	bne	9ccc <udc_event_handle+0x200>
    9ca0:	e59f1598 	ldr	r1, [pc, #1432]	; a240 <udc_event_handle+0x774>
    9ca4:	e59f35ac 	ldr	r3, [pc, #1452]	; a258 <udc_event_handle+0x78c>
    9ca8:	e3a02001 	mov	r2, #1	; 0x1
    9cac:	e5813920 	str	r3, [r1, #2336]
    9cb0:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    9cb4:	e5813b40 	str	r3, [r1, #2880]
    9cb8:	e591381c 	ldr	r3, [r1, #2076]
    9cbc:	e5c02000 	strb	r2, [r0]
    9cc0:	e3833701 	orr	r3, r3, #262144	; 0x40000
    9cc4:	e3833002 	orr	r3, r3, #2	; 0x2
    9cc8:	e581381c 	str	r3, [r1, #2076]
    9ccc:	e59f256c 	ldr	r2, [pc, #1388]	; a240 <udc_event_handle+0x774>
    9cd0:	e59f1574 	ldr	r1, [pc, #1396]	; a24c <udc_event_handle+0x780>
    9cd4:	e59f3574 	ldr	r3, [pc, #1396]	; a250 <udc_event_handle+0x784>
    9cd8:	e5910024 	ldr	r0, [r1, #36]
    9cdc:	e5823b10 	str	r3, [r2, #2832]
    9ce0:	e3a03001 	mov	r3, #1	; 0x1
    9ce4:	e5813020 	str	r3, [r1, #32]
    9ce8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9cec:	e5820b14 	str	r0, [r2, #2836]
    9cf0:	e5823b00 	str	r3, [r2, #2816]
    9cf4:	e300320d 	movw	r3, #525	; 0x20d
    9cf8:	e59f2540 	ldr	r2, [pc, #1344]	; a240 <udc_event_handle+0x774>
    9cfc:	e0053003 	and	r3, r5, r3
    9d00:	e5823908 	str	r3, [r2, #2312]
    9d04:	e3190002 	tst	r9, #2	; 0x2
    9d08:	0a00000a 	beq	9d38 <udc_event_handle+0x26c>
    9d0c:	e59f352c 	ldr	r3, [pc, #1324]	; a240 <udc_event_handle+0x774>
    9d10:	e5932928 	ldr	r2, [r3, #2344]
    9d14:	e3120001 	tst	r2, #1	; 0x1
    9d18:	0a000003 	beq	9d2c <udc_event_handle+0x260>
    9d1c:	e5933930 	ldr	r3, [r3, #2352]
    9d20:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    9d24:	059f3520 	ldreq	r3, [pc, #1312]	; a24c <udc_event_handle+0x780>
    9d28:	05c31002 	strbeq	r1, [r3, #2]
    9d2c:	e59f350c 	ldr	r3, [pc, #1292]	; a240 <udc_event_handle+0x774>
    9d30:	e2022005 	and	r2, r2, #5	; 0x5
    9d34:	e5832928 	str	r2, [r3, #2344]
    9d38:	e3140702 	tst	r4, #524288	; 0x80000
    9d3c:	0a00013c 	beq	a234 <udc_event_handle+0x768>
    9d40:	e3190801 	tst	r9, #65536	; 0x10000
    9d44:	0a000124 	beq	a1dc <udc_event_handle+0x710>
    9d48:	e59f34f0 	ldr	r3, [pc, #1264]	; a240 <udc_event_handle+0x774>
    9d4c:	e593ab08 	ldr	sl, [r3, #2824]
    9d50:	e31a0001 	tst	sl, #1	; 0x1
    9d54:	0a00002b 	beq	9e08 <udc_event_handle+0x33c>
    9d58:	e59f34ec 	ldr	r3, [pc, #1260]	; a24c <udc_event_handle+0x780>
    9d5c:	e5933020 	ldr	r3, [r3, #32]
    9d60:	e2433002 	sub	r3, r3, #2	; 0x2
    9d64:	e3530003 	cmp	r3, #3	; 0x3
    9d68:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    9d6c:	ea000025 	b	9e08 <udc_event_handle+0x33c>
    9d70:	00009dac 	.word	0x00009dac
    9d74:	00009d80 	.word	0x00009d80
    9d78:	00009de0 	.word	0x00009de0
    9d7c:	00009de0 	.word	0x00009de0
    9d80:	e59f14c4 	ldr	r1, [pc, #1220]	; a24c <udc_event_handle+0x780>
    9d84:	e59f24b4 	ldr	r2, [pc, #1204]	; a240 <udc_event_handle+0x774>
    9d88:	e3a03004 	mov	r3, #4	; 0x4
    9d8c:	e591000c 	ldr	r0, [r1, #12]
    9d90:	e5813020 	str	r3, [r1, #32]
    9d94:	e3a03702 	mov	r3, #524288	; 0x80000
    9d98:	e5823910 	str	r3, [r2, #2320]
    9d9c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9da0:	e5820914 	str	r0, [r2, #2324]
    9da4:	e5823900 	str	r3, [r2, #2304]
    9da8:	ea000016 	b	9e08 <udc_event_handle+0x33c>
    9dac:	e59f148c 	ldr	r1, [pc, #1164]	; a240 <udc_event_handle+0x774>
    9db0:	e59f0494 	ldr	r0, [pc, #1172]	; a24c <udc_event_handle+0x780>
    9db4:	e5913b10 	ldr	r3, [r1, #2832]
    9db8:	e590c00c 	ldr	ip, [r0, #12]
    9dbc:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    9dc0:	e3833702 	orr	r3, r3, #524288	; 0x80000
    9dc4:	e5813b10 	str	r3, [r1, #2832]
    9dc8:	e3a02005 	mov	r2, #5	; 0x5
    9dcc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9dd0:	e581cb14 	str	ip, [r1, #2836]
    9dd4:	e5802020 	str	r2, [r0, #32]
    9dd8:	e5813b00 	str	r3, [r1, #2816]
    9ddc:	ea000009 	b	9e08 <udc_event_handle+0x33c>
    9de0:	e59f2458 	ldr	r2, [pc, #1112]	; a240 <udc_event_handle+0x774>
    9de4:	e59f1460 	ldr	r1, [pc, #1120]	; a24c <udc_event_handle+0x780>
    9de8:	e59f3460 	ldr	r3, [pc, #1120]	; a250 <udc_event_handle+0x784>
    9dec:	e5910024 	ldr	r0, [r1, #36]
    9df0:	e5823b10 	str	r3, [r2, #2832]
    9df4:	e3a03001 	mov	r3, #1	; 0x1
    9df8:	e5813020 	str	r3, [r1, #32]
    9dfc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    9e00:	e5820b14 	str	r0, [r2, #2836]
    9e04:	e5823b00 	str	r3, [r2, #2816]
    9e08:	e31a0004 	tst	sl, #4	; 0x4
    9e0c:	159f342c 	ldrne	r3, [pc, #1068]	; a240 <udc_event_handle+0x774>
    9e10:	13a02004 	movne	r2, #4	; 0x4
    9e14:	15832b08 	strne	r2, [r3, #2824]
    9e18:	e31a0008 	tst	sl, #8	; 0x8
    9e1c:	0a0000ea 	beq	a1cc <udc_event_handle+0x700>
    9e20:	e31a0040 	tst	sl, #64	; 0x40
    9e24:	e59f1430 	ldr	r1, [pc, #1072]	; a25c <udc_event_handle+0x790>
    9e28:	0a000006 	beq	9e48 <udc_event_handle+0x37c>
    9e2c:	e59f340c 	ldr	r3, [pc, #1036]	; a240 <udc_event_handle+0x774>
    9e30:	e5933b14 	ldr	r3, [r3, #2836]
    9e34:	e2432004 	sub	r2, r3, #4	; 0x4
    9e38:	e2433008 	sub	r3, r3, #8	; 0x8
    9e3c:	e5812004 	str	r2, [r1, #4]
    9e40:	e5813000 	str	r3, [r1]
    9e44:	ea000005 	b	9e60 <udc_event_handle+0x394>
    9e48:	e59f33fc 	ldr	r3, [pc, #1020]	; a24c <udc_event_handle+0x780>
    9e4c:	e5932024 	ldr	r2, [r3, #36]
    9e50:	e5923000 	ldr	r3, [r2]
    9e54:	e5813000 	str	r3, [r1]
    9e58:	e5923004 	ldr	r3, [r2, #4]
    9e5c:	e5813004 	str	r3, [r1, #4]
    9e60:	e59f23e4 	ldr	r2, [pc, #996]	; a24c <udc_event_handle+0x780>
    9e64:	e5d20014 	ldrb	r0, [r2, #20]
    9e68:	e3100080 	tst	r0, #128	; 0x80
    9e6c:	13a03002 	movne	r3, #2	; 0x2
    9e70:	03a03003 	moveq	r3, #3	; 0x3
    9e74:	e5823020 	str	r3, [r2, #32]
    9e78:	e59f23cc 	ldr	r2, [pc, #972]	; a24c <udc_event_handle+0x780>
    9e7c:	e1d211ba 	ldrh	r1, [r2, #26]
    9e80:	e3510000 	cmp	r1, #0	; 0x0
    9e84:	03a03004 	moveq	r3, #4	; 0x4
    9e88:	05823020 	streq	r3, [r2, #32]
    9e8c:	e59f33b8 	ldr	r3, [pc, #952]	; a24c <udc_event_handle+0x780>
    9e90:	e5d33015 	ldrb	r3, [r3, #21]
    9e94:	e3530009 	cmp	r3, #9	; 0x9
    9e98:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    9e9c:	ea0000ca 	b	a1cc <udc_event_handle+0x700>
    9ea0:	0000a164 	.word	0x0000a164
    9ea4:	0000a1cc 	.word	0x0000a1cc
    9ea8:	0000a1cc 	.word	0x0000a1cc
    9eac:	0000a1cc 	.word	0x0000a1cc
    9eb0:	0000a1cc 	.word	0x0000a1cc
    9eb4:	0000a0b0 	.word	0x0000a0b0
    9eb8:	00009ec8 	.word	0x00009ec8
    9ebc:	0000a1cc 	.word	0x0000a1cc
    9ec0:	0000a1cc 	.word	0x0000a1cc
    9ec4:	0000a100 	.word	0x0000a100
    9ec8:	e3a00040 	mov	r0, #64	; 0x40
    9ecc:	ebffdd22 	bl	135c <malloc>
    9ed0:	e2506000 	subs	r6, r0, #0	; 0x0
    9ed4:	0a0000bc 	beq	a1cc <udc_event_handle+0x700>
    9ed8:	e59f536c 	ldr	r5, [pc, #876]	; a24c <udc_event_handle+0x780>
    9edc:	e1d521b6 	ldrh	r2, [r5, #22]
    9ee0:	e1a03422 	lsr	r3, r2, #8
    9ee4:	e2433001 	sub	r3, r3, #1	; 0x1
    9ee8:	e3530005 	cmp	r3, #5	; 0x5
    9eec:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    9ef0:	ea000054 	b	a048 <udc_event_handle+0x57c>
    9ef4:	00009f0c 	.word	0x00009f0c
    9ef8:	00009f24 	.word	0x00009f24
    9efc:	00009f7c 	.word	0x00009f7c
    9f00:	0000a048 	.word	0x0000a048
    9f04:	0000a048 	.word	0x0000a048
    9f08:	00009f50 	.word	0x00009f50
    9f0c:	e1a00006 	mov	r0, r6
    9f10:	e59f1348 	ldr	r1, [pc, #840]	; a260 <udc_event_handle+0x794>
    9f14:	e3a02012 	mov	r2, #18	; 0x12
    9f18:	ebffdcaa 	bl	11c8 <memcpy>
    9f1c:	e3a03012 	mov	r3, #18	; 0x12
    9f20:	ea000028 	b	9fc8 <udc_event_handle+0x4fc>
    9f24:	e59f3338 	ldr	r3, [pc, #824]	; a264 <udc_event_handle+0x798>
    9f28:	e3a04020 	mov	r4, #32	; 0x20
    9f2c:	e3a0c000 	mov	ip, #0	; 0x0
    9f30:	e1a01003 	mov	r1, r3
    9f34:	e1a00006 	mov	r0, r6
    9f38:	e1a02004 	mov	r2, r4
    9f3c:	e5c3c003 	strb	ip, [r3, #3]
    9f40:	e5c34002 	strb	r4, [r3, #2]
    9f44:	ebffdc9f 	bl	11c8 <memcpy>
    9f48:	e5854004 	str	r4, [r5, #4]
    9f4c:	ea000045 	b	a068 <udc_event_handle+0x59c>
    9f50:	e1d531ba 	ldrh	r3, [r5, #26]
    9f54:	e59fc30c 	ldr	ip, [pc, #780]	; a268 <udc_event_handle+0x79c>
    9f58:	e353000a 	cmp	r3, #10	; 0xa
    9f5c:	23a0300a 	movcs	r3, #10	; 0xa
    9f60:	e5cc3000 	strb	r3, [ip]
    9f64:	e1a0100c 	mov	r1, ip
    9f68:	e1a00006 	mov	r0, r6
    9f6c:	e3a0200a 	mov	r2, #10	; 0xa
    9f70:	ebffdc94 	bl	11c8 <memcpy>
    9f74:	e3a0300a 	mov	r3, #10	; 0xa
    9f78:	ea000012 	b	9fc8 <udc_event_handle+0x4fc>
    9f7c:	e21270ff 	ands	r7, r2, #255	; 0xff
    9f80:	13a03000 	movne	r3, #0	; 0x0
    9f84:	03a03001 	moveq	r3, #1	; 0x1
    9f88:	e3570005 	cmp	r7, #5	; 0x5
    9f8c:	d1a08003 	movle	r8, r3
    9f90:	c3838001 	orrgt	r8, r3, #1	; 0x1
    9f94:	e3580000 	cmp	r8, #0	; 0x0
    9f98:	e59f52ac 	ldr	r5, [pc, #684]	; a24c <udc_event_handle+0x780>
    9f9c:	0a00000b 	beq	9fd0 <udc_event_handle+0x504>
    9fa0:	e1d531ba 	ldrh	r3, [r5, #26]
    9fa4:	e59fc2c0 	ldr	ip, [pc, #704]	; a26c <udc_event_handle+0x7a0>
    9fa8:	e3530004 	cmp	r3, #4	; 0x4
    9fac:	23a03004 	movcs	r3, #4	; 0x4
    9fb0:	e5cc3000 	strb	r3, [ip]
    9fb4:	e1a0100c 	mov	r1, ip
    9fb8:	e1a00006 	mov	r0, r6
    9fbc:	e3a02004 	mov	r2, #4	; 0x4
    9fc0:	ebffdc80 	bl	11c8 <memcpy>
    9fc4:	e3a03004 	mov	r3, #4	; 0x4
    9fc8:	e5853004 	str	r3, [r5, #4]
    9fcc:	ea000025 	b	a068 <udc_event_handle+0x59c>
    9fd0:	e59f4298 	ldr	r4, [pc, #664]	; a270 <udc_event_handle+0x7a4>
    9fd4:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    9fd8:	ebffdc60 	bl	1160 <strlen>
    9fdc:	e6ef0070 	uxtb	r0, r0
    9fe0:	e2803001 	add	r3, r0, #1	; 0x1
    9fe4:	e5d5201a 	ldrb	r2, [r5, #26]
    9fe8:	e1a03083 	lsl	r3, r3, #1
    9fec:	e6ef3073 	uxtb	r3, r3
    9ff0:	e1530002 	cmp	r3, r2
    9ff4:	31a0e003 	movcc	lr, r3
    9ff8:	21a0e002 	movcs	lr, r2
    9ffc:	e3a03003 	mov	r3, #3	; 0x3
    a000:	e5c63001 	strb	r3, [r6, #1]
    a004:	e5c6e000 	strb	lr, [r6]
    a008:	e1a05008 	mov	r5, r8
    a00c:	e3a02001 	mov	r2, #1	; 0x1
    a010:	ea000005 	b	a02c <udc_event_handle+0x560>
    a014:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    a018:	e0833002 	add	r3, r3, r2
    a01c:	e5533001 	ldrb	r3, [r3, #-1]
    a020:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    a024:	e5c15001 	strb	r5, [r1, #1]
    a028:	e6ef207c 	uxtb	r2, ip
    a02c:	e1500002 	cmp	r0, r2
    a030:	e282c001 	add	ip, r2, #1	; 0x1
    a034:	e0861082 	add	r1, r6, r2, lsl #1
    a038:	aafffff5 	bge	a014 <udc_event_handle+0x548>
    a03c:	e59f3208 	ldr	r3, [pc, #520]	; a24c <udc_event_handle+0x780>
    a040:	e583e004 	str	lr, [r3, #4]
    a044:	ea000007 	b	a068 <udc_event_handle+0x59c>
    a048:	e59f21f0 	ldr	r2, [pc, #496]	; a240 <udc_event_handle+0x774>
    a04c:	e3a03702 	mov	r3, #524288	; 0x80000
    a050:	e5951024 	ldr	r1, [r5, #36]
    a054:	e5823910 	str	r3, [r2, #2320]
    a058:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    a05c:	e5821914 	str	r1, [r2, #2324]
    a060:	e5823900 	str	r3, [r2, #2304]
    a064:	ea00000e 	b	a0a4 <udc_event_handle+0x5d8>
    a068:	e59f11dc 	ldr	r1, [pc, #476]	; a24c <udc_event_handle+0x780>
    a06c:	e59f01cc 	ldr	r0, [pc, #460]	; a240 <udc_event_handle+0x774>
    a070:	e1d120b4 	ldrh	r2, [r1, #4]
    a074:	e1d131ba 	ldrh	r3, [r1, #26]
    a078:	e1520003 	cmp	r2, r3
    a07c:	21a02003 	movcs	r2, r3
    a080:	e3823702 	orr	r3, r2, #524288	; 0x80000
    a084:	e5803910 	str	r3, [r0, #2320]
    a088:	e3a03000 	mov	r3, #0	; 0x0
    a08c:	e5c13001 	strb	r3, [r1, #1]
    a090:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    a094:	e5812004 	str	r2, [r1, #4]
    a098:	e5806914 	str	r6, [r0, #2324]
    a09c:	e5803900 	str	r3, [r0, #2304]
    a0a0:	e581600c 	str	r6, [r1, #12]
    a0a4:	e1a00006 	mov	r0, r6
    a0a8:	ebffdd13 	bl	14fc <free>
    a0ac:	ea000046 	b	a1cc <udc_event_handle+0x700>
    a0b0:	e59fc1bc 	ldr	ip, [pc, #444]	; a274 <udc_event_handle+0x7a8>
    a0b4:	e5dc3000 	ldrb	r3, [ip]
    a0b8:	e35300ff 	cmp	r3, #255	; 0xff
    a0bc:	1a000042 	bne	a1cc <udc_event_handle+0x700>
    a0c0:	e59f1178 	ldr	r1, [pc, #376]	; a240 <udc_event_handle+0x774>
    a0c4:	e59f0180 	ldr	r0, [pc, #384]	; a24c <udc_event_handle+0x780>
    a0c8:	e5913800 	ldr	r3, [r1, #2048]
    a0cc:	e1d021b6 	ldrh	r2, [r0, #22]
    a0d0:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    a0d4:	e202207f 	and	r2, r2, #127	; 0x7f
    a0d8:	e1833202 	orr	r3, r3, r2, lsl #4
    a0dc:	e5813800 	str	r3, [r1, #2048]
    a0e0:	e5900024 	ldr	r0, [r0, #36]
    a0e4:	e3a03702 	mov	r3, #524288	; 0x80000
    a0e8:	e5cc2000 	strb	r2, [ip]
    a0ec:	e5813910 	str	r3, [r1, #2320]
    a0f0:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    a0f4:	e5810914 	str	r0, [r1, #2324]
    a0f8:	e5813900 	str	r3, [r1, #2304]
    a0fc:	ea000032 	b	a1cc <udc_event_handle+0x700>
    a100:	e59f016c 	ldr	r0, [pc, #364]	; a274 <udc_event_handle+0x7a8>
    a104:	e5d03000 	ldrb	r3, [r0]
    a108:	e35300ff 	cmp	r3, #255	; 0xff
    a10c:	0a00002e 	beq	a1cc <udc_event_handle+0x700>
    a110:	e59f1134 	ldr	r1, [pc, #308]	; a24c <udc_event_handle+0x780>
    a114:	e1d131b6 	ldrh	r3, [r1, #22]
    a118:	e3530000 	cmp	r3, #0	; 0x0
    a11c:	03e03000 	mvneq	r3, #0	; 0x0
    a120:	05911024 	ldreq	r1, [r1, #36]
    a124:	059f2114 	ldreq	r2, [pc, #276]	; a240 <udc_event_handle+0x774>
    a128:	05c03000 	strbeq	r3, [r0]
    a12c:	0a000013 	beq	a180 <udc_event_handle+0x6b4>
    a130:	e3530001 	cmp	r3, #1	; 0x1
    a134:	1a000024 	bne	a1cc <udc_event_handle+0x700>
    a138:	e59f2100 	ldr	r2, [pc, #256]	; a240 <udc_event_handle+0x774>
    a13c:	e59f3134 	ldr	r3, [pc, #308]	; a278 <udc_event_handle+0x7ac>
    a140:	e5911024 	ldr	r1, [r1, #36]
    a144:	e5823920 	str	r3, [r2, #2336]
    a148:	e2833d07 	add	r3, r3, #448	; 0x1c0
    a14c:	e5823b40 	str	r3, [r2, #2880]
    a150:	e592381c 	ldr	r3, [r2, #2076]
    a154:	e3833701 	orr	r3, r3, #262144	; 0x40000
    a158:	e3833002 	orr	r3, r3, #2	; 0x2
    a15c:	e582381c 	str	r3, [r2, #2076]
    a160:	ea000006 	b	a180 <udc_event_handle+0x6b4>
    a164:	e210c01f 	ands	ip, r0, #31	; 0x1f
    a168:	1a000017 	bne	a1cc <udc_event_handle+0x700>
    a16c:	e3510000 	cmp	r1, #0	; 0x0
    a170:	e59f50d4 	ldr	r5, [pc, #212]	; a24c <udc_event_handle+0x780>
    a174:	1a000004 	bne	a18c <udc_event_handle+0x6c0>
    a178:	e5951024 	ldr	r1, [r5, #36]
    a17c:	e59f20bc 	ldr	r2, [pc, #188]	; a240 <udc_event_handle+0x774>
    a180:	e3a03702 	mov	r3, #524288	; 0x80000
    a184:	e5823910 	str	r3, [r2, #2320]
    a188:	ea00000c 	b	a1c0 <udc_event_handle+0x6f4>
    a18c:	e1d531b6 	ldrh	r3, [r5, #22]
    a190:	e59f20a8 	ldr	r2, [pc, #168]	; a240 <udc_event_handle+0x774>
    a194:	e3530002 	cmp	r3, #2	; 0x2
    a198:	23a03002 	movcs	r3, #2	; 0x2
    a19c:	e6ef3073 	uxtb	r3, r3
    a1a0:	e1530001 	cmp	r3, r1
    a1a4:	21a03001 	movcs	r3, r1
    a1a8:	e3830702 	orr	r0, r3, #524288	; 0x80000
    a1ac:	e28d1006 	add	r1, sp, #6	; 0x6
    a1b0:	e5c5c001 	strb	ip, [r5, #1]
    a1b4:	e5853004 	str	r3, [r5, #4]
    a1b8:	e585100c 	str	r1, [r5, #12]
    a1bc:	e5820910 	str	r0, [r2, #2320]
    a1c0:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    a1c4:	e5821914 	str	r1, [r2, #2324]
    a1c8:	e5823900 	str	r3, [r2, #2304]
    a1cc:	e3003209 	movw	r3, #521	; 0x209
    a1d0:	e59f2068 	ldr	r2, [pc, #104]	; a240 <udc_event_handle+0x774>
    a1d4:	e00a3003 	and	r3, sl, r3
    a1d8:	e5823b08 	str	r3, [r2, #2824]
    a1dc:	e3190701 	tst	r9, #262144	; 0x40000
    a1e0:	0a000013 	beq	a234 <udc_event_handle+0x768>
    a1e4:	e59f1054 	ldr	r1, [pc, #84]	; a240 <udc_event_handle+0x774>
    a1e8:	e5912b48 	ldr	r2, [r1, #2888]
    a1ec:	e3120001 	tst	r2, #1	; 0x1
    a1f0:	0a00000c 	beq	a228 <udc_event_handle+0x75c>
    a1f4:	e5913b50 	ldr	r3, [r1, #2896]
    a1f8:	e59f504c 	ldr	r5, [pc, #76]	; a24c <udc_event_handle+0x780>
    a1fc:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    a200:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    a204:	e3530c02 	cmp	r3, #512	; 0x200
    a208:	05953028 	ldreq	r3, [r5, #40]
    a20c:	13a03000 	movne	r3, #0	; 0x0
    a210:	03833702 	orreq	r3, r3, #524288	; 0x80000
    a214:	05813b50 	streq	r3, [r1, #2896]
    a218:	05913b40 	ldreq	r3, [r1, #2880]
    a21c:	15c53003 	strbne	r3, [r5, #3]
    a220:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    a224:	05813b40 	streq	r3, [r1, #2880]
    a228:	e59f3010 	ldr	r3, [pc, #16]	; a240 <udc_event_handle+0x774>
    a22c:	e2022005 	and	r2, r2, #5	; 0x5
    a230:	e5832b48 	str	r2, [r3, #2888]
    a234:	e28dd008 	add	sp, sp, #8	; 0x8
    a238:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    a23c:	3c00d1dc 	.word	0x3c00d1dc
    a240:	27100000 	.word	0x27100000
    a244:	00010001 	.word	0x00010001
    a248:	0c001780 	.word	0x0c001780
    a24c:	3c00d1e0 	.word	0x3c00d1e0
    a250:	60080040 	.word	0x60080040
    a254:	3c00d1d8 	.word	0x3c00d1d8
    a258:	18088200 	.word	0x18088200
    a25c:	3c00d1f4 	.word	0x3c00d1f4
    a260:	0000f8fc 	.word	0x0000f8fc
    a264:	3c00d0ed 	.word	0x3c00d0ed
    a268:	3c00d10d 	.word	0x3c00d10d
    a26c:	3c00d0c4 	.word	0x3c00d0c4
    a270:	3c00d6f8 	.word	0x3c00d6f8
    a274:	3c00d0ec 	.word	0x3c00d0ec
    a278:	18088040 	.word	0x18088040

0000a27c <udc_write>:
    a27c:	e59f30a4 	ldr	r3, [pc, #164]	; a328 <udc_write+0xac>
    a280:	e3a02001 	mov	r2, #1	; 0x1
    a284:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    a288:	e583000c 	str	r0, [r3, #12]
    a28c:	e1a05001 	mov	r5, r1
    a290:	e5c32001 	strb	r2, [r3, #1]
    a294:	e1a04003 	mov	r4, r3
    a298:	e1a08002 	mov	r8, r2
    a29c:	e59f6088 	ldr	r6, [pc, #136]	; a32c <udc_write+0xb0>
    a2a0:	e59f7088 	ldr	r7, [pc, #136]	; a330 <udc_write+0xb4>
    a2a4:	ea00001b 	b	a318 <udc_write+0x9c>
    a2a8:	e3550701 	cmp	r5, #262144	; 0x40000
    a2ac:	b1a02005 	movlt	r2, r5
    a2b0:	a3a02701 	movge	r2, #262144	; 0x40000
    a2b4:	e292303f 	adds	r3, r2, #63	; 0x3f
    a2b8:	4282307e 	addmi	r3, r2, #126	; 0x7e
    a2bc:	e594100c 	ldr	r1, [r4, #12]
    a2c0:	e1a03343 	asr	r3, r3, #6
    a2c4:	e1823983 	orr	r3, r2, r3, lsl #19
    a2c8:	e5863930 	str	r3, [r6, #2352]
    a2cc:	e5861934 	str	r1, [r6, #2356]
    a2d0:	e5963920 	ldr	r3, [r6, #2336]
    a2d4:	e5842004 	str	r2, [r4, #4]
    a2d8:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    a2dc:	e5863920 	str	r3, [r6, #2336]
    a2e0:	e5c48002 	strb	r8, [r4, #2]
    a2e4:	ea000000 	b	a2ec <udc_write+0x70>
    a2e8:	ebfffdf7 	bl	9acc <udc_event_handle>
    a2ec:	e5d43002 	ldrb	r3, [r4, #2]
    a2f0:	e3530000 	cmp	r3, #0	; 0x0
    a2f4:	0a000002 	beq	a304 <udc_write+0x88>
    a2f8:	e5973000 	ldr	r3, [r7]
    a2fc:	e3530000 	cmp	r3, #0	; 0x0
    a300:	1afffff8 	bne	a2e8 <udc_write+0x6c>
    a304:	e5943004 	ldr	r3, [r4, #4]
    a308:	e594200c 	ldr	r2, [r4, #12]
    a30c:	e0635005 	rsb	r5, r3, r5
    a310:	e0833002 	add	r3, r3, r2
    a314:	e584300c 	str	r3, [r4, #12]
    a318:	e3550000 	cmp	r5, #0	; 0x0
    a31c:	1affffe1 	bne	a2a8 <udc_write+0x2c>
    a320:	e1a00005 	mov	r0, r5
    a324:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    a328:	3c00d1e0 	.word	0x3c00d1e0
    a32c:	27100000 	.word	0x27100000
    a330:	3c00d0e8 	.word	0x3c00d0e8

0000a334 <udc_read>:
    a334:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    a338:	e59f30b4 	ldr	r3, [pc, #180]	; a3f4 <udc_read+0xc0>
    a33c:	e24dd004 	sub	sp, sp, #4	; 0x4
    a340:	e1a04001 	mov	r4, r1
    a344:	e1a05003 	mov	r5, r3
    a348:	e5830010 	str	r0, [r3, #16]
    a34c:	e3a0a001 	mov	sl, #1	; 0x1
    a350:	e59f60a0 	ldr	r6, [pc, #160]	; a3f8 <udc_read+0xc4>
    a354:	e59f80a0 	ldr	r8, [pc, #160]	; a3fc <udc_read+0xc8>
    a358:	e1a07003 	mov	r7, r3
    a35c:	ea00001f 	b	a3e0 <udc_read+0xac>
    a360:	e3540701 	cmp	r4, #262144	; 0x40000
    a364:	b1a02004 	movlt	r2, r4
    a368:	a3a02701 	movge	r2, #262144	; 0x40000
    a36c:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    a370:	e2833003 	add	r3, r3, #3	; 0x3
    a374:	e5951010 	ldr	r1, [r5, #16]
    a378:	e1a034c3 	asr	r3, r3, #9
    a37c:	e1823983 	orr	r3, r2, r3, lsl #19
    a380:	e5863b50 	str	r3, [r6, #2896]
    a384:	e5861b54 	str	r1, [r6, #2900]
    a388:	e5963b40 	ldr	r3, [r6, #2880]
    a38c:	e5852008 	str	r2, [r5, #8]
    a390:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    a394:	e5863b40 	str	r3, [r6, #2880]
    a398:	e5c5a003 	strb	sl, [r5, #3]
    a39c:	ea000002 	b	a3ac <udc_read+0x78>
    a3a0:	ebfffdc9 	bl	9acc <udc_event_handle>
    a3a4:	e3a00001 	mov	r0, #1	; 0x1
    a3a8:	ebfff8d2 	bl	86f8 <udelay>
    a3ac:	e5d53003 	ldrb	r3, [r5, #3]
    a3b0:	e3530000 	cmp	r3, #0	; 0x0
    a3b4:	0a000004 	beq	a3cc <udc_read+0x98>
    a3b8:	e5983000 	ldr	r3, [r8]
    a3bc:	e3530000 	cmp	r3, #0	; 0x0
    a3c0:	1afffff6 	bne	a3a0 <udc_read+0x6c>
    a3c4:	e3e04000 	mvn	r4, #0	; 0x0
    a3c8:	ea000006 	b	a3e8 <udc_read+0xb4>
    a3cc:	e5973008 	ldr	r3, [r7, #8]
    a3d0:	e5972010 	ldr	r2, [r7, #16]
    a3d4:	e0634004 	rsb	r4, r3, r4
    a3d8:	e0833002 	add	r3, r3, r2
    a3dc:	e5873010 	str	r3, [r7, #16]
    a3e0:	e3540000 	cmp	r4, #0	; 0x0
    a3e4:	1affffdd 	bne	a360 <udc_read+0x2c>
    a3e8:	e1a00004 	mov	r0, r4
    a3ec:	e28dd004 	add	sp, sp, #4	; 0x4
    a3f0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    a3f4:	3c00d1e0 	.word	0x3c00d1e0
    a3f8:	27100000 	.word	0x27100000
    a3fc:	3c00d0e8 	.word	0x3c00d0e8

0000a400 <udc_connect>:
    a400:	e92d4070 	push	{r4, r5, r6, lr}
    a404:	e59f41e4 	ldr	r4, [pc, #484]	; a5f0 <udc_connect+0x1f0>
    a408:	e3a02000 	mov	r2, #0	; 0x0
    a40c:	e5943024 	ldr	r3, [r4, #36]
    a410:	e3e01000 	mvn	r1, #0	; 0x0
    a414:	e1530002 	cmp	r3, r2
    a418:	e59f31d4 	ldr	r3, [pc, #468]	; a5f4 <udc_connect+0x1f4>
    a41c:	e5c42003 	strb	r2, [r4, #3]
    a420:	e5c31000 	strb	r1, [r3]
    a424:	e5c42000 	strb	r2, [r4]
    a428:	e5c42002 	strb	r2, [r4, #2]
    a42c:	1a000002 	bne	a43c <udc_connect+0x3c>
    a430:	e3a00028 	mov	r0, #40	; 0x28
    a434:	ebffdbc8 	bl	135c <malloc>
    a438:	e5840024 	str	r0, [r4, #36]
    a43c:	e59f31ac 	ldr	r3, [pc, #428]	; a5f0 <udc_connect+0x1f0>
    a440:	e3a06001 	mov	r6, #1	; 0x1
    a444:	e59f51ac 	ldr	r5, [pc, #428]	; a5f8 <udc_connect+0x1f8>
    a448:	e59f01ac 	ldr	r0, [pc, #428]	; a5fc <udc_connect+0x1fc>
    a44c:	e5836020 	str	r6, [r3, #32]
    a450:	ebffd8ac 	bl	708 <module_enable>
    a454:	e3a02004 	mov	r2, #4	; 0x4
    a458:	e3a03003 	mov	r3, #3	; 0x3
    a45c:	e3a01000 	mov	r1, #0	; 0x0
    a460:	e585243c 	str	r2, [r5, #1084]
    a464:	e5853440 	str	r3, [r5, #1088]
    a468:	e5852444 	str	r2, [r5, #1092]
    a46c:	e5853448 	str	r3, [r5, #1096]
    a470:	e585344c 	str	r3, [r5, #1100]
    a474:	e5853450 	str	r3, [r5, #1104]
    a478:	e5856454 	str	r6, [r5, #1108]
    a47c:	e5856458 	str	r6, [r5, #1112]
    a480:	e585145c 	str	r1, [r5, #1116]
    a484:	e5851460 	str	r1, [r5, #1120]
    a488:	e5953420 	ldr	r3, [r5, #1056]
    a48c:	e1a00006 	mov	r0, r6
    a490:	e3c33007 	bic	r3, r3, #7	; 0x7
    a494:	e3833005 	orr	r3, r3, #5	; 0x5
    a498:	e5853420 	str	r3, [r5, #1056]
    a49c:	e5953424 	ldr	r3, [r5, #1060]
    a4a0:	e3833003 	orr	r3, r3, #3	; 0x3
    a4a4:	e5853424 	str	r3, [r5, #1060]
    a4a8:	e595342c 	ldr	r3, [r5, #1068]
    a4ac:	e1833006 	orr	r3, r3, r6
    a4b0:	e585342c 	str	r3, [r5, #1068]
    a4b4:	e5954400 	ldr	r4, [r5, #1024]
    a4b8:	e384400f 	orr	r4, r4, #15	; 0xf
    a4bc:	e5854400 	str	r4, [r5, #1024]
    a4c0:	ebfff88c 	bl	86f8 <udelay>
    a4c4:	e3c4300e 	bic	r3, r4, #14	; 0xe
    a4c8:	e5853400 	str	r3, [r5, #1024]
    a4cc:	e1a00006 	mov	r0, r6
    a4d0:	e3c4400f 	bic	r4, r4, #15	; 0xf
    a4d4:	ebfff887 	bl	86f8 <udelay>
    a4d8:	e5854400 	str	r4, [r5, #1024]
    a4dc:	e1a00006 	mov	r0, r6
    a4e0:	ebfff884 	bl	86f8 <udelay>
    a4e4:	e59f2114 	ldr	r2, [pc, #276]	; a600 <udc_connect+0x200>
    a4e8:	e59f3114 	ldr	r3, [pc, #276]	; a604 <udc_connect+0x204>
    a4ec:	e1a05002 	mov	r5, r2
    a4f0:	e582300c 	str	r3, [r2, #12]
    a4f4:	e5954010 	ldr	r4, [r5, #16]
    a4f8:	e3a00028 	mov	r0, #40	; 0x28
    a4fc:	ebfff87d 	bl	86f8 <udelay>
    a500:	e3540000 	cmp	r4, #0	; 0x0
    a504:	e59f40f4 	ldr	r4, [pc, #244]	; a600 <udc_connect+0x200>
    a508:	aafffff9 	bge	a4f4 <udc_connect+0xf4>
    a50c:	e5943010 	ldr	r3, [r4, #16]
    a510:	e3a0000a 	mov	r0, #10	; 0xa
    a514:	e3833001 	orr	r3, r3, #1	; 0x1
    a518:	e5843010 	str	r3, [r4, #16]
    a51c:	ebfff875 	bl	86f8 <udelay>
    a520:	e1a05004 	mov	r5, r4
    a524:	e5954010 	ldr	r4, [r5, #16]
    a528:	e3a0000a 	mov	r0, #10	; 0xa
    a52c:	ebfff871 	bl	86f8 <udelay>
    a530:	e3140001 	tst	r4, #1	; 0x1
    a534:	e59fc0c4 	ldr	ip, [pc, #196]	; a600 <udc_connect+0x200>
    a538:	1afffff9 	bne	a524 <udc_connect+0x124>
    a53c:	e3a03027 	mov	r3, #39	; 0x27
    a540:	e58c3008 	str	r3, [ip, #8]
    a544:	e59c303c 	ldr	r3, [ip, #60]
    a548:	e59c3014 	ldr	r3, [ip, #20]
    a54c:	e2130001 	ands	r0, r3, #1	; 0x1
    a550:	1a000024 	bne	a5e8 <udc_connect+0x1e8>
    a554:	e3a03301 	mov	r3, #67108864	; 0x4000000
    a558:	e58c3800 	str	r3, [ip, #2048]
    a55c:	e59f30a4 	ldr	r3, [pc, #164]	; a608 <udc_connect+0x208>
    a560:	e59f20a4 	ldr	r2, [pc, #164]	; a60c <udc_connect+0x20c>
    a564:	e58c3018 	str	r3, [ip, #24]
    a568:	e59f30a0 	ldr	r3, [pc, #160]	; a610 <udc_connect+0x210>
    a56c:	e59f10a0 	ldr	r1, [pc, #160]	; a614 <udc_connect+0x214>
    a570:	e5823014 	str	r3, [r2, #20]
    a574:	e59f309c 	ldr	r3, [pc, #156]	; a618 <udc_connect+0x218>
    a578:	e5821010 	str	r1, [r2, #16]
    a57c:	e5823004 	str	r3, [r2, #4]
    a580:	e59f3094 	ldr	r3, [pc, #148]	; a61c <udc_connect+0x21c>
    a584:	e582100c 	str	r1, [r2, #12]
    a588:	e5823008 	str	r3, [r2, #8]
    a58c:	ebfff87d 	bl	8788 <get_timer>
    a590:	e59f5058 	ldr	r5, [pc, #88]	; a5f0 <udc_connect+0x1f0>
    a594:	e1a04000 	mov	r4, r0
    a598:	ebfffd4b 	bl	9acc <udc_event_handle>
    a59c:	e5d53000 	ldrb	r3, [r5]
    a5a0:	e1a00004 	mov	r0, r4
    a5a4:	e3530000 	cmp	r3, #0	; 0x0
    a5a8:	1a000003 	bne	a5bc <udc_connect+0x1bc>
    a5ac:	ebfff875 	bl	8788 <get_timer>
    a5b0:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    a5b4:	9afffff7 	bls	a598 <udc_connect+0x198>
    a5b8:	ea00000a 	b	a5e8 <udc_connect+0x1e8>
    a5bc:	e59f203c 	ldr	r2, [pc, #60]	; a600 <udc_connect+0x200>
    a5c0:	e59f3058 	ldr	r3, [pc, #88]	; a620 <udc_connect+0x220>
    a5c4:	e3a00000 	mov	r0, #0	; 0x0
    a5c8:	e5823920 	str	r3, [r2, #2336]
    a5cc:	e2833d07 	add	r3, r3, #448	; 0x1c0
    a5d0:	e5823b40 	str	r3, [r2, #2880]
    a5d4:	e592381c 	ldr	r3, [r2, #2076]
    a5d8:	e3833701 	orr	r3, r3, #262144	; 0x40000
    a5dc:	e3833002 	orr	r3, r3, #2	; 0x2
    a5e0:	e582381c 	str	r3, [r2, #2076]
    a5e4:	e8bd8070 	pop	{r4, r5, r6, pc}
    a5e8:	e3e00000 	mvn	r0, #0	; 0x0
    a5ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    a5f0:	3c00d1e0 	.word	0x3c00d1e0
    a5f4:	3c00d0ec 	.word	0x3c00d0ec
    a5f8:	21e38000 	.word	0x21e38000
    a5fc:	21e38400 	.word	0x21e38400
    a600:	27100000 	.word	0x27100000
    a604:	4c001408 	.word	0x4c001408
    a608:	000c3c00 	.word	0x000c3c00
    a60c:	3c00d6f8 	.word	0x3c00d6f8
    a610:	000114ca 	.word	0x000114ca
    a614:	000114df 	.word	0x000114df
    a618:	000114d3 	.word	0x000114d3
    a61c:	3c00d0c8 	.word	0x3c00d0c8
    a620:	18088040 	.word	0x18088040

0000a624 <linkpc_set_stage>:
    a624:	e1a01000 	mov	r1, r0
    a628:	e59f0004 	ldr	r0, [pc, #4]	; a634 <linkpc_set_stage+0x10>
    a62c:	e3a02010 	mov	r2, #16	; 0x10
    a630:	eaffda7a 	b	1020 <strncpy>
    a634:	3c00d118 	.word	0x3c00d118

0000a638 <__response_ok>:
    a638:	e92d4010 	push	{r4, lr}
    a63c:	e24dd020 	sub	sp, sp, #32	; 0x20
    a640:	e59f1038 	ldr	r1, [pc, #56]	; a680 <__response_ok+0x48>
    a644:	e3a02005 	mov	r2, #5	; 0x5
    a648:	e1a0000d 	mov	r0, sp
    a64c:	ebffdadd 	bl	11c8 <memcpy>
    a650:	e3a0201b 	mov	r2, #27	; 0x1b
    a654:	e3a01000 	mov	r1, #0	; 0x0
    a658:	e28d0005 	add	r0, sp, #5	; 0x5
    a65c:	ebffdad1 	bl	11a8 <memset>
    a660:	e1a0000d 	mov	r0, sp
    a664:	e3a01004 	mov	r1, #4	; 0x4
    a668:	ebffff03 	bl	a27c <udc_write>
    a66c:	e59f0010 	ldr	r0, [pc, #16]	; a684 <__response_ok+0x4c>
    a670:	e1a0400d 	mov	r4, sp
    a674:	ebfff81f 	bl	86f8 <udelay>
    a678:	e28dd020 	add	sp, sp, #32	; 0x20
    a67c:	e8bd8010 	pop	{r4, pc}
    a680:	0000f90e 	.word	0x0000f90e
    a684:	0007a120 	.word	0x0007a120

0000a688 <linkpc_loop>:
    a688:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a68c:	e59f05e0 	ldr	r0, [pc, #1504]	; ac74 <linkpc_loop+0x5ec>
    a690:	e24ddf59 	sub	sp, sp, #356	; 0x164
    a694:	e59f45dc 	ldr	r4, [pc, #1500]	; ac78 <linkpc_loop+0x5f0>
    a698:	e1a0e00f 	mov	lr, pc
    a69c:	e594f000 	ldr	pc, [r4]
    a6a0:	ebffff56 	bl	a400 <udc_connect>
    a6a4:	e3500000 	cmp	r0, #0	; 0x0
    a6a8:	13a04001 	movne	r4, #1	; 0x1
    a6ac:	1a000169 	bne	ac58 <linkpc_loop+0x5d0>
    a6b0:	e59f05c4 	ldr	r0, [pc, #1476]	; ac7c <linkpc_loop+0x5f4>
    a6b4:	e1a0e00f 	mov	lr, pc
    a6b8:	e594f000 	ldr	pc, [r4]
    a6bc:	e59f05bc 	ldr	r0, [pc, #1468]	; ac80 <linkpc_loop+0x5f8>
    a6c0:	e1a0e00f 	mov	lr, pc
    a6c4:	e594f000 	ldr	pc, [r4]
    a6c8:	e28d708c 	add	r7, sp, #140	; 0x8c
    a6cc:	e59f95b0 	ldr	r9, [pc, #1456]	; ac84 <linkpc_loop+0x5fc>
    a6d0:	e1a0a004 	mov	sl, r4
    a6d4:	e287b004 	add	fp, r7, #4	; 0x4
    a6d8:	e5991000 	ldr	r1, [r9]
    a6dc:	e3510000 	cmp	r1, #0	; 0x0
    a6e0:	159f25a0 	ldrne	r2, [pc, #1440]	; ac88 <linkpc_loop+0x600>
    a6e4:	028d4f43 	addeq	r4, sp, #268	; 0x10c
    a6e8:	15923000 	ldrne	r3, [r2]
    a6ec:	03a05040 	moveq	r5, #64	; 0x40
    a6f0:	1283430f 	addne	r4, r3, #1006632960	; 0x3c000000
    a6f4:	11a05001 	movne	r5, r1
    a6f8:	e1a01005 	mov	r1, r5
    a6fc:	e1a00004 	mov	r0, r4
    a700:	ebffff0b 	bl	a334 <udc_read>
    a704:	e1a00007 	mov	r0, r7
    a708:	e3a01000 	mov	r1, #0	; 0x0
    a70c:	e3a02080 	mov	r2, #128	; 0x80
    a710:	ebffdaa4 	bl	11a8 <memset>
    a714:	e5993000 	ldr	r3, [r9]
    a718:	e3530000 	cmp	r3, #0	; 0x0
    a71c:	0a00002a 	beq	a7cc <linkpc_loop+0x144>
    a720:	e28d400c 	add	r4, sp, #12	; 0xc
    a724:	e3a02080 	mov	r2, #128	; 0x80
    a728:	e3a01000 	mov	r1, #0	; 0x0
    a72c:	e1a00004 	mov	r0, r4
    a730:	ebffda9c 	bl	11a8 <memset>
    a734:	e59f0550 	ldr	r0, [pc, #1360]	; ac8c <linkpc_loop+0x604>
    a738:	e59f1550 	ldr	r1, [pc, #1360]	; ac90 <linkpc_loop+0x608>
    a73c:	e1a0e00f 	mov	lr, pc
    a740:	e59af000 	ldr	pc, [sl]
    a744:	e59fc53c 	ldr	ip, [pc, #1340]	; ac88 <linkpc_loop+0x600>
    a748:	e5990000 	ldr	r0, [r9]
    a74c:	e59c3000 	ldr	r3, [ip]
    a750:	e0632000 	rsb	r2, r3, r0
    a754:	e1550002 	cmp	r5, r2
    a758:	90833005 	addls	r3, r3, r5
    a75c:	80833002 	addhi	r3, r3, r2
    a760:	e1530000 	cmp	r3, r0
    a764:	e58c3000 	str	r3, [ip]
    a768:	3affffda 	bcc	a6d8 <linkpc_loop+0x50>
    a76c:	e59f3520 	ldr	r3, [pc, #1312]	; ac94 <linkpc_loop+0x60c>
    a770:	e5933000 	ldr	r3, [r3]
    a774:	e3530000 	cmp	r3, #0	; 0x0
    a778:	e59f3518 	ldr	r3, [pc, #1304]	; ac98 <linkpc_loop+0x610>
    a77c:	159f1518 	ldrne	r1, [pc, #1304]	; ac9c <linkpc_loop+0x614>
    a780:	059f1518 	ldreq	r1, [pc, #1304]	; aca0 <linkpc_loop+0x618>
    a784:	e5830000 	str	r0, [r3]
    a788:	e3a03000 	mov	r3, #0	; 0x0
    a78c:	11a00004 	movne	r0, r4
    a790:	01a00004 	moveq	r0, r4
    a794:	e28d400c 	add	r4, sp, #12	; 0xc
    a798:	e5893000 	str	r3, [r9]
    a79c:	eb00061d 	bl	c018 <sprintf>
    a7a0:	e1a00004 	mov	r0, r4
    a7a4:	ebffda6d 	bl	1160 <strlen>
    a7a8:	e1a01000 	mov	r1, r0
    a7ac:	e1a00004 	mov	r0, r4
    a7b0:	ebfffeb1 	bl	a27c <udc_write>
    a7b4:	e59f24cc 	ldr	r2, [pc, #1228]	; ac88 <linkpc_loop+0x600>
    a7b8:	e59f04e4 	ldr	r0, [pc, #1252]	; aca4 <linkpc_loop+0x61c>
    a7bc:	e5921000 	ldr	r1, [r2]
    a7c0:	e1a0e00f 	mov	lr, pc
    a7c4:	e59af000 	ldr	pc, [sl]
    a7c8:	eaffffc2 	b	a6d8 <linkpc_loop+0x50>
    a7cc:	e1a01004 	mov	r1, r4
    a7d0:	e59f04b4 	ldr	r0, [pc, #1204]	; ac8c <linkpc_loop+0x604>
    a7d4:	e1a0e00f 	mov	lr, pc
    a7d8:	e59af000 	ldr	pc, [sl]
    a7dc:	e59f14c4 	ldr	r1, [pc, #1220]	; aca8 <linkpc_loop+0x620>
    a7e0:	e1a00007 	mov	r0, r7
    a7e4:	eb00060b 	bl	c018 <sprintf>
    a7e8:	e1a00004 	mov	r0, r4
    a7ec:	e59f14b8 	ldr	r1, [pc, #1208]	; acac <linkpc_loop+0x624>
    a7f0:	e3a02006 	mov	r2, #6	; 0x6
    a7f4:	ebffda3a 	bl	10e4 <strncmp>
    a7f8:	e3500000 	cmp	r0, #0	; 0x0
    a7fc:	1a000002 	bne	a80c <linkpc_loop+0x184>
    a800:	ebffff8c 	bl	a638 <__response_ok>
    a804:	ebffd7f4 	bl	7dc <system_reset>
    a808:	ea000105 	b	ac24 <linkpc_loop+0x59c>
    a80c:	e1a00004 	mov	r0, r4
    a810:	e59f1498 	ldr	r1, [pc, #1176]	; acb0 <linkpc_loop+0x628>
    a814:	e3a02007 	mov	r2, #7	; 0x7
    a818:	ebffda31 	bl	10e4 <strncmp>
    a81c:	e3500000 	cmp	r0, #0	; 0x0
    a820:	1a00002d 	bne	a8dc <linkpc_loop+0x254>
    a824:	e2844007 	add	r4, r4, #7	; 0x7
    a828:	e1a02004 	mov	r2, r4
    a82c:	e59f1480 	ldr	r1, [pc, #1152]	; acb4 <linkpc_loop+0x62c>
    a830:	e59f0480 	ldr	r0, [pc, #1152]	; acb8 <linkpc_loop+0x630>
    a834:	e1a0e00f 	mov	lr, pc
    a838:	e59af000 	ldr	pc, [sl]
    a83c:	e59f145c 	ldr	r1, [pc, #1116]	; aca0 <linkpc_loop+0x618>
    a840:	e1a00007 	mov	r0, r7
    a844:	ebffd9ee 	bl	1004 <strcpy>
    a848:	e59f146c 	ldr	r1, [pc, #1132]	; acbc <linkpc_loop+0x634>
    a84c:	e1a00004 	mov	r0, r4
    a850:	e3a02007 	mov	r2, #7	; 0x7
    a854:	ebffda22 	bl	10e4 <strncmp>
    a858:	e3500000 	cmp	r0, #0	; 0x0
    a85c:	01a0000b 	moveq	r0, fp
    a860:	059f1458 	ldreq	r1, [pc, #1112]	; acc0 <linkpc_loop+0x638>
    a864:	0a000010 	beq	a8ac <linkpc_loop+0x224>
    a868:	e59f1454 	ldr	r1, [pc, #1108]	; acc4 <linkpc_loop+0x63c>
    a86c:	e1a00004 	mov	r0, r4
    a870:	e3a02005 	mov	r2, #5	; 0x5
    a874:	ebffda1a 	bl	10e4 <strncmp>
    a878:	e3500000 	cmp	r0, #0	; 0x0
    a87c:	01a0000b 	moveq	r0, fp
    a880:	059f1440 	ldreq	r1, [pc, #1088]	; acc8 <linkpc_loop+0x640>
    a884:	0a000008 	beq	a8ac <linkpc_loop+0x224>
    a888:	e1a00004 	mov	r0, r4
    a88c:	e59f1438 	ldr	r1, [pc, #1080]	; accc <linkpc_loop+0x644>
    a890:	e3a02008 	mov	r2, #8	; 0x8
    a894:	ebffda12 	bl	10e4 <strncmp>
    a898:	e3500000 	cmp	r0, #0	; 0x0
    a89c:	1a000004 	bne	a8b4 <linkpc_loop+0x22c>
    a8a0:	e59f3428 	ldr	r3, [pc, #1064]	; acd0 <linkpc_loop+0x648>
    a8a4:	e1a0000b 	mov	r0, fp
    a8a8:	e5931008 	ldr	r1, [r3, #8]
    a8ac:	ebffd9d4 	bl	1004 <strcpy>
    a8b0:	ea0000db 	b	ac24 <linkpc_loop+0x59c>
    a8b4:	e1a00004 	mov	r0, r4
    a8b8:	e59f1414 	ldr	r1, [pc, #1044]	; acd4 <linkpc_loop+0x64c>
    a8bc:	e3a0200c 	mov	r2, #12	; 0xc
    a8c0:	ebffda07 	bl	10e4 <strncmp>
    a8c4:	e3500000 	cmp	r0, #0	; 0x0
    a8c8:	1a0000d5 	bne	ac24 <linkpc_loop+0x59c>
    a8cc:	e1a0000b 	mov	r0, fp
    a8d0:	e59f1400 	ldr	r1, [pc, #1024]	; acd8 <linkpc_loop+0x650>
    a8d4:	e3a02903 	mov	r2, #49152	; 0xc000
    a8d8:	ea000020 	b	a960 <linkpc_loop+0x2d8>
    a8dc:	e1a00004 	mov	r0, r4
    a8e0:	e59f13f4 	ldr	r1, [pc, #1012]	; acdc <linkpc_loop+0x654>
    a8e4:	e3a02009 	mov	r2, #9	; 0x9
    a8e8:	ebffd9fd 	bl	10e4 <strncmp>
    a8ec:	e2505000 	subs	r5, r0, #0	; 0x0
    a8f0:	1a00001c 	bne	a968 <linkpc_loop+0x2e0>
    a8f4:	e59f3398 	ldr	r3, [pc, #920]	; ac94 <linkpc_loop+0x60c>
    a8f8:	e59fc388 	ldr	ip, [pc, #904]	; ac88 <linkpc_loop+0x600>
    a8fc:	e3a02010 	mov	r2, #16	; 0x10
    a900:	e1a01005 	mov	r1, r5
    a904:	e5835000 	str	r5, [r3]
    a908:	e58c5000 	str	r5, [ip]
    a90c:	e2840009 	add	r0, r4, #9	; 0x9
    a910:	eb000234 	bl	b1e8 <simple_strtoul>
    a914:	e1a01000 	mov	r1, r0
    a918:	e5890000 	str	r0, [r9]
    a91c:	e59f03bc 	ldr	r0, [pc, #956]	; ace0 <linkpc_loop+0x658>
    a920:	e1a0e00f 	mov	lr, pc
    a924:	e59af000 	ldr	pc, [sl]
    a928:	e5992000 	ldr	r2, [r9]
    a92c:	e3520000 	cmp	r2, #0	; 0x0
    a930:	01a00007 	moveq	r0, r7
    a934:	059f13a8 	ldreq	r1, [pc, #936]	; ace4 <linkpc_loop+0x65c>
    a938:	0a0000b8 	beq	ac20 <linkpc_loop+0x598>
    a93c:	e3520903 	cmp	r2, #49152	; 0xc000
    a940:	91a00007 	movls	r0, r7
    a944:	959f139c 	ldrls	r1, [pc, #924]	; ace8 <linkpc_loop+0x660>
    a948:	9a000004 	bls	a960 <linkpc_loop+0x2d8>
    a94c:	e1a00007 	mov	r0, r7
    a950:	e59f1394 	ldr	r1, [pc, #916]	; acec <linkpc_loop+0x664>
    a954:	eb0005af 	bl	c018 <sprintf>
    a958:	e5895000 	str	r5, [r9]
    a95c:	ea0000b0 	b	ac24 <linkpc_loop+0x59c>
    a960:	eb0005ac 	bl	c018 <sprintf>
    a964:	ea0000ae 	b	ac24 <linkpc_loop+0x59c>
    a968:	e1a00004 	mov	r0, r4
    a96c:	e59f137c 	ldr	r1, [pc, #892]	; acf0 <linkpc_loop+0x668>
    a970:	e3a02006 	mov	r2, #6	; 0x6
    a974:	ebffd9da 	bl	10e4 <strncmp>
    a978:	e3500000 	cmp	r0, #0	; 0x0
    a97c:	1a000065 	bne	ab18 <linkpc_loop+0x490>
    a980:	e2844006 	add	r4, r4, #6	; 0x6
    a984:	e1a00004 	mov	r0, r4
    a988:	e59f1364 	ldr	r1, [pc, #868]	; acf4 <linkpc_loop+0x66c>
    a98c:	e3a02003 	mov	r2, #3	; 0x3
    a990:	ebffd9d3 	bl	10e4 <strncmp>
    a994:	e3500000 	cmp	r0, #0	; 0x0
    a998:	1a000007 	bne	a9bc <linkpc_loop+0x334>
    a99c:	e280030f 	add	r0, r0, #1006632960	; 0x3c000000
    a9a0:	ebffedb6 	bl	6080 <cdbg_config_isi>
    a9a4:	e3500000 	cmp	r0, #0	; 0x0
    a9a8:	11a00007 	movne	r0, r7
    a9ac:	159f1344 	ldrne	r1, [pc, #836]	; acf8 <linkpc_loop+0x670>
    a9b0:	01a00007 	moveq	r0, r7
    a9b4:	059f12e4 	ldreq	r1, [pc, #740]	; aca0 <linkpc_loop+0x618>
    a9b8:	ea000098 	b	ac20 <linkpc_loop+0x598>
    a9bc:	e1a00004 	mov	r0, r4
    a9c0:	ebffe8b4 	bl	4c98 <vs_device_id>
    a9c4:	e59f6330 	ldr	r6, [pc, #816]	; acfc <linkpc_loop+0x674>
    a9c8:	e59fc330 	ldr	ip, [pc, #816]	; ad00 <linkpc_loop+0x678>
    a9cc:	e5963000 	ldr	r3, [r6]
    a9d0:	e1a05000 	mov	r5, r0
    a9d4:	e58d3000 	str	r3, [sp]
    a9d8:	e59f0324 	ldr	r0, [pc, #804]	; ad04 <linkpc_loop+0x67c>
    a9dc:	e59f1324 	ldr	r1, [pc, #804]	; ad08 <linkpc_loop+0x680>
    a9e0:	e1a02005 	mov	r2, r5
    a9e4:	e59c3000 	ldr	r3, [ip]
    a9e8:	e1a0e00f 	mov	lr, pc
    a9ec:	e59af000 	ldr	pc, [sl]
    a9f0:	e355000e 	cmp	r5, #14	; 0xe
    a9f4:	ca000006 	bgt	aa14 <linkpc_loop+0x38c>
    a9f8:	e1a00005 	mov	r0, r5
    a9fc:	ebffe891 	bl	4c48 <vs_device_erasable>
    aa00:	e3500000 	cmp	r0, #0	; 0x0
    aa04:	0a000005 	beq	aa20 <linkpc_loop+0x398>
    aa08:	e5963000 	ldr	r3, [r6]
    aa0c:	e3530000 	cmp	r3, #0	; 0x0
    aa10:	1a000002 	bne	aa20 <linkpc_loop+0x398>
    aa14:	e1a00007 	mov	r0, r7
    aa18:	e59f12ec 	ldr	r1, [pc, #748]	; ad0c <linkpc_loop+0x684>
    aa1c:	ea00007f 	b	ac20 <linkpc_loop+0x598>
    aa20:	e3a00901 	mov	r0, #16384	; 0x4000
    aa24:	ebffda4c 	bl	135c <malloc>
    aa28:	e2508000 	subs	r8, r0, #0	; 0x0
    aa2c:	01a00007 	moveq	r0, r7
    aa30:	059f12d8 	ldreq	r1, [pc, #728]	; ad10 <linkpc_loop+0x688>
    aa34:	0a000079 	beq	ac20 <linkpc_loop+0x598>
    aa38:	e3a01001 	mov	r1, #1	; 0x1
    aa3c:	e1a00005 	mov	r0, r5
    aa40:	ebffe80c 	bl	4a78 <vs_assign_by_id>
    aa44:	e3500000 	cmp	r0, #0	; 0x0
    aa48:	b1a00007 	movlt	r0, r7
    aa4c:	b59f12c0 	ldrlt	r1, [pc, #704]	; ad14 <linkpc_loop+0x68c>
    aa50:	ba00002c 	blt	ab08 <linkpc_loop+0x480>
    aa54:	e355000d 	cmp	r5, #13	; 0xd
    aa58:	13a06002 	movne	r6, #2	; 0x2
    aa5c:	03a06001 	moveq	r6, #1	; 0x1
    aa60:	e1a04c06 	lsl	r4, r6, #24
    aa64:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    aa68:	03a0500f 	moveq	r5, #15	; 0xf
    aa6c:	e58d414c 	str	r4, [sp, #332]
    aa70:	ebfffb9b 	bl	98e4 <ram_addr2offs>
    aa74:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
    aa78:	e0922000 	adds	r2, r2, r0
    aa7c:	e3e03000 	mvn	r3, #0	; 0x0
    aa80:	e0a33001 	adc	r3, r3, r1
    aa84:	e59f1270 	ldr	r1, [pc, #624]	; acfc <linkpc_loop+0x674>
    aa88:	e59fc270 	ldr	ip, [pc, #624]	; ad00 <linkpc_loop+0x678>
    aa8c:	e1d100b2 	ldrh	r0, [r1, #2]
    aa90:	e1a014a2 	lsr	r1, r2, #9
    aa94:	e1811b83 	orr	r1, r1, r3, lsl #23
    aa98:	e59f31f8 	ldr	r3, [pc, #504]	; ac98 <linkpc_loop+0x610>
    aa9c:	e1844001 	orr	r4, r4, r1
    aaa0:	e5932000 	ldr	r2, [r3]
    aaa4:	e59c3000 	ldr	r3, [ip]
    aaa8:	e3560001 	cmp	r6, #1	; 0x1
    aaac:	e1a0c800 	lsl	ip, r0, #16
    aab0:	e58d414c 	str	r4, [sp, #332]
    aab4:	e58d3150 	str	r3, [sp, #336]
    aab8:	e58d2158 	str	r2, [sp, #344]
    aabc:	e58dc154 	str	ip, [sp, #340]
    aac0:	1a000004 	bne	aad8 <linkpc_loop+0x450>
    aac4:	ebfffedb 	bl	a638 <__response_ok>
    aac8:	e59d0150 	ldr	r0, [sp, #336]
    aacc:	e3a01000 	mov	r1, #0	; 0x0
    aad0:	ebffecf5 	bl	5eac <cdbg_jump>
    aad4:	eafffffe 	b	aad4 <linkpc_loop+0x44c>
    aad8:	e3a0100d 	mov	r1, #13	; 0xd
    aadc:	e1a03005 	mov	r3, r5
    aae0:	e28d0f53 	add	r0, sp, #332	; 0x14c
    aae4:	e3a02000 	mov	r2, #0	; 0x0
    aae8:	e58dc000 	str	ip, [sp]
    aaec:	e58d8004 	str	r8, [sp, #4]
    aaf0:	ebffed77 	bl	60d4 <burn_image_loop>
    aaf4:	e3500000 	cmp	r0, #0	; 0x0
    aaf8:	159f1218 	ldrne	r1, [pc, #536]	; ad18 <linkpc_loop+0x690>
    aafc:	059f119c 	ldreq	r1, [pc, #412]	; aca0 <linkpc_loop+0x618>
    ab00:	11a00007 	movne	r0, r7
    ab04:	01a00007 	moveq	r0, r7
    ab08:	eb000542 	bl	c018 <sprintf>
    ab0c:	e1a00008 	mov	r0, r8
    ab10:	ebffda79 	bl	14fc <free>
    ab14:	ea000042 	b	ac24 <linkpc_loop+0x59c>
    ab18:	e1a00004 	mov	r0, r4
    ab1c:	e59f11f8 	ldr	r1, [pc, #504]	; ad1c <linkpc_loop+0x694>
    ab20:	e3a02008 	mov	r2, #8	; 0x8
    ab24:	ebffd96e 	bl	10e4 <strncmp>
    ab28:	e3500000 	cmp	r0, #0	; 0x0
    ab2c:	1a000003 	bne	ab40 <linkpc_loop+0x4b8>
    ab30:	ebfffec0 	bl	a638 <__response_ok>
    ab34:	e3a00001 	mov	r0, #1	; 0x1
    ab38:	ebffd622 	bl	3c8 <try_boot>
    ab3c:	ea000038 	b	ac24 <linkpc_loop+0x59c>
    ab40:	e1a00004 	mov	r0, r4
    ab44:	e59f11d4 	ldr	r1, [pc, #468]	; ad20 <linkpc_loop+0x698>
    ab48:	e3a0200a 	mov	r2, #10	; 0xa
    ab4c:	ebffd964 	bl	10e4 <strncmp>
    ab50:	e2505000 	subs	r5, r0, #0	; 0x0
    ab54:	1a000019 	bne	abc0 <linkpc_loop+0x538>
    ab58:	e28d1f57 	add	r1, sp, #348	; 0x15c
    ab5c:	e284000b 	add	r0, r4, #11	; 0xb
    ab60:	e3a02010 	mov	r2, #16	; 0x10
    ab64:	eb00019f 	bl	b1e8 <simple_strtoul>
    ab68:	e59f2190 	ldr	r2, [pc, #400]	; ad00 <linkpc_loop+0x678>
    ab6c:	e1a01005 	mov	r1, r5
    ab70:	e5820000 	str	r0, [r2]
    ab74:	e59d015c 	ldr	r0, [sp, #348]
    ab78:	e3a02010 	mov	r2, #16	; 0x10
    ab7c:	e2800001 	add	r0, r0, #1	; 0x1
    ab80:	eb000198 	bl	b1e8 <simple_strtoul>
    ab84:	e59f4170 	ldr	r4, [pc, #368]	; acfc <linkpc_loop+0x674>
    ab88:	e59f3170 	ldr	r3, [pc, #368]	; ad00 <linkpc_loop+0x678>
    ab8c:	e1a02000 	mov	r2, r0
    ab90:	e5840000 	str	r0, [r4]
    ab94:	e5931000 	ldr	r1, [r3]
    ab98:	e59f0184 	ldr	r0, [pc, #388]	; ad24 <linkpc_loop+0x69c>
    ab9c:	e1a0e00f 	mov	lr, pc
    aba0:	e59af000 	ldr	pc, [sl]
    aba4:	e59fc154 	ldr	ip, [pc, #340]	; ad00 <linkpc_loop+0x678>
    aba8:	e1a00007 	mov	r0, r7
    abac:	e5943000 	ldr	r3, [r4]
    abb0:	e59f1170 	ldr	r1, [pc, #368]	; ad28 <linkpc_loop+0x6a0>
    abb4:	e59c2000 	ldr	r2, [ip]
    abb8:	eb000516 	bl	c018 <sprintf>
    abbc:	ea000018 	b	ac24 <linkpc_loop+0x59c>
    abc0:	e1a00004 	mov	r0, r4
    abc4:	e59f1160 	ldr	r1, [pc, #352]	; ad2c <linkpc_loop+0x6a4>
    abc8:	e3a0200d 	mov	r2, #13	; 0xd
    abcc:	ebffd944 	bl	10e4 <strncmp>
    abd0:	e3500000 	cmp	r0, #0	; 0x0
    abd4:	1a000006 	bne	abf4 <linkpc_loop+0x56c>
    abd8:	e59f20f0 	ldr	r2, [pc, #240]	; acd0 <linkpc_loop+0x648>
    abdc:	e284100e 	add	r1, r4, #14	; 0xe
    abe0:	e5920008 	ldr	r0, [r2, #8]
    abe4:	ebffd906 	bl	1004 <strcpy>
    abe8:	ebfffe92 	bl	a638 <__response_ok>
    abec:	ebfffea5 	bl	a688 <linkpc_loop>
    abf0:	ea00000b 	b	ac24 <linkpc_loop+0x59c>
    abf4:	e1a00004 	mov	r0, r4
    abf8:	e59f1130 	ldr	r1, [pc, #304]	; ad30 <linkpc_loop+0x6a8>
    abfc:	e3a02007 	mov	r2, #7	; 0x7
    ac00:	ebffd937 	bl	10e4 <strncmp>
    ac04:	e3500000 	cmp	r0, #0	; 0x0
    ac08:	1a000002 	bne	ac18 <linkpc_loop+0x590>
    ac0c:	ebfffe89 	bl	a638 <__response_ok>
    ac10:	ebffecd7 	bl	5f74 <cdbg_shell>
    ac14:	eafffffe 	b	ac14 <linkpc_loop+0x58c>
    ac18:	e59f1114 	ldr	r1, [pc, #276]	; ad34 <linkpc_loop+0x6ac>
    ac1c:	e1a00007 	mov	r0, r7
    ac20:	eb0004fc 	bl	c018 <sprintf>
    ac24:	e3a01c01 	mov	r1, #256	; 0x100
    ac28:	e1a00007 	mov	r0, r7
    ac2c:	ebffd953 	bl	1180 <strnlen>
    ac30:	e1a01000 	mov	r1, r0
    ac34:	e1a00007 	mov	r0, r7
    ac38:	ebfffd8f 	bl	a27c <udc_write>
    ac3c:	e3500000 	cmp	r0, #0	; 0x0
    ac40:	0afffea4 	beq	a6d8 <linkpc_loop+0x50>
    ac44:	e59f00ec 	ldr	r0, [pc, #236]	; ad38 <linkpc_loop+0x6b0>
    ac48:	e59f3028 	ldr	r3, [pc, #40]	; ac78 <linkpc_loop+0x5f0>
    ac4c:	e1a0e00f 	mov	lr, pc
    ac50:	e593f000 	ldr	pc, [r3]
    ac54:	e3a04000 	mov	r4, #0	; 0x0
    ac58:	e59f00dc 	ldr	r0, [pc, #220]	; ad3c <linkpc_loop+0x6b4>
    ac5c:	e59f3014 	ldr	r3, [pc, #20]	; ac78 <linkpc_loop+0x5f0>
    ac60:	e1a0e00f 	mov	lr, pc
    ac64:	e593f000 	ldr	pc, [r3]
    ac68:	e1a00004 	mov	r0, r4
    ac6c:	e28ddf59 	add	sp, sp, #356	; 0x164
    ac70:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ac74:	000114f0 	.word	0x000114f0
    ac78:	3c00d68c 	.word	0x3c00d68c
    ac7c:	0001033a 	.word	0x0001033a
    ac80:	000114ff 	.word	0x000114ff
    ac84:	3c00d20c 	.word	0x3c00d20c
    ac88:	3c00d210 	.word	0x3c00d210
    ac8c:	0000fd95 	.word	0x0000fd95
    ac90:	0000f92e 	.word	0x0000f92e
    ac94:	3c00d214 	.word	0x3c00d214
    ac98:	3c00d218 	.word	0x3c00d218
    ac9c:	00011512 	.word	0x00011512
    aca0:	0000f90e 	.word	0x0000f90e
    aca4:	00011518 	.word	0x00011518
    aca8:	0001153b 	.word	0x0001153b
    acac:	00011540 	.word	0x00011540
    acb0:	00011547 	.word	0x00011547
    acb4:	0000f93a 	.word	0x0000f93a
    acb8:	0001154f 	.word	0x0001154f
    acbc:	00011557 	.word	0x00011557
    acc0:	3c00d09c 	.word	0x3c00d09c
    acc4:	0001155f 	.word	0x0001155f
    acc8:	3c00d118 	.word	0x3c00d118
    accc:	00011565 	.word	0x00011565
    acd0:	3c00d6f8 	.word	0x3c00d6f8
    acd4:	0001156e 	.word	0x0001156e
    acd8:	000115d1 	.word	0x000115d1
    acdc:	0001157b 	.word	0x0001157b
    ace0:	00011585 	.word	0x00011585
    ace4:	000115a4 	.word	0x000115a4
    ace8:	000115d6 	.word	0x000115d6
    acec:	000115bd 	.word	0x000115bd
    acf0:	000115df 	.word	0x000115df
    acf4:	000101bf 	.word	0x000101bf
    acf8:	000115e6 	.word	0x000115e6
    acfc:	3c00d220 	.word	0x3c00d220
    ad00:	3c00d21c 	.word	0x3c00d21c
    ad04:	000115fe 	.word	0x000115fe
    ad08:	0000f947 	.word	0x0000f947
    ad0c:	0001160e 	.word	0x0001160e
    ad10:	00011625 	.word	0x00011625
    ad14:	00011633 	.word	0x00011633
    ad18:	0001164c 	.word	0x0001164c
    ad1c:	00011661 	.word	0x00011661
    ad20:	0001166a 	.word	0x0001166a
    ad24:	00011675 	.word	0x00011675
    ad28:	0001168d 	.word	0x0001168d
    ad2c:	000116a8 	.word	0x000116a8
    ad30:	000116b6 	.word	0x000116b6
    ad34:	000116be 	.word	0x000116be
    ad38:	000116d2 	.word	0x000116d2
    ad3c:	000116f5 	.word	0x000116f5

0000ad40 <module_get_clock>:
    ad40:	e59f3118 	ldr	r3, [pc, #280]	; ae60 <module_get_clock+0x120>
    ad44:	e1500003 	cmp	r0, r3
    ad48:	0a000038 	beq	ae30 <module_get_clock+0xf0>
    ad4c:	8a000018 	bhi	adb4 <module_get_clock+0x74>
    ad50:	e2433050 	sub	r3, r3, #80	; 0x50
    ad54:	e1500003 	cmp	r0, r3
    ad58:	0a000034 	beq	ae30 <module_get_clock+0xf0>
    ad5c:	8a000008 	bhi	ad84 <module_get_clock+0x44>
    ad60:	e3500003 	cmp	r0, #3	; 0x3
    ad64:	059f00f8 	ldreq	r0, [pc, #248]	; ae64 <module_get_clock+0x124>
    ad68:	012fff1e 	bxeq	lr
    ad6c:	e2433010 	sub	r3, r3, #16	; 0x10
    ad70:	e1500003 	cmp	r0, r3
    ad74:	0a00002d 	beq	ae30 <module_get_clock+0xf0>
    ad78:	e3500001 	cmp	r0, #1	; 0x1
    ad7c:	1a00002d 	bne	ae38 <module_get_clock+0xf8>
    ad80:	ea000034 	b	ae58 <module_get_clock+0x118>
    ad84:	e59f30dc 	ldr	r3, [pc, #220]	; ae68 <module_get_clock+0x128>
    ad88:	e1500003 	cmp	r0, r3
    ad8c:	0a000027 	beq	ae30 <module_get_clock+0xf0>
    ad90:	92433010 	subls	r3, r3, #16	; 0x10
    ad94:	9a000003 	bls	ada8 <module_get_clock+0x68>
    ad98:	e59f30cc 	ldr	r3, [pc, #204]	; ae6c <module_get_clock+0x12c>
    ad9c:	e1500003 	cmp	r0, r3
    ada0:	0a000022 	beq	ae30 <module_get_clock+0xf0>
    ada4:	e2833010 	add	r3, r3, #16	; 0x10
    ada8:	e1500003 	cmp	r0, r3
    adac:	1a000021 	bne	ae38 <module_get_clock+0xf8>
    adb0:	ea00001e 	b	ae30 <module_get_clock+0xf0>
    adb4:	e59f30b4 	ldr	r3, [pc, #180]	; ae70 <module_get_clock+0x130>
    adb8:	e1500003 	cmp	r0, r3
    adbc:	059f00b0 	ldreq	r0, [pc, #176]	; ae74 <module_get_clock+0x134>
    adc0:	012fff1e 	bxeq	lr
    adc4:	8a000009 	bhi	adf0 <module_get_clock+0xb0>
    adc8:	e2433040 	sub	r3, r3, #64	; 0x40
    adcc:	e1500003 	cmp	r0, r3
    add0:	0a00001a 	beq	ae40 <module_get_clock+0x100>
    add4:	e2833010 	add	r3, r3, #16	; 0x10
    add8:	e1500003 	cmp	r0, r3
    addc:	0a000017 	beq	ae40 <module_get_clock+0x100>
    ade0:	e2433020 	sub	r3, r3, #32	; 0x20
    ade4:	e1500003 	cmp	r0, r3
    ade8:	1a000012 	bne	ae38 <module_get_clock+0xf8>
    adec:	ea000013 	b	ae40 <module_get_clock+0x100>
    adf0:	e59f3080 	ldr	r3, [pc, #128]	; ae78 <module_get_clock+0x138>
    adf4:	e1500003 	cmp	r0, r3
    adf8:	059f007c 	ldreq	r0, [pc, #124]	; ae7c <module_get_clock+0x13c>
    adfc:	012fff1e 	bxeq	lr
    ae00:	8a000003 	bhi	ae14 <module_get_clock+0xd4>
    ae04:	e2433010 	sub	r3, r3, #16	; 0x10
    ae08:	e1500003 	cmp	r0, r3
    ae0c:	1a000009 	bne	ae38 <module_get_clock+0xf8>
    ae10:	ea00000c 	b	ae48 <module_get_clock+0x108>
    ae14:	e59f3064 	ldr	r3, [pc, #100]	; ae80 <module_get_clock+0x140>
    ae18:	e1500003 	cmp	r0, r3
    ae1c:	0a000003 	beq	ae30 <module_get_clock+0xf0>
    ae20:	e2833040 	add	r3, r3, #64	; 0x40
    ae24:	e1500003 	cmp	r0, r3
    ae28:	1a000002 	bne	ae38 <module_get_clock+0xf8>
    ae2c:	ea000007 	b	ae50 <module_get_clock+0x110>
    ae30:	e59f004c 	ldr	r0, [pc, #76]	; ae84 <module_get_clock+0x144>
    ae34:	e12fff1e 	bx	lr
    ae38:	e3a00000 	mov	r0, #0	; 0x0
    ae3c:	e12fff1e 	bx	lr
    ae40:	e59f0040 	ldr	r0, [pc, #64]	; ae88 <module_get_clock+0x148>
    ae44:	e12fff1e 	bx	lr
    ae48:	e59f003c 	ldr	r0, [pc, #60]	; ae8c <module_get_clock+0x14c>
    ae4c:	e12fff1e 	bx	lr
    ae50:	e59f0038 	ldr	r0, [pc, #56]	; ae90 <module_get_clock+0x150>
    ae54:	e12fff1e 	bx	lr
    ae58:	e59f0034 	ldr	r0, [pc, #52]	; ae94 <module_get_clock+0x154>
    ae5c:	e12fff1e 	bx	lr
    ae60:	21e0a190 	.word	0x21e0a190
    ae64:	052f83c0 	.word	0x052f83c0
    ae68:	21e0a160 	.word	0x21e0a160
    ae6c:	21e0a170 	.word	0x21e0a170
    ae70:	21e0a290 	.word	0x21e0a290
    ae74:	0160203b 	.word	0x0160203b
    ae78:	21e0a2d0 	.word	0x21e0a2d0
    ae7c:	02faf080 	.word	0x02faf080
    ae80:	21e0a320 	.word	0x21e0a320
    ae84:	0bfb0440 	.word	0x0bfb0440
    ae88:	02625a00 	.word	0x02625a00
    ae8c:	016e3600 	.word	0x016e3600
    ae90:	06ea0500 	.word	0x06ea0500
    ae94:	0ff95b00 	.word	0x0ff95b00

0000ae98 <set_mux>:
    ae98:	e59f3004 	ldr	r3, [pc, #4]	; aea4 <set_mux+0xc>
    ae9c:	e5830000 	str	r0, [r3]
    aea0:	e12fff1e 	bx	lr
    aea4:	21e09000 	.word	0x21e09000

0000aea8 <print_size>:
    aea8:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    aeac:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    aeb0:	e1a05000 	mov	r5, r0
    aeb4:	e1a08001 	mov	r8, r1
    aeb8:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    aebc:	83a07047 	movhi	r7, #71	; 0x47
    aec0:	8a000005 	bhi	aedc <print_size+0x34>
    aec4:	e59f30a8 	ldr	r3, [pc, #168]	; af74 <print_size+0xcc>
    aec8:	e1500003 	cmp	r0, r3
    aecc:	83a0704d 	movhi	r7, #77	; 0x4d
    aed0:	93a0706b 	movls	r7, #107	; 0x6b
    aed4:	83a04601 	movhi	r4, #1048576	; 0x100000
    aed8:	93a04b01 	movls	r4, #1024	; 0x400
    aedc:	e1a01004 	mov	r1, r4
    aee0:	e1a00005 	mov	r0, r5
    aee4:	eb000cc3 	bl	e1f8 <__aeabi_uidiv>
    aee8:	e1a01004 	mov	r1, r4
    aeec:	e1a06000 	mov	r6, r0
    aef0:	e1a00005 	mov	r0, r5
    aef4:	eb000d3b 	bl	e3e8 <__aeabi_uidivmod>
    aef8:	e2510000 	subs	r0, r1, #0	; 0x0
    aefc:	01a04000 	moveq	r4, r0
    af00:	0a000009 	beq	af2c <print_size+0x84>
    af04:	e0625694 	mls	r2, r4, r6, r5
    af08:	e1a030a4 	lsr	r3, r4, #1
    af0c:	e3a0000a 	mov	r0, #10	; 0xa
    af10:	e1a01004 	mov	r1, r4
    af14:	e0203290 	mla	r0, r0, r2, r3
    af18:	eb000cb6 	bl	e1f8 <__aeabi_uidiv>
    af1c:	e3500009 	cmp	r0, #9	; 0x9
    af20:	e1a04000 	mov	r4, r0
    af24:	82866001 	addhi	r6, r6, #1	; 0x1
    af28:	8240400a 	subhi	r4, r0, #10	; 0xa
    af2c:	e1a01006 	mov	r1, r6
    af30:	e59f5040 	ldr	r5, [pc, #64]	; af78 <print_size+0xd0>
    af34:	e59f0040 	ldr	r0, [pc, #64]	; af7c <print_size+0xd4>
    af38:	e1a0e00f 	mov	lr, pc
    af3c:	e595f000 	ldr	pc, [r5]
    af40:	e3540000 	cmp	r4, #0	; 0x0
    af44:	0a000003 	beq	af58 <print_size+0xb0>
    af48:	e1a01004 	mov	r1, r4
    af4c:	e59f002c 	ldr	r0, [pc, #44]	; af80 <print_size+0xd8>
    af50:	e1a0e00f 	mov	lr, pc
    af54:	e595f000 	ldr	pc, [r5]
    af58:	e1a01007 	mov	r1, r7
    af5c:	e1a02008 	mov	r2, r8
    af60:	e59f001c 	ldr	r0, [pc, #28]	; af84 <print_size+0xdc>
    af64:	e59f300c 	ldr	r3, [pc, #12]	; af78 <print_size+0xd0>
    af68:	e1a0e00f 	mov	lr, pc
    af6c:	e593f000 	ldr	pc, [r3]
    af70:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    af74:	000fffff 	.word	0x000fffff
    af78:	3c00d68c 	.word	0x3c00d68c
    af7c:	000116fd 	.word	0x000116fd
    af80:	00011702 	.word	0x00011702
    af84:	00011707 	.word	0x00011707

0000af88 <print_buffer>:
    af88:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    af8c:	e24dd044 	sub	sp, sp, #68	; 0x44
    af90:	e59d6068 	ldr	r6, [sp, #104]
    af94:	e1a07002 	mov	r7, r2
    af98:	e0020692 	mul	r2, r2, r6
    af9c:	e3520040 	cmp	r2, #64	; 0x40
    afa0:	e1a09000 	mov	r9, r0
    afa4:	e1a0a001 	mov	sl, r1
    afa8:	e1a08003 	mov	r8, r3
    afac:	9a000003 	bls	afc0 <print_buffer+0x38>
    afb0:	e3a00040 	mov	r0, #64	; 0x40
    afb4:	e1a01007 	mov	r1, r7
    afb8:	eb000c8e 	bl	e1f8 <__aeabi_uidiv>
    afbc:	e1a06000 	mov	r6, r0
    afc0:	e3560000 	cmp	r6, #0	; 0x0
    afc4:	1a00003c 	bne	b0bc <print_buffer+0x134>
    afc8:	e3a00010 	mov	r0, #16	; 0x10
    afcc:	e1a01007 	mov	r1, r7
    afd0:	eb000c88 	bl	e1f8 <__aeabi_uidiv>
    afd4:	e1a06000 	mov	r6, r0
    afd8:	ea000037 	b	b0bc <print_buffer+0x134>
    afdc:	e59f00f0 	ldr	r0, [pc, #240]	; b0d4 <print_buffer+0x14c>
    afe0:	e1a01009 	mov	r1, r9
    afe4:	e1a0e00f 	mov	lr, pc
    afe8:	e59bf000 	ldr	pc, [fp]
    afec:	e1560008 	cmp	r6, r8
    aff0:	21a06008 	movcs	r6, r8
    aff4:	e1a0500a 	mov	r5, sl
    aff8:	e3a04000 	mov	r4, #0	; 0x0
    affc:	ea00000f 	b	b040 <print_buffer+0xb8>
    b000:	e3570004 	cmp	r7, #4	; 0x4
    b004:	05951000 	ldreq	r1, [r5]
    b008:	059f00c8 	ldreq	r0, [pc, #200]	; b0d8 <print_buffer+0x150>
    b00c:	078d1104 	streq	r1, [sp, r4, lsl #2]
    b010:	0a000006 	beq	b030 <print_buffer+0xa8>
    b014:	e3570002 	cmp	r7, #2	; 0x2
    b018:	01d510b0 	ldrheq	r1, [r5]
    b01c:	15d51000 	ldrbne	r1, [r5]
    b020:	01a03084 	lsleq	r3, r4, #1
    b024:	059f00b0 	ldreq	r0, [pc, #176]	; b0dc <print_buffer+0x154>
    b028:	018d10b3 	strheq	r1, [sp, r3]
    b02c:	17cd1004 	strbne	r1, [sp, r4]
    b030:	e1a0e00f 	mov	lr, pc
    b034:	e59bf000 	ldr	pc, [fp]
    b038:	e0855007 	add	r5, r5, r7
    b03c:	e2844001 	add	r4, r4, #1	; 0x1
    b040:	e1540006 	cmp	r4, r6
    b044:	e59f0094 	ldr	r0, [pc, #148]	; b0e0 <print_buffer+0x158>
    b048:	3affffec 	bcc	b000 <print_buffer+0x78>
    b04c:	e0050697 	mul	r5, r7, r6
    b050:	e59f008c 	ldr	r0, [pc, #140]	; b0e4 <print_buffer+0x15c>
    b054:	e08aa005 	add	sl, sl, r5
    b058:	eb000450 	bl	c1a0 <puts>
    b05c:	e3a04000 	mov	r4, #0	; 0x0
    b060:	ea000009 	b	b08c <print_buffer+0x104>
    b064:	e7dd0004 	ldrb	r0, [sp, r4]
    b068:	e59f2078 	ldr	r2, [pc, #120]	; b0e8 <print_buffer+0x160>
    b06c:	e2844001 	add	r4, r4, #1	; 0x1
    b070:	e7d23000 	ldrb	r3, [r2, r0]
    b074:	e3130097 	tst	r3, #151	; 0x97
    b078:	0a000001 	beq	b084 <print_buffer+0xfc>
    b07c:	e3100080 	tst	r0, #128	; 0x80
    b080:	0a000000 	beq	b088 <print_buffer+0x100>
    b084:	e3a0002e 	mov	r0, #46	; 0x2e
    b088:	eb00046c 	bl	c240 <putc>
    b08c:	e1540005 	cmp	r4, r5
    b090:	3afffff3 	bcc	b064 <print_buffer+0xdc>
    b094:	e59f0050 	ldr	r0, [pc, #80]	; b0ec <print_buffer+0x164>
    b098:	e1a0e00f 	mov	lr, pc
    b09c:	e59bf000 	ldr	pc, [fp]
    b0a0:	eb000469 	bl	c24c <ctrlc>
    b0a4:	e3500000 	cmp	r0, #0	; 0x0
    b0a8:	13e00000 	mvnne	r0, #0	; 0x0
    b0ac:	1a000006 	bne	b0cc <print_buffer+0x144>
    b0b0:	e0899005 	add	r9, r9, r5
    b0b4:	e0668008 	rsb	r8, r6, r8
    b0b8:	ea000000 	b	b0c0 <print_buffer+0x138>
    b0bc:	e59fb02c 	ldr	fp, [pc, #44]	; b0f0 <print_buffer+0x168>
    b0c0:	e3580000 	cmp	r8, #0	; 0x0
    b0c4:	1affffc4 	bne	afdc <print_buffer+0x54>
    b0c8:	e1a00008 	mov	r0, r8
    b0cc:	e28dd044 	add	sp, sp, #68	; 0x44
    b0d0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b0d4:	0001170e 	.word	0x0001170e
    b0d8:	0001135d 	.word	0x0001135d
    b0dc:	00011715 	.word	0x00011715
    b0e0:	0001171b 	.word	0x0001171b
    b0e4:	00011721 	.word	0x00011721
    b0e8:	0000f971 	.word	0x0000f971
    b0ec:	000104fe 	.word	0x000104fe
    b0f0:	3c00d68c 	.word	0x3c00d68c

0000b0f4 <__div64_32>:
    b0f4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    b0f8:	e8900300 	ldm	r0, {r8, r9}
    b0fc:	e1510009 	cmp	r1, r9
    b100:	e1a0a000 	mov	sl, r0
    b104:	e1a04001 	mov	r4, r1
    b108:	83a06000 	movhi	r6, #0	; 0x0
    b10c:	83a07000 	movhi	r7, #0	; 0x0
    b110:	8a000007 	bhi	b134 <__div64_32+0x40>
    b114:	e1a00009 	mov	r0, r9
    b118:	eb000c36 	bl	e1f8 <__aeabi_uidiv>
    b11c:	e0030094 	mul	r3, r4, r0
    b120:	e3a06000 	mov	r6, #0	; 0x0
    b124:	e1a02006 	mov	r2, r6
    b128:	e1a07000 	mov	r7, r0
    b12c:	e0588002 	subs	r8, r8, r2
    b130:	e0c99003 	sbc	r9, r9, r3
    b134:	e1a00004 	mov	r0, r4
    b138:	e3a01000 	mov	r1, #0	; 0x0
    b13c:	e3a05000 	mov	r5, #0	; 0x0
    b140:	e3a04001 	mov	r4, #1	; 0x1
    b144:	ea000003 	b	b158 <__div64_32+0x64>
    b148:	e0900000 	adds	r0, r0, r0
    b14c:	e0a11001 	adc	r1, r1, r1
    b150:	e0944004 	adds	r4, r4, r4
    b154:	e0a55005 	adc	r5, r5, r5
    b158:	e3510000 	cmp	r1, #0	; 0x0
    b15c:	e3a03000 	mov	r3, #0	; 0x0
    b160:	ca000002 	bgt	b170 <__div64_32+0x7c>
    b164:	1a000002 	bne	b174 <__div64_32+0x80>
    b168:	e3500000 	cmp	r0, #0	; 0x0
    b16c:	0a000000 	beq	b174 <__div64_32+0x80>
    b170:	e3a03001 	mov	r3, #1	; 0x1
    b174:	e1590001 	cmp	r9, r1
    b178:	e3a02000 	mov	r2, #0	; 0x0
    b17c:	8a000002 	bhi	b18c <__div64_32+0x98>
    b180:	1a000002 	bne	b190 <__div64_32+0x9c>
    b184:	e1580000 	cmp	r8, r0
    b188:	9a000000 	bls	b190 <__div64_32+0x9c>
    b18c:	e3a02001 	mov	r2, #1	; 0x1
    b190:	e0033002 	and	r3, r3, r2
    b194:	e31300ff 	tst	r3, #255	; 0xff
    b198:	1affffea 	bne	b148 <__div64_32+0x54>
    b19c:	e1510009 	cmp	r1, r9
    b1a0:	8a000006 	bhi	b1c0 <__div64_32+0xcc>
    b1a4:	1a000001 	bne	b1b0 <__div64_32+0xbc>
    b1a8:	e1500008 	cmp	r0, r8
    b1ac:	8a000003 	bhi	b1c0 <__div64_32+0xcc>
    b1b0:	e0588000 	subs	r8, r8, r0
    b1b4:	e0c99001 	sbc	r9, r9, r1
    b1b8:	e0966004 	adds	r6, r6, r4
    b1bc:	e0a77005 	adc	r7, r7, r5
    b1c0:	e1b050a5 	lsrs	r5, r5, #1
    b1c4:	e1a04064 	rrx	r4, r4
    b1c8:	e1943005 	orrs	r3, r4, r5
    b1cc:	0a000002 	beq	b1dc <__div64_32+0xe8>
    b1d0:	e1b010a1 	lsrs	r1, r1, #1
    b1d4:	e1a00060 	rrx	r0, r0
    b1d8:	eaffffef 	b	b19c <__div64_32+0xa8>
    b1dc:	e88a00c0 	stm	sl, {r6, r7}
    b1e0:	e1a00008 	mov	r0, r8
    b1e4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000b1e8 <simple_strtoul>:
    b1e8:	e92d4070 	push	{r4, r5, r6, lr}
    b1ec:	e5d03000 	ldrb	r3, [r0]
    b1f0:	e1a05001 	mov	r5, r1
    b1f4:	e3530030 	cmp	r3, #48	; 0x30
    b1f8:	e1a01002 	mov	r1, r2
    b1fc:	1a00000d 	bne	b238 <simple_strtoul+0x50>
    b200:	e5f03001 	ldrb	r3, [r0, #1]!
    b204:	e3530078 	cmp	r3, #120	; 0x78
    b208:	1a000007 	bne	b22c <simple_strtoul+0x44>
    b20c:	e5d03001 	ldrb	r3, [r0, #1]
    b210:	e59f2084 	ldr	r2, [pc, #132]	; b29c <simple_strtoul+0xb4>
    b214:	e280c001 	add	ip, r0, #1	; 0x1
    b218:	e7d23003 	ldrb	r3, [r2, r3]
    b21c:	e3130044 	tst	r3, #68	; 0x44
    b220:	11a0000c 	movne	r0, ip
    b224:	13a01010 	movne	r1, #16	; 0x10
    b228:	1a000004 	bne	b240 <simple_strtoul+0x58>
    b22c:	e3510000 	cmp	r1, #0	; 0x0
    b230:	03a01008 	moveq	r1, #8	; 0x8
    b234:	ea000001 	b	b240 <simple_strtoul+0x58>
    b238:	e3520000 	cmp	r2, #0	; 0x0
    b23c:	03a0100a 	moveq	r1, #10	; 0xa
    b240:	e3a04000 	mov	r4, #0	; 0x0
    b244:	e59f6050 	ldr	r6, [pc, #80]	; b29c <simple_strtoul+0xb4>
    b248:	ea000001 	b	b254 <simple_strtoul+0x6c>
    b24c:	e0243491 	mla	r4, r1, r4, r3
    b250:	e2800001 	add	r0, r0, #1	; 0x1
    b254:	e5d0c000 	ldrb	ip, [r0]
    b258:	e7d6200c 	ldrb	r2, [r6, ip]
    b25c:	e24c3030 	sub	r3, ip, #48	; 0x30
    b260:	e3120044 	tst	r2, #68	; 0x44
    b264:	0a000008 	beq	b28c <simple_strtoul+0xa4>
    b268:	e3120004 	tst	r2, #4	; 0x4
    b26c:	1a000004 	bne	b284 <simple_strtoul+0x9c>
    b270:	e24c3020 	sub	r3, ip, #32	; 0x20
    b274:	e3120002 	tst	r2, #2	; 0x2
    b278:	e6ef3073 	uxtb	r3, r3
    b27c:	12433037 	subne	r3, r3, #55	; 0x37
    b280:	024c3037 	subeq	r3, ip, #55	; 0x37
    b284:	e1530001 	cmp	r3, r1
    b288:	3affffef 	bcc	b24c <simple_strtoul+0x64>
    b28c:	e3550000 	cmp	r5, #0	; 0x0
    b290:	15850000 	strne	r0, [r5]
    b294:	e1a00004 	mov	r0, r4
    b298:	e8bd8070 	pop	{r4, r5, r6, pc}
    b29c:	0000f971 	.word	0x0000f971

0000b2a0 <simple_strtol>:
    b2a0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    b2a4:	e5d03000 	ldrb	r3, [r0]
    b2a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    b2ac:	e353002d 	cmp	r3, #45	; 0x2d
    b2b0:	1a000004 	bne	b2c8 <simple_strtol+0x28>
    b2b4:	e2800001 	add	r0, r0, #1	; 0x1
    b2b8:	ebffffca 	bl	b1e8 <simple_strtoul>
    b2bc:	e2600000 	rsb	r0, r0, #0	; 0x0
    b2c0:	e28dd004 	add	sp, sp, #4	; 0x4
    b2c4:	e8bd8000 	pop	{pc}
    b2c8:	e28dd004 	add	sp, sp, #4	; 0x4
    b2cc:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    b2d0:	eaffffc4 	b	b1e8 <simple_strtoul>

0000b2d4 <ustrtoul>:
    b2d4:	e92d4010 	push	{r4, lr}
    b2d8:	e1a04001 	mov	r4, r1
    b2dc:	ebffffc1 	bl	b1e8 <simple_strtoul>
    b2e0:	e5941000 	ldr	r1, [r4]
    b2e4:	e5d13000 	ldrb	r3, [r1]
    b2e8:	e353004b 	cmp	r3, #75	; 0x4b
    b2ec:	0a00000a 	beq	b31c <ustrtoul+0x48>
    b2f0:	8a000002 	bhi	b300 <ustrtoul+0x2c>
    b2f4:	e3530047 	cmp	r3, #71	; 0x47
    b2f8:	18bd8010 	popne	{r4, pc}
    b2fc:	ea000004 	b	b314 <ustrtoul+0x40>
    b300:	e353004d 	cmp	r3, #77	; 0x4d
    b304:	0a000003 	beq	b318 <ustrtoul+0x44>
    b308:	e353006b 	cmp	r3, #107	; 0x6b
    b30c:	18bd8010 	popne	{r4, pc}
    b310:	ea000001 	b	b31c <ustrtoul+0x48>
    b314:	e1a00500 	lsl	r0, r0, #10
    b318:	e1a00500 	lsl	r0, r0, #10
    b31c:	e5d13001 	ldrb	r3, [r1, #1]
    b320:	e1a00500 	lsl	r0, r0, #10
    b324:	e3530069 	cmp	r3, #105	; 0x69
    b328:	18bd8010 	popne	{r4, pc}
    b32c:	e5d13002 	ldrb	r3, [r1, #2]
    b330:	e2812002 	add	r2, r1, #2	; 0x2
    b334:	e3530042 	cmp	r3, #66	; 0x42
    b338:	02813003 	addeq	r3, r1, #3	; 0x3
    b33c:	05843000 	streq	r3, [r4]
    b340:	15842000 	strne	r2, [r4]
    b344:	e8bd8010 	pop	{r4, pc}

0000b348 <simple_strtoull>:
    b348:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    b34c:	e5d03000 	ldrb	r3, [r0]
    b350:	e1a07000 	mov	r7, r0
    b354:	e3530030 	cmp	r3, #48	; 0x30
    b358:	e1a08001 	mov	r8, r1
    b35c:	e1a06002 	mov	r6, r2
    b360:	1a00000d 	bne	b39c <simple_strtoull+0x54>
    b364:	e5f73001 	ldrb	r3, [r7, #1]!
    b368:	e3530078 	cmp	r3, #120	; 0x78
    b36c:	1a000007 	bne	b390 <simple_strtoull+0x48>
    b370:	e5d73001 	ldrb	r3, [r7, #1]
    b374:	e59f20ac 	ldr	r2, [pc, #172]	; b428 <simple_strtoull+0xe0>
    b378:	e2870001 	add	r0, r7, #1	; 0x1
    b37c:	e7d23003 	ldrb	r3, [r2, r3]
    b380:	e3130044 	tst	r3, #68	; 0x44
    b384:	11a07000 	movne	r7, r0
    b388:	13a06010 	movne	r6, #16	; 0x10
    b38c:	1a000004 	bne	b3a4 <simple_strtoull+0x5c>
    b390:	e3560000 	cmp	r6, #0	; 0x0
    b394:	03a06008 	moveq	r6, #8	; 0x8
    b398:	ea000001 	b	b3a4 <simple_strtoull+0x5c>
    b39c:	e3520000 	cmp	r2, #0	; 0x0
    b3a0:	03a0600a 	moveq	r6, #10	; 0xa
    b3a4:	e3a04000 	mov	r4, #0	; 0x0
    b3a8:	e3a05000 	mov	r5, #0	; 0x0
    b3ac:	e59f9074 	ldr	r9, [pc, #116]	; b428 <simple_strtoull+0xe0>
    b3b0:	e3a0a000 	mov	sl, #0	; 0x0
    b3b4:	ea000004 	b	b3cc <simple_strtoull+0x84>
    b3b8:	e0832694 	umull	r2, r3, r4, r6
    b3bc:	e0233596 	mla	r3, r6, r5, r3
    b3c0:	e0904002 	adds	r4, r0, r2
    b3c4:	e0a15003 	adc	r5, r1, r3
    b3c8:	e2877001 	add	r7, r7, #1	; 0x1
    b3cc:	e5d7c000 	ldrb	ip, [r7]
    b3d0:	e7d9200c 	ldrb	r2, [r9, ip]
    b3d4:	e24c3030 	sub	r3, ip, #48	; 0x30
    b3d8:	e3120044 	tst	r2, #68	; 0x44
    b3dc:	0a00000c 	beq	b414 <simple_strtoull+0xcc>
    b3e0:	e3120004 	tst	r2, #4	; 0x4
    b3e4:	1a000004 	bne	b3fc <simple_strtoull+0xb4>
    b3e8:	e24c3020 	sub	r3, ip, #32	; 0x20
    b3ec:	e3120002 	tst	r2, #2	; 0x2
    b3f0:	e6ef3073 	uxtb	r3, r3
    b3f4:	12433037 	subne	r3, r3, #55	; 0x37
    b3f8:	024c3037 	subeq	r3, ip, #55	; 0x37
    b3fc:	e1a00003 	mov	r0, r3
    b400:	e1a01fc0 	asr	r1, r0, #31
    b404:	e15a0001 	cmp	sl, r1
    b408:	1a000001 	bne	b414 <simple_strtoull+0xcc>
    b40c:	e1560000 	cmp	r6, r0
    b410:	8affffe8 	bhi	b3b8 <simple_strtoull+0x70>
    b414:	e3580000 	cmp	r8, #0	; 0x0
    b418:	15887000 	strne	r7, [r8]
    b41c:	e1a01005 	mov	r1, r5
    b420:	e1a00004 	mov	r0, r4
    b424:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    b428:	0000f971 	.word	0x0000f971

0000b42c <put_dec_trunc>:
    b42c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    b430:	e1a03421 	lsr	r3, r1, #8
    b434:	e1a04221 	lsr	r4, r1, #4
    b438:	e204400f 	and	r4, r4, #15	; 0xf
    b43c:	e203600f 	and	r6, r3, #15	; 0xf
    b440:	e1a05621 	lsr	r5, r1, #12
    b444:	e0842006 	add	r2, r4, r6
    b448:	e201100f 	and	r1, r1, #15	; 0xf
    b44c:	e3a03006 	mov	r3, #6	; 0x6
    b450:	e0822005 	add	r2, r2, r5
    b454:	e0221293 	mla	r2, r3, r2, r1
    b458:	e3a080cd 	mov	r8, #205	; 0xcd
    b45c:	e00c0298 	mul	ip, r8, r2
    b460:	e3a01009 	mov	r1, #9	; 0x9
    b464:	e0214591 	mla	r1, r1, r5, r4
    b468:	e1a0c5ac 	lsr	ip, ip, #11
    b46c:	e3a0700a 	mov	r7, #10	; 0xa
    b470:	e3a03005 	mov	r3, #5	; 0x5
    b474:	e0231693 	mla	r3, r3, r6, r1
    b478:	e0622c97 	mls	r2, r7, ip, r2
    b47c:	e093100c 	adds	r1, r3, ip
    b480:	e2822030 	add	r2, r2, #48	; 0x30
    b484:	e1a04000 	mov	r4, r0
    b488:	e4c02001 	strb	r2, [r0], #1
    b48c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    b490:	e0030198 	mul	r3, r8, r1
    b494:	e1a035a3 	lsr	r3, r3, #11
    b498:	e0621397 	mls	r2, r7, r3, r1
    b49c:	e0831086 	add	r1, r3, r6, lsl #1
    b4a0:	e2822030 	add	r2, r2, #48	; 0x30
    b4a4:	e1913005 	orrs	r3, r1, r5
    b4a8:	e5c42001 	strb	r2, [r4, #1]
    b4ac:	e2800001 	add	r0, r0, #1	; 0x1
    b4b0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    b4b4:	e3a0300d 	mov	r3, #13	; 0xd
    b4b8:	e0030193 	mul	r3, r3, r1
    b4bc:	e1a033a3 	lsr	r3, r3, #7
    b4c0:	e0621397 	mls	r2, r7, r3, r1
    b4c4:	e0931105 	adds	r1, r3, r5, lsl #2
    b4c8:	e2822030 	add	r2, r2, #48	; 0x30
    b4cc:	e4c02001 	strb	r2, [r0], #1
    b4d0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    b4d4:	e0030198 	mul	r3, r8, r1
    b4d8:	e1a0c5a3 	lsr	ip, r3, #11
    b4dc:	e0631c97 	mls	r3, r7, ip, r1
    b4e0:	e35c0000 	cmp	ip, #0	; 0x0
    b4e4:	e2833030 	add	r3, r3, #48	; 0x30
    b4e8:	e4c03001 	strb	r3, [r0], #1
    b4ec:	128c3030 	addne	r3, ip, #48	; 0x30
    b4f0:	14c03001 	strbne	r3, [r0], #1
    b4f4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000b4f8 <string>:
    b4f8:	e59fc0a4 	ldr	ip, [pc, #164]	; b5a4 <string+0xac>
    b4fc:	e3510000 	cmp	r1, #0	; 0x0
    b500:	e92d4070 	push	{r4, r5, r6, lr}
    b504:	11a06001 	movne	r6, r1
    b508:	01a0600c 	moveq	r6, ip
    b50c:	e1a01003 	mov	r1, r3
    b510:	e1a04000 	mov	r4, r0
    b514:	e1a00006 	mov	r0, r6
    b518:	e1a05002 	mov	r5, r2
    b51c:	ebffd717 	bl	1180 <strnlen>
    b520:	e59d3010 	ldr	r3, [sp, #16]
    b524:	e1a0c000 	mov	ip, r0
    b528:	e3130010 	tst	r3, #16	; 0x10
    b52c:	01a02004 	moveq	r2, r4
    b530:	03a01020 	moveq	r1, #32	; 0x20
    b534:	0a000001 	beq	b540 <string+0x48>
    b538:	ea000006 	b	b558 <string+0x60>
    b53c:	e5421001 	strb	r1, [r2, #-1]
    b540:	e2455001 	sub	r5, r5, #1	; 0x1
    b544:	e2853001 	add	r3, r5, #1	; 0x1
    b548:	e15c0003 	cmp	ip, r3
    b54c:	e1a04002 	mov	r4, r2
    b550:	e2822001 	add	r2, r2, #1	; 0x1
    b554:	bafffff8 	blt	b53c <string+0x44>
    b558:	e3a02000 	mov	r2, #0	; 0x0
    b55c:	ea000002 	b	b56c <string+0x74>
    b560:	e7d63002 	ldrb	r3, [r6, r2]
    b564:	e7c43002 	strb	r3, [r4, r2]
    b568:	e2822001 	add	r2, r2, #1	; 0x1
    b56c:	e152000c 	cmp	r2, ip
    b570:	e0843002 	add	r3, r4, r2
    b574:	bafffff9 	blt	b560 <string+0x68>
    b578:	e1a01003 	mov	r1, r3
    b57c:	e1a02005 	mov	r2, r5
    b580:	e3a03020 	mov	r3, #32	; 0x20
    b584:	ea000000 	b	b58c <string+0x94>
    b588:	e5413001 	strb	r3, [r1, #-1]
    b58c:	e15c0002 	cmp	ip, r2
    b590:	e1a00001 	mov	r0, r1
    b594:	e2422001 	sub	r2, r2, #1	; 0x1
    b598:	e2811001 	add	r1, r1, #1	; 0x1
    b59c:	bafffff9 	blt	b588 <string+0x90>
    b5a0:	e8bd8070 	pop	{r4, r5, r6, pc}
    b5a4:	00011726 	.word	0x00011726

0000b5a8 <put_dec>:
    b5a8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b5ac:	e24dd00c 	sub	sp, sp, #12	; 0xc
    b5b0:	e28d8008 	add	r8, sp, #8	; 0x8
    b5b4:	e16820f8 	strd	r2, [r8, #-8]!
    b5b8:	e1a07000 	mov	r7, r0
    b5bc:	e3a090cd 	mov	r9, #205	; 0xcd
    b5c0:	e3a0a00a 	mov	sl, #10	; 0xa
    b5c4:	e3a0b00d 	mov	fp, #13	; 0xd
    b5c8:	e59d5004 	ldr	r5, [sp, #4]
    b5cc:	e1a0000d 	mov	r0, sp
    b5d0:	e3550000 	cmp	r5, #0	; 0x0
    b5d4:	e59f10f0 	ldr	r1, [pc, #240]	; b6cc <put_dec+0x124>
    b5d8:	e59d4000 	ldr	r4, [sp]
    b5dc:	1a00000f 	bne	b620 <put_dec+0x78>
    b5e0:	e59f30e8 	ldr	r3, [pc, #232]	; b6d0 <put_dec+0x128>
    b5e4:	e1a00004 	mov	r0, r4
    b5e8:	e1540003 	cmp	r4, r3
    b5ec:	8a000004 	bhi	b604 <put_dec+0x5c>
    b5f0:	e1a00007 	mov	r0, r7
    b5f4:	e1a01004 	mov	r1, r4
    b5f8:	ebffff8b 	bl	b42c <put_dec_trunc>
    b5fc:	e28dd00c 	add	sp, sp, #12	; 0xc
    b600:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b604:	eb000b77 	bl	e3e8 <__aeabi_uidivmod>
    b608:	e1a00004 	mov	r0, r4
    b60c:	e1a06001 	mov	r6, r1
    b610:	e59f10b4 	ldr	r1, [pc, #180]	; b6cc <put_dec+0x124>
    b614:	eb000af7 	bl	e1f8 <__aeabi_uidiv>
    b618:	e88d0021 	stm	sp, {r0, r5}
    b61c:	ea000001 	b	b628 <put_dec+0x80>
    b620:	ebfffeb3 	bl	b0f4 <__div64_32>
    b624:	e1a06000 	mov	r6, r0
    b628:	e1a02226 	lsr	r2, r6, #4
    b62c:	e1a0c426 	lsr	ip, r6, #8
    b630:	e20cc00f 	and	ip, ip, #15	; 0xf
    b634:	e202200f 	and	r2, r2, #15	; 0xf
    b638:	e1a0e626 	lsr	lr, r6, #12
    b63c:	e082500c 	add	r5, r2, ip
    b640:	e085500e 	add	r5, r5, lr
    b644:	e206300f 	and	r3, r6, #15	; 0xf
    b648:	e3a01006 	mov	r1, #6	; 0x6
    b64c:	e0253591 	mla	r5, r1, r5, r3
    b650:	e3a03009 	mov	r3, #9	; 0x9
    b654:	e0222e93 	mla	r2, r3, lr, r2
    b658:	e3a01005 	mov	r1, #5	; 0x5
    b65c:	e0000599 	mul	r0, r9, r5
    b660:	e0222c91 	mla	r2, r1, ip, r2
    b664:	e1a005a0 	lsr	r0, r0, #11
    b668:	e0822000 	add	r2, r2, r0
    b66c:	e0010299 	mul	r1, r9, r2
    b670:	e1a015a1 	lsr	r1, r1, #11
    b674:	e081c08c 	add	ip, r1, ip, lsl #1
    b678:	e0030c9b 	mul	r3, fp, ip
    b67c:	e1a033a3 	lsr	r3, r3, #7
    b680:	e083e10e 	add	lr, r3, lr, lsl #2
    b684:	e0040e99 	mul	r4, r9, lr
    b688:	e1a045a4 	lsr	r4, r4, #11
    b68c:	e06ee49a 	mls	lr, sl, r4, lr
    b690:	e060509a 	mls	r0, sl, r0, r5
    b694:	e061219a 	mls	r1, sl, r1, r2
    b698:	e063c39a 	mls	r3, sl, r3, ip
    b69c:	e2800030 	add	r0, r0, #48	; 0x30
    b6a0:	e2811030 	add	r1, r1, #48	; 0x30
    b6a4:	e2833030 	add	r3, r3, #48	; 0x30
    b6a8:	e28ee030 	add	lr, lr, #48	; 0x30
    b6ac:	e2844030 	add	r4, r4, #48	; 0x30
    b6b0:	e5c70000 	strb	r0, [r7]
    b6b4:	e5c71001 	strb	r1, [r7, #1]
    b6b8:	e5c73002 	strb	r3, [r7, #2]
    b6bc:	e5c7e003 	strb	lr, [r7, #3]
    b6c0:	e5c74004 	strb	r4, [r7, #4]
    b6c4:	e2877005 	add	r7, r7, #5	; 0x5
    b6c8:	eaffffbe 	b	b5c8 <put_dec+0x20>
    b6cc:	000186a0 	.word	0x000186a0
    b6d0:	0001869f 	.word	0x0001869f

0000b6d4 <number>:
    b6d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b6d8:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    b6dc:	e1a0a000 	mov	sl, r0
    b6e0:	e59d0080 	ldr	r0, [sp, #128]
    b6e4:	e59d808c 	ldr	r8, [sp, #140]
    b6e8:	e250100a 	subs	r1, r0, #10	; 0xa
    b6ec:	13a01001 	movne	r1, #1	; 0x1
    b6f0:	e0011328 	and	r1, r1, r8, lsr #6
    b6f4:	e3180010 	tst	r8, #16	; 0x10
    b6f8:	e58d1008 	str	r1, [sp, #8]
    b6fc:	e2081020 	and	r1, r8, #32	; 0x20
    b700:	13c88001 	bicne	r8, r8, #1	; 0x1
    b704:	e3180002 	tst	r8, #2	; 0x2
    b708:	e1a06002 	mov	r6, r2
    b70c:	e1a07003 	mov	r7, r3
    b710:	e59d5084 	ldr	r5, [sp, #132]
    b714:	e58d1004 	str	r1, [sp, #4]
    b718:	0a000010 	beq	b760 <number+0x8c>
    b71c:	e3530000 	cmp	r3, #0	; 0x0
    b720:	ba000002 	blt	b730 <number+0x5c>
    b724:	e3180004 	tst	r8, #4	; 0x4
    b728:	0a000008 	beq	b750 <number+0x7c>
    b72c:	ea000004 	b	b744 <number+0x70>
    b730:	e2766000 	rsbs	r6, r6, #0	; 0x0
    b734:	e2e77000 	rsc	r7, r7, #0	; 0x0
    b738:	e2455001 	sub	r5, r5, #1	; 0x1
    b73c:	e3a0902d 	mov	r9, #45	; 0x2d
    b740:	ea000007 	b	b764 <number+0x90>
    b744:	e2455001 	sub	r5, r5, #1	; 0x1
    b748:	e3a0902b 	mov	r9, #43	; 0x2b
    b74c:	ea000004 	b	b764 <number+0x90>
    b750:	e3180008 	tst	r8, #8	; 0x8
    b754:	12455001 	subne	r5, r5, #1	; 0x1
    b758:	13a09020 	movne	r9, #32	; 0x20
    b75c:	1a000000 	bne	b764 <number+0x90>
    b760:	e3a09000 	mov	r9, #0	; 0x0
    b764:	e59d2008 	ldr	r2, [sp, #8]
    b768:	e3520000 	cmp	r2, #0	; 0x0
    b76c:	0a000003 	beq	b780 <number+0xac>
    b770:	e59d3080 	ldr	r3, [sp, #128]
    b774:	e2455001 	sub	r5, r5, #1	; 0x1
    b778:	e3530010 	cmp	r3, #16	; 0x10
    b77c:	02455001 	subeq	r5, r5, #1	; 0x1
    b780:	e196c007 	orrs	ip, r6, r7
    b784:	03a03030 	moveq	r3, #48	; 0x30
    b788:	05cd3016 	strbeq	r3, [sp, #22]
    b78c:	03a04001 	moveq	r4, #1	; 0x1
    b790:	0a000020 	beq	b818 <number+0x144>
    b794:	e59d0080 	ldr	r0, [sp, #128]
    b798:	e350000a 	cmp	r0, #10	; 0xa
    b79c:	0a000017 	beq	b800 <number+0x12c>
    b7a0:	e3500010 	cmp	r0, #16	; 0x10
    b7a4:	03a01004 	moveq	r1, #4	; 0x4
    b7a8:	13a01003 	movne	r1, #3	; 0x3
    b7ac:	e58d100c 	str	r1, [sp, #12]
    b7b0:	e240b001 	sub	fp, r0, #1	; 0x1
    b7b4:	e3a04000 	mov	r4, #0	; 0x0
    b7b8:	e6ef3076 	uxtb	r3, r6
    b7bc:	e59f2164 	ldr	r2, [pc, #356]	; b928 <number+0x254>
    b7c0:	e003300b 	and	r3, r3, fp
    b7c4:	e59dc004 	ldr	ip, [sp, #4]
    b7c8:	e7d23003 	ldrb	r3, [r2, r3]
    b7cc:	e1a00006 	mov	r0, r6
    b7d0:	e18c3003 	orr	r3, ip, r3
    b7d4:	e28dc016 	add	ip, sp, #22	; 0x16
    b7d8:	e1a01007 	mov	r1, r7
    b7dc:	e59d200c 	ldr	r2, [sp, #12]
    b7e0:	e7cc3004 	strb	r3, [ip, r4]
    b7e4:	eb000b93 	bl	e638 <__aeabi_llsr>
    b7e8:	e1a06000 	mov	r6, r0
    b7ec:	e1960001 	orrs	r0, r6, r1
    b7f0:	e1a07001 	mov	r7, r1
    b7f4:	e2844001 	add	r4, r4, #1	; 0x1
    b7f8:	1affffee 	bne	b7b8 <number+0xe4>
    b7fc:	ea000005 	b	b818 <number+0x144>
    b800:	e28d4016 	add	r4, sp, #22	; 0x16
    b804:	e1a00004 	mov	r0, r4
    b808:	e1a02006 	mov	r2, r6
    b80c:	e1a03007 	mov	r3, r7
    b810:	ebffff64 	bl	b5a8 <put_dec>
    b814:	e0644000 	rsb	r4, r4, r0
    b818:	e59d3088 	ldr	r3, [sp, #136]
    b81c:	e1540003 	cmp	r4, r3
    b820:	a1a01004 	movge	r1, r4
    b824:	b1a01003 	movlt	r1, r3
    b828:	e3180011 	tst	r8, #17	; 0x11
    b82c:	e061c005 	rsb	ip, r1, r5
    b830:	01a0300a 	moveq	r3, sl
    b834:	03a02020 	moveq	r2, #32	; 0x20
    b838:	0a000001 	beq	b844 <number+0x170>
    b83c:	ea000004 	b	b854 <number+0x180>
    b840:	e5432001 	strb	r2, [r3, #-1]
    b844:	e25cc001 	subs	ip, ip, #1	; 0x1
    b848:	e1a0a003 	mov	sl, r3
    b84c:	e2833001 	add	r3, r3, #1	; 0x1
    b850:	5afffffa 	bpl	b840 <number+0x16c>
    b854:	e3590000 	cmp	r9, #0	; 0x0
    b858:	14ca9001 	strbne	r9, [sl], #1
    b85c:	e59d2008 	ldr	r2, [sp, #8]
    b860:	e3520000 	cmp	r2, #0	; 0x0
    b864:	0a000006 	beq	b884 <number+0x1b0>
    b868:	e59d3080 	ldr	r3, [sp, #128]
    b86c:	e3530010 	cmp	r3, #16	; 0x10
    b870:	e3a03030 	mov	r3, #48	; 0x30
    b874:	e4ca3001 	strb	r3, [sl], #1
    b878:	059d0004 	ldreq	r0, [sp, #4]
    b87c:	03803058 	orreq	r3, r0, #88	; 0x58
    b880:	04ca3001 	strbeq	r3, [sl], #1
    b884:	e3180010 	tst	r8, #16	; 0x10
    b888:	1a000009 	bne	b8b4 <number+0x1e0>
    b88c:	e3180001 	tst	r8, #1	; 0x1
    b890:	13a02030 	movne	r2, #48	; 0x30
    b894:	03a02020 	moveq	r2, #32	; 0x20
    b898:	e1a0300a 	mov	r3, sl
    b89c:	ea000000 	b	b8a4 <number+0x1d0>
    b8a0:	e5432001 	strb	r2, [r3, #-1]
    b8a4:	e25cc001 	subs	ip, ip, #1	; 0x1
    b8a8:	e1a0a003 	mov	sl, r3
    b8ac:	e2833001 	add	r3, r3, #1	; 0x1
    b8b0:	5afffffa 	bpl	b8a0 <number+0x1cc>
    b8b4:	e1a0000a 	mov	r0, sl
    b8b8:	e3a03030 	mov	r3, #48	; 0x30
    b8bc:	ea000000 	b	b8c4 <number+0x1f0>
    b8c0:	e5403001 	strb	r3, [r0, #-1]
    b8c4:	e2411001 	sub	r1, r1, #1	; 0x1
    b8c8:	e1540001 	cmp	r4, r1
    b8cc:	e1a02000 	mov	r2, r0
    b8d0:	e2800001 	add	r0, r0, #1	; 0x1
    b8d4:	dafffff9 	ble	b8c0 <number+0x1ec>
    b8d8:	e28d1058 	add	r1, sp, #88	; 0x58
    b8dc:	e0813004 	add	r3, r1, r4
    b8e0:	e2431043 	sub	r1, r3, #67	; 0x43
    b8e4:	ea000001 	b	b8f0 <number+0x21c>
    b8e8:	e5d13001 	ldrb	r3, [r1, #1]
    b8ec:	e5423001 	strb	r3, [r2, #-1]
    b8f0:	e2544001 	subs	r4, r4, #1	; 0x1
    b8f4:	e1a03002 	mov	r3, r2
    b8f8:	e2411001 	sub	r1, r1, #1	; 0x1
    b8fc:	e2822001 	add	r2, r2, #1	; 0x1
    b900:	5afffff8 	bpl	b8e8 <number+0x214>
    b904:	e3a02020 	mov	r2, #32	; 0x20
    b908:	ea000000 	b	b910 <number+0x23c>
    b90c:	e5432001 	strb	r2, [r3, #-1]
    b910:	e25cc001 	subs	ip, ip, #1	; 0x1
    b914:	e1a00003 	mov	r0, r3
    b918:	e2833001 	add	r3, r3, #1	; 0x1
    b91c:	5afffffa 	bpl	b90c <number+0x238>
    b920:	e28dd05c 	add	sp, sp, #92	; 0x5c
    b924:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b928:	0000f961 	.word	0x0000f961

0000b92c <vsprintf>:
    b92c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b930:	e24dd084 	sub	sp, sp, #132	; 0x84
    b934:	e58d0028 	str	r0, [sp, #40]
    b938:	e1a08001 	mov	r8, r1
    b93c:	e1a0a000 	mov	sl, r0
    b940:	e28d107b 	add	r1, sp, #123	; 0x7b
    b944:	e28d007d 	add	r0, sp, #125	; 0x7d
    b948:	e58d2024 	str	r2, [sp, #36]
    b94c:	e1a06002 	mov	r6, r2
    b950:	e58d001c 	str	r0, [sp, #28]
    b954:	e58d1018 	str	r1, [sp, #24]
    b958:	ea0001a3 	b	bfec <vsprintf+0x6c0>
    b95c:	e3530025 	cmp	r3, #37	; 0x25
    b960:	1a00015f 	bne	bee4 <vsprintf+0x5b8>
    b964:	e3a07000 	mov	r7, #0	; 0x0
    b968:	e5f82001 	ldrb	r2, [r8, #1]!
    b96c:	e352002b 	cmp	r2, #43	; 0x2b
    b970:	0a00000c 	beq	b9a8 <vsprintf+0x7c>
    b974:	8a000004 	bhi	b98c <vsprintf+0x60>
    b978:	e3520020 	cmp	r2, #32	; 0x20
    b97c:	0a00000b 	beq	b9b0 <vsprintf+0x84>
    b980:	e3520023 	cmp	r2, #35	; 0x23
    b984:	1a00000f 	bne	b9c8 <vsprintf+0x9c>
    b988:	ea00000a 	b	b9b8 <vsprintf+0x8c>
    b98c:	e352002d 	cmp	r2, #45	; 0x2d
    b990:	0a000002 	beq	b9a0 <vsprintf+0x74>
    b994:	e3520030 	cmp	r2, #48	; 0x30
    b998:	1a00000a 	bne	b9c8 <vsprintf+0x9c>
    b99c:	ea000007 	b	b9c0 <vsprintf+0x94>
    b9a0:	e3877010 	orr	r7, r7, #16	; 0x10
    b9a4:	eaffffef 	b	b968 <vsprintf+0x3c>
    b9a8:	e3877004 	orr	r7, r7, #4	; 0x4
    b9ac:	eaffffed 	b	b968 <vsprintf+0x3c>
    b9b0:	e3877008 	orr	r7, r7, #8	; 0x8
    b9b4:	eaffffeb 	b	b968 <vsprintf+0x3c>
    b9b8:	e3877040 	orr	r7, r7, #64	; 0x40
    b9bc:	eaffffe9 	b	b968 <vsprintf+0x3c>
    b9c0:	e3877001 	orr	r7, r7, #1	; 0x1
    b9c4:	eaffffe7 	b	b968 <vsprintf+0x3c>
    b9c8:	e2423030 	sub	r3, r2, #48	; 0x30
    b9cc:	e6ef3073 	uxtb	r3, r3
    b9d0:	e3530009 	cmp	r3, #9	; 0x9
    b9d4:	93a09000 	movls	r9, #0	; 0x0
    b9d8:	93a0100a 	movls	r1, #10	; 0xa
    b9dc:	9a000002 	bls	b9ec <vsprintf+0xc0>
    b9e0:	ea000007 	b	ba04 <vsprintf+0xd8>
    b9e4:	e0292991 	mla	r9, r1, r9, r2
    b9e8:	e2888001 	add	r8, r8, #1	; 0x1
    b9ec:	e5d83000 	ldrb	r3, [r8]
    b9f0:	e2432030 	sub	r2, r3, #48	; 0x30
    b9f4:	e6ef3072 	uxtb	r3, r2
    b9f8:	e3530009 	cmp	r3, #9	; 0x9
    b9fc:	9afffff8 	bls	b9e4 <vsprintf+0xb8>
    ba00:	ea000008 	b	ba28 <vsprintf+0xfc>
    ba04:	e352002a 	cmp	r2, #42	; 0x2a
    ba08:	13e09000 	mvnne	r9, #0	; 0x0
    ba0c:	1a000005 	bne	ba28 <vsprintf+0xfc>
    ba10:	e5969000 	ldr	r9, [r6]
    ba14:	e2888001 	add	r8, r8, #1	; 0x1
    ba18:	e3590000 	cmp	r9, #0	; 0x0
    ba1c:	e2866004 	add	r6, r6, #4	; 0x4
    ba20:	b2699000 	rsblt	r9, r9, #0	; 0x0
    ba24:	b3877010 	orrlt	r7, r7, #16	; 0x10
    ba28:	e5d83000 	ldrb	r3, [r8]
    ba2c:	e353002e 	cmp	r3, #46	; 0x2e
    ba30:	13e02000 	mvnne	r2, #0	; 0x0
    ba34:	158d202c 	strne	r2, [sp, #44]
    ba38:	1a00001d 	bne	bab4 <vsprintf+0x188>
    ba3c:	e5f82001 	ldrb	r2, [r8, #1]!
    ba40:	e2423030 	sub	r3, r2, #48	; 0x30
    ba44:	e6ef3073 	uxtb	r3, r3
    ba48:	e3530009 	cmp	r3, #9	; 0x9
    ba4c:	93a03000 	movls	r3, #0	; 0x0
    ba50:	958d302c 	strls	r3, [sp, #44]
    ba54:	93a0100a 	movls	r1, #10	; 0xa
    ba58:	9a000004 	bls	ba70 <vsprintf+0x144>
    ba5c:	ea000009 	b	ba88 <vsprintf+0x15c>
    ba60:	e59d002c 	ldr	r0, [sp, #44]
    ba64:	e2888001 	add	r8, r8, #1	; 0x1
    ba68:	e0202091 	mla	r0, r1, r0, r2
    ba6c:	e58d002c 	str	r0, [sp, #44]
    ba70:	e5d83000 	ldrb	r3, [r8]
    ba74:	e2432030 	sub	r2, r3, #48	; 0x30
    ba78:	e6ef3072 	uxtb	r3, r2
    ba7c:	e3530009 	cmp	r3, #9	; 0x9
    ba80:	9afffff6 	bls	ba60 <vsprintf+0x134>
    ba84:	ea000005 	b	baa0 <vsprintf+0x174>
    ba88:	e352002a 	cmp	r2, #42	; 0x2a
    ba8c:	1a000006 	bne	baac <vsprintf+0x180>
    ba90:	e5961000 	ldr	r1, [r6]
    ba94:	e2888001 	add	r8, r8, #1	; 0x1
    ba98:	e58d102c 	str	r1, [sp, #44]
    ba9c:	e2866004 	add	r6, r6, #4	; 0x4
    baa0:	e59d202c 	ldr	r2, [sp, #44]
    baa4:	e3520000 	cmp	r2, #0	; 0x0
    baa8:	aa000001 	bge	bab4 <vsprintf+0x188>
    baac:	e3a03000 	mov	r3, #0	; 0x0
    bab0:	e58d302c 	str	r3, [sp, #44]
    bab4:	e5d83000 	ldrb	r3, [r8]
    bab8:	e353006c 	cmp	r3, #108	; 0x6c
    babc:	13530068 	cmpne	r3, #104	; 0x68
    bac0:	0a000008 	beq	bae8 <vsprintf+0x1bc>
    bac4:	e353004c 	cmp	r3, #76	; 0x4c
    bac8:	0a000006 	beq	bae8 <vsprintf+0x1bc>
    bacc:	e353005a 	cmp	r3, #90	; 0x5a
    bad0:	0a000004 	beq	bae8 <vsprintf+0x1bc>
    bad4:	e353007a 	cmp	r3, #122	; 0x7a
    bad8:	0a000002 	beq	bae8 <vsprintf+0x1bc>
    badc:	e3530074 	cmp	r3, #116	; 0x74
    bae0:	13e02000 	mvnne	r2, #0	; 0x0
    bae4:	1a000007 	bne	bb08 <vsprintf+0x1dc>
    bae8:	e353006c 	cmp	r3, #108	; 0x6c
    baec:	e1a02003 	mov	r2, r3
    baf0:	e2888001 	add	r8, r8, #1	; 0x1
    baf4:	1a000003 	bne	bb08 <vsprintf+0x1dc>
    baf8:	e5d83000 	ldrb	r3, [r8]
    bafc:	e353006c 	cmp	r3, #108	; 0x6c
    bb00:	02888001 	addeq	r8, r8, #1	; 0x1
    bb04:	03a0204c 	moveq	r2, #76	; 0x4c
    bb08:	e5d83000 	ldrb	r3, [r8]
    bb0c:	e353006e 	cmp	r3, #110	; 0x6e
    bb10:	0a0000df 	beq	be94 <vsprintf+0x568>
    bb14:	8a00000c 	bhi	bb4c <vsprintf+0x220>
    bb18:	e3530063 	cmp	r3, #99	; 0x63
    bb1c:	0a000017 	beq	bb80 <vsprintf+0x254>
    bb20:	8a000004 	bhi	bb38 <vsprintf+0x20c>
    bb24:	e3530025 	cmp	r3, #37	; 0x25
    bb28:	0a0000ed 	beq	bee4 <vsprintf+0x5b8>
    bb2c:	e3530058 	cmp	r3, #88	; 0x58
    bb30:	1a0000e5 	bne	becc <vsprintf+0x5a0>
    bb34:	ea0000e0 	b	bebc <vsprintf+0x590>
    bb38:	e3530064 	cmp	r3, #100	; 0x64
    bb3c:	0a0000e0 	beq	bec4 <vsprintf+0x598>
    bb40:	e3530069 	cmp	r3, #105	; 0x69
    bb44:	1a0000e0 	bne	becc <vsprintf+0x5a0>
    bb48:	ea0000dd 	b	bec4 <vsprintf+0x598>
    bb4c:	e3530073 	cmp	r3, #115	; 0x73
    bb50:	0a000021 	beq	bbdc <vsprintf+0x2b0>
    bb54:	8a000004 	bhi	bb6c <vsprintf+0x240>
    bb58:	e353006f 	cmp	r3, #111	; 0x6f
    bb5c:	0a0000d3 	beq	beb0 <vsprintf+0x584>
    bb60:	e3530070 	cmp	r3, #112	; 0x70
    bb64:	1a0000d8 	bne	becc <vsprintf+0x5a0>
    bb68:	ea000023 	b	bbfc <vsprintf+0x2d0>
    bb6c:	e3530075 	cmp	r3, #117	; 0x75
    bb70:	0a0000dd 	beq	beec <vsprintf+0x5c0>
    bb74:	e3530078 	cmp	r3, #120	; 0x78
    bb78:	1a0000d3 	bne	becc <vsprintf+0x5a0>
    bb7c:	ea0000cd 	b	beb8 <vsprintf+0x58c>
    bb80:	e3170010 	tst	r7, #16	; 0x10
    bb84:	01a0300a 	moveq	r3, sl
    bb88:	03a02020 	moveq	r2, #32	; 0x20
    bb8c:	0a000001 	beq	bb98 <vsprintf+0x26c>
    bb90:	ea000005 	b	bbac <vsprintf+0x280>
    bb94:	e5432001 	strb	r2, [r3, #-1]
    bb98:	e2499001 	sub	r9, r9, #1	; 0x1
    bb9c:	e3590000 	cmp	r9, #0	; 0x0
    bba0:	e1a0a003 	mov	sl, r3
    bba4:	e2833001 	add	r3, r3, #1	; 0x1
    bba8:	cafffff9 	bgt	bb94 <vsprintf+0x268>
    bbac:	e5963000 	ldr	r3, [r6]
    bbb0:	e1a0200a 	mov	r2, sl
    bbb4:	e4c23001 	strb	r3, [r2], #1
    bbb8:	e3a03020 	mov	r3, #32	; 0x20
    bbbc:	ea000000 	b	bbc4 <vsprintf+0x298>
    bbc0:	e5423001 	strb	r3, [r2, #-1]
    bbc4:	e2499001 	sub	r9, r9, #1	; 0x1
    bbc8:	e3590000 	cmp	r9, #0	; 0x0
    bbcc:	e1a0a002 	mov	sl, r2
    bbd0:	e2822001 	add	r2, r2, #1	; 0x1
    bbd4:	cafffff9 	bgt	bbc0 <vsprintf+0x294>
    bbd8:	ea0000ab 	b	be8c <vsprintf+0x560>
    bbdc:	e58d7000 	str	r7, [sp]
    bbe0:	e1a0000a 	mov	r0, sl
    bbe4:	e5961000 	ldr	r1, [r6]
    bbe8:	e1a02009 	mov	r2, r9
    bbec:	e59d302c 	ldr	r3, [sp, #44]
    bbf0:	ebfffe40 	bl	b4f8 <string>
    bbf4:	e1a0a000 	mov	sl, r0
    bbf8:	ea0000a3 	b	be8c <vsprintf+0x560>
    bbfc:	e596b000 	ldr	fp, [r6]
    bc00:	e35b0000 	cmp	fp, #0	; 0x0
    bc04:	1a000005 	bne	bc20 <vsprintf+0x2f4>
    bc08:	e1a0000a 	mov	r0, sl
    bc0c:	e1a02009 	mov	r2, r9
    bc10:	e59d302c 	ldr	r3, [sp, #44]
    bc14:	e59f13f0 	ldr	r1, [pc, #1008]	; c00c <vsprintf+0x6e0>
    bc18:	e58d7000 	str	r7, [sp]
    bc1c:	ea000081 	b	be28 <vsprintf+0x4fc>
    bc20:	e5d83001 	ldrb	r3, [r8, #1]
    bc24:	e2882001 	add	r2, r8, #1	; 0x1
    bc28:	e353004d 	cmp	r3, #77	; 0x4d
    bc2c:	0a000008 	beq	bc54 <vsprintf+0x328>
    bc30:	8a000002 	bhi	bc40 <vsprintf+0x314>
    bc34:	e3530049 	cmp	r3, #73	; 0x49
    bc38:	1a00007c 	bne	be30 <vsprintf+0x504>
    bc3c:	ea000025 	b	bcd8 <vsprintf+0x3ac>
    bc40:	e3530069 	cmp	r3, #105	; 0x69
    bc44:	0a000022 	beq	bcd4 <vsprintf+0x3a8>
    bc48:	e353006d 	cmp	r3, #109	; 0x6d
    bc4c:	1a000077 	bne	be30 <vsprintf+0x504>
    bc50:	e3877040 	orr	r7, r7, #64	; 0x40
    bc54:	e1a03327 	lsr	r3, r7, #6
    bc58:	e2233001 	eor	r3, r3, #1	; 0x1
    bc5c:	e2034001 	and	r4, r3, #1	; 0x1
    bc60:	e3a0e000 	mov	lr, #0	; 0x0
    bc64:	e28dc05b 	add	ip, sp, #91	; 0x5b
    bc68:	e3a0503a 	mov	r5, #58	; 0x3a
    bc6c:	e7db200e 	ldrb	r2, [fp, lr]
    bc70:	e59f0398 	ldr	r0, [pc, #920]	; c010 <vsprintf+0x6e4>
    bc74:	e202300f 	and	r3, r2, #15	; 0xf
    bc78:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    bc7c:	e7d03003 	ldrb	r3, [r0, r3]
    bc80:	e1a0000c 	mov	r0, ip
    bc84:	e4c02001 	strb	r2, [r0], #1
    bc88:	e35e0005 	cmp	lr, #5	; 0x5
    bc8c:	03a01000 	moveq	r1, #0	; 0x0
    bc90:	12041001 	andne	r1, r4, #1	; 0x1
    bc94:	e3510000 	cmp	r1, #0	; 0x0
    bc98:	e5cc3001 	strb	r3, [ip, #1]
    bc9c:	e28ee001 	add	lr, lr, #1	; 0x1
    bca0:	e280c001 	add	ip, r0, #1	; 0x1
    bca4:	128cc001 	addne	ip, ip, #1	; 0x1
    bca8:	15c05001 	strbne	r5, [r0, #1]
    bcac:	e35e0006 	cmp	lr, #6	; 0x6
    bcb0:	1affffed 	bne	bc6c <vsprintf+0x340>
    bcb4:	e3a03000 	mov	r3, #0	; 0x0
    bcb8:	e5cc3000 	strb	r3, [ip]
    bcbc:	e1a0000a 	mov	r0, sl
    bcc0:	e3c7c040 	bic	ip, r7, #64	; 0x40
    bcc4:	e1a02009 	mov	r2, r9
    bcc8:	e59d302c 	ldr	r3, [sp, #44]
    bccc:	e28d105b 	add	r1, sp, #91	; 0x5b
    bcd0:	ea000053 	b	be24 <vsprintf+0x4f8>
    bcd4:	e3877040 	orr	r7, r7, #64	; 0x40
    bcd8:	e5d23001 	ldrb	r3, [r2, #1]
    bcdc:	e3530036 	cmp	r3, #54	; 0x36
    bce0:	1a00002d 	bne	bd9c <vsprintf+0x470>
    bce4:	e1a03327 	lsr	r3, r7, #6
    bce8:	e2233001 	eor	r3, r3, #1	; 0x1
    bcec:	e1a0500b 	mov	r5, fp
    bcf0:	e3a04000 	mov	r4, #0	; 0x0
    bcf4:	e203b001 	and	fp, r3, #1	; 0x1
    bcf8:	e28dc033 	add	ip, sp, #51	; 0x33
    bcfc:	e5d52000 	ldrb	r2, [r5]
    bd00:	e59fe308 	ldr	lr, [pc, #776]	; c010 <vsprintf+0x6e4>
    bd04:	e202300f 	and	r3, r2, #15	; 0xf
    bd08:	e7de0003 	ldrb	r0, [lr, r3]
    bd0c:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    bd10:	e1a0100c 	mov	r1, ip
    bd14:	e4c13001 	strb	r3, [r1], #1
    bd18:	e5cc0001 	strb	r0, [ip, #1]
    bd1c:	e5d52001 	ldrb	r2, [r5, #1]
    bd20:	e59f02e8 	ldr	r0, [pc, #744]	; c010 <vsprintf+0x6e4>
    bd24:	e202300f 	and	r3, r2, #15	; 0xf
    bd28:	e7d0c003 	ldrb	ip, [r0, r3]
    bd2c:	e59f32dc 	ldr	r3, [pc, #732]	; c010 <vsprintf+0x6e4>
    bd30:	e3540007 	cmp	r4, #7	; 0x7
    bd34:	03a0e000 	moveq	lr, #0	; 0x0
    bd38:	120be001 	andne	lr, fp, #1	; 0x1
    bd3c:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    bd40:	e35e0000 	cmp	lr, #0	; 0x0
    bd44:	e58d2014 	str	r2, [sp, #20]
    bd48:	e2810001 	add	r0, r1, #1	; 0x1
    bd4c:	e59de014 	ldr	lr, [sp, #20]
    bd50:	e2802001 	add	r2, r0, #1	; 0x1
    bd54:	e5c1e001 	strb	lr, [r1, #1]
    bd58:	e2844001 	add	r4, r4, #1	; 0x1
    bd5c:	e5c0c001 	strb	ip, [r0, #1]
    bd60:	13a0003a 	movne	r0, #58	; 0x3a
    bd64:	e282c001 	add	ip, r2, #1	; 0x1
    bd68:	128cc001 	addne	ip, ip, #1	; 0x1
    bd6c:	15c20001 	strbne	r0, [r2, #1]
    bd70:	e3540008 	cmp	r4, #8	; 0x8
    bd74:	e2855002 	add	r5, r5, #2	; 0x2
    bd78:	1affffdf 	bne	bcfc <vsprintf+0x3d0>
    bd7c:	e3a03000 	mov	r3, #0	; 0x0
    bd80:	e5cc3000 	strb	r3, [ip]
    bd84:	e1a0000a 	mov	r0, sl
    bd88:	e3c7c040 	bic	ip, r7, #64	; 0x40
    bd8c:	e1a02009 	mov	r2, r9
    bd90:	e59d302c 	ldr	r3, [sp, #44]
    bd94:	e28d1033 	add	r1, sp, #51	; 0x33
    bd98:	ea000021 	b	be24 <vsprintf+0x4f8>
    bd9c:	e3530034 	cmp	r3, #52	; 0x34
    bda0:	13c77040 	bicne	r7, r7, #64	; 0x40
    bda4:	1a000021 	bne	be30 <vsprintf+0x504>
    bda8:	e3a05000 	mov	r5, #0	; 0x0
    bdac:	e28d406d 	add	r4, sp, #109	; 0x6d
    bdb0:	e7db1005 	ldrb	r1, [fp, r5]
    bdb4:	e28d007d 	add	r0, sp, #125	; 0x7d
    bdb8:	ebfffd9b 	bl	b42c <put_dec_trunc>
    bdbc:	e59d101c 	ldr	r1, [sp, #28]
    bdc0:	e2402001 	sub	r2, r0, #1	; 0x1
    bdc4:	e0610000 	rsb	r0, r1, r0
    bdc8:	e3a01000 	mov	r1, #0	; 0x0
    bdcc:	ea000002 	b	bddc <vsprintf+0x4b0>
    bdd0:	e5d23001 	ldrb	r3, [r2, #1]
    bdd4:	e7c43001 	strb	r3, [r4, r1]
    bdd8:	e2811001 	add	r1, r1, #1	; 0x1
    bddc:	e59d3018 	ldr	r3, [sp, #24]
    bde0:	e2422001 	sub	r2, r2, #1	; 0x1
    bde4:	e1520003 	cmp	r2, r3
    bde8:	1afffff8 	bne	bdd0 <vsprintf+0x4a4>
    bdec:	e3550003 	cmp	r5, #3	; 0x3
    bdf0:	13a0e02e 	movne	lr, #46	; 0x2e
    bdf4:	e0844000 	add	r4, r4, r0
    bdf8:	e2855001 	add	r5, r5, #1	; 0x1
    bdfc:	14c4e001 	strbne	lr, [r4], #1
    be00:	e3550004 	cmp	r5, #4	; 0x4
    be04:	1affffe9 	bne	bdb0 <vsprintf+0x484>
    be08:	e3a03000 	mov	r3, #0	; 0x0
    be0c:	e5c43000 	strb	r3, [r4]
    be10:	e59d302c 	ldr	r3, [sp, #44]
    be14:	e3c7c040 	bic	ip, r7, #64	; 0x40
    be18:	e1a0000a 	mov	r0, sl
    be1c:	e1a02009 	mov	r2, r9
    be20:	e28d106d 	add	r1, sp, #109	; 0x6d
    be24:	e58dc000 	str	ip, [sp]
    be28:	ebfffdb2 	bl	b4f8 <string>
    be2c:	ea00000d 	b	be68 <vsprintf+0x53c>
    be30:	e3a01010 	mov	r1, #16	; 0x10
    be34:	e3790001 	cmn	r9, #1	; 0x1
    be38:	e58d1000 	str	r1, [sp]
    be3c:	e59d102c 	ldr	r1, [sp, #44]
    be40:	e387c020 	orr	ip, r7, #32	; 0x20
    be44:	02899009 	addeq	r9, r9, #9	; 0x9
    be48:	0387c021 	orreq	ip, r7, #33	; 0x21
    be4c:	e1a0000a 	mov	r0, sl
    be50:	e1a0200b 	mov	r2, fp
    be54:	e3a03000 	mov	r3, #0	; 0x0
    be58:	e58d9004 	str	r9, [sp, #4]
    be5c:	e58d1008 	str	r1, [sp, #8]
    be60:	e58dc00c 	str	ip, [sp, #12]
    be64:	ebfffe1a 	bl	b6d4 <number>
    be68:	e1a0a000 	mov	sl, r0
    be6c:	e1a02008 	mov	r2, r8
    be70:	e5d23001 	ldrb	r3, [r2, #1]
    be74:	e59fe198 	ldr	lr, [pc, #408]	; c014 <vsprintf+0x6e8>
    be78:	e1a08002 	mov	r8, r2
    be7c:	e7de3003 	ldrb	r3, [lr, r3]
    be80:	e2822001 	add	r2, r2, #1	; 0x1
    be84:	e3130007 	tst	r3, #7	; 0x7
    be88:	1afffff8 	bne	be70 <vsprintf+0x544>
    be8c:	e2866004 	add	r6, r6, #4	; 0x4
    be90:	ea000054 	b	bfe8 <vsprintf+0x6bc>
    be94:	e59d0028 	ldr	r0, [sp, #40]
    be98:	e5963000 	ldr	r3, [r6]
    be9c:	e2861004 	add	r1, r6, #4	; 0x4
    bea0:	e060200a 	rsb	r2, r0, sl
    bea4:	e1a06001 	mov	r6, r1
    bea8:	e5832000 	str	r2, [r3]
    beac:	ea00004d 	b	bfe8 <vsprintf+0x6bc>
    beb0:	e3a0c008 	mov	ip, #8	; 0x8
    beb4:	ea00000d 	b	bef0 <vsprintf+0x5c4>
    beb8:	e3877020 	orr	r7, r7, #32	; 0x20
    bebc:	e3a0c010 	mov	ip, #16	; 0x10
    bec0:	ea00000a 	b	bef0 <vsprintf+0x5c4>
    bec4:	e3877002 	orr	r7, r7, #2	; 0x2
    bec8:	ea000007 	b	beec <vsprintf+0x5c0>
    becc:	e3a03025 	mov	r3, #37	; 0x25
    bed0:	e4ca3001 	strb	r3, [sl], #1
    bed4:	e5d83000 	ldrb	r3, [r8]
    bed8:	e3530000 	cmp	r3, #0	; 0x0
    bedc:	02488001 	subeq	r8, r8, #1	; 0x1
    bee0:	0a000040 	beq	bfe8 <vsprintf+0x6bc>
    bee4:	e4ca3001 	strb	r3, [sl], #1
    bee8:	ea00003e 	b	bfe8 <vsprintf+0x6bc>
    beec:	e3a0c00a 	mov	ip, #10	; 0xa
    bef0:	e352004c 	cmp	r2, #76	; 0x4c
    bef4:	1a000009 	bne	bf20 <vsprintf+0x5f4>
    bef8:	e59d1024 	ldr	r1, [sp, #36]
    befc:	e3a02000 	mov	r2, #0	; 0x0
    bf00:	e0613006 	rsb	r3, r1, r6
    bf04:	e3130007 	tst	r3, #7	; 0x7
    bf08:	02866004 	addeq	r6, r6, #4	; 0x4
    bf0c:	e896000a 	ldm	r6, {r1, r3}
    bf10:	e1a05003 	mov	r5, r3
    bf14:	e1824001 	orr	r4, r2, r1
    bf18:	e2866008 	add	r6, r6, #8	; 0x8
    bf1c:	ea000028 	b	bfc4 <vsprintf+0x698>
    bf20:	e352006c 	cmp	r2, #108	; 0x6c
    bf24:	1a000006 	bne	bf44 <vsprintf+0x618>
    bf28:	e5963000 	ldr	r3, [r6]
    bf2c:	e3170002 	tst	r7, #2	; 0x2
    bf30:	e1a04003 	mov	r4, r3
    bf34:	e3a05000 	mov	r5, #0	; 0x0
    bf38:	e2866004 	add	r6, r6, #4	; 0x4
    bf3c:	0a000020 	beq	bfc4 <vsprintf+0x698>
    bf40:	ea00001e 	b	bfc0 <vsprintf+0x694>
    bf44:	e352007a 	cmp	r2, #122	; 0x7a
    bf48:	1352005a 	cmpne	r2, #90	; 0x5a
    bf4c:	05963000 	ldreq	r3, [r6]
    bf50:	02866004 	addeq	r6, r6, #4	; 0x4
    bf54:	01a04003 	moveq	r4, r3
    bf58:	03a05000 	moveq	r5, #0	; 0x0
    bf5c:	0a000018 	beq	bfc4 <vsprintf+0x698>
    bf60:	e3520074 	cmp	r2, #116	; 0x74
    bf64:	05963000 	ldreq	r3, [r6]
    bf68:	02866004 	addeq	r6, r6, #4	; 0x4
    bf6c:	0a00000b 	beq	bfa0 <vsprintf+0x674>
    bf70:	e3520068 	cmp	r2, #104	; 0x68
    bf74:	e2861004 	add	r1, r6, #4	; 0x4
    bf78:	e5962000 	ldr	r2, [r6]
    bf7c:	e2070002 	and	r0, r7, #2	; 0x2
    bf80:	1a000009 	bne	bfac <vsprintf+0x680>
    bf84:	e6ff3072 	uxth	r3, r2
    bf88:	e3500000 	cmp	r0, #0	; 0x0
    bf8c:	e1a06001 	mov	r6, r1
    bf90:	e1a04003 	mov	r4, r3
    bf94:	e3a05000 	mov	r5, #0	; 0x0
    bf98:	0a000009 	beq	bfc4 <vsprintf+0x698>
    bf9c:	e6bf3074 	sxth	r3, r4
    bfa0:	e1a04003 	mov	r4, r3
    bfa4:	e1a05fc4 	asr	r5, r4, #31
    bfa8:	ea000005 	b	bfc4 <vsprintf+0x698>
    bfac:	e3500000 	cmp	r0, #0	; 0x0
    bfb0:	e1a06001 	mov	r6, r1
    bfb4:	e1a04002 	mov	r4, r2
    bfb8:	e3a05000 	mov	r5, #0	; 0x0
    bfbc:	0a000000 	beq	bfc4 <vsprintf+0x698>
    bfc0:	e1a05fc4 	asr	r5, r4, #31
    bfc4:	e59de02c 	ldr	lr, [sp, #44]
    bfc8:	e1a0000a 	mov	r0, sl
    bfcc:	e1a02004 	mov	r2, r4
    bfd0:	e1a03005 	mov	r3, r5
    bfd4:	e58dc000 	str	ip, [sp]
    bfd8:	e98d4200 	stmib	sp, {r9, lr}
    bfdc:	e58d700c 	str	r7, [sp, #12]
    bfe0:	ebfffdbb 	bl	b6d4 <number>
    bfe4:	e1a0a000 	mov	sl, r0
    bfe8:	e2888001 	add	r8, r8, #1	; 0x1
    bfec:	e5d83000 	ldrb	r3, [r8]
    bff0:	e3530000 	cmp	r3, #0	; 0x0
    bff4:	1afffe58 	bne	b95c <vsprintf+0x30>
    bff8:	e5ca3000 	strb	r3, [sl]
    bffc:	e59d1028 	ldr	r1, [sp, #40]
    c000:	e061000a 	rsb	r0, r1, sl
    c004:	e28dd084 	add	sp, sp, #132	; 0x84
    c008:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c00c:	0001172d 	.word	0x0001172d
    c010:	0000f950 	.word	0x0000f950
    c014:	0000f971 	.word	0x0000f971

0000c018 <sprintf>:
    c018:	e92d000e 	push	{r1, r2, r3}
    c01c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c020:	e59d1004 	ldr	r1, [sp, #4]
    c024:	e28d2008 	add	r2, sp, #8	; 0x8
    c028:	ebfffe3f 	bl	b92c <vsprintf>
    c02c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    c030:	e28dd00c 	add	sp, sp, #12	; 0xc
    c034:	e12fff1e 	bx	lr

0000c038 <ius_check_header>:
    c038:	e92d4070 	push	{r4, r5, r6, lr}
    c03c:	e5901000 	ldr	r1, [r0]
    c040:	e59f30e8 	ldr	r3, [pc, #232]	; c130 <ius_check_header+0xf8>
    c044:	e24dd008 	sub	sp, sp, #8	; 0x8
    c048:	e1510003 	cmp	r1, r3
    c04c:	e1a04000 	mov	r4, r0
    c050:	0a000005 	beq	c06c <ius_check_header+0x34>
    c054:	e59f00d8 	ldr	r0, [pc, #216]	; c134 <ius_check_header+0xfc>
    c058:	e59f30d8 	ldr	r3, [pc, #216]	; c138 <ius_check_header+0x100>
    c05c:	e1a0e00f 	mov	lr, pc
    c060:	e593f000 	ldr	pc, [r3]
    c064:	e3e00000 	mvn	r0, #0	; 0x0
    c068:	ea00002e 	b	c128 <ius_check_header+0xf0>
    c06c:	ebffe77c 	bl	5e64 <cdbg_verify_burn>
    c070:	e3500000 	cmp	r0, #0	; 0x0
    c074:	0a000011 	beq	c0c0 <ius_check_header+0x88>
    c078:	e5943004 	ldr	r3, [r4, #4]
    c07c:	e3130002 	tst	r3, #2	; 0x2
    c080:	1a000005 	bne	c09c <ius_check_header+0x64>
    c084:	e59f00b0 	ldr	r0, [pc, #176]	; c13c <ius_check_header+0x104>
    c088:	e59f30a8 	ldr	r3, [pc, #168]	; c138 <ius_check_header+0x100>
    c08c:	e1a0e00f 	mov	lr, pc
    c090:	e593f000 	ldr	pc, [r3]
    c094:	e3e00021 	mvn	r0, #33	; 0x21
    c098:	ea000022 	b	c128 <ius_check_header+0xf0>
    c09c:	ebffe774 	bl	5e74 <cdbg_verify_burn_key>
    c0a0:	e59f2098 	ldr	r2, [pc, #152]	; c140 <ius_check_header+0x108>
    c0a4:	e1a01000 	mov	r1, r0
    c0a8:	e3a00000 	mov	r0, #0	; 0x0
    c0ac:	ebffe400 	bl	50b4 <ss_aes_init>
    c0b0:	e2840010 	add	r0, r4, #16	; 0x10
    c0b4:	e1a01000 	mov	r1, r0
    c0b8:	e3a02e1f 	mov	r2, #496	; 0x1f0
    c0bc:	ebffe3eb 	bl	5070 <ss_aes_data>
    c0c0:	e3a06000 	mov	r6, #0	; 0x0
    c0c4:	e5945018 	ldr	r5, [r4, #24]
    c0c8:	e3a01c02 	mov	r1, #512	; 0x200
    c0cc:	e5846018 	str	r6, [r4, #24]
    c0d0:	e1a00006 	mov	r0, r6
    c0d4:	ebffe514 	bl	552c <hash_init>
    c0d8:	e3a01c02 	mov	r1, #512	; 0x200
    c0dc:	e1a00004 	mov	r0, r4
    c0e0:	ebffe4ec 	bl	5498 <hash_data>
    c0e4:	e28d0004 	add	r0, sp, #4	; 0x4
    c0e8:	ebffe4ce 	bl	5428 <hash_value>
    c0ec:	e59d2004 	ldr	r2, [sp, #4]
    c0f0:	e59f3040 	ldr	r3, [pc, #64]	; c138 <ius_check_header+0x100>
    c0f4:	e1550002 	cmp	r5, r2
    c0f8:	e5845018 	str	r5, [r4, #24]
    c0fc:	0a000005 	beq	c118 <ius_check_header+0xe0>
    c100:	e59f003c 	ldr	r0, [pc, #60]	; c144 <ius_check_header+0x10c>
    c104:	e1a01005 	mov	r1, r5
    c108:	e1a0e00f 	mov	lr, pc
    c10c:	e593f000 	ldr	pc, [r3]
    c110:	e3e0001f 	mvn	r0, #31	; 0x1f
    c114:	ea000003 	b	c128 <ius_check_header+0xf0>
    c118:	e59f0028 	ldr	r0, [pc, #40]	; c148 <ius_check_header+0x110>
    c11c:	e1a0e00f 	mov	lr, pc
    c120:	e593f000 	ldr	pc, [r3]
    c124:	e1a00006 	mov	r0, r6
    c128:	e28dd008 	add	sp, sp, #8	; 0x8
    c12c:	e8bd8070 	pop	{r4, r5, r6, pc}
    c130:	7375692b 	.word	0x7375692b
    c134:	00011734 	.word	0x00011734
    c138:	3c00d68c 	.word	0x3c00d68c
    c13c:	0001174e 	.word	0x0001174e
    c140:	3c00d09c 	.word	0x3c00d09c
    c144:	0001175d 	.word	0x0001175d
    c148:	00011785 	.word	0x00011785

0000c14c <printf_off>:
    c14c:	e92d000f 	push	{r0, r1, r2, r3}
    c150:	e28dd010 	add	sp, sp, #16	; 0x10
    c154:	e12fff1e 	bx	lr

0000c158 <disable_ctrlc>:
    c158:	e59f300c 	ldr	r3, [pc, #12]	; c16c <disable_ctrlc+0x14>
    c15c:	e5932000 	ldr	r2, [r3]
    c160:	e5830000 	str	r0, [r3]
    c164:	e1a00002 	mov	r0, r2
    c168:	e12fff1e 	bx	lr
    c16c:	3c00d22c 	.word	0x3c00d22c

0000c170 <had_ctrlc>:
    c170:	e59f3004 	ldr	r3, [pc, #4]	; c17c <had_ctrlc+0xc>
    c174:	e5930000 	ldr	r0, [r3]
    c178:	e12fff1e 	bx	lr
    c17c:	3c00d228 	.word	0x3c00d228

0000c180 <clear_ctrlc>:
    c180:	e59f3008 	ldr	r3, [pc, #8]	; c190 <clear_ctrlc+0x10>
    c184:	e3a02000 	mov	r2, #0	; 0x0
    c188:	e5832000 	str	r2, [r3]
    c18c:	e12fff1e 	bx	lr
    c190:	3c00d228 	.word	0x3c00d228

0000c194 <dbg>:
    c194:	e92d000f 	push	{r0, r1, r2, r3}
    c198:	e28dd010 	add	sp, sp, #16	; 0x10
    c19c:	e12fff1e 	bx	lr

0000c1a0 <puts>:
    c1a0:	eafff0f8 	b	8588 <serial_puts>

0000c1a4 <vprintf>:
    c1a4:	e92d4010 	push	{r4, lr}
    c1a8:	e24ddd06 	sub	sp, sp, #384	; 0x180
    c1ac:	e1a02001 	mov	r2, r1
    c1b0:	e1a01000 	mov	r1, r0
    c1b4:	e1a0000d 	mov	r0, sp
    c1b8:	ebfffddb 	bl	b92c <vsprintf>
    c1bc:	e1a0000d 	mov	r0, sp
    c1c0:	e1a0400d 	mov	r4, sp
    c1c4:	ebfff0ef 	bl	8588 <serial_puts>
    c1c8:	e28ddd06 	add	sp, sp, #384	; 0x180
    c1cc:	e8bd8010 	pop	{r4, pc}

0000c1d0 <printf_on>:
    c1d0:	e92d000f 	push	{r0, r1, r2, r3}
    c1d4:	e92d4010 	push	{r4, lr}
    c1d8:	e24ddd06 	sub	sp, sp, #384	; 0x180
    c1dc:	e1a0000d 	mov	r0, sp
    c1e0:	e59d1188 	ldr	r1, [sp, #392]
    c1e4:	e28d2f63 	add	r2, sp, #396	; 0x18c
    c1e8:	ebfffdcf 	bl	b92c <vsprintf>
    c1ec:	e1a0000d 	mov	r0, sp
    c1f0:	e1a0400d 	mov	r4, sp
    c1f4:	ebfff0e3 	bl	8588 <serial_puts>
    c1f8:	e28ddd06 	add	sp, sp, #384	; 0x180
    c1fc:	e8bd4010 	pop	{r4, lr}
    c200:	e28dd010 	add	sp, sp, #16	; 0x10
    c204:	e12fff1e 	bx	lr

0000c208 <serial_printf>:
    c208:	e92d000f 	push	{r0, r1, r2, r3}
    c20c:	e92d4010 	push	{r4, lr}
    c210:	e24ddd06 	sub	sp, sp, #384	; 0x180
    c214:	e1a0000d 	mov	r0, sp
    c218:	e59d1188 	ldr	r1, [sp, #392]
    c21c:	e28d2f63 	add	r2, sp, #396	; 0x18c
    c220:	ebfffdc1 	bl	b92c <vsprintf>
    c224:	e1a0000d 	mov	r0, sp
    c228:	e1a0400d 	mov	r4, sp
    c22c:	ebfff0d5 	bl	8588 <serial_puts>
    c230:	e28ddd06 	add	sp, sp, #384	; 0x180
    c234:	e8bd4010 	pop	{r4, lr}
    c238:	e28dd010 	add	sp, sp, #16	; 0x10
    c23c:	e12fff1e 	bx	lr

0000c240 <putc>:
    c240:	eafff0b9 	b	852c <serial_putc>

0000c244 <tstc>:
    c244:	eafff0b1 	b	8510 <serial_tstc>

0000c248 <getc>:
    c248:	eafff0df 	b	85cc <serial_getc>

0000c24c <ctrlc>:
    c24c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c250:	e59f3040 	ldr	r3, [pc, #64]	; c298 <ctrlc+0x4c>
    c254:	e24dd004 	sub	sp, sp, #4	; 0x4
    c258:	e5933000 	ldr	r3, [r3]
    c25c:	e3530000 	cmp	r3, #0	; 0x0
    c260:	1a000009 	bne	c28c <ctrlc+0x40>
    c264:	ebfff0a9 	bl	8510 <serial_tstc>
    c268:	e3500000 	cmp	r0, #0	; 0x0
    c26c:	0a000006 	beq	c28c <ctrlc+0x40>
    c270:	ebfff0d5 	bl	85cc <serial_getc>
    c274:	e3500003 	cmp	r0, #3	; 0x3
    c278:	059f301c 	ldreq	r3, [pc, #28]	; c29c <ctrlc+0x50>
    c27c:	03a02001 	moveq	r2, #1	; 0x1
    c280:	05832000 	streq	r2, [r3]
    c284:	01a00002 	moveq	r0, r2
    c288:	0a000000 	beq	c290 <ctrlc+0x44>
    c28c:	e3a00000 	mov	r0, #0	; 0x0
    c290:	e28dd004 	add	sp, sp, #4	; 0x4
    c294:	e8bd8000 	pop	{pc}
    c298:	3c00d22c 	.word	0x3c00d22c
    c29c:	3c00d228 	.word	0x3c00d228

0000c2a0 <parse_line>:
    c2a0:	e92d4010 	push	{r4, lr}
    c2a4:	e3a04000 	mov	r4, #0	; 0x0
    c2a8:	e1a02004 	mov	r2, r4
    c2ac:	ea000000 	b	c2b4 <parse_line+0x14>
    c2b0:	e2800001 	add	r0, r0, #1	; 0x1
    c2b4:	e5d03000 	ldrb	r3, [r0]
    c2b8:	e3530009 	cmp	r3, #9	; 0x9
    c2bc:	13530020 	cmpne	r3, #32	; 0x20
    c2c0:	0afffffa 	beq	c2b0 <parse_line+0x10>
    c2c4:	e3530000 	cmp	r3, #0	; 0x0
    c2c8:	0a00000c 	beq	c300 <parse_line+0x60>
    c2cc:	e7810104 	str	r0, [r1, r4, lsl #2]
    c2d0:	e2844001 	add	r4, r4, #1	; 0x1
    c2d4:	ea000000 	b	c2dc <parse_line+0x3c>
    c2d8:	e2800001 	add	r0, r0, #1	; 0x1
    c2dc:	e5d03000 	ldrb	r3, [r0]
    c2e0:	e3530000 	cmp	r3, #0	; 0x0
    c2e4:	13530020 	cmpne	r3, #32	; 0x20
    c2e8:	0a000002 	beq	c2f8 <parse_line+0x58>
    c2ec:	e3530009 	cmp	r3, #9	; 0x9
    c2f0:	1afffff8 	bne	c2d8 <parse_line+0x38>
    c2f4:	ea000003 	b	c308 <parse_line+0x68>
    c2f8:	e3530000 	cmp	r3, #0	; 0x0
    c2fc:	1a000001 	bne	c308 <parse_line+0x68>
    c300:	e7813104 	str	r3, [r1, r4, lsl #2]
    c304:	ea000007 	b	c328 <parse_line+0x88>
    c308:	e3540010 	cmp	r4, #16	; 0x10
    c30c:	e5c02000 	strb	r2, [r0]
    c310:	1affffe6 	bne	c2b0 <parse_line+0x10>
    c314:	e59f0014 	ldr	r0, [pc, #20]	; c330 <parse_line+0x90>
    c318:	e1a01004 	mov	r1, r4
    c31c:	e59f3010 	ldr	r3, [pc, #16]	; c334 <parse_line+0x94>
    c320:	e1a0e00f 	mov	lr, pc
    c324:	e593f000 	ldr	pc, [r3]
    c328:	e1a00004 	mov	r0, r4
    c32c:	e8bd8010 	pop	{r4, pc}
    c330:	0001179b 	.word	0x0001179b
    c334:	3c00d68c 	.word	0x3c00d68c

0000c338 <run_command>:
    c338:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c33c:	e1a05000 	mov	r5, r0
    c340:	e24ddf93 	sub	sp, sp, #588	; 0x24c
    c344:	e1a0a001 	mov	sl, r1
    c348:	ebffff8c 	bl	c180 <clear_ctrlc>
    c34c:	e3550000 	cmp	r5, #0	; 0x0
    c350:	0a0000ac 	beq	c608 <run_command+0x2d0>
    c354:	e5d53000 	ldrb	r3, [r5]
    c358:	e3530000 	cmp	r3, #0	; 0x0
    c35c:	0a0000a9 	beq	c608 <run_command+0x2d0>
    c360:	e1a00005 	mov	r0, r5
    c364:	ebffd37d 	bl	1160 <strlen>
    c368:	e35000ff 	cmp	r0, #255	; 0xff
    c36c:	9a000002 	bls	c37c <run_command+0x44>
    c370:	e59f02a0 	ldr	r0, [pc, #672]	; c618 <run_command+0x2e0>
    c374:	ebffff89 	bl	c1a0 <puts>
    c378:	ea0000a2 	b	c608 <run_command+0x2d0>
    c37c:	e28d4f41 	add	r4, sp, #260	; 0x104
    c380:	e1a01005 	mov	r1, r5
    c384:	e1a00004 	mov	r0, r4
    c388:	e3a06000 	mov	r6, #0	; 0x0
    c38c:	ebffd31c 	bl	1004 <strcpy>
    c390:	e3a07001 	mov	r7, #1	; 0x1
    c394:	e1a0b006 	mov	fp, r6
    c398:	e28d9f81 	add	r9, sp, #516	; 0x204
    c39c:	ea000091 	b	c5e8 <run_command+0x2b0>
    c3a0:	e3510027 	cmp	r1, #39	; 0x27
    c3a4:	1a000002 	bne	c3b4 <run_command+0x7c>
    c3a8:	e5523001 	ldrb	r3, [r2, #-1]
    c3ac:	e353005c 	cmp	r3, #92	; 0x5c
    c3b0:	12200001 	eorne	r0, r0, #1	; 0x1
    c3b4:	e3500000 	cmp	r0, #0	; 0x0
    c3b8:	1a000009 	bne	c3e4 <run_command+0xac>
    c3bc:	e351003b 	cmp	r1, #59	; 0x3b
    c3c0:	13a03000 	movne	r3, #0	; 0x0
    c3c4:	03a03001 	moveq	r3, #1	; 0x1
    c3c8:	e1520004 	cmp	r2, r4
    c3cc:	03a03000 	moveq	r3, #0	; 0x0
    c3d0:	e3530000 	cmp	r3, #0	; 0x0
    c3d4:	0a000002 	beq	c3e4 <run_command+0xac>
    c3d8:	e5523001 	ldrb	r3, [r2, #-1]
    c3dc:	e353005c 	cmp	r3, #92	; 0x5c
    c3e0:	1a000005 	bne	c3fc <run_command+0xc4>
    c3e4:	e2822001 	add	r2, r2, #1	; 0x1
    c3e8:	e5d21000 	ldrb	r1, [r2]
    c3ec:	e3510000 	cmp	r1, #0	; 0x0
    c3f0:	1affffea 	bne	c3a0 <run_command+0x68>
    c3f4:	e1a08002 	mov	r8, r2
    c3f8:	ea000001 	b	c404 <run_command+0xcc>
    c3fc:	e5c20000 	strb	r0, [r2]
    c400:	e2828001 	add	r8, r2, #1	; 0x1
    c404:	e1a00004 	mov	r0, r4
    c408:	ebffd354 	bl	1160 <strlen>
    c40c:	e3a0c000 	mov	ip, #0	; 0x0
    c410:	e3a0ec01 	mov	lr, #256	; 0x100
    c414:	e1a0100c 	mov	r1, ip
    c418:	e28d5004 	add	r5, sp, #4	; 0x4
    c41c:	ea000049 	b	c548 <run_command+0x210>
    c420:	e35c0003 	cmp	ip, #3	; 0x3
    c424:	e2400001 	sub	r0, r0, #1	; 0x1
    c428:	e4d42001 	ldrb	r2, [r4], #1
    c42c:	0a000035 	beq	c508 <run_command+0x1d0>
    c430:	e352005c 	cmp	r2, #92	; 0x5c
    c434:	13a03000 	movne	r3, #0	; 0x0
    c438:	03a03001 	moveq	r3, #1	; 0x1
    c43c:	e351005c 	cmp	r1, #92	; 0x5c
    c440:	03a03000 	moveq	r3, #0	; 0x0
    c444:	e3530000 	cmp	r3, #0	; 0x0
    c448:	0a000004 	beq	c460 <run_command+0x128>
    c44c:	e3500000 	cmp	r0, #0	; 0x0
    c450:	0a00003f 	beq	c554 <run_command+0x21c>
    c454:	e4d42001 	ldrb	r2, [r4], #1
    c458:	e2400001 	sub	r0, r0, #1	; 0x1
    c45c:	e3a0105c 	mov	r1, #92	; 0x5c
    c460:	e35c0003 	cmp	ip, #3	; 0x3
    c464:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
    c468:	ea000035 	b	c544 <run_command+0x20c>
    c46c:	0000c47c 	.word	0x0000c47c
    c470:	0000c4c0 	.word	0x0000c4c0
    c474:	0000c4f8 	.word	0x0000c4f8
    c478:	0000c508 	.word	0x0000c508
    c47c:	e251105c 	subs	r1, r1, #92	; 0x5c
    c480:	13a01001 	movne	r1, #1	; 0x1
    c484:	e3520027 	cmp	r2, #39	; 0x27
    c488:	13a03000 	movne	r3, #0	; 0x0
    c48c:	02013001 	andeq	r3, r1, #1	; 0x1
    c490:	e3530000 	cmp	r3, #0	; 0x0
    c494:	13a0c003 	movne	ip, #3	; 0x3
    c498:	1a000022 	bne	c528 <run_command+0x1f0>
    c49c:	e3520024 	cmp	r2, #36	; 0x24
    c4a0:	13a03000 	movne	r3, #0	; 0x0
    c4a4:	02013001 	andeq	r3, r1, #1	; 0x1
    c4a8:	e3530000 	cmp	r3, #0	; 0x0
    c4ac:	13a0c001 	movne	ip, #1	; 0x1
    c4b0:	13a02024 	movne	r2, #36	; 0x24
    c4b4:	04c52001 	strbeq	r2, [r5], #1
    c4b8:	024ee001 	subeq	lr, lr, #1	; 0x1
    c4bc:	ea000020 	b	c544 <run_command+0x20c>
    c4c0:	e352007b 	cmp	r2, #123	; 0x7b
    c4c4:	13520028 	cmpne	r2, #40	; 0x28
    c4c8:	13a03000 	movne	r3, #0	; 0x0
    c4cc:	03a03001 	moveq	r3, #1	; 0x1
    c4d0:	03a0c002 	moveq	ip, #2	; 0x2
    c4d4:	0a00001a 	beq	c544 <run_command+0x20c>
    c4d8:	e3a01024 	mov	r1, #36	; 0x24
    c4dc:	e25ee001 	subs	lr, lr, #1	; 0x1
    c4e0:	e4c51001 	strb	r1, [r5], #1
    c4e4:	0a000015 	beq	c540 <run_command+0x208>
    c4e8:	e4c52001 	strb	r2, [r5], #1
    c4ec:	e24ee001 	sub	lr, lr, #1	; 0x1
    c4f0:	e1a0c003 	mov	ip, r3
    c4f4:	ea000012 	b	c544 <run_command+0x20c>
    c4f8:	e352007d 	cmp	r2, #125	; 0x7d
    c4fc:	13520029 	cmpne	r2, #41	; 0x29
    c500:	0a00000e 	beq	c540 <run_command+0x208>
    c504:	ea00000e 	b	c544 <run_command+0x20c>
    c508:	e3520027 	cmp	r2, #39	; 0x27
    c50c:	13a03000 	movne	r3, #0	; 0x0
    c510:	03a03001 	moveq	r3, #1	; 0x1
    c514:	e351005c 	cmp	r1, #92	; 0x5c
    c518:	03a03000 	moveq	r3, #0	; 0x0
    c51c:	e3530000 	cmp	r3, #0	; 0x0
    c520:	0a000002 	beq	c530 <run_command+0x1f8>
    c524:	e3a0c000 	mov	ip, #0	; 0x0
    c528:	e3a02027 	mov	r2, #39	; 0x27
    c52c:	ea000004 	b	c544 <run_command+0x20c>
    c530:	e4c52001 	strb	r2, [r5], #1
    c534:	e24ee001 	sub	lr, lr, #1	; 0x1
    c538:	e3a0c003 	mov	ip, #3	; 0x3
    c53c:	ea000000 	b	c544 <run_command+0x20c>
    c540:	e3a0c000 	mov	ip, #0	; 0x0
    c544:	e1a01002 	mov	r1, r2
    c548:	e3500000 	cmp	r0, #0	; 0x0
    c54c:	135e0000 	cmpne	lr, #0	; 0x0
    c550:	1affffb2 	bne	c420 <run_command+0xe8>
    c554:	e35e0000 	cmp	lr, #0	; 0x0
    c558:	15c5b000 	strbne	fp, [r5]
    c55c:	0545e001 	strbeq	lr, [r5, #-1]
    c560:	e28d0004 	add	r0, sp, #4	; 0x4
    c564:	e1a01009 	mov	r1, r9
    c568:	ebffff4c 	bl	c2a0 <parse_line>
    c56c:	e2505000 	subs	r5, r0, #0	; 0x0
    c570:	0a00000d 	beq	c5ac <run_command+0x274>
    c574:	e59d0204 	ldr	r0, [sp, #516]
    c578:	ebffd976 	bl	2b58 <find_cmd>
    c57c:	e2504000 	subs	r4, r0, #0	; 0x0
    c580:	1a000005 	bne	c59c <run_command+0x264>
    c584:	e59f0090 	ldr	r0, [pc, #144]	; c61c <run_command+0x2e4>
    c588:	e59d1204 	ldr	r1, [sp, #516]
    c58c:	e59f308c 	ldr	r3, [pc, #140]	; c620 <run_command+0x2e8>
    c590:	e1a0e00f 	mov	lr, pc
    c594:	e593f000 	ldr	pc, [r3]
    c598:	ea000003 	b	c5ac <run_command+0x274>
    c59c:	e5943004 	ldr	r3, [r4, #4]
    c5a0:	e1550003 	cmp	r5, r3
    c5a4:	da000002 	ble	c5b4 <run_command+0x27c>
    c5a8:	ebffd924 	bl	2a40 <cmd_usage>
    c5ac:	e3e06000 	mvn	r6, #0	; 0x0
    c5b0:	ea00000b 	b	c5e4 <run_command+0x2ac>
    c5b4:	e1a02005 	mov	r2, r5
    c5b8:	e1a0100a 	mov	r1, sl
    c5bc:	e1a03009 	mov	r3, r9
    c5c0:	e1a0e00f 	mov	lr, pc
    c5c4:	e594f00c 	ldr	pc, [r4, #12]
    c5c8:	e3500000 	cmp	r0, #0	; 0x0
    c5cc:	13e06000 	mvnne	r6, #0	; 0x0
    c5d0:	e5944008 	ldr	r4, [r4, #8]
    c5d4:	ebfffee5 	bl	c170 <had_ctrlc>
    c5d8:	e3500000 	cmp	r0, #0	; 0x0
    c5dc:	1a000009 	bne	c608 <run_command+0x2d0>
    c5e0:	e0077004 	and	r7, r7, r4
    c5e4:	e1a04008 	mov	r4, r8
    c5e8:	e5d43000 	ldrb	r3, [r4]
    c5ec:	e3530000 	cmp	r3, #0	; 0x0
    c5f0:	11a02004 	movne	r2, r4
    c5f4:	13a00000 	movne	r0, #0	; 0x0
    c5f8:	1affff7a 	bne	c3e8 <run_command+0xb0>
    c5fc:	e3560000 	cmp	r6, #0	; 0x0
    c600:	01a06007 	moveq	r6, r7
    c604:	ea000000 	b	c60c <run_command+0x2d4>
    c608:	e3e06000 	mvn	r6, #0	; 0x0
    c60c:	e1a00006 	mov	r0, r6
    c610:	e28ddf93 	add	sp, sp, #588	; 0x24c
    c614:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c618:	000117ba 	.word	0x000117ba
    c61c:	000117d0 	.word	0x000117d0
    c620:	3c00d68c 	.word	0x3c00d68c

0000c624 <cread_add_char>:
    c624:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    c628:	e3510000 	cmp	r1, #0	; 0x0
    c62c:	e24dd004 	sub	sp, sp, #4	; 0x4
    c630:	e1a07000 	mov	r7, r0
    c634:	e1a04002 	mov	r4, r2
    c638:	e1a00003 	mov	r0, r3
    c63c:	e59d6018 	ldr	r6, [sp, #24]
    c640:	e59dc01c 	ldr	ip, [sp, #28]
    c644:	1a000003 	bne	c658 <cread_add_char+0x34>
    c648:	e5922000 	ldr	r2, [r2]
    c64c:	e5933000 	ldr	r3, [r3]
    c650:	e1520003 	cmp	r2, r3
    c654:	1a000026 	bne	c6f4 <cread_add_char+0xd0>
    c658:	e5902000 	ldr	r2, [r0]
    c65c:	e24c3001 	sub	r3, ip, #1	; 0x1
    c660:	e1520003 	cmp	r2, r3
    c664:	9a000003 	bls	c678 <cread_add_char+0x54>
    c668:	e3a00007 	mov	r0, #7	; 0x7
    c66c:	e28dd004 	add	sp, sp, #4	; 0x4
    c670:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    c674:	eafffef1 	b	c240 <putc>
    c678:	e3510000 	cmp	r1, #0	; 0x0
    c67c:	e2821001 	add	r1, r2, #1	; 0x1
    c680:	e5801000 	str	r1, [r0]
    c684:	0a00001a 	beq	c6f4 <cread_add_char+0xd0>
    c688:	e5943000 	ldr	r3, [r4]
    c68c:	e0635001 	rsb	r5, r3, r1
    c690:	e3550001 	cmp	r5, #1	; 0x1
    c694:	9a000004 	bls	c6ac <cread_add_char+0x88>
    c698:	e2830001 	add	r0, r3, #1	; 0x1
    c69c:	e0860000 	add	r0, r6, r0
    c6a0:	e0861003 	add	r1, r6, r3
    c6a4:	e2452001 	sub	r2, r5, #1	; 0x1
    c6a8:	ebffd2cf 	bl	11ec <memmove>
    c6ac:	e5943000 	ldr	r3, [r4]
    c6b0:	e59f0074 	ldr	r0, [pc, #116]	; c72c <cread_add_char+0x108>
    c6b4:	e7c67003 	strb	r7, [r6, r3]
    c6b8:	e5942000 	ldr	r2, [r4]
    c6bc:	e59f306c 	ldr	r3, [pc, #108]	; c730 <cread_add_char+0x10c>
    c6c0:	e0862002 	add	r2, r6, r2
    c6c4:	e1a01005 	mov	r1, r5
    c6c8:	e1a0e00f 	mov	lr, pc
    c6cc:	e593f000 	ldr	pc, [r3]
    c6d0:	e5943000 	ldr	r3, [r4]
    c6d4:	e2833001 	add	r3, r3, #1	; 0x1
    c6d8:	e5843000 	str	r3, [r4]
    c6dc:	ea000000 	b	c6e4 <cread_add_char+0xc0>
    c6e0:	ebfffed6 	bl	c240 <putc>
    c6e4:	e2555001 	subs	r5, r5, #1	; 0x1
    c6e8:	e3a00008 	mov	r0, #8	; 0x8
    c6ec:	1afffffb 	bne	c6e0 <cread_add_char+0xbc>
    c6f0:	ea00000b 	b	c724 <cread_add_char+0x100>
    c6f4:	e5943000 	ldr	r3, [r4]
    c6f8:	e59f002c 	ldr	r0, [pc, #44]	; c72c <cread_add_char+0x108>
    c6fc:	e7c67003 	strb	r7, [r6, r3]
    c700:	e5942000 	ldr	r2, [r4]
    c704:	e59f3024 	ldr	r3, [pc, #36]	; c730 <cread_add_char+0x10c>
    c708:	e0862002 	add	r2, r6, r2
    c70c:	e3a01001 	mov	r1, #1	; 0x1
    c710:	e1a0e00f 	mov	lr, pc
    c714:	e593f000 	ldr	pc, [r3]
    c718:	e5943000 	ldr	r3, [r4]
    c71c:	e2833001 	add	r3, r3, #1	; 0x1
    c720:	e5843000 	str	r3, [r4]
    c724:	e28dd004 	add	sp, sp, #4	; 0x4
    c728:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    c72c:	000117f3 	.word	0x000117f3
    c730:	3c00d68c 	.word	0x3c00d68c

0000c734 <readline_into_buffer>:
    c734:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c738:	e59f367c 	ldr	r3, [pc, #1660]	; cdbc <readline_into_buffer+0x688>
    c73c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    c740:	e593e000 	ldr	lr, [r3]
    c744:	e1a06001 	mov	r6, r1
    c748:	e35e0000 	cmp	lr, #0	; 0x0
    c74c:	1a000014 	bne	c7a4 <readline_into_buffer+0x70>
    c750:	e59f3668 	ldr	r3, [pc, #1640]	; cdc0 <readline_into_buffer+0x68c>
    c754:	e3e02000 	mvn	r2, #0	; 0x0
    c758:	e5832000 	str	r2, [r3]
    c75c:	e59f3660 	ldr	r3, [pc, #1632]	; cdc4 <readline_into_buffer+0x690>
    c760:	e59fc660 	ldr	ip, [pc, #1632]	; cdc8 <readline_into_buffer+0x694>
    c764:	e583e000 	str	lr, [r3]
    c768:	e59f365c 	ldr	r3, [pc, #1628]	; cdcc <readline_into_buffer+0x698>
    c76c:	e59f465c 	ldr	r4, [pc, #1628]	; cdd0 <readline_into_buffer+0x69c>
    c770:	e583e000 	str	lr, [r3]
    c774:	e59f3658 	ldr	r3, [pc, #1624]	; cdd4 <readline_into_buffer+0x6a0>
    c778:	e1a0100e 	mov	r1, lr
    c77c:	e583e000 	str	lr, [r3]
    c780:	e1a0300e 	mov	r3, lr
    c784:	e784c001 	str	ip, [r4, r1]
    c788:	e2811004 	add	r1, r1, #4	; 0x4
    c78c:	e3510010 	cmp	r1, #16	; 0x10
    c790:	e4cc3100 	strb	r3, [ip], #256
    c794:	1afffffa 	bne	c784 <readline_into_buffer+0x50>
    c798:	e59f361c 	ldr	r3, [pc, #1564]	; cdbc <readline_into_buffer+0x688>
    c79c:	e3a02001 	mov	r2, #1	; 0x1
    c7a0:	e5832000 	str	r2, [r3]
    c7a4:	ebfffe7d 	bl	c1a0 <puts>
    c7a8:	e3a03000 	mov	r3, #0	; 0x0
    c7ac:	e58d3014 	str	r3, [sp, #20]
    c7b0:	e58d3010 	str	r3, [sp, #16]
    c7b4:	e1a05003 	mov	r5, r3
    c7b8:	e3a09001 	mov	r9, #1	; 0x1
    c7bc:	e59fb614 	ldr	fp, [pc, #1556]	; cdd8 <readline_into_buffer+0x6a4>
    c7c0:	e59fa5f8 	ldr	sl, [pc, #1528]	; cdc0 <readline_into_buffer+0x68c>
    c7c4:	ea000000 	b	c7cc <readline_into_buffer+0x98>
    c7c8:	e3a05000 	mov	r5, #0	; 0x0
    c7cc:	ebfffe9d 	bl	c248 <getc>
    c7d0:	e6ef0070 	uxtb	r0, r0
    c7d4:	e350000d 	cmp	r0, #13	; 0xd
    c7d8:	1350000a 	cmpne	r0, #10	; 0xa
    c7dc:	13a04000 	movne	r4, #0	; 0x0
    c7e0:	03a04001 	moveq	r4, #1	; 0x1
    c7e4:	1a00000b 	bne	c818 <readline_into_buffer+0xe4>
    c7e8:	e59f35e8 	ldr	r3, [pc, #1512]	; cdd8 <readline_into_buffer+0x6a4>
    c7ec:	e59f05e8 	ldr	r0, [pc, #1512]	; cddc <readline_into_buffer+0x6a8>
    c7f0:	e1a0e00f 	mov	lr, pc
    c7f4:	e593f000 	ldr	pc, [r3]
    c7f8:	e59d5010 	ldr	r5, [sp, #16]
    c7fc:	e3a03000 	mov	r3, #0	; 0x0
    c800:	e7c63005 	strb	r3, [r6, r5]
    c804:	e5d63000 	ldrb	r3, [r6]
    c808:	e3530000 	cmp	r3, #0	; 0x0
    c80c:	13530021 	cmpne	r3, #33	; 0x21
    c810:	0a000162 	beq	cda0 <readline_into_buffer+0x66c>
    c814:	ea00014e 	b	cd54 <readline_into_buffer+0x620>
    c818:	e3550000 	cmp	r5, #0	; 0x0
    c81c:	0a000033 	beq	c8f0 <readline_into_buffer+0x1bc>
    c820:	e3550001 	cmp	r5, #1	; 0x1
    c824:	1a00000c 	bne	c85c <readline_into_buffer+0x128>
    c828:	e350005b 	cmp	r0, #91	; 0x5b
    c82c:	05cd0009 	strbeq	r0, [sp, #9]
    c830:	03a05002 	moveq	r5, #2	; 0x2
    c834:	0affffe4 	beq	c7cc <readline_into_buffer+0x98>
    c838:	e3a0cc01 	mov	ip, #256	; 0x100
    c83c:	e5dd0008 	ldrb	r0, [sp, #8]
    c840:	e1a01009 	mov	r1, r9
    c844:	e28d2014 	add	r2, sp, #20	; 0x14
    c848:	e28d3010 	add	r3, sp, #16	; 0x10
    c84c:	e88d1040 	stm	sp, {r6, ip}
    c850:	e1a05004 	mov	r5, r4
    c854:	ebffff72 	bl	c624 <cread_add_char>
    c858:	eaffffdb 	b	c7cc <readline_into_buffer+0x98>
    c85c:	e2403041 	sub	r3, r0, #65	; 0x41
    c860:	e3530007 	cmp	r3, #7	; 0x7
    c864:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    c868:	ea000009 	b	c894 <readline_into_buffer+0x160>
    c86c:	0000cbd8 	.word	0x0000cbd8
    c870:	0000cc18 	.word	0x0000cc18
    c874:	0000c8e0 	.word	0x0000c8e0
    c878:	0000c9ec 	.word	0x0000c9ec
    c87c:	0000c894 	.word	0x0000c894
    c880:	0000c894 	.word	0x0000c894
    c884:	0000c894 	.word	0x0000c894
    c888:	0000c88c 	.word	0x0000c88c
    c88c:	e3a05000 	mov	r5, #0	; 0x0
    c890:	ea000043 	b	c9a4 <readline_into_buffer+0x270>
    c894:	e28d1018 	add	r1, sp, #24	; 0x18
    c898:	e0813005 	add	r3, r1, r5
    c89c:	e5430010 	strb	r0, [r3, #-16]
    c8a0:	e2855001 	add	r5, r5, #1	; 0x1
    c8a4:	e3a04000 	mov	r4, #0	; 0x0
    c8a8:	e28d8010 	add	r8, sp, #16	; 0x10
    c8ac:	e3a07c01 	mov	r7, #256	; 0x100
    c8b0:	ea000004 	b	c8c8 <readline_into_buffer+0x194>
    c8b4:	e28dc008 	add	ip, sp, #8	; 0x8
    c8b8:	e7dc0004 	ldrb	r0, [ip, r4]
    c8bc:	e88d00c0 	stm	sp, {r6, r7}
    c8c0:	ebffff57 	bl	c624 <cread_add_char>
    c8c4:	e2844001 	add	r4, r4, #1	; 0x1
    c8c8:	e1540005 	cmp	r4, r5
    c8cc:	e1a01009 	mov	r1, r9
    c8d0:	e28d2014 	add	r2, sp, #20	; 0x14
    c8d4:	e1a03008 	mov	r3, r8
    c8d8:	1afffff5 	bne	c8b4 <readline_into_buffer+0x180>
    c8dc:	eaffffb9 	b	c7c8 <readline_into_buffer+0x94>
    c8e0:	e3a00006 	mov	r0, #6	; 0x6
    c8e4:	e350000b 	cmp	r0, #11	; 0xb
    c8e8:	e3a05000 	mov	r5, #0	; 0x0
    c8ec:	ea000001 	b	c8f8 <readline_into_buffer+0x1c4>
    c8f0:	e350000b 	cmp	r0, #11	; 0xb
    c8f4:	0a00005f 	beq	ca78 <readline_into_buffer+0x344>
    c8f8:	8a00000d 	bhi	c934 <readline_into_buffer+0x200>
    c8fc:	e3500004 	cmp	r0, #4	; 0x4
    c900:	0a000042 	beq	ca10 <readline_into_buffer+0x2dc>
    c904:	8a000005 	bhi	c920 <readline_into_buffer+0x1ec>
    c908:	e3500002 	cmp	r0, #2	; 0x2
    c90c:	0a000037 	beq	c9f0 <readline_into_buffer+0x2bc>
    c910:	8a000028 	bhi	c9b8 <readline_into_buffer+0x284>
    c914:	e3500001 	cmp	r0, #1	; 0x1
    c918:	1a000106 	bne	cd38 <readline_into_buffer+0x604>
    c91c:	ea000020 	b	c9a4 <readline_into_buffer+0x270>
    c920:	e3500006 	cmp	r0, #6	; 0x6
    c924:	0a000027 	beq	c9c8 <readline_into_buffer+0x294>
    c928:	3a000066 	bcc	cac8 <readline_into_buffer+0x394>
    c92c:	e3500008 	cmp	r0, #8	; 0x8
    c930:	ea000015 	b	c98c <readline_into_buffer+0x258>
    c934:	e3500015 	cmp	r0, #21	; 0x15
    c938:	0a000070 	beq	cb00 <readline_into_buffer+0x3cc>
    c93c:	8a000007 	bhi	c960 <readline_into_buffer+0x22c>
    c940:	e350000f 	cmp	r0, #15	; 0xf
    c944:	02299001 	eoreq	r9, r9, #1	; 0x1
    c948:	0affff9f 	beq	c7cc <readline_into_buffer+0x98>
    c94c:	e3500010 	cmp	r0, #16	; 0x10
    c950:	0a0000a0 	beq	cbd8 <readline_into_buffer+0x4a4>
    c954:	e350000e 	cmp	r0, #14	; 0xe
    c958:	1a0000f6 	bne	cd38 <readline_into_buffer+0x604>
    c95c:	ea0000ad 	b	cc18 <readline_into_buffer+0x4e4>
    c960:	e350001b 	cmp	r0, #27	; 0x1b
    c964:	05cd0008 	strbeq	r0, [sp, #8]
    c968:	03a05001 	moveq	r5, #1	; 0x1
    c96c:	0affff96 	beq	c7cc <readline_into_buffer+0x98>
    c970:	8a000002 	bhi	c980 <readline_into_buffer+0x24c>
    c974:	e3500018 	cmp	r0, #24	; 0x18
    c978:	1a0000ee 	bne	cd38 <readline_into_buffer+0x604>
    c97c:	ea00005f 	b	cb00 <readline_into_buffer+0x3cc>
    c980:	e350007f 	cmp	r0, #127	; 0x7f
    c984:	0a000075 	beq	cb60 <readline_into_buffer+0x42c>
    c988:	e35000ff 	cmp	r0, #255	; 0xff
    c98c:	1a0000e9 	bne	cd38 <readline_into_buffer+0x604>
    c990:	ea000072 	b	cb60 <readline_into_buffer+0x42c>
    c994:	ebfffe29 	bl	c240 <putc>
    c998:	e59d3014 	ldr	r3, [sp, #20]
    c99c:	e2433001 	sub	r3, r3, #1	; 0x1
    c9a0:	e58d3014 	str	r3, [sp, #20]
    c9a4:	e59d3014 	ldr	r3, [sp, #20]
    c9a8:	e3a00008 	mov	r0, #8	; 0x8
    c9ac:	e3530000 	cmp	r3, #0	; 0x0
    c9b0:	1afffff7 	bne	c994 <readline_into_buffer+0x260>
    c9b4:	eaffff84 	b	c7cc <readline_into_buffer+0x98>
    c9b8:	e3a03000 	mov	r3, #0	; 0x0
    c9bc:	e5c63000 	strb	r3, [r6]
    c9c0:	e3e00000 	mvn	r0, #0	; 0x0
    c9c4:	ea0000fa 	b	cdb4 <readline_into_buffer+0x680>
    c9c8:	e59d2014 	ldr	r2, [sp, #20]
    c9cc:	e59d3010 	ldr	r3, [sp, #16]
    c9d0:	e1520003 	cmp	r2, r3
    c9d4:	2affff7c 	bcs	c7cc <readline_into_buffer+0x98>
    c9d8:	e7d60002 	ldrb	r0, [r6, r2]
    c9dc:	ebfffe17 	bl	c240 <putc>
    c9e0:	e59d3014 	ldr	r3, [sp, #20]
    c9e4:	e2833001 	add	r3, r3, #1	; 0x1
    c9e8:	ea0000d0 	b	cd30 <readline_into_buffer+0x5fc>
    c9ec:	e3a05000 	mov	r5, #0	; 0x0
    c9f0:	e59d3014 	ldr	r3, [sp, #20]
    c9f4:	e3530000 	cmp	r3, #0	; 0x0
    c9f8:	0affff73 	beq	c7cc <readline_into_buffer+0x98>
    c9fc:	e3a00008 	mov	r0, #8	; 0x8
    ca00:	ebfffe0e 	bl	c240 <putc>
    ca04:	e59d3014 	ldr	r3, [sp, #20]
    ca08:	e2433001 	sub	r3, r3, #1	; 0x1
    ca0c:	ea0000c7 	b	cd30 <readline_into_buffer+0x5fc>
    ca10:	e59d2014 	ldr	r2, [sp, #20]
    ca14:	e59d3010 	ldr	r3, [sp, #16]
    ca18:	e1520003 	cmp	r2, r3
    ca1c:	2affff6a 	bcs	c7cc <readline_into_buffer+0x98>
    ca20:	e2433001 	sub	r3, r3, #1	; 0x1
    ca24:	e0534002 	subs	r4, r3, r2
    ca28:	0a00000a 	beq	ca58 <readline_into_buffer+0x324>
    ca2c:	e2821001 	add	r1, r2, #1	; 0x1
    ca30:	e0861001 	add	r1, r6, r1
    ca34:	e0860002 	add	r0, r6, r2
    ca38:	e1a02004 	mov	r2, r4
    ca3c:	ebffd1ea 	bl	11ec <memmove>
    ca40:	e59d2014 	ldr	r2, [sp, #20]
    ca44:	e59f0394 	ldr	r0, [pc, #916]	; cde0 <readline_into_buffer+0x6ac>
    ca48:	e0862002 	add	r2, r6, r2
    ca4c:	e1a01004 	mov	r1, r4
    ca50:	e1a0e00f 	mov	lr, pc
    ca54:	e59bf000 	ldr	pc, [fp]
    ca58:	e3a00020 	mov	r0, #32	; 0x20
    ca5c:	ebfffdf7 	bl	c240 <putc>
    ca60:	e2444001 	sub	r4, r4, #1	; 0x1
    ca64:	e3a00008 	mov	r0, #8	; 0x8
    ca68:	ebfffdf4 	bl	c240 <putc>
    ca6c:	e3740001 	cmn	r4, #1	; 0x1
    ca70:	1afffffa 	bne	ca60 <readline_into_buffer+0x32c>
    ca74:	ea000053 	b	cbc8 <readline_into_buffer+0x494>
    ca78:	e59d2014 	ldr	r2, [sp, #20]
    ca7c:	e59d3010 	ldr	r3, [sp, #16]
    ca80:	e1520003 	cmp	r2, r3
    ca84:	31a04002 	movcc	r4, r2
    ca88:	3a000002 	bcc	ca98 <readline_into_buffer+0x364>
    ca8c:	eaffff4e 	b	c7cc <readline_into_buffer+0x98>
    ca90:	ebfffdea 	bl	c240 <putc>
    ca94:	e2844001 	add	r4, r4, #1	; 0x1
    ca98:	e59d3010 	ldr	r3, [sp, #16]
    ca9c:	e3a00020 	mov	r0, #32	; 0x20
    caa0:	e1540003 	cmp	r4, r3
    caa4:	3afffff9 	bcc	ca90 <readline_into_buffer+0x35c>
    caa8:	ea000000 	b	cab0 <readline_into_buffer+0x37c>
    caac:	ebfffde3 	bl	c240 <putc>
    cab0:	e59d2014 	ldr	r2, [sp, #20]
    cab4:	e3a00008 	mov	r0, #8	; 0x8
    cab8:	e1540002 	cmp	r4, r2
    cabc:	e2444001 	sub	r4, r4, #1	; 0x1
    cac0:	8afffff9 	bhi	caac <readline_into_buffer+0x378>
    cac4:	ea000023 	b	cb58 <readline_into_buffer+0x424>
    cac8:	e59d3014 	ldr	r3, [sp, #20]
    cacc:	e59d1010 	ldr	r1, [sp, #16]
    cad0:	e1530001 	cmp	r3, r1
    cad4:	2affff3c 	bcs	c7cc <readline_into_buffer+0x98>
    cad8:	e0631001 	rsb	r1, r3, r1
    cadc:	e0862003 	add	r2, r6, r3
    cae0:	e59f02f8 	ldr	r0, [pc, #760]	; cde0 <readline_into_buffer+0x6ac>
    cae4:	e1a0e00f 	mov	lr, pc
    cae8:	e59bf000 	ldr	pc, [fp]
    caec:	ea00008e 	b	cd2c <readline_into_buffer+0x5f8>
    caf0:	ebfffdd2 	bl	c240 <putc>
    caf4:	e59d3014 	ldr	r3, [sp, #20]
    caf8:	e2433001 	sub	r3, r3, #1	; 0x1
    cafc:	e58d3014 	str	r3, [sp, #20]
    cb00:	e59d2014 	ldr	r2, [sp, #20]
    cb04:	e3a00008 	mov	r0, #8	; 0x8
    cb08:	e3520000 	cmp	r2, #0	; 0x0
    cb0c:	1afffff7 	bne	caf0 <readline_into_buffer+0x3bc>
    cb10:	e59d3010 	ldr	r3, [sp, #16]
    cb14:	e3530000 	cmp	r3, #0	; 0x0
    cb18:	11a04002 	movne	r4, r2
    cb1c:	1a000002 	bne	cb2c <readline_into_buffer+0x3f8>
    cb20:	eaffff29 	b	c7cc <readline_into_buffer+0x98>
    cb24:	ebfffdc5 	bl	c240 <putc>
    cb28:	e2844001 	add	r4, r4, #1	; 0x1
    cb2c:	e59d3010 	ldr	r3, [sp, #16]
    cb30:	e3a00020 	mov	r0, #32	; 0x20
    cb34:	e1540003 	cmp	r4, r3
    cb38:	3afffff9 	bcc	cb24 <readline_into_buffer+0x3f0>
    cb3c:	ea000000 	b	cb44 <readline_into_buffer+0x410>
    cb40:	ebfffdbe 	bl	c240 <putc>
    cb44:	e59d2014 	ldr	r2, [sp, #20]
    cb48:	e3a00008 	mov	r0, #8	; 0x8
    cb4c:	e1540002 	cmp	r4, r2
    cb50:	e2444001 	sub	r4, r4, #1	; 0x1
    cb54:	8afffff9 	bhi	cb40 <readline_into_buffer+0x40c>
    cb58:	e58d2010 	str	r2, [sp, #16]
    cb5c:	eaffff1a 	b	c7cc <readline_into_buffer+0x98>
    cb60:	e59d0014 	ldr	r0, [sp, #20]
    cb64:	e3500000 	cmp	r0, #0	; 0x0
    cb68:	0affff17 	beq	c7cc <readline_into_buffer+0x98>
    cb6c:	e59d2010 	ldr	r2, [sp, #16]
    cb70:	e2403001 	sub	r3, r0, #1	; 0x1
    cb74:	e0604002 	rsb	r4, r0, r2
    cb78:	e0861000 	add	r1, r6, r0
    cb7c:	e1a02004 	mov	r2, r4
    cb80:	e0860003 	add	r0, r6, r3
    cb84:	e58d3014 	str	r3, [sp, #20]
    cb88:	ebffd197 	bl	11ec <memmove>
    cb8c:	e3a00008 	mov	r0, #8	; 0x8
    cb90:	ebfffdaa 	bl	c240 <putc>
    cb94:	e59d2014 	ldr	r2, [sp, #20]
    cb98:	e59f0240 	ldr	r0, [pc, #576]	; cde0 <readline_into_buffer+0x6ac>
    cb9c:	e0862002 	add	r2, r6, r2
    cba0:	e1a01004 	mov	r1, r4
    cba4:	e1a0e00f 	mov	lr, pc
    cba8:	e59bf000 	ldr	pc, [fp]
    cbac:	e3a00020 	mov	r0, #32	; 0x20
    cbb0:	ebfffda2 	bl	c240 <putc>
    cbb4:	e2444001 	sub	r4, r4, #1	; 0x1
    cbb8:	e3a00008 	mov	r0, #8	; 0x8
    cbbc:	ebfffd9f 	bl	c240 <putc>
    cbc0:	e3740001 	cmn	r4, #1	; 0x1
    cbc4:	1afffffa 	bne	cbb4 <readline_into_buffer+0x480>
    cbc8:	e59d3010 	ldr	r3, [sp, #16]
    cbcc:	e2433001 	sub	r3, r3, #1	; 0x1
    cbd0:	e58d3010 	str	r3, [sp, #16]
    cbd4:	eafffefc 	b	c7cc <readline_into_buffer+0x98>
    cbd8:	e59a2000 	ldr	r2, [sl]
    cbdc:	e3520000 	cmp	r2, #0	; 0x0
    cbe0:	ba000024 	blt	cc78 <readline_into_buffer+0x544>
    cbe4:	e2423001 	sub	r3, r2, #1	; 0x1
    cbe8:	e3730001 	cmn	r3, #1	; 0x1
    cbec:	059f01d0 	ldreq	r0, [pc, #464]	; cdc4 <readline_into_buffer+0x690>
    cbf0:	e58a3000 	str	r3, [sl]
    cbf4:	05903000 	ldreq	r3, [r0]
    cbf8:	e59fc1cc 	ldr	ip, [pc, #460]	; cdcc <readline_into_buffer+0x698>
    cbfc:	058a3000 	streq	r3, [sl]
    cc00:	e59a1000 	ldr	r1, [sl]
    cc04:	e59c3000 	ldr	r3, [ip]
    cc08:	e1510003 	cmp	r1, r3
    cc0c:	058a2000 	streq	r2, [sl]
    cc10:	1a000014 	bne	cc68 <readline_into_buffer+0x534>
    cc14:	ea000017 	b	cc78 <readline_into_buffer+0x544>
    cc18:	e59a1000 	ldr	r1, [sl]
    cc1c:	e3510000 	cmp	r1, #0	; 0x0
    cc20:	ba000014 	blt	cc78 <readline_into_buffer+0x544>
    cc24:	e59f21a0 	ldr	r2, [pc, #416]	; cdcc <readline_into_buffer+0x698>
    cc28:	e5923000 	ldr	r3, [r2]
    cc2c:	e1510003 	cmp	r1, r3
    cc30:	0a000010 	beq	cc78 <readline_into_buffer+0x544>
    cc34:	e59f3188 	ldr	r3, [pc, #392]	; cdc4 <readline_into_buffer+0x690>
    cc38:	e59fc18c 	ldr	ip, [pc, #396]	; cdcc <readline_into_buffer+0x698>
    cc3c:	e5932000 	ldr	r2, [r3]
    cc40:	e2813001 	add	r3, r1, #1	; 0x1
    cc44:	e1530002 	cmp	r3, r2
    cc48:	e58a3000 	str	r3, [sl]
    cc4c:	c3a03000 	movgt	r3, #0	; 0x0
    cc50:	c58a3000 	strgt	r3, [sl]
    cc54:	e59a1000 	ldr	r1, [sl]
    cc58:	e59c3000 	ldr	r3, [ip]
    cc5c:	e1510003 	cmp	r1, r3
    cc60:	059f517c 	ldreq	r5, [pc, #380]	; cde4 <readline_into_buffer+0x6b0>
    cc64:	0a00000a 	beq	cc94 <readline_into_buffer+0x560>
    cc68:	e59f0160 	ldr	r0, [pc, #352]	; cdd0 <readline_into_buffer+0x69c>
    cc6c:	e7905101 	ldr	r5, [r0, r1, lsl #2]
    cc70:	e3550000 	cmp	r5, #0	; 0x0
    cc74:	1a000006 	bne	cc94 <readline_into_buffer+0x560>
    cc78:	e3a00007 	mov	r0, #7	; 0x7
    cc7c:	ebfffd6f 	bl	c240 <putc>
    cc80:	eafffed0 	b	c7c8 <readline_into_buffer+0x94>
    cc84:	ebfffd6d 	bl	c240 <putc>
    cc88:	e59d3014 	ldr	r3, [sp, #20]
    cc8c:	e2433001 	sub	r3, r3, #1	; 0x1
    cc90:	e58d3014 	str	r3, [sp, #20]
    cc94:	e59d2014 	ldr	r2, [sp, #20]
    cc98:	e3a00008 	mov	r0, #8	; 0x8
    cc9c:	e3520000 	cmp	r2, #0	; 0x0
    cca0:	1afffff7 	bne	cc84 <readline_into_buffer+0x550>
    cca4:	e59d3010 	ldr	r3, [sp, #16]
    cca8:	e3530000 	cmp	r3, #0	; 0x0
    ccac:	11a04002 	movne	r4, r2
    ccb0:	1a000002 	bne	ccc0 <readline_into_buffer+0x58c>
    ccb4:	ea00000d 	b	ccf0 <readline_into_buffer+0x5bc>
    ccb8:	ebfffd60 	bl	c240 <putc>
    ccbc:	e2844001 	add	r4, r4, #1	; 0x1
    ccc0:	e59d3010 	ldr	r3, [sp, #16]
    ccc4:	e3a00020 	mov	r0, #32	; 0x20
    ccc8:	e1540003 	cmp	r4, r3
    cccc:	3afffff9 	bcc	ccb8 <readline_into_buffer+0x584>
    ccd0:	ea000000 	b	ccd8 <readline_into_buffer+0x5a4>
    ccd4:	ebfffd59 	bl	c240 <putc>
    ccd8:	e59d2014 	ldr	r2, [sp, #20]
    ccdc:	e3a00008 	mov	r0, #8	; 0x8
    cce0:	e1540002 	cmp	r4, r2
    cce4:	e2444001 	sub	r4, r4, #1	; 0x1
    cce8:	8afffff9 	bhi	ccd4 <readline_into_buffer+0x5a0>
    ccec:	e58d2010 	str	r2, [sp, #16]
    ccf0:	e1a01005 	mov	r1, r5
    ccf4:	e1a00006 	mov	r0, r6
    ccf8:	ebffd0c1 	bl	1004 <strcpy>
    ccfc:	e1a00006 	mov	r0, r6
    cd00:	ebffd116 	bl	1160 <strlen>
    cd04:	e59d3014 	ldr	r3, [sp, #20]
    cd08:	e58d0010 	str	r0, [sp, #16]
    cd0c:	e1530000 	cmp	r3, r0
    cd10:	2afffeac 	bcs	c7c8 <readline_into_buffer+0x94>
    cd14:	e0631000 	rsb	r1, r3, r0
    cd18:	e0862003 	add	r2, r6, r3
    cd1c:	e59f00bc 	ldr	r0, [pc, #188]	; cde0 <readline_into_buffer+0x6ac>
    cd20:	e1a0e00f 	mov	lr, pc
    cd24:	e59bf000 	ldr	pc, [fp]
    cd28:	e3a05000 	mov	r5, #0	; 0x0
    cd2c:	e59d3010 	ldr	r3, [sp, #16]
    cd30:	e58d3014 	str	r3, [sp, #20]
    cd34:	eafffea4 	b	c7cc <readline_into_buffer+0x98>
    cd38:	e3a0cc01 	mov	ip, #256	; 0x100
    cd3c:	e1a01009 	mov	r1, r9
    cd40:	e28d2014 	add	r2, sp, #20	; 0x14
    cd44:	e28d3010 	add	r3, sp, #16	; 0x10
    cd48:	e88d1040 	stm	sp, {r6, ip}
    cd4c:	ebfffe34 	bl	c624 <cread_add_char>
    cd50:	eafffe9d 	b	c7cc <readline_into_buffer+0x98>
    cd54:	e59f4070 	ldr	r4, [pc, #112]	; cdcc <readline_into_buffer+0x698>
    cd58:	e59f3070 	ldr	r3, [pc, #112]	; cdd0 <readline_into_buffer+0x69c>
    cd5c:	e5942000 	ldr	r2, [r4]
    cd60:	e1a01006 	mov	r1, r6
    cd64:	e7930102 	ldr	r0, [r3, r2, lsl #2]
    cd68:	ebffd0a5 	bl	1004 <strcpy>
    cd6c:	e5943000 	ldr	r3, [r4]
    cd70:	e59f104c 	ldr	r1, [pc, #76]	; cdc4 <readline_into_buffer+0x690>
    cd74:	e2833001 	add	r3, r3, #1	; 0x1
    cd78:	e5912000 	ldr	r2, [r1]
    cd7c:	e3530003 	cmp	r3, #3	; 0x3
    cd80:	c3a03000 	movgt	r3, #0	; 0x0
    cd84:	e1530002 	cmp	r3, r2
    cd88:	e59f2044 	ldr	r2, [pc, #68]	; cdd4 <readline_into_buffer+0x6a0>
    cd8c:	e5843000 	str	r3, [r4]
    cd90:	c5813000 	strgt	r3, [r1]
    cd94:	e5923000 	ldr	r3, [r2]
    cd98:	e2833001 	add	r3, r3, #1	; 0x1
    cd9c:	e5823000 	str	r3, [r2]
    cda0:	e59f3024 	ldr	r3, [pc, #36]	; cdcc <readline_into_buffer+0x698>
    cda4:	e1a00005 	mov	r0, r5
    cda8:	e5932000 	ldr	r2, [r3]
    cdac:	e59f300c 	ldr	r3, [pc, #12]	; cdc0 <readline_into_buffer+0x68c>
    cdb0:	e5832000 	str	r2, [r3]
    cdb4:	e28dd01c 	add	sp, sp, #28	; 0x1c
    cdb8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    cdbc:	3c00d234 	.word	0x3c00d234
    cdc0:	3c00d128 	.word	0x3c00d128
    cdc4:	3c00d238 	.word	0x3c00d238
    cdc8:	3c00d820 	.word	0x3c00d820
    cdcc:	3c00d23c 	.word	0x3c00d23c
    cdd0:	3c00d810 	.word	0x3c00d810
    cdd4:	3c00d230 	.word	0x3c00d230
    cdd8:	3c00d68c 	.word	0x3c00d68c
    cddc:	000104fe 	.word	0x000104fe
    cde0:	000117f3 	.word	0x000117f3
    cde4:	000104ff 	.word	0x000104ff

0000cde8 <readline>:
    cde8:	e59f1000 	ldr	r1, [pc, #0]	; cdf0 <readline+0x8>
    cdec:	eafffe50 	b	c734 <readline_into_buffer>
    cdf0:	3c00d710 	.word	0x3c00d710

0000cdf4 <main_loop>:
    cdf4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    cdf8:	e24dd004 	sub	sp, sp, #4	; 0x4
    cdfc:	e59f0030 	ldr	r0, [pc, #48]	; ce34 <main_loop+0x40>
    ce00:	ebfffff8 	bl	cde8 <readline>
    ce04:	e1a03000 	mov	r3, r0
    ce08:	e3730001 	cmn	r3, #1	; 0x1
    ce0c:	e3a01000 	mov	r1, #0	; 0x0
    ce10:	e59f0020 	ldr	r0, [pc, #32]	; ce38 <main_loop+0x44>
    ce14:	1a000002 	bne	ce24 <main_loop+0x30>
    ce18:	e59f001c 	ldr	r0, [pc, #28]	; ce3c <main_loop+0x48>
    ce1c:	ebfffcdf 	bl	c1a0 <puts>
    ce20:	eafffff5 	b	cdfc <main_loop+0x8>
    ce24:	e3530000 	cmp	r3, #0	; 0x0
    ce28:	dafffff3 	ble	cdfc <main_loop+0x8>
    ce2c:	ebfffd41 	bl	c338 <run_command>
    ce30:	eafffff1 	b	cdfc <main_loop+0x8>
    ce34:	00011813 	.word	0x00011813
    ce38:	3c00d710 	.word	0x3c00d710
    ce3c:	000104fe 	.word	0x000104fe

0000ce40 <parse_num>:
    ce40:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ce44:	e1a08001 	mov	r8, r1
    ce48:	e1a07002 	mov	r7, r2
    ce4c:	e1a06003 	mov	r6, r3
    ce50:	ea000000 	b	ce58 <parse_num+0x18>
    ce54:	e2800001 	add	r0, r0, #1	; 0x1
    ce58:	e5d03000 	ldrb	r3, [r0]
    ce5c:	e3530020 	cmp	r3, #32	; 0x20
    ce60:	0afffffb 	beq	ce54 <parse_num+0x14>
    ce64:	ea000037 	b	cf48 <parse_num+0x108>
    ce68:	e3520000 	cmp	r2, #0	; 0x0
    ce6c:	0a00000a 	beq	ce9c <parse_num+0x5c>
    ce70:	e3530030 	cmp	r3, #48	; 0x30
    ce74:	1a000008 	bne	ce9c <parse_num+0x5c>
    ce78:	e5d02001 	ldrb	r2, [r0, #1]
    ce7c:	e2423041 	sub	r3, r2, #65	; 0x41
    ce80:	e6ef3073 	uxtb	r3, r3
    ce84:	e3530019 	cmp	r3, #25	; 0x19
    ce88:	92823020 	addls	r3, r2, #32	; 0x20
    ce8c:	96ef2073 	uxtbls	r2, r3
    ce90:	e3520078 	cmp	r2, #120	; 0x78
    ce94:	02800002 	addeq	r0, r0, #2	; 0x2
    ce98:	03a04010 	moveq	r4, #16	; 0x10
    ce9c:	e4d0c001 	ldrb	ip, [r0], #1
    cea0:	e24c3030 	sub	r3, ip, #48	; 0x30
    cea4:	e24c2041 	sub	r2, ip, #65	; 0x41
    cea8:	e6ef3073 	uxtb	r3, r3
    ceac:	e6ef2072 	uxtb	r2, r2
    ceb0:	e3530009 	cmp	r3, #9	; 0x9
    ceb4:	83a01000 	movhi	r1, #0	; 0x0
    ceb8:	93a01001 	movls	r1, #1	; 0x1
    cebc:	e3520005 	cmp	r2, #5	; 0x5
    cec0:	83a02000 	movhi	r2, #0	; 0x0
    cec4:	93a02001 	movls	r2, #1	; 0x1
    cec8:	e1913002 	orrs	r3, r1, r2
    cecc:	1a000003 	bne	cee0 <parse_num+0xa0>
    ced0:	e24c3061 	sub	r3, ip, #97	; 0x61
    ced4:	e6ef3073 	uxtb	r3, r3
    ced8:	e3530005 	cmp	r3, #5	; 0x5
    cedc:	8a00000f 	bhi	cf20 <parse_num+0xe0>
    cee0:	e3510000 	cmp	r1, #0	; 0x0
    cee4:	124c2030 	subne	r2, ip, #48	; 0x30
    cee8:	1a000007 	bne	cf0c <parse_num+0xcc>
    ceec:	e24c3061 	sub	r3, ip, #97	; 0x61
    cef0:	e6ef3073 	uxtb	r3, r3
    cef4:	e3530005 	cmp	r3, #5	; 0x5
    cef8:	924c2057 	subls	r2, ip, #87	; 0x57
    cefc:	9a000002 	bls	cf0c <parse_num+0xcc>
    cf00:	e3520000 	cmp	r2, #0	; 0x0
    cf04:	0a000002 	beq	cf14 <parse_num+0xd4>
    cf08:	e24c2037 	sub	r2, ip, #55	; 0x37
    cf0c:	e1520004 	cmp	r2, r4
    cf10:	aa000002 	bge	cf20 <parse_num+0xe0>
    cf14:	e0252495 	mla	r5, r5, r4, r2
    cf18:	e3a02000 	mov	r2, #0	; 0x0
    cf1c:	ea00000c 	b	cf54 <parse_num+0x114>
    cf20:	e3560000 	cmp	r6, #0	; 0x0
    cf24:	0a000012 	beq	cf74 <parse_num+0x134>
    cf28:	e3a03000 	mov	r3, #0	; 0x0
    cf2c:	e7d62003 	ldrb	r2, [r6, r3]
    cf30:	e2833001 	add	r3, r3, #1	; 0x1
    cf34:	e3520000 	cmp	r2, #0	; 0x0
    cf38:	0a00000d 	beq	cf74 <parse_num+0x134>
    cf3c:	e15c0002 	cmp	ip, r2
    cf40:	1afffff9 	bne	cf2c <parse_num+0xec>
    cf44:	ea000005 	b	cf60 <parse_num+0x120>
    cf48:	e3a02001 	mov	r2, #1	; 0x1
    cf4c:	e3a0400a 	mov	r4, #10	; 0xa
    cf50:	e3a05000 	mov	r5, #0	; 0x0
    cf54:	e5d03000 	ldrb	r3, [r0]
    cf58:	e3530000 	cmp	r3, #0	; 0x0
    cf5c:	1affffc1 	bne	ce68 <parse_num+0x28>
    cf60:	e3570000 	cmp	r7, #0	; 0x0
    cf64:	15870000 	strne	r0, [r7]
    cf68:	e5885000 	str	r5, [r8]
    cf6c:	e3a00001 	mov	r0, #1	; 0x1
    cf70:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    cf74:	e3a00000 	mov	r0, #0	; 0x0
    cf78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000cf7c <xyzModem_stream_close>:
    cf7c:	e59f2044 	ldr	r2, [pc, #68]	; cfc8 <xyzModem_stream_close+0x4c>
    cf80:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    cf84:	e5920428 	ldr	r0, [r2, #1064]
    cf88:	e59f303c 	ldr	r3, [pc, #60]	; cfcc <xyzModem_stream_close+0x50>
    cf8c:	e592c424 	ldr	ip, [r2, #1060]
    cf90:	e592e418 	ldr	lr, [r2, #1048]
    cf94:	e3500000 	cmp	r0, #0	; 0x0
    cf98:	e24dd00c 	sub	sp, sp, #12	; 0xc
    cf9c:	e59f102c 	ldr	r1, [pc, #44]	; cfd0 <xyzModem_stream_close+0x54>
    cfa0:	01a01003 	moveq	r1, r3
    cfa4:	e59f3028 	ldr	r3, [pc, #40]	; cfd4 <xyzModem_stream_close+0x58>
    cfa8:	e88d5000 	stm	sp, {ip, lr}
    cfac:	e593c000 	ldr	ip, [r3]
    cfb0:	e59f0020 	ldr	r0, [pc, #32]	; cfd8 <xyzModem_stream_close+0x5c>
    cfb4:	e5923420 	ldr	r3, [r2, #1056]
    cfb8:	e592241c 	ldr	r2, [r2, #1052]
    cfbc:	e12fff3c 	blx	ip
    cfc0:	e28dd00c 	add	sp, sp, #12	; 0xc
    cfc4:	e8bd8000 	pop	{pc}
    cfc8:	3c00d250 	.word	0x3c00d250
    cfcc:	00011817 	.word	0x00011817
    cfd0:	0001181d 	.word	0x0001181d
    cfd4:	3c00d68c 	.word	0x3c00d68c
    cfd8:	00011821 	.word	0x00011821

0000cfdc <xyzModem_error>:
    cfdc:	e2800008 	add	r0, r0, #8	; 0x8
    cfe0:	e3500007 	cmp	r0, #7	; 0x7
    cfe4:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    cfe8:	ea000017 	b	d04c <xyzModem_error+0x70>
    cfec:	0000d044 	.word	0x0000d044
    cff0:	0000d03c 	.word	0x0000d03c
    cff4:	0000d034 	.word	0x0000d034
    cff8:	0000d02c 	.word	0x0000d02c
    cffc:	0000d024 	.word	0x0000d024
    d000:	0000d01c 	.word	0x0000d01c
    d004:	0000d00c 	.word	0x0000d00c
    d008:	0000d014 	.word	0x0000d014
    d00c:	e59f0040 	ldr	r0, [pc, #64]	; d054 <xyzModem_error+0x78>
    d010:	e12fff1e 	bx	lr
    d014:	e59f003c 	ldr	r0, [pc, #60]	; d058 <xyzModem_error+0x7c>
    d018:	e12fff1e 	bx	lr
    d01c:	e59f0038 	ldr	r0, [pc, #56]	; d05c <xyzModem_error+0x80>
    d020:	e12fff1e 	bx	lr
    d024:	e59f0034 	ldr	r0, [pc, #52]	; d060 <xyzModem_error+0x84>
    d028:	e12fff1e 	bx	lr
    d02c:	e59f0030 	ldr	r0, [pc, #48]	; d064 <xyzModem_error+0x88>
    d030:	e12fff1e 	bx	lr
    d034:	e59f002c 	ldr	r0, [pc, #44]	; d068 <xyzModem_error+0x8c>
    d038:	e12fff1e 	bx	lr
    d03c:	e59f0028 	ldr	r0, [pc, #40]	; d06c <xyzModem_error+0x90>
    d040:	e12fff1e 	bx	lr
    d044:	e59f0024 	ldr	r0, [pc, #36]	; d070 <xyzModem_error+0x94>
    d048:	e12fff1e 	bx	lr
    d04c:	e59f0020 	ldr	r0, [pc, #32]	; d074 <xyzModem_error+0x98>
    d050:	e12fff1e 	bx	lr
    d054:	00011862 	.word	0x00011862
    d058:	00011882 	.word	0x00011882
    d05c:	00011894 	.word	0x00011894
    d060:	0001189e 	.word	0x0001189e
    d064:	000118aa 	.word	0x000118aa
    d068:	000118b4 	.word	0x000118b4
    d06c:	000118c4 	.word	0x000118c4
    d070:	000118d7 	.word	0x000118d7
    d074:	000118ec 	.word	0x000118ec

0000d078 <delay>:
    d078:	e12fff1e 	bx	lr

0000d07c <init_num>:
    d07c:	e59f305c 	ldr	r3, [pc, #92]	; d0e0 <init_num+0x64>
    d080:	e3a01001 	mov	r1, #1	; 0x1
    d084:	e5c31000 	strb	r1, [r3]
    d088:	e59f3054 	ldr	r3, [pc, #84]	; d0e4 <init_num+0x68>
    d08c:	e3a02000 	mov	r2, #0	; 0x0
    d090:	e5c32000 	strb	r2, [r3]
    d094:	e59f304c 	ldr	r3, [pc, #76]	; d0e8 <init_num+0x6c>
    d098:	e5c32000 	strb	r2, [r3]
    d09c:	e59f3048 	ldr	r3, [pc, #72]	; d0ec <init_num+0x70>
    d0a0:	e5c32000 	strb	r2, [r3]
    d0a4:	e59f3044 	ldr	r3, [pc, #68]	; d0f0 <init_num+0x74>
    d0a8:	e5c32000 	strb	r2, [r3]
    d0ac:	e59f3040 	ldr	r3, [pc, #64]	; d0f4 <init_num+0x78>
    d0b0:	e5832000 	str	r2, [r3]
    d0b4:	e59f303c 	ldr	r3, [pc, #60]	; d0f8 <init_num+0x7c>
    d0b8:	e5c32000 	strb	r2, [r3]
    d0bc:	e59f3038 	ldr	r3, [pc, #56]	; d0fc <init_num+0x80>
    d0c0:	e5c32000 	strb	r2, [r3]
    d0c4:	e59f3034 	ldr	r3, [pc, #52]	; d100 <init_num+0x84>
    d0c8:	e5c32000 	strb	r2, [r3]
    d0cc:	e59f3030 	ldr	r3, [pc, #48]	; d104 <init_num+0x88>
    d0d0:	e5c32000 	strb	r2, [r3]
    d0d4:	e59f302c 	ldr	r3, [pc, #44]	; d108 <init_num+0x8c>
    d0d8:	e5c32000 	strb	r2, [r3]
    d0dc:	e12fff1e 	bx	lr
    d0e0:	3c00d12c 	.word	0x3c00d12c
    d0e4:	3c00d24d 	.word	0x3c00d24d
    d0e8:	3c00d240 	.word	0x3c00d240
    d0ec:	3c00d241 	.word	0x3c00d241
    d0f0:	3c00d242 	.word	0x3c00d242
    d0f4:	3c00d244 	.word	0x3c00d244
    d0f8:	3c00d248 	.word	0x3c00d248
    d0fc:	3c00d249 	.word	0x3c00d249
    d100:	3c00d24a 	.word	0x3c00d24a
    d104:	3c00d24b 	.word	0x3c00d24b
    d108:	3c00d24c 	.word	0x3c00d24c

0000d10c <out_message>:
    d10c:	e92d4010 	push	{r4, lr}
    d110:	e59f30c8 	ldr	r3, [pc, #200]	; d1e0 <out_message+0xd4>
    d114:	e59f40c8 	ldr	r4, [pc, #200]	; d1e4 <out_message+0xd8>
    d118:	e5d31000 	ldrb	r1, [r3]
    d11c:	e59f00c4 	ldr	r0, [pc, #196]	; d1e8 <out_message+0xdc>
    d120:	e1a0e00f 	mov	lr, pc
    d124:	e594f000 	ldr	pc, [r4]
    d128:	e59f30bc 	ldr	r3, [pc, #188]	; d1ec <out_message+0xe0>
    d12c:	e59f00bc 	ldr	r0, [pc, #188]	; d1f0 <out_message+0xe4>
    d130:	e5d31000 	ldrb	r1, [r3]
    d134:	e1a0e00f 	mov	lr, pc
    d138:	e594f000 	ldr	pc, [r4]
    d13c:	e59f30b0 	ldr	r3, [pc, #176]	; d1f4 <out_message+0xe8>
    d140:	e59f00b0 	ldr	r0, [pc, #176]	; d1f8 <out_message+0xec>
    d144:	e5931000 	ldr	r1, [r3]
    d148:	e1a0e00f 	mov	lr, pc
    d14c:	e594f000 	ldr	pc, [r4]
    d150:	e59f30a4 	ldr	r3, [pc, #164]	; d1fc <out_message+0xf0>
    d154:	e59f00a4 	ldr	r0, [pc, #164]	; d200 <out_message+0xf4>
    d158:	e5d31000 	ldrb	r1, [r3]
    d15c:	e1a0e00f 	mov	lr, pc
    d160:	e594f000 	ldr	pc, [r4]
    d164:	e59f3098 	ldr	r3, [pc, #152]	; d204 <out_message+0xf8>
    d168:	e59f0098 	ldr	r0, [pc, #152]	; d208 <out_message+0xfc>
    d16c:	e5d31000 	ldrb	r1, [r3]
    d170:	e1a0e00f 	mov	lr, pc
    d174:	e594f000 	ldr	pc, [r4]
    d178:	e59f308c 	ldr	r3, [pc, #140]	; d20c <out_message+0x100>
    d17c:	e59f008c 	ldr	r0, [pc, #140]	; d210 <out_message+0x104>
    d180:	e5d31000 	ldrb	r1, [r3]
    d184:	e1a0e00f 	mov	lr, pc
    d188:	e594f000 	ldr	pc, [r4]
    d18c:	e59f3080 	ldr	r3, [pc, #128]	; d214 <out_message+0x108>
    d190:	e59f0080 	ldr	r0, [pc, #128]	; d218 <out_message+0x10c>
    d194:	e5d31000 	ldrb	r1, [r3]
    d198:	e1a0e00f 	mov	lr, pc
    d19c:	e594f000 	ldr	pc, [r4]
    d1a0:	e59f3074 	ldr	r3, [pc, #116]	; d21c <out_message+0x110>
    d1a4:	e59f0074 	ldr	r0, [pc, #116]	; d220 <out_message+0x114>
    d1a8:	e5d31000 	ldrb	r1, [r3]
    d1ac:	e1a0e00f 	mov	lr, pc
    d1b0:	e594f000 	ldr	pc, [r4]
    d1b4:	e59f3068 	ldr	r3, [pc, #104]	; d224 <out_message+0x118>
    d1b8:	e59f0068 	ldr	r0, [pc, #104]	; d228 <out_message+0x11c>
    d1bc:	e5d31000 	ldrb	r1, [r3]
    d1c0:	e1a0e00f 	mov	lr, pc
    d1c4:	e594f000 	ldr	pc, [r4]
    d1c8:	e59f305c 	ldr	r3, [pc, #92]	; d22c <out_message+0x120>
    d1cc:	e59f005c 	ldr	r0, [pc, #92]	; d230 <out_message+0x124>
    d1d0:	e5d31000 	ldrb	r1, [r3]
    d1d4:	e1a0e00f 	mov	lr, pc
    d1d8:	e594f000 	ldr	pc, [r4]
    d1dc:	e8bd8010 	pop	{r4, pc}
    d1e0:	3c00d240 	.word	0x3c00d240
    d1e4:	3c00d68c 	.word	0x3c00d68c
    d1e8:	000118fa 	.word	0x000118fa
    d1ec:	3c00d241 	.word	0x3c00d241
    d1f0:	00011908 	.word	0x00011908
    d1f4:	3c00d244 	.word	0x3c00d244
    d1f8:	0001191b 	.word	0x0001191b
    d1fc:	3c00d248 	.word	0x3c00d248
    d200:	0001192d 	.word	0x0001192d
    d204:	3c00d249 	.word	0x3c00d249
    d208:	00011940 	.word	0x00011940
    d20c:	3c00d24a 	.word	0x3c00d24a
    d210:	00011957 	.word	0x00011957
    d214:	3c00d12c 	.word	0x3c00d12c
    d218:	0001196f 	.word	0x0001196f
    d21c:	3c00d24b 	.word	0x3c00d24b
    d220:	0001197e 	.word	0x0001197e
    d224:	3c00d24c 	.word	0x3c00d24c
    d228:	00011997 	.word	0x00011997
    d22c:	3c00d24d 	.word	0x3c00d24d
    d230:	000119a9 	.word	0x000119a9

0000d234 <Procpacket>:
    d234:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d238:	e59fc1ac 	ldr	ip, [pc, #428]	; d3ec <Procpacket+0x1b8>
    d23c:	e59f21ac 	ldr	r2, [pc, #428]	; d3f0 <Procpacket+0x1bc>
    d240:	e5dc3000 	ldrb	r3, [ip]
    d244:	e1a06000 	mov	r6, r0
    d248:	e5c03000 	strb	r3, [r0]
    d24c:	e5923000 	ldr	r3, [r2]
    d250:	e1a05001 	mov	r5, r1
    d254:	e5c03001 	strb	r3, [r0, #1]
    d258:	e5923000 	ldr	r3, [r2]
    d25c:	e1e03003 	mvn	r3, r3
    d260:	e5c03002 	strb	r3, [r0, #2]
    d264:	e59f3188 	ldr	r3, [pc, #392]	; d3f4 <Procpacket+0x1c0>
    d268:	e5922000 	ldr	r2, [r2]
    d26c:	e5933000 	ldr	r3, [r3]
    d270:	e1520003 	cmp	r2, r3
    d274:	1a000008 	bne	d29c <Procpacket+0x68>
    d278:	e59f3178 	ldr	r3, [pc, #376]	; d3f8 <Procpacket+0x1c4>
    d27c:	e5d32000 	ldrb	r2, [r3]
    d280:	e59f3174 	ldr	r3, [pc, #372]	; d3fc <Procpacket+0x1c8>
    d284:	e3520001 	cmp	r2, #1	; 0x1
    d288:	e5934000 	ldr	r4, [r3]
    d28c:	e5cc2000 	strb	r2, [ip]
    d290:	03a07080 	moveq	r7, #128	; 0x80
    d294:	13a07b01 	movne	r7, #1024	; 0x400
    d298:	ea000004 	b	d2b0 <Procpacket+0x7c>
    d29c:	e5dc3000 	ldrb	r3, [ip]
    d2a0:	e3530001 	cmp	r3, #1	; 0x1
    d2a4:	13a04b01 	movne	r4, #1024	; 0x400
    d2a8:	03a04080 	moveq	r4, #128	; 0x80
    d2ac:	e1a07004 	mov	r7, r4
    d2b0:	e59f3148 	ldr	r3, [pc, #328]	; d400 <Procpacket+0x1cc>
    d2b4:	e5d33000 	ldrb	r3, [r3]
    d2b8:	e3530000 	cmp	r3, #0	; 0x0
    d2bc:	1a000008 	bne	d2e4 <Procpacket+0xb0>
    d2c0:	e59f313c 	ldr	r3, [pc, #316]	; d404 <Procpacket+0x1d0>
    d2c4:	e2860003 	add	r0, r6, #3	; 0x3
    d2c8:	e5931000 	ldr	r1, [r3]
    d2cc:	e59f3134 	ldr	r3, [pc, #308]	; d408 <Procpacket+0x1d4>
    d2d0:	e1a02004 	mov	r2, r4
    d2d4:	e5933000 	ldr	r3, [r3]
    d2d8:	e0631001 	rsb	r1, r3, r1
    d2dc:	e0851001 	add	r1, r5, r1
    d2e0:	ebffcfb8 	bl	11c8 <memcpy>
    d2e4:	e59f111c 	ldr	r1, [pc, #284]	; d408 <Procpacket+0x1d4>
    d2e8:	e59f311c 	ldr	r3, [pc, #284]	; d40c <Procpacket+0x1d8>
    d2ec:	e5912000 	ldr	r2, [r1]
    d2f0:	e5d33000 	ldrb	r3, [r3]
    d2f4:	e0642002 	rsb	r2, r4, r2
    d2f8:	e3530000 	cmp	r3, #0	; 0x0
    d2fc:	e5812000 	str	r2, [r1]
    d300:	0a000006 	beq	d320 <Procpacket+0xec>
    d304:	e1a02007 	mov	r2, r7
    d308:	e2860003 	add	r0, r6, #3	; 0x3
    d30c:	e3a01000 	mov	r1, #0	; 0x0
    d310:	ebffcfa4 	bl	11a8 <memset>
    d314:	e59f30f4 	ldr	r3, [pc, #244]	; d410 <Procpacket+0x1dc>
    d318:	e3a02001 	mov	r2, #1	; 0x1
    d31c:	e5c32000 	strb	r2, [r3]
    d320:	e59f30c8 	ldr	r3, [pc, #200]	; d3f0 <Procpacket+0x1bc>
    d324:	e5932000 	ldr	r2, [r3]
    d328:	e59f30c4 	ldr	r3, [pc, #196]	; d3f4 <Procpacket+0x1c0>
    d32c:	e5933000 	ldr	r3, [r3]
    d330:	e1520003 	cmp	r2, r3
    d334:	1a000004 	bne	d34c <Procpacket+0x118>
    d338:	e2840003 	add	r0, r4, #3	; 0x3
    d33c:	e0860000 	add	r0, r6, r0
    d340:	e0642007 	rsb	r2, r4, r7
    d344:	e3a0101a 	mov	r1, #26	; 0x1a
    d348:	ebffcf96 	bl	11a8 <memset>
    d34c:	e3a00000 	mov	r0, #0	; 0x0
    d350:	e59f80bc 	ldr	r8, [pc, #188]	; d414 <Procpacket+0x1e0>
    d354:	e1a04006 	mov	r4, r6
    d358:	e1a0c000 	mov	ip, r0
    d35c:	e1a05000 	mov	r5, r0
    d360:	e1a0342c 	lsr	r3, ip, #8
    d364:	e5d42003 	ldrb	r2, [r4, #3]
    d368:	e1a03083 	lsl	r3, r3, #1
    d36c:	e19810b3 	ldrh	r1, [r8, r3]
    d370:	e2800001 	add	r0, r0, #1	; 0x1
    d374:	e182340c 	orr	r3, r2, ip, lsl #8
    d378:	e0233001 	eor	r3, r3, r1
    d37c:	e0852002 	add	r2, r5, r2
    d380:	e1500007 	cmp	r0, r7
    d384:	e6ffc073 	uxth	ip, r3
    d388:	e6ef5072 	uxtb	r5, r2
    d38c:	e2844001 	add	r4, r4, #1	; 0x1
    d390:	e59f107c 	ldr	r1, [pc, #124]	; d414 <Procpacket+0x1e0>
    d394:	3afffff1 	bcc	d360 <Procpacket+0x12c>
    d398:	e59f3078 	ldr	r3, [pc, #120]	; d418 <Procpacket+0x1e4>
    d39c:	e0860007 	add	r0, r6, r7
    d3a0:	e5d33000 	ldrb	r3, [r3]
    d3a4:	e3530000 	cmp	r3, #0	; 0x0
    d3a8:	05c05003 	strbeq	r5, [r0, #3]
    d3ac:	0a00000c 	beq	d3e4 <Procpacket+0x1b0>
    d3b0:	e1a0342c 	lsr	r3, ip, #8
    d3b4:	e1a03083 	lsl	r3, r3, #1
    d3b8:	e19120b3 	ldrh	r2, [r1, r3]
    d3bc:	e022240c 	eor	r2, r2, ip, lsl #8
    d3c0:	e1a03442 	asr	r3, r2, #8
    d3c4:	e20330ff 	and	r3, r3, #255	; 0xff
    d3c8:	e1a03083 	lsl	r3, r3, #1
    d3cc:	e19130b3 	ldrh	r3, [r1, r3]
    d3d0:	e0233402 	eor	r3, r3, r2, lsl #8
    d3d4:	e6ff3073 	uxth	r3, r3
    d3d8:	e1a02423 	lsr	r2, r3, #8
    d3dc:	e5c03004 	strb	r3, [r0, #4]
    d3e0:	e5c02003 	strb	r2, [r0, #3]
    d3e4:	e3a00000 	mov	r0, #0	; 0x0
    d3e8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d3ec:	3c00d242 	.word	0x3c00d242
    d3f0:	3c00d244 	.word	0x3c00d244
    d3f4:	3c00dc20 	.word	0x3c00dc20
    d3f8:	3c00dc28 	.word	0x3c00dc28
    d3fc:	3c00dc24 	.word	0x3c00dc24
    d400:	3c00d248 	.word	0x3c00d248
    d404:	3c00d6ec 	.word	0x3c00d6ec
    d408:	3c00d6f0 	.word	0x3c00d6f0
    d40c:	3c00d24d 	.word	0x3c00d24d
    d410:	3c00d24c 	.word	0x3c00d24c
    d414:	0000f6da 	.word	0x0000f6da
    d418:	3c00d6e8 	.word	0x3c00d6e8

0000d41c <procsend>:
    d41c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d420:	e59f31f8 	ldr	r3, [pc, #504]	; d620 <procsend+0x204>
    d424:	e1a06000 	mov	r6, r0
    d428:	e5d30000 	ldrb	r0, [r3]
    d42c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d430:	e3500000 	cmp	r0, #0	; 0x0
    d434:	e1a0c002 	mov	ip, r2
    d438:	e59f41e4 	ldr	r4, [pc, #484]	; d624 <procsend+0x208>
    d43c:	0a000065 	beq	d5d8 <procsend+0x1bc>
    d440:	e59f31e0 	ldr	r3, [pc, #480]	; d628 <procsend+0x20c>
    d444:	e1a02b02 	lsl	r2, r2, #22
    d448:	e1a02b22 	lsr	r2, r2, #22
    d44c:	e583c000 	str	ip, [r3]
    d450:	e59f31d4 	ldr	r3, [pc, #468]	; d62c <procsend+0x210>
    d454:	e59f01d4 	ldr	r0, [pc, #468]	; d630 <procsend+0x214>
    d458:	e3520000 	cmp	r2, #0	; 0x0
    d45c:	e1a0152c 	lsr	r1, ip, #10
    d460:	e583c000 	str	ip, [r3]
    d464:	03a03b01 	moveq	r3, #1024	; 0x400
    d468:	e5802000 	str	r2, [r0]
    d46c:	e5841000 	str	r1, [r4]
    d470:	05803000 	streq	r3, [r0]
    d474:	0a000006 	beq	d494 <procsend+0x78>
    d478:	e2813001 	add	r3, r1, #1	; 0x1
    d47c:	e352007f 	cmp	r2, #127	; 0x7f
    d480:	e59f21ac 	ldr	r2, [pc, #428]	; d634 <procsend+0x218>
    d484:	e5843000 	str	r3, [r4]
    d488:	93a03001 	movls	r3, #1	; 0x1
    d48c:	83a03002 	movhi	r3, #2	; 0x2
    d490:	e5c23000 	strb	r3, [r2]
    d494:	e59f019c 	ldr	r0, [pc, #412]	; d638 <procsend+0x21c>
    d498:	ebffcf30 	bl	1160 <strlen>
    d49c:	e59f2198 	ldr	r2, [pc, #408]	; d63c <procsend+0x220>
    d4a0:	e3500080 	cmp	r0, #128	; 0x80
    d4a4:	83a03002 	movhi	r3, #2	; 0x2
    d4a8:	93a03001 	movls	r3, #1	; 0x1
    d4ac:	85c23000 	strbhi	r3, [r2]
    d4b0:	95c23000 	strbls	r3, [r2]
    d4b4:	e59f3184 	ldr	r3, [pc, #388]	; d640 <procsend+0x224>
    d4b8:	83a07b01 	movhi	r7, #1024	; 0x400
    d4bc:	e5d31000 	ldrb	r1, [r3]
    d4c0:	93a07080 	movls	r7, #128	; 0x80
    d4c4:	e3510000 	cmp	r1, #0	; 0x0
    d4c8:	13a01000 	movne	r1, #0	; 0x0
    d4cc:	12872005 	addne	r2, r7, #5	; 0x5
    d4d0:	02872004 	addeq	r2, r7, #4	; 0x4
    d4d4:	11a00006 	movne	r0, r6
    d4d8:	01a00006 	moveq	r0, r6
    d4dc:	ebffcf31 	bl	11a8 <memset>
    d4e0:	e59f3154 	ldr	r3, [pc, #340]	; d63c <procsend+0x220>
    d4e4:	e3a05000 	mov	r5, #0	; 0x0
    d4e8:	e5d33000 	ldrb	r3, [r3]
    d4ec:	e59f0144 	ldr	r0, [pc, #324]	; d638 <procsend+0x21c>
    d4f0:	e5c63000 	strb	r3, [r6]
    d4f4:	e3e03000 	mvn	r3, #0	; 0x0
    d4f8:	e5c63002 	strb	r3, [r6, #2]
    d4fc:	e5c65001 	strb	r5, [r6, #1]
    d500:	ebffcf16 	bl	1160 <strlen>
    d504:	e59f112c 	ldr	r1, [pc, #300]	; d638 <procsend+0x21c>
    d508:	e1a02000 	mov	r2, r0
    d50c:	e1a04000 	mov	r4, r0
    d510:	e2860003 	add	r0, r6, #3	; 0x3
    d514:	ebffcec1 	bl	1020 <strncpy>
    d518:	e59f3108 	ldr	r3, [pc, #264]	; d628 <procsend+0x20c>
    d51c:	e2844004 	add	r4, r4, #4	; 0x4
    d520:	e0860004 	add	r0, r6, r4
    d524:	e5932000 	ldr	r2, [r3]
    d528:	e59f1114 	ldr	r1, [pc, #276]	; d644 <procsend+0x228>
    d52c:	e59f4114 	ldr	r4, [pc, #276]	; d648 <procsend+0x22c>
    d530:	ebfffab8 	bl	c018 <sprintf>
    d534:	e1a00006 	mov	r0, r6
    d538:	e1a0c005 	mov	ip, r5
    d53c:	e1a0e005 	mov	lr, r5
    d540:	e1a0342c 	lsr	r3, ip, #8
    d544:	e5d02003 	ldrb	r2, [r0, #3]
    d548:	e1a03083 	lsl	r3, r3, #1
    d54c:	e19410b3 	ldrh	r1, [r4, r3]
    d550:	e2855001 	add	r5, r5, #1	; 0x1
    d554:	e182340c 	orr	r3, r2, ip, lsl #8
    d558:	e0233001 	eor	r3, r3, r1
    d55c:	e08e2002 	add	r2, lr, r2
    d560:	e1550007 	cmp	r5, r7
    d564:	e6ffc073 	uxth	ip, r3
    d568:	e6efe072 	uxtb	lr, r2
    d56c:	e2800001 	add	r0, r0, #1	; 0x1
    d570:	e59f10d0 	ldr	r1, [pc, #208]	; d648 <procsend+0x22c>
    d574:	3afffff1 	bcc	d540 <procsend+0x124>
    d578:	e59f30c0 	ldr	r3, [pc, #192]	; d640 <procsend+0x224>
    d57c:	e0860007 	add	r0, r6, r7
    d580:	e5d33000 	ldrb	r3, [r3]
    d584:	e3530000 	cmp	r3, #0	; 0x0
    d588:	05c0e003 	strbeq	lr, [r0, #3]
    d58c:	0a00000c 	beq	d5c4 <procsend+0x1a8>
    d590:	e1a0342c 	lsr	r3, ip, #8
    d594:	e1a03083 	lsl	r3, r3, #1
    d598:	e19120b3 	ldrh	r2, [r1, r3]
    d59c:	e022240c 	eor	r2, r2, ip, lsl #8
    d5a0:	e1a03442 	asr	r3, r2, #8
    d5a4:	e20330ff 	and	r3, r3, #255	; 0xff
    d5a8:	e1a03083 	lsl	r3, r3, #1
    d5ac:	e19130b3 	ldrh	r3, [r1, r3]
    d5b0:	e0233402 	eor	r3, r3, r2, lsl #8
    d5b4:	e6ff3073 	uxth	r3, r3
    d5b8:	e1a02423 	lsr	r2, r3, #8
    d5bc:	e5c03004 	strb	r3, [r0, #4]
    d5c0:	e5c02003 	strb	r2, [r0, #3]
    d5c4:	e59f3080 	ldr	r3, [pc, #128]	; d64c <procsend+0x230>
    d5c8:	e3a02001 	mov	r2, #1	; 0x1
    d5cc:	e5c32000 	strb	r2, [r3]
    d5d0:	e3a00000 	mov	r0, #0	; 0x0
    d5d4:	ea00000f 	b	d618 <procsend+0x1fc>
    d5d8:	e59f2070 	ldr	r2, [pc, #112]	; d650 <procsend+0x234>
    d5dc:	e5943000 	ldr	r3, [r4]
    d5e0:	e5922000 	ldr	r2, [r2]
    d5e4:	e1520003 	cmp	r2, r3
    d5e8:	8a00000a 	bhi	d618 <procsend+0x1fc>
    d5ec:	e59f3060 	ldr	r3, [pc, #96]	; d654 <procsend+0x238>
    d5f0:	e59f2044 	ldr	r2, [pc, #68]	; d63c <procsend+0x220>
    d5f4:	e5d33000 	ldrb	r3, [r3]
    d5f8:	e1a00006 	mov	r0, r6
    d5fc:	e3530000 	cmp	r3, #0	; 0x0
    d600:	13a03001 	movne	r3, #1	; 0x1
    d604:	03a03002 	moveq	r3, #2	; 0x2
    d608:	e5c23000 	strb	r3, [r2]
    d60c:	e1a0200c 	mov	r2, ip
    d610:	ebffff07 	bl	d234 <Procpacket>
    d614:	e6ef0070 	uxtb	r0, r0
    d618:	e28dd004 	add	sp, sp, #4	; 0x4
    d61c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d620:	3c00d12c 	.word	0x3c00d12c
    d624:	3c00dc20 	.word	0x3c00dc20
    d628:	3c00d6f0 	.word	0x3c00d6f0
    d62c:	3c00d6ec 	.word	0x3c00d6ec
    d630:	3c00dc24 	.word	0x3c00dc24
    d634:	3c00dc28 	.word	0x3c00dc28
    d638:	000119b8 	.word	0x000119b8
    d63c:	3c00d242 	.word	0x3c00d242
    d640:	3c00d6e8 	.word	0x3c00d6e8
    d644:	000112b5 	.word	0x000112b5
    d648:	0000f6da 	.word	0x0000f6da
    d64c:	3c00d241 	.word	0x3c00d241
    d650:	3c00d244 	.word	0x3c00d244
    d654:	3c00d24d 	.word	0x3c00d24d

0000d658 <CYGACC_COMM_IF_PUTC>:
    d658:	e1a00001 	mov	r0, r1
    d65c:	eafffaf7 	b	c240 <putc>

0000d660 <real_send>:
    d660:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d664:	e59f30ec 	ldr	r3, [pc, #236]	; d758 <real_send+0xf8>
    d668:	e24ddb01 	sub	sp, sp, #1024	; 0x400
    d66c:	e5d33000 	ldrb	r3, [r3]
    d670:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d674:	e3530000 	cmp	r3, #0	; 0x0
    d678:	e1a0e000 	mov	lr, r0
    d67c:	e1a0c001 	mov	ip, r1
    d680:	159f30d4 	ldrne	r3, [pc, #212]	; d75c <real_send+0xfc>
    d684:	13a02000 	movne	r2, #0	; 0x0
    d688:	15832000 	strne	r2, [r3]
    d68c:	1a000006 	bne	d6ac <real_send+0x4c>
    d690:	e59f30c8 	ldr	r3, [pc, #200]	; d760 <real_send+0x100>
    d694:	e5d33000 	ldrb	r3, [r3]
    d698:	e3530000 	cmp	r3, #0	; 0x0
    d69c:	059f20b8 	ldreq	r2, [pc, #184]	; d75c <real_send+0xfc>
    d6a0:	05923000 	ldreq	r3, [r2]
    d6a4:	02833001 	addeq	r3, r3, #1	; 0x1
    d6a8:	05823000 	streq	r3, [r2]
    d6ac:	e28d0008 	add	r0, sp, #8	; 0x8
    d6b0:	e2400005 	sub	r0, r0, #5	; 0x5
    d6b4:	e1a0100e 	mov	r1, lr
    d6b8:	e1a0200c 	mov	r2, ip
    d6bc:	ebffff56 	bl	d41c <procsend>
    d6c0:	e2507000 	subs	r7, r0, #0	; 0x0
    d6c4:	1a00001f 	bne	d748 <real_send+0xe8>
    d6c8:	e59f3094 	ldr	r3, [pc, #148]	; d764 <real_send+0x104>
    d6cc:	e28d5008 	add	r5, sp, #8	; 0x8
    d6d0:	e5d31000 	ldrb	r1, [r3]
    d6d4:	e59f308c 	ldr	r3, [pc, #140]	; d768 <real_send+0x108>
    d6d8:	e3510001 	cmp	r1, #1	; 0x1
    d6dc:	e5d32000 	ldrb	r2, [r3]
    d6e0:	e3003404 	movw	r3, #1028	; 0x404
    d6e4:	11a06003 	movne	r6, r3
    d6e8:	03a06084 	moveq	r6, #132	; 0x84
    d6ec:	e3520000 	cmp	r2, #0	; 0x0
    d6f0:	12866001 	addne	r6, r6, #1	; 0x1
    d6f4:	e2455005 	sub	r5, r5, #5	; 0x5
    d6f8:	e3a04000 	mov	r4, #0	; 0x0
    d6fc:	e7d50004 	ldrb	r0, [r5, r4]
    d700:	e2844001 	add	r4, r4, #1	; 0x1
    d704:	ebfffacd 	bl	c240 <putc>
    d708:	e1540006 	cmp	r4, r6
    d70c:	3afffffa 	bcc	d6fc <real_send+0x9c>
    d710:	e59f3054 	ldr	r3, [pc, #84]	; d76c <real_send+0x10c>
    d714:	e5d33000 	ldrb	r3, [r3]
    d718:	e3530000 	cmp	r3, #0	; 0x0
    d71c:	13e07000 	mvnne	r7, #0	; 0x0
    d720:	1a000008 	bne	d748 <real_send+0xe8>
    d724:	e59f3030 	ldr	r3, [pc, #48]	; d75c <real_send+0xfc>
    d728:	e5932000 	ldr	r2, [r3]
    d72c:	e59f303c 	ldr	r3, [pc, #60]	; d770 <real_send+0x110>
    d730:	e5933000 	ldr	r3, [r3]
    d734:	e1520003 	cmp	r2, r3
    d738:	059f3034 	ldreq	r3, [pc, #52]	; d774 <real_send+0x114>
    d73c:	03a02001 	moveq	r2, #1	; 0x1
    d740:	05c32000 	strbeq	r2, [r3]
    d744:	03a07003 	moveq	r7, #3	; 0x3
    d748:	e1a00007 	mov	r0, r7
    d74c:	e28dd00c 	add	sp, sp, #12	; 0xc
    d750:	e28ddb01 	add	sp, sp, #1024	; 0x400
    d754:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d758:	3c00d24d 	.word	0x3c00d24d
    d75c:	3c00d244 	.word	0x3c00d244
    d760:	3c00d12c 	.word	0x3c00d12c
    d764:	3c00d242 	.word	0x3c00d242
    d768:	3c00d6e8 	.word	0x3c00d6e8
    d76c:	3c00d24c 	.word	0x3c00d24c
    d770:	3c00dc20 	.word	0x3c00dc20
    d774:	3c00d248 	.word	0x3c00d248

0000d778 <CYGACC_COMM_IF_GETC_TIMEOUT>:
    d778:	e92d4070 	push	{r4, r5, r6, lr}
    d77c:	e1a05001 	mov	r5, r1
    d780:	e3a04000 	mov	r4, #0	; 0x0
    d784:	e59f603c 	ldr	r6, [pc, #60]	; d7c8 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
    d788:	ea000000 	b	d790 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
    d78c:	ebffebd9 	bl	86f8 <udelay>
    d790:	ebfffaab 	bl	c244 <tstc>
    d794:	e3500000 	cmp	r0, #0	; 0x0
    d798:	e3a00014 	mov	r0, #20	; 0x14
    d79c:	1a000002 	bne	d7ac <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
    d7a0:	e1540006 	cmp	r4, r6
    d7a4:	e2844001 	add	r4, r4, #1	; 0x1
    d7a8:	1afffff7 	bne	d78c <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
    d7ac:	ebfffaa4 	bl	c244 <tstc>
    d7b0:	e3500000 	cmp	r0, #0	; 0x0
    d7b4:	08bd8070 	popeq	{r4, r5, r6, pc}
    d7b8:	ebfffaa2 	bl	c248 <getc>
    d7bc:	e5c50000 	strb	r0, [r5]
    d7c0:	e3a00001 	mov	r0, #1	; 0x1
    d7c4:	e8bd8070 	pop	{r4, r5, r6, pc}
    d7c8:	000186a0 	.word	0x000186a0

0000d7cc <control_send>:
    d7cc:	e59f323c 	ldr	r3, [pc, #572]	; da10 <control_send+0x244>
    d7d0:	e3a02000 	mov	r2, #0	; 0x0
    d7d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d7d8:	e5c32000 	strb	r2, [r3]
    d7dc:	e59f3230 	ldr	r3, [pc, #560]	; da14 <control_send+0x248>
    d7e0:	e59f4230 	ldr	r4, [pc, #560]	; da18 <control_send+0x24c>
    d7e4:	e5c32000 	strb	r2, [r3]
    d7e8:	e59f322c 	ldr	r3, [pc, #556]	; da1c <control_send+0x250>
    d7ec:	e59f522c 	ldr	r5, [pc, #556]	; da20 <control_send+0x254>
    d7f0:	e5832000 	str	r2, [r3]
    d7f4:	e3a03001 	mov	r3, #1	; 0x1
    d7f8:	e59fe224 	ldr	lr, [pc, #548]	; da24 <control_send+0x258>
    d7fc:	e59f8224 	ldr	r8, [pc, #548]	; da28 <control_send+0x25c>
    d800:	e59f6224 	ldr	r6, [pc, #548]	; da2c <control_send+0x260>
    d804:	e59f7224 	ldr	r7, [pc, #548]	; da30 <control_send+0x264>
    d808:	e59fc224 	ldr	ip, [pc, #548]	; da34 <control_send+0x268>
    d80c:	e5c43000 	strb	r3, [r4]
    d810:	e59f3220 	ldr	r3, [pc, #544]	; da38 <control_send+0x26c>
    d814:	e24dd014 	sub	sp, sp, #20	; 0x14
    d818:	e5c52000 	strb	r2, [r5]
    d81c:	e5c32000 	strb	r2, [r3]
    d820:	e58d0004 	str	r0, [sp, #4]
    d824:	e1a0b001 	mov	fp, r1
    d828:	e5c82000 	strb	r2, [r8]
    d82c:	e5c62000 	strb	r2, [r6]
    d830:	e5c72000 	strb	r2, [r7]
    d834:	e5ce2000 	strb	r2, [lr]
    d838:	e5cc2000 	strb	r2, [ip]
    d83c:	e1a0a002 	mov	sl, r2
    d840:	e1a0900e 	mov	r9, lr
    d844:	e59f51f0 	ldr	r5, [pc, #496]	; da3c <control_send+0x270>
    d848:	ea000000 	b	d850 <control_send+0x84>
    d84c:	e3a0a000 	mov	sl, #0	; 0x0
    d850:	e3a00000 	mov	r0, #0	; 0x0
    d854:	e28d100f 	add	r1, sp, #15	; 0xf
    d858:	ebffffc6 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    d85c:	e3500000 	cmp	r0, #0	; 0x0
    d860:	0afffffa 	beq	d850 <control_send+0x84>
    d864:	e5dd300f 	ldrb	r3, [sp, #15]
    d868:	e3530018 	cmp	r3, #24	; 0x18
    d86c:	0a000042 	beq	d97c <control_send+0x1b0>
    d870:	8a000004 	bhi	d888 <control_send+0xbc>
    d874:	e3530006 	cmp	r3, #6	; 0x6
    d878:	0a00002c 	beq	d930 <control_send+0x164>
    d87c:	e3530015 	cmp	r3, #21	; 0x15
    d880:	1afffff1 	bne	d84c <control_send+0x80>
    d884:	ea000010 	b	d8cc <control_send+0x100>
    d888:	e3530043 	cmp	r3, #67	; 0x43
    d88c:	0a000001 	beq	d898 <control_send+0xcc>
    d890:	e3530063 	cmp	r3, #99	; 0x63
    d894:	1affffec 	bne	d84c <control_send+0x80>
    d898:	e5d73000 	ldrb	r3, [r7]
    d89c:	e5d42000 	ldrb	r2, [r4]
    d8a0:	e5d61000 	ldrb	r1, [r6]
    d8a4:	e1833002 	orr	r3, r3, r2
    d8a8:	e1913003 	orrs	r3, r1, r3
    d8ac:	0affffe6 	beq	d84c <control_send+0x80>
    d8b0:	e3510000 	cmp	r1, #0	; 0x0
    d8b4:	159f2178 	ldrne	r2, [pc, #376]	; da34 <control_send+0x268>
    d8b8:	13a03001 	movne	r3, #1	; 0x1
    d8bc:	15c23000 	strbne	r3, [r2]
    d8c0:	e3a03001 	mov	r3, #1	; 0x1
    d8c4:	e5c53000 	strb	r3, [r5]
    d8c8:	ea00002f 	b	d98c <control_send+0x1c0>
    d8cc:	e59f313c 	ldr	r3, [pc, #316]	; da10 <control_send+0x244>
    d8d0:	e5d33000 	ldrb	r3, [r3]
    d8d4:	e3530000 	cmp	r3, #0	; 0x0
    d8d8:	0a000008 	beq	d900 <control_send+0x134>
    d8dc:	e59f2138 	ldr	r2, [pc, #312]	; da1c <control_send+0x250>
    d8e0:	e5923000 	ldr	r3, [r2]
    d8e4:	e2433001 	sub	r3, r3, #1	; 0x1
    d8e8:	e5823000 	str	r3, [r2]
    d8ec:	e3a03001 	mov	r3, #1	; 0x1
    d8f0:	e5c43000 	strb	r3, [r4]
    d8f4:	e3a03000 	mov	r3, #0	; 0x0
    d8f8:	e5c93000 	strb	r3, [r9]
    d8fc:	ea000022 	b	d98c <control_send+0x1c0>
    d900:	e5d42000 	ldrb	r2, [r4]
    d904:	e3520000 	cmp	r2, #0	; 0x0
    d908:	15c53000 	strbne	r3, [r5]
    d90c:	1a000007 	bne	d930 <control_send+0x164>
    d910:	e5d93000 	ldrb	r3, [r9]
    d914:	e3530000 	cmp	r3, #0	; 0x0
    d918:	15c92000 	strbne	r2, [r9]
    d91c:	e59f20f8 	ldr	r2, [pc, #248]	; da1c <control_send+0x250>
    d920:	e5923000 	ldr	r3, [r2]
    d924:	e2433001 	sub	r3, r3, #1	; 0x1
    d928:	e5823000 	str	r3, [r2]
    d92c:	ea000016 	b	d98c <control_send+0x1c0>
    d930:	e5d42000 	ldrb	r2, [r4]
    d934:	e3520000 	cmp	r2, #0	; 0x0
    d938:	1affffc3 	bne	d84c <control_send+0x80>
    d93c:	e5d91000 	ldrb	r1, [r9]
    d940:	e3510000 	cmp	r1, #0	; 0x0
    d944:	0a000005 	beq	d960 <control_send+0x194>
    d948:	e59f30c0 	ldr	r3, [pc, #192]	; da10 <control_send+0x244>
    d94c:	e1a0a002 	mov	sl, r2
    d950:	e5c32000 	strb	r2, [r3]
    d954:	e3a03001 	mov	r3, #1	; 0x1
    d958:	e5c73000 	strb	r3, [r7]
    d95c:	eaffffbb 	b	d850 <control_send+0x84>
    d960:	e5d83000 	ldrb	r3, [r8]
    d964:	e3530000 	cmp	r3, #0	; 0x0
    d968:	13a03001 	movne	r3, #1	; 0x1
    d96c:	15c63000 	strbne	r3, [r6]
    d970:	11a0a001 	movne	sl, r1
    d974:	1affffb5 	bne	d850 <control_send+0x84>
    d978:	ea000003 	b	d98c <control_send+0x1c0>
    d97c:	e28aa001 	add	sl, sl, #1	; 0x1
    d980:	e35a0002 	cmp	sl, #2	; 0x2
    d984:	1affffb1 	bne	d850 <control_send+0x84>
    d988:	ea00001d 	b	da04 <control_send+0x238>
    d98c:	e59f20a0 	ldr	r2, [pc, #160]	; da34 <control_send+0x268>
    d990:	e5d23000 	ldrb	r3, [r2]
    d994:	e3530000 	cmp	r3, #0	; 0x0
    d998:	1a000008 	bne	d9c0 <control_send+0x1f4>
    d99c:	e59f207c 	ldr	r2, [pc, #124]	; da20 <control_send+0x254>
    d9a0:	e5d23000 	ldrb	r3, [r2]
    d9a4:	e3530000 	cmp	r3, #0	; 0x0
    d9a8:	0a000004 	beq	d9c0 <control_send+0x1f4>
    d9ac:	e3a00004 	mov	r0, #4	; 0x4
    d9b0:	ebfffa22 	bl	c240 <putc>
    d9b4:	e3a03001 	mov	r3, #1	; 0x1
    d9b8:	e5c83000 	strb	r3, [r8]
    d9bc:	eaffffa2 	b	d84c <control_send+0x80>
    d9c0:	e59d0004 	ldr	r0, [sp, #4]
    d9c4:	e1a0100b 	mov	r1, fp
    d9c8:	ebffff24 	bl	d660 <real_send>
    d9cc:	e3500003 	cmp	r0, #3	; 0x3
    d9d0:	0affff9d 	beq	d84c <control_send+0x80>
    d9d4:	e3500000 	cmp	r0, #0	; 0x0
    d9d8:	1a00000a 	bne	da08 <control_send+0x23c>
    d9dc:	e5d93000 	ldrb	r3, [r9]
    d9e0:	e3530000 	cmp	r3, #0	; 0x0
    d9e4:	e5d43000 	ldrb	r3, [r4]
    d9e8:	15c90000 	strbne	r0, [r9]
    d9ec:	e3530000 	cmp	r3, #0	; 0x0
    d9f0:	13a03000 	movne	r3, #0	; 0x0
    d9f4:	15c43000 	strbne	r3, [r4]
    d9f8:	12833001 	addne	r3, r3, #1	; 0x1
    d9fc:	15c93000 	strbne	r3, [r9]
    da00:	eaffff91 	b	d84c <control_send+0x80>
    da04:	e3a00000 	mov	r0, #0	; 0x0
    da08:	e28dd014 	add	sp, sp, #20	; 0x14
    da0c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    da10:	3c00d241 	.word	0x3c00d241
    da14:	3c00d242 	.word	0x3c00d242
    da18:	3c00d12c 	.word	0x3c00d12c
    da1c:	3c00d244 	.word	0x3c00d244
    da20:	3c00d248 	.word	0x3c00d248
    da24:	3c00d24b 	.word	0x3c00d24b
    da28:	3c00d240 	.word	0x3c00d240
    da2c:	3c00d249 	.word	0x3c00d249
    da30:	3c00d24a 	.word	0x3c00d24a
    da34:	3c00d24d 	.word	0x3c00d24d
    da38:	3c00d24c 	.word	0x3c00d24c
    da3c:	3c00d6e8 	.word	0x3c00d6e8

0000da40 <xyzModem_stream_terminate>:
    da40:	e92d4030 	push	{r4, r5, lr}
    da44:	e3500000 	cmp	r0, #0	; 0x0
    da48:	e24dd00c 	sub	sp, sp, #12	; 0xc
    da4c:	e1a04001 	mov	r4, r1
    da50:	0a000020 	beq	dad8 <xyzModem_stream_terminate+0x98>
    da54:	e59f4098 	ldr	r4, [pc, #152]	; daf4 <xyzModem_stream_terminate+0xb4>
    da58:	e5943414 	ldr	r3, [r4, #1044]
    da5c:	e2433001 	sub	r3, r3, #1	; 0x1
    da60:	e3530001 	cmp	r3, #1	; 0x1
    da64:	8a000020 	bhi	daec <xyzModem_stream_terminate+0xac>
    da68:	e3a00018 	mov	r0, #24	; 0x18
    da6c:	ebfff9f3 	bl	c240 <putc>
    da70:	e3a00018 	mov	r0, #24	; 0x18
    da74:	ebfff9f1 	bl	c240 <putc>
    da78:	e3a00018 	mov	r0, #24	; 0x18
    da7c:	ebfff9ef 	bl	c240 <putc>
    da80:	e3a00018 	mov	r0, #24	; 0x18
    da84:	ebfff9ed 	bl	c240 <putc>
    da88:	e3a00008 	mov	r0, #8	; 0x8
    da8c:	ebfff9eb 	bl	c240 <putc>
    da90:	e3a00008 	mov	r0, #8	; 0x8
    da94:	ebfff9e9 	bl	c240 <putc>
    da98:	e3a00008 	mov	r0, #8	; 0x8
    da9c:	ebfff9e7 	bl	c240 <putc>
    daa0:	e3a00008 	mov	r0, #8	; 0x8
    daa4:	ebfff9e5 	bl	c240 <putc>
    daa8:	e1a05004 	mov	r5, r4
    daac:	e28d4007 	add	r4, sp, #7	; 0x7
    dab0:	e5953000 	ldr	r3, [r5]
    dab4:	e1a01004 	mov	r1, r4
    dab8:	e5d30000 	ldrb	r0, [r3]
    dabc:	ebffff2d 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dac0:	e3500000 	cmp	r0, #0	; 0x0
    dac4:	1afffff9 	bne	dab0 <xyzModem_stream_terminate+0x70>
    dac8:	e59f3024 	ldr	r3, [pc, #36]	; daf4 <xyzModem_stream_terminate+0xb4>
    dacc:	e3a02001 	mov	r2, #1	; 0x1
    dad0:	e583242c 	str	r2, [r3, #1068]
    dad4:	ea000004 	b	daec <xyzModem_stream_terminate+0xac>
    dad8:	e12fff34 	blx	r4
    dadc:	e3500000 	cmp	r0, #0	; 0x0
    dae0:	aafffffc 	bge	dad8 <xyzModem_stream_terminate+0x98>
    dae4:	e59f000c 	ldr	r0, [pc, #12]	; daf8 <xyzModem_stream_terminate+0xb8>
    dae8:	ebffeb02 	bl	86f8 <udelay>
    daec:	e28dd00c 	add	sp, sp, #12	; 0xc
    daf0:	e8bd8030 	pop	{r4, r5, pc}
    daf4:	3c00d250 	.word	0x3c00d250
    daf8:	0003d090 	.word	0x0003d090

0000dafc <xyzModem_get_hdr>:
    dafc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    db00:	e59f42a0 	ldr	r4, [pc, #672]	; dda8 <xyzModem_get_hdr+0x2ac>
    db04:	e24dd00c 	sub	sp, sp, #12	; 0xc
    db08:	e5943430 	ldr	r3, [r4, #1072]
    db0c:	e3530000 	cmp	r3, #0	; 0x0
    db10:	0a000003 	beq	db24 <xyzModem_get_hdr+0x28>
    db14:	e3a00006 	mov	r0, #6	; 0x6
    db18:	ebfff9c8 	bl	c240 <putc>
    db1c:	e3a03000 	mov	r3, #0	; 0x0
    db20:	e5843430 	str	r3, [r4, #1072]
    db24:	e3a04000 	mov	r4, #0	; 0x0
    db28:	e59f6278 	ldr	r6, [pc, #632]	; dda8 <xyzModem_get_hdr+0x2ac>
    db2c:	e1a05004 	mov	r5, r4
    db30:	e28d7007 	add	r7, sp, #7	; 0x7
    db34:	e5963000 	ldr	r3, [r6]
    db38:	e1a01007 	mov	r1, r7
    db3c:	e5d30000 	ldrb	r0, [r3]
    db40:	ebffff0c 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    db44:	e3500000 	cmp	r0, #0	; 0x0
    db48:	e2855001 	add	r5, r5, #1	; 0x1
    db4c:	e59f2254 	ldr	r2, [pc, #596]	; dda8 <xyzModem_get_hdr+0x2ac>
    db50:	0a000021 	beq	dbdc <xyzModem_get_hdr+0xe0>
    db54:	e5dd3007 	ldrb	r3, [sp, #7]
    db58:	e3530002 	cmp	r3, #2	; 0x2
    db5c:	0a00000c 	beq	db94 <xyzModem_get_hdr+0x98>
    db60:	8a000002 	bhi	db70 <xyzModem_get_hdr+0x74>
    db64:	e3530001 	cmp	r3, #1	; 0x1
    db68:	1afffff1 	bne	db34 <xyzModem_get_hdr+0x38>
    db6c:	ea000004 	b	db84 <xyzModem_get_hdr+0x88>
    db70:	e3530004 	cmp	r3, #4	; 0x4
    db74:	0a000012 	beq	dbc4 <xyzModem_get_hdr+0xc8>
    db78:	e3530018 	cmp	r3, #24	; 0x18
    db7c:	1affffec 	bne	db34 <xyzModem_get_hdr+0x38>
    db80:	ea000007 	b	dba4 <xyzModem_get_hdr+0xa8>
    db84:	e592341c 	ldr	r3, [r2, #1052]
    db88:	e2833001 	add	r3, r3, #1	; 0x1
    db8c:	e582341c 	str	r3, [r2, #1052]
    db90:	ea00007c 	b	dd88 <xyzModem_get_hdr+0x28c>
    db94:	e5923420 	ldr	r3, [r2, #1056]
    db98:	e2833001 	add	r3, r3, #1	; 0x1
    db9c:	e5823420 	str	r3, [r2, #1056]
    dba0:	ea000078 	b	dd88 <xyzModem_get_hdr+0x28c>
    dba4:	e5963424 	ldr	r3, [r6, #1060]
    dba8:	e2844001 	add	r4, r4, #1	; 0x1
    dbac:	e2833001 	add	r3, r3, #1	; 0x1
    dbb0:	e3540003 	cmp	r4, #3	; 0x3
    dbb4:	e5863424 	str	r3, [r6, #1060]
    dbb8:	1affffdd 	bne	db34 <xyzModem_get_hdr+0x38>
    dbbc:	e3e00004 	mvn	r0, #4	; 0x4
    dbc0:	ea00006e 	b	dd80 <xyzModem_get_hdr+0x284>
    dbc4:	e3550001 	cmp	r5, #1	; 0x1
    dbc8:	1affffd9 	bne	db34 <xyzModem_get_hdr+0x38>
    dbcc:	e3a00006 	mov	r0, #6	; 0x6
    dbd0:	ebfff99a 	bl	c240 <putc>
    dbd4:	e3e00003 	mvn	r0, #3	; 0x3
    dbd8:	ea000068 	b	dd80 <xyzModem_get_hdr+0x284>
    dbdc:	e1a05002 	mov	r5, r2
    dbe0:	e28d4006 	add	r4, sp, #6	; 0x6
    dbe4:	e5953000 	ldr	r3, [r5]
    dbe8:	e1a01004 	mov	r1, r4
    dbec:	e5d30000 	ldrb	r0, [r3]
    dbf0:	ebfffee0 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dbf4:	e3500000 	cmp	r0, #0	; 0x0
    dbf8:	1afffff9 	bne	dbe4 <xyzModem_get_hdr+0xe8>
    dbfc:	e59f01a8 	ldr	r0, [pc, #424]	; ddac <xyzModem_get_hdr+0x2b0>
    dc00:	ebffeabc 	bl	86f8 <udelay>
    dc04:	ea00005a 	b	dd74 <xyzModem_get_hdr+0x278>
    dc08:	e5943000 	ldr	r3, [r4]
    dc0c:	e59f119c 	ldr	r1, [pc, #412]	; ddb0 <xyzModem_get_hdr+0x2b4>
    dc10:	e5d30000 	ldrb	r0, [r3]
    dc14:	ebfffed7 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dc18:	e3500000 	cmp	r0, #0	; 0x0
    dc1c:	0a000054 	beq	dd74 <xyzModem_get_hdr+0x278>
    dc20:	e28d5008 	add	r5, sp, #8	; 0x8
    dc24:	e5753001 	ldrb	r3, [r5, #-1]!
    dc28:	e2842004 	add	r2, r4, #4	; 0x4
    dc2c:	e3530001 	cmp	r3, #1	; 0x1
    dc30:	03a03080 	moveq	r3, #128	; 0x80
    dc34:	13a03b01 	movne	r3, #1024	; 0x400
    dc38:	e5843410 	str	r3, [r4, #1040]
    dc3c:	e5842404 	str	r2, [r4, #1028]
    dc40:	e1a07004 	mov	r7, r4
    dc44:	e3a04000 	mov	r4, #0	; 0x0
    dc48:	ea000008 	b	dc70 <xyzModem_get_hdr+0x174>
    dc4c:	e5973000 	ldr	r3, [r7]
    dc50:	e5d30000 	ldrb	r0, [r3]
    dc54:	ebfffec7 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dc58:	e3500000 	cmp	r0, #0	; 0x0
    dc5c:	e0872004 	add	r2, r7, r4
    dc60:	e2844001 	add	r4, r4, #1	; 0x1
    dc64:	0a000042 	beq	dd74 <xyzModem_get_hdr+0x278>
    dc68:	e5dd3007 	ldrb	r3, [sp, #7]
    dc6c:	e5c23004 	strb	r3, [r2, #4]
    dc70:	e5973410 	ldr	r3, [r7, #1040]
    dc74:	e1a01005 	mov	r1, r5
    dc78:	e1540003 	cmp	r4, r3
    dc7c:	e59f6124 	ldr	r6, [pc, #292]	; dda8 <xyzModem_get_hdr+0x2ac>
    dc80:	bafffff1 	blt	dc4c <xyzModem_get_hdr+0x150>
    dc84:	e5963000 	ldr	r3, [r6]
    dc88:	e59f1124 	ldr	r1, [pc, #292]	; ddb4 <xyzModem_get_hdr+0x2b8>
    dc8c:	e5d30000 	ldrb	r0, [r3]
    dc90:	ebfffeb8 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dc94:	e3500000 	cmp	r0, #0	; 0x0
    dc98:	0a000035 	beq	dd74 <xyzModem_get_hdr+0x278>
    dc9c:	e5963428 	ldr	r3, [r6, #1064]
    dca0:	e3530000 	cmp	r3, #0	; 0x0
    dca4:	0a000005 	beq	dcc0 <xyzModem_get_hdr+0x1c4>
    dca8:	e5963000 	ldr	r3, [r6]
    dcac:	e59f1104 	ldr	r1, [pc, #260]	; ddb8 <xyzModem_get_hdr+0x2bc>
    dcb0:	e5d30000 	ldrb	r0, [r3]
    dcb4:	ebfffeaf 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dcb8:	e3500000 	cmp	r0, #0	; 0x0
    dcbc:	0a00002c 	beq	dd74 <xyzModem_get_hdr+0x278>
    dcc0:	e59f40e0 	ldr	r4, [pc, #224]	; dda8 <xyzModem_get_hdr+0x2ac>
    dcc4:	e5d43409 	ldrb	r3, [r4, #1033]
    dcc8:	e5d42408 	ldrb	r2, [r4, #1032]
    dccc:	e0233002 	eor	r3, r3, r2
    dcd0:	e35300ff 	cmp	r3, #255	; 0xff
    dcd4:	0a000009 	beq	dd00 <xyzModem_get_hdr+0x204>
    dcd8:	e1a05004 	mov	r5, r4
    dcdc:	e28d4006 	add	r4, sp, #6	; 0x6
    dce0:	e5953000 	ldr	r3, [r5]
    dce4:	e1a01004 	mov	r1, r4
    dce8:	e5d30000 	ldrb	r0, [r3]
    dcec:	ebfffea1 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dcf0:	e3500000 	cmp	r0, #0	; 0x0
    dcf4:	1afffff9 	bne	dce0 <xyzModem_get_hdr+0x1e4>
    dcf8:	e3e00005 	mvn	r0, #5	; 0x5
    dcfc:	ea00001f 	b	dd80 <xyzModem_get_hdr+0x284>
    dd00:	e5943428 	ldr	r3, [r4, #1064]
    dd04:	e3530000 	cmp	r3, #0	; 0x0
    dd08:	01a02003 	moveq	r2, r3
    dd0c:	01a0c004 	moveq	ip, r4
    dd10:	05940410 	ldreq	r0, [r4, #1040]
    dd14:	01a01003 	moveq	r1, r3
    dd18:	0a00000a 	beq	dd48 <xyzModem_get_hdr+0x24c>
    dd1c:	e2840004 	add	r0, r4, #4	; 0x4
    dd20:	e5941410 	ldr	r1, [r4, #1040]
    dd24:	eb000110 	bl	e16c <cyg_crc16>
    dd28:	e5d4340b 	ldrb	r3, [r4, #1035]
    dd2c:	e5d4240a 	ldrb	r2, [r4, #1034]
    dd30:	e1833402 	orr	r3, r3, r2, lsl #8
    dd34:	e1500003 	cmp	r0, r3
    dd38:	ea00000a 	b	dd68 <xyzModem_get_hdr+0x26c>
    dd3c:	e5d33004 	ldrb	r3, [r3, #4]
    dd40:	e0813003 	add	r3, r1, r3
    dd44:	e6ff1073 	uxth	r1, r3
    dd48:	e1520000 	cmp	r2, r0
    dd4c:	e08c3002 	add	r3, ip, r2
    dd50:	e2822001 	add	r2, r2, #1	; 0x1
    dd54:	bafffff8 	blt	dd3c <xyzModem_get_hdr+0x240>
    dd58:	e59f3048 	ldr	r3, [pc, #72]	; dda8 <xyzModem_get_hdr+0x2ac>
    dd5c:	e5d3240a 	ldrb	r2, [r3, #1034]
    dd60:	e20130ff 	and	r3, r1, #255	; 0xff
    dd64:	e1520003 	cmp	r2, r3
    dd68:	13e00006 	mvnne	r0, #6	; 0x6
    dd6c:	1a000003 	bne	dd80 <xyzModem_get_hdr+0x284>
    dd70:	ea000001 	b	dd7c <xyzModem_get_hdr+0x280>
    dd74:	e3e00002 	mvn	r0, #2	; 0x2
    dd78:	ea000000 	b	dd80 <xyzModem_get_hdr+0x284>
    dd7c:	e3a00000 	mov	r0, #0	; 0x0
    dd80:	e28dd00c 	add	sp, sp, #12	; 0xc
    dd84:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    dd88:	e59f4018 	ldr	r4, [pc, #24]	; dda8 <xyzModem_get_hdr+0x2ac>
    dd8c:	e59f1028 	ldr	r1, [pc, #40]	; ddbc <xyzModem_get_hdr+0x2c0>
    dd90:	e5943000 	ldr	r3, [r4]
    dd94:	e5d30000 	ldrb	r0, [r3]
    dd98:	ebfffe76 	bl	d778 <CYGACC_COMM_IF_GETC_TIMEOUT>
    dd9c:	e3500000 	cmp	r0, #0	; 0x0
    dda0:	0afffff3 	beq	dd74 <xyzModem_get_hdr+0x278>
    dda4:	eaffff97 	b	dc08 <xyzModem_get_hdr+0x10c>
    dda8:	3c00d250 	.word	0x3c00d250
    ddac:	0003d090 	.word	0x0003d090
    ddb0:	3c00d659 	.word	0x3c00d659
    ddb4:	3c00d65a 	.word	0x3c00d65a
    ddb8:	3c00d65b 	.word	0x3c00d65b
    ddbc:	3c00d658 	.word	0x3c00d658

0000ddc0 <xyzModem_stream_read>:
    ddc0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ddc4:	e59f5240 	ldr	r5, [pc, #576]	; e00c <xyzModem_stream_read+0x24c>
    ddc8:	e24dd004 	sub	sp, sp, #4	; 0x4
    ddcc:	e1a08000 	mov	r8, r0
    ddd0:	e1a07001 	mov	r7, r1
    ddd4:	e1a0b002 	mov	fp, r2
    ddd8:	e3e06004 	mvn	r6, #4	; 0x4
    dddc:	e3a0a000 	mov	sl, #0	; 0x0
    dde0:	e1a09005 	mov	r9, r5
    dde4:	ea00007e 	b	dfe4 <xyzModem_stream_read+0x224>
    dde8:	e5953410 	ldr	r3, [r5, #1040]
    ddec:	e3530000 	cmp	r3, #0	; 0x0
    ddf0:	03a04014 	moveq	r4, #20	; 0x14
    ddf4:	0a00005d 	beq	df70 <xyzModem_stream_read+0x1b0>
    ddf8:	ea000065 	b	df94 <xyzModem_stream_read+0x1d4>
    ddfc:	ebffff3e 	bl	dafc <xyzModem_get_hdr>
    de00:	e2506000 	subs	r6, r0, #0	; 0x0
    de04:	1a000039 	bne	def0 <xyzModem_stream_read+0x130>
    de08:	e5d5240c 	ldrb	r2, [r5, #1036]
    de0c:	e5d51408 	ldrb	r1, [r5, #1032]
    de10:	e2423001 	sub	r3, r2, #1	; 0x1
    de14:	e1510002 	cmp	r1, r2
    de18:	e20330ff 	and	r3, r3, #255	; 0xff
    de1c:	1a00002d 	bne	ded8 <xyzModem_stream_read+0x118>
    de20:	e599c414 	ldr	ip, [r9, #1044]
    de24:	e2813001 	add	r3, r1, #1	; 0x1
    de28:	e5c9340c 	strb	r3, [r9, #1036]
    de2c:	e35c0001 	cmp	ip, #1	; 0x1
    de30:	e3a03001 	mov	r3, #1	; 0x1
    de34:	e5893430 	str	r3, [r9, #1072]
    de38:	0a000002 	beq	de48 <xyzModem_stream_read+0x88>
    de3c:	e5993434 	ldr	r3, [r9, #1076]
    de40:	e3530000 	cmp	r3, #0	; 0x0
    de44:	1a000017 	bne	dea8 <xyzModem_stream_read+0xe8>
    de48:	e5953410 	ldr	r3, [r5, #1040]
    de4c:	e5950404 	ldr	r0, [r5, #1028]
    de50:	e0802003 	add	r2, r0, r3
    de54:	e5523001 	ldrb	r3, [r2, #-1]
    de58:	e353001a 	cmp	r3, #26	; 0x1a
    de5c:	1a00000f 	bne	dea0 <xyzModem_stream_read+0xe0>
    de60:	e5523002 	ldrb	r3, [r2, #-2]
    de64:	e353001a 	cmp	r3, #26	; 0x1a
    de68:	1a00000c 	bne	dea0 <xyzModem_stream_read+0xe0>
    de6c:	e5523003 	ldrb	r3, [r2, #-3]
    de70:	e353001a 	cmp	r3, #26	; 0x1a
    de74:	1a000009 	bne	dea0 <xyzModem_stream_read+0xe0>
    de78:	ea000000 	b	de80 <xyzModem_stream_read+0xc0>
    de7c:	e5851410 	str	r1, [r5, #1040]
    de80:	e5953410 	ldr	r3, [r5, #1040]
    de84:	e3530000 	cmp	r3, #0	; 0x0
    de88:	e0802003 	add	r2, r0, r3
    de8c:	e2431001 	sub	r1, r3, #1	; 0x1
    de90:	0a000002 	beq	dea0 <xyzModem_stream_read+0xe0>
    de94:	e5523001 	ldrb	r3, [r2, #-1]
    de98:	e353001a 	cmp	r3, #26	; 0x1a
    de9c:	0afffff6 	beq	de7c <xyzModem_stream_read+0xbc>
    dea0:	e35c0001 	cmp	ip, #1	; 0x1
    dea4:	0a00003a 	beq	df94 <xyzModem_stream_read+0x1d4>
    dea8:	e5951434 	ldr	r1, [r5, #1076]
    deac:	e3510000 	cmp	r1, #0	; 0x0
    deb0:	0a000037 	beq	df94 <xyzModem_stream_read+0x1d4>
    deb4:	e5953438 	ldr	r3, [r5, #1080]
    deb8:	e5950410 	ldr	r0, [r5, #1040]
    debc:	e0802003 	add	r2, r0, r3
    dec0:	e1520001 	cmp	r2, r1
    dec4:	80803001 	addhi	r3, r0, r1
    dec8:	80623003 	rsbhi	r3, r2, r3
    decc:	e5852438 	str	r2, [r5, #1080]
    ded0:	85853410 	strhi	r3, [r5, #1040]
    ded4:	ea00002e 	b	df94 <xyzModem_stream_read+0x1d4>
    ded8:	e1510003 	cmp	r1, r3
    dedc:	13e06007 	mvnne	r6, #7	; 0x7
    dee0:	1a00001a 	bne	df50 <xyzModem_stream_read+0x190>
    dee4:	e3a00006 	mov	r0, #6	; 0x6
    dee8:	ebfff8d4 	bl	c240 <putc>
    deec:	ea00001f 	b	df70 <xyzModem_stream_read+0x1b0>
    def0:	e3760005 	cmn	r6, #5	; 0x5
    def4:	0a000021 	beq	df80 <xyzModem_stream_read+0x1c0>
    def8:	e3760004 	cmn	r6, #4	; 0x4
    defc:	1a000013 	bne	df50 <xyzModem_stream_read+0x190>
    df00:	e3a00006 	mov	r0, #6	; 0x6
    df04:	ebfff8cd 	bl	c240 <putc>
    df08:	e5953414 	ldr	r3, [r5, #1044]
    df0c:	e3530002 	cmp	r3, #2	; 0x2
    df10:	1a00000b 	bne	df44 <xyzModem_stream_read+0x184>
    df14:	e5950428 	ldr	r0, [r5, #1064]
    df18:	e3500000 	cmp	r0, #0	; 0x0
    df1c:	13a00043 	movne	r0, #67	; 0x43
    df20:	03a00015 	moveq	r0, #21	; 0x15
    df24:	ebfff8c5 	bl	c240 <putc>
    df28:	e5953418 	ldr	r3, [r5, #1048]
    df2c:	e2833001 	add	r3, r3, #1	; 0x1
    df30:	e5853418 	str	r3, [r5, #1048]
    df34:	ebfffef0 	bl	dafc <xyzModem_get_hdr>
    df38:	e1a06000 	mov	r6, r0
    df3c:	e3a00006 	mov	r0, #6	; 0x6
    df40:	ebfff8be 	bl	c240 <putc>
    df44:	e3a03001 	mov	r3, #1	; 0x1
    df48:	e585342c 	str	r3, [r5, #1068]
    df4c:	ea000009 	b	df78 <xyzModem_stream_read+0x1b8>
    df50:	e5950428 	ldr	r0, [r5, #1064]
    df54:	e3500000 	cmp	r0, #0	; 0x0
    df58:	13a00043 	movne	r0, #67	; 0x43
    df5c:	03a00015 	moveq	r0, #21	; 0x15
    df60:	ebfff8b6 	bl	c240 <putc>
    df64:	e5953418 	ldr	r3, [r5, #1048]
    df68:	e2833001 	add	r3, r3, #1	; 0x1
    df6c:	e5853418 	str	r3, [r5, #1048]
    df70:	e2544001 	subs	r4, r4, #1	; 0x1
    df74:	2affffa0 	bcs	ddfc <xyzModem_stream_read+0x3c>
    df78:	e3560000 	cmp	r6, #0	; 0x0
    df7c:	aa000004 	bge	df94 <xyzModem_stream_read+0x1d4>
    df80:	e59f3084 	ldr	r3, [pc, #132]	; e00c <xyzModem_stream_read+0x24c>
    df84:	e3e02000 	mvn	r2, #0	; 0x0
    df88:	e58b6000 	str	r6, [fp]
    df8c:	e5832410 	str	r2, [r3, #1040]
    df90:	ea00001a 	b	e000 <xyzModem_stream_read+0x240>
    df94:	e595342c 	ldr	r3, [r5, #1068]
    df98:	e3530000 	cmp	r3, #0	; 0x0
    df9c:	1a000010 	bne	dfe4 <xyzModem_stream_read+0x224>
    dfa0:	e5954410 	ldr	r4, [r5, #1040]
    dfa4:	e1a00008 	mov	r0, r8
    dfa8:	e1570004 	cmp	r7, r4
    dfac:	b1a04007 	movlt	r4, r7
    dfb0:	a1a04004 	movge	r4, r4
    dfb4:	e1a02004 	mov	r2, r4
    dfb8:	e5951404 	ldr	r1, [r5, #1028]
    dfbc:	ebffcc81 	bl	11c8 <memcpy>
    dfc0:	e5953410 	ldr	r3, [r5, #1040]
    dfc4:	e5952404 	ldr	r2, [r5, #1028]
    dfc8:	e0643003 	rsb	r3, r4, r3
    dfcc:	e0822004 	add	r2, r2, r4
    dfd0:	e5853410 	str	r3, [r5, #1040]
    dfd4:	e5852404 	str	r2, [r5, #1028]
    dfd8:	e0647007 	rsb	r7, r4, r7
    dfdc:	e0888004 	add	r8, r8, r4
    dfe0:	e08aa004 	add	sl, sl, r4
    dfe4:	e595342c 	ldr	r3, [r5, #1068]
    dfe8:	e2733001 	rsbs	r3, r3, #1	; 0x1
    dfec:	33a03000 	movcc	r3, #0	; 0x0
    dff0:	e3570000 	cmp	r7, #0	; 0x0
    dff4:	d3a03000 	movle	r3, #0	; 0x0
    dff8:	e3530000 	cmp	r3, #0	; 0x0
    dffc:	1affff79 	bne	dde8 <xyzModem_stream_read+0x28>
    e000:	e1a0000a 	mov	r0, sl
    e004:	e28dd004 	add	sp, sp, #4	; 0x4
    e008:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e00c:	3c00d250 	.word	0x3c00d250

0000e010 <xyzModem_stream_open>:
    e010:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e014:	e59f4140 	ldr	r4, [pc, #320]	; e15c <xyzModem_stream_open+0x14c>
    e018:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e01c:	e3a05000 	mov	r5, #0	; 0x0
    e020:	e28d3008 	add	r3, sp, #8	; 0x8
    e024:	e3a02001 	mov	r2, #1	; 0x1
    e028:	e5235004 	str	r5, [r3, #-4]!
    e02c:	e5842428 	str	r2, [r4, #1064]
    e030:	e5845410 	str	r5, [r4, #1040]
    e034:	e584542c 	str	r5, [r4, #1068]
    e038:	e5845430 	str	r5, [r4, #1072]
    e03c:	e5902004 	ldr	r2, [r0, #4]
    e040:	e5843000 	str	r3, [r4]
    e044:	e3a00043 	mov	r0, #67	; 0x43
    e048:	e5842414 	str	r2, [r4, #1044]
    e04c:	e5845418 	str	r5, [r4, #1048]
    e050:	e584541c 	str	r5, [r4, #1052]
    e054:	e5845420 	str	r5, [r4, #1056]
    e058:	e5845424 	str	r5, [r4, #1060]
    e05c:	e5845438 	str	r5, [r4, #1080]
    e060:	e5845434 	str	r5, [r4, #1076]
    e064:	e1a08001 	mov	r8, r1
    e068:	ebfff874 	bl	c240 <putc>
    e06c:	e5943414 	ldr	r3, [r4, #1044]
    e070:	e3530001 	cmp	r3, #1	; 0x1
    e074:	05c4340c 	strbeq	r3, [r4, #1036]
    e078:	01a00005 	moveq	r0, r5
    e07c:	0a000034 	beq	e154 <xyzModem_stream_open+0x144>
    e080:	e1a07004 	mov	r7, r4
    e084:	e3a06014 	mov	r6, #20	; 0x14
    e088:	e3a0400a 	mov	r4, #10	; 0xa
    e08c:	e1a0a005 	mov	sl, r5
    e090:	ea00002b 	b	e144 <xyzModem_stream_open+0x134>
    e094:	ebfffe98 	bl	dafc <xyzModem_get_hdr>
    e098:	e2505000 	subs	r5, r0, #0	; 0x0
    e09c:	1a000016 	bne	e0fc <xyzModem_stream_open+0xec>
    e0a0:	e59f20b4 	ldr	r2, [pc, #180]	; e15c <xyzModem_stream_open+0x14c>
    e0a4:	e5d23408 	ldrb	r3, [r2, #1032]
    e0a8:	e3530000 	cmp	r3, #0	; 0x0
    e0ac:	1a00000b 	bne	e0e0 <xyzModem_stream_open+0xd0>
    e0b0:	e1a03002 	mov	r3, r2
    e0b4:	e5930404 	ldr	r0, [r3, #1028]
    e0b8:	e4d02001 	ldrb	r2, [r0], #1
    e0bc:	e3520000 	cmp	r2, #0	; 0x0
    e0c0:	e5830404 	str	r0, [r3, #1028]
    e0c4:	1afffffa 	bne	e0b4 <xyzModem_stream_open+0xa4>
    e0c8:	e59f3090 	ldr	r3, [pc, #144]	; e160 <xyzModem_stream_open+0x150>
    e0cc:	e59f1090 	ldr	r1, [pc, #144]	; e164 <xyzModem_stream_open+0x154>
    e0d0:	ebfffb5a 	bl	ce40 <parse_num>
    e0d4:	e59f3080 	ldr	r3, [pc, #128]	; e15c <xyzModem_stream_open+0x14c>
    e0d8:	e3a02001 	mov	r2, #1	; 0x1
    e0dc:	e5832430 	str	r2, [r3, #1072]
    e0e0:	e59f3074 	ldr	r3, [pc, #116]	; e15c <xyzModem_stream_open+0x14c>
    e0e4:	e3a01000 	mov	r1, #0	; 0x0
    e0e8:	e3a02001 	mov	r2, #1	; 0x1
    e0ec:	e5c3240c 	strb	r2, [r3, #1036]
    e0f0:	e5831410 	str	r1, [r3, #1040]
    e0f4:	e1a00001 	mov	r0, r1
    e0f8:	ea000015 	b	e154 <xyzModem_stream_open+0x144>
    e0fc:	e3750003 	cmn	r5, #3	; 0x3
    e100:	1a00000d 	bne	e13c <xyzModem_stream_open+0x12c>
    e104:	e2444001 	sub	r4, r4, #1	; 0x1
    e108:	e3540000 	cmp	r4, #0	; 0x0
    e10c:	e59f0054 	ldr	r0, [pc, #84]	; e168 <xyzModem_stream_open+0x158>
    e110:	d587a428 	strle	sl, [r7, #1064]
    e114:	ebffe977 	bl	86f8 <udelay>
    e118:	e5970428 	ldr	r0, [r7, #1064]
    e11c:	e3500000 	cmp	r0, #0	; 0x0
    e120:	13a00043 	movne	r0, #67	; 0x43
    e124:	03a00015 	moveq	r0, #21	; 0x15
    e128:	ebfff844 	bl	c240 <putc>
    e12c:	e5973418 	ldr	r3, [r7, #1048]
    e130:	e2833001 	add	r3, r3, #1	; 0x1
    e134:	e5873418 	str	r3, [r7, #1048]
    e138:	ea000001 	b	e144 <xyzModem_stream_open+0x134>
    e13c:	e3750005 	cmn	r5, #5	; 0x5
    e140:	0a000001 	beq	e14c <xyzModem_stream_open+0x13c>
    e144:	e2566001 	subs	r6, r6, #1	; 0x1
    e148:	2affffd1 	bcs	e094 <xyzModem_stream_open+0x84>
    e14c:	e5885000 	str	r5, [r8]
    e150:	e3e00000 	mvn	r0, #0	; 0x0
    e154:	e28dd00c 	add	sp, sp, #12	; 0xc
    e158:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    e15c:	3c00d250 	.word	0x3c00d250
    e160:	00011724 	.word	0x00011724
    e164:	3c00d684 	.word	0x3c00d684
    e168:	0007a120 	.word	0x0007a120

0000e16c <cyg_crc16>:
    e16c:	e3a02000 	mov	r2, #0	; 0x0
    e170:	e92d4010 	push	{r4, lr}
    e174:	e1a0c000 	mov	ip, r0
    e178:	e59f402c 	ldr	r4, [pc, #44]	; e1ac <cyg_crc16+0x40>
    e17c:	e1a00002 	mov	r0, r2
    e180:	ea000006 	b	e1a0 <cyg_crc16+0x34>
    e184:	e7dc3002 	ldrb	r3, [ip, r2]
    e188:	e2822001 	add	r2, r2, #1	; 0x1
    e18c:	e0233420 	eor	r3, r3, r0, lsr #8
    e190:	e1a03083 	lsl	r3, r3, #1
    e194:	e19430b3 	ldrh	r3, [r4, r3]
    e198:	e0233400 	eor	r3, r3, r0, lsl #8
    e19c:	e6ff0073 	uxth	r0, r3
    e1a0:	e1520001 	cmp	r2, r1
    e1a4:	bafffff6 	blt	e184 <cyg_crc16+0x18>
    e1a8:	e8bd8010 	pop	{r4, pc}
    e1ac:	0000f6da 	.word	0x0000f6da

0000e1b0 <ss_jtag_en>:
    e1b0:	e59f300c 	ldr	r3, [pc, #12]	; e1c4 <ss_jtag_en+0x14>
    e1b4:	e2500000 	subs	r0, r0, #0	; 0x0
    e1b8:	13a00001 	movne	r0, #1	; 0x1
    e1bc:	e58308b8 	str	r0, [r3, #2232]
    e1c0:	e12fff1e 	bx	lr
    e1c4:	21e08000 	.word	0x21e08000

0000e1c8 <raise>:
    e1c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e1cc:	e1a01000 	mov	r1, r0
    e1d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    e1d4:	e59f0014 	ldr	r0, [pc, #20]	; e1f0 <raise+0x28>
    e1d8:	e59f3014 	ldr	r3, [pc, #20]	; e1f4 <raise+0x2c>
    e1dc:	e1a0e00f 	mov	lr, pc
    e1e0:	e593f000 	ldr	pc, [r3]
    e1e4:	e3a00000 	mov	r0, #0	; 0x0
    e1e8:	e28dd004 	add	sp, sp, #4	; 0x4
    e1ec:	e8bd8000 	pop	{pc}
    e1f0:	000119c0 	.word	0x000119c0
    e1f4:	3c00d68c 	.word	0x3c00d68c

0000e1f8 <__aeabi_uidiv>:
    e1f8:	e2512001 	subs	r2, r1, #1	; 0x1
    e1fc:	012fff1e 	bxeq	lr
    e200:	3a000074 	bcc	e3d8 <__aeabi_uidiv+0x1e0>
    e204:	e1500001 	cmp	r0, r1
    e208:	9a00006b 	bls	e3bc <__aeabi_uidiv+0x1c4>
    e20c:	e1110002 	tst	r1, r2
    e210:	0a00006c 	beq	e3c8 <__aeabi_uidiv+0x1d0>
    e214:	e16f3f10 	clz	r3, r0
    e218:	e16f2f11 	clz	r2, r1
    e21c:	e0423003 	sub	r3, r2, r3
    e220:	e273301f 	rsbs	r3, r3, #31	; 0x1f
    e224:	10833083 	addne	r3, r3, r3, lsl #1
    e228:	e3a02000 	mov	r2, #0	; 0x0
    e22c:	108ff103 	addne	pc, pc, r3, lsl #2
    e230:	e1a00000 	nop			(mov r0,r0)
    e234:	e1500f81 	cmp	r0, r1, lsl #31
    e238:	e0a22002 	adc	r2, r2, r2
    e23c:	20400f81 	subcs	r0, r0, r1, lsl #31
    e240:	e1500f01 	cmp	r0, r1, lsl #30
    e244:	e0a22002 	adc	r2, r2, r2
    e248:	20400f01 	subcs	r0, r0, r1, lsl #30
    e24c:	e1500e81 	cmp	r0, r1, lsl #29
    e250:	e0a22002 	adc	r2, r2, r2
    e254:	20400e81 	subcs	r0, r0, r1, lsl #29
    e258:	e1500e01 	cmp	r0, r1, lsl #28
    e25c:	e0a22002 	adc	r2, r2, r2
    e260:	20400e01 	subcs	r0, r0, r1, lsl #28
    e264:	e1500d81 	cmp	r0, r1, lsl #27
    e268:	e0a22002 	adc	r2, r2, r2
    e26c:	20400d81 	subcs	r0, r0, r1, lsl #27
    e270:	e1500d01 	cmp	r0, r1, lsl #26
    e274:	e0a22002 	adc	r2, r2, r2
    e278:	20400d01 	subcs	r0, r0, r1, lsl #26
    e27c:	e1500c81 	cmp	r0, r1, lsl #25
    e280:	e0a22002 	adc	r2, r2, r2
    e284:	20400c81 	subcs	r0, r0, r1, lsl #25
    e288:	e1500c01 	cmp	r0, r1, lsl #24
    e28c:	e0a22002 	adc	r2, r2, r2
    e290:	20400c01 	subcs	r0, r0, r1, lsl #24
    e294:	e1500b81 	cmp	r0, r1, lsl #23
    e298:	e0a22002 	adc	r2, r2, r2
    e29c:	20400b81 	subcs	r0, r0, r1, lsl #23
    e2a0:	e1500b01 	cmp	r0, r1, lsl #22
    e2a4:	e0a22002 	adc	r2, r2, r2
    e2a8:	20400b01 	subcs	r0, r0, r1, lsl #22
    e2ac:	e1500a81 	cmp	r0, r1, lsl #21
    e2b0:	e0a22002 	adc	r2, r2, r2
    e2b4:	20400a81 	subcs	r0, r0, r1, lsl #21
    e2b8:	e1500a01 	cmp	r0, r1, lsl #20
    e2bc:	e0a22002 	adc	r2, r2, r2
    e2c0:	20400a01 	subcs	r0, r0, r1, lsl #20
    e2c4:	e1500981 	cmp	r0, r1, lsl #19
    e2c8:	e0a22002 	adc	r2, r2, r2
    e2cc:	20400981 	subcs	r0, r0, r1, lsl #19
    e2d0:	e1500901 	cmp	r0, r1, lsl #18
    e2d4:	e0a22002 	adc	r2, r2, r2
    e2d8:	20400901 	subcs	r0, r0, r1, lsl #18
    e2dc:	e1500881 	cmp	r0, r1, lsl #17
    e2e0:	e0a22002 	adc	r2, r2, r2
    e2e4:	20400881 	subcs	r0, r0, r1, lsl #17
    e2e8:	e1500801 	cmp	r0, r1, lsl #16
    e2ec:	e0a22002 	adc	r2, r2, r2
    e2f0:	20400801 	subcs	r0, r0, r1, lsl #16
    e2f4:	e1500781 	cmp	r0, r1, lsl #15
    e2f8:	e0a22002 	adc	r2, r2, r2
    e2fc:	20400781 	subcs	r0, r0, r1, lsl #15
    e300:	e1500701 	cmp	r0, r1, lsl #14
    e304:	e0a22002 	adc	r2, r2, r2
    e308:	20400701 	subcs	r0, r0, r1, lsl #14
    e30c:	e1500681 	cmp	r0, r1, lsl #13
    e310:	e0a22002 	adc	r2, r2, r2
    e314:	20400681 	subcs	r0, r0, r1, lsl #13
    e318:	e1500601 	cmp	r0, r1, lsl #12
    e31c:	e0a22002 	adc	r2, r2, r2
    e320:	20400601 	subcs	r0, r0, r1, lsl #12
    e324:	e1500581 	cmp	r0, r1, lsl #11
    e328:	e0a22002 	adc	r2, r2, r2
    e32c:	20400581 	subcs	r0, r0, r1, lsl #11
    e330:	e1500501 	cmp	r0, r1, lsl #10
    e334:	e0a22002 	adc	r2, r2, r2
    e338:	20400501 	subcs	r0, r0, r1, lsl #10
    e33c:	e1500481 	cmp	r0, r1, lsl #9
    e340:	e0a22002 	adc	r2, r2, r2
    e344:	20400481 	subcs	r0, r0, r1, lsl #9
    e348:	e1500401 	cmp	r0, r1, lsl #8
    e34c:	e0a22002 	adc	r2, r2, r2
    e350:	20400401 	subcs	r0, r0, r1, lsl #8
    e354:	e1500381 	cmp	r0, r1, lsl #7
    e358:	e0a22002 	adc	r2, r2, r2
    e35c:	20400381 	subcs	r0, r0, r1, lsl #7
    e360:	e1500301 	cmp	r0, r1, lsl #6
    e364:	e0a22002 	adc	r2, r2, r2
    e368:	20400301 	subcs	r0, r0, r1, lsl #6
    e36c:	e1500281 	cmp	r0, r1, lsl #5
    e370:	e0a22002 	adc	r2, r2, r2
    e374:	20400281 	subcs	r0, r0, r1, lsl #5
    e378:	e1500201 	cmp	r0, r1, lsl #4
    e37c:	e0a22002 	adc	r2, r2, r2
    e380:	20400201 	subcs	r0, r0, r1, lsl #4
    e384:	e1500181 	cmp	r0, r1, lsl #3
    e388:	e0a22002 	adc	r2, r2, r2
    e38c:	20400181 	subcs	r0, r0, r1, lsl #3
    e390:	e1500101 	cmp	r0, r1, lsl #2
    e394:	e0a22002 	adc	r2, r2, r2
    e398:	20400101 	subcs	r0, r0, r1, lsl #2
    e39c:	e1500081 	cmp	r0, r1, lsl #1
    e3a0:	e0a22002 	adc	r2, r2, r2
    e3a4:	20400081 	subcs	r0, r0, r1, lsl #1
    e3a8:	e1500001 	cmp	r0, r1
    e3ac:	e0a22002 	adc	r2, r2, r2
    e3b0:	20400001 	subcs	r0, r0, r1
    e3b4:	e1a00002 	mov	r0, r2
    e3b8:	e12fff1e 	bx	lr
    e3bc:	03a00001 	moveq	r0, #1	; 0x1
    e3c0:	13a00000 	movne	r0, #0	; 0x0
    e3c4:	e12fff1e 	bx	lr
    e3c8:	e16f2f11 	clz	r2, r1
    e3cc:	e262201f 	rsb	r2, r2, #31	; 0x1f
    e3d0:	e1a00230 	lsr	r0, r0, r2
    e3d4:	e12fff1e 	bx	lr
    e3d8:	e52de008 	str	lr, [sp, #-8]!
    e3dc:	eb0002c4 	bl	eef4 <__div0>
    e3e0:	e3a00000 	mov	r0, #0	; 0x0
    e3e4:	e49df008 	ldr	pc, [sp], #8

0000e3e8 <__aeabi_uidivmod>:
    e3e8:	e92d4003 	push	{r0, r1, lr}
    e3ec:	ebffff81 	bl	e1f8 <__aeabi_uidiv>
    e3f0:	e8bd4006 	pop	{r1, r2, lr}
    e3f4:	e0030092 	mul	r3, r2, r0
    e3f8:	e0411003 	sub	r1, r1, r3
    e3fc:	e12fff1e 	bx	lr

0000e400 <__aeabi_idiv>:
    e400:	e3510000 	cmp	r1, #0	; 0x0
    e404:	e020c001 	eor	ip, r0, r1
    e408:	0a000080 	beq	e610 <__aeabi_idiv+0x210>
    e40c:	42611000 	rsbmi	r1, r1, #0	; 0x0
    e410:	e2512001 	subs	r2, r1, #1	; 0x1
    e414:	0a000070 	beq	e5dc <__aeabi_idiv+0x1dc>
    e418:	e1b03000 	movs	r3, r0
    e41c:	42603000 	rsbmi	r3, r0, #0	; 0x0
    e420:	e1530001 	cmp	r3, r1
    e424:	9a00006f 	bls	e5e8 <__aeabi_idiv+0x1e8>
    e428:	e1110002 	tst	r1, r2
    e42c:	0a000071 	beq	e5f8 <__aeabi_idiv+0x1f8>
    e430:	e16f2f13 	clz	r2, r3
    e434:	e16f0f11 	clz	r0, r1
    e438:	e0402002 	sub	r2, r0, r2
    e43c:	e272201f 	rsbs	r2, r2, #31	; 0x1f
    e440:	10822082 	addne	r2, r2, r2, lsl #1
    e444:	e3a00000 	mov	r0, #0	; 0x0
    e448:	108ff102 	addne	pc, pc, r2, lsl #2
    e44c:	e1a00000 	nop			(mov r0,r0)
    e450:	e1530f81 	cmp	r3, r1, lsl #31
    e454:	e0a00000 	adc	r0, r0, r0
    e458:	20433f81 	subcs	r3, r3, r1, lsl #31
    e45c:	e1530f01 	cmp	r3, r1, lsl #30
    e460:	e0a00000 	adc	r0, r0, r0
    e464:	20433f01 	subcs	r3, r3, r1, lsl #30
    e468:	e1530e81 	cmp	r3, r1, lsl #29
    e46c:	e0a00000 	adc	r0, r0, r0
    e470:	20433e81 	subcs	r3, r3, r1, lsl #29
    e474:	e1530e01 	cmp	r3, r1, lsl #28
    e478:	e0a00000 	adc	r0, r0, r0
    e47c:	20433e01 	subcs	r3, r3, r1, lsl #28
    e480:	e1530d81 	cmp	r3, r1, lsl #27
    e484:	e0a00000 	adc	r0, r0, r0
    e488:	20433d81 	subcs	r3, r3, r1, lsl #27
    e48c:	e1530d01 	cmp	r3, r1, lsl #26
    e490:	e0a00000 	adc	r0, r0, r0
    e494:	20433d01 	subcs	r3, r3, r1, lsl #26
    e498:	e1530c81 	cmp	r3, r1, lsl #25
    e49c:	e0a00000 	adc	r0, r0, r0
    e4a0:	20433c81 	subcs	r3, r3, r1, lsl #25
    e4a4:	e1530c01 	cmp	r3, r1, lsl #24
    e4a8:	e0a00000 	adc	r0, r0, r0
    e4ac:	20433c01 	subcs	r3, r3, r1, lsl #24
    e4b0:	e1530b81 	cmp	r3, r1, lsl #23
    e4b4:	e0a00000 	adc	r0, r0, r0
    e4b8:	20433b81 	subcs	r3, r3, r1, lsl #23
    e4bc:	e1530b01 	cmp	r3, r1, lsl #22
    e4c0:	e0a00000 	adc	r0, r0, r0
    e4c4:	20433b01 	subcs	r3, r3, r1, lsl #22
    e4c8:	e1530a81 	cmp	r3, r1, lsl #21
    e4cc:	e0a00000 	adc	r0, r0, r0
    e4d0:	20433a81 	subcs	r3, r3, r1, lsl #21
    e4d4:	e1530a01 	cmp	r3, r1, lsl #20
    e4d8:	e0a00000 	adc	r0, r0, r0
    e4dc:	20433a01 	subcs	r3, r3, r1, lsl #20
    e4e0:	e1530981 	cmp	r3, r1, lsl #19
    e4e4:	e0a00000 	adc	r0, r0, r0
    e4e8:	20433981 	subcs	r3, r3, r1, lsl #19
    e4ec:	e1530901 	cmp	r3, r1, lsl #18
    e4f0:	e0a00000 	adc	r0, r0, r0
    e4f4:	20433901 	subcs	r3, r3, r1, lsl #18
    e4f8:	e1530881 	cmp	r3, r1, lsl #17
    e4fc:	e0a00000 	adc	r0, r0, r0
    e500:	20433881 	subcs	r3, r3, r1, lsl #17
    e504:	e1530801 	cmp	r3, r1, lsl #16
    e508:	e0a00000 	adc	r0, r0, r0
    e50c:	20433801 	subcs	r3, r3, r1, lsl #16
    e510:	e1530781 	cmp	r3, r1, lsl #15
    e514:	e0a00000 	adc	r0, r0, r0
    e518:	20433781 	subcs	r3, r3, r1, lsl #15
    e51c:	e1530701 	cmp	r3, r1, lsl #14
    e520:	e0a00000 	adc	r0, r0, r0
    e524:	20433701 	subcs	r3, r3, r1, lsl #14
    e528:	e1530681 	cmp	r3, r1, lsl #13
    e52c:	e0a00000 	adc	r0, r0, r0
    e530:	20433681 	subcs	r3, r3, r1, lsl #13
    e534:	e1530601 	cmp	r3, r1, lsl #12
    e538:	e0a00000 	adc	r0, r0, r0
    e53c:	20433601 	subcs	r3, r3, r1, lsl #12
    e540:	e1530581 	cmp	r3, r1, lsl #11
    e544:	e0a00000 	adc	r0, r0, r0
    e548:	20433581 	subcs	r3, r3, r1, lsl #11
    e54c:	e1530501 	cmp	r3, r1, lsl #10
    e550:	e0a00000 	adc	r0, r0, r0
    e554:	20433501 	subcs	r3, r3, r1, lsl #10
    e558:	e1530481 	cmp	r3, r1, lsl #9
    e55c:	e0a00000 	adc	r0, r0, r0
    e560:	20433481 	subcs	r3, r3, r1, lsl #9
    e564:	e1530401 	cmp	r3, r1, lsl #8
    e568:	e0a00000 	adc	r0, r0, r0
    e56c:	20433401 	subcs	r3, r3, r1, lsl #8
    e570:	e1530381 	cmp	r3, r1, lsl #7
    e574:	e0a00000 	adc	r0, r0, r0
    e578:	20433381 	subcs	r3, r3, r1, lsl #7
    e57c:	e1530301 	cmp	r3, r1, lsl #6
    e580:	e0a00000 	adc	r0, r0, r0
    e584:	20433301 	subcs	r3, r3, r1, lsl #6
    e588:	e1530281 	cmp	r3, r1, lsl #5
    e58c:	e0a00000 	adc	r0, r0, r0
    e590:	20433281 	subcs	r3, r3, r1, lsl #5
    e594:	e1530201 	cmp	r3, r1, lsl #4
    e598:	e0a00000 	adc	r0, r0, r0
    e59c:	20433201 	subcs	r3, r3, r1, lsl #4
    e5a0:	e1530181 	cmp	r3, r1, lsl #3
    e5a4:	e0a00000 	adc	r0, r0, r0
    e5a8:	20433181 	subcs	r3, r3, r1, lsl #3
    e5ac:	e1530101 	cmp	r3, r1, lsl #2
    e5b0:	e0a00000 	adc	r0, r0, r0
    e5b4:	20433101 	subcs	r3, r3, r1, lsl #2
    e5b8:	e1530081 	cmp	r3, r1, lsl #1
    e5bc:	e0a00000 	adc	r0, r0, r0
    e5c0:	20433081 	subcs	r3, r3, r1, lsl #1
    e5c4:	e1530001 	cmp	r3, r1
    e5c8:	e0a00000 	adc	r0, r0, r0
    e5cc:	20433001 	subcs	r3, r3, r1
    e5d0:	e35c0000 	cmp	ip, #0	; 0x0
    e5d4:	42600000 	rsbmi	r0, r0, #0	; 0x0
    e5d8:	e12fff1e 	bx	lr
    e5dc:	e13c0000 	teq	ip, r0
    e5e0:	42600000 	rsbmi	r0, r0, #0	; 0x0
    e5e4:	e12fff1e 	bx	lr
    e5e8:	33a00000 	movcc	r0, #0	; 0x0
    e5ec:	01a00fcc 	asreq	r0, ip, #31
    e5f0:	03800001 	orreq	r0, r0, #1	; 0x1
    e5f4:	e12fff1e 	bx	lr
    e5f8:	e16f2f11 	clz	r2, r1
    e5fc:	e262201f 	rsb	r2, r2, #31	; 0x1f
    e600:	e35c0000 	cmp	ip, #0	; 0x0
    e604:	e1a00233 	lsr	r0, r3, r2
    e608:	42600000 	rsbmi	r0, r0, #0	; 0x0
    e60c:	e12fff1e 	bx	lr
    e610:	e52de008 	str	lr, [sp, #-8]!
    e614:	eb000236 	bl	eef4 <__div0>
    e618:	e3a00000 	mov	r0, #0	; 0x0
    e61c:	e49df008 	ldr	pc, [sp], #8

0000e620 <__aeabi_idivmod>:
    e620:	e92d4003 	push	{r0, r1, lr}
    e624:	ebffff75 	bl	e400 <__aeabi_idiv>
    e628:	e8bd4006 	pop	{r1, r2, lr}
    e62c:	e0030092 	mul	r3, r2, r0
    e630:	e0411003 	sub	r1, r1, r3
    e634:	e12fff1e 	bx	lr

0000e638 <__aeabi_llsr>:
    e638:	e2523020 	subs	r3, r2, #32	; 0x20
    e63c:	e262c020 	rsb	ip, r2, #32	; 0x20
    e640:	41a00230 	lsrmi	r0, r0, r2
    e644:	51a00331 	lsrpl	r0, r1, r3
    e648:	41800c11 	orrmi	r0, r0, r1, lsl ip
    e64c:	e1a01231 	lsr	r1, r1, r2
    e650:	e12fff1e 	bx	lr

0000e654 <__aeabi_llsl>:
    e654:	e2523020 	subs	r3, r2, #32	; 0x20
    e658:	e262c020 	rsb	ip, r2, #32	; 0x20
    e65c:	41a01211 	lslmi	r1, r1, r2
    e660:	51a01310 	lslpl	r1, r0, r3
    e664:	41811c30 	orrmi	r1, r1, r0, lsr ip
    e668:	e1a00210 	lsl	r0, r0, r2
    e66c:	e12fff1e 	bx	lr

0000e670 <__aeabi_drsub>:
    e670:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
    e674:	ea000000 	b	e67c <__adddf3>

0000e678 <__aeabi_dsub>:
    e678:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

0000e67c <__adddf3>:
    e67c:	e92d4030 	push	{r4, r5, lr}
    e680:	e1a04081 	lsl	r4, r1, #1
    e684:	e1a05083 	lsl	r5, r3, #1
    e688:	e1340005 	teq	r4, r5
    e68c:	01300002 	teqeq	r0, r2
    e690:	1194c000 	orrsne	ip, r4, r0
    e694:	1195c002 	orrsne	ip, r5, r2
    e698:	11f0cac4 	mvnsne	ip, r4, asr #21
    e69c:	11f0cac5 	mvnsne	ip, r5, asr #21
    e6a0:	0a000079 	beq	e88c <__adddf3+0x210>
    e6a4:	e1a04aa4 	lsr	r4, r4, #21
    e6a8:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
    e6ac:	b2655000 	rsblt	r5, r5, #0	; 0x0
    e6b0:	da000006 	ble	e6d0 <__adddf3+0x54>
    e6b4:	e0844005 	add	r4, r4, r5
    e6b8:	e0202002 	eor	r2, r0, r2
    e6bc:	e0213003 	eor	r3, r1, r3
    e6c0:	e0220000 	eor	r0, r2, r0
    e6c4:	e0231001 	eor	r1, r3, r1
    e6c8:	e0202002 	eor	r2, r0, r2
    e6cc:	e0213003 	eor	r3, r1, r3
    e6d0:	e3550036 	cmp	r5, #54	; 0x36
    e6d4:	88bd8030 	pophi	{r4, r5, pc}
    e6d8:	e3110102 	tst	r1, #-2147483648	; 0x80000000
    e6dc:	e1a01601 	lsl	r1, r1, #12
    e6e0:	e3a0c601 	mov	ip, #1048576	; 0x100000
    e6e4:	e18c1621 	orr	r1, ip, r1, lsr #12
    e6e8:	0a000001 	beq	e6f4 <__adddf3+0x78>
    e6ec:	e2700000 	rsbs	r0, r0, #0	; 0x0
    e6f0:	e2e11000 	rsc	r1, r1, #0	; 0x0
    e6f4:	e3130102 	tst	r3, #-2147483648	; 0x80000000
    e6f8:	e1a03603 	lsl	r3, r3, #12
    e6fc:	e18c3623 	orr	r3, ip, r3, lsr #12
    e700:	0a000001 	beq	e70c <__adddf3+0x90>
    e704:	e2722000 	rsbs	r2, r2, #0	; 0x0
    e708:	e2e33000 	rsc	r3, r3, #0	; 0x0
    e70c:	e1340005 	teq	r4, r5
    e710:	0a000057 	beq	e874 <__adddf3+0x1f8>
    e714:	e2444001 	sub	r4, r4, #1	; 0x1
    e718:	e275e020 	rsbs	lr, r5, #32	; 0x20
    e71c:	ba000005 	blt	e738 <__adddf3+0xbc>
    e720:	e1a0ce12 	lsl	ip, r2, lr
    e724:	e0900532 	adds	r0, r0, r2, lsr r5
    e728:	e2a11000 	adc	r1, r1, #0	; 0x0
    e72c:	e0900e13 	adds	r0, r0, r3, lsl lr
    e730:	e0b11553 	adcs	r1, r1, r3, asr r5
    e734:	ea000006 	b	e754 <__adddf3+0xd8>
    e738:	e2455020 	sub	r5, r5, #32	; 0x20
    e73c:	e28ee020 	add	lr, lr, #32	; 0x20
    e740:	e3520001 	cmp	r2, #1	; 0x1
    e744:	e1a0ce13 	lsl	ip, r3, lr
    e748:	238cc002 	orrcs	ip, ip, #2	; 0x2
    e74c:	e0900553 	adds	r0, r0, r3, asr r5
    e750:	e0b11fc3 	adcs	r1, r1, r3, asr #31
    e754:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    e758:	5a000002 	bpl	e768 <__adddf3+0xec>
    e75c:	e27cc000 	rsbs	ip, ip, #0	; 0x0
    e760:	e2f00000 	rscs	r0, r0, #0	; 0x0
    e764:	e2e11000 	rsc	r1, r1, #0	; 0x0
    e768:	e3510601 	cmp	r1, #1048576	; 0x100000
    e76c:	3a00000e 	bcc	e7ac <__adddf3+0x130>
    e770:	e3510602 	cmp	r1, #2097152	; 0x200000
    e774:	3a000006 	bcc	e794 <__adddf3+0x118>
    e778:	e1b010a1 	lsrs	r1, r1, #1
    e77c:	e1b00060 	rrxs	r0, r0
    e780:	e1a0c06c 	rrx	ip, ip
    e784:	e2844001 	add	r4, r4, #1	; 0x1
    e788:	e1a02a84 	lsl	r2, r4, #21
    e78c:	e3720501 	cmn	r2, #4194304	; 0x400000
    e790:	2a000055 	bcs	e8ec <__adddf3+0x270>
    e794:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
    e798:	01b0c0a0 	lsrseq	ip, r0, #1
    e79c:	e2b00000 	adcs	r0, r0, #0	; 0x0
    e7a0:	e0a11a04 	adc	r1, r1, r4, lsl #20
    e7a4:	e1811005 	orr	r1, r1, r5
    e7a8:	e8bd8030 	pop	{r4, r5, pc}
    e7ac:	e1b0c08c 	lsls	ip, ip, #1
    e7b0:	e0b00000 	adcs	r0, r0, r0
    e7b4:	e0a11001 	adc	r1, r1, r1
    e7b8:	e3110601 	tst	r1, #1048576	; 0x100000
    e7bc:	e2444001 	sub	r4, r4, #1	; 0x1
    e7c0:	1afffff3 	bne	e794 <__adddf3+0x118>
    e7c4:	e3310000 	teq	r1, #0	; 0x0
    e7c8:	01a01000 	moveq	r1, r0
    e7cc:	03a00000 	moveq	r0, #0	; 0x0
    e7d0:	e16f3f11 	clz	r3, r1
    e7d4:	02833020 	addeq	r3, r3, #32	; 0x20
    e7d8:	e243300b 	sub	r3, r3, #11	; 0xb
    e7dc:	e2532020 	subs	r2, r3, #32	; 0x20
    e7e0:	aa000007 	bge	e804 <__adddf3+0x188>
    e7e4:	e292200c 	adds	r2, r2, #12	; 0xc
    e7e8:	da000004 	ble	e800 <__adddf3+0x184>
    e7ec:	e282c014 	add	ip, r2, #20	; 0x14
    e7f0:	e262200c 	rsb	r2, r2, #12	; 0xc
    e7f4:	e1a00c11 	lsl	r0, r1, ip
    e7f8:	e1a01231 	lsr	r1, r1, r2
    e7fc:	ea000004 	b	e814 <__adddf3+0x198>
    e800:	e2822014 	add	r2, r2, #20	; 0x14
    e804:	d262c020 	rsble	ip, r2, #32	; 0x20
    e808:	e1a01211 	lsl	r1, r1, r2
    e80c:	d1811c30 	orrle	r1, r1, r0, lsr ip
    e810:	d1a00210 	lslle	r0, r0, r2
    e814:	e0544003 	subs	r4, r4, r3
    e818:	a0811a04 	addge	r1, r1, r4, lsl #20
    e81c:	a1811005 	orrge	r1, r1, r5
    e820:	a8bd8030 	popge	{r4, r5, pc}
    e824:	e1e04004 	mvn	r4, r4
    e828:	e254401f 	subs	r4, r4, #31	; 0x1f
    e82c:	aa00000d 	bge	e868 <__adddf3+0x1ec>
    e830:	e294400c 	adds	r4, r4, #12	; 0xc
    e834:	ca000005 	bgt	e850 <__adddf3+0x1d4>
    e838:	e2844014 	add	r4, r4, #20	; 0x14
    e83c:	e2642020 	rsb	r2, r4, #32	; 0x20
    e840:	e1a00430 	lsr	r0, r0, r4
    e844:	e1800211 	orr	r0, r0, r1, lsl r2
    e848:	e1851431 	orr	r1, r5, r1, lsr r4
    e84c:	e8bd8030 	pop	{r4, r5, pc}
    e850:	e264400c 	rsb	r4, r4, #12	; 0xc
    e854:	e2642020 	rsb	r2, r4, #32	; 0x20
    e858:	e1a00230 	lsr	r0, r0, r2
    e85c:	e1800411 	orr	r0, r0, r1, lsl r4
    e860:	e1a01005 	mov	r1, r5
    e864:	e8bd8030 	pop	{r4, r5, pc}
    e868:	e1a00431 	lsr	r0, r1, r4
    e86c:	e1a01005 	mov	r1, r5
    e870:	e8bd8030 	pop	{r4, r5, pc}
    e874:	e3340000 	teq	r4, #0	; 0x0
    e878:	e2233601 	eor	r3, r3, #1048576	; 0x100000
    e87c:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
    e880:	02844001 	addeq	r4, r4, #1	; 0x1
    e884:	12455001 	subne	r5, r5, #1	; 0x1
    e888:	eaffffa1 	b	e714 <__adddf3+0x98>
    e88c:	e1f0cac4 	mvns	ip, r4, asr #21
    e890:	11f0cac5 	mvnsne	ip, r5, asr #21
    e894:	0a000018 	beq	e8fc <__adddf3+0x280>
    e898:	e1340005 	teq	r4, r5
    e89c:	01300002 	teqeq	r0, r2
    e8a0:	0a000003 	beq	e8b4 <__adddf3+0x238>
    e8a4:	e194c000 	orrs	ip, r4, r0
    e8a8:	01a01003 	moveq	r1, r3
    e8ac:	01a00002 	moveq	r0, r2
    e8b0:	e8bd8030 	pop	{r4, r5, pc}
    e8b4:	e1310003 	teq	r1, r3
    e8b8:	13a01000 	movne	r1, #0	; 0x0
    e8bc:	13a00000 	movne	r0, #0	; 0x0
    e8c0:	18bd8030 	popne	{r4, r5, pc}
    e8c4:	e1b0caa4 	lsrs	ip, r4, #21
    e8c8:	1a000003 	bne	e8dc <__adddf3+0x260>
    e8cc:	e1b00080 	lsls	r0, r0, #1
    e8d0:	e0b11001 	adcs	r1, r1, r1
    e8d4:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
    e8d8:	e8bd8030 	pop	{r4, r5, pc}
    e8dc:	e2944501 	adds	r4, r4, #4194304	; 0x400000
    e8e0:	32811601 	addcc	r1, r1, #1048576	; 0x100000
    e8e4:	38bd8030 	popcc	{r4, r5, pc}
    e8e8:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    e8ec:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
    e8f0:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
    e8f4:	e3a00000 	mov	r0, #0	; 0x0
    e8f8:	e8bd8030 	pop	{r4, r5, pc}
    e8fc:	e1f0cac4 	mvns	ip, r4, asr #21
    e900:	11a01003 	movne	r1, r3
    e904:	11a00002 	movne	r0, r2
    e908:	01f0cac5 	mvnseq	ip, r5, asr #21
    e90c:	11a03001 	movne	r3, r1
    e910:	11a02000 	movne	r2, r0
    e914:	e1904601 	orrs	r4, r0, r1, lsl #12
    e918:	01925603 	orrseq	r5, r2, r3, lsl #12
    e91c:	01310003 	teqeq	r1, r3
    e920:	13811702 	orrne	r1, r1, #524288	; 0x80000
    e924:	e8bd8030 	pop	{r4, r5, pc}

0000e928 <__aeabi_ui2d>:
    e928:	e3300000 	teq	r0, #0	; 0x0
    e92c:	03a01000 	moveq	r1, #0	; 0x0
    e930:	012fff1e 	bxeq	lr
    e934:	e92d4030 	push	{r4, r5, lr}
    e938:	e3a04b01 	mov	r4, #1024	; 0x400
    e93c:	e2844032 	add	r4, r4, #50	; 0x32
    e940:	e3a05000 	mov	r5, #0	; 0x0
    e944:	e3a01000 	mov	r1, #0	; 0x0
    e948:	eaffff9d 	b	e7c4 <__adddf3+0x148>

0000e94c <__aeabi_i2d>:
    e94c:	e3300000 	teq	r0, #0	; 0x0
    e950:	03a01000 	moveq	r1, #0	; 0x0
    e954:	012fff1e 	bxeq	lr
    e958:	e92d4030 	push	{r4, r5, lr}
    e95c:	e3a04b01 	mov	r4, #1024	; 0x400
    e960:	e2844032 	add	r4, r4, #50	; 0x32
    e964:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
    e968:	42600000 	rsbmi	r0, r0, #0	; 0x0
    e96c:	e3a01000 	mov	r1, #0	; 0x0
    e970:	eaffff93 	b	e7c4 <__adddf3+0x148>

0000e974 <__aeabi_f2d>:
    e974:	e1b02080 	lsls	r2, r0, #1
    e978:	e1a011c2 	asr	r1, r2, #3
    e97c:	e1a01061 	rrx	r1, r1
    e980:	e1a00e02 	lsl	r0, r2, #28
    e984:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
    e988:	133304ff 	teqne	r3, #-16777216	; 0xff000000
    e98c:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
    e990:	112fff1e 	bxne	lr
    e994:	e3320000 	teq	r2, #0	; 0x0
    e998:	133304ff 	teqne	r3, #-16777216	; 0xff000000
    e99c:	012fff1e 	bxeq	lr
    e9a0:	e92d4030 	push	{r4, r5, lr}
    e9a4:	e3a04d0e 	mov	r4, #896	; 0x380
    e9a8:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    e9ac:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
    e9b0:	eaffff83 	b	e7c4 <__adddf3+0x148>

0000e9b4 <__aeabi_ul2d>:
    e9b4:	e1902001 	orrs	r2, r0, r1
    e9b8:	012fff1e 	bxeq	lr
    e9bc:	e92d4030 	push	{r4, r5, lr}
    e9c0:	e3a05000 	mov	r5, #0	; 0x0
    e9c4:	ea000006 	b	e9e4 <__aeabi_l2d+0x1c>

0000e9c8 <__aeabi_l2d>:
    e9c8:	e1902001 	orrs	r2, r0, r1
    e9cc:	012fff1e 	bxeq	lr
    e9d0:	e92d4030 	push	{r4, r5, lr}
    e9d4:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
    e9d8:	5a000001 	bpl	e9e4 <__aeabi_l2d+0x1c>
    e9dc:	e2700000 	rsbs	r0, r0, #0	; 0x0
    e9e0:	e2e11000 	rsc	r1, r1, #0	; 0x0
    e9e4:	e3a04b01 	mov	r4, #1024	; 0x400
    e9e8:	e2844032 	add	r4, r4, #50	; 0x32
    e9ec:	e1b0cb21 	lsrs	ip, r1, #22
    e9f0:	0affff5c 	beq	e768 <__adddf3+0xec>
    e9f4:	e3a02003 	mov	r2, #3	; 0x3
    e9f8:	e1b0c1ac 	lsrs	ip, ip, #3
    e9fc:	12822003 	addne	r2, r2, #3	; 0x3
    ea00:	e1b0c1ac 	lsrs	ip, ip, #3
    ea04:	12822003 	addne	r2, r2, #3	; 0x3
    ea08:	e08221ac 	add	r2, r2, ip, lsr #3
    ea0c:	e2623020 	rsb	r3, r2, #32	; 0x20
    ea10:	e1a0c310 	lsl	ip, r0, r3
    ea14:	e1a00230 	lsr	r0, r0, r2
    ea18:	e1800311 	orr	r0, r0, r1, lsl r3
    ea1c:	e1a01231 	lsr	r1, r1, r2
    ea20:	e0844002 	add	r4, r4, r2
    ea24:	eaffff4f 	b	e768 <__adddf3+0xec>

0000ea28 <__aeabi_dmul>:
    ea28:	e92d4070 	push	{r4, r5, r6, lr}
    ea2c:	e3a0c0ff 	mov	ip, #255	; 0xff
    ea30:	e38ccc07 	orr	ip, ip, #1792	; 0x700
    ea34:	e01c4a21 	ands	r4, ip, r1, lsr #20
    ea38:	101c5a23 	andsne	r5, ip, r3, lsr #20
    ea3c:	1134000c 	teqne	r4, ip
    ea40:	1135000c 	teqne	r5, ip
    ea44:	0b00006f 	bleq	ec08 <__aeabi_dmul+0x1e0>
    ea48:	e0844005 	add	r4, r4, r5
    ea4c:	e0216003 	eor	r6, r1, r3
    ea50:	e1c11a8c 	bic	r1, r1, ip, lsl #21
    ea54:	e1c33a8c 	bic	r3, r3, ip, lsl #21
    ea58:	e1905601 	orrs	r5, r0, r1, lsl #12
    ea5c:	11925603 	orrsne	r5, r2, r3, lsl #12
    ea60:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    ea64:	e3833601 	orr	r3, r3, #1048576	; 0x100000
    ea68:	0a00001c 	beq	eae0 <__aeabi_dmul+0xb8>
    ea6c:	e08ec290 	umull	ip, lr, r0, r2
    ea70:	e3a05000 	mov	r5, #0	; 0x0
    ea74:	e0a5e291 	umlal	lr, r5, r1, r2
    ea78:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
    ea7c:	e0a5e390 	umlal	lr, r5, r0, r3
    ea80:	e3a06000 	mov	r6, #0	; 0x0
    ea84:	e0a65391 	umlal	r5, r6, r1, r3
    ea88:	e33c0000 	teq	ip, #0	; 0x0
    ea8c:	138ee001 	orrne	lr, lr, #1	; 0x1
    ea90:	e24440ff 	sub	r4, r4, #255	; 0xff
    ea94:	e3560c02 	cmp	r6, #512	; 0x200
    ea98:	e2c44c03 	sbc	r4, r4, #768	; 0x300
    ea9c:	2a000002 	bcs	eaac <__aeabi_dmul+0x84>
    eaa0:	e1b0e08e 	lsls	lr, lr, #1
    eaa4:	e0b55005 	adcs	r5, r5, r5
    eaa8:	e0a66006 	adc	r6, r6, r6
    eaac:	e1821586 	orr	r1, r2, r6, lsl #11
    eab0:	e1811aa5 	orr	r1, r1, r5, lsr #21
    eab4:	e1a00585 	lsl	r0, r5, #11
    eab8:	e1800aae 	orr	r0, r0, lr, lsr #21
    eabc:	e1a0e58e 	lsl	lr, lr, #11
    eac0:	e254c0fd 	subs	ip, r4, #253	; 0xfd
    eac4:	835c0c07 	cmphi	ip, #1792	; 0x700
    eac8:	8a00000f 	bhi	eb0c <__aeabi_dmul+0xe4>
    eacc:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
    ead0:	01b0e0a0 	lsrseq	lr, r0, #1
    ead4:	e2b00000 	adcs	r0, r0, #0	; 0x0
    ead8:	e0a11a04 	adc	r1, r1, r4, lsl #20
    eadc:	e8bd8070 	pop	{r4, r5, r6, pc}
    eae0:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
    eae4:	e1861001 	orr	r1, r6, r1
    eae8:	e1800002 	orr	r0, r0, r2
    eaec:	e0211003 	eor	r1, r1, r3
    eaf0:	e05440ac 	subs	r4, r4, ip, lsr #1
    eaf4:	c074500c 	rsbsgt	r5, r4, ip
    eaf8:	c1811a04 	orrgt	r1, r1, r4, lsl #20
    eafc:	c8bd8070 	popgt	{r4, r5, r6, pc}
    eb00:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    eb04:	e3a0e000 	mov	lr, #0	; 0x0
    eb08:	e2544001 	subs	r4, r4, #1	; 0x1
    eb0c:	ca000058 	bgt	ec74 <__aeabi_dmul+0x24c>
    eb10:	e3740036 	cmn	r4, #54	; 0x36
    eb14:	d3a00000 	movle	r0, #0	; 0x0
    eb18:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
    eb1c:	d8bd8070 	pople	{r4, r5, r6, pc}
    eb20:	e2644000 	rsb	r4, r4, #0	; 0x0
    eb24:	e2544020 	subs	r4, r4, #32	; 0x20
    eb28:	aa000018 	bge	eb90 <__aeabi_dmul+0x168>
    eb2c:	e294400c 	adds	r4, r4, #12	; 0xc
    eb30:	ca00000b 	bgt	eb64 <__aeabi_dmul+0x13c>
    eb34:	e2844014 	add	r4, r4, #20	; 0x14
    eb38:	e2645020 	rsb	r5, r4, #32	; 0x20
    eb3c:	e1a03510 	lsl	r3, r0, r5
    eb40:	e1a00430 	lsr	r0, r0, r4
    eb44:	e1800511 	orr	r0, r0, r1, lsl r5
    eb48:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
    eb4c:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
    eb50:	e0900fa3 	adds	r0, r0, r3, lsr #31
    eb54:	e0a21431 	adc	r1, r2, r1, lsr r4
    eb58:	e19ee083 	orrs	lr, lr, r3, lsl #1
    eb5c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    eb60:	e8bd8070 	pop	{r4, r5, r6, pc}
    eb64:	e264400c 	rsb	r4, r4, #12	; 0xc
    eb68:	e2645020 	rsb	r5, r4, #32	; 0x20
    eb6c:	e1a03410 	lsl	r3, r0, r4
    eb70:	e1a00530 	lsr	r0, r0, r5
    eb74:	e1800411 	orr	r0, r0, r1, lsl r4
    eb78:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    eb7c:	e0900fa3 	adds	r0, r0, r3, lsr #31
    eb80:	e2a11000 	adc	r1, r1, #0	; 0x0
    eb84:	e19ee083 	orrs	lr, lr, r3, lsl #1
    eb88:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    eb8c:	e8bd8070 	pop	{r4, r5, r6, pc}
    eb90:	e2645020 	rsb	r5, r4, #32	; 0x20
    eb94:	e18ee510 	orr	lr, lr, r0, lsl r5
    eb98:	e1a03430 	lsr	r3, r0, r4
    eb9c:	e1833511 	orr	r3, r3, r1, lsl r5
    eba0:	e1a00431 	lsr	r0, r1, r4
    eba4:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    eba8:	e1c00431 	bic	r0, r0, r1, lsr r4
    ebac:	e0800fa3 	add	r0, r0, r3, lsr #31
    ebb0:	e19ee083 	orrs	lr, lr, r3, lsl #1
    ebb4:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    ebb8:	e8bd8070 	pop	{r4, r5, r6, pc}
    ebbc:	e3340000 	teq	r4, #0	; 0x0
    ebc0:	1a000008 	bne	ebe8 <__aeabi_dmul+0x1c0>
    ebc4:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
    ebc8:	e1b00080 	lsls	r0, r0, #1
    ebcc:	e0a11001 	adc	r1, r1, r1
    ebd0:	e3110601 	tst	r1, #1048576	; 0x100000
    ebd4:	02444001 	subeq	r4, r4, #1	; 0x1
    ebd8:	0afffffa 	beq	ebc8 <__aeabi_dmul+0x1a0>
    ebdc:	e1811006 	orr	r1, r1, r6
    ebe0:	e3350000 	teq	r5, #0	; 0x0
    ebe4:	11a0f00e 	movne	pc, lr
    ebe8:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
    ebec:	e1b02082 	lsls	r2, r2, #1
    ebf0:	e0a33003 	adc	r3, r3, r3
    ebf4:	e3130601 	tst	r3, #1048576	; 0x100000
    ebf8:	02455001 	subeq	r5, r5, #1	; 0x1
    ebfc:	0afffffa 	beq	ebec <__aeabi_dmul+0x1c4>
    ec00:	e1833006 	orr	r3, r3, r6
    ec04:	e1a0f00e 	mov	pc, lr
    ec08:	e134000c 	teq	r4, ip
    ec0c:	e00c5a23 	and	r5, ip, r3, lsr #20
    ec10:	1135000c 	teqne	r5, ip
    ec14:	0a000006 	beq	ec34 <__aeabi_dmul+0x20c>
    ec18:	e1906081 	orrs	r6, r0, r1, lsl #1
    ec1c:	11926083 	orrsne	r6, r2, r3, lsl #1
    ec20:	1affffe5 	bne	ebbc <__aeabi_dmul+0x194>
    ec24:	e0211003 	eor	r1, r1, r3
    ec28:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    ec2c:	e3a00000 	mov	r0, #0	; 0x0
    ec30:	e8bd8070 	pop	{r4, r5, r6, pc}
    ec34:	e1906081 	orrs	r6, r0, r1, lsl #1
    ec38:	01a00002 	moveq	r0, r2
    ec3c:	01a01003 	moveq	r1, r3
    ec40:	11926083 	orrsne	r6, r2, r3, lsl #1
    ec44:	0a00000f 	beq	ec88 <__aeabi_dmul+0x260>
    ec48:	e134000c 	teq	r4, ip
    ec4c:	1a000001 	bne	ec58 <__aeabi_dmul+0x230>
    ec50:	e1906601 	orrs	r6, r0, r1, lsl #12
    ec54:	1a00000b 	bne	ec88 <__aeabi_dmul+0x260>
    ec58:	e135000c 	teq	r5, ip
    ec5c:	1a000003 	bne	ec70 <__aeabi_dmul+0x248>
    ec60:	e1926603 	orrs	r6, r2, r3, lsl #12
    ec64:	11a00002 	movne	r0, r2
    ec68:	11a01003 	movne	r1, r3
    ec6c:	1a000005 	bne	ec88 <__aeabi_dmul+0x260>
    ec70:	e0211003 	eor	r1, r1, r3
    ec74:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    ec78:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
    ec7c:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
    ec80:	e3a00000 	mov	r0, #0	; 0x0
    ec84:	e8bd8070 	pop	{r4, r5, r6, pc}
    ec88:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
    ec8c:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
    ec90:	e8bd8070 	pop	{r4, r5, r6, pc}

0000ec94 <__aeabi_ddiv>:
    ec94:	e92d4070 	push	{r4, r5, r6, lr}
    ec98:	e3a0c0ff 	mov	ip, #255	; 0xff
    ec9c:	e38ccc07 	orr	ip, ip, #1792	; 0x700
    eca0:	e01c4a21 	ands	r4, ip, r1, lsr #20
    eca4:	101c5a23 	andsne	r5, ip, r3, lsr #20
    eca8:	1134000c 	teqne	r4, ip
    ecac:	1135000c 	teqne	r5, ip
    ecb0:	0b00005c 	bleq	ee28 <__aeabi_ddiv+0x194>
    ecb4:	e0444005 	sub	r4, r4, r5
    ecb8:	e021e003 	eor	lr, r1, r3
    ecbc:	e1925603 	orrs	r5, r2, r3, lsl #12
    ecc0:	e1a01601 	lsl	r1, r1, #12
    ecc4:	0a00004b 	beq	edf8 <__aeabi_ddiv+0x164>
    ecc8:	e1a03603 	lsl	r3, r3, #12
    eccc:	e3a05201 	mov	r5, #268435456	; 0x10000000
    ecd0:	e1853223 	orr	r3, r5, r3, lsr #4
    ecd4:	e1833c22 	orr	r3, r3, r2, lsr #24
    ecd8:	e1a02402 	lsl	r2, r2, #8
    ecdc:	e1855221 	orr	r5, r5, r1, lsr #4
    ece0:	e1855c20 	orr	r5, r5, r0, lsr #24
    ece4:	e1a06400 	lsl	r6, r0, #8
    ece8:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
    ecec:	e1550003 	cmp	r5, r3
    ecf0:	01560002 	cmpeq	r6, r2
    ecf4:	e2a440fd 	adc	r4, r4, #253	; 0xfd
    ecf8:	e2844c03 	add	r4, r4, #768	; 0x300
    ecfc:	2a000001 	bcs	ed08 <__aeabi_ddiv+0x74>
    ed00:	e1b030a3 	lsrs	r3, r3, #1
    ed04:	e1a02062 	rrx	r2, r2
    ed08:	e0566002 	subs	r6, r6, r2
    ed0c:	e0c55003 	sbc	r5, r5, r3
    ed10:	e1b030a3 	lsrs	r3, r3, #1
    ed14:	e1a02062 	rrx	r2, r2
    ed18:	e3a00601 	mov	r0, #1048576	; 0x100000
    ed1c:	e3a0c702 	mov	ip, #524288	; 0x80000
    ed20:	e056e002 	subs	lr, r6, r2
    ed24:	e0d5e003 	sbcs	lr, r5, r3
    ed28:	20466002 	subcs	r6, r6, r2
    ed2c:	21a0500e 	movcs	r5, lr
    ed30:	2180000c 	orrcs	r0, r0, ip
    ed34:	e1b030a3 	lsrs	r3, r3, #1
    ed38:	e1a02062 	rrx	r2, r2
    ed3c:	e056e002 	subs	lr, r6, r2
    ed40:	e0d5e003 	sbcs	lr, r5, r3
    ed44:	20466002 	subcs	r6, r6, r2
    ed48:	21a0500e 	movcs	r5, lr
    ed4c:	218000ac 	orrcs	r0, r0, ip, lsr #1
    ed50:	e1b030a3 	lsrs	r3, r3, #1
    ed54:	e1a02062 	rrx	r2, r2
    ed58:	e056e002 	subs	lr, r6, r2
    ed5c:	e0d5e003 	sbcs	lr, r5, r3
    ed60:	20466002 	subcs	r6, r6, r2
    ed64:	21a0500e 	movcs	r5, lr
    ed68:	2180012c 	orrcs	r0, r0, ip, lsr #2
    ed6c:	e1b030a3 	lsrs	r3, r3, #1
    ed70:	e1a02062 	rrx	r2, r2
    ed74:	e056e002 	subs	lr, r6, r2
    ed78:	e0d5e003 	sbcs	lr, r5, r3
    ed7c:	20466002 	subcs	r6, r6, r2
    ed80:	21a0500e 	movcs	r5, lr
    ed84:	218001ac 	orrcs	r0, r0, ip, lsr #3
    ed88:	e195e006 	orrs	lr, r5, r6
    ed8c:	0a00000d 	beq	edc8 <__aeabi_ddiv+0x134>
    ed90:	e1a05205 	lsl	r5, r5, #4
    ed94:	e1855e26 	orr	r5, r5, r6, lsr #28
    ed98:	e1a06206 	lsl	r6, r6, #4
    ed9c:	e1a03183 	lsl	r3, r3, #3
    eda0:	e1833ea2 	orr	r3, r3, r2, lsr #29
    eda4:	e1a02182 	lsl	r2, r2, #3
    eda8:	e1b0c22c 	lsrs	ip, ip, #4
    edac:	1affffdb 	bne	ed20 <__aeabi_ddiv+0x8c>
    edb0:	e3110601 	tst	r1, #1048576	; 0x100000
    edb4:	1a000006 	bne	edd4 <__aeabi_ddiv+0x140>
    edb8:	e1811000 	orr	r1, r1, r0
    edbc:	e3a00000 	mov	r0, #0	; 0x0
    edc0:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
    edc4:	eaffffd5 	b	ed20 <__aeabi_ddiv+0x8c>
    edc8:	e3110601 	tst	r1, #1048576	; 0x100000
    edcc:	01811000 	orreq	r1, r1, r0
    edd0:	03a00000 	moveq	r0, #0	; 0x0
    edd4:	e254c0fd 	subs	ip, r4, #253	; 0xfd
    edd8:	835c0c07 	cmphi	ip, #1792	; 0x700
    eddc:	8affff4a 	bhi	eb0c <__aeabi_dmul+0xe4>
    ede0:	e055c003 	subs	ip, r5, r3
    ede4:	0056c002 	subseq	ip, r6, r2
    ede8:	01b0c0a0 	lsrseq	ip, r0, #1
    edec:	e2b00000 	adcs	r0, r0, #0	; 0x0
    edf0:	e0a11a04 	adc	r1, r1, r4, lsl #20
    edf4:	e8bd8070 	pop	{r4, r5, r6, pc}
    edf8:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
    edfc:	e18e1621 	orr	r1, lr, r1, lsr #12
    ee00:	e09440ac 	adds	r4, r4, ip, lsr #1
    ee04:	c074500c 	rsbsgt	r5, r4, ip
    ee08:	c1811a04 	orrgt	r1, r1, r4, lsl #20
    ee0c:	c8bd8070 	popgt	{r4, r5, r6, pc}
    ee10:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    ee14:	e3a0e000 	mov	lr, #0	; 0x0
    ee18:	e2544001 	subs	r4, r4, #1	; 0x1
    ee1c:	eaffff3a 	b	eb0c <__aeabi_dmul+0xe4>
    ee20:	e185e006 	orr	lr, r5, r6
    ee24:	eaffff38 	b	eb0c <__aeabi_dmul+0xe4>
    ee28:	e00c5a23 	and	r5, ip, r3, lsr #20
    ee2c:	e134000c 	teq	r4, ip
    ee30:	0135000c 	teqeq	r5, ip
    ee34:	0affff93 	beq	ec88 <__aeabi_dmul+0x260>
    ee38:	e134000c 	teq	r4, ip
    ee3c:	1a000006 	bne	ee5c <__aeabi_ddiv+0x1c8>
    ee40:	e1904601 	orrs	r4, r0, r1, lsl #12
    ee44:	1affff8f 	bne	ec88 <__aeabi_dmul+0x260>
    ee48:	e135000c 	teq	r5, ip
    ee4c:	1affff87 	bne	ec70 <__aeabi_dmul+0x248>
    ee50:	e1a00002 	mov	r0, r2
    ee54:	e1a01003 	mov	r1, r3
    ee58:	eaffff8a 	b	ec88 <__aeabi_dmul+0x260>
    ee5c:	e135000c 	teq	r5, ip
    ee60:	1a000004 	bne	ee78 <__aeabi_ddiv+0x1e4>
    ee64:	e1925603 	orrs	r5, r2, r3, lsl #12
    ee68:	0affff6d 	beq	ec24 <__aeabi_dmul+0x1fc>
    ee6c:	e1a00002 	mov	r0, r2
    ee70:	e1a01003 	mov	r1, r3
    ee74:	eaffff83 	b	ec88 <__aeabi_dmul+0x260>
    ee78:	e1906081 	orrs	r6, r0, r1, lsl #1
    ee7c:	11926083 	orrsne	r6, r2, r3, lsl #1
    ee80:	1affff4d 	bne	ebbc <__aeabi_dmul+0x194>
    ee84:	e1904081 	orrs	r4, r0, r1, lsl #1
    ee88:	1affff78 	bne	ec70 <__aeabi_dmul+0x248>
    ee8c:	e1925083 	orrs	r5, r2, r3, lsl #1
    ee90:	1affff63 	bne	ec24 <__aeabi_dmul+0x1fc>
    ee94:	eaffff7b 	b	ec88 <__aeabi_dmul+0x260>

0000ee98 <__aeabi_d2iz>:
    ee98:	e1a02081 	lsl	r2, r1, #1
    ee9c:	e2922602 	adds	r2, r2, #2097152	; 0x200000
    eea0:	2a00000c 	bcs	eed8 <__aeabi_d2iz+0x40>
    eea4:	5a000009 	bpl	eed0 <__aeabi_d2iz+0x38>
    eea8:	e3e03e3e 	mvn	r3, #992	; 0x3e0
    eeac:	e0532ac2 	subs	r2, r3, r2, asr #21
    eeb0:	9a00000a 	bls	eee0 <__aeabi_d2iz+0x48>
    eeb4:	e1a03581 	lsl	r3, r1, #11
    eeb8:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
    eebc:	e1833aa0 	orr	r3, r3, r0, lsr #21
    eec0:	e3110102 	tst	r1, #-2147483648	; 0x80000000
    eec4:	e1a00233 	lsr	r0, r3, r2
    eec8:	12600000 	rsbne	r0, r0, #0	; 0x0
    eecc:	e12fff1e 	bx	lr
    eed0:	e3a00000 	mov	r0, #0	; 0x0
    eed4:	e12fff1e 	bx	lr
    eed8:	e1900601 	orrs	r0, r0, r1, lsl #12
    eedc:	1a000002 	bne	eeec <__aeabi_d2iz+0x54>
    eee0:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
    eee4:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
    eee8:	e12fff1e 	bx	lr
    eeec:	e3a00000 	mov	r0, #0	; 0x0
    eef0:	e12fff1e 	bx	lr

0000eef4 <__div0>:
    eef4:	e92d4002 	push	{r1, lr}
    eef8:	e3a00008 	mov	r0, #8	; 0x8
    eefc:	ebfffcb1 	bl	e1c8 <raise>
    ef00:	e8bd8002 	pop	{r1, pc}
