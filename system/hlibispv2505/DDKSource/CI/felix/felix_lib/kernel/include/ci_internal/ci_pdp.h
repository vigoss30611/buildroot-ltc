/**
******************************************************************************
@file ci_pdp.h

@brief Port Display Pipeline function declaration (IMG specific)

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

******************************************************************************/
#ifndef CI_PDP_H_
#define CI_PDP_H_

#define BAR0_BANK "REG_FPGA_BAR0_PDP"
#define BAR0_ENABLE_PDP_BIT 0x80000000
#define BAR0_PDP_OFFSET (0xC000)
#define BAR0_PDP_ADDRESS (0x4)

struct SYS_MEM;  // from ci_internal/sys_mem.h

IMG_RESULT Platform_MemWritePDPAddress(const struct SYS_MEM *pToWrite,
    IMG_SIZE uiToWriteOffset);

#endif /* CI_PDP_H_ */
