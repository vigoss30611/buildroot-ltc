#ifndef __CI_FODET_H__
#define __CI_FODET_H__



#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus


#if defined(IMG_KERNEL_MODULE)
#define FODET_MSG_LOG(...)        printk(KERN_DEBUG __VA_ARGS__)
#define FODET_MSG_INFO(...)        printk(KERN_INFO __VA_ARGS__)
#define FODET_MSG_ERR(...)        printk(KERN_ERR __VA_ARGS__)
#else
#define FODET_MSG_LOG             printf
#endif //IMG_KERNEL_MODULE


//#define TURN_ON_TILTED_SUM


#if defined(IMG_KERNEL_MODULE)
#include <stddef.h> // size_t and ptrdiff_t
//#define TEST_FD_PERFORMANCE_TIME_MEASURE
#endif //IMG_KERNEL_MODULE


typedef int8_t                  FODET_INT8;
typedef int16_t                 FODET_INT16;
typedef int32_t                 FODET_INT32;
typedef int64_t                 FODET_INT64;

typedef uint8_t                 FODET_UINT8;
typedef uint16_t                FODET_UINT16;
typedef uint32_t                FODET_UINT32;
typedef uint64_t                FODET_UINT64;

typedef FODET_INT8 *            FODET_PINT8;
typedef FODET_INT16 *           FODET_PINT16;
typedef FODET_INT32 *           FODET_PINT32;
typedef FODET_INT64 *           FODET_PINT64;

typedef FODET_UINT8 *           FODET_PUINT8;
typedef FODET_UINT16 *          FODET_PUINT16;
typedef FODET_UINT32 *          FODET_PUINT32;
typedef FODET_UINT64 *          FODET_PUINT64;

typedef void *            FODET_PVOID;
typedef void *            FODET_HANDLE;

typedef FODET_INT32             FODET_RESULT;

typedef FODET_UINT8             FODET_BOOL8;
typedef FODET_UINT8             FODET_BYTE;
typedef size_t                  FODET_SIZE;
typedef uintptr_t               FODET_UINTPTR;
typedef ptrdiff_t               FODET_PTRDIFF;


#define FODET_DUMPFILE          0
#define FODET_LOADFILE          1

#define FODET_DISABLE           0
#define FODET_ENABLE            1

#define FODET_OFF             	0
#define FODET_ON              	1

#define FODET_FALSE             0
#define FODET_TRUE              1

#define FODET_DONE            	0
#define FODET_BUSY            	1

#define FODET_EXIT_FAILURE      1
#define FODET_EXIT_SUCCESS      0

#define FODET_DUMP_TRIG_OFF     0
#define FODET_DUMP_TRIG_ON      2


#define FOD_STATUS_DONE         0x00000000
#define FOD_STATUS_ENABLE     0x00000001
#define FOD_STATUS_BUSY         0x00000001 
#define FOD_STATUS_RESUME    0x00000002
#define FOD_STATUS_HOLD         0x00000002
#define FOD_STATUS_DETECTED 0x00000004
#define FOD_STATUS_RESET        0x00000004
#define FOD_STATUS_DEBUG       0x00000008

#define FOD_STATUS_HAR           0x00004000
#define FOD_STATUS_INT            0x01000000
#define MAX_ARY_SCAN_ALL 200


//================================================
typedef enum _INTGRL_IMG_MODE {
	INTGRL_IMG_MODE_SUM = 0,
	INTGRL_IMG_MODE_SQ_SUM,
	INTGRL_IMG_MODE_TILTED_SUM,
	INTGRL_IMG_MODE_MAX
} INTGRL_IMG_MODE, *PINTGRL_IMG_MODE;

typedef enum _SRC_IMG_FMT {
	SRC_IMG_FMT_YUV422 = 0,
	SRC_IMG_FMT_YUV411,
	SRC_IMG_FMT_MAX
} SRC_IMG_FMT, *PSRC_IMG_FMT;

typedef enum _FODET_STATUS {
	FODET_STATUS_DONE = 0,
	FODET_STATUS_ENABLE = 1,
	FODET_STATUS_BUSY = 1,
	FODET_STATUS_RESUME = 2,
	FODET_STATUS_HOLD = 2,
	FODET_STATUS_DETECTED = 4,
	FODET_STATUS_MAX
} FODET_STATUS, *PFODET_STATUS;

