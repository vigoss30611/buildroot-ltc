/**
 ******************************************************************************
 @file i2c-img.h

 @brief
 Linux kernel mode I2C (SCB) driver for PCI FPGA card.

 @copyright Imagination Technologies Ltd. All Rights Reserved.

 @license Strictly Confidential.
 No part of this software, either material or conceptual may be copied or
 distributed, transmitted, transcribed, stored in a retrieval system or
 translated into any human or computer language in any form by any means,
 electronic, mechanical, manual or other-wise, or disclosed to third
 parties without the express written permission of
 Imagination Technologies Limited,
 Unit 8, HomePark Industrial Estate,
 King's Langley, Hertfordshire,
 WD4 8LZ, U.K.

 *****************************************************************************/
#ifndef I2C_IMG_H_
#define I2C_IMG_H_

#include <linux/pci.h>

int i2c_img_probe(struct pci_dev *dev, uintptr_t i2c_base_addr, int irq);
int i2c_img_remove(struct pci_dev *dev);
int i2c_img_suspend_noirq(struct pci_dev *dev);
int i2c_img_resume(struct pci_dev *dev);

#endif /* I2C_IMG_H_ */
