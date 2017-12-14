#ifndef __HLIB_ISPOST_H__
#define __HLIB_ISPOST_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define ISPOST_NODE "/dev/ispost"

#define ISPOST_MAGIC    'b'
#define ISPOST_IOCMAX   10
#define ISPOST_NUM      8
#define ISPOST_NAME     "ispost"

#define ISPOST_ENV		  		_IOW(ISPOST_MAGIC, 1, unsigned long)
#define ISPOST_FULL_TRIGGER    _IOW(ISPOST_MAGIC, 2, unsigned long)

//---------------------------------------------------------------------------

typedef signed char       ISPOST_INT8;
typedef signed short      ISPOST_INT16;
typedef signed long       ISPOST_INT32;

typedef unsigned char     ISPOST_UINT8;
typedef unsigned short    ISPOST_UINT16;
typedef unsigned long     ISPOST_UINT32;


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
#define ISPOST_UDP_TMS			0x0020  //TimeStamp
#define ISPOST_UDP_ALL			(ISPOST_UDP_LC | ISPOST_UDP_OV | ISPOST_UDP_DN | ISPOST_UDP_SS0 | ISPOST_UDP_SS1)

#define ISPOST_PRN_LC			0x0001
#define ISPOST_PRN_OV			0x0002
#define ISPOST_PRN_DN			0x0004
#define ISPOST_PRN_SS0			0x0008
#define ISPOST_PRN_SS1			0x0010
#define ISPOST_PRN_CTRL			0x0020
#define ISPOST_PRN_ALL			(ISPOST_PRN_LC | ISPOST_PRN_OV | ISPOST_PRN_DN | ISPOST_PRN_SS0 | ISPOST_PRN_SS1 | ISPOST_PRN_CTRL)


#define ISPOST_DN_ARY_COUNT		5

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

typedef union _ISPOST_CTRL0
{
	ISPOST_UINT32 ui32Ctrl0;
	struct
	{
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
} ISPOST_CTRL0;

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

//---------------------------------------------------------------------------

void ispost_debug(const char* function, int line, const char* format, ...);

#define ISPOST_DEBUG(...) ispost_debug(__FUNCTION__, __LINE__, __VA_ARGS__)

extern int ispost_open(void);
extern int ispost_close(void);
extern int ispost_get_env(void);
extern int ispost_trigger(ISPOST_USER* pIspostUser);


#ifdef __cplusplus
}
#endif //__cplusplus




#endif //__HLIB_ISPOST_H__
