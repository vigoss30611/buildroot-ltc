#ifndef __CI_ISPOST_H__
#define __CI_ISPOST_H__


#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus


#if defined(IMG_KERNEL_MODULE)
#define ISPOST_LOG(...)        printk(KERN_DEBUG __VA_ARGS__)
#else
#define ISPOST_LOG             printf
#endif //IMG_KERNEL_MODULE


#if defined(IMG_KERNEL_MODULE)
//#include <stdint.h>
#include <stddef.h> // size_t and ptrdiff_t
//#include <wchar.h>


//typedef __int8                 int8_t;
//typedef __int16                	int16_t;
//typedef __int32                int32_t;
//typedef __int64                	int64_t;
//
//typedef unsigned __int8       uint8_t;
//typedef unsigned __int16      uint16_t;
//typedef unsigned __int32      uint32_t;
//typedef unsigned __int64      uint64_t;

#endif //IMG_KERNEL_MODULE
typedef int8_t                 ISPOST_INT8;
typedef int16_t                ISPOST_INT16;
typedef int32_t                ISPOST_INT32;
typedef int64_t                ISPOST_INT64;

typedef uint8_t                ISPOST_UINT8;
typedef uint16_t               ISPOST_UINT16;
typedef uint32_t               ISPOST_UINT32;
typedef uint64_t               ISPOST_UINT64;

typedef ISPOST_INT8 *          ISPOST_PINT8;
typedef ISPOST_INT16 *         ISPOST_PINT16;
typedef ISPOST_INT32 *         ISPOST_PINT32;
typedef ISPOST_INT64 *         ISPOST_PINT64;

typedef ISPOST_UINT8 *         ISPOST_PUINT8;
typedef ISPOST_UINT16 *        ISPOST_PUINT16;
typedef ISPOST_UINT32 *        ISPOST_PUINT32;
typedef ISPOST_UINT64 *        ISPOST_PUINT64;


typedef ISPOST_INT32           ISPOST_RESULT;

typedef ISPOST_UINT8           ISPOST_BOOL8;
typedef ISPOST_UINT8           ISPOST_BYTE;
typedef size_t                 ISPOST_SIZE;
typedef uintptr_t              ISPOST_UINTPTR;
typedef ptrdiff_t              ISPOST_PTRDIFF;


#define ISPOST_DUMPFILE        0
#define ISPOST_LOADFILE        1

#define ISPOST_DISABLE         0
#define ISPOST_ENABLE          1

#define ISPOST_OFF             0
#define ISPOST_ON              1

#define ISPOST_FALSE           0
#define ISPOST_TRUE            1

#define ISPOST_DONE            0
#define ISPOST_BUSY            1

#define ISPOST_EXIT_FAILURE    1
#define ISPOST_EXIT_SUCCESS    0

#define ISPOST_DUMP_TRIG_OFF   0
#define ISPOST_DUMP_TRIG_ON    2

#define ISPOST_SS_SF_MAX       (1 << 12)

#define ISPOST_UDP_LC			0x0001
#define ISPOST_UDP_OV			0x0002
#define ISPOST_UDP_DN			0x0004
#define ISPOST_UDP_SS0			0x0008
#define ISPOST_UDP_SS1			0x0010
#define ISPOST_UDP_ALL			(ISPOST_UDP_LC | ISPOST_UDP_OV | ISPOST_UDP_DN | ISPOST_UDP_SS0 | ISPOST_UDP_SS1)

#define ISPOST_PRN_LC			0x0001
#define ISPOST_PRN_OV			0x0002
#define ISPOST_PRN_DN			0x0004
#define ISPOST_PRN_SS0			0x0008
#define ISPOST_PRN_SS1			0x0010
#define ISPOST_PRN_CTRL			0x0020
#define ISPOST_PRN_ALL			(ISPOST_PRN_LC | ISPOST_PRN_OV | ISPOST_PRN_DN | ISPOST_PRN_SS0 | ISPOST_PRN_SS1 | ISPOST_PRN_CTRL)


