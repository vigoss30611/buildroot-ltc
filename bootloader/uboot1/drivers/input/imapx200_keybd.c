/***********************************************************************
 *
 * (C) Copyright 2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 * All rights reserved.
 *
 * Keyboard driver
 *
 ***********************************************************************/

#include <common.h>
#include <asm/io.h>
#include <stdio_dev.h>
#include <keyboard.h>

#undef KBG_DEBUG

#ifdef KBG_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#define keypad_readl(off)       __raw_readl(off)
#define keypad_writel(off, v)   __raw_writel((v),(off))
#define MAX_MATRIX_KEY_ROWS     (18)
#define MAX_MATRIX_KEY_COLS     (8)
#define MAX_MATRIX_KEY_NUM      (MAX_MATRIX_KEY_ROWS * MAX_MATRIX_KEY_COLS)


#define	DEVNAME			"kbd"

#define	LED_SCR			0x01	/* scroll lock led */
#define	LED_CAP			0x04	/* caps lock led */
#define	LED_NUM			0x02	/* num lock led */

#define	KBD_BUFFER_LEN		0x20  /* size of the keyboardbuffer */

static void imapx200_kbd_put_queue(char data);

static volatile char kbd_buffer[KBD_BUFFER_LEN];
static volatile int in_pointer = 0;
static volatile int out_pointer = 0;

/* By warits, to avoid the warnnings 05/20/2010 */
#if 0
static unsigned char leds = 0;
static unsigned char num_lock = 0;
static unsigned char caps_lock = 0;
static unsigned char scroll_lock = 0;
static unsigned char shift = 0;
static unsigned char ctrl = 0;
static unsigned char alt = 0;
static unsigned char e0 = 0;
#endif
#if 0//for devlop board
static unsigned char matrix_keycode[] = {
	 '1',0x1b, 0x9, 'q', 'z',0xff, '`', 'a',0xff,0xff,0x10,0xff,0xff,0xff,0x70,0x10,	/* 0x00 - 0x0f */
	0x74,0xff,0xff,0x13,0xff,0xff,0x11,0xff,0xff,0xff, 0x8,0xff, 0xD,0xff,0x78,0xff,	/* 0x10 - 0x1f */
	0xff,0xff,0xff,0xff,0xff,0xff,0x72,0xff, '2','\\',0x14, 'w', 'x',0xff,0xff, 's',	/* 0x20 - 0x2f */
	 '4', 'g', 't', 'r', 'v', 'b', '5', 'f', '3',0x73,0x72, 'e', 'c',0xff,0xff, 'd',	/* 0x30 - 0x3f */
	 '7', 'h', 'y', 'u', 'm', 'n', '6', 'j',0x79,0xff,0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x40 - 0x4f */
	 '8',0x75, ']', 'i', ',',0xff, '=', 'k', '9',0xff,0x76, 'o', '.',0xff,0x77, 'l',	/* 0x50 - 0x5F */
	 '0','\'', '[', 'p','\\', '/', '-', ';',0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x60 - 0x6F */
	0xff,0xff,0xff,0xff,0xff,0xff,0x2D,0xff,0xff,0x28,0xff,0xff,0x25,0x27,0xff,0x26,	/* 0x70 - 0x7F */
	0xff, ' ',0xff,0xff,0xff,0xff,0x2E,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff		/* 0x80 - 0x8F */
	};
#endif

//for product board
static unsigned char matrix_keycode[] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x12,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x00 - 0x0f */
	0xff,0xff, 'z',0x14, 'a', 's',0x11,0xff,0xff, 'c', 'x', 'd', 'q', 0x9, '1',0xff,	/* 0x10 - 0x1f */
	0xff, 'f', 'e', 'w', '2', '3',0x70,0x1b, 'v', 'g', 't', 'r', '4', '5',0x72,0x71,	/* 0x20 - 0x2f */
	0xff, 'h', 'u', 'y', '6', '7',0x74,0x73,0xff, 'k', 'j', 'i', '8', '9',0x76,0x75,	/* 0x30 - 0x3f */
	0xff, 'l', 'p', 'o', '0',0xff,0x78,0x77, 'm', ';', '[',0xff, '-',0xff,0x7a,0x79,	/* 0x40 - 0x4f */
	 'b', ',','\'',0xff,0xff, '=',0x90,0x7b,0xff,0xff, 'n', '.',0xff,0xff,0x13,0x2d,	/* 0x50 - 0x5f */
	 ' ', '/',0xff,0xff, ']',0xff,0x25,0x2a,0x10,0xff,0xff,0xff,0xff,0xff,0xff,0x10,	/* 0x60 - 0x6f */
	0x27,0x28,0xff,0x26, 0xd,'\\', 0x8,0x2e,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x11,	/* 0x70 - 0x7f */
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x80 - 0x8f */
	};

