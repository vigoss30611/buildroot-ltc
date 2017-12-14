/**
******************************************************************************
 @file driver_init.h

 @brief Fake driver initialise/finalise function (used to fake insmod/rmmod
 when fake interface is used)

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
#ifndef ISPCT_DRV_INIT_H
#define ISPCT_DRV_INIT_H

int initializeFakeDriver(unsigned int useMMU = 0, int gmaCurve = -1, 
    unsigned int pagesize = 4<<10, unsigned int tilingScheme = 256,
    char *pszTransifLib = 0, char *pszTransifSimParam = 0);
int finaliseFakeDriver(void); 

#endif // ISPCT_DRV_INIT_H