//================================================
// Define Lens Distortion input image information.
#define LC_SRC_IMG_STRIGE        (640)
#define LC_SRC_IMG_WIDTH         (640)
#define LC_SRC_IMG_HEIGHT        (480)
#define LC_SRC_IMG_Y_SIZE        (LC_SRC_IMG_STRIGE * LC_SRC_IMG_HEIGHT)
#define LC_SRC_IMG_UV_SIZE       (LC_SRC_IMG_Y_SIZE / 2)
#define LC_SRC_IMG_SIZE          (LC_SRC_IMG_Y_SIZE + LC_SRC_IMG_UV_SIZE)
// Define Lens Distortion grid buffer information.
//#define LC_GRID_BUF_GRID_SIZE    (GRID_SIZE_16_16)
#define LC_GRID_BUF_GRID_SIZE    (1)
#define LC_GRID_BUF_STRIDE       (1936)
#define LC_GRID_BUF_LENGTH       (133584)
// Define Lens Distortion Pixel cache mode information.
//#define LC_CACHE_MODE_CTRL_CFG   (CACHE_MODE_2)
#define LC_CACHE_MODE_CTRL_CFG   (CACHE_MODE_4)
//#define LC_CACHE_MODE_CBCR_SWAP  (CBCR_SWAP_MODE_SWAP)
#define LC_CACHE_MODE_CBCR_SWAP  (CBCR_SWAP_MODE_NOSWAP)
// Define Lens Distortion Scan mode information.
#define LC_SCAN_MODE_SCAN_W      (SCAN_WIDTH_16)
#define LC_SCAN_MODE_SCAN_H      (SCAN_HEIGHT_16)
#if 1
#define LC_SCAN_MODE_SCAN_M      (SCAN_MODE_LR_TB)
#else
#define LC_SCAN_MODE_SCAN_M      (SCAN_MODE_TB_LR)
#endif
// Define Lens Distortion Destination image information.
#define LC_DST_SIZE_WIDTH        (1920)
#define LC_DST_SIZE_HEIGHT       (1080)
// Define Lens Distortion Back Fill Color.
#if 1
// Pink color.
#define LC_BACK_FILL_Y           (0xFF)
#define LC_BACK_FILL_CB          (0x55)
#define LC_BACK_FILL_CR          (0xAA)
#else
// White color.
#define LC_BACK_FILL_Y           (0xFF)
#define LC_BACK_FILL_CB          (0x80)
#define LC_BACK_FILL_CR          (0x80)
#endif
//================================================
// Define Overlay image information.
#define OV_IMG_STRIDE            (128)
#define OV_IMG_WIDTH             (128)
#define OV_IMG_HEIGHT            (72)
#define OV_IMG_Y_SIZE            (OV_IMG_STRIDE * OV_IMG_HEIGHT)
#define OV_IMG_UV_SIZE           (OV_IMG_Y_SIZE / 2)
#define OV_IMG_SIZE              (OV_IMG_Y_SIZE + OV_IMG_UV_SIZE)
// Define Overlay position information.
#define OV_IMG_OV_WIDTH          (80)
#define OV_IMG_OV_HEIGHT         (64)
#if 1
#define OV_IMG_OV_OFFSET_X       (80)
#define OV_IMG_OV_OFFSET_Y       (80)
#else
#define OV_IMG_OV_OFFSET_X       (128)
#define OV_IMG_OV_OFFSET_Y       (128)
#endif
// Define Overlay mode information.
#define OV_IMG_OV_MODE           (OVERLAY_MODE_YUV)
//================================================
// Define 3D-Denoise reference image information.
#define DN_REF_IMG_STRIDE       LC_DST_SIZE_WIDTH
#define DN_REF_IMG_WIDTH        LC_DST_SIZE_WIDTH
#define DN_REF_IMG_HEIGHT       LC_DST_SIZE_HEIGHT
#define DN_REF_IMG_Y_SIZE       (DN_REF_IMG_STRIDE * DN_REF_IMG_HEIGHT)
#define DN_REF_IMG_UV_SIZE      (DN_REF_IMG_Y_SIZE / 2)
#define DN_REF_IMG_SIZE         (DN_REF_IMG_Y_SIZE + DN_REF_IMG_UV_SIZE)
//================================================
// Define 3D-Denoise output image information.
#define DN_OUT_IMG_STRIDE       LC_DST_SIZE_WIDTH
#define DN_OUT_IMG_WIDTH        LC_DST_SIZE_WIDTH
#define DN_OUT_IMG_HEIGHT       LC_DST_SIZE_HEIGHT
#define DN_OUT_IMG_Y_SIZE       (DN_OUT_IMG_STRIDE * DN_OUT_IMG_HEIGHT)
#define DN_OUT_IMG_UV_SIZE      (DN_OUT_IMG_Y_SIZE / 2)
#define DN_OUT_IMG_SIZE         (DN_OUT_IMG_Y_SIZE + DN_OUT_IMG_UV_SIZE)
//================================================
// Define Scaling stream 0 output image information.
#define SS0_OUT_IMG_STRIDE      (800)
#define SS0_OUT_IMG_WIDTH       (800)
#define SS0_OUT_IMG_HEIGHT      (480)
#define SS0_OUT_IMG_Y_SIZE      (SS0_OUT_IMG_WIDTH * SS0_OUT_IMG_HEIGHT)
#define SS0_OUT_IMG_UV_SIZE     (SS0_OUT_IMG_Y_SIZE / 2)
#define SS0_OUT_IMG_SIZE        (SS0_OUT_IMG_Y_SIZE + SS0_OUT_IMG_UV_SIZE)
// Define Scaling stream 0 horizontal scale factor and mode information.
#define SS0_OUT_IMG_HSF_SF      (1708)
#define SS0_OUT_IMG_HSF_SM      (SCALING_MODE_SCALING_DOWN_1)
// Define Scaling stream 0 vertical scale factor and mode information.
//#define SS0_OUT_IMG_VSF_SF      (1824)
#define SS0_OUT_IMG_VSF_SF      (1840)
#define SS0_OUT_IMG_VSF_SM      (SCALING_MODE_SCALING_DOWN_1)
//================================================
// Define Scaling stream 0 output image information.
#define SS1_OUT_IMG_STRIDE      (1280)
#define SS1_OUT_IMG_WIDTH       (1280)
#define SS1_OUT_IMG_HEIGHT      (720)
#define SS1_OUT_IMG_Y_SIZE      (SS1_OUT_IMG_WIDTH * SS1_OUT_IMG_HEIGHT)
#define SS1_OUT_IMG_UV_SIZE     (SS1_OUT_IMG_Y_SIZE / 2)
#define SS1_OUT_IMG_SIZE        (SS1_OUT_IMG_Y_SIZE + SS1_OUT_IMG_UV_SIZE)
// Define Scaling stream 1 horizontal scale factor and mode information.
#define SS1_OUT_IMG_HSF_SF      (2732)
#define SS1_OUT_IMG_HSF_SM      (SCALING_MODE_SCALING_DOWN_1)
// Define Scaling stream 1 vertical scale factor and mode information.
#define SS1_OUT_IMG_VSF_SF      (2734)
#define SS1_OUT_IMG_VSF_SM      (SCALING_MODE_SCALING_DOWN_1)
//================================================