typedef enum _FODET_CLSIFR_IDX {
	FODET_CLSIFR_IDX_MEAN = 0,
	FODET_CLSIFR_IDX_VARIANCE,
	FODET_CLSIFR_IDX_STAGE_SUM,
	FODET_CLSIFR_IDX_M,
	FODET_CLSIFR_IDX_N,
	FODET_CLSIFR_IDX_MAX
} FODET_CLSIFR_IDX, *PFODET_CLSIFR_IDX;

typedef enum _FODET_INT_STATUS {
	FODET_INT_STATUS_OFF = 0,
	FODET_INT_STATUS_ON,
	FODET_INT_STATUS_MAX
} FODET_INT_STATUS, *PFODET_INT_STATUS;

typedef enum _FODET_WIN_SCAN_MODE {
	FODET_WIN_SCAN_LARGE2SMALL = 0,
	FODET_WIN_SCAN_SMALL2LARGE
} FODET_WIN_SCAN_MODE, *PFODET_WIN_SCAN_MODE;

//=======================================
typedef struct _SUM_RESULT {
	FODET_UINT32 ui32Mean;
	FODET_UINT32 ui32Variance;
	FODET_UINT32 ui32StageSum;
} SUM_RESULT, *PSUM_RESULT;

typedef union _FODET_INTGRL_IMG_CTRL {
	FODET_UINT32 ui32Ctrl;
	struct {
		FODET_UINT32 ui1EnBusy:1;
		FODET_UINT32 ui7Rev:7;
		FODET_UINT32 ui2IntgrlImgMode:2;
		FODET_UINT32 ui2Rev:2;
		FODET_UINT32 ui1SrcImgFmt:1;
		FODET_UINT32 ui1InternalSram:1;
		FODET_UINT32 ui18Rev:18;
	};
} FODET_INTGRL_IMG_CTRL, *PFODET_INTGRL_IMG_CTRL;

typedef struct _FODET_IMG_INFO {
	FODET_UINT32 ui32Addr;
	FODET_UINT16 ui16Stride;
	FODET_UINT16 ui16Width;
	FODET_UINT16 ui16Height;
	FODET_UINT16 ui16Offset;
        FODET_UINT16 ui16Format;    // YUV422 = 0, YUV411 = 1 .
} FODET_IMG_INFO, *PFODET_IMG_INFO;

typedef struct _FODET_BUS_CTRL {
	FODET_UINT8 ui8MemWriteBurstSize;
	FODET_UINT8 ui8MemWriteThreshold;
	FODET_UINT8 ui8MemReadThreshold;
} FODET_BUS_CTRL, *PFODET_BUS_CTRL;

typedef union _FODET_CLSFR_CTRL {
	FODET_UINT32 ui32Ctrl;
	struct {
		FODET_UINT32 ui1EnBusy:1;
		FODET_UINT32 ui1ResumeHold:1;
		FODET_UINT32 ui1ResetDetected:1;
		FODET_UINT32 ui1Debug:1;
		FODET_UINT32 ui4Rev:4;
		FODET_UINT32 ui8YCoordinate:8;
		FODET_UINT32 ui9XCoordinate:9;
		FODET_UINT32 ui7Rev:7;
	};
} FODET_CLSFR_CTRL, *PFODET_CLSFR_CTRL;

typedef union _FODET_CLSFR_SCALE_SIZE {
	FODET_UINT32 ui32Value;
	struct {
		FODET_UINT32 ui14ScalingFactor:14;
		FODET_UINT32 ui2Rev:2;
		FODET_UINT32 ui8EqWinWidth:8;
		FODET_UINT32 ui8EqWinHeight:8;
	};
} FODET_CLSFR_SCALE_SIZE, *PFODET_CLSFR_SCALE_SIZE;

typedef union _FODET_CLSFR_WND_LMT {
	FODET_UINT32 ui32Value;
	struct {
		FODET_UINT32 ui14Step:14;
		FODET_UINT32 ui2Rev:2;
		FODET_UINT32 ui8StopWidth:8;
		FODET_UINT32 ui7StopHeight:7;
		FODET_UINT32 ui1Rev:1;
	};
} FODET_CLSFR_WND_LMT, *PFODET_CLSFR_WND_LMT;

