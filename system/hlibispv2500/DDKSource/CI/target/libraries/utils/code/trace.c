/* -*- c-file-style: "img" -*-
<module>
* Name         : trace.c
* Title        : Buffer Management Trace Support
* Author       : Marcus Shawcroft
* Created      : 14 May 2003
*
* Copyright    : 2003 by Imagination Technologies Limited.
*                All rights reserved.  No part of this software, either
*                material or conceptual may be copied or distributed,
*                transmitted, transcribed, stored in a retrieval system
*                or translated into any human or computer language in any
*                form by any means, electronic, mechanical, manual or
*                other-wise, or disclosed to third parties without the
*                express written permission of Imagination Technologies
*                Limited, Unit 8, HomePark Industrial Estate,
*                King's Langley, Hertfordshire, WD4 8LZ, U.K.
*
* Description :
*
*                Diagnostic tracing support.
* 
* Platform     : all bareboards
*
</module>
********************************************************************************/

#if !defined (IMG_KERNEL_MODULE)
#include <stdio.h>
#endif
#include <stdarg.h>

#include "trace.h"

void
trace_ (char *format, ...)
{
  va_list ap;
     
  va_start (ap, format);
#if !defined (IMG_KERNEL_MODULE)
  vfprintf (stderr, format, ap);
#endif
  va_end (ap);
}
