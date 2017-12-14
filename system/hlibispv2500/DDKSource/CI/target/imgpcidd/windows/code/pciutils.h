/*****************************************************************************
 Name		: PCIUTILS.H 
 Title		: PCI Utilitites - header file

 Author		: John Metcalfe
 Created	: 15/9/97

 Copyright	: 1997 by VideoLogic Limited. All rights reserved.
		  No part of this software, either material or conceptual 
		  may be copied or distributed, transmitted, transcribed,
		  stored in a retrieval system or translated into any 
		  human or computer language in any form by any means,
		  electronic, mechanical, manual or other-wise, or 
		  disclosed to third parties without the express written
		  permission of VideoLogic Limited, Unit 8, HomePark
		  Industrial Estate, King's Langley, Hertfordshire,
		  WD4 8LZ, U.K.

 Description : PCI Utilitities - header file

*****************************************************************************/
#ifndef PCI_UTILS
#define PCI_UTILS

void PCIWriteByte  	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister, BYTE bData);
void PCIWriteWord  	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister, WORD bData);
void PCIWriteDword 	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister, DWORD bData);

BYTE PCIReadByte	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister);
WORD PCIReadWord	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister);
DWORD PCIReadDword	(BYTE bBus, BYTE bFunction, BYTE bDevice, BYTE bRegister);


#endif // PCI_UTILS
/****************************************************************************
	End of File PCIUTILS.H
****************************************************************************/
