#include <stdio.h>
#include <config.h>

#	ifndef CONFIG_SYS_IMAPX200_APLL
#		define CONFIG_SYS_IMAPX200_APLL		(0x80000041)
#	endif
#	ifndef CONFIG_SYS_IMAPX200_DCFG0
#		define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	endif
#	define spl_fclk	\
		(((CONFIG_SYS_CLK_FREQ / 1000) * (	\
		2*((CONFIG_SYS_IMAPX200_APLL & 0x7f) + 1))) / (		\
		(((CONFIG_SYS_IMAPX200_APLL >> 7) & 0x1f) + 1) * (	\
		1 << ((CONFIG_SYS_IMAPX200_APLL >> 12) & 0x3)))	* 1000)
#	define spl_hclk	\
		(spl_fclk / ((CONFIG_SYS_IMAPX200_DCFG0 >> 4) & 0xf))

#	define spl_pclk	\
		spl_hclk / (1 << ((CONFIG_SYS_IMAPX200_DCFG0 >> 16) & 0x3))

#	define spl_prescaler	\
		(spl_pclk / (8 * 100000) - 1)

int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		printf("RAM_TEXT = 0x%08x\n", CONFIG_SYS_PHY_UBOOT_BASE + 0x40);
		printf("RAM_TEXT_SPL = 0x%08x\n", CONFIG_SYS_PHY_SPL_BASE);
		return 0;
	}

	printf("uboot0 address: 0x%08x\n", CONFIG_SYS_PHY_SPL_BASE);
	printf("uboot1 address: 0x%08x\n", CONFIG_SYS_PHY_UBOOT_BASE + 0x40);
#if 0
	printf("CPU  freq. :  %d MHz\n",
	   spl_fclk / (CONFIG_SYS_IMAPX200_DCFG0 & 0xf) / 1000000);
	printf("HCLK freq. :  %d MHz\n", spl_hclk / 1000000);
	printf("PCLK freq. :  %d MHz\n", spl_pclk / 1000000);
#endif
	return 0;
}

