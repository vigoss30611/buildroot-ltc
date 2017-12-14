#ifndef __IMAP_HWID_H__
#define __IMAP_HWID_H__

struct product_data {
	char		bid[16];
	uint16_t	sdio;
	uint16_t	nand;
	uint16_t	phone;
	uint16_t	rf;
	uint16_t	gsensor;
	uint16_t	camera;		/* 1: gt2005 */
	uint16_t	hdmi;
	uint16_t	ads;
	uint16_t	gpio_keys;
	uint16_t	lcd;
	uint16_t	audio;
	uint16_t	nor;
	uint16_t	wifi;
	uint16_t	tp;
	uint16_t	batt;
};

enum imap_product_id {
	IMAP_PRODUCT_SDIO = 0,
	IMAP_PRODUCT_NAND,
	IMAP_PRODUCT_PHONE,
	IMAP_PRODUCT_RF,
	IMAP_PRODUCT_GSENSOR,
	IMAP_PRODUCT_CAMERA,
	IMAP_PRODUCT_HDMI,
	IMAP_PRODUCT_ADS,
	IMAP_PRODUCT_GPIOKEYS,
	IMAP_PRODUCT_LCD,
	IMAP_PRODUCT_AUDIO,
	IMAP_PRODUCT_NOR,
	IMAP_PRODUCT_WIFI,
	IMAP_PRODUCT_TP,
	IMAP_PRODUCT_BATT,
};

extern int imap_product_getid(enum imap_product_id id);
#endif /* __IMAP_HWID_H__ */
