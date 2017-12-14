#ifndef __IROM_UDC_H__
#define __IROM_UDC_H__
#include <ch9.h>
typedef enum {
	false = 0,
	true = 1
}bool;

extern int udc_vs_reset(void);
extern int udc_vs_align(void);
extern int udc_vs_special(void *x);
extern int udc_get_cable_state(void);
extern int udc_connect(void);
extern int udc_disconnect(void);
extern int udc_read(uint8_t *buf, int len);
extern int udc_write(const uint8_t *buf, int len);
extern  bool udc_cable_connected(void);
extern bool udc_server_on(void);

enum ep0_state {
	EP0_DISCONNECT = 0, 
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_OUT_DATA_PHASE,
	EP0_IN_STATUS_PHASE,
	EP0_OUT_STATUS_PHASE,
	EP0_STALL,
};

struct udc_dev {
	uint8_t		link_ok;
	uint8_t		tx_ep;
	uint8_t		tx_en;
	uint8_t		rx_en;
	int		tx_len;
	int		rx_len;
	uint32_t	tx_addr;
	uint32_t	rx_addr;
	struct usb_ctrlrequest crq;
	uint8_t		speed;
	enum ep0_state ep0_sts;
	uint8_t*	buff_setup;
	int		rx_ram;
};

struct udc_settings_descriptor {
	struct usb_config_descriptor    cdesc;
	struct usb_interface_descriptor idesc;
	struct usb_endpoint_descriptor  epIN;
	struct usb_endpoint_descriptor  epOUT;
};

#define UDC_MAX_PACKET		0x200
#define UDC_IN_MAX_PACKET  0x40
#define UDC_MAX_PACKET_EP0	0x40

/* use ep1 as IN */
#define UDC_EP_IN			1
/* use ep2 as OUT */
#define UDC_EP_OUT			2//UDC_EP_IN

#endif /* __IROM_UDC_H__ */