typedef enum _GRID_SIZE {
	GRID_SIZE_8_8 = 0,
	GRID_SIZE_16_16,
	GRID_SIZE_32_32,
	GRID_SIZE_MAX
} GRID_SIZE, *PGRID_SIZE;

typedef enum _CACHE_MODE {
	CACHE_MODE_1 = 1,
	CACHE_MODE_2,
	CACHE_MODE_3,
	CACHE_MODE_4,
	CACHE_MODE_5,
	CACHE_MODE_6,
	CACHE_MODE_MAX
} CACHE_MODE, *PCACHE_MODE;

typedef enum _CBCR_SWAP_MODE {
	CBCR_SWAP_MODE_NOSWAP = 0,
	CBCR_SWAP_MODE_SWAP,
	CBCR_SWAP_MODE_MAX
} CBCR_SWAP_MODE, *PCBCR_SWAP_MODE;

typedef enum _SCAN_WIDTH {
	SCAN_WIDTH_8 = 0,
	SCAN_WIDTH_16,
	SCAN_WIDTH_32,
	SCAN_WIDTH_MAX
} SCAN_WIDTH, *PSCAN_WIDTH;

typedef enum _SCAN_HEIGHT {
	SCAN_HEIGHT_8 = 0,
	SCAN_HEIGHT_16,
	SCAN_HEIGHT_32,
	SCAN_HEIGHT_MAX
} SCAN_HEIGHT, *PSCAN_HEIGHT;

typedef enum _SCAN_MODE {
	SCAN_MODE_LR_TB = 0,
	SCAN_MODE_LR_BT,
	SCAN_MODE_RL_TB,
	SCAN_MODE_RL_BT,
	SCAN_MODE_TB_LR,
	SCAN_MODE_BT_LR,
	SCAN_MODE_TB_RL,
	SCAN_MODE_BT_RL,
	SCAN_MODE_MAX
} SCAN_MODE, *PSCAN_MODE;

