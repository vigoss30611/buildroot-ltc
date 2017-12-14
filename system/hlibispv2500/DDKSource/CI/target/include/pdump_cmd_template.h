/*! 
********************************************************************************
 @file   : pdump_cmd_template.h

 @brief  Pdump Command API

 @Author Jose Massada

 @date   20/06/2012
 
         <b>Copyright 2010 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         Header file containing Pdump Command API.

*******************************************************************************/

#ifndef __PDUMP_CMD_TEMPLATE_H__
#define __PDUMP_CMD_TEMPLATE_H__

#include "pdump_element.h"

#define PDUMP_CMD_MAX_NO_TOKENS	  (3)	/* 3 ought to be enough for anybody */
#define PDUMP_CMD_MAX_NO_ELEMENTS (20)	/* 20 ought to be enough for anybody */

typedef struct _pdump_cmd_sCommandTemplate
{
	const IMG_CHAR *pszCommandName;
	PDUMP_eElementType eElementType[PDUMP_CMD_MAX_NO_ELEMENTS + 1];
} pdump_cmd_sCommandTemplate;

PDUMP_CMD_BEGIN_EXTERN_C

extern
const pdump_cmd_sCommandTemplate gasCommandTemplate[PDUMP_CMD_COMMAND_NO_TYPES];

PDUMP_CMD_END_EXTERN_C

#endif /* __PDUMP_CMD_TEMPLATE_H__ */
