
#ifndef __HLIB_FODET_H__
#define __HLIB_FODET_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define FODET_NODE "/dev/fodetv2"

#define FODET_MAGIC 'k'
#define FODET_IOCMAX    10
#define FODET_NUM   8
#define FODET_NAME  "fodetv2"

#define FODET_ENV           _IOW(FODET_MAGIC, 1, unsigned long)
#define FODET_INITIAL           _IOW(FODET_MAGIC, 2, unsigned long)
#define FODET_OBJECT_DETECT     _IOW(FODET_MAGIC, 3, unsigned long)
#define FODET_GET_COORDINATE _IOR(FODET_MAGIC, 4, unsigned long)
//---------------------------------------------------------------------------

typedef signed char       FODET_INT8;
typedef signed short      FODET_INT16;
typedef signed long       FODET_INT32;

typedef unsigned char     FODET_UINT8;
typedef unsigned short    FODET_UINT16;
typedef unsigned long     FODET_UINT32;

#define FODET_DUMPFILE        0
#define FODET_LOADFILE        1

#define FODET_DISABLE         0
#define FODET_ENABLE          1

#define FODET_OFF             0
#define FODET_ON              1

#define FODET_FALSE           0
#define FODET_TRUE            1

#define FODET_DONE            0
#define FODET_BUSY            1

#define FODET_EXIT_FAILURE    1
#define FODET_EXIT_SUCCESS    0

#define FODET_DUMP_TRIG_OFF   0
#define FODET_DUMP_TRIG_ON    2

#define FOD_SRC_IMG_STRIDE          (320)
#define FOD_SRC_IMG_WIDTH           (320)
#define FOD_SRC_IMG_HEIGHT          (240)
#define FOD_CROP_IMG_WIDTH           (188)
#define FOD_CROP_IMG_HEIGHT          (96)

#define FOD_SRC_IMG_Y_SIZE          (FOD_SRC_IMG_STRIDE * FOD_SRC_IMG_HEIGHT)
#define FOD_SRC_IMG_UV_SIZE         (FOD_SRC_IMG_Y_SIZE / 2)
#define FOD_SRC_IMG_SIZE            (FOD_SRC_IMG_Y_SIZE + FOD_SRC_IMG_UV_SIZE)

#define FOD_INTGRL_IMG_OUT_SIZE     (960*1024)
#define FOD_CLSFR_HID_CSCD_OUT_SIZE (52*1024)
#define FOD_INTGRL_IMG_OUT_STRIDE 0x0A
#define FOD_INTGRL_IMG_OUT_OFFSET 0x0500

#define FOD_TOTAL_IDX_START   0
#define FOD_TOTAL_IDX_END     24
#define MAX_ARY_SCAN_ALL 200

//================================================
typedef union _FODET_INFO_CTRL
{
    FODET_UINT32 ui32Ctrl;
    struct
    {
        FODET_UINT32 ui1SourceImageEn: 1;
        FODET_UINT32 ui1DatabaseEn: 1;
        FODET_UINT32 ui1DummyDbEn: 1;
        FODET_UINT32 ui1IISumEn: 1;
        FODET_UINT32 ui1HIDCCEn: 1;
        FODET_UINT32 ui1ArrayHIDCCEn: 1;
        FODET_UINT32 ui1DummyHIDCCEn: 1;
        FODET_UINT32 ui1StartIdxEn: 1;
        FODET_UINT32 ui1EndIdxEn: 1;
        FODET_UINT32 ui1IICacheEn: 1;
        FODET_UINT32 ui1HIDCacheEn: 1;
        FODET_UINT32 ui26Rev: 20;
        FODET_UINT32 ui1SrcImageCp: 1;      // load file used,  copy data used
    };
} FODET_INFO_CTRL, *PFODET_INFO_CTRL;

typedef struct _FODET_HIDCC_MEM
{
    FODET_UINT32 addr;
    FODET_UINT32 length;
} FODET_HIDCC_MEM, *PFODET_HIDCC_MEM;

typedef struct CvRect
{
    int x;
    int y;
    int w;
    int h;
} CvRect;

typedef struct CvAvgComp
{
    CvRect rect;
    int neighbors;
} CvAvgComp;

typedef struct CvPTreeNode
{
    struct CvPTreeNode* pParent;
    char* pElement;
    int iRank;
    int iIdxClass;
} CvPTreeNode;

typedef struct _OR_DATA{
        FODET_UINT16 scaling_factor;
        FODET_UINT16 eq_win_wd;
        FODET_UINT16 eq_win_ht;
        FODET_UINT8 step;
        FODET_UINT16 real_wd;
        FODET_UINT16 real_ht;
}OR_DATA, *POR_DATA;

typedef struct _FODET_COORDINATE{
        FODET_UINT16 ui16CoordinateNum;
        void*  pAvgComps1Addr;
}FODET_COORDINATE, *PFODET_COORDINATE;


typedef struct _FODET_USER
{
        FODET_UINT32 ui32FodetCount;
        FODET_INFO_CTRL stInfoCtrl;
        FODET_UINT16 ui16ImgFormat;    // YUV422 = 0, YUV411 = 1 .
        FODET_UINT16 ui32ImgOffset;
        FODET_UINT16 ui16ImgWidth;
        FODET_UINT16 ui16ImgHeight;
        FODET_UINT16 ui16ImgStride;
        FODET_UINT16 ui16CropXost;
        FODET_UINT16 ui16CropYost;
        FODET_UINT16 ui16CropWidth;
        FODET_UINT16 ui16CropHeight;
        FODET_UINT32 ui32SrcImgAddr;
        FODET_UINT32 ui32SrcImgLength;
        FODET_UINT32 ui32IISumAddr;
        FODET_UINT32 ui32IISumLength;
        FODET_UINT32 ui32DatabaseAddr;
        FODET_UINT32 ui32DatabaseLength;
        FODET_UINT32 ui32HIDCCAddr;
        FODET_UINT32 ui32HIDCCLength;
        FODET_UINT16 ui16IIoutOffset;
        FODET_UINT16 ui16IIoutStride;
        FODET_UINT16 ui16StartIndex;
        FODET_UINT16 ui16EndIndex;
        FODET_UINT16 ui16ScaleFactor;
        FODET_UINT16 ui16EqWinWd;
        FODET_UINT16 ui16EqWinHt;
        FODET_UINT16 ui16ShiftStep;
        FODET_UINT16 ui16StopWd;
        FODET_UINT16 ui16StopHt;
        FODET_UINT32 ui32RWA;
        FODET_UINT8 ui8CacheFlag;
        FODET_UINT32 ui32IICacheAddr;        
        FODET_UINT32 ui32HIDCacheAddr;
        FODET_UINT16 ui16CoordinateNum;
        FODET_UINT32 ui32CoordinateAddr;
        FODET_UINT32 ui32ORDataAddr;
}FODET_USER, *PFODET_USER;


//---------------------------------------------------------------------------

void fodet_debug(const char* function, int line, const char* format, ...);

#define FODET_DEBUG(...) fodet_debug(__FUNCTION__, __LINE__, __VA_ARGS__)

extern int fodet_open(void);
extern int fodet_close(void);
extern int fodet_get_env(void);
extern int fodet_initial(FODET_USER* pFodetUser);
extern int fodet_object_detect(FODET_USER* pFodetUser);
extern int fodet_get_coordinate(FODET_USER* pFodetUser);

#ifdef __cplusplus
}
#endif //__cplusplus




#endif //__HLIB_FODET_H__
