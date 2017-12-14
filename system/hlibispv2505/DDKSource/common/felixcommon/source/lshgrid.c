/**
*******************************************************************************
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
#include "img_types.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES  // for win32 M_PI definition
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
#ifdef INFOTM_LSH_FUNC

#ifdef OTP_DBG_LOG
#define LSH_DBG_LOG
#endif

#ifdef OUTSIDE_RING_WB_CALI
#define SKIP_OTP_LSH_FOR_DEBUG
#endif

static int BIG_RING_R = 47;

static int SMALL_RING_R = 24;

static int MAX_RADIUS = 48;

static float s_center_rggb_L[4] = {1.0, 1.0, 1.0, 1.0};
static float s_center_rggb_R[4] = {1.0, 1.0, 1.0, 1.0};

static float s_big_rggb_L[4][RING_SECTOR_CNT] = {2.4};
static float s_small_rggb_L[4][RING_SECTOR_CNT] = {1.4};

static float s_big_rggb_R[4][RING_SECTOR_CNT] = {2.4};
static float s_small_rggb_R[4][RING_SECTOR_CNT] = {1.4};

//rgb min/max
static unsigned char s_limit_rgb[2][3][2] = {0};

//static float s_linear_curve[RING_CNT] = {0, 0.15, 0.30, 0.39, 0.47, 0.54, 0.60, 0.66, 0.71, 0.76, 0.81,0.86,0.90,0.93,0.95,0.96,0.97, 0.99, 1.0};

static float sim_s_linear_curve_L[4][RING_CNT] = 
{
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0}
};

static float sim_s_linear_curve_R[4][RING_CNT] = 
{
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0},
	{0, 0.15, 0.30, 0.42, 0.52, 0.61, 0.69, 0.76, 0.82, 0.87, 0.91, 0.94, 0.96, 0.97, 0.98, 0.985, 0.99, 0.995, 1.0}
};

#if 0
static float s_linear_curve_L[4][RING_CNT] = 
{
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
};

static float s_linear_curve_R[4][RING_CNT] = 
{
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
	{0, 2/19, 3.6/19, 5.4/19, 6.9/19, 8.3/19, 9.7/19, 11/19, 12.2/19, 13.4/19, 14.5/19, 15.5/19, 16.4/19, 17/19, 17.5/19, 18/19, 18.4/19, 18.8/19, 1.0},
};
#elif 0
static float s_linear_curve_L[4][RING_CNT] = 
{
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
};

static float s_linear_curve_R[4][RING_CNT] = 
{
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
	{0, 1/19, 2/19, 3/19, 4/19, 5/19, 6/19, 7/19, 8/19, 9/19, 10/19, 11/19, 12/19, 13/19, 14/19, 15/19, 16/19, 17/19, 18/19},
};
#elif 0
static float s_linear_curve_L[4][RING_CNT] = 
{
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
};

static float s_linear_curve_R[4][RING_CNT] = 
{
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.18, 0.33, 0.45, 0.56, 0.66, 0.75, 0.83, 0.88, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
};
#elif 0
static float s_linear_curve_L[4][RING_CNT] = 
{
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0}
};

static float s_linear_curve_R[4][RING_CNT] = 
{
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0},
	{0, 0.195, 0.342, 0.46, 0.566, 0.664, 0.753, 0.832, 0.881, 0.92, 0.95, 0.97, 0.98, 0.985, 0.989, 0.993, 0.996, 0.998, 1.0}
};
#else
static float s_linear_curve_L[4][RING_CNT] = 
{
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0}
};

static float s_linear_curve_R[4][RING_CNT] = 
{
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0},
	{0, 0.20, 0.35, 0.45, 0.54, 0.61, 0.67, 0.72, 0.76, 0.79, 0.81, 0.84, 0.87, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0}
};
#endif

static float s_rggb_linear_data_L[4][RING_CNT][RING_SECTOR_CNT] = {0};
static float s_rggb_linear_data_R[4][RING_CNT][RING_SECTOR_CNT] = {0};

static float s_ring_position_L[RING_CNT][RING_SECTOR_CNT][2] = {0};
static float s_ring_position_R[RING_CNT][RING_SECTOR_CNT][2] = {0};

static float s_radius[RING_CNT] = {MAX_R, MAX_R*(RING_CNT-1)/RING_CNT, MAX_R*(RING_CNT-2)/RING_CNT, MAX_R*(RING_CNT-3)/RING_CNT, MAX_R*(RING_CNT-4)/RING_CNT, MAX_R*(RING_CNT-5)/RING_CNT, MAX_R*(RING_CNT-6)/RING_CNT, MAX_R*(RING_CNT-7)/RING_CNT, MAX_R*(RING_CNT-8)/RING_CNT, MAX_R*(RING_CNT-9)/RING_CNT, MAX_R*(RING_CNT-10)/RING_CNT, MAX_R*(RING_CNT-11)/RING_CNT, MAX_R*(RING_CNT-12)/RING_CNT, MAX_R*(RING_CNT-13)/RING_CNT, MAX_R*(RING_CNT-14)/RING_CNT, MAX_R*(RING_CNT-15)/RING_CNT, MAX_R*(RING_CNT-16)/RING_CNT, MAX_R*(RING_CNT-17)/RING_CNT, MAX_R*(RING_CNT-18)/RING_CNT};

// degree split: 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5
//const g_radius_cos_table[RING_CNT] = {0.923879533, 0.707106781, 0.382683432, 0, -0.382683432, -0.707106781, -0.923879533, -1, -0.923879533, -0.707106781, -0.382683432, 0, 0.382683432, 0.707106781, 0.923879533};

// degree split: 180, 157.5, 135, 112.5, 90, 67.5, 45, 22.5, 0, 337.5, 315, 292.5, 270, 247.5, 225, 202.5
const g_radius_cos_table[RING_SECTOR_CNT] = {-1, -0.923879533, -0.707106781, -0.382683432, 0, 0.382683432, 0.707106781, 0.923879533, 1, 0.923879533, 0.707106781, 0.382683432, 0, -0.382683432, -0.707106781, -0.923879533};
const g_radius_sin_table[RING_SECTOR_CNT] = {0, 0.382683432, 0.707106781, 0.923879533, 1, 0.923879533, 0.707106781, 0.382683432, 0, -0.382683432, -0.707106781, -0.923879533, -1, -0.923879533, -0.707106781, -0.382683432};


#define GRID_DOUBLE_CHECK  1//( sizeof(LSH_FLOAT) > sizeof(float) )

int LSH_GetSectorIndex(int x, int y, int offsetX, int offsetY)
{
	int nSectorIndex = 0;
    float y_x_ratio = 0;
	
	if (x < (MAX_RADIUS + offsetX))
	{// left half circle
		if (y <= (MAX_RADIUS + offsetY))
		{// top
		    y_x_ratio = (float)((MAX_RADIUS + offsetY) - y)/(MAX_RADIUS + offsetX - x);
                    
			if (y_x_ratio < 0.75)
			{
				nSectorIndex = 0;
			}
			else if (y_x_ratio < 1.0)
			{
				nSectorIndex = 1;
			}
			else if (y_x_ratio < 1.3334)
			{
				nSectorIndex = 2;
			}
			else
			{
				nSectorIndex = 3;
			}
		}
		else
		{// bottm
		    y_x_ratio = (float)(y - (MAX_RADIUS + offsetY))/(MAX_RADIUS + offsetX - x);
            
		    // printf("@@LSHbot[%f]\n",y_x_ratio);
			if (y_x_ratio < 0.75)
			{
				nSectorIndex = 15;

			}
			else if (y_x_ratio < 1.0)
			{
				nSectorIndex = 14;
			}
			else if (y_x_ratio < 1.3334)
			{
				nSectorIndex = 13;
			}
			else
			{
				nSectorIndex = 12;
			}
		}
	}
	else
	{// right half circle
		if (x == (MAX_RADIUS + offsetX))
		{
			if (y <= (MAX_RADIUS + offsetY))
			{// top
				nSectorIndex = 4;
			}
			else
			{// bottm
				nSectorIndex = 12; // ***
			}
            
            return nSectorIndex;
		}
		
		if (y <= (MAX_RADIUS + offsetY))
		{// top  
		    y_x_ratio = (float)((MAX_RADIUS + offsetY) - y)/(x - (MAX_RADIUS + offsetX));
                    
			if (y_x_ratio < 0.75)
			{
				nSectorIndex = 7;
			}
			else if (y_x_ratio < 1.0)
			{
				nSectorIndex = 6;
			}
			else if (y_x_ratio < 1.3334)
			{
				nSectorIndex = 5;
			}
			else
			{
				nSectorIndex = 4;
			}
		}
		else
		{// bottm
		    y_x_ratio = (float)(y - (MAX_RADIUS + offsetY))/(x - (MAX_RADIUS + offsetX));
            
			if (y_x_ratio < 0.75)
			{
				nSectorIndex = 8;
			}
			else if (y_x_ratio < 1.0)
			{
				nSectorIndex = 9;
			}
			else if (y_x_ratio < 1.3334)
			{
				nSectorIndex = 10;
			}
			else
			{
				nSectorIndex = 11;
			}						
		}
	}
	return nSectorIndex;
}

float LSH_GetGainValue(float Radius, int x, int y, int IsLeftRing, int MatixIndex, OTP_CALIBRATION_DATA* calibration_data)
{
	int m = 0, n = 0, nSectorIndex = 0;
	float AbsVal = 0.0;
	float Ret = -1.0;
	//float angleX, angleY;
	int offsetX, offsetY;

	if (!IsLeftRing)
	{
		x -= Radius;
	}

	if (IsLeftRing)
	{
		offsetX = calibration_data->radius_center_offset_L[0];
		offsetY = calibration_data->radius_center_offset_L[1];
	}
	else
	{
		offsetX = calibration_data->radius_center_offset_R[0];
		offsetY = calibration_data->radius_center_offset_R[1];
	}
	
	for (m = 0; m < RING_CNT; m++)
	{
		if (Radius > s_radius[m])
		{			
			if (m == 0) // concer area
			{
				if (Radius <= MAX_RADIUS && BIG_RING_R < MAX_RADIUS && s_radius[m] > BIG_RING_R)
				{
					nSectorIndex = LSH_GetSectorIndex(x, y, offsetX, offsetY);
					if (IsLeftRing)
					{
						Ret = s_rggb_linear_data_L[MatixIndex][0][nSectorIndex] + s_rggb_linear_data_L[MatixIndex][0][nSectorIndex]*(MAX_RADIUS - Radius)/(MAX_RADIUS);
					}
					else
					{	
						Ret = s_rggb_linear_data_R[MatixIndex][0][nSectorIndex] + s_rggb_linear_data_R[MatixIndex][0][nSectorIndex]*(MAX_RADIUS - Radius)/(MAX_RADIUS);
					}
					return Ret;
				}

				if (x <= Radius)
				{
					if (y <= Radius)
					{
						nSectorIndex = 1;
					}
					else
					{
						nSectorIndex = 14;
					}
				}
				else
				{
					if (y <= Radius)
					{
						nSectorIndex = 5;

					}
					else
					{
						nSectorIndex = 9;
					}						
				}
				
				AbsVal = Radius - s_radius[0];
				
				if (IsLeftRing)
				{
					Ret = s_rggb_linear_data_L[MatixIndex][0][nSectorIndex] - AbsVal/6;
				}
				else
				{	
					Ret = s_rggb_linear_data_R[MatixIndex][0][nSectorIndex] - AbsVal/6;				
				}

				//printf("==>concer %f\n", Ret);

				if (Ret < 1.0)
				{
					Ret = 1.0;
				}
				
				return Ret;
			}
			
			nSectorIndex = RING_SECTOR_CNT - 1;
#if 0
			if (IsLeftRing)
			{
				for (n = 0; n < RING_SECTOR_CNT - 1; n++)
				{
					if ((x >= s_ring_position_L[m-1][n][0] && x <= s_ring_position_L[m][n+1][0]) && (y >= s_ring_position_L[m][n][1] && y <= s_ring_position_L[m-1][n+1][1]))
					{
						nSectorIndex = n;
						break;
					}
				}
			
				Ret = s_rggb_linear_data_L[MatixIndex][m-1][nSectorIndex] - (s_rggb_linear_data_L[MatixIndex][m-1][nSectorIndex] - s_rggb_linear_data_L[MatixIndex][m][nSectorIndex])*(s_radius[m-1] - Radius)/(s_radius[m-1] - s_radius[m]);
			}
			else
			{	
				for (n = 0; n < RING_SECTOR_CNT - 1; n++)
				{
					if ((x >= s_ring_position_R[m-1][n][0] && x <= s_ring_position_R[m][n+1][0]) && (y >= s_ring_position_R[m][n][1] && y <= s_ring_position_R[m-1][n+1][1]))
					{
						nSectorIndex = n;
						break;
					}
				}

				Ret = s_rggb_linear_data_R[MatixIndex][m-1][nSectorIndex] - (s_rggb_linear_data_R[MatixIndex][m-1][nSectorIndex] - s_rggb_linear_data_R[MatixIndex][m][nSectorIndex])*(s_radius[m-1] - Radius)/(s_radius[m-1] - s_radius[m]);
			}
#else
			//angleX = (x - (Radius + offsetX))/Radius;
			//angleY = (y - (Radius + offsetY))/Radius;

			nSectorIndex = LSH_GetSectorIndex(x, y, offsetX, offsetY);

			if (IsLeftRing)
			{
				Ret = s_rggb_linear_data_L[MatixIndex][m-1][nSectorIndex] - (s_rggb_linear_data_L[MatixIndex][m-1][nSectorIndex] - s_rggb_linear_data_L[MatixIndex][m][nSectorIndex])*(s_radius[m-1] - Radius)/(s_radius[m-1] - s_radius[m]);
			}
			else
			{	
				Ret = s_rggb_linear_data_R[MatixIndex][m-1][nSectorIndex] - (s_rggb_linear_data_R[MatixIndex][m-1][nSectorIndex] - s_rggb_linear_data_R[MatixIndex][m][nSectorIndex])*(s_radius[m-1] - Radius)/(s_radius[m-1] - s_radius[m]);
			}
#endif

/*
			printf("===>L[%d][%d][%d]=%f,L[%d][%d][%d]=%f, R=%f, s_radius[%d]=%f, s_radius[%d]=%f, Ret = %f\n", 
                   MatixIndex, m-1, nSectorIndex, s_rggb_linear_data_L[MatixIndex][m-1][nSectorIndex], 
                   MatixIndex, m, nSectorIndex, s_rggb_linear_data_L[MatixIndex][m][nSectorIndex], 
                   Radius, m-1, s_radius[m-1], m, s_radius[m], Ret);
*/			
			if (Ret < 1.0)
			{
				Ret = 1.0;
			}
			
			return Ret;
		}
		else
		{
			if (m == RING_CNT - 1)
			{
				AbsVal = s_radius[RING_CNT - 1] - Radius;
				
				if (IsLeftRing)
				{
					Ret = s_rggb_linear_data_L[MatixIndex][m][nSectorIndex] - AbsVal/10;
					if (Ret < s_center_rggb_L[MatixIndex])
					{
						Ret = s_center_rggb_L[MatixIndex];
					}
				}
				else
				{
					Ret = s_rggb_linear_data_R[MatixIndex][m][nSectorIndex] - AbsVal/10;	
					if (Ret < s_center_rggb_R[MatixIndex])
					{
						Ret = s_center_rggb_R[MatixIndex];
					}
				}
				
				return Ret;
			}
		}
	}
	
	return Ret;
}

