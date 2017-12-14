/**
*******************************************************************************
 @file pciuser.h

 @brief 
 
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
#ifndef _user_map_h


typedef struct _user_map_
{
	int fd;
	int size;
	void *mapped;
}user_device_mapping;

void CloseUserMapping(user_device_mapping *psMapping);
void * UserGetMapping(int ui16DeviceID,int ui16VendorID,char ui8Bar,unsigned long int ui32Offset,int ui16Size,user_device_mapping *psMapping );
#define _user_map_h
#endif