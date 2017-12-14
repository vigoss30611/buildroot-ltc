/* display item defination */

#ifndef __DISPLAY_ITEMS_HEADER__
#define __DISPLAY_ITEMS_HEADER__

#define DISPLAY_ITEM_PWRSEQ_MAX					32

#define DISPLAY_UBOOT_LOGO_ADDR					"idslogofb"

#define DISPLAY_ITEM_PERIPHERAL_DEFAULT				"ids%d.peripheral.default"	// lcd_panel, chip, hdmi, mipi_panel
#define DISPLAY_ITEM_PERIPHERAL_TYPE					"ids%d.peripheral%d"			// lcd_panel, chip, hdmi, mipi_panel
#define DISPLAY_ITEM_PERIPHERAL_NAME					"ids%d.%s.name"			// GM05004001Q_800_480, auto_detect(to be implement)
#define DISPLAY_ITEM_PERIPHERAL_I2CBUS				"ids%d.%s.i2cbus"				// i2c bus number if present
#define DISPLAY_ITEM_PERIPHERAL_IRQ						"ids%d.%s.irq"					// chip irq if present, usually coexist with i2c control interface. hdmi gpio irq instead of phy irq.
#define DISPLAY_ITEM_PERIPHERAL_CLKSRC				"ids%d.%s.clksrc"				// imap-clk.1
#define DISPLAY_ITEM_PERIPHERAL_PWRSEQ				"ids%d.%s.pwrseq.%d"		// gpio130.1.5, ldo3.1800000.10, clk.27000000.10
#define DISPLAY_ITEM_PERIPHERAL_HDMI_DFVIC		"ids%d.%s.default_vic"		// for hdmi. default vic
#define DISPLAY_ITEM_PERIPHERAL_HDMI_HDCP		"ids%d.%s.hdcp"					// for hdmi. enable hdcp or not
#define DISPLAY_ITEM_PERIPHERAL_HDMI_DVI			"ids%d.%s.hdmi_dvi"			// for hdmi. refers to enum display_hdmidvi_set


#define DISPLAY_ITEM_PRODUCT_NAME				"board.string.product"

#endif
