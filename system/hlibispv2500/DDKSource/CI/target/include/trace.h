/*! 
******************************************************************************
 @file   : trace.h

 @brief  

 @Author Marcus Shawcroft

 @date   13/02/2003
 
         <b>Copyright 2004 by Imagination Technologies Limited.</b>\n
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
         Resource allocator trace header file.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/
/*
 */

#ifndef _trace_h_
#define _trace_h_

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef TRACE
#define trace(P) trace_ P
#else
#define trace(P)
#endif

void
trace_ (char *format, ...);


#if defined (__cplusplus)
}
#endif


#endif
