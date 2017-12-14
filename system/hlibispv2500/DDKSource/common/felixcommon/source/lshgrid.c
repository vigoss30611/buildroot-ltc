/**
******************************************************************************
 @file lshgrid.c

 @brief implementation of the LSH file handling

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
#include "felixcommon/lshgrid.h"

#include <img_defs.h>
#include <img_errors.h>

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES // for win32 M_PI definition
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#define LOG_TAG "LSH_OUT"
#include <felixcommon/userlog.h>

#define LSH_VERSION 1
#define LSH_HEAD "LSH\0"
#define LSH_HEAD_SIZE 4

// needed because store as 32b float
// should be optimised by the compiler!
#define GRID_DOUBLE_CHECK ( sizeof(LSH_FLOAT) > sizeof(float) )

IMG_RESULT LSH_CreateMatrix(LSH_GRID *pLSH, IMG_UINT16 ui16Width, IMG_UINT16 ui16Height, IMG_UINT16 ui16TileSize)
{
	IMG_INT32 c;

	/*if ( ui16TileSize < LSH_TILE_MIN || ui16TileSize > LSH_TILE_MAX )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}*/

	if ( pLSH->apMatrix[0] != NULL )
	{
        LOG_DEBUG("Matrix already allocated\n");
		return IMG_ERROR_ALREADY_INITIALISED;
	}
	{
		IMG_UINT32 tmp = ui16TileSize;
		IMG_UINT32 log2Res = 0;
		while(tmp >>= 1) // log2
		{
			log2Res++;
		}

		if ( ui16TileSize != (1<<log2Res) )
		{
			LOG_DEBUG("ui16TileSize has to be a power of 2 (%u given)\n", ui16TileSize);
			return IMG_ERROR_NOT_SUPPORTED;
		}
	}

	pLSH->ui16Width = ui16Width/ui16TileSize; // computer number of tiles
	pLSH->ui16Height = ui16Height/ui16TileSize;
	pLSH->ui16TileSize = ui16TileSize;

	if ( ui16Width%ui16TileSize != 0 )
	{
		pLSH->ui16Width++;
	}
	if ( ui16Height%ui16TileSize != 0 )
	{
		pLSH->ui16Height++;
	}
	pLSH->ui16Width++; // add the last cell - it is a bilinear interpolation
	pLSH->ui16Height++;

	for ( c = 0 ; c < LSH_MAT_NO ; c++ )
	{
		pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));
		if ( pLSH->apMatrix[c] == NULL )
		{
			c--;
			while (c>=0)
			{
				IMG_FREE(pLSH->apMatrix[c]);
				c--;
			}
            LOG_ERROR("failed to allocate matrix\n");
			return IMG_ERROR_MALLOC_FAILED;
		}
	}
	return IMG_SUCCESS;
}

IMG_RESULT LSH_FillLinear(LSH_GRID *pLSH, IMG_UINT8 c, const float aCorners[4])
{
	IMG_UINT32 row, col;

	//for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
	{
		if ( pLSH->apMatrix[c] == NULL )
		{
            LOG_DEBUG("The matrix %d is not allocated\n", c);
			return IMG_ERROR_FATAL;
		}
	}
	if ( pLSH->ui16Width <= 1 || pLSH->ui16Height <= 1 )
	{
        LOG_DEBUG("Size is too small\n");
		return IMG_ERROR_FATAL;
	}

	LOG_INFO("generate linear LSH %ux%u grid (tile size is %u CFA) - corners %f %f %f %f\n",
			pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize, aCorners[0], aCorners[1], aCorners[2], aCorners[3]);

	//for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
	{
		for ( row = 0 ; row < pLSH->ui16Height ; row++ )
		{
			double rr = row / (LSH_FLOAT)(pLSH->ui16Height -1); // ratio row

			for ( col = 0 ; col < pLSH->ui16Width ; col++ )
			{
				double rc = col / (LSH_FLOAT)(pLSH->ui16Width-1); // ratio column
				double val = (aCorners[0])*(1.0-rr)*(1.0-rc) +
                                (aCorners[1])*(1.0-rr)*rc +
                                (aCorners[2])*rr*(1.0-rc) +
                                (aCorners[3])*rr*rc;

				pLSH->apMatrix[c][row*pLSH->ui16Width + col] = (LSH_FLOAT)val;
			}//for col
		}//for row
	}

	return IMG_SUCCESS;
}