typedef struct _FODET_AXI_ID {
	FODET_UINT8 ui8IMCID;
	FODET_UINT8 ui8HCID;
	FODET_UINT8 ui8CID;
	FODET_UINT8 ui8SRCID;
	FODET_UINT8 ui8FWID;
	FODET_UINT8 ui8HCCID;
} FODET_AXI_ID, *PFODET_AXI_ID;

typedef union _FODET_CACHE_CTRL {
	FODET_UINT32 ui32Ctrl;
	struct {
		FODET_UINT32 ui1FodMstrEn:1;
		FODET_UINT32 ui1FodMstrRst:1;
		FODET_UINT32 ui6Rev:6;
		FODET_UINT32 ui1EnIntgrlImgCache:1;
		FODET_UINT32 ui1EnHidCascadeCache:1;
		FODET_UINT32 ui1LdIntgrlImgCache:1;
		FODET_UINT32 ui1LdHidCascadeCache:1;
		FODET_UINT32 ui1RstIntgrlImgCache:1;
		FODET_UINT32 ui1RstHidCascadeCache:1;
		FODET_UINT32 ui1HoldAftrRenew:1;
		FODET_UINT32 ui9Rev:9;
		FODET_UINT32 ui1IntStat:1;
		FODET_UINT32 ui7Rev:7;
	};
} FODET_CACHE_CTRL, *PFODET_CACHE_CTRL;


typedef struct _FODET_INTGRL_IMG_INFO {
	FODET_INTGRL_IMG_CTRL stIntgrlImgCtrl;
	FODET_IMG_INFO stIntgrlImgSrcInfo;
	FODET_IMG_INFO stIntgrlImgOutInfo;
	FODET_BUS_CTRL stIntgrlImgBusCtrl;
	FODET_CACHE_CTRL stCacheCtrl;   // Master reset/enable
} FODET_INTGRL_IMG_INFO, *PFODET_INTGRL_IMG_INFO;

typedef struct _FODET_CLSFR_INFO {
	FODET_CLSFR_CTRL stClsfrCtrl;
	FODET_UINT32 ui32ClsfrCscdImgAddr;
	FODET_UINT32 ui32ClsfrHidCscdOutAddr;
	FODET_IMG_INFO stClsfrIntgrlImgOut;
	FODET_BUS_CTRL stClsfrIntgrlImgBusCtrl;
	FODET_CLSFR_SCALE_SIZE stClsfrScaleSize;
	FODET_CLSFR_WND_LMT stClsfrWndLmt;
	FODET_UINT16 ui16ReciprocalWndArea;
} FODET_CLSFR_INFO, *PFODET_CLSFR_INFO;

typedef struct _FODET_CACHE_INFO {
	FODET_UINT32 ui32Addr;
	FODET_UINT16 ui16Stride;
	FODET_UINT16 ui16Offset;
	FODET_UINT32 ui32LoadAddr;
	FODET_UINT32 ui32LoadLocalAddr;
	FODET_UINT32 ui32LoadLocalLength;
	FODET_UINT32 ui32HidCascadeCacheAddr;
} FODET_CACHE_INFO, *PFODET_CACHE_INFO;


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

typedef union _FODET_INFO_CTRL 
{
	FODET_UINT32 ui32Ctrl;
	struct 
	{
            FODET_UINT32 ui1SourceImageEn:1;
            FODET_UINT32 ui1DatabaseEn:1;
            FODET_UINT32 ui1DummyDbEn:1;
            FODET_UINT32 ui1IISumEn:1;
            FODET_UINT32 ui1HIDCCEn:1;
            FODET_UINT32 ui1ArrayHIDCCEn:1;
            FODET_UINT32 ui1DummyHIDCCEn:1;
            FODET_UINT32 ui1StartIdxEn:1;
            FODET_UINT32 ui1EndIdxEn:1;
            FODET_UINT32 ui1IICacheEn:1;
            FODET_UINT32 ui1HIDCacheEn:1;
            FODET_UINT32 ui26Rev:20;
            FODET_UINT32 ui1SrcImageCp:1;       // load file used,  copy data used
	};
} FODET_INFO_CTRL, *PFODET_INFO_CTRL;