static void imapx200_keybd_scan_matrix(void)
{
	int iRow, iCol;
	uint32_t kbRowData_old[MAX_MATRIX_KEY_ROWS];
	uint32_t kbRowData[MAX_MATRIX_KEY_ROWS];

	memset(kbRowData, 0, sizeof(kbRowData));
	kbRowData[0] = keypad_readl(KBROWD0) & 0xff;
	kbRowData[1] = (keypad_readl(KBROWD0) & (0xff<<8))>>8;
	kbRowData[2] = (keypad_readl(KBROWD0) & (0xff<<16))>>16;
	kbRowData[3] = (keypad_readl(KBROWD0) & (0xff<<24))>>24;
	kbRowData[4] = keypad_readl(KBROWD1) & 0xff;
	kbRowData[5] = (keypad_readl(KBROWD1) & (0xff<<8))>>8;
	kbRowData[6] = (keypad_readl(KBROWD1) & (0xff<<16))>>16;
	kbRowData[7] = (keypad_readl(KBROWD1) & (0xff<<24))>>24;
	kbRowData[8] = keypad_readl(KBROWD2) & 0xff;
	kbRowData[9] = (keypad_readl(KBROWD2) & (0xff<<8))>>8;
	kbRowData[10] = (keypad_readl(KBROWD2) & (0xff<<16))>>16;
	kbRowData[11] = (keypad_readl(KBROWD2) & (0xff<<24))>>24;
	kbRowData[12] = keypad_readl(KBROWD3) & 0xff;
	kbRowData[13] = (keypad_readl(KBROWD3) & (0xff<<8))>>8;
	kbRowData[14] = (keypad_readl(KBROWD3) & (0xff<<16))>>16;
	kbRowData[15] = (keypad_readl(KBROWD3) & (0xff<<24))>>24;
	kbRowData[16] = keypad_readl(KBROWD4) & 0xff;
	kbRowData[17] = (keypad_readl(KBROWD4) & (0xff<<8))>>8;

        for (iRow = 0; iRow < MAX_MATRIX_KEY_ROWS; iRow++)
        {
		uint32_t bits_changed;

		bits_changed = kbRowData_old[iRow] ^ kbRowData[iRow];
		if (bits_changed == 0)
			continue;

		for (iCol = 0; iCol < MAX_MATRIX_KEY_COLS ; iCol++)
		{
			if ((bits_changed & (1 << iCol)) == 0)
				continue;
			if (!(kbRowData[iRow] & (1 << iCol)))
				imapx200_kbd_put_queue(matrix_keycode[(iRow << 3) + iCol]);
		}
	}
	memcpy(kbRowData_old, kbRowData, sizeof(kbRowData));
}

static unsigned char handle_kbd_event(void)
{
	unsigned int status = readl(KBINT);
	unsigned int work = 10;

	while ((--work > 0) && (status & KBDCNT_DRDYINT))
	{
		keypad_writel(KBINT , 0x1ffff);
		imapx200_keybd_scan_matrix();
	}
	if (!work)
		PRINTF("pc_keyb: controller jammed (0x%02X).\n", status);
	return status;
}

/******************************************************************
 * Queue handling
 ******************************************************************/

/* puts character in the queue and sets up the in and out pointer */
static void imapx200_kbd_put_queue(char data)
{
	if((in_pointer+1)==KBD_BUFFER_LEN) {
		if(out_pointer==0) {
			return; /* buffer full */
		} else{
			in_pointer=0;
		}
	} else {
		if((in_pointer+1)==out_pointer)
			return; /* buffer full */
		in_pointer++;
	}
	kbd_buffer[in_pointer]=data;
	return;
}

/* test if a character is in the queue */
static int imapx200_kbd_testc(void)
{
	handle_kbd_event();
	if(in_pointer==out_pointer)
		return(0); /* no data */
	else
		return(1);
}

/* gets the character from the queue */
static int imapx200_kbd_getc(void)
{
	char c;
	while(in_pointer==out_pointer) 
	{
		handle_kbd_event();
	}
	if((out_pointer+1)==KBD_BUFFER_LEN)
		out_pointer=0;
	else
		out_pointer++;
	c=kbd_buffer[out_pointer];
	return (int)c;

}

