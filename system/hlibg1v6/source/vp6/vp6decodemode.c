/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp6decodemode.c,v $
-  $Revision: 1.2 $
-  $Date: 2008/04/24 12:08:29 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp6dec.h"
#include "vp6decodemode.h"

/****************************************************************************
 * 
 *  ROUTINE       : VP6HWdecodeModeDiff
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       : None.
 *
 *  RETURNS       : a probability difference value decoded from the bitstream.
 *
 *  FUNCTION      : this function returns a probability difference value in
 *                  the range -256 to +256 (in steps of 4) transmitted in the
 *                  bitstream using a fixed tree with hardcoded probabilities.
 *
 *  SPECIAL NOTES : The hard coded probabilities for the difference tree
 *                  were calcualated by taking the average number of times a 
 *                  branch was taken on some sample material ie 
 *                  (bond,bike,beautifulmind)
 *
 ****************************************************************************/
int VP6HWdecodeModeDiff ( PB_INSTANCE *pbi )
{
	int sign;

    if ( VP6HWDecodeBool(&pbi->br, 205) == 0 )
		return 0;
	
	sign = 1 + -2 * VP6HWDecodeBool128(&pbi->br);
	
	if( !VP6HWDecodeBool(&pbi->br,171) )
	{
        return sign<<(3-VP6HWDecodeBool(	&pbi->br,83));
	}
	else
	{
		if( !VP6HWDecodeBool(	&pbi->br,199) ) 
		{
			if(VP6HWDecodeBool(	&pbi->br,140))
				return sign * 12;

			if(VP6HWDecodeBool(	&pbi->br,125))
				return sign * 16;

			if(VP6HWDecodeBool(	&pbi->br,104))
				return sign * 20;

			return sign * 24;
		}
		else 
		{
			int diff = VP6HWbitread(&pbi->br,7);
			return sign * diff * 4;
		}
	}
}
/****************************************************************************
 * 
 *  ROUTINE       : VP6HWBuildModeTree
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       : None.
 *
 *  RETURNS       : void    
 *
 *  FUNCTION      : Fills in probabilities at each branch of the huffman tree
 *                  based upon probXmitted, the frequencies transmitted in the bitstream.
 *
 ****************************************************************************/
void VP6HWBuildModeTree ( PB_INSTANCE *pbi )
{
	int i,j,k;

	// create a huffman tree and code array for each of our modes 
    // Note: each of the trees is minus the node give by probmodesame
	for ( i=0; i<10; i++ )
	{
		unsigned int Counts[MAX_MODES];
		unsigned int total;

		// set up the probabilities for each tree
		for(k=0;k<MODETYPES;k++)
		{
			total=0;
			for ( j=0; j<10; j++ )
			{	
				if ( i == j )
				{
					Counts[j]=0;
				}
				else
				{
					Counts[j]=100*pbi->probXmitted[k][0][j];
				}

				total+=Counts[j];
			}

			pbi->probModeSame[k][i] = 255-
				255 * pbi->probXmitted[k][1][i] 
				/
				(	1 +
					pbi->probXmitted[k][1][i] +	
					pbi->probXmitted[k][0][i]
				);

			// each branch is basically calculated via 
			// summing all posibilities at that branch.
			pbi->probMode[k][i][0]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]+
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				) / 
				(   1 +
				    total
				);

			pbi->probMode[k][i][1]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]
				) / 
				(
					1 + 
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]+
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				);

			pbi->probMode[k][i][2]= 1 + 255 *
				(
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]
				) / 
				(
					1 + 
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]+
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]+
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);
			
			pbi->probMode[k][i][3]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]
				) / 
				(
					1 +
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]
				);

			pbi->probMode[k][i][4]= 1 + 255 *
				(
					Counts[CODE_INTER_NEAREST_MV]
				) / 
				(
					1 +
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				) ;

			pbi->probMode[k][i][5]= 1 + 255 *
				(
					Counts[CODE_INTRA]
				) / 
				(
					1 +
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]
				);

			pbi->probMode[k][i][6]= 1 + 255 *
				(
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]
				) / 
				(
					1 +
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]+
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);

			pbi->probMode[k][i][7]= 1 + 255 *
				(
					Counts[CODE_USING_GOLDEN]
				) / 
				(
					1 +
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]
				);

			pbi->probMode[k][i][8]= 1 + 255 *
				(
					Counts[CODE_GOLD_NEAREST_MV]
				) / 
				(
					1 +
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);
		}
	}
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6HWDecodeModeProbs
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     This function parses the probabilities transmitted in 
 *                      the bitstream. The bitstream may either use the 
 *                      last frames' baselines, or transmit a pointer to a
 *                      vector of new probabilities. It may then additionally
 *                      contain updates to each of these probabilities.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6HWDecodeModeProbs ( PB_INSTANCE *pbi )
{
	int i,j;

	// For each mode type (all modes available, no nearest, no near mode)
	for ( j=0; j<MODETYPES; j++ )
	{
		// determine whether we are sending a vector for this mode byte
		if ( VP6HWDecodeBool( &pbi->br, PROBVECTORXMIT ) )
		{
			// figure out which vector we have encoded
			int whichVector = VP6HWbitread(&pbi->br, 4);

			// adjust the vector
			for ( i=0; i<MAX_MODES; i++ )
			{
				pbi->probXmitted[j][1][i] = VP6HWModeVq[j][whichVector][i*2];
				pbi->probXmitted[j][0][i] = VP6HWModeVq[j][whichVector][i*2+1];
			}

            pbi->probModeUpdate = 1;
		} 

		// decode updates to bring it closer to ideal 
		if ( VP6HWDecodeBool( &pbi->br, PROBIDEALXMIT) )
		{
			for ( i=0; i<10; i++ )
			{
				int diff;

				// determine difference 
				diff = VP6HWdecodeModeDiff(pbi);
				diff += pbi->probXmitted[j][1][i];

				pbi->probXmitted[j][1][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

				// determine difference 
				diff = VP6HWdecodeModeDiff(pbi);
				diff += pbi->probXmitted[j][0][i];

				pbi->probXmitted[j][0][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

			}

            pbi->probModeUpdate = 1;
		}
	}
	
	VP6HWBuildModeTree(pbi);
}
