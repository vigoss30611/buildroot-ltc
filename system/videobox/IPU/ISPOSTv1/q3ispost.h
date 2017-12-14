// Hermite.h : the definitions
//

#ifndef __1D_HERMITE__
#define __1D_HERMITE__

#define Q3_ISP_VERISON "Q3 ISP VERSION 20150324-18:47"

#define Q3_DENOISE_REG_NUM     48
#define PERI_BASE_PA        (0x20000000)
#define ISPOST_BASE_ADDR    (PERI_BASE_PA+0x4300000)

typedef struct
{
    unsigned int reg;    // Q3 HW ISP post register.
    unsigned int value; // Q3 HW ISP post register value.
} Q3_REG_VALUE;

/*==========================================================================================================
//                        Q3 ISP POSE DN API
//==========================================================================================================
// int DN_HermiteGenerator(unsigned char *threshold, unsigned char weighting, Q3_REG_VALUE *hermitetable)
// threshold:
//                    This input parameter points to the array of de-noise threshold values. The array should contain
//                    at least 3 elements. De-noise threshold values in three channels are filled in this array (Ty Tu
//                    Tv). The higher the value, the stronger the de-noise effect. The values are expected to be even.
//                    When odd values are detected in the function, the values are converted to be the nearest
//                    lower even numbers. The range of acceptable values are 254~2. The default threshold values
//                    are set to 20 for all of three channels.
//                    threshold[0]: Y channel threshold.
//                    threshold[1]: U channel threshold.
//                    threshold[2]: V channel threshold.
// weighting:
//                    This input parameter is used to set the weighting factor of the current and referenced frame.
//                    The range of acceptable values are 5~1. The default values are set to 5.
// hermitetable:
//                    This output parameter returns a reference hermite table to the caller. The table contains a data
//                    structure that describes the Q3 hardware register address and its value. In order to fill in full
//                    range of the hermite table, at least 48 elements in array is required.
//
//==========================================================================================================*/
int DN_HermiteGenerator(unsigned char *threshold, unsigned char weighting, Q3_REG_VALUE *hermitetable);


/*=====================================/
                FE API
======================================*/

#define Q3_FISHEYE_REG_NUM 8

typedef struct
{
    Q3_REG_VALUE FE_REG[Q3_FISHEYE_REG_NUM];
    unsigned char *pGridDataBuf;
}Q3_FE_REG;

typedef struct _CMD_DATA_
{
    int input_width;
    int input_height;
    int output_width;
    int output_height;
    int fisheye_startPhi;
    int fisheye_endPhi;
    int fisheye_startTheta;
    int fisheye_endTheta;
    int fisheye_radius;
    int fisheye_gridSize;
    int fisheye_centerWidth;
    int fisheye_centerHeight;
    double scalingWidth;
    double scalingHeight;
    double rotateAngle;
    double rotateScale;
}FE_CMD_DATA;

int FE_DOWNVIEW_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);
int FE_FRONTVIEW_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);
int FE_CENTERSCALE_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);
int FE_ORIGINSCALE_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);
int FE_ROTATE_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);
int FE_UPVIEW_MODEL(FE_CMD_DATA *cmdparam, Q3_FE_REG *FE_Reg);

/*=====================================/
                VERSION
======================================*/
const char *ISPOST_GetVersion(void);

#endif//#ifndef __1D_HERMITE__
