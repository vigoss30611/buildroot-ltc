#ifndef __IMAPX_USBHOST_H__
#define __IMAPX_USBHOST_H__

#define UHPHcRevision			(USBHOST_BASE_REG_PA+0x00)   /*  Revision */
#define UHPHcControl			(USBHOST_BASE_REG_PA+0x04)   /*  Operating modes for the Host Controller */
#define UHPHcCommandStatus		(USBHOST_BASE_REG_PA+0x08)   /*  Command & status Register */
#define UHPHcInterruptStatus	(USBHOST_BASE_REG_PA+0x0C)   /*  Interrupt Status Register */
#define UHPHcInterruptEnable	(USBHOST_BASE_REG_PA+0x10)   /*  Interrupt Enable Register */
#define UHPHcInterruptDisable	(USBHOST_BASE_REG_PA+0x14)   /*  Interrupt Disable Register */
#define UHPHcHCCA				(USBHOST_BASE_REG_PA+0x18)   /*  Pointer to the Host Controller Communication Area */
#define UHPHcPeriodCurrentED	(USBHOST_BASE_REG_PA+0x1C)   /*  Current Isochronous or Interrupt Endpoint Descriptor */
#define UHPHcControlHeadED		(USBHOST_BASE_REG_PA+0x20)   /*  First Endpoint Descriptor of the Control list */
#define UHPHcControlCurrentED	(USBHOST_BASE_REG_PA+0x24)   /*  Endpoint Control and Status Register */
#define UHPHcBulkHeadED			(USBHOST_BASE_REG_PA+0x28)   /*  First endpoint register of the Bulk list */
#define UHPHcBulkCurrentED		(USBHOST_BASE_REG_PA+0x2C)   /*  Current endpoint of the Bulk list */
#define UHPHcBulkDoneHead		(USBHOST_BASE_REG_PA+0x30)   /*  Last completed transfer descriptor */
#define UHPHcFmInterval			(USBHOST_BASE_REG_PA+0x34)   /*  Bit time between 2 consecutive SOFs */
#define UHPHcFmRemaining		(USBHOST_BASE_REG_PA+0x38)   /*  Bit time remaining in the current Frame */
#define UHPHcFmNumber			(USBHOST_BASE_REG_PA+0x3C)   /*  Frame number */
#define UHPHcPeriodicStart		(USBHOST_BASE_REG_PA+0x40)   /*  Periodic Start */
#define UHPHcLSThreshold		(USBHOST_BASE_REG_PA+0x44)   /*  LS Threshold */
#define UHPHcRhDescriptorA		(USBHOST_BASE_REG_PA+0x48)   /*  Root Hub characteristics A */
#define UHPHcRhDescriptorB		(USBHOST_BASE_REG_PA+0x4C)   /*  Root Hub characteristics B */
#define UHPHcRhStatus			(USBHOST_BASE_REG_PA+0x50)   /*  Root Hub Status register */
#define UHPHcRhPortStatus0		(USBHOST_BASE_REG_PA+0x54)   /*  Root Hub Port Status Register */
#define UHPHcRhPortStatus1		(USBHOST_BASE_REG_PA+0x58)   /*  Root Hub Port Status Register */

#endif /*__IMAPX_USBHOST_H__*/
