#ifndef __MACH_IMAP_DEVICE_H__
#define __MACH_IMAP_DEVICE_H__

#include <linux/platform_device.h>
#include <linux/amba/bus.h>

/* amba device */
extern struct amba_device imap_uart0_device; 
extern struct amba_device imap_uart1_device;
extern struct amba_device imap_uart2_device;
extern struct amba_device imap_uart3_device;
extern struct amba_device imap_pic0_device;
extern struct amba_device imap_pic1_device;
extern struct amba_device imap_dma_device;
extern struct amba_device imap_ssp0_device;
extern struct amba_device imap_ssp1_device;

/* platform device */
extern struct platform_device imap_nand_device;
extern struct platform_device imap_gpio_device;
extern struct platform_device imap_mmc0_device;
extern struct platform_device imap_mmc1_device;
extern struct platform_device imap_mmc2_device;
extern struct platform_device imap_pmu_device;
extern struct platform_device imap_sata_device;
extern struct platform_device imap_ohci_device;
extern struct platform_device imap_ehci_device;
extern struct platform_device imap_otg_device;

#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
extern struct platform_device android_storage_device;
#endif
extern struct platform_device android_device_batt;
extern struct platform_device imap_sensor_device;
extern struct platform_device imap_backlight_device;
extern struct platform_device imap_mac_device;
extern struct platform_device imap_memalloc_device;
extern struct platform_device imap_gps_device;
extern struct platform_device imap_pwm_device;
extern struct platform_device imap_iic0_device;
extern struct platform_device imap_iic1_device;
extern struct platform_device imap_iic2_device;
extern struct platform_device imap_iic3_device;
extern struct platform_device imap_iic4_device;
extern struct platform_device imap_iic5_device;
extern struct platform_device imap_rtc_device;
extern struct platform_device imap_es8328_device;
extern struct platform_device imap_null_device;
extern struct platform_device imap_rt5631_device;
extern struct platform_device imap_iis_device0;
extern struct platform_device imap_iis_device1;
extern struct platform_device imap_pcm0_device;
extern struct platform_device imap_pcm1_device;
extern struct platform_device imap_pwma_device;
extern struct platform_device imap_spdif_device;
extern struct platform_device imap_ac97_device;
extern struct platform_device imap_asoc_device;
extern struct platform_device imap_wtd_device;
extern struct platform_device imap_keybd_device;

#ifdef CONFIG_ANDROID_PMEM
extern struct platform_device android_pmem_device;
extern struct platform_device android_pmem_adsp_device;
#endif

extern struct platform_device android_wifi_switch_device;

extern struct platform_device imap_lcd_device;

#ifdef CONFIG_ION_INFOTM
extern struct platform_device infotm_ion_device;
#endif

#ifdef CONFIG_INFOTM_MEDIA_SUPPORT
#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT
extern struct platform_device infotm_dmmu_device;
#endif
#ifdef CONFIG_INFOTM_MEDIA_VENC_8270_SUPPORT
extern struct platform_device imap_venc_device;
#endif
#endif

#ifdef CONFIG_RGB2VGA_OUTPUT_SUPPORT
extern struct platform_device imap_rgb2vga_device;
#endif
#ifdef CONFIG_LEDS_APOLLO
extern struct platform_device apollo_leds_device;
#endif




#endif
