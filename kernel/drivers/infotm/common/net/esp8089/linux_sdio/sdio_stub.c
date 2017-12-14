/* Copyright (c) 2008 -2014 Espressif System.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 *  sdio stub code for infoTM
 */


#define SDIO_ID 0

extern void  sdio_scan(int flag);
extern void wifi_power(int power);

void sif_platform_rescan_card(unsigned insert)
{
    bool in = insert;
	printk("%s id %d %u\n", __func__, SDIO_ID, insert);
//	sw_mci_rescan_card(SDIO_ID, insert);
	sdio_scan(!in);
    msleep(10);
}

void sif_platform_reset_target(void)
{
//        gpio_ctrl("esp_wl_chip_en", 0);
        mdelay(100);
//        gpio_ctrl("esp_wl_chip_en", 1);

	mdelay(100);
}

void sif_platform_target_poweroff(void)
{
    wifi_power(0);
//        gpio_ctrl("esp_wl_chip_en", 0);
	mdelay(100);
}

void sif_platform_target_poweron(void)
{
    wifi_power(1);
//        gpio_ctrl("esp_wl_chip_en",1);
	mdelay(100);
}

void sif_platform_target_speed(int high_speed)
{
}

void sif_platform_check_r1_ready(struct esp_pub *epub)
{
}


#ifdef ESP_ACK_INTERRUPT
extern void dw_mmc_ack_interrupt(struct mmc_host *mmc);

void sif_platform_ack_interrupt(struct esp_pub *epub)
{
        struct esp_sdio_ctrl *sctrl = NULL;
        struct sdio_func *func = NULL;

	if (epub == NULL) {
        	ESSERT(epub != NULL);
		return;
	}
        sctrl = (struct esp_sdio_ctrl *)epub->sif;
        func = sctrl->func;
	if (func == NULL) {
        	ESSERT(func != NULL);
		return;
	}
        
	dw_mmc_ack_interrupt(func->card->host);
}
#endif //ESP_ACK_INTERRUPT