IMG_RESULT LSH_FillBowl(LSH_GRID *pLSH, IMG_UINT8 c, const float aCorners[5])
{
	IMG_UINT32 row, col;
	double bowlx, bowly, bowl;

	//for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
	{
		if ( pLSH->apMatrix[c] == NULL )
		{
            LOG_DEBUG("The matrix %d is not allocated\n", c);
			return IMG_ERROR_FATAL;
		}
	}
	if ( pLSH->ui16Width <= 1 || pLSH->ui16Width <= 1 )
	{
        LOG_DEBUG("Size is too small\n");
		return IMG_ERROR_FATAL;
	}

	LOG_INFO("generate bowl LSH %ux%u grid (tile size is %u CFA) - corners %f %f %f %f center %f\n",
			pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize, aCorners[0], aCorners[1], aCorners[2], aCorners[3], aCorners[4]);

	//for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
	{
		for ( row = 0 ; row < pLSH->ui16Height ; row++ )
		{
			double ry = (double)row / (pLSH->ui16Height-1);
#ifdef USE_MATH_NEON
			bowly = 1 - (double)cosf_neon((float)(ry-0.5f)*M_PI);	//cos(+/- PI/2)
#else
			bowly = 1-cos((ry-0.5f)*M_PI);	//cos(+/- PI/2)
#endif

			for ( col = 0 ; col < pLSH->ui16Width ; col++ )
			{
				double val,rx;
				rx = (double)col / (pLSH->ui16Width-1);
#ifdef USE_MATH_NEON
				bowlx = 1-(double)cosf_neon((float)(rx-0.5)*M_PI);	//cos(+/- PI/2)
#else
				bowlx = 1-cos((rx-0.5)*M_PI);	//cos(+/- PI/2)
#endif
				bowl = (bowlx+bowly)/2;		//one on top of another, div by 2 to get back to amplitude 0=center,1.0=corners
				val = ((aCorners[0]-aCorners[4])*(1-ry)*(1-rx)
						+ (aCorners[1]-aCorners[4])*(1-ry)*rx
						+ (aCorners[2]-aCorners[4])*ry*(1-rx)
						+ (aCorners[3]-aCorners[4])*ry*rx) * bowl + aCorners[4];
                pLSH->apMatrix[c][row*pLSH->ui16Width + col] = (LSH_FLOAT)val;
			}//for col
		}//for row
	}

	return IMG_SUCCESS;
}

void LSH_Free(LSH_GRID *pLSH)
{
	IMG_UINT32 i;

	IMG_ASSERT(pLSH != NULL);

	for ( i = 0 ; i < LSH_MAT_NO ; i++ )
	{
		if ( pLSH->apMatrix[i] != NULL )
		{
			IMG_FREE(pLSH->apMatrix[i]);
			pLSH->apMatrix[i] = NULL;
		}
	}
}

//
// file operations
//

