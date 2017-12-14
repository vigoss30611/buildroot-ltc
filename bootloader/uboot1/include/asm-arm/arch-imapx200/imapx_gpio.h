#ifndef __IMAPX_GPIO_H__
#define __IMAPX_GPIO_H__

#define GPADAT    	 (GPIO_BASE_REG_PA+0x00 )     /* Port A Data Register */
#define GPACON   	 (GPIO_BASE_REG_PA+0x04 )     /* Port A Data Direction Register */
#define GPAPUD     	 (GPIO_BASE_REG_PA+0x08 )     /* Port A Data Source */
#define GPBDAT    	 (GPIO_BASE_REG_PA+0x10 )     /* Port A Data Register */
#define GPBCON   	 (GPIO_BASE_REG_PA+0x14 )     /* Port A Data Direction Register */
#define GPBPUD     	 (GPIO_BASE_REG_PA+0x18 )     /* Port A Data Source */
#define GPCDAT    	 (GPIO_BASE_REG_PA+0x20 )     /* Port A Data Register */
#define GPCCON   	 (GPIO_BASE_REG_PA+0x24 )     /* Port A Data Direction Register */
#define GPCPUD     	 (GPIO_BASE_REG_PA+0x28 )     /* Port A Data Source */
#define GPDDAT    	 (GPIO_BASE_REG_PA+0x30 )     /* Port A Data Register */
#define GPDCON   	 (GPIO_BASE_REG_PA+0x34 )     /* Port A Data Direction Register */
#define GPDPUD     	 (GPIO_BASE_REG_PA+0x38 )     /* Port A Data Source */
#define GPEDAT    	 (GPIO_BASE_REG_PA+0x40 )     /* Port A Data Register */
#define GPECON   	 (GPIO_BASE_REG_PA+0x44 )     /* Port A Data Direction Register */
#define GPEPUD    	 (GPIO_BASE_REG_PA+0x48 )     /* Port A Data Source */
#define GPFDAT    	 (GPIO_BASE_REG_PA+0x50 )     /* Port A Data Register */
#define GPFCON   	 (GPIO_BASE_REG_PA+0x54 )     /* Port A Data Direction Register */
#define GPFPUD     	 (GPIO_BASE_REG_PA+0x58 )     /* Port A Data Source */
#define GPGDAT    	 (GPIO_BASE_REG_PA+0x60 )     /* Port A Data Register */
#define GPGCON   	 (GPIO_BASE_REG_PA+0x64 )     /* Port A Data Direction Register */
#define GPGPUD    	 (GPIO_BASE_REG_PA+0x68 )     /* Port A Data Source */
#define GPHDAT    	 (GPIO_BASE_REG_PA+0x70 )     /* Port A Data Register */
#define GPHCON   	 (GPIO_BASE_REG_PA+0x74 )     /* Port A Data Direction Register */
#define GPHPUD     	 (GPIO_BASE_REG_PA+0x78 )     /* Port A Data Source */
#define GPIDAT    	 (GPIO_BASE_REG_PA+0x80 )     /* Port A Data Register */
#define GPICON   	 (GPIO_BASE_REG_PA+0x84 )     /* Port A Data Direction Register */
#define GPIPUD     	 (GPIO_BASE_REG_PA+0x88 )     /* Port A Data Source */
#define GPJDAT    	 (GPIO_BASE_REG_PA+0x90 )     /* Port A Data Register */
#define GPJCON   	 (GPIO_BASE_REG_PA+0x94 )     /* Port A Data Direction Register */
#define GPJPUD     	 (GPIO_BASE_REG_PA+0x98 )     /* Port A Data Source */
#define GPKDAT   	 (GPIO_BASE_REG_PA+0xA0 )     /* Port A Data Register */
#define GPKCON   	 (GPIO_BASE_REG_PA+0xA4 )     /* Port A Data Direction Register */
#define GPKPUD     	 (GPIO_BASE_REG_PA+0xA8 )     /* Port A Data Source */
#define GPLDAT    	 (GPIO_BASE_REG_PA+0xB0 )     /* Port A Data Register */
#define GPLCON   	 (GPIO_BASE_REG_PA+0xB4 )     /* Port A Data Direction Register */
#define GPLPUD     	 (GPIO_BASE_REG_PA+0xB8 )     /* Port A Data Source */
#define GPMDAT    	 (GPIO_BASE_REG_PA+0xC0 )     /* Port A Data Register */
#define GPMCON    	 (GPIO_BASE_REG_PA+0xC4 )     /* Port A Data Direction Register */
#define GPMPUD     	 (GPIO_BASE_REG_PA+0xC8 )     /* Port A Data Source */
#define GPNDAT     	 (GPIO_BASE_REG_PA+0xD0 )     /* Port A Data Register */
#define GPNCON   	 (GPIO_BASE_REG_PA+0xD4 )     /* Port A Data Direction Register */
#define GPNPUD     	 (GPIO_BASE_REG_PA+0xD8 )     /* Port A Data Source */
#define GPODAT    	 (GPIO_BASE_REG_PA+0xE0 )     /* Port A Data Register */
#define GPOCON   	 (GPIO_BASE_REG_PA+0xE4 )     /* Port A Data Direction Register */
#define GPOPUD     	 (GPIO_BASE_REG_PA+0xE8 )     /* Port A Data Source */
#define GPPDAT    	 (GPIO_BASE_REG_PA+0xF0 )     /* Port A Data Register */
#define GPPCON   	 (GPIO_BASE_REG_PA+0xF4 )     /* Port A Data Direction Register */
#define GPPPUD     	 (GPIO_BASE_REG_PA+0xF8 )     /* Port A Data Source */
#define GPQDAT    	 (GPIO_BASE_REG_PA+0x100 )    /* Port A Data Register */
#define GPQCON   	 (GPIO_BASE_REG_PA+0x104 )    /* Port A Data Direction Register */
#define GPQPUD     	 (GPIO_BASE_REG_PA+0x108 )    /* Port A Data Source */