typedef enum _OVERLAY_MODE {
	OVERLAY_MODE_UV = 0,
	OVERLAY_MODE_Y,
	OVERLAY_MODE_YUV,
	OVERLAY_MODE_MAX
} OVERLAY_MODE, *POVERLAY_MODE;

typedef enum _DN_MSK_CRV {
	DN_MSK_CRV_DNMC0 = 0,
	DN_MSK_CRV_DNMC1,
	DN_MSK_CRV_DNMC2,
	DN_MSK_CRV_DNMC3,
	DN_MSK_CRV_DNMC4,
	DN_MSK_CRV_DNMC5,
	DN_MSK_CRV_DNMC6,
	DN_MSK_CRV_DNMC7,
	DN_MSK_CRV_DNMC8,
	DN_MSK_CRV_DNMC9,
	DN_MSK_CRV_DNMC10,
	DN_MSK_CRV_DNMC11,
	DN_MSK_CRV_DNMC12,
	DN_MSK_CRV_DNMC13,
	DN_MSK_CRV_DNMC14,
	DN_MSK_CRV_MAX
} DN_MSK_CRV, *PDN_MSK_CRV;

typedef enum _DN_MSK_CRV_IDX {
	DN_MSK_CRV_IDX_Y = 0,
	DN_MSK_CRV_IDX_U,
	DN_MSK_CRV_IDX_V,
	DN_MSK_CRV_IDX_MAX
} DN_MSK_CRV_IDX, *PDN_MSK_CRV_IDX;

typedef enum _SCALING_STREAM {
	SCALING_STREAM_0 = 0,
	SCALING_STREAM_1,
	SCALING_STREAM_MAX
} SCALING_STREAM, *PSCALING_STREAM;

typedef enum _SCALING_MODE {
	SCALING_MODE_SCALING_DOWN = 0,
	SCALING_MODE_SCALING_DOWN_1,
	SCALING_MODE_NO_SCALING,
	SCALING_MODE_NO_SCALING_1,
	SCALING_MODE_MAX
} SCALING_MODE, *PSCALING_MODE;

typedef enum _DUMP_FILE_ID {
	DUMP_FILE_ID_DN_Y = 2,
	DUMP_FILE_ID_DN_UV,
	DUMP_FILE_ID_SS0_Y,
	DUMP_FILE_ID_SS0_UV,
	DUMP_FILE_ID_SS1_Y,
	DUMP_FILE_ID_SS1_UV,
	DUMP_FILE_ID_MAX
} DUMP_FILE_ID, *PDUMP_FILE_ID;


typedef union _ISPOST_CTRL0 {
	ISPOST_UINT32 ui32Ctrl0;
	struct {
		ISPOST_UINT32 ui1En:1;
		ISPOST_UINT32 ui1Rst:1;
		ISPOST_UINT32 ui6Rev:6;
		ISPOST_UINT32 ui1EnLC:1;
		ISPOST_UINT32 ui1EnOV:1;
		ISPOST_UINT32 ui1EnDN:1;
		ISPOST_UINT32 ui1EnSS0:1;
		ISPOST_UINT32 ui1EnSS1:1;
		ISPOST_UINT32 ui11Rev:11;
		ISPOST_UINT32 ui1Int:1;
		ISPOST_UINT32 ui7Rev:4;
	};
} ISPOST_CTRL0, *PISPOST_CTRL0;

typedef union _ISPOST_STAT0 {
	ISPOST_UINT32 ui32Stat0;
	struct {
		ISPOST_UINT32 ui1Stat0:1;
		ISPOST_UINT32 ui1StatDn:1;
		ISPOST_UINT32 ui1StatSS0:1;
		ISPOST_UINT32 ui1StatSS1:1;
		ISPOST_UINT32 ui28Rev:28;
	};
} ISPOST_STAT0, *PISPOST_STAT0;

typedef struct _IMG_INFO {
	ISPOST_UINT32 ui32YAddr;
	ISPOST_UINT32 ui32UVAddr;
	ISPOST_UINT16 ui16Stride;
	ISPOST_UINT16 ui16Width;
	ISPOST_UINT16 ui16Height;
} IMG_INFO, *PIMG_INFO;

typedef struct _LC_GRID_BUF_INFO {
	ISPOST_UINT32 ui32BufAddr;
	ISPOST_UINT16 ui16Stride;
	ISPOST_UINT8 ui8LineBufEnable;
	ISPOST_UINT8 ui8Size;
} LC_GRID_BUF_INFO, *PLC_GRID_BUF_INFO;