#ifdef INFOTM_E2PROM_METHOD
float LSH_E2prom_GetGainValue(float Radius, int x, int y, int IsLeftRing, int MatixIndex, 
									  OTP_CALIBRATION_DATA* calibration_data, E2PROM_VERSION_1* e2prom_data, float *linear_dataL, float *linear_dataR, float* radius_val)
{
	int m = 0, n = 0, index = 0, nSectorIndex = 0, ring_cnt = e2prom_data->data[0]->ring_cnt, setctor_cnt = RING_SECTOR_CNT;
	float AbsVal = 0.0;
	float Ret = -1.0;
	int offsetX, offsetY;
	float *linear_data = linear_dataL;
       float  temp1,temp2;
	if (!IsLeftRing)
	{
		//x -= Radius;
		x -=2*MAX_RADIUS;
		index = 1;
		ring_cnt = e2prom_data->data[1]->ring_cnt;
		linear_data = linear_dataR;
        offsetX = calibration_data->radius_center_offset_R[0];
        offsetY = calibration_data->radius_center_offset_R[1];
	}
    else
    {
        offsetX = calibration_data->radius_center_offset_L[0];
        offsetY = calibration_data->radius_center_offset_L[1];
    }
    
	//printf("==>offsetX %d, offsetY %d, linear_data %x, Radius %f, ring_cnt %d\n", offsetX, offsetY, linear_data, Radius, ring_cnt);

    
	for (m = 0; m < ring_cnt; m++)
	{
		if (Radius >= radius_val[m])
		{			
			if (m == 0) // concer area
			{
				if (Radius <= MAX_RADIUS && BIG_RING_R < MAX_RADIUS && radius_val[m] > BIG_RING_R)
				{
					nSectorIndex = LSH_GetSectorIndex(x, y, offsetX, offsetY);

					Ret = linear_data[4*(ring_cnt-1)*setctor_cnt + nSectorIndex*4 + MatixIndex] + linear_data[4*(ring_cnt-1)*setctor_cnt + nSectorIndex*4 + MatixIndex]*(MAX_RADIUS - Radius)/(MAX_RADIUS);
					return Ret;
				}

				if (x <= Radius)
				{
					if (y <= Radius)
					{
						nSectorIndex = 1;
					}
					else
					{
						nSectorIndex = 14;
					}
				}
				else
				{
					if (y <= Radius)
					{
						nSectorIndex = 5;

					}
					else
					{
						nSectorIndex = 9;
					}						
				}
				
				AbsVal = Radius - radius_val[0];
                
				//Ret = linear_data[4*(ring_cnt-1)*setctor_cnt + nSectorIndex*4 + MatixIndex] - AbsVal/10;
				Ret = linear_data[4*(ring_cnt-1)*setctor_cnt + nSectorIndex*4 + MatixIndex]*radius_val[0]/Radius;

				if (Ret < 1.0/*1.35*/)
				{
					Ret = 1.0;
				}

                //printf("R %f (%d, %d) Coner<%d, %f>\n", Radius, x, y, nSectorIndex, Ret);
                
				return Ret;
			}
			     
			nSectorIndex = LSH_GetSectorIndex(x, y, offsetX, offsetY);

            Ret = linear_data[4*(ring_cnt-m-1)*setctor_cnt + nSectorIndex*4 + MatixIndex] + 
                  (linear_data[4*(ring_cnt-m)*setctor_cnt + nSectorIndex*4 + MatixIndex] - 
                   linear_data[4*(ring_cnt-m-1)*setctor_cnt + nSectorIndex*4 + MatixIndex])*(Radius-radius_val[m])/(radius_val[m-1] - radius_val[m]);        
            
			if (Ret <= 1.0)
			{
//			    printf("\n==>min m %d, Radius %f, Radius Range<%f, %f>, SectorIndex %d, MatixIndex %d, range[%f, %f]\n", 
//                        m, Radius, radius_val[m-1], radius_val[m], nSectorIndex, MatixIndex, 
//                        linear_data[4*(ring_cnt-m-1)*setctor_cnt + nSectorIndex*4 + MatixIndex], 
//                        linear_data[4*(ring_cnt-m)*setctor_cnt + nSectorIndex*4 + MatixIndex]);
	
				Ret = 1.0;
			}
	             
			//printf("R %f (%d, %d)<%d, %f>\n", Radius, x, y, nSectorIndex, Ret);	  
			return Ret;
		}
		else
		{
		    nSectorIndex = LSH_GetSectorIndex(x, y, offsetX, offsetY);
		    Ret = linear_data[nSectorIndex*4 + MatixIndex];
/*		
			if (m == ring_cnt - 1)
			{
				AbsVal = radius_val[ring_cnt - 1] - Radius;

				Ret = linear_data[4*(ring_cnt-m-1)*setctor_cnt + nSectorIndex*4 + MatixIndex] - AbsVal/10;
                
				if (Ret < 1.0)
				{
					Ret = 1.0;
				}
				return Ret;
			}
*/
		}
	}
	
	return Ret;
}

#endif

/////////////////////////////////
#ifdef INFOTM_LSH_CURVE_FITTING

//#include <stdio.h>
//#include <conio.h>
#include <math.h>
//#include <process.h>

#define N 8 // samples cnt
#define DOUBLE_N (N)*2 // samples cnt
#define T 4 // n
#define W 1 // quan

#define PRECISION 0.00001

static float curve_fitting_param[2][4][T+1] = {0};
static float samples_x[N] = {0, 48/7, 48*2/7, 48*3/7, 48*4/7, 48*5/7, 48*6/7, 48 - 1};

float pow_n(float a,int n)
{
	int i;
	float res=a;
	
	if (n==0)
	{
		return(1);
	}
	
	for (i=1;i<n;i++)
	{
		res *= a;
	}
	
	return(res);
}

void mutiple(float a[][N],float b[][T+1],float c[][T+1])
{
	float res=0;
	int i,j,k;
	for (i=0;i<T+1;i++)
	{
		for (j=0;j<T+1;j++)
		{
			res=0;
			for (k=0;k<N;k++)
			{
				res+=a[i][k]*b[k][j];
				c[i][j]=res;
			}
		}
	}
}

void matrix_trans(float a[][T+1],float b[][N])
{
	int i,j;
	for (i=0;i<N;i++)
	{
		for (j=0;j<T+1;j++)
		{
			b[j][i]=a[i][j];
		}
	}
}

void get_A(float matrix_A[][T+1],float x_y[][2],int n)
{
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<T+1;j++)
		{
			matrix_A[i][j] = W*pow_n(x_y[i][0],j);
		}
	}
}

void convert(float argu[][T+2],int n)
{
	int i,j,k,p,t;
	float rate,temp;
	for (i=1;i<n;i++)
	{
		for (j=i;j<n;j++)
		{
			if (argu[i-1][i-1]==0)
			{
				for (p=i;p<n;p++)
				{
					if (argu[p][i-1]!=0)
					{
						break;
					}
				
					if (p==n)
					{
						printf("invalid p==n\n");
						return;
					}
				
					for (t=0;t<n+1;t++)
					{
						temp = argu[i-1][t];
						argu[i-1][t] = argu[p][t];
						argu[p][t] = temp;
					}
				}
					
				rate = argu[j][i-1]/argu[i-1][i-1];
				
				for (k=i-1;k<n+1;k++)
				{
					argu[j][k] -= argu[i-1][k]*rate;
					if (fabs(argu[j][k]) <= PRECISION)
					{
						argu[j][k]=0;
					}
				}		
			}
		}
	}
}

void compute(float argu[][T+2],int n,float root[])
{
	int i,j;
	float temp;
	for (i=n-1;i>=0;i--)
	{
		temp = argu[i][n];
		
		for (j=n-1;j>i;j--)
		{
			temp -= argu[i][j]*root[j];
		}
		
		root[i] = temp/argu[i][i];
	}
}
void get_y(float trans_A[][N],float x_y[][2],float y[],int n)
{
	int i,j;
	float temp;
	
	for (i=0;i<n;i++)
	{
		temp = 0;
		
		for (j=0;j<N;j++)
		{
			temp += trans_A[i][j]*x_y[j][1];
		}
		
		y[i] = temp;
	}
}
void cons_formula(float coef_A[][T+1],float y[],float coef_form[][T+2])
{
	int i,j;
	for (i=0;i<T+1;i++)
	{
		for (j=0;j<T+2;j++)
		{
			if (j==T+1)
			{
				coef_form[i][j]=y[i];
			}
			else
			{
				coef_form[i][j]=coef_A[i][j];
			}
		}
	}
}

void calc_curfitting_param(int index, float avg_rgb_ring[][4])
{
	int i = 0, j = 0;
	float x_y[N][2];
	float matrix_A[N][T+1], trans_A[T+1][N], coef_A[T+1][T+1], coef_formu[T+1][T+2], y[T+1], a[T+1];
		
	for (i = 0; i < 4; i++)
	{	
		if (i == 2)
		{
			memcpy(curve_fitting_param[index][i], 
				   curve_fitting_param[index][i-1], 
				   sizeof(curve_fitting_param[index][0][0])*(T+1));
#ifdef LSH_DBG_LOG
			printf("==>sensor %d RGB[%d]\n", index, i);
			for (j = 0; j < T+1; j++)
			{
				printf("%f ", curve_fitting_param[index][i][j]);
			}
			printf("====\n");
#endif

			continue;
		}
		
		for (j = 0; j < N; j++)
		{
			x_y[j][0] = samples_x[j];
			x_y[j][1] = (float)avg_rgb_ring[j][i];
#ifdef LSH_DBG_LOG
			printf("===>x_y[%d][0] = %f,x_y[%d][1] = %f\n", j, x_y[j][0], j, x_y[j][1]);
#endif
		}
		
		get_A(matrix_A, x_y, N);

		matrix_trans(matrix_A, trans_A);
		mutiple(trans_A, matrix_A, coef_A);

		get_y(trans_A, x_y, y, T+1);

		cons_formula(coef_A, y, coef_formu);

		convert(coef_formu, T+1);

		compute(coef_formu, T+1, curve_fitting_param[index][i]);

#ifdef LSH_DBG_LOG
		printf("==>sensor %d RGB[%d]\n", index, i);
		for (j = 0; j < T+1; j++)
		{
			printf("%f ", curve_fitting_param[index][i][j]);
		}
		printf("====\n");
#endif

	}
}

void calc_linear_curve(void)
{
	int i, j, k, l;
	double tmp = 0, total_L[4] = {0}, total_R[4] = {0}, tmpL = 0, tmpR = 0;
	
#ifdef LSH_DBG_LOG
	printf("=====>curve fitting 0\n");
#endif

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < RING_CNT; j++)
		{
			s_linear_curve_L[i][j] = curve_fitting_param[0][i][0];
			for (k = 1; k<(T+1); k++)
			{
				tmp = curve_fitting_param[0][i][k];
				for (l = 0; l < k; l++)
				{
					tmp *= (j*BIG_RING_R/RING_CNT);
				}
				s_linear_curve_L[i][j] += tmp;
			}
			
			total_L[i] += s_linear_curve_L[i][j];
			
#ifdef LSH_DBG_LOG
			printf("=====>s_linear_curve_L[%d][%d]=%f\n", i, j, s_linear_curve_L[i][j]);
#endif
		}
	}

#ifdef LSH_DBG_LOG
	printf("=====>curve fitting 1\n");
#endif

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < RING_CNT; j++)
		{
			s_linear_curve_R[i][j] = curve_fitting_param[1][i][0];
			for (k = 1; k<(T+1); k++)
			{
				tmp = curve_fitting_param[1][i][k];
				for (l = 0; l < k; l++)
				{
					tmp *= (j*BIG_RING_R/RING_CNT);
				}
				
				s_linear_curve_R[i][j] += tmp;
			}

			total_R[i] += s_linear_curve_R[i][j];
			
#ifdef LSH_DBG_LOG
			printf("=====>s_linear_curve_R[%d][%d]=%f\n", i, j, s_linear_curve_R[i][j]);
#endif
		}
	}

	////////////////////////////
	float tmp_linear_curve_L[RING_CNT];
	float tmp_linear_curve_R[RING_CNT];

	for (i = 0; i < 4; i++)
	{
		tmpL = 0;
		tmpR = 0;
		
		for (j = 0; j < RING_CNT; j++)
		{	
			tmpL += s_linear_curve_L[i][j];
			tmp_linear_curve_L[j] = 1.0 - tmpL/total_L[i];

			tmpR += s_linear_curve_R[i][j];
			tmp_linear_curve_R[j] = 1.0 - tmpR/total_R[i];
		}

//		{
//			tmp_linear_curve_L[RING_CNT-1] = 1.0;
//			tmp_linear_curve_R[RING_CNT-1] = 1.0;
//		}
		
		for (j = 0; j < RING_CNT; j++)
		{
			s_linear_curve_L[i][j] = tmp_linear_curve_L[RING_CNT - j - 1];
			s_linear_curve_R[i][j] = tmp_linear_curve_R[RING_CNT - j - 1];
		}

		s_linear_curve_L[i][RING_CNT - 1] = 1.0;
		s_linear_curve_R[i][RING_CNT - 1] = 1.0;
		

#ifdef LSH_DBG_LOG		
		for (j = 0; j < RING_CNT; j++)
		{
			printf("=====>new s_linear_curve_L[%d][%d]=%f, s_linear_curve_R[%d][%d]=%f\n", 
					i, j, s_linear_curve_L[i][j], i, j, s_linear_curve_R[i][j]);

		}
#endif
	}

	////////////////////////////
}

#endif
/////////////////////////////////

#else
#define GRID_DOUBLE_CHECK  ( sizeof(LSH_FLOAT) > sizeof(float) )
#endif

