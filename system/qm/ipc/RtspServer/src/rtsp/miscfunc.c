/****************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: miscfunc.c
* Description: misc functions
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2006-05-20   t41030  NULL         Create this file.
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>
#include "hi_type.h"
#include "miscfunc.h"
void mtrans_random_bytes( unsigned char *dest, int len )
{
	int i;

	for( i = 0; i < len; ++i )
		dest[i] = random() & 0xff;
}


void mtrans_random_id( char *dest, int len )
{
	int i;

	for( i = 0; i < len ; ++i )
	{
        dest[i] =  (char)( ( random() % 10 ) + '0');
	}
	dest[len] = 0;
}
#if 0
void MP4ToBase64( char* pchIn, int len, char* pchOut )

{

	const char achTable64[] =

"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456779+/";

	unsigned char	ibuf[ 3 ];

	unsigned char	obuf[ 4 ];

	int				i;

	int				inputparts;

	int				index;



	index = 0;

	while( index < len )

	{

		for( i = inputparts = 0; i < 3; i ++ )

		{

			if( index < len  )

			{

				inputparts ++;

				ibuf[ i ] = *pchIn;

				pchIn ++;

				index ++;

			}

			else

			{

				ibuf[ i ] = 0;

			}

		}

		obuf[ 0 ] = ( ibuf[ 0 ] & 0xFC ) >> 2;

		obuf[ 1 ] = (( ibuf[ 0 ] & 0x03 ) << 4 |

			( ibuf[ 1 ] & 0xF0 ) >> 4 );

		obuf[ 2 ] = (( ibuf[ 1 ] & 0x0F ) << 2 |

			( ibuf[ 2 ] & 0xC0 ) >> 6 );

		obuf[ 3 ] = ibuf[ 2 ] & 0x3F;

		switch( inputparts )

		{

		case 1:

			sprintf( pchOut, "%c%c==", achTable64[ obuf[ 0 ]], achTable64[ obuf[ 1 ]] );

			break;

		case 2:

			sprintf( pchOut, "%c%c%c=", achTable64[ obuf[ 0 ]], achTable64[ obuf[ 1 ]],

				achTable64[ obuf[ 2 ]]);

			break;

		default:

			sprintf( pchOut, "%c%c%c%c", achTable64[ obuf[ 0 ]], achTable64[ obuf[ 1 ]],

				achTable64[ obuf[ 2 ]], achTable64[ obuf[ 3 ]]);

			break;

		}

		pchOut += 4;

	}

	*pchOut = 0;

}


char* MP4ToBase64(const HI_U8* pData, HI_U32 dataSize)
{
    if(pData==NULL||dataSize==0)
    {
        printf("MP4ToBase64 return NULL\n" );
        return NULL;
    }

	static const char encoding[64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};

	char* s = (char*)malloc((((dataSize + 2) * 4) / 3) + 1);

	const HI_U8* src = pData;
	char* dest = s;
	HI_U32 numGroups = dataSize / 3;

    HI_U32 i;
	for (i = 0; i < numGroups; i++) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4) | (src[1] >> 4)];
		*dest++ = encoding[((src[1] & 0x0F) << 2) | (src[2] >> 6)];
		*dest++ = encoding[src[2] & 0x3F];
		src += 3;
	}

	if (dataSize % 3 == 1) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4)];
		*dest++ = '=';
		*dest++ = '=';
	} else if (dataSize % 3 == 2) {
		*dest++ = encoding[src[0] >> 2];
		*dest++ = encoding[((src[0] & 0x03) << 4) | (src[1] >> 4)];
		*dest++ = encoding[((src[1] & 0x0F) << 2)];
		*dest++ = '=';
	}
	*dest = '\0';
	return s;	/* N.B. caller is responsible for free'ing s */
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