typedef struct _LC_PXL_CACHE_MODE {
	ISPOST_UINT8 ui8CtrlCfg;
	ISPOST_UINT8 ui8CBCRSwap;
} LC_PXL_CACHE_MODE, *PLC_PXL_CACHE_MODE;

typedef struct _LC_PXL_SCAN_MODE {
	ISPOST_UINT8 ui8ScanW;
	ISPOST_UINT8 ui8ScanH;
	ISPOST_UINT8 ui8ScanM;
} LC_PXL_SCAN_MODE, *PLC_PXL_SCAN_MODE;

typedef struct _LC_PXL_DST_SIZE {
	ISPOST_UINT16 ui16Width;
	ISPOST_UINT16 ui16Height;
} LC_PXL_DST_SIZE, *PLC_PXL_DST_SIZE;

typedef struct _LC_AXI_ID {
	ISPOST_UINT8 ui8SrcReqLmt;
	ISPOST_UINT8 ui8GBID;
	ISPOST_UINT8 ui8SRCID;
} LC_AXI_ID, *PLC_AXI_ID;

typedef struct _LC_INFO {
	IMG_INFO stSrcImgInfo;
	LC_GRID_BUF_INFO stGridBufInfo;
	LC_PXL_CACHE_MODE stPxlCacheMode;
	LC_PXL_SCAN_MODE stPxlScanMode;
	LC_PXL_DST_SIZE stPxlDstSize;
	LC_AXI_ID stAxiId;
} LC_INFO, *PLC_INFO;

typedef struct _OV_OFFSET {
	ISPOST_UINT16 ui16X;
	ISPOST_UINT16 ui16Y;
} OV_OFFSET, *POV_OFFSET;

typedef union _OV_ALPHA_VALUE {
	ISPOST_UINT32 ui32Alpha;
	struct {
	ISPOST_UINT8 ui8Alpha0:8;
	ISPOST_UINT8 ui8Alpha1:8;
	ISPOST_UINT8 ui8Alpha2:8;
	ISPOST_UINT8 ui8Alpha3:8;
	};
} OV_ALPHA_VALUE, *POV_ALPHA_VALUE;

typedef struct _OV_INFO {
	IMG_INFO stOvImgInfo;
	ISPOST_UINT8 ui8OverlayMode;
	OV_OFFSET stOvOffset;
	ISPOST_UINT8 ui8OVID;
} OV_INFO, *POV_INFO;

typedef struct _OV_ALPHA {
	OV_ALPHA_VALUE stValue0;
	OV_ALPHA_VALUE stValue1;
	OV_ALPHA_VALUE stValue2;
	OV_ALPHA_VALUE stValue3;
} OV_ALPHA, *POV_ALPHA;

typedef union _MSK_CRV_INFO {
	ISPOST_UINT32 ui32MskCrv;
	struct {
		ISPOST_UINT8 ui8X;
		ISPOST_UINT8 ui8Y;
		ISPOST_UINT16 ui3R:3;
		ISPOST_UINT16 ui1Rev:1;
		ISPOST_UINT16 ui12S:12;
	};
} MSK_CRV_INFO, *PMSK_CRV_INFO;

typedef struct _DN_MSK_CRV_INFO {
	ISPOST_UINT8 ui8MskCrvIdx;
	MSK_CRV_INFO stMskCrv[DN_MSK_CRV_MAX];
} DN_MSK_CRV_INFO, *PDN_MSK_CRV_INFO;

typedef struct _DN_AXI_ID {
	ISPOST_UINT8 ui8RefWrID;
	ISPOST_UINT8 ui8RefRdID;
} DN_AXI_ID, *PDN_AXI_ID;

typedef struct _DN_INFO {
	IMG_INFO stRefImgInfo;
	IMG_INFO stOutImgInfo;
	DN_AXI_ID stAxiId;
} DN_INFO, *PDN_INFO;

typedef struct _SCALING_FACTOR {
	ISPOST_UINT16 ui16SF;
	ISPOST_UINT8 ui8SM;
} SS_SCALING_FACTOR, *PSS_SCALING_FACTOR;

typedef struct _SS_INFO {
	ISPOST_UINT8 ui8StreamSel;
	IMG_INFO stOutImgInfo;
	SS_SCALING_FACTOR stHSF;
	SS_SCALING_FACTOR stVSF;
	ISPOST_UINT8 ui8SSWID;
} SS_INFO, *PSS_INFO;