/* Simple translation table for the keys */
#if 0
static unsigned char kbd_plain_xlate[] = {
	0xff,0x1b, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=','\b','\t',	/* 0x00 - 0x0f */
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']','\r',0xff, 'a', 's',	/* 0x10 - 0x1f */
	 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',0xff,'\\', 'z', 'x', 'c', 'v',	/* 0x20 - 0x2f */
	 'b', 'n', 'm', ',', '.', '/',0xff,0xff,0xff, ' ',0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x30 - 0x3f */
	0xff,0xff,0xff,0xff,0xff,0xff,0xff, '7', '8', '9', '-', '4', '5', '6', '+', '1',	/* 0x40 - 0x4f */
	 '2', '3', '0', '.',0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 0x50 - 0x5F */
	'\r',0xff,0xff
	};

static unsigned char kbd_shift_xlate[] = {
	0xff,0x1b, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+','\b','\t',	/* 0x00 - 0x0f */
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}','\r',0xff, 'A', 'S',	/* 0x10 - 0x1f */
	 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',0xff, '|', 'Z', 'X', 'C', 'V',	/* 0x20 - 0x2f */
	 'B', 'N', 'M', '<', '>', '?',0xff,0xff,0xff, ' ',0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x30 - 0x3f */
	0xff,0xff,0xff,0xff,0xff,0xff,0xff, '7', '8', '9', '-', '4', '5', '6', '+', '1',	/* 0x40 - 0x4f */
	 '2', '3', '0', '.',0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 0x50 - 0x5F */
	'\r',0xff,0xff
	};

static unsigned char kbd_ctrl_xlate[] = {
	0xff,0x1b, '1',0x00, '3', '4', '5',0x1E, '7', '8', '9', '0',0x1F, '=','\b','\t',	/* 0x00 - 0x0f */
	0x11,0x17,0x05,0x12,0x14,0x18,0x15,0x09,0x0f,0x10,0x1b,0x1d,'\n',0xff,0x01,0x13,	/* 0x10 - 0x1f */
	0x04,0x06,0x08,0x09,0x0a,0x0b,0x0c, ';','\'', '~',0x00,0x1c,0x1a,0x18,0x03,0x16,	/* 0x20 - 0x2f */
	0x02,0x0e,0x0d, '<', '>', '?',0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,	/* 0x30 - 0x3f */
	0xff,0xff,0xff,0xff,0xff,0xff,0xff, '7', '8', '9', '-', '4', '5', '6', '+', '1',	/* 0x40 - 0x4f */
	 '2', '3', '0', '.',0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 0x50 - 0x5F */
	'\r',0xff,0xff
	};

void handle_scancode(unsigned char scancode)
{
	unsigned char keycode;

	/*  Convert scancode to keycode */
	PRINTF("scancode %x\n",scancode);
	if(scancode==0xe0) {
		e0=1; /* special charakters */
		return;
	}
	if(e0==1) {
		e0=0; /* delete flag */
		if(!(	((scancode&0x7F)==0x38)|| /* the right ctrl key */
					((scancode&0x7F)==0x1D)|| /* the right alt key */
					((scancode&0x7F)==0x35)||	/* the right '/' key */
					((scancode&0x7F)==0x1C) ))  /* the right enter key */
			/* we swallow unknown e0 codes */
			return;
	}
	/* special cntrl keys */
	switch(scancode) {
	case 0x2A:
	case 0x36: /* shift pressed */
		shift=1;
		return; /* do nothing else */
	case 0xAA:
	case 0xB6: /* shift released */
		shift=0;
		return; /* do nothing else */
	case 0x38: /* alt pressed */
		alt=1;
		return; /* do nothing else */
	case 0xB8: /* alt released */
		alt=0;
		return; /* do nothing else */
	case 0x1d: /* ctrl pressed */
		ctrl=1;
		return; /* do nothing else */
	case 0x9d: /* ctrl released */
		ctrl=0;
		return; /* do nothing else */
	case 0x46: /* scrollock pressed */
		scroll_lock=~scroll_lock;
		if(scroll_lock==0)
			leds&=~LED_SCR; /* switch LED off */
		else
			leds|=LED_SCR; /* switch on LED */
		pckbd_leds(leds);
		return; /* do nothing else */
	case 0x3A: /* capslock pressed */
		caps_lock=~caps_lock;
		if(caps_lock==0)
			leds&=~LED_CAP; /* switch caps_lock off */
		else
			leds|=LED_CAP; /* switch on LED */
		pckbd_leds(leds);
		return;
	case 0x45: /* numlock pressed */
		num_lock=~num_lock;
		if(num_lock==0)
			leds&=~LED_NUM; /* switch LED off */
		else
			leds|=LED_NUM;  /* switch on LED */
		pckbd_leds(leds);
		return;
	case 0xC6: /* scroll lock released */
	case 0xC5: /* num lock released */
	case 0xBA: /* caps lock released */
		return; /* just swallow */
	}
#if 1
	if((scancode&0x80)==0x80) /* key released */
		return;
#else
	if((scancode&0x80)==0x00) /* key pressed */
		return;
	scancode &= ~0x80;
#endif
	/* now, decide which table we need */
	if(scancode > (sizeof(kbd_plain_xlate)/sizeof(kbd_plain_xlate[0]))) { /* scancode not in list */
		PRINTF("unkown scancode %X\n",scancode);
		return; /* swallow it */
	}
	/* setup plain code first */
	keycode=kbd_plain_xlate[scancode];
	if(caps_lock==1) { /* caps_lock is pressed, overwrite plain code */
		if(scancode > (sizeof(kbd_shift_xlate)/sizeof(kbd_shift_xlate[0]))) { /* scancode not in list */
			PRINTF("unkown caps-locked scancode %X\n",scancode);
			return; /* swallow it */
		}
		keycode=kbd_shift_xlate[scancode];
		if(keycode<'A') { /* we only want the alphas capital */
			keycode=kbd_plain_xlate[scancode];
		}
	}
	if(shift==1) { /* shift overwrites caps_lock */
		if(scancode > (sizeof(kbd_shift_xlate)/sizeof(kbd_shift_xlate[0]))) { /* scancode not in list */
			PRINTF("unkown shifted scancode %X\n",scancode);
			return; /* swallow it */
		}
		keycode=kbd_shift_xlate[scancode];
	}
	if(ctrl==1) { /* ctrl overwrites caps_lock and shift */
		if(scancode > (sizeof(kbd_ctrl_xlate)/sizeof(kbd_ctrl_xlate[0]))) { /* scancode not in list */
			PRINTF("unkown ctrl scancode %X\n",scancode);
			return; /* swallow it */
		}
		keycode=kbd_ctrl_xlate[scancode];
	}
	/* check if valid keycode */
	if(keycode==0xff) {
		PRINTF("unkown scancode %X\n",scancode);
		return; /* swallow unknown codes */
	}

	imapx200_kbd_put_queue(keycode);
	PRINTF("%x\n",keycode);
}
#endif
/******************************************************************
 * Init
 ******************************************************************/
