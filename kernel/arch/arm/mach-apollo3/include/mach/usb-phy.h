#ifndef __USB_PHY_H__
#define __USB_PHY_H__


int host_phy_config(int32_t ref_clk, int utmi_16bit);
int otg_phy_config(int32_t ref_clk, int otg_flag, int utmi_16bit);
int otg_phy_reconfig(int utmi_16bit, int otg_flag);

/* get ehci usb port status */
uint32_t imap_ehci_port_state(void);
/* get ohci usb port status */
uint32_t imap_ohci_port_state(void);
/* get otg status for ensuring weather otg in slave mode or not */
int get_otg_status(void);
/* for start otg switch thread */
void start_monitor_thread(void);
#endif