extern ISPOST_UINT32 g_ui32RegAddr;
extern ISPOST_UINT32 g_ui32RegExpectedData;
extern ISPOST_UINT32 g_ui32RegData;


extern
ISPOST_RESULT
ispost_module_enable(
	void
	);

extern
void
ispost_module_disable(
	void
	);

//===== Lens correction (distortion correction) =====
extern
ISPOST_RESULT
IspostLcSrcImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	);

extern
ISPOST_RESULT
IspostLcSrcImgSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	);

extern
ISPOST_RESULT
IspostLcSrcImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	);

extern
ISPOST_RESULT
IspostLcBackFillColorSet(
	ISPOST_UINT8 ui8BackFillY,
	ISPOST_UINT8 ui8BackFillCB,
	ISPOST_UINT8 ui8BackFillCR
	);

extern
ISPOST_UINT32
IspostLcGridBufStrideCalc(
	ISPOST_UINT16 ui16DstWidth,
	ISPOST_UINT8 ui8Size
	);

extern
ISPOST_UINT32
IspostLcGridBufLengthCalc(
	ISPOST_UINT16 ui32DstHeight,
	ISPOST_UINT32 ui32Stride,
	ISPOST_UINT8 ui8Size
	);

extern
ISPOST_RESULT
IspostLcGridBufSetWithStride(
	ISPOST_UINT32 ui32BufAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT8 ui8LineBufEnable,
	ISPOST_UINT8 ui8Size
	);

extern
ISPOST_RESULT
IspostLcGridBufSetWithDestSize(
	ISPOST_UINT32 ui32BufAddr,
	ISPOST_UINT16 ui16DstWidth,
	ISPOST_UINT16 ui16DstHeight,
	ISPOST_UINT8 ui8Size
	);

extern
ISPOST_RESULT
IspostLcPxlCacheModeSet(
	ISPOST_UINT8 ui8CtrlCfg,
	ISPOST_UINT8 ui8CBCRSwap
	);

extern
ISPOST_RESULT
IspostLcPxlScanModeSet(
	ISPOST_UINT8 ui8ScanW,
	ISPOST_UINT8 ui8ScanH,
	ISPOST_UINT8 ui8ScanM
	);

extern
ISPOST_RESULT
IspostLcPxlDstSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	);

extern
ISPOST_RESULT
IspostLcAxiIdSet(
	ISPOST_UINT8 ui8SrcReqLmt,
	ISPOST_UINT8 ui8GBID,
	ISPOST_UINT8 ui8SRCID
	);

extern
ISPOST_RESULT
IspostLenCorrectionSet(
	LC_INFO stLcInfo
	);

//===== Image Overlay =====
extern
ISPOST_RESULT
IspostOvImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	);

extern
ISPOST_RESULT
IspostOvImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	);

extern
ISPOST_RESULT
IspostOvModeSet(
	ISPOST_UINT8 ui8OverlayMode
	);

extern
ISPOST_RESULT
IspostOvOffsetSet(
	ISPOST_UINT16 ui16X,
	ISPOST_UINT16 ui16Y
	);

extern
ISPOST_RESULT
IspostOvAlphaValueSet(
	OV_ALPHA_VALUE stValue0,
	OV_ALPHA_VALUE stValue1,
	OV_ALPHA_VALUE stValue2,
	OV_ALPHA_VALUE stValue3
	);

extern
ISPOST_RESULT
IspostOvAxiIdSet(
	ISPOST_UINT8 ui8OVID
	);

extern
ISPOST_RESULT
IspostOverlayAlphaValueSet(
	OV_ALPHA stOvAlpha
	);

extern
ISPOST_RESULT
IspostOverlaySet(
	OV_INFO stOvInfo
	);

//===== 3D Denoise =====
extern
ISPOST_RESULT
IspostDnRefImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	);

extern
ISPOST_RESULT
IspostDnRefImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	);

extern
ISPOST_RESULT
IspostDnOutImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	);

extern
ISPOST_RESULT
IspostDnOutImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	);

extern
ISPOST_RESULT
Ispost3DDenoiseMskCrvSet(
	DN_MSK_CRV_INFO stDnMskCrvInfo
	);

extern
ISPOST_RESULT
IspostDnAxiIdSet(
	ISPOST_UINT8 ui8RefWrID,
	ISPOST_UINT8 ui8RefRdID
	);