IMG_RESULT LSH_Save_txt(const LSH_GRID *pLSH, const char *filename)
{
	FILE *f = NULL;

    if ( pLSH == NULL || filename == NULL  )
	{
        LOG_DEBUG("NULL parameters given\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( (f = fopen(filename, "w")) != NULL )
	{
		int c, x, y;

		fprintf(f, "LSH matrix %ux%u, tile size %u, 4 channels\n", pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize);
		for ( c = 0 ; c < LSH_MAT_NO ; c++ )
		{
			fprintf(f, "channel %u\n", c);
			for ( y = 0 ; y < pLSH->ui16Height ; y++ )
			{
				for ( x = 0 ; x < pLSH->ui16Width ; x++ )
				{
					fprintf(f, "%f ", pLSH->apMatrix[c][y*pLSH->ui16Width + x]);
				}
				fprintf(f, "\n");
			}
		}

		fclose(f);
	}
	else
	{
        LOG_ERROR("Failed to open output file %s\n", filename);
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	return IMG_SUCCESS;
}

#define LSH_BIN_HEAD 3 // nb of i32 in the header

IMG_RESULT LSH_Save_bin(const LSH_GRID *pLSH, const char *filename)
{
    FILE *f = NULL;

    if ( pLSH == NULL || filename == NULL  )
	{
        LOG_DEBUG("NULL parameters given\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( (f = fopen(filename, "wb")) != NULL )
	{
		int c, x, y;
        IMG_INT32 sizes[LSH_BIN_HEAD] = { (IMG_INT32)pLSH->ui16Width, (IMG_INT32)pLSH->ui16Height, (IMG_INT32)pLSH->ui16TileSize };

        fwrite(LSH_HEAD, sizeof(char), LSH_HEAD_SIZE, f); // type of file
        c = 1;
        fwrite(&c, sizeof(int), 1, f); // version
        fwrite(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f);
		for ( c = 0 ; c < LSH_MAT_NO ; c++ )
		{
            if( GRID_DOUBLE_CHECK )
            {
                for ( y = 0 ; y < pLSH->ui16Height ; y++ )
                {
                    for ( x = 0 ; x < pLSH->ui16Width ; x++ )
                    {
                        float v = (float)pLSH->apMatrix[c][y*pLSH->ui16Width+x];
                        if ( fwrite(&v, sizeof(float), 1, f) != 1 )
                        {
                            LOG_ERROR("failed to write LSH value c=%d y=%d x=%d\n", c, y, x);
                            fclose(f);
                            return IMG_ERROR_FATAL;
                        }
                    }
                }
            }
            else
            {
                fwrite(pLSH->apMatrix[c], sizeof(LSH_FLOAT), pLSH->ui16Width*pLSH->ui16Height, f);
            }
		}

		fclose(f);
	}
	else
	{
        LOG_ERROR("Failed to open %s\n", filename);
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	return IMG_SUCCESS;
}

IMG_RESULT LSH_Load_bin(LSH_GRID *pLSH, const char *filename)
{
	int ret = 0;
    FILE *f = NULL;

	if ( pLSH == NULL || filename == NULL  )
	{
        LOG_DEBUG("NULL parameter given\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( (f = fopen(filename, "rb")) != NULL )
	{
		int c;
        IMG_UINT32 sizes[LSH_BIN_HEAD];
        char name[4];
        float *buff = NULL; // 1 line buffer used only if compiled with LSH_FLOAT as double

        if ( fread(name, sizeof(char), 4, f) != 4 )
        {
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if ( strncmp(name, LSH_HEAD, LSH_HEAD_SIZE-1) != 0 )
        {
            LOG_ERROR("Wrong LSH file format - %s couldn't be read\n", LSH_HEAD);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        if ( fread(&c, sizeof(int), 1, f) != 1 )
        {
            LOG_ERROR("Failed to read LSH file format\n");
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if ( c != 1 )
        {
            LOG_ERROR("wrong LSH file format - version %d found, version %d supported\n", c, LSH_VERSION);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }

		if ( fread(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f) != LSH_BIN_HEAD )
		{
            LOG_ERROR("failed to read %d int at the beginning of the file\n", LSH_BIN_HEAD);
			fclose(f);
			return IMG_ERROR_FATAL;
		}

        pLSH->ui16Width = (IMG_UINT16)sizes[0];
        pLSH->ui16Height = (IMG_UINT16)sizes[1];
        pLSH->ui16TileSize = (IMG_UINT16)sizes[2];

        if ( pLSH->ui16Width*pLSH->ui16Height == 0 )
        {
            LOG_ERROR("lsh grid size is 0 (w=%d h=%d)!\n", sizes[0], sizes[1]);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        if ( GRID_DOUBLE_CHECK )
        {
            buff = (float*)IMG_MALLOC(pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
            if ( buff == NULL )
            {
                LOG_ERROR("failed to allocate the output matrix\n");
                fclose(f);
                return IMG_ERROR_MALLOC_FAILED;
            }
        }

        for ( c = 0 ; c < LSH_MAT_NO ; c++ )
	    {

		    pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));

		    if ( pLSH->apMatrix[c] == NULL )
		    {
                LOG_ERROR("Failed to allocate matrix for channel %d (%ld Bytes)\n", c, pLSH->ui16Width*pLSH->ui16Height*sizeof(LSH_FLOAT));
			    c--;
			    while (c>=0)
			    {
				    IMG_FREE(pLSH->apMatrix[c]);
				    c--;
			    }
                if ( GRID_DOUBLE_CHECK ) IMG_FREE(buff);

			    return IMG_ERROR_MALLOC_FAILED;
		    }
	    }

        for ( c = 0 ; c < LSH_MAT_NO ; c++ )
		{
            if ( GRID_DOUBLE_CHECK )
            {
                int x, y;

                 // matrix is double! so load in buff
                if ( (ret=fread(buff, sizeof(float), pLSH->ui16Width*pLSH->ui16Height, f)) != pLSH->ui16Width*pLSH->ui16Height )
                {
                    LOG_ERROR("Failed to read channel %d - read %d/%lu Bytes\n", c, ret, pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
                    IMG_FREE(buff);
                    fclose(f);
                    return IMG_ERROR_FATAL;
                }
                for ( y = 0 ; y < pLSH->ui16Height ; y++ )
                {
                    for ( x = 0 ; x < pLSH->ui16Width ; x++ )
                    {
                        pLSH->apMatrix[c][y*pLSH->ui16Width+x] = (LSH_FLOAT)buff[y*pLSH->ui16Width+x];
                    }
                }
            }
            else
            {
                if ( (ret=fread(pLSH->apMatrix[c], sizeof(float), pLSH->ui16Width*pLSH->ui16Height, f)) != pLSH->ui16Width*pLSH->ui16Height )
                {
                    LOG_ERROR("Failed to read channel %d - read %d/%lu Bytes\n", c, ret, pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
                    fclose(f);
                    return IMG_ERROR_FATAL;
                }
            }
		}

        if (GRID_DOUBLE_CHECK) IMG_FREE(buff);

		fclose(f);
	}
	else
	{
        LOG_ERROR("Failed to open file %s\n", filename);
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	return IMG_SUCCESS;
}

#undef LSH_BIN_HEAD

IMG_RESULT LSH_Save_pgm(const LSH_GRID *pLSH, const char *filename)
{
	FILE *f = NULL;

    if ( pLSH == NULL || filename == NULL )
    {
        LOG_DEBUG("NULL parameters\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

	if ( (f = fopen(filename, "w")) != NULL )
	{
		int c, x, y;
		double min = 3.0, max = 0.0;

		for ( c = 0 ; c < LSH_MAT_NO ; c++ )
		{
			for ( y = 0 ; y < pLSH->ui16Height ; y++ )
			{
				for ( x = 0 ; x < pLSH->ui16Width ; x++ )
				{
					if ( pLSH->apMatrix[c][y*pLSH->ui16Width + x] < min )
					{
						min = pLSH->apMatrix[c][y*pLSH->ui16Width + x];
					}
					if ( pLSH->apMatrix[c][y*pLSH->ui16Width + x] > max )
					{
						max = pLSH->apMatrix[c][y*pLSH->ui16Width + x];
					}
				}
			}
		}
		if ( min == max )
		{
			max+=1.0;
		}

		fprintf(f, "P2\n%u %u 255\n", pLSH->ui16Width, pLSH->ui16Height*4);
		for ( c = 0 ; c < LSH_MAT_NO ; c++ )
		{
			for ( y = 0 ; y < pLSH->ui16Height ; y++ )
			{
				for ( x = 0 ; x < pLSH->ui16Width ; x++ )
				{
					int v = (int)( ((pLSH->apMatrix[c][y*pLSH->ui16Width + x])-min)/(max-min) )*255;
					v = IMG_MIN_INT(v, 255);
					fprintf(f, "%d ", v);
				}
				fprintf(f, "\n");
			}
		}

		fclose(f);
	}
    else
    {
        LOG_ERROR("Failed to open %s\n", filename);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}