typedef struct _FODET_HIDCC_MEM
{
	FODET_UINT32 addr;
	FODET_UINT32 length;
}FODET_HIDCC_MEM, *PFODET_HIDCC_MEM;


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


//=============================================
extern FODET_UINT32 g_ui32FodetRegAddr;
extern FODET_UINT32 g_ui32FodetRegExpectedData;
extern FODET_UINT32 g_ui32FodetRegData;
extern CvAvgComp avgComps1[MAX_ARY_SCAN_ALL];
extern FODET_INT16 iCandidateCount;


extern
void
fodet_module_enable(
	void
	);

extern
void
fodet_module_disable(
	void
	);

extern
FODET_INTGRL_IMG_CTRL
FodetIntgrlImgCtrlGet(
	void
	);

extern
FODET_RESULT
FodetIntgrlImgCtrlSet(
	FODET_INTGRL_IMG_CTRL stFodetIntgrlImgCtrl
	);

extern
void
FodetIntgrlImgTrigger(
	FODET_INTGRL_IMG_CTRL stFodetIntgrlImgCtrl
	);

extern
FODET_RESULT
FodetStatusChk(
	FODET_UINT8 ui8StatusDoneBusy
	);

extern
FODET_STATUS
FodetStatusGet(
	void
	);

extern
FODET_RESULT
FodetIntgrlImgSrcAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetIntgrlImgSrcStrideSet(
	FODET_UINT16 ui16Stride
	);

extern
FODET_RESULT
FodetIntgrlImgSrcSizeSet(
	FODET_UINT16 ui16Width,
	FODET_UINT16 ui16Height
	);

extern
FODET_RESULT
FodetIntgrlImgSrcSet(
	FODET_IMG_INFO stIntgrlImgSrcInfo
	);

extern
FODET_RESULT
FodetIntgrlImgOutAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetIntgrlImgOutStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	);

extern
FODET_RESULT
FodetIntgrlImgOutSet(
	FODET_IMG_INFO stIntgrlImgOutInfo
	);

extern
FODET_RESULT
FodetIntgrlImgBusCtrlSet(
	FODET_UINT8 ui8MemWriteBurstSize,
	FODET_UINT8 ui8MemWriteThreshold,
	FODET_UINT8 ui8MemReadThreshold
	);

extern
FODET_UINT32
FodetClsfrCtrlGet(
	void
	);

extern
FODET_RESULT
FodetClsfrCtrlSet(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	);

extern
void
FodetClsfrReset(
	void
	);

extern
void
FodetClsfrTrigger(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	);

extern
void
FodetClsfrResumeAndTrigger(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	);

extern
FODET_RESULT
FodetClsfrStatusDetectedChk(
	FODET_UINT8 ui8StatusResetDetected
	);

extern
FODET_RESULT
FodetClsfrStatusHoldChk(
	FODET_UINT8 ui8StatusResumeHold
	);

extern
FODET_RESULT
FodetClsfrStatusChk(
	FODET_UINT8 ui8StatusDoneBusy
	);

extern
FODET_STATUS
FodetClsfrStatusGet(
	void
	);

extern
FODET_RESULT
FodetClsfrCascadeImgAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetClsfrHidCascadeOutAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetClsfrIntgrlImgOutAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetClsfrIntgrlImgOutStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	);

extern
FODET_RESULT
FodetClsfrIntgrlImgOutSet(
	FODET_IMG_INFO stClsfrIntgrlImgOutInfo
	);

extern
FODET_RESULT
FodetClsfrIntgrlImgBusCtrlSet(
	FODET_UINT8 ui8MemWriteBurstSize,
	FODET_UINT8 ui8MemWriteThreshold,
	FODET_UINT8 ui8MemReadThreshold
	);

extern
FODET_RESULT
FodetClsfrIntgrlImgScaleSizeSet(
	FODET_UINT16 ui16ScalingFactor,
	FODET_UINT8 ui8EqWinWidth,
	FODET_UINT8 ui8EqWinHeight
	);

