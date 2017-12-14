/**
******************************************************************************
 @file info_helper.h

 @brief Functions to gather information from HW in test applications

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
#ifndef _ISPCT_INFOHELPER_
#define _ISPCT_INFOHELPER_

#include <iostream>
#include <ispc/Camera.h>

void PrintGasketInfo(ISPC::Camera &camera, std::ostream &os = std::cout);

void PrintRTMInfo(ISPC::Camera &camera, std::ostream &os = std::cout);

#endif // _ISPCT_INFOHELPER_
