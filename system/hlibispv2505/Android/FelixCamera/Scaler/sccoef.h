/**************************************************************************
 Name         : sccoef.h
 Title        : SCCoef.h
 Author       : Paul Buxton
 Created      : 29-9-99

 Copyright    : 1999 by Imagination Technologies Limited. All rights reserved.
              : No part of this software, either material or conceptual
              : may be copied or distributed, transmitted, transcribed,
              : stored in a retrieval system or translated into any
              : human or computer language in any form by any means,
              : electronic, mechanical, manual or other-wise, or
              : disclosed to third parties without the express written
              : permission of VideoLogic Limited, Unit 8, HomePark
              : Industrial Estate, King's Langley, Hertfordshire,
              : WD4 8LZ, U.K.

 Description  : Scale Factor Calculation and Coeficcient Generation.

 Platform     : Independent
 Version      : $Revision: 1.1 $
 $Log: sccoef.h

**************************************************************************/
#ifndef __SCCOEF
#define __SCCOEF

void CalcCoefs(DWORD dwSrc,DWORD dwDest,BYTE Table[8][8],DWORD I, DWORD T, DWORD Tdiv);

#endif

#define OLD_COEFFS 0                     /* 1 for old coefficients */

#define MAXTAP 9
#define MAXINTPT 8

#define IzeroEPSILON 1E-21               /* Max error acceptable in Izero */

float Beta;
float Lanczos;
