/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Abstract : H2 HEVC Encoder API
--
------------------------------------------------------------------------------*/

#ifndef API_H
#define API_H

#include "base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define EXPORT  __declspec(dllexport)
#endif

  /*------------------------------------------------------------------------------
      1. Type definition for encoder instance
  ------------------------------------------------------------------------------*/
  typedef const void *HEVCEncInst;


  /*------------------------------------------------------------------------------
      2. Enumerations for API parameters
  ------------------------------------------------------------------------------*/

  /* Function return values */
  typedef enum
  {
    HEVCENC_OK = 0,
    HEVCENC_FRAME_READY = 1,

    HEVCENC_ERROR = -1,
    HEVCENC_NULL_ARGUMENT = -2,
    HEVCENC_INVALID_ARGUMENT = -3,
    HEVCENC_MEMORY_ERROR = -4,
    HEVCENC_EWL_ERROR = -5,
    HEVCENC_EWL_MEMORY_ERROR = -6,
    HEVCENC_INVALID_STATUS = -7,
    HEVCENC_OUTPUT_BUFFER_OVERFLOW = -8,
    HEVCENC_HW_BUS_ERROR = -9,
    HEVCENC_HW_DATA_ERROR = -10,
    HEVCENC_HW_TIMEOUT = -11,
    HEVCENC_HW_RESERVED = -12,
    HEVCENC_SYSTEM_ERROR = -13,
    HEVCENC_INSTANCE_ERROR = -14,
    HEVCENC_HRD_ERROR = -15,
    HEVCENC_HW_RESET = -16
  } HEVCEncRet;

  /* Stream type for initialization */
  typedef enum
  {
    HEVCENC_BYTE_STREAM = 0,    /* Hevc annex B: NAL unit starts with
                                     * hex bytes '00 00 00 01' */
    HEVCENC_NAL_UNIT_STREAM = 1 /* Plain NAL units without startcode */
  } HEVCEncStreamType;


  /* Level for initialization */
  typedef enum
  {
    HEVCENC_LEVEL_1 = 30,
    HEVCENC_LEVEL_2 = 60,
    HEVCENC_LEVEL_2_1 = 63,
    HEVCENC_LEVEL_3 = 90,
    HEVCENC_LEVEL_3_1 = 93,
    HEVCENC_LEVEL_4 = 120,
    HEVCENC_LEVEL_4_1 = 123,
    HEVCENC_LEVEL_5 = 150,
    HEVCENC_LEVEL_5_1 = 153,
    HEVCENC_LEVEL_5_2 = 156,
    HEVCENC_LEVEL_6 = 180,
    HEVCENC_LEVEL_6_1 = 183,
    HEVCENC_LEVEL_6_2 = 186

  } HEVCEncLevel;

  /* Picture YUV type for initialization */
  typedef enum
  {
    HEVCENC_YUV420_PLANAR = 0,              /* YYYY... UUUU... VVVV...  */
    HEVCENC_YUV420_SEMIPLANAR,              /* YYYY... UVUVUV...        */
    HEVCENC_YUV420_SEMIPLANAR_VU,           /* YYYY... VUVUVU...        */
    HEVCENC_YUV422_INTERLEAVED_YUYV,        /* YUYVYUYV...              */
    HEVCENC_YUV422_INTERLEAVED_UYVY,        /* UYVYUYVY...              */
    HEVCENC_RGB565,                         /* 16-bit RGB 16bpp         */
    HEVCENC_BGR565,                         /* 16-bit RGB 16bpp         */
    HEVCENC_RGB555,                         /* 15-bit RGB 16bpp         */
    HEVCENC_BGR555,                         /* 15-bit RGB 16bpp         */
    HEVCENC_RGB444,                         /* 12-bit RGB 16bpp         */
    HEVCENC_BGR444,                         /* 12-bit RGB 16bpp         */
    HEVCENC_RGB888,                         /* 24-bit RGB 32bpp         */
    HEVCENC_BGR888,                         /* 24-bit RGB 32bpp         */
    HEVCENC_RGB101010,                      /* 30-bit RGB 32bpp         */
    HEVCENC_BGR101010                       /* 30-bit RGB 32bpp         */
  } HEVCEncPictureType;

  /* Picture rotation for pre-processing */
  typedef enum
  {
    HEVCENC_ROTATE_0 = 0,
    HEVCENC_ROTATE_90R = 1, /* Rotate 90 degrees clockwise */
    HEVCENC_ROTATE_90L = 2  /* Rotate 90 degrees counter-clockwise */
  } HEVCEncPictureRotation;

  /* Picture color space conversion (RGB input) for pre-processing */
  typedef enum
  {
    HEVCENC_RGBTOYUV_BT601 = 0, /* Color conversion according to BT.601 */
    HEVCENC_RGBTOYUV_BT709 = 1, /* Color conversion according to BT.709 */
    HEVCENC_RGBTOYUV_USER_DEFINED = 2   /* User defined color conversion */
  } HEVCEncColorConversionType;
  /* Picture type for encoding */
  typedef enum
  {
    HEVCENC_INTRA_FRAME = 0,
    HEVCENC_PREDICTED_FRAME = 1,
    HEVCENC_NOTCODED_FRAME  /* Used just as a return value */
  } HEVCEncPictureCodingType;

  /* Reference picture mode for reading and writing */
  typedef enum
  {
    HEVCENC_NO_REFERENCE_NO_REFRESH = 0,
    HEVCENC_REFERENCE = 1,
    HEVCENC_REFRESH = 2,
    HEVCENC_REFERENCE_AND_REFRESH = 3
  } HEVCEncRefPictureMode;

  /*------------------------------------------------------------------------------
      3. Structures for API function parameters
  ------------------------------------------------------------------------------*/

  /* Configuration info for initialization
   * Width and height are picture dimensions after rotation
   * Width and height are restricted by level limitations
   */
  typedef struct
  {
    HEVCEncStreamType streamType;   /* Byte stream / Plain NAL units */
#if 0
    HEVCEncViewMode viewMode;       /* Mode of stream to be generated and
                                           the corresponding amount of encoder
                                           internal frame buffers required */
#endif


    HEVCEncLevel level;
    /*        u32 ctb_size;*/
    u32 width;           /* Encoded picture width in pixels, multiple of 2 */
    u32 height;          /* Encoded picture height in pixels, multiple of 2 */
    u32 frameRateNum;    /* The stream time scale, [1..1048575] */
    u32 frameRateDenom;  /* Maximum frame rate is frameRateNum/frameRateDenom
                              * in frames/second. The actual frame rate will be
                              * defined by timeIncrement of encoded pictures,
                              * [1..frameRateNum] */
    u32 refFrameAmount; /* Amount of reference frame buffers, [1..2]
                             * 1 = only last frame buffered,
                             * 2 = last and long term frames buffered */
    u32 constrainedIntraPrediction; /* 0 = No constrains,
                                         * 1 = Only use intra neighbours */
    u32 strongIntraSmoothing;       /* 0 = Normal smoothing,
                                         * 1 = Strong smoothing. */
    u32 sliceSize;       /* slice size in ctu rows */


    u32 compressor;
    u32 interlacedFrame;
  } HEVCEncConfig;

  /* Defining rectangular macroblock area in encoder picture */
  typedef struct
  {
    u32 enable;         /* [0,1] Enables this area */
    u32 top;            /* Top macroblock row inside area [0..heightMbs-1] */
    u32 left;           /* Left macroblock row inside area [0..widthMbs-1] */
    u32 bottom;         /* Bottom macroblock row inside area [top..heightMbs-1] */
    u32 right;          /* Right macroblock row inside area [left..widthMbs-1] */
  } HEVCEncPictureArea;

  /* Coding control parameters */
  typedef struct
  {
    u32 sliceSize;       /* Slice size in macroblock rows,
                              * 0 to encode each picture in one slice,
                              * [0..height/ctu_size]
                              */
    u32 seiMessages;     /* Insert picture timing and buffering
                              * period SEI messages into the stream,
                              * [0,1]
                              */
    u32 videoFullRange;  /* Input video signal sample range, [0,1]
                              * 0 = Y range in [16..235],
                              * Cb&Cr range in [16..240]
                              * 1 = Y, Cb and Cr range in [0..255]
                              */
    u32 disableDeblockingFilter;    /* 0 = Filter enabled,
                                         * 1 = Filter disabled,
                                         * 2 = Filter disabled on slice edges */
    i32 tc_Offset;

    i32 beta_Offset;


    u32 enableDeblockOverride;
    u32 deblockOverride;

    u32 enableSao;

    u32 enableScalingList;

    u32 sampleAspectRatioWidth; /* Horizontal size of the sample aspect
                                     * ratio (in arbitrary units), 0 for
                                     * unspecified, [0..65535]
                                     */
    u32 sampleAspectRatioHeight;    /* Vertical size of the sample aspect ratio
                                         * (in same units as sampleAspectRatioWidth)
                                         * 0 for unspecified, [0..65535]
                                         */
    u32 cabacInitFlag;    /* [0,1] CABAC table initial flag */
    u32 cirStart;           /* [0..mbTotal] First macroblock for
                                   Cyclic Intra Refresh */
    u32 cirInterval;        /* [0..mbTotal] Macroblock interval for
                                   Cyclic Intra Refresh, 0=disabled */
    HEVCEncPictureArea intraArea;   /* Area for forcing intra macroblocks */
    HEVCEncPictureArea roi1Area;    /* Area for 1st Region-Of-Interest */
    HEVCEncPictureArea roi2Area;    /* Area for 2nd Region-Of-Interest */
    i32 roi1DeltaQp;                /* [-15..0] QP delta value for 1st ROI */
    i32 roi2DeltaQp;                /* [-15..0] QP delta value for 2nd ROI */

    i32 max_cu_size;    /* Max coding unit size in pixels */
    i32 min_cu_size;    /* Min coding unit size in pixels */
    i32 max_tr_size;    /* Max transform size in pixels */
    i32 min_tr_size;    /* Min transform size in pixels */
    i32 tr_depth_intra;   /* Max transform hierarchy depth */
    i32 tr_depth_inter;   /* Max transform hierarchy depth */

    u32 fieldOrder;         /* Field order for interlaced coding,
                                   0 = bottom field first, 1 = top field first */
    i32 chroma_qp_offset;    /* chroma qp offset */

    u32 insertrecoverypointmessage;     /* Insert recovery point SEI messages into the stream,
                              * [0,1]
                              */
    u32 recoverypointpoc;


  } HEVCEncCodingCtrl;

  /* Rate control parameters */
  typedef struct
  {
    u32 pictureRc;       /* Adjust QP between pictures, [0,1] */
    u32 pictureSkip;     /* Allow rate control to skip pictures, [0,1] */
    i32 qpHdr;           /* QP for next encoded picture, [-1..51]
                              * -1 = Let rate control calculate initial QP
                              * This QP is used for all pictures if
                              * HRD and pictureRc and mbRc are disabled
                              * If HRD is enabled it may override this QP
                              */
    u32 qpMin;           /* Minimum QP for any picture, [0..51] */
    u32 qpMax;           /* Maximum QP for any picture, [0..51] */
    u32 bitPerSecond;    /* Target bitrate in bits/second, this is
                              * needed if pictureRc, mbRc, pictureSkip or
                              * hrd is enabled [10000..60000000]
                              */
    u32 hrd;             /* Hypothetical Reference Decoder model, [0,1]
                              * restricts the instantaneous bitrate and
                              * total bit amount of every coded picture.
                              * Enabling HRD will cause tight constrains
                              * on the operation of the rate control
                              */
    u32 hrdCpbSize;      /* Size of Coded Picture Buffer in HRD (bits) */
    u32 gopLen;          /* Length for Group of Pictures, indicates
                              * the distance of two intra pictures,
                              * including first intra [1..300]
                              */
    i32 intraQpDelta;    /* Intra QP delta. intraQP = QP + intraQpDelta
                              * This can be used to change the relative quality
                              * of the Intra pictures or to lower the size
                              * of Intra pictures. [-12..12]
                              */
    u32 fixedIntraQp;    /* Fixed QP value for all Intra pictures, [0..51]
                              * 0 = Rate control calculates intra QP.
                              */
    u32 u32NewStream; /* patch by infotm reset ratectrl window*/
  } HEVCEncRateCtrl;

  /* Encoder input structure */
  typedef struct
  {
    u32 busLuma;         /* Bus address for input picture
                              * planar format: luminance component
                              * semiplanar format: luminance component
                              * interleaved format: whole picture
                              */
    u32 busChromaU;      /* Bus address for input chrominance
                              * planar format: cb component
                              * semiplanar format: both chrominance
                              * interleaved format: not used
                              */
    u32 busChromaV;      /* Bus address for input chrominance
                              * planar format: cr component
                              * semiplanar format: not used
                              * interleaved format: not used
                              */
    u32 timeIncrement;   /* The previous picture duration in units
                              * of 1/frameRateNum. 0 for the very first picture
                              * and typically equal to frameRateDenom for the rest.
                              */
    u32 *pOutBuf;        /* Pointer to output stream buffer */
    u32 busOutBuf;       /* Bus address of output stream buffer */
    u32 outBufSize;      /* Size of output stream buffer in bytes */
    HEVCEncPictureCodingType codingType;    /* Proposed picture coding type,
                                                 * INTRA/PREDICTED
                                                 */
    i32 poc;      /* encoded Picture order count */


  } HEVCEncIn;

  /* Encoder output structure */
  typedef struct
  {
    HEVCEncPictureCodingType codingType;    /* Realized picture coding type,
                                                 * INTRA/PREDICTED/NOTCODED
                                                 */
    u32 streamSize;      /* Size of output stream in bytes */

    u32 *pNaluSizeBuf;   /* Output buffer for NAL unit sizes
                              * pNaluSizeBuf[0] = NALU 0 size in bytes
                              * pNaluSizeBuf[1] = NALU 1 size in bytes
                              * etc
                              * Zero value is written after last NALU.
                              */
    u32 numNalus;        /* Amount of NAL units */

    u32 busScaledLuma;   /* Bus address for scaled encoder picture luma   */
    u8 *scaledPicture;   /* Pointer for scaled encoder picture            */

  } HEVCEncOut;

  /* Input pre-processing */
  typedef struct
  {
    HEVCEncColorConversionType type;
    u16 coeffA;          /* User defined color conversion coefficient */
    u16 coeffB;          /* User defined color conversion coefficient */
    u16 coeffC;          /* User defined color conversion coefficient */
    u16 coeffE;          /* User defined color conversion coefficient */
    u16 coeffF;          /* User defined color conversion coefficient */
  } HEVCEncColorConversion;

  typedef struct
  {
    u32 origWidth;                          /* Input camera picture width */
    u32 origHeight;                         /* Input camera picture height*/
    u32 xOffset;                            /* Horizontal offset          */
    u32 yOffset;                            /* Vertical offset            */
    HEVCEncPictureType inputType;           /* Input picture color format */
    HEVCEncPictureRotation rotation;        /* Input picture rotation     */
    u32 videoStabilization;                 /* Enable video stabilization */
    HEVCEncColorConversion colorConversion; /* Define color conversion
                                                   parameters for RGB input   */
    u32 scaledWidth;    /* Optional down-scaled output picture width,
                              multiple of 4. 0=disabled. [16..width] */
    u32 scaledHeight;   /* Optional down-scaled output picture height,
                              multiple of 2. [96..height]                    */

    u32 scaledOutput;                       /* Enable output of down-scaled
                                                   encoder picture. Dimensions
                                                   specified at Init.         */
    u32 *virtualAddressScaledBuff;
    u32 busAddressScaledBuff;
    u32 sizeScaledBuff;

    u32 interlacedFrame;                    /* Enable input frame format
                                                   with two interlaced fields.
                                                   Even pictures will be read
                                                   from top field, odd pictures
                                                   from bottom field.         */
  } HEVCEncPreProcessingCfg;

  /* Callback struct and function type. The callback is made by the encoder
   * when a slice is completed and available in the encoder stream output buffer. */

  typedef struct
  {
    u32 slicesReadyPrev;/* Indicates how many slices were completed at
                               previous callback. This is given because
                               several slices can be completed between
                               the callbacks. */
    u32 slicesReady;    /* Indicates how many slices are completed. */
    u32 nalUnitInfoNum;  /* Indicates how many information nal units are completed, including all kinds of sei information.*/
    u32 *sliceSizes;    /* Holds the size (bytes) of every completed slice. */
    u32 *pOutBuf;       /* Pointer to beginning of output stream buffer. */
    void *pAppData;     /* Pointer to application data. */
  } HEVCEncSliceReady;

  typedef void (*HEVCEncSliceReadyCallBackFunc)(HEVCEncSliceReady *sliceReady);

  /* Version information */
  typedef struct
  {
    u32 major;           /* Encoder API major version */
    u32 minor;           /* Encoder API minor version */
  } HEVCEncApiVersion;

  typedef struct
  {
    u32 swBuild;         /* Software build ID */
    u32 hwBuild;         /* Hardware build ID */
  } HEVCEncBuild;

  struct hevc_buffer
  {
    struct hevc_buffer *next;
    u8 *buffer;     /* Data store */
    u32 cnt;      /* Data byte cnt */
    u32 busaddr;      /* Data store bus address */
  };
  enum HEVCEncStatus
  {
    HEVCENCSTAT_INIT = 0xA1,
    HEVCENCSTAT_START_STREAM,
    HEVCENCSTAT_START_FRAME,
    HEVCENCSTAT_ERROR
  };



  /*------------------------------------------------------------------------------
      4. Encoder API function prototypes
  ------------------------------------------------------------------------------*/
 EXPORT HEVCEncRet HEVCEncOff(HEVCEncInst inst);
 EXPORT HEVCEncRet HEVCEncOn(HEVCEncInst inst);
 EXPORT HEVCEncRet HEVCEncInitOn(void);

  /* Version information */
  HEVCEncApiVersion HEVCEncGetApiVersion(void);
  HEVCEncBuild HEVCEncGetBuild(void);

  /* Helper for input format bit-depths */
  u32 HEVCEncGetBitsPerPixel(HEVCEncPictureType type);

  /* Initialization & release */
 EXPORT HEVCEncRet HEVCEncInit(HEVCEncConfig *config, HEVCEncInst *instAddr);
 EXPORT HEVCEncRet HEVCEncRelease(HEVCEncInst inst);

  /* Encoder configuration before stream generation */
 EXPORT HEVCEncRet HEVCEncSetCodingCtrl(HEVCEncInst instAddr, const HEVCEncCodingCtrl *pCodeParams);
 EXPORT HEVCEncRet HEVCEncGetCodingCtrl(HEVCEncInst inst,
                                  HEVCEncCodingCtrl *pCodeParams);

  /* Encoder configuration before and during stream generation */
 EXPORT HEVCEncRet HEVCEncSetRateCtrl(HEVCEncInst inst, const HEVCEncRateCtrl *pRateCtrl);
 EXPORT HEVCEncRet HEVCEncGetRateCtrl(HEVCEncInst inst, HEVCEncRateCtrl *pRateCtrl);

 EXPORT HEVCEncRet HEVCEncSetPreProcessing(HEVCEncInst inst, const HEVCEncPreProcessingCfg *pPreProcCfg);
 EXPORT HEVCEncRet HEVCEncGetPreProcessing(HEVCEncInst inst,
                                     HEVCEncPreProcessingCfg *pPreProcCfg);

  /* Encoder user data insertion during stream generation */
 EXPORT  HEVCEncRet HEVCEncSetSeiUserData(HEVCEncInst inst, const u8 *pUserData,
                                   u32 userDataSize);

  /* Stream generation */

  /* HEVCEncStrmStart generates the SPS and PPS. SPS is the first NAL unit and PPS
   * is the second NAL unit. NaluSizeBuf indicates the size of NAL units.
   */
 EXPORT HEVCEncRet HEVCEncStrmStart(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                              HEVCEncOut *pEncOut);

  /* HEVCEncStrmEncode encodes one video frame. If SEI messages are enabled the
   * first NAL unit is a SEI message. When MVC mode is selected first encoded
   * frame belongs to view=0 and second encoded frame belongs to view=1 and so on.
   * When MVC mode is selected a prefix NAL unit is generated before view=0 frames.
   */
 EXPORT HEVCEncRet HEVCEncStrmEncode(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                               HEVCEncOut *pEncOut,
                               HEVCEncSliceReadyCallBackFunc sliceReadyCbFunc,
                               void *pAppData);

  /* HEVCEncStrmEnd ends a stream with an EOS code. */

 EXPORT HEVCEncRet HEVCEncStrmEnd(HEVCEncInst inst, const HEVCEncIn *pEncIn,
                            HEVCEncOut *pEncOut);

 EXPORT HEVCEncRet HEVCEncCalculateInitialQp(HEVCEncInst inst, i32 *s32QpHdr, i32 s32BitRate);
 EXPORT HEVCEncRet HEVCEncGetRealQp(HEVCEncInst inst, i32 *s32QpHdr);

  /*  internal encoder testing */
  HEVCEncRet HEVCEncSetTestId(HEVCEncInst inst, u32 testId);

   /*------------------------------------------------------------------------------
      5. Encoder API tracing callback function
  ------------------------------------------------------------------------------*/

  void HEVCEncTrace(const char *msg);

#ifdef __cplusplus
}
#endif

#endif
