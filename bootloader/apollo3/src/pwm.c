#include <linux/types.h>
#include <preloader.h>
#include <asm-arm/io.h>
#include <isi.h>

#define IMAP_PWM_BASE 0x20200000
#define PWM_NUM 5

//1stpart,1:bule  2:green, 3:read;  2nd para, 0: on, 1: off, 2: slow flash, 3: quick flash
typedef enum {
	LIGHT_ON 	= 0,
	LIGHT_OFF 	= 1,
	LIGHT_SLOW 	= 2,
	LIGHT_QUICK 	= 3,
} light_t;

static int imap_pwm_start(int chan)
{
	unsigned long tcon;

	tcon = readl(IMAP_PWM_BASE+0x8);

	switch (chan) {
	case 1:
		tcon |= (1<<8);
		tcon &= ~(1<<9);
		tcon |= (1<<10);
		
		break;
		
	case 2:
		tcon |= (1<<12);		
		tcon &= ~(1<<13);
		tcon |= (1<<14);
		
		break;
		
	case 3:
		tcon |= (1<<16);
		tcon &= ~(1<<17);
		tcon |= (1<<18);
		
		break;
		
	}

	writel(tcon, (IMAP_PWM_BASE+0x8));
	
	return 0;
}

int pwm_out(unsigned char pwmNo, unsigned char status)
{
	unsigned long tcon;

	tcon = readl(IMAP_PWM_BASE+0x8);

//	spl_printf ( "pwm_out pwmNo %d status = %d\n", pwmNo, status);

	switch (pwmNo) {
	case 3:
		tcon &= ~(7 << 16);
		tcon |= (1<<19);
		tcon |= (1<<18);
		break;
		
	case 1:
		tcon &= ~(7 << 8);
		tcon |= (1<<11);
		tcon |= (1<<10);
		break;
		
	case 2:
		tcon &= ~(7 << 12);
		tcon |= (1<<15);
		tcon |= (1<<14);
		break;
		
	default:
		break;
	}

	writel(tcon, (IMAP_PWM_BASE+0x8));
//	writel(0x52f8, (IMAP_PWM_BASE+pwmNo*0x0C+0x0C));
//	writel(0x297c, (IMAP_PWM_BASE+pwmNo*0x0C+0x10));

	switch (status) {
		case 2:
			writel(0x5C00, (IMAP_PWM_BASE+pwmNo*0x0C+0x0C));	//57e0
			writel(0x2C00, (IMAP_PWM_BASE+pwmNo*0x0C+0x10));	//2dd0
			break;
		case 3:
			writel(0x18E4, (IMAP_PWM_BASE+pwmNo*0x0C+0x0C));
			writel(0x0C72, (IMAP_PWM_BASE+pwmNo*0x0C+0x10));
			break;
		case 0:
			writel(0x52f8, (IMAP_PWM_BASE+pwmNo*0x0C+0x0C));
			writel(0x0000, (IMAP_PWM_BASE+pwmNo*0x0C+0x10));
			break;
	}

	switch (pwmNo) {
	case 1:
		tcon |= (1<<9);
		break;
	case 2:
		tcon |= (1<<13);
		break;
	case 3:
		tcon |= (1<<17);
		break;
	default:
		break;
	}
	
	writel(tcon, (IMAP_PWM_BASE+0x8));

	imap_pwm_start(pwmNo);

	return 0;
}

static int pwm_clk_div_start(int pwmNo)
{
	uint32_t tmp, i = 0, j = 0;

	i |= (0xB);
	j |= (0xff);
	
//	tmp = readl(ioaddr + IMAP_TCFG1);
	tmp = readl(IMAP_PWM_BASE+0x4);
	 
	tmp &= ~0xffff;
	switch(pwmNo)
	{
		case 0:
			tmp |= (i << 0);
			break;
		case 1:
			tmp |= (i << 4);
			break;
		case 2:
			tmp |= (i << 8);
			break;
		case 3:
			tmp |= (i << 12);
			break;
	}
	//writel(tmp, ioaddr + IMAP_TCFG1);
	writel(tmp, (IMAP_PWM_BASE+0x4));
	
	tmp = readl(IMAP_PWM_BASE+0x0);
	tmp &= ~(0xffff);
	tmp |= (j | (j << 8));
	writel(tmp, (IMAP_PWM_BASE+0x0));

	return 0;
}

void pwm_init(int pwmNo)
{
	switch(pwmNo)
	{
		case 1:
			*((volatile unsigned int *)(0x2d009004+(108/8)*4)) &= 0<(108%8);//GPIO: function mode
			*((volatile unsigned int *)(0x2d009004+(120/8)*4)) |=1<<(120%8);//GPIO function
		  	*((volatile unsigned int *)(0x2d009004+(121/8)*4)) |=1<<(121%8);//GPIO function
			break;
		case 2:
			*((volatile unsigned int *)(0x2d009004+(120/8)*4)) &= 0<(120%8);//GPIO: function mode
			*((volatile unsigned int *)(0x2d009004+(108/8)*4)) |=1<<(108%8);//GPIO function
		  	*((volatile unsigned int *)(0x2d009004+(121/8)*4)) |=1<<(121%8);//GPIO function
			break;
		case 3:
			*((volatile unsigned int *)(0x2d009004+(121/8)*4)) &= 0<(121%8);//GPIO: function mode
			*((volatile unsigned int *)(0x2d009004+(120/8)*4)) |=1<<(120%8);//GPIO function
		  	*((volatile unsigned int *)(0x2d009004+(108/8)*4)) |=1<<(108%8);//GPIO function
			break;
	}
	
    
//  	spl_printf ( "pwm_clk_div_start pwmNo %d\n", pwmNo );
	pwm_clk_div_start(pwmNo);
//    spl_printf ( "pwm_clk_div_start over\n" );

	
 //   *(unsigned int *)(0x20F00000+num*0x40+0x8) =1;//direct:output
 //   *(unsigned int *)(0x20F00000+num*0x40+0x4) =1;//high 
}

void led_color(int boot_mode){

	char pwm[16];
	char color[16];
	int i, tmp_id, blue_id, green_id, red_id;

	for(i=0; i<PWM_NUM; i++){
		irf->sprintf(pwm, "led%d.pwm", i);
		if (item_exist(pwm)) {
			item_string(color, pwm, 0);
			tmp_id = item_integer(pwm, 1);
			if (irf->strncmp ( color, "blue", 4) == 0){
				blue_id = tmp_id;
			}else if (irf->strncmp ( color, "green", 5) == 0){
				green_id = tmp_id;
			}else if (irf->strncmp ( color, "red", 3) == 0){
				red_id = tmp_id;
			}else
				irf->printf("Attention: not know color!\n");
		}
	}

	if(boot_mode == BOOT_RECOVERY_OTA){
		pwm_init(green_id);
		pwm_out(green_id, LIGHT_ON); 
	}else if (boot_mode == BOOT_RECOVERY_IUS){
		pwm_init(red_id);
		pwm_out(red_id, LIGHT_SLOW);
	}else{
		pwm_init(green_id);
		pwm_out(green_id, LIGHT_SLOW);
	}
}
