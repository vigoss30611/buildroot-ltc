/**
*******************************************************************************
 @file felix.h

 @brief Helper functions to implement a test application using the CI layer

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
#ifndef __FELIX_TEST__
#define __FELIX_TEST__

#ifdef __cplusplus
extern "C" {
#endif

#include <img_types.h>

#include <ci/ci_api_structs.h>
//#include <dg/dg_api_structs.h>
#include <felix_hw_info.h> // CI_N_CONTEXT CI_N_EXT_DATAGEN
#include <sim_image.h>

struct DG_PARAM
{
    IMG_CHAR *pszInputFLX;
    /** @brief Maximum number of frames to load from input file */
    IMG_INT iFrames;
    IMG_UINT aBlanking[2];

    IMG_BOOL8 bMIPIUsesLineFlag;
    IMG_BOOL8 bPreload;
    IMG_UINT uiMipiLanes;
};

struct IIFDG_PARAM
{
    IMG_CHAR *pszInputFLX;
    IMG_INT iFrames;
    IMG_UINT aBlanking[2];
    IMG_BOOL8 bPreload;
    IMG_UINT uiGasket;
};

#define N_HEAPS 7
#define LSH_N_CORNERS 5
struct CAPTURE_PARAM
{
    IMG_UINT ui8imager;
    /**
     * @brief Save config flag - same as HW save config flag - -1 means all 
     * supported ones
     */
    IMG_INT iStats;
    /** @brief Enable CRC generation (for HW testing) */
    IMG_BOOL8 bCRC;
    /** @brief enable timestamps output */
    IMG_BOOL8 bEnableTimestamps;
    /**
     * @brief update maxEncOutWidth maxEncOutHeight maxDispOutWidth
     * and maxDispOutHeight using pitch computed output size
     */
    IMG_BOOL8 bUpdateMaxSize;
    /**
     * @brief If true allocates buffer after configuration,
     * if false allocates buffer after starting
     * does not affect the shots
     */
    IMG_BOOL8 bAllocateLate;

    IMG_UINT uiSysBlack;
    /**
     * @brief 1st value is to fix multipliers and bit diffs while 2nd value
     * gives the right to change the given tile size to allow support of
     * size limitation from BRN56734
     */
    IMG_BOOL8 bFixLSH[2];
    /**
     * @brief 0 means no deshading, 1 means linear gen from 4 corners, 2 means 
     * bowl gen from 4 corners + centre
     */
    IMG_INT iUseDeshading;
    /** 
     * @brief Number of tiles for LSH (between LSH_TILE_MIN andLSH_TILE_MAX)
     */
    IMG_UINT uiLshTile;
    /** @brief Corners used to generate the LSH matrix */
    float lshCorners[LSH_GRADS_NO*LSH_N_CORNERS];
    /** @brief The Gradients - {X,Y} per channel */
    float lshGradients[2*LSH_GRADS_NO];
    /** @brief RLT mode */
    IMG_UINT ui32RLTMode;
    /** @brief RLT slopes */
    float aRLTSlopes[RLT_SLICE_N];
    IMG_UINT aRLTKnee[RLT_SLICE_N-1];

    /** @brief AWS curve coefficients if AWS_LINE_SEG_AVAILABLE */
    float aCurveCoeffX[AWS_LINE_SEG_N];
    /** @brief AWS curve coefficients if AWS_LINE_SEG_AVAILABLE */
    float aCurveCoeffY[AWS_LINE_SEG_N];
    /** @brief AWS curve offsets if AWS_LINE_SEG_AVAILABLE */
    float aCurveOffset[AWS_LINE_SEG_N];
    /** @brief AWS curve boundaries if AWS_LINE_SEG_AVAILABLE */
    float aCurveBoundary[AWS_LINE_SEG_N];

    IMG_INT iDPFEnabled;
    IMG_CHAR *pszDPFRead;
    
    IMG_UINT ui32ConfigBuffers;
    /** should be <=ui32ConfigBuffers */
    IMG_UINT ui32NEncTiled;
    /** should be <=ui32ConfigBuffers */
    IMG_UINT ui32NDispTiled;
    /** should be <=ui32ConfigBuffers */
    IMG_UINT ui32NHDRExtTiled;
    /**
     * @brief number of frames to run for - default 0 means run for the number
     * of frames in DG input
     */
    IMG_INT i32RunForFrames;
    /** @brief Also output raw HDR extraction if pszHDRextOutput */
    IMG_BOOL8 bAddRawHDR;

    /** @brief Filename as the output of the encoder pipeline */
    IMG_CHAR *pszEncoderOutput;
    /** @brief filename as the output of the display pipeline */
    IMG_CHAR *pszDisplayOutput;
    IMG_CHAR *pszDataExtOutput;
    /** @brief Filename as the output of the HDR extraction */
    IMG_CHAR *pszHDRextOutput;
    /** @brief Filename as the input for HDR insertion */
    IMG_CHAR *pszHDRInsInput;
    /** @brief Filename as the output of Raw2D extraction */
    IMG_CHAR *pszRaw2ExtDOutput;

    /** @brief Output raw encoder output (no stride removal) */
    IMG_BOOL8 bRawYUV;
    /** @brief Encoder should output 422 instead of 420 */
    IMG_BOOL8 bEncOut422;
    /** @brief Encoder should output 10b instead of 8b */
    IMG_BOOL8 bEncOut10b;
    /** @brief allocation size in bytes */
    IMG_UINT uiYUVAlloc;
    /** @brief stride size for Y and CbCr buffers in bytes */
    IMG_UINT uiYUVStrides[2];
    /** @brief offset in bytes for Y and CbCr buffers */
    IMG_UINT uiYUVOffsets[2];

