/***************************************************
** arch/arm/mach-q3f/devices.c
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** Use of Infotm's code is governed by terms and conditions
** stated in the accompanying licensing statement.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Author:
**     Raymond Wang   <raymond.wang@infotmic.com.cn>
**
** Revision History:
**     1.2  25/11/2009    Raymond Wang
*******************************************************/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/ata_platform.h>
#include <linux/mmc/dw_mmc.h>
#include <linux/mmc/host.h>
#include <linux/amba/bus.h>
#include <linux/amba/pl022.h>
#include <linux/pmu.h>
#include <linux/pwm_backlight.h>
#include <linux/leds_pwm.h>
#include <linux/adc.h>
#include <linux/pwm.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <asm/setup.h>
#include <asm/irq.h>
#include <mach/audio.h>
#include <linux/leds.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/ads7846.h>
#include <linux/platform_data/camera-imapx.h>
#include <linux/platform_data/spi-imapx.h>
#include <mach/hardware.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include <mach/hw_cfg.h>
#include <linux/amba/pl330.h>
#include <mach/imap_pl330.h>
#include <linux/amba/serial.h>
#include <linux/i2c.h>
//#include <linux/regulator/machine.h>
//#include <linux/regulator/fixed.h>
//#include <mach/ricoh618.h>
//#include <mach/ricoh618-regulator.h>
//#include <mach/ricoh618_battery.h>
#include <mach/ip6303.h>
#include <mach/ip6208.h>
#include <mach/ip6205.h>
#include <mach/ec610.h>

#include "core.h"
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <linux/display_device.h>

#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
#include <linux/usb/android_composite.h>
#endif

#if 1
static inline bool pl011_filter(struct dma_chan *chan, void *param)
{
	u8 *peri_id;
	peri_id = chan->private;
	return *peri_id == (unsigned)param;
}

struct amba_pl011_data imap_pl011_data1 = {
	.dma_tx_param = (void *)IMAPX_UART1_TX,
	.dma_rx_param = (void *)IMAPX_UART1_RX,
	.dma_filter = pl011_filter,
};

struct amba_pl011_data imap_pl011_data3 = {
	.dma_tx_param = (void *)IMAPX_UART3_TX,
	.dma_rx_param = (void *)IMAPX_UART3_RX,
	.dma_filter = pl011_filter,
};

#define imap_uart1_data (&imap_pl011_data1)
#else
#define imap_uart1_data NULL
#endif

/* amba devices define */
struct amba_device imap_uart0_device = {
	.dev = {
		.init_name = "imap-uart.0",
		.platform_data = NULL,
		},
	.res = {
		.start = IMAP_UART0_BASE,
		.end = IMAP_UART0_BASE + IMAP_UART0_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_UART0_ID},
	.periphid = 0x00041011,
};

/*struct amba_device imap_uart1_device = {
	.dev = {
		.init_name = "imap-uart.1",
		.platform_data = NULL,
		},
	.res = {
		.start = IMAP_UART1_BASE,
		.end = IMAP_UART1_BASE + IMAP_UART1_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_UART1_ID},
	.periphid = 0x00041011,
};*/

struct amba_device imap_uart3_device = {
	.dev = {
		.init_name = "imap-uart.3",
		.platform_data = NULL,
		},
	.res = {
		.start = IMAP_UART3_BASE,
		.end = IMAP_UART3_BASE + IMAP_UART3_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_UART3_ID},
	.periphid = 0x00041011,
};

static u8 imap_pl330_peri[] = {
	IMAPX_PCM0_TX,
	IMAPX_PCM1_TX,
	IMAPX_PCM0_RX,
	IMAPX_PCM1_RX,
	IMAPX_SSP0_TX,
	IMAPX_SSP1_TX,
	IMAPX_SSP0_RX,
	IMAPX_SSP1_RX,
	IMAPX_AC97_TX,
	IMAPX_AC97_RX,
	IMAPX_UART0_TX,
	IMAPX_UART1_TX,
	IMAPX_UART2_TX,
	IMAPX_UART3_TX,
	IMAPX_UART0_RX,
	IMAPX_UART1_RX,
	IMAPX_UART2_RX,
	IMAPX_UART3_RX,
	IMAPX_I2S_SLAVE_TX,
	IMAPX_I2S_SLAVE_RX,
	IMAPX_I2S_MASTER_TX,
	IMAPX_I2S_MASTER_RX,
	IMAPX_TOUCHSCREEN_RX,

};

static struct dma_pl330_platdata imap_pl330_platdata = {
	.nr_valid_peri = 23,
	.peri_id = imap_pl330_peri,
	.mcbuf_sz = 512,
};

struct amba_device imap_dma_device = {
	.dev = {
		.init_name = "dma-pl330",
		.platform_data = &imap_pl330_platdata,
		.coherent_dma_mask = ~0,
		},
	.res = {
		.start = IMAP_GDMA_BASE,
		.end = IMAP_GDMA_BASE + IMAP_GDMA_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_DMA0_ID, GIC_DMA1_ID,
		GIC_DMA2_ID, GIC_DMA3_ID,
		GIC_DMA4_ID, GIC_DMA5_ID,
		GIC_DMA6_ID, GIC_DMA7_ID,
		GIC_DMA8_ID, GIC_DMA9_ID,
		GIC_DMA10_ID, GIC_DMA11_ID,
		GIC_DMABT_ID, NO_IRQ},
	.periphid = 0x00041330,

};

static struct resource imap_gpio_source[] = {
	[0] = {
	       .start = IMAP_GPIO_BASE,
	       .end = IMAP_GPIO_BASE + IMAP_GPIO_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_GPG_ID,
	       .end = GIC_GPG_ID,
	       .flags = IORESOURCE_IRQ,
	       },
	[2] = {
	       .start = GIC_GP1_ID,
	       .end = GIC_GP1_ID,
	       .flags = IORESOURCE_IRQ,
	       },
	[3] = {
	       .start = GIC_GP2_ID,
	       .end = GIC_GP2_ID,
	       .flags = IORESOURCE_IRQ,
	       },

};

struct platform_device imap_gpio_device = {
	.name = "imap-gpio",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_gpio_source),
	.resource = imap_gpio_source,
	.dev = {
		.platform_data = NULL,
	}
};


struct platform_device imap_gpio_pwrkey_device = {
	.name = "q3f_gpio-pwrkey",
	.id = -1,
	.dev = {
		.platform_data = NULL,
	}
};

/* gpio chip */
static struct resource imap_gpiochip_source[] = {
	[0] = {
		.start = IMAP_GPIO_BASE,
		.end = IMAP_GPIO_BASE + IMAP_GPIO_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
};

struct platform_device imap_gpiochip_device = {
	.name = "imap-gpiochip",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_gpiochip_source),
	.resource = imap_gpiochip_source,
	.dev = {
		.platform_data = NULL,
	}
};

/* sdmmc_0_dev */
static int imap_mmc0_get_bus_wd(u32 slot_id)
{
	return 8;
}

static int imap_mmc0_init(u32 slot_id, irq_handler_t handler, void *data)
{
	int ret;

	ret = imapx_pad_init("sd0");
	module_power_on(SYSMGR_MMC0_BASE);
	return 0;
}

static struct dw_mci_board imap_mmc0_platdata = {
	.num_slots = 1,
	.quirks = DW_MCI_QUIRK_BROKEN_CARD_DETECTION,
	.bus_hz = 100 * 1000 * 1000,
	.detect_delay_ms = 2000,
	.init = imap_mmc0_init,
	.get_bus_wd = imap_mmc0_get_bus_wd,
};

static struct resource imap_mmc0_resource[] = {
	[0] = {
	       .start = IMAP_MMC0_BASE,
	       .end = IMAP_MMC0_BASE + IMAP_MMC0_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_MMC0_ID,
	       .end = GIC_MMC0_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

static u64 sdmmc0_dma_mask = DMA_BIT_MASK(32);
struct platform_device imap_mmc0_device = {
	.name = "imap-mmc1",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_mmc0_resource),
	.resource = imap_mmc0_resource,
	.dev = {
		.platform_data = &imap_mmc0_platdata,
		.dma_mask = &sdmmc0_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),

		},

};