#define EINTCON			(GPIO_BASE_REG_PA+0x200 )     /* Port A Data Register */
#define EINTFLTCON0		(GPIO_BASE_REG_PA+0x204 )     /* Port A Data Direction Register */
#define EINTFLTCON1		(GPIO_BASE_REG_PA+0x208 )     /* Port A Data Source */
#define EINTGCON		(GPIO_BASE_REG_PA+0x210 )     /* Port A Data Register */
#define EINTGFLTCON0	(GPIO_BASE_REG_PA+0x214 )     /* Port A Data Direction Register */
#define EINTGFLTCON1	(GPIO_BASE_REG_PA+0x218 )     /* Port A Data Source */
#define EINTG1MASK		(GPIO_BASE_REG_PA+0x21C )     /* Port A Data Register */
#define EINTG2MASK		(GPIO_BASE_REG_PA+0x220 )     /* Port A Data Direction Register */
#define EINTG3MASK		(GPIO_BASE_REG_PA+0x224 )     /* Port A Data Source */
#define EINTG4MASK		(GPIO_BASE_REG_PA+0x228 )     /* Port A Data Register */
#define EINTG5MASK		(GPIO_BASE_REG_PA+0x22C )     /* Port A Data Direction Register */
#define EINTG6MASK		(GPIO_BASE_REG_PA+0x230 )     /* Port A Data Source */
#define EINTG1PEND		(GPIO_BASE_REG_PA+0x234 )     /* Port A Data Register */
#define EINTG2PEND		(GPIO_BASE_REG_PA+0x238 )     /* Port A Data Direction Register */
#define EINTG3PEND		(GPIO_BASE_REG_PA+0x23C )     /* Port A Data Source */
#define EINTG4PEND		(GPIO_BASE_REG_PA+0x240 )     /* Port A Data Register */
#define EINTG5PEND		(GPIO_BASE_REG_PA+0x244 )     /* Port A Data Direction Register */
#define EINTG6PEND		(GPIO_BASE_REG_PA+0x248 )     /* Port A Data Source */

#endif /*__IMAPX_GPIO_H__*/