    /** @brief Display should output RGB888 24b instead of RGB888 32b */
    IMG_BOOL8 bDispOut24;
    /** @brief Display should output RGB101010 32b instead of RGB888 32b */
    IMG_BOOL8 bDispOut10;
    /** @brief saving bitdepth */
    IMG_UINT uiDEBitDepth;
    /** @brief save raw RGB or raw Bayer (no stride removal) */
    IMG_BOOL8 bRawDisp;
    /** @brief allocation size in bytes */
    IMG_UINT uiDispAlloc;
    /** @brief stride in bytes */
    IMG_UINT uiDispStride;
    /** @brief offset in bytes */
    IMG_UINT uiDispOffset;

    /** @brief Raw 2D format bit depth to use (should be 10 or 12) */
    IMG_UINT uiRaw2DBitDepth;
    /** @brief Raw 2D raw output from HW */
    IMG_BOOL8 brawTiff;
    /** @brief allocation size in bytes */
    IMG_UINT uiRaw2DAlloc;
    /** @brief stride in bytes */
    IMG_UINT uiRaw2DStride;
    /** @brief offset in bytes */
    IMG_UINT uiRaw2DOffset;

    /** @brief allocation size in bytes */
    IMG_UINT uiHDRExtAlloc;
    /** @brief stride in bytes */
    IMG_UINT uiHDRExtStride;
    /** @brief offset in bytes */
    IMG_UINT uiHDRExtOffset;

    /** @brief Rescale provided HDR Insertion to 16b per pixels */
    IMG_BOOL8 bRescaleHDRIns;

    float escPitch[2];
    float dscPitch[2];

    /** @brief Encode stats parameters: Nb of lines */
    IMG_UINT uiENSNLines;

    /**
     * @brief Allow the usage of register override debug functions (only if 
     * compiled with support). 
     */
    IMG_UINT uiRegOverride; 
    int iIIFDG; // which internal DG it uses, -1 for none
};

struct CMD_PARAM
{
    /**
     * @brief Path to the transif library given by the simulator - only used 
     * with FAKE interface AND transif support at compilation
     */
    IMG_CHAR *pszTransifLib;
    /** @brief Parameter file to use to setup the simulator (-argfile file) */
    IMG_CHAR *pszTransifSimParam;
    /** @brief Specify whether timed model of transif should be used */
    IMG_BOOL8 bTransifUseTimedModel;
    /** @brief Turn on transif debug messages */
    IMG_BOOL8 bTransifDebug;
    /** @brief if Fake driver choose CSIM IP to connect to (TCP) */
    IMG_CHAR *pszSimIP;
    /** @brief if Fake driver choose CSIM port to connect to (TCP) */
    IMG_INT uiSimPort;
    /** @brief if Fake driver disables RDW in pdump generation */
    IMG_BOOL8 bEnableRDW;
    /** @brief CPU Page size in kB - only if fake driver */
    IMG_UINT uiCPUPageSize;
    IMG_UINT aHeapOffsets[N_HEAPS];

    IMG_BOOL8 bParallel;
    IMG_BOOL8 bIgnoreDPF;
    /** @brief configure the MMU usage when using fake driver */
    IMG_UINT uiUseMMU;
    /**
     * @brief external data generator PLL - only if fake driver with
     * external DG
     */
    IMG_UINT aEDGPLL[2];
    /** @brief do not write output to file */
    IMG_BOOL8 bNowrite;
    /** @brief when quitting only close connections and rely on CI to clean */
    IMG_BOOL8 bRoughExit;
    /** 
     * @brief 0 is none - 1 is read/write all regs - 2 is verify load structure
     */
    IMG_UINT uiRegisterTest;
    /** @brief prints the HW information at the beginning of the test */
    IMG_BOOL bPrintHWInfo;
    /** @brief 256 = 256Bx16 or 512 = 512Bx8 */
    IMG_UINT uiTilingScheme;
    /** @brief if non-0 enfore a tiling stride */
    IMG_UINT uiTilingStride;
    /** @brief write ExtDG converted frames to file */
    IMG_BOOL8 bWriteDGFrame;
    /**
     * @brief Gamma curve to use - see KRN_CI_DriverDefaultsGammaLUT() for 
     * valid values - only fake driver
     */
    IMG_UINT uiGammaCurve;
    IMG_UINT ui8DataExtraction;
    IMG_UINT aEncOutSelect[2];  // 1st value is context second value is pulse width

    struct DG_PARAM sDataGen[CI_N_EXT_DATAGEN];
    struct CAPTURE_PARAM sCapture[CI_N_CONTEXT];
    struct IIFDG_PARAM sIIFDG[CI_N_IIF_DATAGEN];
};

void init_Param(struct CMD_PARAM *param);
void dumpParameters(const struct CMD_PARAM *parameters, 
    const char *pszFilename);
void printConnection(const CI_CONNECTION *pConn, FILE *file);
void clean_Param(struct CMD_PARAM *param);

/** @brief similar to insmod */
int init_drivers(const struct CMD_PARAM *parameters);
/** @brief similar to rmmod */
int finalise_drivers();

/** @brief configures and run the drivers */
int run_drivers(struct CMD_PARAM *parameters);

#ifdef __cplusplus
}
#endif

#endif // __FELIX_TEST__