/* sdmmc_1_dev */
static int imap_mmc1_get_bus_wd(u32 slot_id)
{
	return 4;
}

static int imap_mmc1_init(u32 slot_id, irq_handler_t handler, void *data)
{
	imapx_pad_init("sd1");
	module_power_on(SYSMGR_MMC1_BASE);
	return 0;
}

static struct dw_mci_board imap_mmc1_platdata = {
	.num_slots = 1,
	.quirks = DW_MCI_QUIRK_HIGHSPEED,
	.bus_hz = 50 * 1000 * 1000,
	.detect_delay_ms = 500,
	.init = imap_mmc1_init,
	.get_bus_wd = imap_mmc1_get_bus_wd,
	.R_delayline_val = -1,
	.W_delayline_val = -1,
};

static struct resource imap_mmc1_resource[] = {
	[0] = {
	       .start = IMAP_MMC1_BASE,
	       .end = IMAP_MMC1_BASE + IMAP_MMC1_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_MMC1_ID,
	       .end = GIC_MMC1_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

static u64 sdmmc1_dma_mask = DMA_BIT_MASK(32);
struct platform_device imap_mmc1_device = {
	.name = "imap-mmc1",
	.id = 1,
	.num_resources = ARRAY_SIZE(imap_mmc1_resource),
	.resource = imap_mmc1_resource,
	.dev = {
		.platform_data = &imap_mmc1_platdata,
		.dma_mask = &sdmmc1_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		},
};

/* sdmmc_2_dev */
static int imap_mmc2_get_bus_wd(u32 slot_id)
{
	return 4;
}

static int imap_mmc2_init(u32 slot_id, irq_handler_t handler, void *data)
{
	imapx_pad_init("sd2");
	module_power_on(SYSMGR_MMC2_BASE);
#if 0
	imapx_pad_set_mode(MODE_GPIO, 1, 153);
	imapx_pad_set_dir(DIRECTION_OUTPUT, 1, 153);
	imapx_pad_set_outdat(OUTPUT_1, 1, 153);
#endif
	return 0;
}

static struct dw_mci_board imap_mmc2_platdata = {
	.num_slots = 1,
	.wifi_en = 1,	//Eason
	//.quirks = DW_MCI_QUIRK_BROKEN_CARD_DETECTION,	//Eason
	.quirks = DW_MCI_QUIRK_RETRY_DELAY|DW_MCI_QUIRK_IDMAC_DTO|DW_MCI_SLOT_QUIRK_NO_WRITE_PROTECT|DW_MCI_QUIRK_BROKEN_CARD_DETECTION,
	.caps = MMC_CAP_SDIO_IRQ,
	.bus_hz = 50 * 1000 * 1000,//12 * 1000 * 1000,//50 * 1000 * 1000,	//Eason
	.detect_delay_ms = 500,
	.init = imap_mmc2_init,
	.get_bus_wd = imap_mmc2_get_bus_wd,
	.R_delayline_val = -1,
	.W_delayline_val = -1,
};

static struct resource imap_mmc2_resource[] = {
	[0] = {
	       .start = IMAP_MMC2_BASE,
	       .end = IMAP_MMC2_BASE + IMAP_MMC2_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_MMC2_ID,
	       .end = GIC_MMC2_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

static u64 sdmmc2_dma_mask = DMA_BIT_MASK(32);
struct platform_device imap_mmc2_device = {
	.name = "imap-mmc1",	//Eason
	.id = 2,
	.num_resources = ARRAY_SIZE(imap_mmc2_resource),
	.resource = imap_mmc2_resource,
	.dev = {
		.platform_data = &imap_mmc2_platdata,
		.dma_mask = &sdmmc2_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),

		},
};

/************ PMU ************/
static struct resource imap_pmu_resource[] = {
	[0] = {
	       .start = GIC_CPUPMU0_ID,
	       .end = GIC_CPUPMU0_ID,
	       .flags = IORESOURCE_IRQ,
	       },
};

struct platform_device imap_pmu_device = {
	.name = "arm-pmu",
/*
 * [KP-peter] ARM_PMU_DEVICE_CPU  not defined by 3.10
 */
	.id = 0,
	.resource = imap_pmu_resource,
	.num_resources = ARRAY_SIZE(imap_pmu_resource),
};


/* usb ohci controller device */
static u64 ohci_dma_mask = 0xffffffffUL;
static struct resource imap_ohci_resource[] = {
	[0] = {
	       .start = IMAP_OHCI_BASE,
	       .end = IMAP_OHCI_BASE + IMAP_OHCI_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_OHCI_ID,
	       .end = GIC_OHCI_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_ohci_device = {
	.name = "imap-ohci",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_ohci_resource),
	.resource = imap_ohci_resource,
	.dev = {
		.dma_mask = &ohci_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		}
};

/* usb ehci controller device */
static u64 ehci_dma_mask = 0xffffffffUL;
static struct resource imap_ehci_resource[] = {
	[0] = {
	       .start = IMAP_EHCI_BASE,
	       .end = IMAP_EHCI_BASE + IMAP_EHCI_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_EHCI_ID,
	       .end = GIC_EHCI_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_ehci_device = {
	.name = "imap-ehci",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_ehci_resource),
	.resource = imap_ehci_resource,
	.dev = {
		.dma_mask = &ehci_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		}

};

/* usb otg controller device */
static u64 otg_dma_mask = 0xffffffffUL;
static struct resource imap_otg_resource[] = {
	[0] = {
	       .start = IMAP_OTG_BASE,
	       .end = IMAP_OTG_BASE + IMAP_OTG_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_OTG_ID,
	       .end = GIC_OTG_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_otg_device = {
	.name = "dwc_otg",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_otg_resource),
	.resource = imap_otg_resource,
	.dev = {
		.dma_mask = &otg_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		}
};

#ifdef CONFIG_USB_G_ANDROID

struct platform_device android_storage_device = {
	.name = "android_usb",
	.id = -1,
	.dev = {
		.platform_data = NULL,
	},
};

#endif
/***********************************************************/

static struct resource imap_sensor_resource[] = {

};

struct platform_device imap_sensor_device = {
	.name = "imap-sensor",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_sensor_resource),
	.resource = imap_sensor_resource,
};

static struct resource imap_backlight_resource[] = {

};

static struct platform_pwm_backlight_data imap_backlight_data = {
	.pwm_id = 0,
	.max_brightness = 255,
	.dft_brightness = 125,
	.lth_brightness = 0,
	.pwm_period_ns = 99040,
	.polarity = 0,
};

struct platform_device imap_backlight_device = {
	.name = "lcd-backlight",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_backlight_resource),
	.resource = imap_backlight_resource,
	.dev = {
		.platform_data = &imap_backlight_data,
	},
};

/*********MAC*************/
static struct resource imap_mac_resource[] = {
	[0] = {
	       .start = IMAP_MAC_BASE,
	       .end = IMAP_MAC_BASE + IMAP_MAC_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_MAC_ID,
	       .end = GIC_MAC_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_mac_device = {
	.name = "imap-mac",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_mac_resource),
	.resource = imap_mac_resource,
	.dev = {
		.dma_mask = &imap_mac_device.dev.coherent_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		}
};

struct platform_device imap_rda5868_device = {
	.name = "rda5868-bt",
	.id = -1,
};

/***************Memory Alloc******************/
static struct resource imap_memalloc_resource[] = {
	[0] = {
	       .start = 0,
	       .end = 0,
	       .flags = IORESOURCE_MEM,
	       },
};

struct platform_device imap_memalloc_device = {
	.name = "imap-memalloc",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_memalloc_resource),
	.resource = imap_memalloc_resource,
};

/********************* crypto ********************************/
static struct resource imap_crypto_resource[] = {
	[0] = {
	       .start = IMAP_CRYPTO_BASE,
	       .end = IMAP_CRYPTO_BASE + IMAP_CRYPTO_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_CRYPTO_ID,
	       .end = GIC_CRYPTO_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_crypto_device = {
	.name = "imap-crypto",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_crypto_resource),
	.resource = imap_crypto_resource,
};

/*********PWM**************/
static struct resource imap_pwm_resource[] = {
	[0] = {
	       .start = IMAP_PWM_BASE,
	       .end = IMAP_PWM_BASE + IMAP_PWM_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
};

static struct imapx_pwm_platform_data imapx_pwm_pdata = {
	.npwm = 4,
};

struct platform_device imap_pwm_device = {
	.name = "imap-pwm",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_pwm_resource),
	.resource = imap_pwm_resource,
	.dev = {
		.platform_data = &imapx_pwm_pdata,
	}
};

/************IIC***************/
static struct resource imap_iic0_resource[] = {
	[0] = {
	       .start = IMAP_IIC0_BASE,
	       .end = IMAP_IIC0_BASE + IMAP_IIC0_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IIC0_ID,
	       .end = GIC_IIC0_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_iic0_device = {
	.name = "imap-iic",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_iic0_resource),
	.resource = imap_iic0_resource,
};

static struct resource imap_iic1_resource[] = {
	[0] = {
	       .start = IMAP_IIC1_BASE,
	       .end = IMAP_IIC1_BASE + IMAP_IIC1_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IIC1_ID,
	       .end = GIC_IIC1_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_iic1_device = {
	.name = "imap-iic",
	.id = 1,
	.num_resources = ARRAY_SIZE(imap_iic1_resource),
	.resource = imap_iic1_resource,
};

static struct resource imap_iic2_resource[] = {
	[0] = {
	       .start = IMAP_IIC2_BASE,
	       .end = IMAP_IIC2_BASE + IMAP_IIC2_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IIC2_ID,
	       .end = GIC_IIC2_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_iic2_device = {
	.name = "imap-iic",
	.id = 2,
	.num_resources = ARRAY_SIZE(imap_iic2_resource),
	.resource = imap_iic2_resource,
};

static struct resource imap_iic3_resource[] = {
	[0] = {
	       .start = IMAP_IIC3_BASE,
	       .end = IMAP_IIC3_BASE + IMAP_IIC3_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IIC3_ID,
	       .end = GIC_IIC3_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_iic3_device = {
	.name = "imap-iic",
	.id = 3,
	.num_resources = ARRAY_SIZE(imap_iic3_resource),
	.resource = imap_iic3_resource,
};



/*************RTC******************/
static struct resource imap_rtc_resource[] = {
	[0] = {
	       .start = SYSMGR_RTC_BASE,
	       .end = SYSMGR_RTC_BASE + 0x3ff,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_RTCINT_ID,
	       .end = GIC_RTCINT_ID,
	       .flags = IORESOURCE_IRQ,
	       },
/*
	[2] = {
	       .start = GIC_RTCGP3_ID,
	       .end = GIC_RTCGP3_ID,
	       .flags = IORESOURCE_IRQ,
	       }
*/
};

struct platform_device imap_rtc_device = {
	.name = "imap-rtc",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_rtc_resource),
	.resource = imap_rtc_resource,
	.dev = {
		.platform_data = NULL,
	}
};

struct platform_device pcf8563_rtc_device = {
	.name = "pcf8563-rtc",
	.id = -1,
	.dev = {
		.platform_data = NULL,
	}
};

/*****************IIS*****************/
static struct resource imap_iis_resource0[] = {
	[0] = {
	       .start = IMAP_IIS0_BASE,
	       .end = IMAP_IIS0_BASE + 0xfff,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_MIIS_ID,
	       .end = GIC_MIIS_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_iis_device0 = {
	.name = "imapx-i2s",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_iis_resource0),
	.resource = imap_iis_resource0,
};

struct platform_device imap_null_device = {
	.name = "virtual-codec",
	.id = -1,
};

struct platform_device imap_ip6205_device = {
	.name = "ip6205",
	.id = 0,
};

/*
static struct resource imap_tqlp9624_resource[] = {
	[0] = {
	       .start = IMAP_TQLP9624_BASE,
	       .end = IMAP_TQLP9624_BASE + 0xfff,
	       .flags = IORESOURCE_MEM,
	       },
};

struct platform_device imap_tqlp9624_device = {
	.name = "tqlp9624",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_tqlp9624_resource),
	.resource = imap_tqlp9624_resource,
};
*/

/***************PCM***********************/
static struct resource imap_pcm0_resource[] = {
	[0] = {
	       .start = IMAP_PCM0_BASE,
	       .end = IMAP_PCM0_BASE + 0xfff,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_PCM0_ID,
	       .end = GIC_PCM0_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_pcm0_device = {
	.name = "imapx-pcm",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_pcm0_resource),
	.resource = imap_pcm0_resource,
};

static struct resource imap_pcm1_resource[] = {
	[0] = {
	       .start = IMAP_PCM1_BASE,
	       .end = IMAP_PCM1_BASE + 0xfff,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_PCM0_ID,
	       .end = GIC_PCM0_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_pcm1_device = {
	.name = "imapx-pcm",
	.id = 1,
	.num_resources = ARRAY_SIZE(imap_pcm1_resource),
	.resource = imap_pcm1_resource,
};

static inline bool pl022_filter(struct dma_chan *chan, void *param)
{
       u8 *peri_id;
       peri_id = chan->private;
       return *peri_id == (unsigned)param;
}

static struct pl022_ssp_controller imap_ssp0_data = {
	.bus_id = 0,
	.num_chipselect = 2,
	.enable_dma = 1,
	.dma_tx_param = (void *)IMAPX_SSP0_TX,
	.dma_rx_param = (void *)IMAPX_SSP0_RX,
	.dma_filter = pl022_filter,
};

struct amba_device imap_ssp0_device = {
	.dev = {
		.init_name = "imap-ssp.0",
		.platform_data = &imap_ssp0_data,
		.coherent_dma_mask = ~0,
		},
	.res = {
		.start = IMAP_SSP0_BASE,
		.end = IMAP_SSP0_BASE + IMAP_SSP0_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_SSP0_ID},
	.periphid = 0x00041022,
};

static struct pl022_ssp_controller imap_ssp1_data = {
	.bus_id = 1,
	.num_chipselect = 1,
	.enable_dma = 0,
};

struct amba_device imap_ssp1_device = {
	.dev = {
		.init_name = "imap-ssp.1",
		.platform_data = &imap_ssp1_data,
		},
	.res = {
		.start = IMAP_SSP1_BASE,
		.end = IMAP_SSP1_BASE + IMAP_SSP1_SIZE - 1,
		.flags = IORESOURCE_MEM,
		},
	.irq = {GIC_SSP1_ID},
	.periphid = 0x00041022,
};

/******************* watchdog ******************************/
static struct resource imap_wtd_resource[] = {
	[0] = {
	       .start = IMAP_WDT_BASE,
	       .end = IMAP_WDT_BASE + IMAP_WDT_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_WDT_ID,
	       .end = GIC_WDT_ID,
	       .flags = IORESOURCE_IRQ,
	       }

};

struct platform_device imap_wtd_device = {
	.name = "imap-wdt",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_wtd_resource),
	.resource = imap_wtd_resource,
};


/************ adc **********/
static struct resource imap_adc_resource[] = {
	[0] = {
	       .start = IMAP_ADC_BASE,
	       .end = IMAP_ADC_BASE + IMAP_ADC_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_ADC_ID,
	       .end = GIC_ADC_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

static struct adc_platform_data  imapx_adc_pdata = {
	.chan_bitmap = 0xE00,
};

struct platform_device imap_adc_device = {
	.name = "imap-adc",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_adc_resource),
	.resource = imap_adc_resource,
	.dev = {
		.platform_data = &imapx_adc_pdata,
	},
};

/************ emif **********/
static struct resource imap_emif_resource[] = {
	[0] = {
	       .start = IMAP_EMIF_BASE,
	       .end = IMAP_EMIF_BASE + IMAP_EMIF_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       }
};

struct platform_device imap_emif_device = {
	.name = "imap-emif",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_emif_resource),
	.resource = imap_emif_resource,
};

#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.no_allocator = 1,
	.cached = 1,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.no_allocator = 0,
	.cached = 0,
};

struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = {.platform_data = &android_pmem_adsp_pdata},
};
#endif

#ifdef CONFIG_SWITCH_WIFI
static struct gpio_switch_platform_data android_wifi_switch_pdata = {
	.name = "switch-wifi",
	.gpio = GIC_GP1_ID,
	.name_on = "switch-wifi",
	.name_off = "switch-wifi",
	.state_on = "1",
	.state_off = "0",
};

struct platform_device android_wifi_switch_device = {
	.name = "switch-wifi",
	.id = 0,
	.dev = {
		.platform_data = &android_wifi_switch_pdata},
};
#endif

/*******LCD*********/
static struct resource imap_lcd_resource[] = {
	[0] = {
	       .start = IMAP_IDS0_BASE,
	       .end = IMAP_IDS0_BASE + IMAP_IDS0_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IDS0_ID,
	       .end = GIC_IDS0_ID,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_lcd_device = {
	.name = "imap-fb",
	.id = 0,
	.num_resources = ARRAY_SIZE(imap_lcd_resource),
	.resource = imap_lcd_resource,
};

#ifdef CONFIG_ION_INFOTM
struct platform_device infotm_ion_device = {
	.name		= "infotm-ion",
	.id 		= -1,
};
#endif

#ifdef CONFIG_INFOTM_MEDIA_SUPPORT
static struct resource imap_venc_resource[] = {
	[0] = {
	       .start = 0,
	       .end = 0,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = 0,
	       .end = 0,
	       .flags = IORESOURCE_IRQ,
	       }
};

struct platform_device imap_venc_device = {
	.name = "imap-venc",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_venc_resource),
	.resource = imap_venc_resource,
};
#endif  // CONFIG_INFOTM_MEDIA_SUPPORT

#ifdef CONFIG_RGB2VGA_OUTPUT_SUPPORT
struct platform_device imap_rgb2vga_device = {
	.name = "imap-vga",
	.id = 0,
};
#endif

//camif power on cfg
static int imapx_cam_init()
{
	module_power_on(SYSMGR_CAMIF_BASE);
	module_reset(SYSMGR_CAMIF_BASE, 1);
	//imapx_pad_init("camif");
	return 0;
}

//camif power down 
static int imapx_cam_deinit()
{
	module_power_down(SYSMGR_CAMIF_BASE);

	//int ret = __raw_readl(SYS_CLOCK_GATE_EN(IO_ADDRESS(SYSMGR_CAMIF_BASE)));
	//printk("camif gate enable register value :0x%x", ret);

	return 0;
}

static struct imapx_cam_board imapx_cam_pdata = {
	.init = imapx_cam_init,
	.deinit = imapx_cam_deinit,
};

//camif device structure
static struct resource imap_camif_resource[] = {
	[0] = {
			.start = IMAP_CAMIF_BASE,
			.end   = IMAP_CAMIF_BASE + IMAP_CAMIF_SIZE - 1,
			.flags = IORESOURCE_MEM,
	},
	[1] = {
			.start = GIC_CAMIF_ID,
			.end   = GIC_CAMIF_ID,
			.flags = IORESOURCE_IRQ,
	}	
};

static u64 camif_dma_mask = DMA_BIT_MASK(32);

struct platform_device imap_camif_device = {
	.name = "imapx-camif",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_camif_resource),
	.resource = imap_camif_resource,
	.dev = {
		.platform_data = &imapx_cam_pdata,
		.dma_mask = &camif_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	}
};

static struct imapx_spi_host_config imapx_spi_data = {
	.bus_id = 0,
	.num_chipselect = 1,
	.enable_dma = 1,
	.rt = 0,
	.quirk = SPIMUL_QUIRK_RXLEN_ALLIGN_8BYTES | SPIMUL_QUIRK_DMA_BYTESWAP,
	.dma_tx_param = (void *)IMAPX_SSP0_TX,
	.dma_rx_param = (void *)IMAPX_SSP0_RX,
	.dma_filter = pl022_filter,
	.extclk_rate = 60000000 /*20MHZ*/
};

static struct resource imapx_spi_resource[] = {
	[0] = {
			.start = IMAP_SPIMUL_BASE,
			.end   = IMAP_SPIMUL_BASE + IMAP_SPIMUL_SIZE - 1,
			.flags = IORESOURCE_MEM,
	},
	[1] = {
			.start = GIC_SPIMUL_ID,
			.end   = GIC_SPIMUL_ID,
			.flags = IORESOURCE_IRQ,
	}	
};

struct platform_device imap_spi_device = {
	.name = "imapx-spi",
	.id = -1,
	.num_resources = ARRAY_SIZE(imapx_spi_resource),
	.resource = imapx_spi_resource,
	.dev = {
		.platform_data = &imapx_spi_data,
		.coherent_dma_mask = ~0,
	},

};

static struct resource imap_batt_resource[] = {
	[0] = {
	       .start = GIC_MGRSD_ID,
	       .end = GIC_MGRSD_ID,
	       .flags = IORESOURCE_IRQ,
	       },
};

struct platform_device android_device_batt = {
	.name = "imap820_battery",
	.id = -1,
	.num_resources = ARRAY_SIZE(imap_batt_resource),
	.resource = imap_batt_resource,
	.dev = {
		.platform_data = NULL,
	}
};
EXPORT_SYMBOL(android_device_batt);
#if 0
static struct gpio_switch_platform_data android_wifi_switch_pdata = {
	.name = "switch-wifi",
	.gpio = IRQ_GPIO,
	.name_on = "switch-wifi",
	.name_off = "switch-wifi",
	.state_on = "1",
	.state_off = "0",
};
#endif

struct platform_device android_wifi_switch_device = {
	.name = "switch-wifi",
	.id = 0,
};

#ifdef CONFIG_LEDS_IMAPX
static struct gpio_led leds_data[] = {

		[0] = {
				.name = "led0",
				.gpio = -1,
				.default_state = 0,
				.delay_on = 0,
				.delay_off = 1,
			  },
		[1] = {
				.name = "led1",
				.gpio = -1,
				.default_state = 0,
				.delay_on = 0,
				.delay_off = 1,
			  },
		[2] = {
				.name = "led2",
				.gpio = -1,
				.default_state = 0,
				.delay_on = 0,
				.delay_off = 1,
			  },
		[3] = {
				.name = "led3",
				.gpio = -1,
				.default_state = 0,
				.delay_on = 0,
				.delay_off = 1,
			  },
		[4] = {
				.name = "led4",
				.gpio = -1,
				.default_state = 0,
				.delay_on = 0,
				.delay_off = 1,
			  },
};

static struct gpio_led_platform_data imap_leds_data = {
	.num_leds = 0,
	.leds = leds_data,
};


struct platform_device imapx_leds_device = {
	.name = "imap_led",
	.id = 0,
	.dev = {
         .platform_data = &imap_leds_data,
        }
};
#endif

#ifdef CONFIG_LIGHTSENSOR_PT550
struct platform_device lightsensor_device = {
	.name = "pt550",
	.id = -1,
	.dev = {
		.platform_data = NULL,
	}
};
#endif

#ifdef CONFIG_BCM6212_BT
struct platform_device imap_bcm6212_bt_device = {
        .name = "bcm6212",
        .id = 0,
};
#endif
struct platform_device android_vibrator_device = {
	.name = "timed-gpio",
	.id = -1,
};

static struct resource imapx_ids_resource[] = {
	[0] = {
	       .start = IMAP_IDS0_BASE,
	       .end = IMAP_IDS0_BASE + IMAP_IDS0_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IDS0_ID,
	       .end = GIC_IDS0_ID,
	       .flags = IORESOURCE_IRQ,
	       },
};

struct platform_device imapx_ids_device = {
	.name = "display",
	.id = -1,
	.num_resources = ARRAY_SIZE(imapx_ids_resource),
	.resource = imapx_ids_resource,
};

#ifdef CONFIG_INFOTM_DISPLAY2

static struct resource display_path0_resource[] = {
	[0] = {
	       .start = IMAP_IDS0_BASE,
	       .end = IMAP_IDS0_BASE + IMAP_IDS0_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = GIC_IDS0_ID,
	       .end = GIC_IDS0_ID,
	       .flags = IORESOURCE_IRQ,
	       },
};

static u64 display_fb_dma_mask = ~(u32)0;

static struct display_device display_fb_device = {
	.name = "fb",
	.id = 0,
	.path = 0,
	.dev = {
		.dma_mask		= &display_fb_dma_mask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
};
static struct display_device display_clock_device = {
	.name = "clock",
	.id = 0,
	.path = 0,
};
static struct display_device display_overlay_device = {
	.name = "overlay",
	.id = 0,
	.path = 0,
};
static struct display_device display_scaler_device = {
	.name = "scaler",
	.id = 0,
	.path = 0,
};
static struct display_device display_lcdc_device = {
	.name = "lcdc",
	.id = 0,
	.path = 0,
};
static struct display_device display_tvif_device = {
	.name = "tvif",
	.id = 0,
	.path = 0,
};
static struct display_device display_irq_device = {
	.name = "irq",
	.id = 0,
	.path = 0,
	.num_resources = ARRAY_SIZE(display_path0_resource),
	.resource = display_path0_resource,
};
static struct display_device display_vic_device = {
	.name = "vic",
	.id = 0,
	.path = 0,
};
#endif

#ifdef CONFIG_ADC_KEYS_IMAPX 

static struct adc_keys_platform_data adc_button_data;

static struct platform_device adc_button_device = {
	.name 		= "adc-keys",
	.id 		= -1,
	.dev		= {
		.platform_data	= &adc_button_data,
	}
};
		
#endif

#if 0
static struct regulator_consumer_supply ricoh618_dc1_supply_0[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};

static struct regulator_consumer_supply ricoh618_dc2_supply_0[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};

static struct regulator_consumer_supply ricoh618_dc3_supply_0[] = {
	REGULATOR_SUPPLY("dc3", NULL),
};

static struct regulator_consumer_supply ricoh618_ldo1_supply_0[] = {
	REGULATOR_SUPPLY("ldo1", NULL),
};

static struct regulator_consumer_supply ricoh618_ldo2_supply_0[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};

static struct regulator_consumer_supply ricoh618_ldo3_supply_0[] = {
	REGULATOR_SUPPLY("ldo3", NULL),
};

static struct regulator_consumer_supply ricoh618_ldo4_supply_0[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};

static struct regulator_consumer_supply ricoh618_ldo5_supply_0[] = {
	REGULATOR_SUPPLY("ldo5", NULL),
};

RICOH_PDATA_INIT(dc1, 0, 900, 1500, 0, 1, 1, 0, -1, 0, 0, 0, 0, 0);
RICOH_PDATA_INIT(dc2, 0, 900, 1500, 0, 1, 1, 0, -1, 0, 0, 0, 0, 0);
RICOH_PDATA_INIT(dc3, 0, 900, 3300, 0, 1, 1, 0, -1, 0, 0, 0, 0, 0);

RICOH_PDATA_INIT(ldo1, 0, 1000, 3300, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);
RICOH_PDATA_INIT(ldo2, 0, 1000, 3300, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0);

RICOH_PDATA_INIT(ldo3, 0, 1000, 3300, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);
RICOH_PDATA_INIT(ldo4, 0, 1000, 3300, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);
RICOH_PDATA_INIT(ldo5, 0, 1000, 3300, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);

static struct ricoh618_pwrkey_platform_data ricoh618_pwrkey_data = {
	.irq		= INT_PMIC_BASE,
	.delay_ms	= 20,
};

static struct ricoh618_battery_platform_data ricoh618_battery_data = {
	.irq		= RICOH618_IRQ_BASE,
	.alarm_vol_mv	= 3300,
	/* .adc_channel = RICOH618_ADC_CHANNEL_VBAT, */
	.multiple	= 100, /* 100% */
	.monitor_time	= 60,

	/* VFCHG = 0 - 4 (4.05v, 4.10v, 4.15v, 4.20v, 4.35v) */
	.ch_vfchg	= 0x03,

	/* VRCHG = 0 - 4 (3.85v, 3.90v, 3.95v, 4.00v, 4.10v) */
	.ch_vrchg	= 0x04,

	/* VBATOVSET = 0 or 1
	 * (0 : 4.38v(up)/3.95v(down) 1 : 4.53v(up)/4.10v(down)) */
	.ch_vbatovset	= 0xFF,
	.ch_ichg	= 0x09,	/* ICHG		= 0 - 0x1D (100mA - 3000mA) */
	.ch_ilim_adp	= 0x18,	/* ILIM_ADP	= 0 - 0x1D (100mA - 3000mA) */
	.ch_ilim_usb	= 0x08,	/* ILIM_USB	= 0 - 0x1D (100mA - 3000mA) */
	.ch_icchg	= 0x01,	/* ICCHG = 0 - 3 (50mA 100mA 150mA 200mA) */
	.fg_target_vsys	= 3350,	/* This value is the target one to DSOC=0% */
	.fg_target_ibat	= 1000, /* This value is the target one to DSOC=0% */

	/* JEITA Enable = 0 or 1 (1:enable, 0:disable) */
	.jt_en		= 0,

	/* JEITA HW or SW = 0 or 1 (1:HardWare, 0:SoftWare) */
	.jt_hw_sw	= 1,
	.jt_temp_h	= 50,	/* degree C */
	.jt_temp_l	= 12,	/* degree C */

	/* VFCHG High = 0 - 4 (4.05v, 4.10v, 4.15v, 4.20v, 4.35v) */
	.jt_vfchg_h	= 0x03,

	/* VFCHG High = 0 - 4 (4.05v, 4.10v, 4.15v, 4.20v, 4.35v) */
	.jt_vfchg_l	= 0,

	/* VFCHG Low = 0 - 4 (4.05v, 4.10v, 4.15v, 4.20v, 4.35v) */
	.jt_ichg_h	= 0x0D,
	.jt_ichg_l	= 0x09,	/* ICHG Low = 0 - 0x1D (100mA - 3000mA) */

	.ip		= NULL,

/*  JEITA Parameter
*
*          VCHG
*            |
* jt_vfchg_h~+~~~~~~~~~~~~~~~~~~~+
*            |                   |
* jt_vfchg_l-| - - - - - - - - - +~~~~~~~~~~+
*            |    Charge area    +          |
*  -------0--+-------------------+----------+--- Temp
*            !                   +
*          ICHG
*            |                   +
*  jt_ichg_h-+ - -+~~~~~~~~~~~~~~+~~~~~~~~~~+
*            +    |              +          |
*  jt_ichg_l-+~~~~+   Charge area           |
*            |    +              +          |
*         0--+----+--------------+----------+--- Temp
*            0   jt_temp_l      jt_temp_h   55
*/
};

static struct ricoh618_subdev_info ricoh_devs_dcdc[] = {
	RICOH_REG(DC1, dc1, 0),
	RICOH_REG(DC2, dc2, 0),
	RICOH_REG(DC3, dc3, 0),
	RICOH_REG(LDO1, ldo1, 0),
	RICOH_REG(LDO2, ldo2, 0),
	RICOH_REG(LDO3, ldo3, 0),
	RICOH_REG(LDO4, ldo4, 0),
	RICOH_REG(LDO5, ldo5, 0),
	RICOH618_BATTERY_REG,
	RICOH618_PWRKEY_REG,
};

struct ricoh618_gpio_init_data ricoh_gpio_data[] = {
	RICOH_GPIO_INIT(false, false, 0, 0, 0),
	RICOH_GPIO_INIT(false, false, 0, 0, 0),
	RICOH_GPIO_INIT(false, false, 0, 0, 0),
	RICOH_GPIO_INIT(false, false, 0, 0, 0),
};

static struct ricoh618_platform_data ricoh_platform = {
	.num_subdevs		= ARRAY_SIZE(ricoh_devs_dcdc),
	.subdevs		= ricoh_devs_dcdc,
	.irq_base		= RICOH618_IRQ_BASE,
	.gpio_base		= PLATFORM_RICOH_GPIO_BASE,
	.gpio_init_data		= ricoh_gpio_data,
	.num_gpioinit_data	= ARRAY_SIZE(ricoh_gpio_data),
	.enable_shutdown_pin	= true,
	.ip			= NULL,
};

static struct i2c_board_info __initdata ricoh618_regulators[] = {
	{
		I2C_BOARD_INFO("ricoh618", 0x32),
		.irq		= PMIC_IRQ,
		.platform_data	= &ricoh_platform,
	},
};
#endif
static struct i2c_board_info __initdata ip6303_info[] = {
	{
		I2C_BOARD_INFO("ip6303", 0x30),
		//.irq		= IP6208_PMIC_IRQ,
		.platform_data = &ip6303_platform,
	},
};

static struct i2c_board_info __initdata ip620x_info1[] = {
	{
		I2C_BOARD_INFO("ip6208", 0x30),
		//.irq		= IP6208_PMIC_IRQ,
		.platform_data = &ip6208_platform,
	},
};
static struct i2c_board_info __initdata ip620x_info2[] = {
	{
		I2C_BOARD_INFO("ip6205", 0x30),
		//.irq		= IP6208_PMIC_IRQ,
		.platform_data = &ip6205_platform,
	},
};

static struct i2c_board_info __initdata ip6103_info[] = {
	{
		I2C_BOARD_INFO("ec610", 0x30),
		.irq		= EC610_PMIC_IRQ,
		.platform_data = &ec610_platform,
	},
};

/*
 * EEPROM devices
 */
static struct i2c_board_info __initdata eeprom_devs[] = {
	{
		I2C_BOARD_INFO("24c02", 0x50),
	},
};

#ifdef CONFIG_INFOTM_DISPLAY2
static struct display_device *display_devices[] __initdata = {
	&display_fb_device,
	&display_clock_device,
	&display_overlay_device,
	&display_scaler_device,
	&display_lcdc_device,
	&display_tvif_device,
	&display_irq_device,
	&display_vic_device,
};
#endif

static struct platform_device *imap_devices[] __initdata = {
	&imap_gpio_device,
	&imap_gpiochip_device,
#ifdef CONFIG_Q3F_FPGA_PLATFORM
	&imap_gpio_pwrkey_device,
#endif
	&imap_mmc0_device,
	&imap_mmc1_device,
	&imap_mmc2_device,
	&imap_pmu_device,
	&imap_ohci_device,
	&imap_ehci_device,
#ifdef	CONFIG_USB_G_ANDROID
	&android_storage_device,
#endif
	&imap_sensor_device,
	&imap_camif_device,
	&imap_spi_device,
	&imap_mac_device,
	&imap_memalloc_device,
	&imap_crypto_device,
	&imap_pwm_device,
	&imap_iic0_device,
	&imap_iic1_device,
	&imap_iic2_device,
	&imap_iic3_device,
	&imap_rtc_device,
	&pcf8563_rtc_device,
	&imap_wtd_device,
	&imap_adc_device,
	&android_device_batt,
#ifdef CONFIG_ADC_KEYS_IMAPX
	&adc_button_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
	&android_pmem_device,
	&android_pmem_adsp_device,
#endif
#ifdef	CONFIG_ANDROID_TIMED_GPIO
	&android_vibrator_device,
#endif
#ifdef CONFIG_BOSCH_BMA150
#endif
	&android_wifi_switch_device,

#ifdef CONFIG_ION_INFOTM
	&infotm_ion_device,
#endif

	&imap_venc_device,
	&imap_lcd_device,
	&imap_backlight_device,
	&imap_emif_device,
#ifdef CONFIG_LEDS_IMAPX
	&imapx_leds_device,
#endif
#ifdef CONFIG_LIGHTSENSOR_PT550
	&lightsensor_device,
#endif
#ifdef CONFIG_BCM6212_BT
	&imap_bcm6212_bt_device,
#endif
	&imapx_ids_device,
};

static struct platform_device *imap_dev_simple[] __initdata = {
	&imap_gpio_device,
	&imap_gpiochip_device,
	&imap_pmu_device,
	&imap_ohci_device,
	&imap_ehci_device,
	&imap_memalloc_device,
	&imap_pwm_device,
	&imap_iic0_device,
	&imap_iic1_device,
	&imap_iic2_device,
	&imap_iic3_device,
	&imap_rtc_device,
	&pcf8563_rtc_device,
	&imap_wtd_device,
	&imap_adc_device,
	&android_device_batt,
#ifdef CONFIG_ANDROID_PMEM
	&android_pmem_device,
	&android_pmem_adsp_device,
#endif
#ifdef	CONFIG_ANDROID_TIMED_GPIO
	&android_vibrator_device,
#endif

	&imap_lcd_device,
	&imap_backlight_device,
	&imap_emif_device,
#ifdef CONFIG_LEDS_IMAPX
	&imapx_leds_device,
#endif
	&imapx_ids_device,
};

static struct amba_device *amba_devs[] __initdata = {
	&imap_uart0_device,
	//&imap_uart1_device,
	&imap_uart3_device,
	&imap_dma_device,
#ifndef CONFIG_IMAPX_SPIMUL
	&imap_ssp0_device,
#endif
	&imap_ssp1_device,
};

static struct pl022_config_chip spi_flash_chip_info = {
	.com_mode = DMA_TRANSFER,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	.hierarchy = SSP_MASTER,
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
};

static void test_cs_control(u32 command) {
	imapx_pad_set_mode(74, "output");
	imapx_pad_set_value(74, !!command);
}

static struct imapx_spi_chip_config test_chip_info = {
	.com_mode = SPIMUL_TRANS_DMA,
	.rx_lev_trig = SPIMUL_RX_8_OR_MORE_ELEM,
	.tx_lev_trig = SPIMUL_TX_8_OR_MORE_EMPTY_LOC,
	.cs_control = test_cs_control,
};

#ifdef CONFIG_MTD_M25P80
static void m25p80_cs_control(u32 command)
{
//	printk(KERN_ERR "pl022: dummy chip select control, CS=0x%x\n", command);
	//printk("%s ssp:::::::cs--->%d\n", __func__, command);
#ifdef CONFIG_ARCH_APOLLO
	imapx_pad_set_mode(95, "output");
	imapx_pad_set_value(95, !!command);
#else
	imapx_pad_set_mode(74, "output");
	imapx_pad_set_value(74, !!command);
#endif
}

static struct pl022_config_chip m25p80_chip_info = {
	.com_mode = DMA_TRANSFER,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	.hierarchy = SSP_MASTER,
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
	.cs_control = m25p80_cs_control,
};

static struct mtd_partition spi_flash_part[] = {
	{
		.name = "root",
		.offset = 0,
		.size = MTDPART_SIZ_FULL,
	},
};      

static struct flash_platform_data spi_flash_data = {
	.type = "mx25l12805d",
        .parts = &spi_flash_part,
        .nr_parts = ARRAY_SIZE(spi_flash_part),
};
static struct spi_board_info spi_flash_info[] = {
	{
#ifdef CONFIG_IMAPX_SPIMUL
	 .max_speed_hz = 35000000,
	 .controller_data = &test_chip_info,
#else
	 .max_speed_hz = 25000000,
	 .controller_data = &m25p80_chip_info,
#endif
	 .modalias = "m25p80",
	 .bus_num = 0,
	 .chip_select = 0,
	 //.mode = SPI_CPHA | SPI_CPOL | SPI_MODE_0 | SPI_CS_HIGH,
	 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_0 | SPI_CS_HIGH | SPI_RX_QUAD | SPI_TX_QUAD,
	 .platform_data = &spi_flash_data,
	 },
};
#endif

#ifdef CONFIG_Q3F_LCD_SPI_ILI8961
static void ili8961_cs_control(u32 command)
{
#if 0 /* It is for debug spi. */
	printk(KERN_ERR "####### pl022: dummy chip select control, CS=0x%x\n", command);
	printk(KERN_ERR "%s ssp: cs--->%d\n", __func__, !!command);
#endif

#if 0 /*for spi 0*/
	imapx_pad_set_mode(69, "output");
	imapx_pad_set_value(69, !!command);
#else
	/*for spi 1*/
	imapx_pad_set_mode(74, "output");
	imapx_pad_set_value(74, !!command);
#endif
}

static struct pl022_config_chip ili8961_chip_info = {
	.com_mode = POLLING_TRANSFER,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	.hierarchy = SSP_MASTER,
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
	.cs_control = ili8961_cs_control,
};
static struct spi_board_info spi_lcd_info[] = {
	{
	 .modalias = "ili8961",
	 .max_speed_hz = 1000000,
	 .bus_num = 1, /* select spi bus 1 */
	 .chip_select = 0, /* only one device in spi1*/
	 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
	 .controller_data = &ili8961_chip_info,
	 .platform_data = NULL,
	 },
};
#endif

#ifdef CONFIG_Q3F_LCD_SPI_OTA5182A
static void ota5182a_cs_control(u32 command)
{
#if 0 /* It is for debug spi. */
	printk(KERN_ERR "####### pl022: dummy chip select control, CS=0x%x\n", command);
	printk(KERN_ERR "%s ssp: cs--->%d\n", __func__, !!command);
#endif

	/*for spi 1*/
	imapx_pad_set_mode(74, "output");
	imapx_pad_set_value(74, !!command);
}

static struct pl022_config_chip ota5182a_chip_info = {
	.com_mode = POLLING_TRANSFER,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	.hierarchy = SSP_MASTER,
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
	.cs_control = ota5182a_cs_control,
};
static struct spi_board_info ota5182a_spi_lcd_info[] = {
	{
	 .modalias = "ota5182a",
	 .max_speed_hz = 1*1000*1000, /* 1 * 1000* 1000,*///Note: old: 1000000 now: 10M,
	 .bus_num = 1, /* select spi bus 1 */
	 .chip_select = 0, /* only one device in spi1*/
	 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
	 .controller_data = &ota5182a_chip_info,
	 .platform_data = NULL,
	 },
};
#endif

void usb_disable(void){
	return;
}
/*
void tp_disable(void){
	return;
}
*/

#ifdef CONFIG_INFOTM_SOUND
static struct i2c_board_info __initdata audio_i2c[] = {
	[0] = {
		I2C_BOARD_INFO("es8328", 0x10),
	},
	[1] = {
		I2C_BOARD_INFO("rt5631", 0x1a),
	},
	[2] = {
		I2C_BOARD_INFO("es8323", 0x10),
	},
	[3] = {
		I2C_BOARD_INFO("fr1023", 0x10),
	},
	[4] = {
		I2C_BOARD_INFO("ip2906", 0x75),
	},
	/*
	[4] = {
		I2C_BOARD_INFO("ip6205", 0x30),
	},
	*/
};

struct audio_register audio[] = {
#ifdef CONFIG_SND_IMAPX_SOC_ES8328
	{
		.codec_name = "es8328",
		.ctrl = IIC_MODE,
		.info = &audio_i2c[0],
		.imapx_audio_cfg_init = imapx_es8328_init,
	},
#endif
#ifdef CONFIG_SND_IMAPX_SOC_ALC5631
	{
		.codec_name = "rt5631",
		.ctrl = IIC_MODE,
		.info = &audio_i2c[1],
		.imapx_audio_cfg_init = imapx_rt5631_init,
	},
#endif
#ifdef CONFIG_SND_IMAPX_SOC_ES8323
	{
		.codec_name = "es8323",
		.ctrl = IIC_MODE,
		.info = &audio_i2c[2],
		.imapx_audio_cfg_init = imapx_es8323_init,
	},
#endif
/*
#ifdef CONFIG_SND_IMAPX_SOC_TQLP9624
	{
		.codec_name = "9624tqlp",
		.ctrl = NOR_MODE,
		.codec_device = &imap_tqlp9624_device,
		//.imapx_audio_cfg_init = imapx_tqlp9624_init,
	},
#endif
*/
#ifdef CONFIG_SND_IMAPX_SOC_FR1023
	{
		.codec_name = "fr1023",
		.ctrl = IIC_MODE,
		.info = &audio_i2c[3],
		.imapx_audio_cfg_init = imapx_fr1023_init,
	},
#endif
#ifdef CONFIG_SND_IMAPX_SOC_IP6205
	{
		.codec_name = "ip6205",
		.ctrl = NOR_MODE,
//		.info = &audio_i2c[4],
		.codec_device = &imap_ip6205_device,
		.imapx_audio_cfg_init = imapx_ip6205_init,
	},
#endif
#ifdef CONFIG_SND_IMAPX_SOC_IP2906
	{
		.codec_name = "ip2906",
		.ctrl = IIC_MODE,
		.info = &audio_i2c[4],
		.imapx_audio_cfg_init = imapx_ip2906_init,
	},
#endif

};

/*
 * For config codec control, maybe by i2c, spi, or normal mem
 */

static void imapx_audio_ctrl_register(struct audio_register *ctrl, int id)
{
	struct codec_cfg *subdata = imapx_audio_cfg[id].codec;
	printk("codec pmu %s\n", imapx_audio_cfg[id].codec->power_pmu);
	
	if (ctrl->ctrl == IIC_MODE) {
		sprintf(ctrl->codec_name, "%s.%d-00%x",
				imapx_audio_cfg[id].name, imapx_audio_cfg[id].ctrl_busnum,
				ctrl->info->addr);
		ctrl->info->platform_data = subdata;
		i2c_register_board_info(imapx_audio_cfg[id].ctrl_busnum,
				ctrl->info, 1);
		sprintf("%s.%d-00%x",
				imapx_audio_cfg[id].name, imapx_audio_cfg[id].ctrl_busnum,
				ctrl->info->addr);		
	} else if (ctrl->ctrl == SPI_MODE) {
		/* TODO */
	} else {
		ctrl->codec_device->dev.platform_data = subdata;
		platform_device_register(ctrl->codec_device);
	}
}

/*
 * For config audio data transfer mode, maybe by i2s, pcm
 */

static void imapx_audio_data_register(struct audio_register *data, int id)
{
	struct codec_cfg *subdata = imapx_audio_cfg[id].codec;

	if (strcmp(imapx_audio_cfg[id].data_bus, "nobus"))
		sprintf(data->cpu_dai_name, "imapx-%s.%d",
				imapx_audio_cfg[id].data_bus,
				imapx_audio_cfg[id].data_busnum);

	if (subdata->i2s_mode == IIS_MODE)
		data->cpu_dai_device = &imap_iis_device0;
	else if (subdata->i2s_mode == PCM_MODE) {
		if (imapx_audio_cfg[id].data_busnum == 0)
			data->cpu_dai_device = &imap_pcm0_device;
		else
			data->cpu_dai_device = &imap_pcm1_device;
	}

	platform_device_register(data->cpu_dai_device);
	data->imapx_audio_cfg_init(data->codec_name,
			data->cpu_dai_name, subdata->i2s_mode, imapx_audio_cfg[id].spdif_en, id);
}
#endif//CONFIG_INFOTM_SOUND

/*
 * item_parse()
 */

static struct module_parse_status imapx_module_parse_status[] = {
	[0] = {
		.imapx_module_type = IMAPX_KEY,
		.need_parse = 1,
		.parse_done = 0,
	},
	[1] = {
		.imapx_module_type = IMAPX_WIFI,
		.need_parse = 1,	//Eason
		.parse_done = 0,
	},
	[2] = {
		.imapx_module_type = IMAPX_RTC,
		.need_parse = 1,
		.parse_done = 0,
	},
	[3] = {
		.imapx_module_type = IMAPX_BL,
		.need_parse = 1,
		.parse_done = 0,
	},
	[4] = {
		.imapx_module_type = IMAPX_LED,
		.need_parse = 1,
		.parse_done = 0,
	},
	[5] = {
		.imapx_module_type = IMAPX_PMU,
		.need_parse = 1,
		.parse_done = 0,
	},
	[6] = {
		.imapx_module_type = IMAPX_USB,
		.need_parse = 1,
		.parse_done = 0,
	},
	[7] = {
		.imapx_module_type = IMAPX_DWC_OTG,
		.need_parse = 1,
		.parse_done = 0,
	},
	[8] = {
		.imapx_module_type = IMAPX_AUDIO,
		.need_parse = 1,
		.parse_done = 0,
	},
	[9] = {
		.imapx_module_type = IMAPX_CAMERA,
		.need_parse = 1,
		.parse_done = 0,
	},
	[10] = {
		.imapx_module_type = IMAPX_ETH,
		.need_parse = 1,
		.parse_done = 0,
	},
	[11] = {
		.imapx_module_type = IMAPX_LS_HWMON,
		.need_parse = 1,
		.parse_done = 0,
	},
 	[12] = {
		.imapx_module_type = IMAPX_BATTERY,
 		.need_parse = 1,
 		.parse_done = 0,
 	},
 	[13] = {
		.imapx_module_type = IMAPX_SD,
 		.need_parse = 1,
 		.parse_done = 0,
 	}
};

void __init imapx_hwcfg_fix_to_device(struct module_parse_status *mps, int nr)
{
	int i, j, k;
	for (i = 0; i < nr; i++) {
		switch (mps->imapx_module_type) {
		case IMAPX_KEY:
			imap_gpio_device.dev.platform_data =
						&imapx_gpio_key_cfg;
			imap_gpiochip_device.dev.platform_data =
						&imapx_gpio_key_cfg;
#ifdef CONFIG_ADC_KEYS_IMAPX
			memset(&adc_button_data, 0x0, sizeof(struct adc_keys_platform_data));
			for (j = 0, k = 0; j < MAX_ADC_KEYS_NUM; j++) {
				if (adc_buttons_cfg[j].valid ) {
					memcpy(&adc_button_data.buttons[k], &adc_buttons_cfg[j]
						, sizeof(struct adc_keys_button));
					k++;
					adc_button_data.nbuttons = k;
				}
			}		
#endif 

			break;
		case IMAPX_WIFI:
			if(imapx_wifi_cfg.model_bus == 2)	//Eason
				imap_mmc2_platdata.wifi_en = 1;
			else
				imap_mmc2_platdata.wifi_en = 0;
			break;
		case IMAPX_RTC:
			imap_rtc_device.dev.platform_data =
						&imapx_rtc_cfg;
			pcf8563_rtc_device.dev.platform_data =
						&imapx_rtc_cfg;
			break;
		case IMAPX_BL:
			imap_backlight_data.pwm_id = imapx_bl_cfg.pwm_id;
			imap_backlight_data.pwm_period_ns = imapx_bl_cfg.pwm_period_ns;
			imap_backlight_data.max_brightness = imapx_bl_cfg.max_brightness;
			imap_backlight_data.dft_brightness = imapx_bl_cfg.dft_brightness;
			imap_backlight_data.lth_brightness = imapx_bl_cfg.lth_brightness;
			imap_backlight_data.polarity = imapx_bl_cfg.polarity;
			break;
#ifdef CONFIG_LEDS_IMAPX
		case IMAPX_LED:
			imap_leds_data.num_leds = imapx_led_cfg.num_leds;
			for(j=0;j<imap_leds_data.num_leds;j++){
			imap_leds_data.leds[j].name = imapx_led_cfg.led_data[j].model_name;
			imap_leds_data.leds[j].gpio = imapx_led_cfg.led_data[j].gpio;
			imap_leds_data.leds[j].active_low = imapx_led_cfg.led_data[j].polarity;
			imap_leds_data.leds[j].default_state = imapx_led_cfg.led_data[j].default_state;
			imap_leds_data.leds[j].delay_on = imapx_led_cfg.led_data[j].delay_on;
			imap_leds_data.leds[j].delay_off = imapx_led_cfg.led_data[j].delay_off;
			}
			break;
#endif
		case IMAPX_PMU:
			ip6208_platform.ip = &imapx_pmu_cfg;
			ip6205_platform.ip = &imapx_pmu_cfg;
			ip6303_platform.ip = &imapx_pmu_cfg;
			ec610_platform.ip = &imapx_pmu_cfg;
			break;
		case IMAPX_USB:
			imap_ohci_device.dev.platform_data =
				&imapx_usb_cfg;
			break;
		case IMAPX_DWC_OTG:
			imap_otg_device.dev.platform_data =
				&imapx_otg_cfg;
			if (imapx_otg_cfg.exist_otg > 0)
				platform_device_register(&imap_otg_device);
			break;
#ifdef CONFIG_INFOTM_SOUND
		case IMAPX_AUDIO:
			for (j = 0; j < ARRAY_SIZE(audio); j++) {
				for(k = 0; k < 2; k++){
					pr_err("support codec[%d] %s\n", k, imapx_audio_cfg[k].name);
					if (!strcmp(audio[j].codec_name,
							imapx_audio_cfg[k].name)) {
						imapx_audio_ctrl_register(&audio[j], k);
						imapx_audio_data_register(&audio[j], k);
                    	if (imapx_audio_cfg[k].spdif_en == 1) {
							pr_err("not support spdif\n");
                    	}
						break;
					}
				}
			}
			if (j == ARRAY_SIZE(audio))
				pr_err("Now, Not support this codec %s\n",
						imapx_audio_cfg[k].name);
			break;
#endif
		case IMAPX_CAMERA:
			{
				break;
			}
		case IMAPX_ETH:
			imap_mac_device.dev.platform_data =
				&imapx_eth_cfg;
			break;
		case IMAPX_LS_HWMON:
#ifdef CONFIG_LIGHTSENSOR_PT550
			lightsensor_device.dev.platform_data =
				&imapx_lightsensor_cfg;
#endif
			break;
		case IMAPX_BATTERY:
			if(PARSE_OK == mps->parse_done) {
				android_device_batt.dev.platform_data =
				&imapx_battery_cfg;
			}
			else {
				android_device_batt.dev.platform_data =
				NULL;
			}
			break;
		case IMAPX_SD:
			imap_mmc1_platdata.R_delayline_val = imapx_sd_cfg.sd1_Rdelayline_val;
			imap_mmc1_platdata.W_delayline_val = imapx_sd_cfg.sd1_Wdelayline_val;
			imap_mmc2_platdata.R_delayline_val = imapx_sd_cfg.sd2_Rdelayline_val;
			imap_mmc2_platdata.W_delayline_val = imapx_sd_cfg.sd2_Wdelayline_val;
			break;
		default:
			break;
		}
		mps++;
	}
}


void __init q3f_init_devices(void)
{
	int i;

	/* for dma plat-data */
	dma_cap_set(DMA_MEMCPY, imap_pl330_platdata.cap_mask);
	dma_cap_set(DMA_SLAVE, imap_pl330_platdata.cap_mask);
	dma_cap_set(DMA_CYCLIC, imap_pl330_platdata.cap_mask);
	imapx_item_parse(imapx_module_parse_status,
			ARRAY_SIZE(imapx_module_parse_status));
	imapx_hwcfg_fix_to_device(imapx_module_parse_status,
			ARRAY_SIZE(imapx_module_parse_status));
	/* Register the AMBA devices in the AMBA bus abstraction layer */
	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		struct amba_device *d = amba_devs[i];
		amba_device_register(d, &iomem_resource);
	}

#if defined(CONFIG_Q3F_LCD_SPI_ILI8961)
	spi_register_board_info(spi_lcd_info, ARRAY_SIZE(spi_lcd_info));
#endif

#if defined(CONFIG_Q3F_LCD_SPI_OTA5182A)
	spi_register_board_info(ota5182a_spi_lcd_info, ARRAY_SIZE(ota5182a_spi_lcd_info));
#endif

	/*add pmu i2c info*/
	if(!strcmp(imapx_pmu_cfg.name ,"ip6208")){
		i2c_register_board_info(0, ip620x_info1, ARRAY_SIZE(ip620x_info1));
	}else if(!strcmp(imapx_pmu_cfg.name ,"ip6205")){
		i2c_register_board_info(0, ip620x_info2, ARRAY_SIZE(ip620x_info2));
	}else if(!strcmp(imapx_pmu_cfg.name ,"ec610")){
		i2c_register_board_info(0, ip6103_info, ARRAY_SIZE(ip6103_info));
	}else if(!strcmp(imapx_pmu_cfg.name ,"ip6303")){
		i2c_register_board_info(0, ip6303_info, ARRAY_SIZE(ip6303_info));
	}

	/* register EEPROM */
	i2c_register_board_info(3, eeprom_devs, ARRAY_SIZE(eeprom_devs));

	if (strstr(boot_command_line, "charger")) {
		platform_add_devices(imap_dev_simple,
				     ARRAY_SIZE(imap_dev_simple));
	} else
		platform_add_devices(imap_devices, ARRAY_SIZE(imap_devices));

#ifdef CONFIG_INFOTM_DISPLAY2
	display_add_devices(display_devices, ARRAY_SIZE(display_devices));
#endif

#if defined(CONFIG_MTD_M25P80)
	spi_register_board_info(spi_flash_info, ARRAY_SIZE(spi_flash_info));
#endif
}