extern
ISPOST_RESULT
Ispost3DDenoiseSet(
	DN_INFO stDnInfo
	);

//===== Scale =====
extern
ISPOST_UINT32
IspostScalingFactotCal(
	ISPOST_UINT32 ui32SrcSize,
	ISPOST_UINT32 ui32DstSize
	);

extern
ISPOST_UINT32
IspostScalingDstSizeCal(
	ISPOST_UINT32 ui32SrcSize,
	ISPOST_UINT32 ui32SaclingFactor
	);

extern
ISPOST_RESULT
IspostSSOutImgAddrSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	);

extern
ISPOST_RESULT
IspostSSOutImgUpdate(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	);

extern
ISPOST_RESULT
IspostSSOutImgSet(
	SS_INFO stSsInfo
	);

extern
ISPOST_RESULT
IspostScaleStreamSet(
	SS_INFO stSsInfo
	);

//===== ISPOST control =====
extern
void
IspostActivateReset(
	void
	);

extern
void
IspostDeActivateReset(
	void
	);

extern
void
IspostSetCtrl(
	ISPOST_CTRL0 stCtrl0
	);

extern
void
IspostProcessReset(
	void
	);

extern
void
IspostProcessDisable(
	void
	);

extern
ISPOST_RESULT
IspostCtrl0Chk(
	ISPOST_CTRL0 stCtrl0
	);

extern
ISPOST_RESULT
IspostIntChk(
	ISPOST_UINT8 ui8IntOffOn
	);

extern
ISPOST_CTRL0
IspostInterruptGet(
	void
	);

extern void
IspostInterruptClear(
	void
	);

extern
ISPOST_STAT0
IspostStatusGet(
	void
	);

extern
void
IspostResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	);

extern
void
IspostTrigger(
	ISPOST_CTRL0 stCtrl0
	);

extern
void
IspostResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	);

//===== Dump Image to Memory =====
//extern
//void
//IspostDumpImage(
//	ISPOST_PVOID pDumpBuf,
//	ISPOST_UINT32 ui32DumpSize,
//	ISPOST_UINT8 ui8FileID
//	);
//
#if defined(IMG_KERNEL_MODULE)
//===== Dump Registers and Memory =====
extern
void
IspostRegDump(
	char* pszFilename
	);

extern
void
IspostMemoryDump(
	char* pszFilename,
	ISPOST_UINT32 ui32StartAddress,
	ISPOST_UINT32 ui32Length
	);

#endif //IMG_KERNEL_MODULE
//===== Single API interface test =====
extern
ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pSet(
	ISPOST_CTRL0 stCtrl0,
	LC_INFO * pstLcInfo,
	OV_INFO * pstOvInfo,
	DN_INFO * pstDnInfo,
	SS_INFO * pstSS0Info,
	SS_INFO * pstSS1Info
	);

extern
ISPOST_RESULT
IspostLcOvDn1080pSS0800x480SS1720pInitial(
	ISPOST_UINT32 ui32LcSrcWidth,
	ISPOST_UINT32 ui32LcSrcHeight,
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT8 ui8CBCRSwap,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32LcGridBufAddr,
	ISPOST_UINT32 ui32OvImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	);

extern
ISPOST_RESULT
IspostLcOvDn1080pSS0800x480SS1720pSetBuf(
	ISPOST_UINT32 ui32LcSrcWidth,
	ISPOST_UINT32 ui32LcSrcHeight,
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	);

extern
ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pInitial(
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT8 ui8CBCRSwap,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32LcGridBufAddr,
	ISPOST_UINT32 ui32OvImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	);

extern
ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pSetBuf(
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	);