extern
FODET_RESULT
FodetClsfrWndLmtSet(
	FODET_CLSFR_WND_LMT stClsfrWndLmt
	);

extern
FODET_RESULT
FodetClsfrReciprocalWndAreaSet(
	FODET_UINT16 ui16ReciprocalWndArea
	);

extern
FODET_RESULT
FodetClsfrStatusIdxSet(
	FODET_UINT8 ui8ClsfrStatusIdx
	);

extern
FODET_RESULT
FodetClsfrStatusValChk(
	FODET_UINT32 ui32FodetClsfrStatusVal
	);

extern
FODET_UINT32
FodetClsfrStatusValGet(
	void
	);

extern
FODET_RESULT
FodetClsfrStatusValSet(
	FODET_UINT32 ui32FodetClsfrStatusVal
	);

extern
FODET_RESULT
FodetAxiCtrl0Set(
	FODET_UINT8 ui8IMCID,
	FODET_UINT8 ui8HCID,
	FODET_UINT8 ui8CID,
	FODET_UINT8 ui8SRCID
	);

extern
FODET_RESULT
FodetAxiCtrl1Set(
	FODET_UINT8 ui8FWID,
	FODET_UINT8 ui8HCCID
	);

extern
FODET_RESULT
FodetAxiCtrlSet(
	FODET_AXI_ID stAxiId
	);

extern
FODET_UINT32
FodetCacheCtrlGet(
	void
	);

extern
FODET_RESULT
FodetCacheCtrlSet(
	FODET_CACHE_CTRL stFodetCacheCtrl
	);

extern
FODET_RESULT
FodetMasterReset(
	FODET_CACHE_CTRL stFodetCacheCtrl
	);

extern
void
FodetMasterReset2(
	void
	);

extern
FODET_RESULT
FodetMasterEnable(
	FODET_CACHE_CTRL *pstFodetCacheCtrl
	);

extern
FODET_RESULT
FodetIntStatusChk(
	FODET_UINT8 ui8IntOffOn,
	FODET_CACHE_CTRL stFodetCacheCtrl
	);

extern
FODET_CACHE_CTRL
FodetIntStatusGet(
	void
	);

extern
void
FodetIntStatusClear(
	void
	);

extern
FODET_RESULT
FodetIntgrlImgCacheAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetIntgrlImgCacheStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	);

extern
FODET_RESULT
FodetIntgrlImgCacheLoadAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetIntgrlImgCacheLoadLocalAddrSet(
	FODET_UINT32 ui32Addr
	);

extern
FODET_RESULT
FodetIntgrlImgCacheLoadLocalLengthSet(
	FODET_UINT32 ui32Length
	);

extern
FODET_RESULT
FodetHidCascadeCacheAddrSet(
	FODET_UINT32 ui32Addr
	);

//===== Dump Registers and Memory =====
extern
void
FodetRegDump(
	char* pszFilename
	);

extern
void
FodetMemoryDump(
	char* pszFilename,
	FODET_UINT32 ui32StartAddress,
	FODET_UINT32 ui32Length
	);

//=============================

extern
FODET_RESULT
FodetIntgrlImgSet(
	FODET_INTGRL_IMG_INFO stIntgrlImgInfo
	);

extern
FODET_RESULT
FodetClsfrInfoSet(
	FODET_CLSFR_INFO stClsfrInfo
	);

extern
FODET_RESULT
FodetCacheInfoSet(
	FODET_CACHE_INFO stCacheInfo
	);

extern
void 
InitGroupingArray_Scanall(
        void);

extern
FODET_UINT16 
CheckStartIdx(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_IntegralImage(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_RenewRun(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_RenewRunWithCache(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_IntegralImage_pollng(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_RenewRun_pollng(FODET_USER* pFodetUser);

extern
FODET_INT32 
fodet_RenewRunWithCache_pollng(FODET_USER* pFodetUser);

extern 
int 
fodet_ioremap(void);

extern
void 
fodet_iounmap(void);

extern
FODET_RESULT 
FodetUserSetup(FODET_USER* pFodetUser);

extern
FODET_RESULT 
FodetUserObjectRecongtion(FODET_USER* pFodetUser);


#if defined(__cplusplus)
}
#endif //__cplusplus

#endif // __CI_FODET_H__