int imapx200_kbd_init_hw(void)
{
        int gphcon, gpicon, gpjcon;
	
        PRINTF("[drivers/input/imapx200_uboot_keybd.c]:imapx200_kbd_init_hw!\r\n");

	//config the GPIO port for keyboard 
	gphcon = readl(GPHCON);
	gphcon |= 0xff;
	writel(gphcon , GPHCON);
	
	gpicon = readl(GPICON);
	gpicon |= 0xfffffff;
	writel(gpicon , GPICON);
	
	gpjcon = readl(GPJCON);
	gpjcon |= 0xffff;
	writel(gpjcon , GPJCON);

        /* enable matrix keys with automatic scan */
	keypad_writel(KBCKD , 1024);
	keypad_writel(KBDCNT , 100);
	keypad_writel(KBCOLD , 0);

	keypad_writel(KBRPTC , 1024*16);
	keypad_writel(KBCON , (KBCON_RPTEN |KBCON_FCEN| KBCON_DFEN | KBCON_DRDYINTEN));
	keypad_writel(KBCOEN , (KBCOEN_COLNUM | KBCOEN_COLOEN));

	return 1;
}

#ifdef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console (void);
#define OVERWRITE_CONSOLE overwrite_console ()
#else
#define OVERWRITE_CONSOLE 0
#endif /* CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE */

int imapx200_kbd_init (void)
{
	int error;
	struct stdio_dev kbddev ;
	char *stdinname  = getenv ("stdin");

	PRINTF("[drivers/input/imapx200_uboot_keybd.c]:imapx200_kbd_init!\r\n");

	if(imapx200_kbd_init_hw()==-1)
		return -1;
	memset (&kbddev, 0, sizeof(kbddev));
	strcpy(kbddev.name, DEVNAME);
	kbddev.flags =  DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	kbddev.putc = NULL ;
	kbddev.puts = NULL ;
	kbddev.getc = imapx200_kbd_getc ;
	kbddev.tstc = imapx200_kbd_testc ;

	error = stdio_register (&kbddev);
	if(error==0) {
		/* check if this is the standard input device */
		if(strcmp(stdinname,DEVNAME)==0) {
			/* reassign the console */
			if(OVERWRITE_CONSOLE) {
				return 1;
			}
			error=console_assign(stdin,DEVNAME);
			if(error==0)
				return 1;
			else
				return error;
		}
		return 1;
	}
	return error;
}