//---------------------------------------------------------------------------
typedef struct ISPOST_USER
{
	//Ispost control
	ISPOST_CTRL0 stCtrl0;
	ISPOST_UINT32 ui32UpdateFlag;
	ISPOST_UINT32 ui32PrintFlag;
	ISPOST_UINT32 ui32IspostCount;

	//Lens correction information
	ISPOST_UINT32 ui32LcSrcBufYaddr;
	ISPOST_UINT32 ui32LcSrcBufUVaddr;
	ISPOST_UINT32 ui32LcSrcWidth;
	ISPOST_UINT32 ui32LcSrcHeight;
	ISPOST_UINT32 ui32LcSrcStride;
	ISPOST_UINT32 ui32LcDstWidth;
	ISPOST_UINT32 ui32LcDstHeight;
	ISPOST_UINT32 ui32LcGridBufAddr;
	ISPOST_UINT32 ui32LcGridBufSize;
	ISPOST_UINT32 ui32LcGridTargetIdx;
	ISPOST_UINT32 ui32LcGridLineBufEn;
	ISPOST_UINT32 ui32LcGridSize;
	ISPOST_UINT32 ui32LcCacheModeCtrlCfg;
	ISPOST_UINT32 ui32LcCBCRSwap;
	ISPOST_UINT32 ui32LcScanModeScanW;
	ISPOST_UINT32 ui32LcScanModeScanH;
	ISPOST_UINT32 ui32LcScanModeScanM;
	ISPOST_UINT32 ui32LcFillY;
	ISPOST_UINT32 ui32LcFillCB;
	ISPOST_UINT32 ui32LcFillCR;

	//Overlay image information
	ISPOST_UINT32 ui32OvMode;
	ISPOST_UINT32 ui32OvImgBufYaddr;
	ISPOST_UINT32 ui32OvImgBufUVaddr;
	ISPOST_UINT32 ui32OvImgWidth;
	ISPOST_UINT32 ui32OvImgHeight;
	ISPOST_UINT32 ui32OvImgStride;
	ISPOST_UINT32 ui32OvImgOffsetX;
	ISPOST_UINT32 ui32OvImgOffsetY;
	ISPOST_UINT32 ui32OvAlphaValue0;
	ISPOST_UINT32 ui32OvAlphaValue1;
	ISPOST_UINT32 ui32OvAlphaValue2;
	ISPOST_UINT32 ui32OvAlphaValue3;

	//3D-denoise information
	ISPOST_UINT32 ui32DnOutImgBufYaddr;
	ISPOST_UINT32 ui32DnOutImgBufUVaddr;
	ISPOST_UINT32 ui32DnOutImgBufStride;
	ISPOST_UINT32 ui32DnRefImgBufYaddr;
	ISPOST_UINT32 ui32DnRefImgBufUVaddr;
	ISPOST_UINT32 ui32DnRefImgBufStride;
	ISPOST_UINT32 ui32DnTargetIdx;
	DN_MSK_CRV_INFO stDnMskCrvInfo[DN_MSK_CRV_IDX_MAX];

	//Scaling stream 0 information
	ISPOST_UINT32 ui32SS0OutImgBufYaddr;
	ISPOST_UINT32 ui32SS0OutImgBufUVaddr;
	ISPOST_UINT32 ui32SS0OutImgBufStride;
	ISPOST_UINT32 ui32SS0OutImgDstWidth;
	ISPOST_UINT32 ui32SS0OutImgDstHeight;
	ISPOST_UINT32 ui32SS0OutImgMaxWidth;
	ISPOST_UINT32 ui32SS0OutImgMaxHeight;
	ISPOST_UINT32 ui32SS0OutHorzScaling;
	ISPOST_UINT32 ui32SS0OutVertScaling;

	//Scaling stream 1 information
	ISPOST_UINT32 ui32SS1OutImgBufYaddr;
	ISPOST_UINT32 ui32SS1OutImgBufUVaddr;
	ISPOST_UINT32 ui32SS1OutImgBufStride;
	ISPOST_UINT32 ui32SS1OutImgDstWidth;
	ISPOST_UINT32 ui32SS1OutImgDstHeight;
	ISPOST_UINT32 ui32SS1OutImgMaxWidth;
	ISPOST_UINT32 ui32SS1OutImgMaxHeight;
	ISPOST_UINT32 ui32SS1OutHorzScaling;
	ISPOST_UINT32 ui32SS1OutVertScaling;
} ISPOST_USER;

ISPOST_RESULT IspostUserSetup(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserSetupLC(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserSetupOV(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserSetupDN(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserSetupSS0(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserSetupSS1(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserMakeTimestamp(ISPOST_USER* pIspostUser, char* szOvTimestampString, char* apOvTimestampStringLine2nd);
ISPOST_RESULT IspostUserTrigger(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserUpdate(ISPOST_USER* pIspostUser);
ISPOST_RESULT IspostUserIsBusy(void);
ISPOST_UINT32 IspostRegRead(ISPOST_UINT32 RegAddr);
ISPOST_UINT32 IspostRegWrite(ISPOST_UINT32 RegAddr, ISPOST_UINT32 RegData);
//---------------------------------------------------------------------------



#if defined(__cplusplus)
}
#endif //__cplusplus


#endif // __CI_ISPOST_H__
