/**
******************************************************************************
@file           img_errors.c

@brief           Error string printing function

@copyright Imagination Technologies Ltd. All Rights Reserved. 
    
@license        <Strictly Confidential.> 
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
#include "img_errors.h"

const char * IMG_STR_ERROR(int ierr)
{
	const char * messages[IMG_NO_ERRORS] = {
		"No Error",
		"Timeout",
		"Malloc Failed",
		"Fatal Error",
		"Out of Memory",
		"Device Not Found",
		"Device Unavailable",
		"Generic Failure",
		"Interrupted",
		"Invalid Id",
		"Signature Incorrect",
		"Invalid Parameters",
		"Storage Empty",
		"Storage Full",
		"Already Complete",
		"Unexpected State",
		"Could not obtain Resource",
		"Not Initialised",
		"Already Initialised",
		"Value out of Range",
		"Cancelled",
		"Minimum Limit not met",
		"Not Supported",
		"Idle",
		"Busy",
		"Disabled",
		"Operation Prohibited",
		"MMU Page Directory Fault",
		"MMU Page Table Fault",
		"MMU Page Catalogue Fault",
		"Memory In Use",
		"Test Mismatch"
	};
	return (ierr > 0 && ierr < IMG_NO_ERRORS) ? messages[ierr] : "Unknown Error";
}