IMG_RESULT LSH_CreateMatrix(LSH_GRID *pLSH, IMG_UINT16 ui16Width,
    IMG_UINT16 ui16Height, IMG_UINT16 ui16TileSize)
{
    IMG_UINT16 uiGridWidth;
    IMG_UINT16 uiGridHeight;

    if (pLSH->apMatrix[0] != NULL)
    {
        LOG_DEBUG("Matrix already allocated\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }
    // tile size power of 2 checked by LSH_AllocateMatrix()

    uiGridWidth = ui16Width / ui16TileSize;  // computer number of tiles
    uiGridHeight = ui16Height / ui16TileSize;

    if (ui16Width%ui16TileSize != 0)
    {
        uiGridWidth++;
    }
    if (ui16Height%ui16TileSize != 0)
    {
        uiGridHeight++;
    }
    uiGridWidth++;  // add the last cell - it is a bilinear interpolation
    uiGridHeight++;

    return LSH_AllocateMatrix(pLSH, uiGridWidth, uiGridHeight, ui16TileSize);
}

IMG_RESULT LSH_AllocateMatrix(LSH_GRID *pLSH, IMG_UINT16 uiGridWidth,
    IMG_UINT16 ui16GridHeight, IMG_UINT16 ui16TileSize)
{
    IMG_INT32 c;

    /*if ( ui16TileSize < LSH_TILE_MIN || ui16TileSize > LSH_TILE_MAX )
    {
    return IMG_ERROR_INVALID_PARAMETERS;
    }*/

    if (pLSH->apMatrix[0] != NULL)
    {
        LOG_DEBUG("Matrix already allocated\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }
    {
        IMG_UINT32 tmp = ui16TileSize;
        IMG_UINT32 log2Res = 0;
        while (tmp >>= 1)  // log2
        {
            log2Res++;
        }

        if (ui16TileSize != (1 << log2Res))
        {
            LOG_DEBUG("ui16TileSize has to be a power of 2 (%u given)\n",
                ui16TileSize);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }
    pLSH->ui16Width = uiGridWidth;
    pLSH->ui16Height = ui16GridHeight;
    pLSH->ui16TileSize = ui16TileSize;

    for (c = 0; c < LSH_MAT_NO; c++)
    {
        pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(
            pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));
        if (pLSH->apMatrix[c] == NULL)
        {
            c--;
            while (c >= 0)
            {
                IMG_FREE(pLSH->apMatrix[c]);
                pLSH->apMatrix[c] = NULL;
                c--;
            }
            LOG_ERROR("failed to allocate matrix\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
    }
    return IMG_SUCCESS;
}

IMG_RESULT LSH_FillLinear(LSH_GRID *pLSH, IMG_UINT8 c,
    const float aCorners[4])
{
    IMG_UINT32 row, col;

    // for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        if (pLSH->apMatrix[c] == NULL)
        {
            LOG_DEBUG("The matrix %d is not allocated\n", c);
            return IMG_ERROR_FATAL;
        }
    }
    if (pLSH->ui16Width <= 1 || pLSH->ui16Height <= 1)
    {
        LOG_DEBUG("Size is too small\n");
        return IMG_ERROR_FATAL;
    }

    LOG_INFO("generate linear LSH %ux%u grid (tile size is %u CFA) "\
        "- corners %f %f %f %f\n",
        pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize,
        aCorners[0], aCorners[1], aCorners[2], aCorners[3]);

    // for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        for (row = 0; row < pLSH->ui16Height; row++)
        {
            // ratio row
            double rr = row / (LSH_FLOAT)(pLSH->ui16Height - 1);

            for (col = 0; col < pLSH->ui16Width; col++)
            {
                // ratio column
                double rc = col / (LSH_FLOAT)(pLSH->ui16Width - 1);
                double val = (aCorners[0])*(1.0 - rr)*(1.0 - rc) +
                    (aCorners[1])*(1.0 - rr)*rc +
                    (aCorners[2])*rr*(1.0 - rc) +
                    (aCorners[3])*rr*rc;

                pLSH->apMatrix[c][row*pLSH->ui16Width + col] = (LSH_FLOAT)val;
            }  // for col
        }  // for row
    }

    return IMG_SUCCESS;
}

IMG_RESULT LSH_FillBowl(LSH_GRID *pLSH, IMG_UINT8 c, const float aCorners[5])
{
    IMG_UINT32 row, col;
    double bowlx, bowly, bowl;

    // for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        if (pLSH->apMatrix[c] == NULL)
        {
            LOG_DEBUG("The matrix %d is not allocated\n", c);
            return IMG_ERROR_FATAL;
        }
    }
    if (pLSH->ui16Width <= 1 || pLSH->ui16Width <= 1)
    {
        LOG_DEBUG("Size is too small\n");
        return IMG_ERROR_FATAL;
    }

    LOG_INFO("generate bowl LSH %ux%u grid (tile size is %u CFA) "\
        "- corners %f %f %f %f center %f\n",
        pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize,
        aCorners[0], aCorners[1], aCorners[2], aCorners[3], aCorners[4]);

    // for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        for (row = 0; row < pLSH->ui16Height; row++)
        {
            double ry = (double)row / (pLSH->ui16Height - 1);
#ifdef USE_MATH_NEON
            bowly = 1 - (double)cosf_neon((float)(ry-0.5f)*M_PI);	//cos(+/- PI/2)
#else
            bowly = 1 - cos((ry - 0.5f)*M_PI);  // cos(+/- PI/2)
#endif

            for (col = 0; col < pLSH->ui16Width; col++)
            {
                double val, rx;
                rx = (double)col / (pLSH->ui16Width - 1);
#ifdef USE_MATH_NEON
                bowlx = 1-(double)cosf_neon((float)(rx-0.5)*M_PI);	//cos(+/- PI/2)
#else
                bowlx = 1 - cos((rx - 0.5)*M_PI);  // cos(+/- PI/2)
#endif
                /* one on top of another, div by 2 to get back to
                 * amplitude 0=center,1.0=corners */
                bowl = (bowlx + bowly) / 2;
                val = ((aCorners[0] - aCorners[4])*(1 - ry)*(1 - rx)
                    + (aCorners[1] - aCorners[4])*(1 - ry)*rx
                    + (aCorners[2] - aCorners[4])*ry*(1 - rx)
                    + (aCorners[3] - aCorners[4])*ry*rx) * bowl + aCorners[4];
                pLSH->apMatrix[c][row*pLSH->ui16Width + col] = (LSH_FLOAT)val;
            }  // for col
        }  // for row
    }

    return IMG_SUCCESS;
}

void LSH_Free(LSH_GRID *pLSH)
{
    IMG_UINT32 i;

    IMG_ASSERT(pLSH != NULL);

    for (i = 0; i < LSH_MAT_NO; i++)
    {
        if (pLSH->apMatrix[i] != NULL)
        {
            IMG_FREE(pLSH->apMatrix[i]);
            pLSH->apMatrix[i] = NULL;
        }
    }
}

/*
 * file operations
 */

IMG_RESULT LSH_Save_txt(const LSH_GRID *pLSH, const char *filename)
{
    FILE *f = NULL;

    if (pLSH == NULL || filename == NULL)
    {
        LOG_DEBUG("NULL parameters given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    f = fopen(filename, "w");
    if (f != NULL)
    {
        int c, x, y;

        fprintf(f, "LSH matrix %ux%u, tile size %u, 4 channels\n",
            pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize);
        for (c = 0; c < LSH_MAT_NO; c++)
        {
            fprintf(f, "channel %u\n", c);
            for (y = 0; y < pLSH->ui16Height; y++)
            {
                for (x = 0; x < pLSH->ui16Width; x++)
                {
                    fprintf(f, "%f ",
                        pLSH->apMatrix[c][y*pLSH->ui16Width + x]);
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

/** nb of i32 in the header */
#define LSH_BIN_HEAD 3

IMG_RESULT LSH_Save_bin(const LSH_GRID *pLSH, const char *filename)
{
    FILE *f = NULL;

    if (pLSH == NULL || filename == NULL)
    {
        LOG_DEBUG("NULL parameters given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    f = fopen(filename, "wb");
    if (f != NULL)
    {
        int c, x, y;
        IMG_INT32 sizes[LSH_BIN_HEAD] = {
            (IMG_INT32)pLSH->ui16Width,
            (IMG_INT32)pLSH->ui16Height,
            (IMG_INT32)pLSH->ui16TileSize };

        fwrite(LSH_HEAD, sizeof(char), LSH_HEAD_SIZE, f);  // type of file
        c = 1;
        fwrite(&c, sizeof(int), 1, f);  // version
        fwrite(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f);
        for (c = 0; c < LSH_MAT_NO; c++)
        {
            if (GRID_DOUBLE_CHECK)
            {
                for (y = 0; y < pLSH->ui16Height; y++)
                {
                    for (x = 0; x < pLSH->ui16Width; x++)
                    {
                        float v =
                            (float)pLSH->apMatrix[c][y*pLSH->ui16Width + x];
                        if (fwrite(&v, sizeof(float), 1, f) != 1)
                        {
                            LOG_ERROR("failed to write LSH value "\
                                "c=%d y=%d x=%d\n", c, y, x);
                            fclose(f);
                            return IMG_ERROR_FATAL;
                        }
                    }
                }
            }
            else
            {
                fwrite(pLSH->apMatrix[c], sizeof(LSH_FLOAT),
                    pLSH->ui16Width*pLSH->ui16Height, f);
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

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE

#ifdef INFOTM_E2PROM_METHOD
IMG_RESULT lsh_e2prom(LSH_GRID *pLSH, const char *filename, OTP_CALIBRATION_DATA* calibration_data)
{
	int x, y, z, w, ring_cnt,index, ring_data_index, sensor_index = 0;
	E2PROM_VERSION_1 e2prom;
	float *linear_data[2];
	float *radius_val;
	IMG_RESULT ret;
	FILE *f = NULL;
	int c;
    float *buff = NULL;
	unsigned char tmp_val;
    float factor[2];//0.60;//0.725;
    
    factor[0] = calibration_data->pullup_gain_L;
    factor[1] = calibration_data->pullup_gain_R;

	e2prom.data[0] = (E2PROM_DATA*)calibration_data->sensor_calibration_data_L;
	e2prom.data[1] = (E2PROM_DATA*)calibration_data->sensor_calibration_data_R;

	ring_cnt = 19;//e2prom.data[0]->ring_cnt;//(unsigned short)(e2prom.data[0]->ring_cnt<<8)|(unsigned short)(e2prom.data[0]->ring_cnt>>8);

    e2prom.data[0]->ring_cnt = ring_cnt;
    e2prom.data[1]->ring_cnt = ring_cnt;

	ret = LSH_Load_head(pLSH, filename, &f, IMG_TRUE);
	if (ret != IMG_SUCCESS)
	{
		//fclose(f);
		return ret;
	}
	
	printf("==>e2prom pLSH->ui16Width %d, pLSH->ui16Height %d, pLSH->ui16TileSize %d, ring_cnt %d<%x>\n", pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize, ring_cnt, e2prom.data[0]->ring_cnt);
	
	if (pLSH->ui16Width*pLSH->ui16Height == 0)
	{
		LOG_ERROR("lsh grid size is 0 (w=%d h=%d)!\n", pLSH->ui16Width, pLSH->ui16Height);
		//fclose(f);
		return IMG_ERROR_FATAL;
	}

	radius_val = (float*)IMG_MALLOC(ring_cnt * sizeof(float));

	if (radius_val == NULL)
	{
		//fclose(f);
		printf("radius_val malloc failed!\n");
		return IMG_ERROR_FATAL;
	}
    
	BIG_RING_R = (pLSH->ui16Height*calibration_data->big_ring_radius)/(2*calibration_data->max_radius);
//	if ((float)(pLSH->ui16Height*calibration_data->big_ring_radius)/(2*calibration_data->max_radius) - (float)BIG_RING_R > 0.5)
//	{
//		BIG_RING_R += 1;
//	}
	
	SMALL_RING_R = (pLSH->ui16Height*calibration_data->small_ring_radius)/(2*calibration_data->max_radius);
//	if ((float)(pLSH->ui16Height*calibration_data->small_ring_radius)/(2*calibration_data->max_radius) - (float)SMALL_RING_R > 0.5)
//	{
//		SMALL_RING_R += 1;
//	}

    MAX_RADIUS = pLSH->ui16Height/2;

#ifdef INFOTM_LSH_CURVE_FITTING
    for (x = 0; x <N; x++)
    {
        samples_x[x] = BIG_RING_R*x/N;
    }
#endif


    //printf("===>max_radius %d, big_radius %d, small_radius %d\n", calibration_data->max_radius, calibration_data->big_ring_radius, calibration_data->small_ring_radius);

	for (x = 0; x < ring_cnt; x++)
	{
		radius_val[x] = BIG_RING_R*(ring_cnt - x)/ring_cnt;
	}

	linear_data[0] = (float*)IMG_MALLOC(LSH_MAT_NO*ring_cnt*RING_SECTOR_CNT*sizeof(float));
	if (linear_data[0] == NULL)
	{
		//fclose(f);
		printf("linear_data[0] malloc failed!\n");
		IMG_FREE(radius_val);
		return IMG_ERROR_FATAL;
	}
	
	linear_data[1] = (float*)IMG_MALLOC(LSH_MAT_NO*ring_cnt*RING_SECTOR_CNT*sizeof(float));
	if (linear_data[1] == NULL)
	{
		//fclose(f);
		printf("linear_data[1] malloc failed!\n");
		IMG_FREE(radius_val);
		IMG_FREE(linear_data[0]);
		return IMG_ERROR_FATAL;
	}

    //printf("==>start cala linear_data, data size %d\n", LSH_MAT_NO*ring_cnt*RING_SECTOR_CNT*sizeof(float));
    //printf("==>center_rgb %x, center[%d, %d, %d], e2prom.data[0] %x\n", 
    //e2prom.data[0]->center_rgb, e2prom.data[0]->center_rgb[0], e2prom.data[0]->center_rgb[1], e2prom.data[0]->center_rgb[2], e2prom.data[0]->ring_rgb);
   //printf("***0**");

   unsigned max_rgb[3];
   max_rgb[0] = e2prom.data[0]->ring_rgb[0];
   max_rgb[1] = e2prom.data[0]->ring_rgb[1];
   max_rgb[2] = e2prom.data[0]->ring_rgb[2];
   for (x = 0; x < RING_SECTOR_CNT; x++)
   {
        for (y = 0; y < 2; y++)
        {
            if (max_rgb[0] < e2prom.data[y]->ring_rgb[x*3])
            {
                max_rgb[0] = e2prom.data[y]->ring_rgb[x*3];
            }

            if (max_rgb[1] < e2prom.data[y]->ring_rgb[x*3 + 1])
            {
                max_rgb[1] = e2prom.data[y]->ring_rgb[x*3 + 1];
            }
            
            if (max_rgb[2] < e2prom.data[y]->ring_rgb[x*3 + 2])
            {
                max_rgb[2] = e2prom.data[y]->ring_rgb[x*3 + 2];
            }
        }
   }  
#ifdef LSH_DBG_LOG   
   printf("==>max_rgb[0] = %d, max_rgb[1] = %d, max_rgb[2] = %d\n", max_rgb[0], max_rgb[1], max_rgb[2]);
#endif
   //LSH_fite_eeprom_data(ring_cnt*RING_SECTOR_CNT,e2prom.data[0]->ring_rgb);
   //LSH_fite_eeprom_data(ring_cnt*RING_SECTOR_CNT,e2prom.data[1]->ring_rgb);

   //printf("==>center_rgb0[%d, %d, %d]\n", e2prom.data[0]->center_rgb[0], e2prom.data[0]->center_rgb[1], e2prom.data[0]->center_rgb[2]);
   //printf("==>center_rgb1[%d, %d, %d]\n", e2prom.data[1]->center_rgb[0], e2prom.data[1]->center_rgb[1], e2prom.data[1]->center_rgb[2]);
    w = 0;
    for (x = 0; x < ring_cnt; x++)
    {
        for (y = 0; y < RING_SECTOR_CNT; y++)
        {
            z = x*RING_SECTOR_CNT*3 + y*3;	
#if 0//def LSH_DBG_LOG
            printf("L(%d, %d, %d), R(%d, %d, %d)\n", e2prom.data[0]->ring_rgb[z], e2prom.data[0]->ring_rgb[z + 1], e2prom.data[0]->ring_rgb[z + 2], e2prom.data[1]->ring_rgb[z], e2prom.data[1]->ring_rgb[z + 1], e2prom.data[1]->ring_rgb[z + 2]);
#endif
            for (sensor_index = 0; sensor_index < 2; sensor_index++)
            {
#if 0
                if (e2prom.data[sensor_index]->center_rgb[0] > e2prom.data[sensor_index]->ring_rgb[z])
                {
                    linear_data[sensor_index][w] = 1.0 + factor[sensor_index]*(float)(e2prom.data[sensor_index]->center_rgb[0] - e2prom.data[sensor_index]->ring_rgb[z])/e2prom.data[sensor_index]->ring_rgb[z];
                }
                else
                {
                    linear_data[sensor_index][w] = 1.0;
                }

                if (e2prom.data[sensor_index]->center_rgb[1] > e2prom.data[sensor_index]->ring_rgb[z+1])
                {
                    linear_data[sensor_index][w+1] = 1.0 + factor[sensor_index]*(float)(e2prom.data[sensor_index]->center_rgb[1] - e2prom.data[sensor_index]->ring_rgb[z+1])/e2prom.data[sensor_index]->ring_rgb[z+1];
                }
                else
                {
                    linear_data[sensor_index][w+1] = 1.0;
                }
                
                linear_data[sensor_index][w+2] = linear_data[sensor_index][w+1];
                
                if (e2prom.data[sensor_index]->center_rgb[2] > e2prom.data[sensor_index]->ring_rgb[z+2])
                {
                    linear_data[sensor_index][w+3] = 1.0 +factor[sensor_index]*(float)(e2prom.data[sensor_index]->center_rgb[2] - e2prom.data[sensor_index]->ring_rgb[z+2])/e2prom.data[sensor_index]->ring_rgb[z+2];
                }
                else
                {
                    linear_data[sensor_index][w+3] = 1.0;
                }
#elif 1
                if (e2prom.data[sensor_index]->ring_rgb[z] < max_rgb[0])
                {
                    linear_data[sensor_index][w] = 1.0 + factor[sensor_index]*(max_rgb[0] - e2prom.data[sensor_index]->ring_rgb[z])/e2prom.data[sensor_index]->ring_rgb[z];
                }
                else
                {
                    linear_data[sensor_index][w] = 1.0;
                }

                if (e2prom.data[sensor_index]->ring_rgb[z+1] < max_rgb[1])
                {
                    linear_data[sensor_index][w+1] = 1.0 + factor[sensor_index]*(float)(max_rgb[1] - e2prom.data[sensor_index]->ring_rgb[z+1])/e2prom.data[sensor_index]->ring_rgb[z+1];
                }
                else
                {
                    linear_data[sensor_index][w+1] = 1.0;
                }

                linear_data[sensor_index][w+2] = linear_data[sensor_index][w+1];

                if (e2prom.data[sensor_index]->ring_rgb[z+2] < max_rgb[2])
                {
                    linear_data[sensor_index][w+3] = 1.0 + factor[sensor_index]*(max_rgb[2] - e2prom.data[sensor_index]->ring_rgb[z+2])/e2prom.data[sensor_index]->ring_rgb[z+2];
                }
                else
                {
                    linear_data[sensor_index][w+3] = 1.0;
                }

                if (y >= 1 && w > RING_SECTOR_CNT*4)
                {
                    if (linear_data[sensor_index][w] < linear_data[sensor_index][w - RING_SECTOR_CNT*4])
                    {
                        linear_data[sensor_index][w] = linear_data[sensor_index][w - RING_SECTOR_CNT*4];
                    }

                    if (linear_data[sensor_index][w+1] < linear_data[sensor_index][w+1 - RING_SECTOR_CNT*4])
                    {
                        linear_data[sensor_index][w+1] = linear_data[sensor_index][w+1 - RING_SECTOR_CNT*4];
                        linear_data[sensor_index][w+2] = linear_data[sensor_index][w+2 - RING_SECTOR_CNT*4];
                    }
                    
                    if (linear_data[sensor_index][w+3] < linear_data[sensor_index][w+3 - RING_SECTOR_CNT*4])
                    {
                        linear_data[sensor_index][w+3] = linear_data[sensor_index][w+3 - RING_SECTOR_CNT*4];
                    }
                }
#else
                
#endif
            }
#ifdef LSH_DBG_LOG
//            printf("***>e2prom.data[0]->ring_rgb<%d, %d, %d>, e2prom.data[1]->ring_rgb<%d, %d, %d>\n", 
//                   e2prom.data[0]->ring_rgb[z], e2prom.data[0]->ring_rgb[z+1], e2prom.data[0]->ring_rgb[z+2], 
//                   e2prom.data[1]->ring_rgb[z], e2prom.data[1]->ring_rgb[z+1], e2prom.data[1]->ring_rgb[z+2]);

            printf("===>[%f, %f, %f, %f]/[%f, %f, %f, %f]\n", linear_data[0][w], linear_data[0][w+1], linear_data[0][w+2], linear_data[0][w+3], linear_data[1][w], linear_data[1][w+1], linear_data[1][w+2], linear_data[1][w+3]);
#endif
	     w += 4;	
        }
    }
/*    
	for (x = 0; x < LSH_MAT_NO; x++)
	{
		if (x >= 2){c = x - 1;}
		else
		{
			c = x;
		}
		
		for (y = 0; y < ring_cnt; y++)
		{
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				index = x*ring_cnt*RING_SECTOR_CNT + y*RING_SECTOR_CNT + z;
                
				if (x == 2)
				{
					*(linear_data[0] + index) = *(linear_data[0] + index - y*RING_SECTOR_CNT);
					continue;
				}
				
				ring_data_index = (ring_cnt - y - 1)*3 + c;
                
				tmp_val = e2prom.data[0]->ring_rgb[ring_data_index];
                
				if (e2prom.data[0]->center_rgb[c] > tmp_val)
				{
					*(linear_data[0] + index) = 1.0 + (e2prom.data[0]->center_rgb[c] - tmp_val)/tmp_val;
				}
				else
				{
					*(linear_data[0] + index) = 1.0;
				}
                
				tmp_val = *(e2prom.data[1]->ring_rgb + ring_data_index);
                                
				if (e2prom.data[1]->center_rgb[c] > tmp_val)
				{
					*(linear_data[1] + index) = 1.0 + (e2prom.data[1]->center_rgb[c] - tmp_val)/tmp_val;
				}
				else
				{
					*(linear_data[1] + index) = 1.0;
				}
                
                printf("===>linear_data[0][%d]==%f, linear_data[1][%d]==%f, ring_rgb_0 %d, ring_rgb_1 %d\n", index, *(linear_data[0] + index), index, *(linear_data[1] + 
                index), e2prom.data[0]->ring_rgb[ring_data_index], e2prom.data[1]->ring_rgb[ring_data_index]);
			}
		}
	}
*/
///////////////////////////////
#if 0
    buff = (float*)IMG_MALLOC(pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
    if (buff == NULL)
    {
        LOG_ERROR("failed to allocate the output matrix\n");
        fclose(f);
		IMG_FREE(radius_val);
        return IMG_ERROR_MALLOC_FAILED;
    }
#endif

    for (c = 0; c < LSH_MAT_NO; c++)
    {
        pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));

        if (pLSH->apMatrix[c] == NULL)
        {
            LOG_ERROR("Failed to allocate matrix for "\
                "channel %d (%ld Bytes)\n",
                c, pLSH->ui16Width*pLSH->ui16Height*sizeof(LSH_FLOAT));
            c--;
            while (c >= 0)
            {
                IMG_FREE(pLSH->apMatrix[c]);
                c--;
            }
			
            //IMG_FREE(buff);
            IMG_FREE(radius_val);
			IMG_FREE(linear_data[0]);
			IMG_FREE(linear_data[1]);
			//fclose(f);
            return IMG_ERROR_MALLOC_FAILED;
        }
    }
	
	float R = 0, MAX_R_1;
	MAX_R_1 = MAX_RADIUS*3;
	float AbsVal = 0;

    for (c = 0; c < LSH_MAT_NO; c++)
    {
   	    int k = 0;
		R = 0;
#if 0
        // matrix is double! so load in buff
        ret = fread(buff, sizeof(float),
            pLSH->ui16Width*pLSH->ui16Height, f);
        if (ret != pLSH->ui16Width*pLSH->ui16Height)
        {
            LOG_ERROR("Failed to read channel %d "\
                "- read %d/%lu Bytes\n",
                c, ret,
                pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
            IMG_FREE(buff);
			IMG_FREE(radius_val);
            fclose(f);
            return IMG_ERROR_FATAL;
        }
#endif

#ifdef LSH_DBG_LOG
        printf("===>Marix %d\n", c);
#endif
        for (y = 0; y < pLSH->ui16Height; y++)
        {
            for (x = 0; x < pLSH->ui16Width; x++)
            {
                if (0)//calibration_data->flat_mode)
                {
                    pLSH->apMatrix[c][y*pLSH->ui16Width + x] = 0.5;
                    continue;
                }
                
            	if (x <= MAX_RADIUS)
        		{
                    if (y < MAX_RADIUS+calibration_data->radius_center_offset_L[1])
                    {
                        R = (float)sqrt((MAX_RADIUS-x+calibration_data->radius_center_offset_L[0])*(MAX_RADIUS-x+calibration_data->radius_center_offset_L[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_L[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_L[1]));// - 3;
                    }
                    else
                    {
                        R = (float)sqrt((MAX_RADIUS-x+calibration_data->radius_center_offset_L[0])*(MAX_RADIUS-x+calibration_data->radius_center_offset_L[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_L[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_L[1]));// - 3;
                    }

                    //printf("==>offset[%d, %d], MAX_R %d\n", calibration_data->radius_center_offset_L[0], calibration_data->radius_center_offset_L[1], MAX_RADIUS);
            	}
        		else
        		{
        			if (x >= MAX_RADIUS*2)
        			{
        				if (x <= MAX_R_1+calibration_data->radius_center_offset_R[0])
        				{
                            if (y < MAX_RADIUS+calibration_data->radius_center_offset_R[1])
                            {
                                R = (float)sqrt((MAX_R_1-x+calibration_data->radius_center_offset_R[0])*(MAX_R_1-x+calibration_data->radius_center_offset_R[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_R[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_R[1]));// - 3;
                            }
                            else
                            {
                                R = (float)sqrt((MAX_R_1-x+calibration_data->radius_center_offset_R[0])*(MAX_R_1-x+calibration_data->radius_center_offset_R[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_R[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_R[1]));// - 3;
                            }
                        }
                        else
                        {
                            if (y < MAX_RADIUS+calibration_data->radius_center_offset_R[1])
                            {
                                R = (float)sqrt((x-MAX_R_1+calibration_data->radius_center_offset_R[0])*(x-MAX_R_1+calibration_data->radius_center_offset_R[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_R[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_R[1]));// - 3;
                            }
                            else
                            {
                                R = (float)sqrt((x-MAX_R_1+calibration_data->radius_center_offset_R[0])*(x-MAX_R_1+calibration_data->radius_center_offset_R[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_R[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_R[1]));// - 3;
                            }
    			        }
        			}
        			else
        			{
                        if (y < MAX_RADIUS+calibration_data->radius_center_offset_L[1])
                        {
                            R = (float)sqrt((x-MAX_RADIUS+calibration_data->radius_center_offset_L[0])*(x-MAX_RADIUS+calibration_data->radius_center_offset_L[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_L[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_L[1]));// - 3;
                        }
                        else
                        {
                            R = (float)sqrt((x-MAX_RADIUS+calibration_data->radius_center_offset_L[0])*(x-MAX_RADIUS+calibration_data->radius_center_offset_L[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_L[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_L[1]));// - 3;
                        }
        			}
		        }
                
        		if (R < 0){R = 0;}
                
		        pLSH->apMatrix[c][y*pLSH->ui16Width + x] = LSH_E2prom_GetGainValue(R, x, y, !(x >= MAX_RADIUS*2), c, calibration_data, &e2prom, linear_data[0], linear_data[1], radius_val);
//                printf("%.3f,", pLSH->apMatrix[c][y*pLSH->ui16Width + x] );
            }
#if 0 //def LSH_DBG_LOG
	     printf("--\n");
#endif
        }

 	    //printf("\n===>Marix %d End\n", c);
    }

    //IMG_FREE(buff);
    //fclose(f);

///////////////////////////////

	IMG_FREE(radius_val);
	IMG_FREE(linear_data[0]);
	IMG_FREE(linear_data[1]);

	return IMG_SUCCESS;
}
#endif

IMG_RESULT LSH_Load_head(LSH_GRID *pLSH, const char *filename, FILE **fp, IMG_BOOL close_fp_flag)
{    
    FILE *f = NULL;
    f = fopen(filename, "rb");
    if (f != NULL)
    {
        int c;
        IMG_UINT32 sizes[LSH_BIN_HEAD];
        char name[4];

        // 1 line buffer used only if compiled with LSH_FLOAT as double
        float *buff = NULL;

        if (fread(name, sizeof(char), 4, f) != 4)
        {
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (strncmp(name, LSH_HEAD, LSH_HEAD_SIZE - 1) != 0)
        {
            LOG_ERROR("Wrong LSH file format - %s couldn't be read\n",
                LSH_HEAD);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        if (fread(&c, sizeof(int), 1, f) != 1)
        {
            LOG_ERROR("Failed to read LSH file format\n");
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (c != 1)
        {
            LOG_ERROR("wrong LSH file format - version %d found, "\
                "version %d supported\n", c, LSH_VERSION);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        if (fread(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f) != LSH_BIN_HEAD)
        {
            LOG_ERROR("failed to read %d int at the beginning of the file\n",
                LSH_BIN_HEAD);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        pLSH->ui16Width = (IMG_UINT16)sizes[0];
        pLSH->ui16Height = (IMG_UINT16)sizes[1];
        pLSH->ui16TileSize = (IMG_UINT16)sizes[2];

		if (close_fp_flag)
		{
        	fclose(f);
		}
		else
		{
			if (fp != NULL)
			{
				*fp = f;				
			}
		}
		return IMG_SUCCESS;
    }
    return IMG_ERROR_FATAL;    
}


IMG_RESULT IMG_LSH_Load_bin(LSH_GRID *pLSH, const char *filename)
{
    int ret = 0;
    FILE *f = NULL;

    if (pLSH == NULL || filename == NULL)
    {
        LOG_DEBUG("NULL parameter given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    f = fopen(filename, "rb");
    if (f != NULL)
    {
        int c;
        IMG_UINT32 sizes[LSH_BIN_HEAD];
        char name[4];
        // 1 line buffer used only if compiled with LSH_FLOAT as double
        float *buff = NULL;

        if (fread(name, sizeof(char), 4, f) != 4)
        {
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (strncmp(name, LSH_HEAD, LSH_HEAD_SIZE - 1) != 0)
        {
            LOG_ERROR("Wrong LSH file format - %s couldn't be read\n",
                LSH_HEAD);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        if (fread(&c, sizeof(int), 1, f) != 1)
        {
            LOG_ERROR("Failed to read LSH file format\n");
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (c != 1)
        {
            LOG_ERROR("wrong LSH file format - version %d found, "\
                "version %d supported\n", c, LSH_VERSION);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        if (fread(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f) != LSH_BIN_HEAD)
        {
            LOG_ERROR("failed to read %d int at the beginning of the file\n",
                LSH_BIN_HEAD);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        pLSH->ui16Width = (IMG_UINT16)sizes[0];
        pLSH->ui16Height = (IMG_UINT16)sizes[1];
        pLSH->ui16TileSize = (IMG_UINT16)sizes[2];

        if (pLSH->ui16Width*pLSH->ui16Height == 0)
        {
            LOG_ERROR("lsh grid size is 0 (w=%d h=%d)!\n", sizes[0], sizes[1]);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));

            if (pLSH->apMatrix[c] == NULL)
            {
                LOG_ERROR("Failed to allocate matrix for "\
                    "channel %d (%ld Bytes)\n",
                    c, pLSH->ui16Width*pLSH->ui16Height*sizeof(LSH_FLOAT));
                c--;
                while (c >= 0)
                {
                    IMG_FREE(pLSH->apMatrix[c]);
                    c--;
                }

                return IMG_ERROR_MALLOC_FAILED;
            }
        }

        for (c = 0; c < LSH_MAT_NO; c++)
        {            
            ret = fread(pLSH->apMatrix[c], sizeof(float),
                pLSH->ui16Width*pLSH->ui16Height, f);
            if (ret != pLSH->ui16Width*pLSH->ui16Height)
            {
                LOG_ERROR("Failed to read channel %d "\
                    "- read %d/%lu Bytes\n",
                    c, ret,
                    pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
                fclose(f);
                return IMG_ERROR_FATAL;
            }
        }

        if (GRID_DOUBLE_CHECK)
        {
            IMG_FREE(buff);
        }

        fclose(f);
    }
    else
    {
        LOG_ERROR("Failed to open file %s\n", filename);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}


IMG_RESULT LSH_Load_bin(LSH_GRID *pLSH, const char *filename, int infotm_method, OTP_CALIBRATION_DATA* calibration_data)
#else
IMG_RESULT LSH_Load_bin(LSH_GRID *pLSH, const char *filename)
#endif
{
    int ret = 0;
    FILE *f = NULL;
    
#ifdef SKIP_OTP_LSH_FOR_DEBUG
    calibration_data->sensor_calibration_data_L = NULL; // ??? for debug
    calibration_data->sensor_calibration_data_R = NULL;
#endif

    if (pLSH == NULL || filename == NULL)
    {
        LOG_DEBUG("NULL parameter given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

	if (infotm_method < 0)
	{
		return IMG_LSH_Load_bin(pLSH, filename);
	}
	
#ifdef LSH_DBG_LOG
	if (calibration_data != NULL && 
		calibration_data->sensor_calibration_data_L != NULL && 
		calibration_data->sensor_calibration_data_R != NULL)
	{
		printf("==>LSH_Load_bin %x, %x, %d, %d, %d, (%d, %d), (%d, %d)\n", 
				calibration_data->sensor_calibration_data_L, calibration_data->sensor_calibration_data_R, 
				calibration_data->big_ring_radius, calibration_data->small_ring_radius, calibration_data->max_radius, 
				calibration_data->radius_center_offset_L[0], calibration_data->radius_center_offset_L[1], 
				calibration_data->radius_center_offset_R[0], calibration_data->radius_center_offset_R[1]);
	}
#endif

#ifdef INFOTM_E2PROM_METHOD
	if (infotm_method == 2 && calibration_data != NULL && 
		calibration_data->sensor_calibration_data_L != NULL && 
		calibration_data->sensor_calibration_data_R != NULL)
	{
	    printf("==>LSH_Load_bin eeprom method\n");
		return lsh_e2prom(pLSH, filename, calibration_data);
	}
#endif


#ifdef INFOTM_LSH_FUNC
	int x = 0, y = 0, z = 0, small_y = (RING_CNT*SMALL_RING_R)/BIG_RING_R;
	static int runned_flag = 0;
	float fSetupL = 0;
	float fSetupR = 0;

	float fInnerRingSetupL = 0;
	float fInnerRingSetupR = 0;

	if (calibration_data != NULL)
	{
#ifdef INFOTM_LSH_CURVE_FITTING
		float avg_rgb_ring[N][4];
		float diffR, diffG, diffB;

        if (infotm_method == 1 && calibration_data->sensor_calibration_data_L != NULL && calibration_data->sensor_calibration_data_R != NULL)
        { // baikangxing
            BAIKANGXING_OTP_VERSION_1* V1_4_data[2];
            V1_4_data[0] = (BAIKANGXING_OTP_VERSION_1*)calibration_data->sensor_calibration_data_L;
            V1_4_data[1] = (BAIKANGXING_OTP_VERSION_1*)calibration_data->sensor_calibration_data_R;

            int tmp_r, tmp_g, tmp_b;
            unsigned char tmp_ring[6][3];           
            
            for (z = 0; z < 2; z++)
            {
                y = 0;
                tmp_r = 0;
                tmp_g = 0;
                tmp_b = 0;
                
                for (x = 1; x <= 8; x++)
                {
                    tmp_r += V1_4_data[z]->ring[x][0];
                    tmp_g += V1_4_data[z]->ring[x][1];
                    tmp_b += V1_4_data[z]->ring[x][2];
                }
                
                tmp_ring[y][0] = tmp_r/8;
                tmp_ring[y][1] = tmp_g/8;
                tmp_ring[y][2] = tmp_b/8;

                y++;
                
                for (x = 9; x <= 13; x++)
                {
                    tmp_ring[y][0] = V1_4_data[z]->ring[x][0];
                    tmp_ring[y][1] = V1_4_data[z]->ring[x][1];
                    tmp_ring[y][2] = V1_4_data[z]->ring[x][2];
                    y++;
                }

                tmp_r = 0;
                tmp_g = 0;
                tmp_b = 0;
                
                for (x = 13; x <= 29; x++)
                {
                    tmp_r += V1_4_data[z]->ring[x][0];
                    tmp_g += V1_4_data[z]->ring[x][1];
                    tmp_b += V1_4_data[z]->ring[x][2];
                }
                
                tmp_ring[y][0] = tmp_r/16;
                tmp_ring[y][1] = tmp_g/16;
                tmp_ring[y][2] = tmp_b/16;

                ///??? for debug?????????????????????????????????
                V1_4_data[z]->ring[0][0] = tmp_ring[0][0];
                V1_4_data[z]->ring[0][1] = tmp_ring[0][1];
                V1_4_data[z]->ring[0][2] = tmp_ring[0][2];
                
                V1_4_data[z]->ring[1][0] = tmp_ring[2][0];
                V1_4_data[z]->ring[1][1] = tmp_ring[2][1];
                V1_4_data[z]->ring[1][2] = tmp_ring[2][2];

                V1_4_data[z]->ring[2][0] = tmp_ring[4][0];
                V1_4_data[z]->ring[2][1] = tmp_ring[4][1];
                V1_4_data[z]->ring[2][2] = tmp_ring[4][2];

                V1_4_data[z]->ring[3][0] = tmp_ring[5][0];
                V1_4_data[z]->ring[3][1] = tmp_ring[5][1];
                V1_4_data[z]->ring[3][2] = tmp_ring[5][2];
                calibration_data->version = 0x1400;


                //////////////

                diffR = ((float)V1_4_data[z]->redius_center[0]/V1_4_data[z]->ring[0][0]);
                diffG = ((float)V1_4_data[z]->redius_center[1]/V1_4_data[z]->ring[0][1]);
                diffB = ((float)V1_4_data[z]->redius_center[2]/V1_4_data[z]->ring[0][2]);
                
                y = 0;
                for (x = 4, y = 0; x <= 7; x++, y++)
                {
                    avg_rgb_ring[x][0] = calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[0]/V1_4_data[z]->ring[y][0]);
                    avg_rgb_ring[x][1] = calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[1]/V1_4_data[z]->ring[y][1]);
                    avg_rgb_ring[x][2] = avg_rgb_ring[x][1];
                    avg_rgb_ring[x][3] = calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[2]/V1_4_data[z]->ring[y][2]);     
                    
                }

                for (x = 0; x < 4; x++)
                {
                    avg_rgb_ring[x][0] = (float)diffR*x/4;
                    avg_rgb_ring[x][1] = (float)diffG*x/4;
                    avg_rgb_ring[x][2] = (float)avg_rgb_ring[x][1];
                    avg_rgb_ring[x][3] = (float)diffB*x/4;
                }
                    
                calc_curfitting_param(z, avg_rgb_ring);     
            }
            calc_linear_curve();
        }   
        

		else if (infotm_method == 0/*calibration_data->version == 0x1400*/ && calibration_data->sensor_calibration_data_L != NULL && calibration_data->sensor_calibration_data_R != NULL)
		{
			LIPIN_OTP_VERSION_1_4* V1_4_data[2];
			V1_4_data[0] = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_L;
			V1_4_data[1] = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_R;

			for (z = 0; z < 2; z++)
			{
#if 0
				diffR = (float)(V1_4_data[z]->redius_center[0] - V1_4_data[z]->ring[0][0])/V1_4_data[z]->redius_center[0];
				diffG = (float)(V1_4_data[z]->redius_center[1] - V1_4_data[z]->ring[0][1])/V1_4_data[z]->redius_center[1];///2;
				diffB = (float)(V1_4_data[z]->redius_center[2] - V1_4_data[z]->ring[0][2])/V1_4_data[z]->redius_center[2];

				for (x = 4, y = 0; x <= 7; x++, y++)
				{
					avg_rgb_ring[x][0] = 1.0 + (float)(V1_4_data[z]->redius_center[0] - V1_4_data[z]->ring[y][0])/V1_4_data[z]->redius_center[0];
					avg_rgb_ring[x][1] = 1.0 + (float)(V1_4_data[z]->redius_center[1] - V1_4_data[z]->ring[y][1])/V1_4_data[z]->redius_center[1];
					avg_rgb_ring[x][2] = avg_rgb_ring[x][1];
					avg_rgb_ring[x][3] = 1.0 + (float)(V1_4_data[z]->redius_center[2] - V1_4_data[z]->ring[y][2])/V1_4_data[z]->redius_center[2];		
				}

				for (x = 0; x < 4; x++)
				{
					avg_rgb_ring[x][0] = 1.0 + (float)diffR*x/4;
					avg_rgb_ring[x][1] = 1.0 + (float)diffG*x/4;
					avg_rgb_ring[x][2] = 1.0 + (float)avg_rgb_ring[x][1];
					avg_rgb_ring[x][3] = 1.0 + (float)diffB*x/4;
				}
#else
				diffR = ((float)V1_4_data[z]->redius_center[0]-V1_4_data[z]->ring[0][0]);
				diffG = ((float)V1_4_data[z]->redius_center[1]-V1_4_data[z]->ring[0][1]);
				diffB = ((float)V1_4_data[z]->redius_center[2]-V1_4_data[z]->ring[0][2]);

				for (x = 4, y = 0; x <= 7; x++, y++)
				{
					avg_rgb_ring[x][0] = /*V1_4_data[z]->redius_center[0] + */calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[0]-V1_4_data[z]->ring[y][0]);
					avg_rgb_ring[x][1] = /*V1_4_data[z]->redius_center[1] + */calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[1]-V1_4_data[z]->ring[y][1]);
					avg_rgb_ring[x][2] = avg_rgb_ring[x][1];
					avg_rgb_ring[x][3] = /*V1_4_data[z]->redius_center[2] + */calibration_data->curve_base_val[z] + (float)((float)V1_4_data[z]->redius_center[2]-V1_4_data[z]->ring[y][2]);		
				}

				for (x = 0; x < 4; x++)
				{

					avg_rgb_ring[x][0] = /*V1_4_data[z]->redius_center[0] + */ (float)diffR*x/4;
					avg_rgb_ring[x][1] = /*V1_4_data[z]->redius_center[1] + */ (float)diffG*x/4;
					avg_rgb_ring[x][2] = (float)avg_rgb_ring[x][1];
					avg_rgb_ring[x][3] = /*V1_4_data[z]->redius_center[2] + */ (float)diffB*x/4;

				}
#endif
				calc_curfitting_param(z, avg_rgb_ring);
			}
			calc_linear_curve();
		}
#endif

		ret = LSH_Load_head(pLSH, filename, &f, IMG_FALSE);
		if (ret != IMG_SUCCESS)
		{
			fclose(f);
			return ret;
		}

		//pLSH->ui16Width = (IMG_UINT16)96*2 - 1;
        //pLSH->ui16Height = (IMG_UINT16)96;
		
		BIG_RING_R = (pLSH->ui16Height*calibration_data->big_ring_radius)/(2*calibration_data->max_radius);
		if ((float)(pLSH->ui16Height*calibration_data->big_ring_radius)/(2*calibration_data->max_radius) - (float)BIG_RING_R > 0.5)
		{
			BIG_RING_R += 1;
		}
		
		SMALL_RING_R = (pLSH->ui16Height*calibration_data->small_ring_radius)/(2*calibration_data->max_radius);
		if ((float)(pLSH->ui16Height*calibration_data->small_ring_radius)/(2*calibration_data->max_radius) - (float)SMALL_RING_R > 0.5)
		{
			SMALL_RING_R += 1;
		}

		MAX_RADIUS = pLSH->ui16Height/2;

#ifdef INFOTM_LSH_CURVE_FITTING
		for (x = 0; x <N; x++)
		{
			samples_x[x] = BIG_RING_R*x/N;
		}
#endif
		// ??? just an temp patch for simular lsc data
		//if (1)//
		if (0)//calibration_data->sensor_calibration_data_L == NULL || calibration_data->sensor_calibration_data_R == NULL)
		{
			int i = 0;
			for (i = 0; i < 4; i++)
			{
				memcpy(s_linear_curve_L[i], sim_s_linear_curve_L[i], sizeof(float)*RING_CNT);
				memcpy(s_linear_curve_R[i], sim_s_linear_curve_R[i], sizeof(float)*RING_CNT);
			}
			//BIG_RING_R = MAX_RADIUS;
		}
		
		for (x = 0; x < RING_CNT; x++)
		{
			s_radius[x] = BIG_RING_R*(RING_CNT - x)/RING_CNT;//MAX_RADIUS*(RING_CNT - x)/RING_CNT;
		}

		calibration_data->radius_center_offset_L[0] = MAX_RADIUS*calibration_data->radius_center_offset_L[0]/calibration_data->max_radius;
		calibration_data->radius_center_offset_L[1] = MAX_RADIUS*calibration_data->radius_center_offset_L[1]/calibration_data->max_radius;

		calibration_data->radius_center_offset_R[0] = MAX_RADIUS*calibration_data->radius_center_offset_R[0]/calibration_data->max_radius;
		calibration_data->radius_center_offset_R[1] = MAX_RADIUS*calibration_data->radius_center_offset_R[1]/calibration_data->max_radius;
		
#ifdef LSH_DBG_LOG
		printf("==>BIG_RING_R %d, SMALL_RING_R %d, MAX_RADIUS %d, offset (%d, %d), (%d, %d)\n", BIG_RING_R, SMALL_RING_R, MAX_RADIUS, calibration_data->radius_center_offset_L[0], calibration_data->radius_center_offset_L[1], 
		       calibration_data->radius_center_offset_R[0], calibration_data->radius_center_offset_R[1]);
#endif

	}
	//runned_flag++;
	if (calibration_data == NULL || calibration_data->sensor_calibration_data_L == NULL || calibration_data->sensor_calibration_data_R == NULL)
	{
	    float def_max_gain = 1.5;
		for (x = 0; x < LSH_MAT_NO; x++)
		{
/*		
                        if (x == 0)
                        {
                            def_max_gain = 1.8;
                        }
                        else
                        {
                            def_max_gain = 1.8;
                        }
*/
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{

				s_big_rggb_L[x][z] = def_max_gain*calibration_data->pullup_gain_L;
				s_big_rggb_R[x][z] = def_max_gain*calibration_data->pullup_gain_R;

				if (calibration_data->pullup_gain_L >= 1.3)
				{
					s_small_rggb_L[x][z] = (calibration_data->pullup_gain_L-1.0)/2;
				}
				else
				{
					s_small_rggb_L[x][z] = 1.0;
				}
				
				if (calibration_data->pullup_gain_R >= 1.3)
				{
					s_small_rggb_R[x][z] = (calibration_data->pullup_gain_R-1.0)/2;
				}
				else
				{
					s_small_rggb_R[x][z] = 1.0;
				}
			}
		}
	}
	else
	{
		if (calibration_data->version == 0x1400)
		{
			LIPIN_OTP_VERSION_1_4* V1_4_data_L = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_L;
			LIPIN_OTP_VERSION_1_4* V1_4_data_R = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_R;

#ifdef LSH_DBG_LOG
			printf("==>center L(%f, %f, %f, %f), R(%f, %f, %f, %f)\n", 
				   s_center_rggb_L[0], s_center_rggb_L[1], s_center_rggb_L[2], s_center_rggb_L[3], 
				   s_center_rggb_R[0], s_center_rggb_R[1], s_center_rggb_R[2], s_center_rggb_R[3]);
#endif
#ifdef INFOTM_LSH_CHANGE_BY_DUAL_RGB_DIFF
			/////////////////////////
			if (calibration_data != NULL && calibration_data->sensor_calibration_data_L != NULL && calibration_data->sensor_calibration_data_R != NULL)
			{
				int m, n;
				float r_ratio = 0.1, g_ratio = 0.1, b_ratio = 0.1;
				float r_ratio_max = 0.1, g_ratio_max = 0.1, b_ratio_max = 0.1;
				for (m = 0; m < 3; m++)
				{
					for (n = 0; n < RING_SECTOR_CNT; n++)
					{
						if (s_limit_rgb[0][m][0] == 0 || s_limit_rgb[0][m][0] > V1_4_data_L->big_ring_sector[n][m])
						{
							s_limit_rgb[0][m][0] = V1_4_data_L->big_ring_sector[n][m];
						}
						if (s_limit_rgb[0][m][1] < V1_4_data_L->big_ring_sector[n][m])
						{
							s_limit_rgb[0][m][1] = V1_4_data_L->big_ring_sector[n][m];
						}

						if (s_limit_rgb[1][m][0] == 0 || s_limit_rgb[1][m][0] > V1_4_data_R->big_ring_sector[n][m])
						{
							s_limit_rgb[1][m][0] = V1_4_data_R->big_ring_sector[n][m];
						}
						if (s_limit_rgb[1][m][1] < V1_4_data_R->big_ring_sector[n][m])
						{
							s_limit_rgb[1][m][1] = V1_4_data_R->big_ring_sector[n][m];
						}
					}
				}
				
#ifdef LSH_DBG_LOG
				printf("==>r (%d, %d)(%d, %d)\n==>g (%d, %d)(%d, %d)\n==>b (%d, %d)(%d, %d)\n", 
				s_limit_rgb[0][0][0], s_limit_rgb[0][0][1], s_limit_rgb[1][0][0], s_limit_rgb[1][0][1], 
				s_limit_rgb[0][1][0], s_limit_rgb[0][1][1], s_limit_rgb[1][1][0], s_limit_rgb[1][1][1],
				s_limit_rgb[0][2][0], s_limit_rgb[0][2][1], s_limit_rgb[1][2][0], s_limit_rgb[1][2][1]);
#endif

				r_ratio = ((float)s_limit_rgb[0][0][0]/s_limit_rgb[1][0][0]);
				if (r_ratio > 1.0)
				{
					r_ratio = ((float)s_limit_rgb[1][0][0]/s_limit_rgb[0][0][0]);
				}

				g_ratio = ((float)s_limit_rgb[0][1][0]/s_limit_rgb[1][1][0]);
				if (g_ratio > 1.0)
				{
					g_ratio = ((float)s_limit_rgb[1][1][0]/s_limit_rgb[0][1][0]);
				}

				b_ratio = ((float)s_limit_rgb[0][2][0]/s_limit_rgb[1][2][0]);
				if (b_ratio > 1.0)
				{
					b_ratio = ((float)s_limit_rgb[1][2][0]/s_limit_rgb[0][2][0]);
				}
				////////////////////
				r_ratio_max = ((float)s_limit_rgb[0][0][1]/s_limit_rgb[1][0][1]);
				if (r_ratio_max > 1.0)
				{
					r_ratio_max = ((float)s_limit_rgb[1][0][1]/s_limit_rgb[0][0][1]);
				}

				if (r_ratio > r_ratio_max)
				{
					r_ratio = r_ratio_max;
				}

				g_ratio_max = ((float)s_limit_rgb[0][1][1]/s_limit_rgb[1][1][1]);
				if (g_ratio_max > 1.0)
				{
					g_ratio_max = ((float)s_limit_rgb[1][1][1]/s_limit_rgb[0][1][1]);
				}

				if (g_ratio > g_ratio_max)
				{
					g_ratio = g_ratio_max;
				}

				b_ratio_max = ((float)s_limit_rgb[0][2][1]/s_limit_rgb[1][2][1]);
				if (b_ratio_max > 1.0)
				{
					b_ratio_max = ((float)s_limit_rgb[1][2][1]/s_limit_rgb[0][2][1]);
				}

				if (b_ratio > b_ratio_max)
				{
					b_ratio = b_ratio_max;
				}

#ifdef LSH_DBG_LOG
				printf("==>r_ratio %f, g_ratio %f, b_ratio %f\n", 1.0 - r_ratio, 1.0 - g_ratio, 1.0 - b_ratio);
#endif
				if ((1.0 - r_ratio) >= 0.32 || (1.0 - g_ratio) >= 0.32 || (1.0 - b_ratio) >= 0.32)
				{
					if (calibration_data->pullup_gain_L > 1.0 || calibration_data->pullup_gain_R > 1.0)
					{
						calibration_data->pullup_gain_L = 1.0;
						calibration_data->pullup_gain_R = 1.0;
					}
				}
				
				else if ((1.0 - r_ratio) >= 0.30 || (1.0 - g_ratio) >= 0.30 || (1.0 - b_ratio) >= 0.30)
				{
					if (calibration_data->pullup_gain_L > 1.05 || calibration_data->pullup_gain_R > 1.05)
					{
						calibration_data->pullup_gain_L = 1.05;
						calibration_data->pullup_gain_R = 1.05;
					}
				}
				
				else if ((1.0 - r_ratio) >= 0.26 || (1.0 - g_ratio) >= 0.26 || (1.0 - b_ratio) >= 0.26)
				{
					if (calibration_data->pullup_gain_L > 1.10 || calibration_data->pullup_gain_R > 1.10)
					{
						calibration_data->pullup_gain_L = 1.10;
						calibration_data->pullup_gain_R = 1.10;
					}
				}

				else if ((1.0 - r_ratio) >= 0.2 || (1.0 - g_ratio) >= 0.2 || (1.0 - b_ratio) >= 0.2)
				{
					if (calibration_data->pullup_gain_L > 1.2 || calibration_data->pullup_gain_R > 1.2)
					{
						calibration_data->pullup_gain_L = 1.2;
						calibration_data->pullup_gain_R = 1.2;

					}
				}

				else if ((1.0 - r_ratio) >= 0.15 || (1.0 - g_ratio) >= 0.15 || (1.0 - b_ratio) >= 0.15)
				{
					if (calibration_data->pullup_gain_L > 1.3 || calibration_data->pullup_gain_R > 1.3)
					{
						calibration_data->pullup_gain_L = 1.3;
						calibration_data->pullup_gain_R = 1.3;
					}
				}

				else if ((1.0 - r_ratio) >= 0.1 || (1.0 - g_ratio) >= 0.1 || (1.0 - b_ratio) >= 0.1)
				{
					if (calibration_data->pullup_gain_L > 1.4 || calibration_data->pullup_gain_R > 1.4)
					{
						calibration_data->pullup_gain_L = 1.4;
						calibration_data->pullup_gain_R = 1.4;
					}
				}
				else if ((1.0 - r_ratio) >= 0.05 || (1.0 - g_ratio) >= 0.1 || (1.0 - b_ratio) >= 0.05)
				{
					if (calibration_data->pullup_gain_L > 1.5 || calibration_data->pullup_gain_R > 1.5)
					{
						calibration_data->pullup_gain_L = 1.5;
						calibration_data->pullup_gain_R = 1.5;
					}
				}				

			}
			/////////////////////////
#endif

#ifdef LSH_DBG_LOG
			if (calibration_data != NULL && calibration_data->sensor_calibration_data_L != NULL && calibration_data->sensor_calibration_data_R != NULL)
			{
				printf("==>pullup_gain_L %f, pullup_gain_R %f\n", calibration_data->pullup_gain_L, calibration_data->pullup_gain_R);
			}
#endif
		
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				//if (z == 0)
				{
				if (V1_4_data_L->redius_center[1] >= V1_4_data_L->big_ring_sector[z][1])
				{
				    s_big_rggb_L[1][z] = s_center_rggb_L[1]  + (float)(s_center_rggb_L[1]*(V1_4_data_L->redius_center[1]-V1_4_data_L->big_ring_sector[z][1])*calibration_data->pullup_gain_L)/V1_4_data_L->big_ring_sector[z][1];
					//s_big_rggb_L[1][z] = (s_center_rggb_L[1] + s_center_rggb_L[1]*(1.0 - (float)V1_4_data_L->big_ring_sector[z][1]/V1_4_data_L->redius_center[1]))*calibration_data->pullup_gain_L;
				}
				else
				{
					s_big_rggb_L[1][z] = s_center_rggb_L[1];
				}
				
				s_big_rggb_L[2][z] = s_big_rggb_L[1][z];

				if (V1_4_data_L->redius_center[0] >= V1_4_data_L->big_ring_sector[z][0])
				{
					s_big_rggb_L[0][z] = s_center_rggb_L[0]  + (float)(s_center_rggb_L[0]*(V1_4_data_L->redius_center[0]-V1_4_data_L->big_ring_sector[z][0])*calibration_data->pullup_gain_L)/V1_4_data_L->big_ring_sector[z][0];
					//s_big_rggb_L[0][z] = (s_center_rggb_L[0] + s_center_rggb_L[0]*(1.0 - (float)V1_4_data_L->big_ring_sector[z][0]/V1_4_data_L->redius_center[0]))*calibration_data->pullup_gain_L;
				}
				else
				{
					s_big_rggb_L[0][z] = s_center_rggb_L[0];
				}

				if (V1_4_data_L->redius_center[2] >= V1_4_data_L->big_ring_sector[z][2])
				{
					s_big_rggb_L[3][z] = s_center_rggb_L[3]  + (float)(s_center_rggb_L[3]*(V1_4_data_L->redius_center[2]-V1_4_data_L->big_ring_sector[z][2])*calibration_data->pullup_gain_L)/V1_4_data_L->big_ring_sector[z][2];
                    //s_big_rggb_L[3][z] = (s_center_rggb_L[3] + s_center_rggb_L[3]*(1.0 - (float)V1_4_data_L->big_ring_sector[z][2]/V1_4_data_L->redius_center[2]))*calibration_data->pullup_gain_L;
				}
				else
				{
					s_big_rggb_L[3][z] = s_center_rggb_L[3];
				}
				}
/*
				else
				{
					s_big_rggb_L[1][z] = s_big_rggb_L[1][0];
					s_big_rggb_L[2][z] = s_big_rggb_L[2][0];
					s_big_rggb_L[0][z] = s_big_rggb_L[0][0];
					s_big_rggb_L[3][z] = s_big_rggb_L[3][0];
				}

				if (z == 0)
*/
				{
				if (V1_4_data_R->redius_center[1] >= V1_4_data_R->big_ring_sector[z][1])
				{
					s_big_rggb_R[1][z] = s_center_rggb_R[1]  + (float)(s_center_rggb_R[1]*(V1_4_data_R->redius_center[1]-V1_4_data_R->big_ring_sector[z][1])*calibration_data->pullup_gain_R)/V1_4_data_R->big_ring_sector[z][1];
                    //s_big_rggb_R[1][z] = (s_center_rggb_R[1] + s_center_rggb_R[1]*(1.0 - (float)V1_4_data_R->big_ring_sector[z][1]/V1_4_data_R->redius_center[1]))*calibration_data->pullup_gain_R;
				}
				else
				{
					s_big_rggb_R[1][z] = s_center_rggb_R[1];
				}
				s_big_rggb_R[2][z] = (s_big_rggb_R[1][z]);
				
				if (V1_4_data_R->redius_center[0] >= V1_4_data_R->big_ring_sector[z][0])
				{
					s_big_rggb_R[0][z] = s_center_rggb_R[0]  + (float)(s_center_rggb_R[0]*(V1_4_data_R->redius_center[0]-V1_4_data_R->big_ring_sector[z][0])*calibration_data->pullup_gain_R)/V1_4_data_R->big_ring_sector[z][0];
                    //s_big_rggb_R[0][z] = (s_center_rggb_R[0] + s_center_rggb_R[0]*(1.0 - (float)V1_4_data_R->big_ring_sector[z][0]/V1_4_data_R->redius_center[0]))*calibration_data->pullup_gain_R;
				}
				else
				{
					s_big_rggb_R[0][z] = s_center_rggb_R[0];
				}

				if (V1_4_data_R->redius_center[2] >= V1_4_data_R->big_ring_sector[z][2])
				{
					s_big_rggb_R[3][z] = s_center_rggb_R[3]  + (float)(s_center_rggb_R[3]*(V1_4_data_R->redius_center[2]-V1_4_data_R->big_ring_sector[z][2])*calibration_data->pullup_gain_R)/V1_4_data_R->big_ring_sector[z][2];
                    //s_big_rggb_R[3][z] = (s_center_rggb_R[2] + s_center_rggb_R[2]*(1.0 - (float)V1_4_data_R->big_ring_sector[z][2]/V1_4_data_R->redius_center[2]))*calibration_data->pullup_gain_R;
				}	
				else
				{
					s_big_rggb_R[3][z] = s_center_rggb_R[3];
				}
				}
/*
				else
				{
					s_big_rggb_R[1][z] = s_big_rggb_L[1][0];
					s_big_rggb_R[2][z] = s_big_rggb_L[2][0];
					s_big_rggb_R[0][z] = s_big_rggb_L[0][0];
					s_big_rggb_R[3][z] = s_big_rggb_L[3][0];
				}
*/
//				s_big_rggb_L[1][z] = s_big_rggb_R[1][0];
//				s_big_rggb_L[2][z] = s_big_rggb_R[2][0];
//				s_big_rggb_L[0][z] = s_big_rggb_R[0][0];
//				s_big_rggb_L[3][z] = s_big_rggb_R[3][0];
				
#ifdef LSH_DBG_LOG
				printf("==>L(%f, %f, %f, %f), R(%f, %f, %f, %f)\n", 
					   s_big_rggb_L[0][z], s_big_rggb_L[1][z], s_big_rggb_L[2][z], s_big_rggb_L[3][z], 
					   s_big_rggb_R[0][z], s_big_rggb_R[1][z], s_big_rggb_R[2][z], s_big_rggb_R[3][z]);
#endif
			}


			for (z = 0; z < RING_SECTOR_CNT; z+=2)
			{
				//s_small_rggb_L[1][z] = (s_center_rggb_L[1] + s_center_rggb_L[1]*(1.0 - (float)V1_4_data_L->small_ring_sector[z/2][1]/V1_4_data_L->redius_center[1]))*calibration_data->pullup_gain_L;
				s_small_rggb_L[1][z] = s_center_rggb_L[1]  + (float)(s_center_rggb_L[1]*(V1_4_data_L->redius_center[1]-V1_4_data_L->small_ring_sector[z/2][1])*calibration_data->pullup_gain_L)/V1_4_data_L->small_ring_sector[z/2][1];
				s_small_rggb_L[2][z] = s_small_rggb_L[1][z];

				//s_small_rggb_L[0][z] = (s_center_rggb_L[0] + s_center_rggb_L[0]*(1.0 - (float)V1_4_data_L->small_ring_sector[z/2][0]/V1_4_data_L->redius_center[0]))*calibration_data->pullup_gain_L;
				//s_small_rggb_L[3][z] = (s_center_rggb_L[3] + s_center_rggb_L[3]*(1.0 - (float)V1_4_data_L->small_ring_sector[z/2][2]/V1_4_data_L->redius_center[2]))*calibration_data->pullup_gain_L;
                s_small_rggb_L[0][z] = s_center_rggb_L[0]  + (float)(s_center_rggb_L[0]*(V1_4_data_L->redius_center[0]-V1_4_data_L->small_ring_sector[z/2][0])*calibration_data->pullup_gain_L)/V1_4_data_L->small_ring_sector[z/2][0];
				s_small_rggb_L[3][z] = s_center_rggb_L[3]  + (float)(s_center_rggb_L[3]*(V1_4_data_L->redius_center[2]-V1_4_data_L->small_ring_sector[z/2][2])*calibration_data->pullup_gain_L)/V1_4_data_L->small_ring_sector[z/2][2];
                
				s_small_rggb_L[1][z+1] = s_small_rggb_L[1][z];
				s_small_rggb_L[2][z+1] = s_small_rggb_L[1][z+1];
				
				s_small_rggb_L[0][z+1] = s_small_rggb_L[0][z];
				s_small_rggb_L[3][z+1] = s_small_rggb_L[3][z];
                
				//////////////////////
				//s_small_rggb_R[1][z] = (s_center_rggb_R[1] + s_center_rggb_R[1]*(1.0 - (float)V1_4_data_R->small_ring_sector[z/2][1]/V1_4_data_R->redius_center[1]))*calibration_data->pullup_gain_R;
                s_small_rggb_R[1][z] = s_center_rggb_R[1]  + (float)(s_center_rggb_R[1]*(V1_4_data_R->redius_center[1]-V1_4_data_R->small_ring_sector[z/2][1])*calibration_data->pullup_gain_R)/V1_4_data_R->small_ring_sector[z/2][1];
                s_small_rggb_R[2][z] = s_small_rggb_R[1][z];

				//s_small_rggb_R[0][z] = (s_center_rggb_R[0] + s_center_rggb_R[0]*(1.0 - (float)V1_4_data_R->small_ring_sector[z/2][0]/V1_4_data_R->redius_center[0]))*calibration_data->pullup_gain_R;
				//s_small_rggb_R[3][z] = (s_center_rggb_R[3] + s_center_rggb_R[3]*(1.0 - (float)V1_4_data_R->small_ring_sector[z/2][2]/V1_4_data_R->redius_center[2]))*calibration_data->pullup_gain_R;

                s_small_rggb_R[0][z] = s_center_rggb_R[0]  + (float)(s_center_rggb_R[0]*(V1_4_data_R->redius_center[0]-V1_4_data_R->small_ring_sector[z/2][0])*calibration_data->pullup_gain_R)/V1_4_data_R->small_ring_sector[z/2][0];
                s_small_rggb_R[3][z] = s_center_rggb_R[3]  + (float)(s_center_rggb_R[3]*(V1_4_data_R->redius_center[2]-V1_4_data_R->small_ring_sector[z/2][2])*calibration_data->pullup_gain_R)/V1_4_data_R->small_ring_sector[z/2][2];
				
				s_small_rggb_R[1][z+1] = s_small_rggb_R[1][z];
				s_small_rggb_R[2][z+1] = s_small_rggb_R[1][z+1];
				
				s_small_rggb_R[0][z+1] = s_small_rggb_R[0][z];
				s_small_rggb_R[3][z+1] = s_small_rggb_R[3][z];
#ifdef LSH_DBG_LOG
                printf("==>L_m(%f, %f, %f, %f), R_m(%f, %f, %f, %f)\n", 
                       s_small_rggb_L[0][z], s_small_rggb_L[1][z], s_small_rggb_L[2][z], s_small_rggb_L[3][z], 
                       s_small_rggb_R[0][z], s_small_rggb_R[1][z], s_small_rggb_R[2][z], s_small_rggb_R[3][z]);
#endif

			}
		}
		else
		{
			LIPIN_OTP_VERSION_1_4* V1_3_data_L = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_L;
			LIPIN_OTP_VERSION_1_4* V1_3_data_R = (LIPIN_OTP_VERSION_1_4*)calibration_data->sensor_calibration_data_R;

			if (V1_3_data_L->redius_center[0] < V1_3_data_R->redius_center[0])
			{
				s_center_rggb_L[0] = 2.0 - ((float)V1_3_data_L->redius_center[0]/V1_3_data_R->redius_center[0]);
				s_center_rggb_R[0] = 1.0;
			}
			else
			{
				s_center_rggb_R[0] = 2.0 - ((float)V1_3_data_R->redius_center[0]/V1_3_data_L->redius_center[0]);
				s_center_rggb_L[0] = 1.0;
			
			}
			
			if (V1_3_data_L->redius_center[1] < V1_3_data_R->redius_center[1])
			{
				s_center_rggb_L[1] = 2.0 - ((float)V1_3_data_L->redius_center[1]/V1_3_data_R->redius_center[1]);
				s_center_rggb_R[1] = 1.0;
			}
			else
			{
				s_center_rggb_R[1] = 2.0 - ((float)V1_3_data_R->redius_center[1]/V1_3_data_L->redius_center[1]);
				s_center_rggb_L[1] = 1.0;
			
			}
			
			s_center_rggb_L[2] = s_center_rggb_L[1];
			s_center_rggb_R[2] = s_center_rggb_R[1];
			
			if (V1_3_data_L->redius_center[2] < V1_3_data_R->redius_center[2])
			{
				s_center_rggb_L[1] = 2.0 - ((float)V1_3_data_L->redius_center[2]/V1_3_data_R->redius_center[2]);
				s_center_rggb_R[1] = 1.0;
			}
			else
			{
				s_center_rggb_R[1] = 2.0 - ((float)V1_3_data_R->redius_center[2]/V1_3_data_L->redius_center[2]);
				s_center_rggb_L[1] = 1.0;
			
			}

#ifdef LSH_DBG_LOG
			printf("==>center L(%f, %f, %f, %f), R(%f, %f, %f, %f)\n", 
				   s_center_rggb_L[0], s_center_rggb_L[1], s_center_rggb_L[2], s_center_rggb_L[3], 
				   s_center_rggb_R[0], s_center_rggb_R[1], s_center_rggb_R[2], s_center_rggb_R[3]);
#endif

			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				s_big_rggb_L[1][z] = (s_center_rggb_L[1] + s_center_rggb_L[1]*(1.0 - (float)V1_3_data_L->big_ring_sector[z][1]/V1_3_data_L->redius_center[1]))*calibration_data->pullup_gain_L;
				s_big_rggb_L[2][z] = s_big_rggb_L[1][z];

				s_big_rggb_L[0][z] = (s_center_rggb_L[0] + s_center_rggb_L[0]*(1.0 - (float)V1_3_data_L->big_ring_sector[z][0]/V1_3_data_L->redius_center[0]))*calibration_data->pullup_gain_L;
				s_big_rggb_L[3][z] = (s_center_rggb_L[2] + s_center_rggb_L[2]*(1.0 - (float)V1_3_data_L->big_ring_sector[z][2]/V1_3_data_L->redius_center[2]))*calibration_data->pullup_gain_L;


				s_big_rggb_R[1][z] = (s_center_rggb_R[1] + s_center_rggb_R[1]*(1.0 - (float)V1_3_data_R->big_ring_sector[z][1]/V1_3_data_R->redius_center[1]))*calibration_data->pullup_gain_R;
				s_big_rggb_R[2][z] = (s_big_rggb_R[1][z]);

				s_big_rggb_R[0][z] = (s_center_rggb_R[0] + s_center_rggb_R[0]*(1.0 - (float)V1_3_data_R->big_ring_sector[z][0]/V1_3_data_R->redius_center[0]))*calibration_data->pullup_gain_R;
				s_big_rggb_R[3][z] = (s_center_rggb_R[2] + s_center_rggb_R[2]*(1.0 - (float)V1_3_data_R->big_ring_sector[z][2]/V1_3_data_R->redius_center[2]))*calibration_data->pullup_gain_R;

#ifdef LSH_DBG_LOG
				printf("==>L(%f, %f, %f, %f), R(%f, %f, %f, %f)\n", 
					   s_big_rggb_L[0][z], s_big_rggb_L[1][z], s_big_rggb_L[2][z], s_big_rggb_L[3][z], 
					   s_big_rggb_R[0][z], s_big_rggb_R[1][z], s_big_rggb_R[2][z], s_big_rggb_R[3][z]);
#endif
			}

			for (z = 0; z < RING_SECTOR_CNT; z+=2)
			{
				s_small_rggb_L[1][z] = (s_center_rggb_L[1] + s_center_rggb_L[1]*(1.0 - (float)V1_3_data_L->small_ring_sector[z/2][1]/V1_3_data_L->redius_center[1]))*calibration_data->pullup_gain_L;
				s_small_rggb_L[2][z] = s_small_rggb_L[1][z];

				s_small_rggb_L[0][z] = (s_center_rggb_L[0] + s_center_rggb_L[0]*(1.0 - (float)V1_3_data_L->small_ring_sector[z/2][0]/V1_3_data_L->redius_center[0]))*calibration_data->pullup_gain_L;
				s_small_rggb_L[3][z] = (s_center_rggb_L[3] + s_center_rggb_L[3]*(1.0 - (float)V1_3_data_L->small_ring_sector[z/2][2]/V1_3_data_L->redius_center[2]))*calibration_data->pullup_gain_L;
				
				s_small_rggb_L[1][z+1] = s_small_rggb_L[1][z];
				s_small_rggb_L[2][z+1] = s_small_rggb_L[1][z+1];
				
				s_small_rggb_L[0][z+1] = s_small_rggb_L[0][z];
				s_small_rggb_L[3][z+1] = s_small_rggb_L[3][z];
				//////////////////////
				s_small_rggb_R[1][z] = (s_center_rggb_R[1] + s_center_rggb_R[1]*(1.0 - (float)V1_3_data_R->small_ring_sector[z/2][1]/V1_3_data_R->redius_center[1]))*calibration_data->pullup_gain_R;
				s_small_rggb_R[2][z] = s_small_rggb_R[1][z];

				s_small_rggb_R[0][z] = (s_center_rggb_R[0] + s_center_rggb_R[0]*(1.0 - (float)V1_3_data_R->small_ring_sector[z/2][0]/V1_3_data_R->redius_center[0]))*calibration_data->pullup_gain_R;
				s_small_rggb_R[3][z] = (s_center_rggb_R[3] + s_center_rggb_R[3]*(1.0 - (float)V1_3_data_R->small_ring_sector[z/2][2]/V1_3_data_R->redius_center[2]))*calibration_data->pullup_gain_R;
				
				s_small_rggb_R[1][z+1] = s_small_rggb_R[1][z];
				s_small_rggb_R[2][z+1] = s_small_rggb_R[1][z+1];
				
				s_small_rggb_R[0][z+1] = s_small_rggb_R[0][z];
				s_small_rggb_R[3][z+1] = s_small_rggb_R[3][z];
			}
		}
	}	
	
	for (x = 0; x < LSH_MAT_NO; x++)
	{
#ifdef INFOTM_9080_HM2131_PROJECT
		for (y = 0; y < SMALL_RING_R; y++)
		{
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				fSetupL = (float)(s_big_rggb_L[x][z] - s_small_rggb_L[x][z])*(s_linear_curve_L[x][y] - s_linear_curve_L[x][0])/(s_linear_curve_L[x][SMALL_RING_R-1] - s_linear_curve_L[x][0]);
				fSetupR = (float)(s_big_rggb_R[x][z] - s_small_rggb_R[x][z])*(s_linear_curve_R[x][y] - s_linear_curve_R[x][0])/(s_linear_curve_R[x][SMALL_RING_R-1] - s_linear_curve_R[x][0]);

				s_rggb_linear_data_L[x][y][z] = s_big_rggb_L[x][z] - y*fSetupL;
				s_rggb_linear_data_R[x][y][z] = s_big_rggb_R[x][z] - y*fSetupR;

				if (s_rggb_linear_data_L[x][y][z] < s_small_rggb_L[x][z])
				{
					s_rggb_linear_data_L[x][y][z] = s_small_rggb_L[x][z];
				}

				if (s_rggb_linear_data_R[x][y][z] < s_small_rggb_R[x][z])
				{
					s_rggb_linear_data_R[x][y][z] = s_small_rggb_R[x][z];
				}
#ifdef LSH_DBG_LOG
                printf("---->data_L[%d][%d][%d] = %f, data_R[%d][%d][%d] = %f\n", x, y,z, s_rggb_linear_data_L[x][y][z], x, y,z, s_rggb_linear_data_R[x][y][z]);
#endif

                if (x == 0)
                {
                    s_ring_position_L[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_L[0] + s_radius[y]*g_radius_cos_table[z];
                    s_ring_position_L[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_L[1] + s_radius[y]*g_radius_sin_table[z];
                    
                    s_ring_position_R[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_R[0] + s_radius[y]*g_radius_cos_table[z];
                    s_ring_position_R[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_R[1] + s_radius[y]*g_radius_sin_table[z];
                }

			}
		}

		for (y = SMALL_RING_R; y < RING_CNT; y++)
		{
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				fSetupL = (float)(s_small_rggb_L[x][z] - s_center_rggb_L[x])*(s_linear_curve_L[x][y] - s_linear_curve_L[x][SMALL_RING_R])/(s_linear_curve_L[x][RING_CNT - 1] - s_linear_curve_L[x][SMALL_RING_R]);
				fSetupR = (float)(s_small_rggb_R[x][z] - s_center_rggb_R[x])*(s_linear_curve_R[x][y] - s_linear_curve_R[x][SMALL_RING_R])/(s_linear_curve_R[x][RING_CNT - 1] - s_linear_curve_R[x][SMALL_RING_R]);

				s_rggb_linear_data_L[x][y][z] = s_small_rggb_L[x][z] - fSetupL;
				s_rggb_linear_data_R[x][y][z] = s_small_rggb_R[x][z] - fSetupR;

				if (s_rggb_linear_data_L[x][y][z] < s_center_rggb_L[x])
				{
					s_rggb_linear_data_L[x][y][z] = s_center_rggb_L[x];
				}

				if (s_rggb_linear_data_R[x][y][z] < s_center_rggb_R[x])
				{
					s_rggb_linear_data_R[x][y][z] = s_center_rggb_R[x];
				}
				
                if (x == 0)
                {
                    s_ring_position_L[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_L[0] + s_radius[y]*g_radius_cos_table[z];
                    s_ring_position_L[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_L[1] + s_radius[y]*g_radius_sin_table[z];
                    
                    s_ring_position_R[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_R[0] + s_radius[y]*g_radius_cos_table[z];
                    s_ring_position_R[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_R[1] + s_radius[y]*g_radius_sin_table[z];
                }
			}
		}
#else					
		for (y = 0; y < RING_CNT; y++)
		{
#ifdef LSH_DBG_LOG
			printf("---->s_linear_curve_L[%d][%d] = %f, s_linear_curve_R[%d][%d] = %f\n", x, y, s_linear_curve_L[x][y], x, y, s_linear_curve_R[x][y]);
#endif
			for (z = 0; z < RING_SECTOR_CNT; z++)
			{
				fSetupL = (float)(s_big_rggb_L[x][z] - s_center_rggb_L[x])*s_linear_curve_L[x][y];//*sin(delta_degree*y);
				fSetupR = (float)(s_big_rggb_R[x][z] - s_center_rggb_R[x])*s_linear_curve_R[x][y];//*sin(delta_degree*y);
#ifdef SKIP_OTP_LSH_FOR_DEBUG
				s_rggb_linear_data_L[x][y][z] = 1.0;//s_big_rggb_L[x][z] - fSetupL;
				s_rggb_linear_data_R[x][y][z] = 1.0;//s_big_rggb_R[x][z] - fSetupR;
#else
                s_rggb_linear_data_L[x][y][z] = s_big_rggb_L[x][z] - fSetupL;
                s_rggb_linear_data_R[x][y][z] = s_big_rggb_R[x][z] - fSetupR;
#endif
				if (s_rggb_linear_data_L[x][y][z] < s_center_rggb_L[x])
				{
					s_rggb_linear_data_L[x][y][z] = s_center_rggb_L[x];
				}

				if (s_rggb_linear_data_R[x][y][z] < s_center_rggb_R[x])
				{
					s_rggb_linear_data_R[x][y][z] = s_center_rggb_R[x];
				}
				
				if (x == 0)
				{
					s_ring_position_L[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_L[0] + s_radius[y]*g_radius_cos_table[z];
					s_ring_position_L[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_L[1] + s_radius[y]*g_radius_sin_table[z];
					
					s_ring_position_R[y][z][0] = MAX_RADIUS+calibration_data->radius_center_offset_R[0] + s_radius[y]*g_radius_cos_table[z];
					s_ring_position_R[y][z][1] = MAX_RADIUS+calibration_data->radius_center_offset_R[1] + s_radius[y]*g_radius_sin_table[z];
				}
			}
		}
#endif
	}
#endif

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
    if (1)
    {
        int c;
        float *buff = NULL;
#else
    f = fopen(filename, "rb");
    if (f != NULL)
    {
        int c;
        IMG_UINT32 sizes[LSH_BIN_HEAD];
        char name[4];
        // 1 line buffer used only if compiled with LSH_FLOAT as double
        float *buff = NULL;

        if (fread(name, sizeof(char), 4, f) != 4)
        {
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (strncmp(name, LSH_HEAD, LSH_HEAD_SIZE - 1) != 0)
        {
            LOG_ERROR("Wrong LSH file format - %s couldn't be read\n",
                LSH_HEAD);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        if (fread(&c, sizeof(int), 1, f) != 1)
        {
            LOG_ERROR("Failed to read LSH file format\n");
            fclose(f);
            return IMG_ERROR_FATAL;
        }
        if (c != 1)
        {
            LOG_ERROR("wrong LSH file format - version %d found, "\
                "version %d supported\n", c, LSH_VERSION);
            fclose(f);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        if (fread(sizes, sizeof(IMG_UINT32), LSH_BIN_HEAD, f) != LSH_BIN_HEAD)
        {
            LOG_ERROR("failed to read %d int at the beginning of the file\n",
                LSH_BIN_HEAD);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        pLSH->ui16Width = (IMG_UINT16)sizes[0];
        pLSH->ui16Height = (IMG_UINT16)sizes[1];
        pLSH->ui16TileSize = (IMG_UINT16)sizes[2];
#endif

#ifdef INFOTM_LSH_FUNC
        printf("==>pLSH->ui16Width %d, pLSH->ui16Height %d, pLSH->ui16TileSize %d\n", pLSH->ui16Width, pLSH->ui16Height, pLSH->ui16TileSize);
#endif

        if (pLSH->ui16Width*pLSH->ui16Height == 0)
        {
            LOG_ERROR("lsh grid size is 0 (w=%d h=%d)!\n", pLSH->ui16Width, pLSH->ui16Height);
            fclose(f);
            return IMG_ERROR_FATAL;
        }

        if (GRID_DOUBLE_CHECK)
        {
            buff = (float*)IMG_MALLOC(
                pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
            if (buff == NULL)
            {
                LOG_ERROR("failed to allocate the output matrix\n");
                fclose(f);
                return IMG_ERROR_MALLOC_FAILED;
            }
        }

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            pLSH->apMatrix[c] = (LSH_FLOAT*)IMG_CALLOC(pLSH->ui16Width*pLSH->ui16Height, sizeof(LSH_FLOAT));

            if (pLSH->apMatrix[c] == NULL)
            {
                LOG_ERROR("Failed to allocate matrix for "\
                    "channel %d (%ld Bytes)\n",
                    c, pLSH->ui16Width*pLSH->ui16Height*sizeof(LSH_FLOAT));
                c--;
                while (c >= 0)
                {
                    IMG_FREE(pLSH->apMatrix[c]);
                    c--;
                }
                if (GRID_DOUBLE_CHECK) IMG_FREE(buff);

                return IMG_ERROR_MALLOC_FAILED;
            }
        }
		
#ifdef INFOTM_LSH_FUNC
		float R = 0, MAX_R_1;
		MAX_R_1 = MAX_RADIUS*3;
		float AbsVal = 0;
#endif

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            if (GRID_DOUBLE_CHECK)
            {
                int x, y;
	       	    int k = 0;
#ifdef INFOTM_LSH_FUNC
				R = 0;
#endif

                // matrix is double! so load in buff
                ret = fread(buff, sizeof(float),
                    pLSH->ui16Width*pLSH->ui16Height, f);
                if (ret != pLSH->ui16Width*pLSH->ui16Height)
                {
                    LOG_ERROR("Failed to read channel %d "\
                        "- read %d/%lu Bytes\n",
                        c, ret,
                        pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
                    IMG_FREE(buff);
                    fclose(f);
                    return IMG_ERROR_FATAL;
                }

#ifdef INFOTM_LSH_FUNC
				//if (runned_flag == 1)
				{
                    //printf("===>Marix %d\n", c);

                    for (y = 0; y < pLSH->ui16Height; y++)
                    {
                        for (x = 0; x < pLSH->ui16Width; x++)
                        {
                        	//pLSH->apMatrix[c][y*pLSH->ui16Width + x] = buff[k++];
                        	if (x <= MAX_RADIUS)
							{
								if (y < MAX_RADIUS+calibration_data->radius_center_offset_L[1])
								{
                        			R = (float)sqrt((MAX_RADIUS-x+calibration_data->radius_center_offset_L[0])*(MAX_RADIUS-x+calibration_data->radius_center_offset_L[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_L[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_L[1]));// - 3;
								}
								else
								{
									R = (float)sqrt((MAX_RADIUS-x+calibration_data->radius_center_offset_L[0])*(MAX_RADIUS-x+calibration_data->radius_center_offset_L[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_L[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_L[1]));// - 3;
								}
                        	}
							else
							{
								if (x >= MAX_RADIUS*2)
								{
									if (x <= MAX_R_1+calibration_data->radius_center_offset_R[0])
									{
										if (y < MAX_RADIUS+calibration_data->radius_center_offset_R[1])
										{
		                        			R = (float)sqrt((MAX_R_1-x+calibration_data->radius_center_offset_R[0])*(MAX_R_1-x+calibration_data->radius_center_offset_R[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_R[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_R[1]));// - 3;
										}
										else
										{
											R = (float)sqrt((MAX_R_1-x+calibration_data->radius_center_offset_R[0])*(MAX_R_1-x+calibration_data->radius_center_offset_R[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_R[0])*(y-MAX_RADIUS+calibration_data->radius_center_offset_R[1]));// - 3;
										}
		                        	}
									else
									{
										if (y < MAX_RADIUS+calibration_data->radius_center_offset_R[1])
										{
		                        			R = (float)sqrt((x-MAX_R_1+calibration_data->radius_center_offset_R[0])*(x-MAX_R_1+calibration_data->radius_center_offset_R[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_R[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_R[1]));// - 3;
										}
										else
										{
											R = (float)sqrt((x-MAX_R_1+calibration_data->radius_center_offset_R[0])*(x-MAX_R_1+calibration_data->radius_center_offset_R[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_R[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_R[0]));// - 3;
										}
									}
								}
								else
								{
									if (y < MAX_RADIUS+calibration_data->radius_center_offset_L[1])
									{
	                        			R = (float)sqrt((x-MAX_RADIUS+calibration_data->radius_center_offset_L[0])*(x-MAX_RADIUS+calibration_data->radius_center_offset_L[0]) + (MAX_RADIUS-y+calibration_data->radius_center_offset_L[1])*(MAX_RADIUS-y+calibration_data->radius_center_offset_L[1]));// - 3;
									}
									else
									{
										R = (float)sqrt((x-MAX_RADIUS+calibration_data->radius_center_offset_L[0])*(x-MAX_RADIUS+calibration_data->radius_center_offset_L[0]) + (y-MAX_RADIUS+calibration_data->radius_center_offset_L[1])*(y-MAX_RADIUS+calibration_data->radius_center_offset_L[1]));// - 3;
									}
								}
							}
							if (R < 0){R = 0;}
							pLSH->apMatrix[c][y*pLSH->ui16Width + x] = LSH_GetGainValue(R, x, y, !(x >= MAX_RADIUS*2), c, calibration_data);
                            //printf("%f,\n", pLSH->apMatrix[c][y*pLSH->ui16Width + x] );
                        }
					
					    //printf("-------\n");
                    }

         	        //printf("\n===>Marix %d End\n", c);
				}
#endif

            }
            else
            {
                ret = fread(pLSH->apMatrix[c], sizeof(float), pLSH->ui16Width*pLSH->ui16Height, f);
                if (ret != pLSH->ui16Width*pLSH->ui16Height)
                {
                    LOG_ERROR("Failed to read channel %d "\
                        "- read %d/%lu Bytes\n",
                        c, ret,
                        pLSH->ui16Width*pLSH->ui16Height*sizeof(float));
                    fclose(f);
                    return IMG_ERROR_FATAL;
                }
            }
        }

        if (GRID_DOUBLE_CHECK)
        {
            IMG_FREE(buff);
        }

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

    if (pLSH == NULL || filename == NULL)
    {
        LOG_DEBUG("NULL parameters\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    f = fopen(filename, "w");
    if (f != NULL)
    {
        int c, x, y;
        double min = 3.0, max = 0.0;

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            for (y = 0; y < pLSH->ui16Height; y++)
            {
                for (x = 0; x < pLSH->ui16Width; x++)
                {
                    if (pLSH->apMatrix[c][y*pLSH->ui16Width + x] < min)
                    {
                        min = pLSH->apMatrix[c][y*pLSH->ui16Width + x];
                    }
                    if (pLSH->apMatrix[c][y*pLSH->ui16Width + x] > max)
                    {
                        max = pLSH->apMatrix[c][y*pLSH->ui16Width + x];
                    }
                }
            }
        }
        if (min == max)
        {
            max += 1.0;
        }

        fprintf(f, "P2\n%u %u 255\n", pLSH->ui16Width, pLSH->ui16Height * 4);
        for (c = 0; c < LSH_MAT_NO; c++)
        {
            for (y = 0; y < pLSH->ui16Height; y++)
            {
                for (x = 0; x < pLSH->ui16Width; x++)
                {
                    int v = (int)(((pLSH->apMatrix[c][y*pLSH->ui16Width + x])
                        - min) / (max - min)) * 255;
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
//Eeprom Data smoothing
//p--one side eeprom data
//c--one channel data size
void LSH_fite_eeprom_data(int c,unsigned char *p)
{
  int  xpoint[4]={1,50,180,303};
  unsigned char  yvlaue[4]={0};
  float  value;
  float  k1,k2,k3,k4;
  int i,j;
  //one channel data buffer
  unsigned char *py=(unsigned char *)calloc(c , sizeof(unsigned char));
   if(NULL==py)
  {
  	printf("calloc-errpy\n");
  }
  //printf("sourse\n");
  //for(i=0;i<c*3;i++)
  // {
  //     printf("%d,",p[i]);
//	if(!(i%3))
//		printf("\n");
 //  }	

  xpoint[1]=(int)((c)*0.17);
  xpoint[2]=(int)((c)*0.6);
  xpoint[3]=(c)-1;

  for(i=0;i<3;i++)
  {
     
     for(j=0;j<c;j++)
     {
          py[j]=*(p+i+3*j); 
     }
     yvlaue[0]=py[xpoint[0]];
     yvlaue[1]=py[xpoint[1]];
     yvlaue[2]=py[xpoint[2]];
     yvlaue[3]=py[xpoint[3]];
     k1=(xpoint[0]-xpoint[1])*(xpoint[0]-xpoint[2])*(xpoint[0]-xpoint[3]);
     k2=(xpoint[1]-xpoint[0])*(xpoint[1]-xpoint[2])*(xpoint[1]-xpoint[3]);
     k3=(xpoint[2]-xpoint[0])*(xpoint[2]-xpoint[1])*(xpoint[2]-xpoint[3]);
     k4=(xpoint[3]-xpoint[0])*(xpoint[3]-xpoint[1])*(xpoint[3]-xpoint[2]);
	 	
      for(j=0;j<c;j++)
     {
            //Lagrange interpolation curve y=ax^3+bx^2+cx+d
          value=yvlaue[0]*(j-xpoint[1])*(j-xpoint[2])*(j-xpoint[3])/k1+yvlaue[1]*(j-xpoint[0])*(j-xpoint[2])*(j-xpoint[3])/k2
		  	+yvlaue[2]*(j-xpoint[1])*(j-xpoint[0])*(j-xpoint[3])/k3+yvlaue[3]*(j-xpoint[0])*(j-xpoint[1])*(j-xpoint[2])/k4;
	   if(value<0.01)
	   	value=0;
	   if(value>255.0)
	   	value=255;
          *(p+i+3*j)=(unsigned char)value;
      }
   }
  
  //for(i=0;i<c*3;i++)
   //{
    //    printf("%d,",p[i]);
//	if(!(i%3))
//		printf("\n");
 //  }	
   //printf("====\n");
   free(py);
}


