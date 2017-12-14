#ifndef __HLIB_ISPOST_H__
#define __HLIB_ISPOST_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define ISPOST_NODE "/dev/ispostv2"

#define ISPOST_MAGIC	'b'
#define ISPOST_IOCMAX	10
#define ISPOST_NUM		8
#define ISPOST_NAME		"ispostv2"

#define ISPOST_ENV              _IOW(ISPOST_MAGIC, 1, unsigned long)
#define ISPOST_INIT             _IOW(ISPOST_MAGIC, 2, unsigned long)
#define ISPOST_UPDATE           _IOW(ISPOST_MAGIC, 3, unsigned long)
#define ISPOST_TRIGGER          _IOW(ISPOST_MAGIC, 4, unsigned long)
#define ISPOST_WAIT_CPMPLETE    _IOW(ISPOST_MAGIC, 5, unsigned long)
#define ISPOST_FULL_TRIGGER     _IOW(ISPOST_MAGIC, 6, unsigned long)

//---------------------------------------------------------------------------

typedef signed char       ISPOST_INT8;
typedef signed short      ISPOST_INT16;
typedef signed long       ISPOST_INT32;

typedef unsigned char     ISPOST_UINT8;
typedef unsigned short    ISPOST_UINT16;
typedef unsigned long     ISPOST_UINT32;

typedef ISPOST_INT8 *           ISPOST_PINT8;
typedef ISPOST_INT16 *          ISPOST_PINT16;
typedef ISPOST_INT32 *          ISPOST_PINT32;

typedef ISPOST_UINT8 *          ISPOST_PUINT8;
typedef ISPOST_UINT16 *         ISPOST_PUINT16;
typedef ISPOST_UINT32 *         ISPOST_PUINT32;

typedef void                    ISPOST_VOID;
typedef void *                  ISPOST_PVOID;
typedef void *                  ISPOST_HANDLE;
typedef ISPOST_INT32            ISPOST_RESULT;

typedef ISPOST_UINT8            ISPOST_BOOL8;
typedef ISPOST_UINT8            ISPOST_BYTE;



#define ISPOST_DUMPFILE         0
#define ISPOST_LOADFILE         1

#define ISPOST_DISABLE          0
#define ISPOST_ENABLE           1

#define ISPOST_OFF              0
#define ISPOST_ON               1

#define ISPOST_FALSE            0
#define ISPOST_TRUE             1

#define ISPOST_DONE             0
#define ISPOST_BUSY             1

#define ISPOST_EXIT_FAILURE     -1
#define ISPOST_EXIT_SUCCESS     0

#define ISPOST_DUMP_TRIG_OFF    0
#define ISPOST_DUMP_TRIG_ON     2

#define ISPOST_SS_SF_MAX        (1 << 12)

#define ISPOST_EN               (ISPOST_ISPCTRL0_EN_MASK)
#define ISPOST_RST              (ISPOST_ISPCTRL0_RST_MASK)
#define ISPOST_EN_ILC           (ISPOST_ISPCTRL0_ENILC_MASK)
#define ISPOST_RST_ILC          (ISPOST_ISPCTRL0_RSTILC_MASK)
#define ISPOST_EN_VS_LMV        (ISPOST_ISPCTRL0_ENVS_MASK)
#define ISPOST_RST_VS_LMV       (ISPOST_ISPCTRL0_RSTVS_MASK)
#define ISPOST_EN_ISP_L         (ISPOST_ISPCTRL0_ENISPL_MASK)
#define ISPOST_RST_ISP_L        (ISPOST_ISPCTRL0_RSTISPL_MASK)
#define ISPOST_EN_VENC_L        (ISPOST_ISPCTRL0_ENVENCL_MASK)
#define ISPOST_RST_VENC_L       (ISPOST_ISPCTRL0_RSTVENCL_MASK)

#define ISPOST_UDP_LC           0x0001
#define ISPOST_UDP_VS           0x0002
#define ISPOST_UDP_OV           0x0004
#define ISPOST_UDP_DN           0x0008
#define ISPOST_UDP_UO           0x0010
#define ISPOST_UDP_SS0          0x0020
#define ISPOST_UDP_SS1          0x0040
#define ISPOST_UDP_ALL          (ISPOST_UDP_LC | ISPOST_UDP_VS | ISPOST_UDP_OV | ISPOST_UDP_DN | ISPOST_UDP_UO | ISPOST_UDP_SS0 | ISPOST_UDP_SS1)

#define ISPOST_PRN_LC			0x0001
#define ISPOST_PRN_VS			0x0002
#define ISPOST_PRN_OV			0x0004
#define ISPOST_PRN_DN			0x0008
#define ISPOST_PRN_UO			0x0010
#define ISPOST_PRN_SS0			0x0020
#define ISPOST_PRN_SS1			0x0040
#define ISPOST_PRN_CTRL			0x0080
#define ISPOST_PRN_ALL			(ISPOST_PRN_LC | ISPOST_PRN_VS | ISPOST_PRN_OV | ISPOST_PRN_DN | ISPOST_PRN_UO | ISPOST_PRN_SS0 | ISPOST_PRN_SS1 | ISPOST_PRN_CTRL)

//================================================
// Define Lens Distortion input image information.
#define LC_SRC_IMG_STRIGE       (640)
#define LC_SRC_IMG_WIDTH        (640)
#define LC_SRC_IMG_HEIGHT       (480)
#define LC_SRC_IMG_Y_SIZE       (LC_SRC_IMG_STRIGE * LC_SRC_IMG_HEIGHT)
#define LC_SRC_IMG_UV_SIZE      (LC_SRC_IMG_Y_SIZE / 2)
#define LC_SRC_IMG_SIZE         (LC_SRC_IMG_Y_SIZE + LC_SRC_IMG_UV_SIZE)
// Define Lens Distortion grid buffer information.
#define LC_GRID_BUF_GRID_SIZE   (GRID_SIZE_16_16)
#define LC_GRID_BUF_STRIDE      (1936)
#define LC_GRID_BUF_LENGTH      (133584)
// Define Lens Distortion Pixel cache mode information.
#define LC_CACHE_MODE_WLBE      (ISPOST_DISABLE)
#define LC_CACHE_MODE_FIFO_TYPE (FIFO_TYPE_STACK)
#define LC_CACHE_MODE_LBFMT     (LB_FMT_YUV420)
#define LC_CACHE_MODE_DAT_SHAPE (CCH_DAT_SHAPE_1)
#define LC_CACHE_MODE_BURST_LEN (BURST_LEN_8QW_BARREL)
//#define LC_CACHE_MODE_BURST_LEN (BURST_LEN_4QW_FISHEYE)
#define LC_CACHE_MODE_CTRL_CFG  (PREFATCH_CTRL_EN_FIFO_PREF)
#define LC_CACHE_MODE_CBCR_SWAP (CBCR_SWAP_MODE_SWAP)
//#define LC_CACHE_MODE_CBCR_SWAP (CBCR_SWAP_MODE_NOSWAP)
// Define Lens Distortion Scan mode information.
#define LC_SCAN_MODE_SCAN_W     (SCAN_WIDTH_16)
#define LC_SCAN_MODE_SCAN_H     (SCAN_HEIGHT_16)
#if 1
#define LC_SCAN_MODE_SCAN_M     (SCAN_MODE_LR_TB)
#else
#define LC_SCAN_MODE_SCAN_M     (SCAN_MODE_TB_LR)
#endif
// Define Lens Distortion Destination image information.
#define LC_DST_SIZE_WIDTH       (1920)
#define LC_DST_SIZE_HEIGHT      (1080)
// Define Lens Distortion Back Fill Color.
#if 1
// Pink color.
#define LC_BACK_FILL_Y          (0xFF)
#define LC_BACK_FILL_CB         (0x55)
#define LC_BACK_FILL_CR         (0xAA)
#else
// White color.
#define LC_BACK_FILL_Y          (0xFF)
#define LC_BACK_FILL_CB         (0x80)
#define LC_BACK_FILL_CR         (0x80)
#endif
//================================================
// Define Overlay image information.
#define OV_IMG_STRIDE           (128)
#define OV_IMG_WIDTH            (72)
#define OV_IMG_HEIGHT           (72)
#define OV_IMG_Y_SIZE           (OV_IMG_STRIDE * OV_IMG_HEIGHT)
#define OV_IMG_UV_SIZE          (OV_IMG_Y_SIZE / 2)
#define OV_IMG_SIZE             (OV_IMG_Y_SIZE + OV_IMG_UV_SIZE)
// Define Overlay position information.
#if 1
#define OV_IMG_OV_OFFSET_X      (80)
#define OV_IMG_OV_OFFSET_Y      (80)
#else
#define OV_IMG_OV_OFFSET_X      (128)
#define OV_IMG_OV_OFFSET_Y      (128)
#endif
// Define Overlay mode information.
#define OV_IMG_OV_MODE          (OVERLAY_MODE_YUV)
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
// Define Un-Scaled output image information.
#define UO_OUT_IMG_SCAN_H       (SCAN_HEIGHT_16)
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
#define SS0_OUT_IMG_HSF_SM      (SCALING_MODE_SCALING_DOWN)
// Define Scaling stream 0 vertical scale factor and mode information.
#define SS0_OUT_IMG_VSF_SF      (1824)
#define SS0_OUT_IMG_VSF_SM      (SCALING_MODE_SCALING_DOWN)
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
#define SS1_OUT_IMG_HSF_SM      (SCALING_MODE_SCALING_DOWN)
// Define Scaling stream 1 vertical scale factor and mode information.
#define SS1_OUT_IMG_VSF_SF      (2734)
#define SS1_OUT_IMG_VSF_SM      (SCALING_MODE_SCALING_DOWN)
//================================================
// Define Video Link control information.
#define V_LINK_CTRL_DATA_SEL    (DATA_SEL_UN_SCALED)
//#define V_LINK_CTRL_DATA_SEL    (DATA_SEL_SCALE_0)
//================================================


typedef enum _GRID_SIZE {
	GRID_SIZE_8_8 = 0,
	GRID_SIZE_16_16,
	GRID_SIZE_32_32,
#if 0
	GRID_SIZE_64_64,        // Not for Q3 and Q3F use.
                            // This feature is using for H.265
                            // due to venc link buffer considerations.
                            // (h.265 will require 64 lines hifh scan)
#else
	GRID_SIZE_RESERVED,
#endif
	GRID_SIZE_MAX
} GRID_SIZE, *PGRID_SIZE;

typedef enum _FIFO_TYPE {
	FIFO_TYPE_STACK = 0,
	FIFO_TYPE_FIFO,
	FIFO_TYPE_MAX
} FIFO_TYPE, *PFIFO_TYPE;

typedef enum _LB_FMT {
	LB_FMT_YUV420 = 0,
	LB_FMT_YUV422,
	LB_FMT_YUV444,
	LB_FMT_MAX
} LB_FMT, *PLB_FMT;

typedef enum _CACHE_MODE {
	CACHE_MODE_1 = 1,
	CACHE_MODE_2,
	CACHE_MODE_3,
	CACHE_MODE_4,
	CACHE_MODE_5,
	CACHE_MODE_6,
	CACHE_MODE_MAX
} CACHE_MODE, *PCACHE_MODE;

typedef enum _BURST_LEN {
	BURST_LEN_4QW_FISHEYE = 0,
	BURST_LEN_8QW_BARREL,
	BURST_LEN_MAX
} BURST_LEN, *PBURST_LEN;

typedef enum _CCH_DAT_SHAPE {
	CCH_DAT_SHAPE_0 = 0,
	CCH_DAT_SHAPE_1,
	CCH_DAT_SHAPE_2,
	CCH_DAT_SHAPE_3,
	CCH_DAT_SHAPE_MAX
} CCH_DAT_SHAPE, *PCCH_DAT_SHAPE;

typedef enum _PREFATCH_CTRL {
	PREFATCH_CTRL_EN_FIFO_PREF = 0,
	PREFATCH_CTRL_EN_FIFO,
	PREFATCH_CTRL_EN_PREF,
	PREFATCH_CTRL_DIS_FIFO_PREF,
	PREFATCH_CTRL_MAX
} PREFATCH_CTRL, *PPREFATCH_CTRL;

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
	SACN_HEIGHT_2,
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

typedef enum _ROT_MODE {
	ROT_MODE_NONE = 0,
	ROT_MODE_RESERVED,
	ROT_MODE_90_DEG,
	ROT_MODE_270_DEG,
	ROT_MODE_MAX
} ROT_MODE, *PROT_MODE;

typedef enum _OVERLAY_MODE {
	OVERLAY_MODE_UV = 0,
	OVERLAY_MODE_Y,
	OVERLAY_MODE_YUV,
	OVERLAY_MODE_MAX
} OVERLAY_MODE, *POVERLAY_MODE;

typedef enum _OVERLAY_CHNL {
	OVERLAY_CHNL_0 = 0,
	OVERLAY_CHNL_1,
	OVERLAY_CHNL_2,
	OVERLAY_CHNL_3,
	OVERLAY_CHNL_4,
	OVERLAY_CHNL_5,
	OVERLAY_CHNL_6,
	OVERLAY_CHNL_7,
	OVERLAY_CHNL_MAX
} OVERLAY_CHNL, *POVERLAY_CHNL;

//typedef enum _BG_COLOR_OVERRIDE {
//	BG_COLOR_OVERRIDE_DISABLE = 0,
//	BG_COLOR_OVERRIDE_ENABLE,
//	BG_COLOR_OVERRIDE_MAX
//} BG_COLOR_OVERRIDE, *PBG_COLOR_OVERRIDE;
//
typedef enum _DN_MSK_CRV {
	DN_MSK_CRV_DNMC_00 = 0,
	DN_MSK_CRV_DNMC_01,
	DN_MSK_CRV_DNMC_02,
	DN_MSK_CRV_DNMC_03,
	DN_MSK_CRV_DNMC_04,
	DN_MSK_CRV_DNMC_05,
	DN_MSK_CRV_DNMC_06,
	DN_MSK_CRV_DNMC_07,
	DN_MSK_CRV_DNMC_08,
	DN_MSK_CRV_DNMC_09,
	DN_MSK_CRV_DNMC_10,
	DN_MSK_CRV_DNMC_11,
	DN_MSK_CRV_DNMC_12,
	DN_MSK_CRV_DNMC_13,
	DN_MSK_CRV_DNMC_14,
	DN_MSK_CRV_DNMC_MAX
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
#if 0
	SCALING_MODE_SCALING_DOWN = 0,
	SCALING_MODE_SCALING_DOWN_1,
	SCALING_MODE_NO_SCALING,
	SCALING_MODE_NO_SCALING_1,
#else
	SCALING_MODE_SCALING_DOWN = 0,
	SCALING_MODE_RESERVED_01,
	SCALING_MODE_NO_SCALING,
	SCALING_MODE_RESERVED_11,
#endif
	SCALING_MODE_MAX
} SCALING_MODE, *PSCALING_MODE;

typedef enum _VERT_OFST {
	VERT_OFST_0 = 0,
	VERT_OFST_1,
	VERT_OFST_MAX
} VERT_OFST, *PVERT_OFST;

typedef enum _REODER_MODE {
	REODER_MODE_BURST = 0,
	REODER_MODE_ADDR_FILTER,
	REODER_MODE_MAX
} REODER_MODE, *PREODER_MODE;

typedef enum _DATA_SEL {
	DATA_SEL_UN_SCALED = 0,
	DATA_SEL_SCALE_0,
	DATA_SEL_MAX
} DATA_SEL, *PDATA_SEL;

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
		ISPOST_UINT32 ui1En:1;      //[0]
		ISPOST_UINT32 ui1Rst:1;     //[1]
		ISPOST_UINT32 ui1EnILc:1;   //[2]
		ISPOST_UINT32 ui1RstILc:1;  //[3]
		ISPOST_UINT32 ui1EnVS:1;    //[4]
		ISPOST_UINT32 ui1RstVS:1;   //[5]
		ISPOST_UINT32 ui1EnIspL:1;  //[6]
		ISPOST_UINT32 ui1RstIspL:1; //[7]
		ISPOST_UINT32 ui1EnLC:1;    //[8]
		ISPOST_UINT32 ui1EnOV:1;    //[9]
		ISPOST_UINT32 ui1EnDN:1;    //[10]
		ISPOST_UINT32 ui1EnSS0:1;   //[11]
		ISPOST_UINT32 ui1EnSS1:1;   //[12]
		ISPOST_UINT32 ui1EnVSRC:1;  //[13]
		ISPOST_UINT32 ui1EnVSR:1;   //[14]
		ISPOST_UINT32 ui1EnVencL:1; //[15]
		ISPOST_UINT32 ui1EnLcLb:1;  //[16]
		ISPOST_UINT32 ui3Rev17:3;   //[17:19]
		ISPOST_UINT32 ui1EnUO:1;    //[20]
		ISPOST_UINT32 ui1EnDNWR:1;  //[21]
		ISPOST_UINT32 ui1Rev22:1;   //[22]
		ISPOST_UINT32 ui1RstVencL:1;//[23]
		ISPOST_UINT32 ui1Int:1;     //[24]
		ISPOST_UINT32 ui1VSInt:1;   //[25]
		ISPOST_UINT32 ui1VSFWInt:1; //[26]
		ISPOST_UINT32 ui1LbErrInt:1;//[27]
		ISPOST_UINT32 ui1Rev28:1;   //[28]
		ISPOST_UINT32 ui1BufDN:1;   //[29] software flag
		ISPOST_UINT32 ui1BufSS0:1;  //[30] software flag
		ISPOST_UINT32 ui1BufSS1:1;  //[31] software flag
	};
} ISPOST_CTRL0, *PISPOST_CTRL0;

typedef union _ISPOST_STAT0 {
	ISPOST_UINT32 ui32Stat0;
	struct {
		ISPOST_UINT32 ui1Stat0:1;   //[0]
		ISPOST_UINT32 ui1StatUO:1;  //[1]
		ISPOST_UINT32 ui1StatSS0:1; //[2]
		ISPOST_UINT32 ui1StatSS1:1; //[3]
		ISPOST_UINT32 ui1Stat1:1;   //[4]
		ISPOST_UINT32 ui1Stat2:1;   //[5]
		ISPOST_UINT32 ui1StatVO:1;  //[6]
		ISPOST_UINT32 ui1Rev:1;     //[7]
		ISPOST_UINT32 ui1LbErr:1;   //[8]
		ISPOST_UINT32 ui23Rev:23;   //[9:31]
	};
} ISPOST_STAT0, *PISPOST_STAT0;

typedef struct _IMG_INFO {
	ISPOST_UINT32 ui32YAddr;
	ISPOST_UINT32 ui32UVAddr;
	ISPOST_UINT16 ui16Stride;
	ISPOST_UINT16 ui16Width;
	ISPOST_UINT16 ui16Height;
} IMG_INFO, *PIMG_INFO;

//typedef union _BACK_FILL_COLOR {
//	ISPOST_UINT32 ui32BackFillColor;
//	struct {
//		ISPOST_UINT32 ui8CR:8;      //[7:0]
//		ISPOST_UINT32 ui8CB:8;      //[15:8]
//		ISPOST_UINT32 ui8Y:8;       //[23:16]
//		ISPOST_UINT32 ui8Rev:8;     //[31:24]
//	};
//} BACK_FILL_COLOR, *PBACK_FILL_COLOR;
typedef struct _BACK_FILL_COLOR {
	ISPOST_UINT8 ui8CR:8;
	ISPOST_UINT8 ui8CB:8;
	ISPOST_UINT8 ui8Y:8;
} BACK_FILL_COLOR, *PBACK_FILL_COLOR;

typedef struct _LC_GRID_BUF_INFO {
	ISPOST_UINT32 ui32BufAddr;
	ISPOST_UINT16 ui16Stride;
	ISPOST_UINT32 ui32BufLen;
	ISPOST_UINT8 ui8LineBufEnable;
	ISPOST_UINT8 ui8Size;
	ISPOST_UINT32 ui32GeoBufAddr;
	ISPOST_UINT32 ui32GeoBufLen;
	ISPOST_UINT32 ui32GridTargetIdx;
} LC_GRID_BUF_INFO, *PLC_GRID_BUF_INFO;

//// 7. The signal "pv_einfo" provides some information if pixel query
////    encounters error.
////       00: No Error
////       01: Pixel Coordinate Out of Image
////       10: Pixel Pixel Value Expired (Overwritten)
////       11: Pixel Coordinate Out of Range of a Line
//
//assign pv_einfo[2:0] = psC_err[1] ? 3'b010 :
//                       psC_err[0] ? 3'b001 :
//                       psC_err[2] ? 3'b011 : 3'b000;
//
//assign pv_einfo[14: 3] = psC_x0;
//assign pv_einfo[25:15] = psC_y0;
//assign pv_einfo[31:26] = 6'b0;
typedef union _LC_LB_ERR {
	ISPOST_UINT32 ui32Error;
	struct {
		ISPOST_UINT32 ui3ErrorType:3;           //[2:0]
		ISPOST_UINT32 ui12X:12;                 //[14:3]
		ISPOST_UINT32 ui11y:11;                 //[25:15]
		ISPOST_UINT32 ui5Rev26:5;               //[30:26]
		ISPOST_UINT32 ui1Empty:1;               //[31]
	};
} LC_LB_ERR, *PLC_LB_ERR;

typedef union _LC_PXL_CACHE_MODE {
	ISPOST_UINT32 ui32PxlCacheMode;
	struct {
		ISPOST_UINT32 ui1CBCRSwap:1;            //[0]
		ISPOST_UINT32 ui1Rev1:1;                //[1]
		ISPOST_UINT32 ui2PreFetchCtrl:2;        //[3:2]
		ISPOST_UINT32 ui1BurstLenSet:1;         //[4]
		ISPOST_UINT32 ui2CacheDataShapeSel:2;   //[6:5]
		ISPOST_UINT32 ui1Rev7:1;                //[7]
		ISPOST_UINT32 ui2LineBufFormat:2;       //[9:8]
		ISPOST_UINT32 ui1LineBufFifoType:1;     //[10]
		ISPOST_UINT32 ui1WaitLineBufEnd:1;      //[11]
		ISPOST_UINT32 ui20Rev12:20;             //[31:12]
	};
} LC_PXL_CACHE_MODE, *PLC_PXL_CACHE_MODE;

typedef struct _LC_PXL_SCAN_MODE {
	ISPOST_UINT8 ui8ScanW;
	ISPOST_UINT8 ui8ScanH;
	ISPOST_UINT8 ui8ScanM;
} LC_PXL_SCAN_MODE, *PLC_PXL_SCAN_MODE;

typedef struct _LC_AXI_ID {
	ISPOST_UINT8 ui8SrcReqLmt;
	ISPOST_UINT8 ui8GBID;
	ISPOST_UINT8 ui8SRCID;
} LC_AXI_ID, *PLC_AXI_ID;

typedef struct _LC_INFO {
	IMG_INFO stSrcImgInfo;
	BACK_FILL_COLOR stBackFillColor;
	LC_GRID_BUF_INFO stGridBufInfo;
	LC_PXL_CACHE_MODE stPxlCacheMode;
	LC_PXL_SCAN_MODE stPxlScanMode;
	IMG_INFO stOutImgInfo;
	LC_AXI_ID stAxiId;
} LC_INFO, *PLC_INFO;

typedef union _VS_CTRL_0 {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		ISPOST_UINT32 ui1Ren:1;                 //[0]
		ISPOST_UINT32 ui1Rev1:1;                //[1]
		ISPOST_UINT32 ui2Rot:2;                 //[3:2]
		ISPOST_UINT32 ui1FlipV:1;               //[4]
		ISPOST_UINT32 ui1FlipH:1;               //[5]
		ISPOST_UINT32 ui26Rev6:26;              //[31:6]
	};
} VS_CTRL_0, *PVS_CTRL_0;

typedef union _VS_WIN_COOR {
	ISPOST_UINT32 ui32WinCoor;
	struct {
		ISPOST_UINT32 ui13COLX:13;              //[12:0]
		ISPOST_UINT32 ui3Rev13:3;               //[15:13]
		ISPOST_UINT32 ui13ROWY:13;              //[28:16]
		ISPOST_UINT32 ui3Rev29:3;               //[31:29]
	};
} VS_WIN_COOR, *PVS_WIN_COOR;

typedef struct _VS_REG_OFFSET {
	ISPOST_UINT16 ui16Roll;
	ISPOST_UINT16 ui16HOff;
	ISPOST_UINT8 ui8VOff;
} VS_REG_OFFSET, *PVS_REG_OFFSET;

typedef struct _VS_AXI_ID {
	ISPOST_UINT8 ui8FWID;
	ISPOST_UINT8 ui8RDID;
} VS_AXI_ID, *PVS_AXI_ID;

typedef struct _VS_INFO {
	VS_CTRL_0 stCtrl0;
	VS_WIN_COOR stWinCoor[4];
	IMG_INFO stSrcImgInfo;
	VS_REG_OFFSET stRegOffset;
	VS_AXI_ID stAxiId;
} VS_INFO, *PVS_INFO;

typedef union _OV_MODE {
	ISPOST_UINT32 ui32OvMode;
	struct {
		ISPOST_UINT32 ui2OVM:2;                 //[1:0]
		ISPOST_UINT32 ui2Rev2:2;                //[3:2]
		ISPOST_UINT32 ui1BCO:1;                 //[4]
		ISPOST_UINT32 ui3Rev5:3;                //[7:5]
		ISPOST_UINT32 ui1ENOV0:1;               //[8]
		ISPOST_UINT32 ui1ENOV1:1;               //[9]
		ISPOST_UINT32 ui1ENOV2:1;               //[10]
		ISPOST_UINT32 ui1ENOV3:1;               //[11]
		ISPOST_UINT32 ui1ENOV4:1;               //[12]
		ISPOST_UINT32 ui1ENOV5:1;               //[13]
		ISPOST_UINT32 ui1ENOV6:1;               //[14]
		ISPOST_UINT32 ui1ENOV7:1;               //[15]
		ISPOST_UINT32 ui16Rev16:16;             //[31:16]
	};
} OV_MODE, *POV_MODE;

typedef struct _OV_OFFSET {
	ISPOST_UINT16 ui16X;
	ISPOST_UINT16 ui16Y;
} OV_OFFSET, *POV_OFFSET;

typedef union _OV_ALPHA_VALUE {
	ISPOST_UINT32 ui32Alpha;
	struct {
		ISPOST_UINT32 ui8Alpha0:8;
		ISPOST_UINT32 ui8Alpha1:8;
		ISPOST_UINT32 ui8Alpha2:8;
		ISPOST_UINT32 ui8Alpha3:8;
	};
} OV_ALPHA_VALUE, *POV_ALPHA_VALUE;

typedef struct _OV_AXI_ID {
	ISPOST_UINT8 ui8OVID;
} OV_AXI_ID, *POV_AXI_ID;

typedef struct _OV_ALPHA {
	OV_ALPHA_VALUE stValue0;
	OV_ALPHA_VALUE stValue1;
	OV_ALPHA_VALUE stValue2;
	OV_ALPHA_VALUE stValue3;
} OV_ALPHA, *POV_ALPHA;

typedef struct _OV_INFO {
	ISPOST_UINT8 ui8OvIdx;
	IMG_INFO stOvImgInfo;
	OV_MODE stOverlayMode;
	OV_OFFSET stOvOffset;
	OV_ALPHA stOvAlpha;
	OV_AXI_ID stAxiId;
} OV_INFO, *POV_INFO;

typedef struct _OV_INFO_Q3F {
	ISPOST_UINT8 ui8OvIdx[OVERLAY_CHNL_MAX];
	IMG_INFO stOvImgInfo[OVERLAY_CHNL_MAX];
	OV_MODE stOverlayMode;
	OV_OFFSET stOvOffset[OVERLAY_CHNL_MAX];
	OV_ALPHA stOvAlpha[OVERLAY_CHNL_MAX];
	OV_AXI_ID stAxiId[OVERLAY_CHNL_MAX];
} OV_INFO_Q3F, *POV_INFO_Q3F;

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
#if 0
	MSK_CRV_INFO stMskCrv00;
	MSK_CRV_INFO stMskCrv01;
	MSK_CRV_INFO stMskCrv02;
	MSK_CRV_INFO stMskCrv03;
	MSK_CRV_INFO stMskCrv04;
	MSK_CRV_INFO stMskCrv05;
	MSK_CRV_INFO stMskCrv06;
	MSK_CRV_INFO stMskCrv07;
	MSK_CRV_INFO stMskCrv08;
	MSK_CRV_INFO stMskCrv09;
	MSK_CRV_INFO stMskCrv10;
	MSK_CRV_INFO stMskCrv11;
	MSK_CRV_INFO stMskCrv12;
	MSK_CRV_INFO stMskCrv13;
	MSK_CRV_INFO stMskCrv14;
#else
	MSK_CRV_INFO stMskCrv[DN_MSK_CRV_DNMC_MAX];
#endif
} DN_MSK_CRV_INFO, *PDN_MSK_CRV_INFO;

typedef struct _DN_AXI_ID {
	ISPOST_UINT8 ui8RefWrID;
	ISPOST_UINT8 ui8RefRdID;
} DN_AXI_ID, *PDN_AXI_ID;

typedef struct _DN_INFO {
	IMG_INFO stRefImgInfo;
	IMG_INFO stOutImgInfo;
	ISPOST_UINT32 ui32DnTargetIdx;
	DN_MSK_CRV_INFO stDnMskCrvInfo[DN_MSK_CRV_IDX_MAX];
	DN_AXI_ID stAxiId;
} DN_INFO, *PDN_INFO;

typedef struct _UO_AXI_ID {
	ISPOST_UINT8 ui8RefWrID;
} UO_AXI_ID, *PUO_AXI_ID;

typedef struct _UO_INFO {
	IMG_INFO stOutImgInfo;
	ISPOST_UINT8 ui8ScanH;
	UO_AXI_ID stAxiId;
} UO_INFO, *PUO_INFO;

typedef struct _SCALING_FACTOR {
	ISPOST_UINT16 ui16SF;
	ISPOST_UINT8 ui8SM;
} SS_SCALING_FACTOR, *PSS_SCALING_FACTOR;

typedef struct _SS_AXI_ID {
	ISPOST_UINT8 ui8SSWID;
} SS_AXI_ID, *PSS_AXI_ID;

typedef struct _SS_INFO {
	ISPOST_UINT8 ui8StreamSel;
	IMG_INFO stOutImgInfo;
	SS_SCALING_FACTOR stHSF;
	SS_SCALING_FACTOR stVSF;
	SS_AXI_ID stAxiId;
} SS_INFO, *PSS_INFO;

typedef union _V_LINK_CTRL {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		ISPOST_UINT32 ui5FifoThreshold:5;       //[4:0]
		ISPOST_UINT32 ui1HMB:1;                 //[5]
		ISPOST_UINT32 ui5CmdFifoThreshold:5;    //[10:6]
		ISPOST_UINT32 ui1DataSel:1;             //[11]
		ISPOST_UINT32 ui20Rev12:20;             //[31:12]
	};
} V_LINK_CTRL, *PV_LINK_CTRL;

typedef struct _V_LINK_INFO {
	V_LINK_CTRL stVLinkCtrl;
	ISPOST_UINT32 ui32StartAddr;
} V_LINK_INFO, *PV_LINK_INFO;

// For ISP Link 2 method
typedef union _ISP_LINK_CTRL_0_LINK2_METHOD {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		//ISPOST_UINT32 ui16BlankH:16;            //[15:0]
		ISPOST_UINT32 ui16Ctrl0:16;             //[15:0]
		ISPOST_UINT32 ui16BlankW:16;            //[31:16]
	};
} ISP_LINK_CTRL_0_LINK2_METHOD, *PISP_LINK_CTRL_0_LINK2_METHOD;

// For ISP Link 3 method
typedef union _ISP_LINK_CTRL_0_LINK3_METHOD {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		ISPOST_UINT32 ui4UvAddrMask:4;          //[3:0]
		ISPOST_UINT32 ui4UvAddrMatchValue:4;    //[7:4] (bits 29,28,12,11)
		ISPOST_UINT32 ui4Y1AddrMask:4;          //[11:8]
		ISPOST_UINT32 ui4Y1AddrMatchValue:4;    //[15:12] (bits 29,28,12,11)
		ISPOST_UINT32 ui4Y0AddrMask:4;          //[19:16]
		ISPOST_UINT32 ui4Y0AddrMatchValue:4;    //[23:20] (bits 29,28,12,11)
		ISPOST_UINT32 ui8Rev:8;                 //[31:24]
	};
} ISP_LINK_CTRL_0_LINK3_METHOD, *PISP_LINK_CTRL_0_LINK3_METHOD;

typedef union _ISP_LINK_CTRL_1 {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		ISPOST_UINT32 ui8ReOrdSeqSel0:8;        //[7:0]
		ISPOST_UINT32 ui8ReOrdSeqSel1:8;        //[15:8]
		ISPOST_UINT32 ui8ReOrdSeqLast:8;        //[23:16]
		ISPOST_UINT32 ui3Rev24:3;               //[26:24]
		ISPOST_UINT32 ui1ReOrdMode:1;           //[27]
		ISPOST_UINT32 ui1ReOrdEn:1;             //[28]
		ISPOST_UINT32 ui1InsertBurstsEn:1;      //[29]
		ISPOST_UINT32 ui1PadAllBurstsTo4W:1;    //[30]
		ISPOST_UINT32 ui1InvertIspAddr:1;       //[31]
	};
} ISP_LINK_CTRL_1, *PISP_LINK_CTRL_1;

typedef union _ISP_LINK_CTRL_2 {
	ISPOST_UINT32 ui32Ctrl;
	struct {
		ISPOST_UINT32 ui12BurstInsertW:12;      //[11:0]
		ISPOST_UINT32 ui4Rev12:4;               //[15:12]
		ISPOST_UINT32 ui8BurstInsertLen:8;      //[23:16]
		ISPOST_UINT32 ui8Rev24:8;               //[31:24]
	};
} ISP_LINK_CTRL_2, *PISP_LINK_CTRL_2;

typedef struct _ISP_LINK_CTRL {
	ISP_LINK_CTRL_0_LINK2_METHOD stCtrl0Link2Method;
	ISP_LINK_CTRL_0_LINK3_METHOD stCtrl0Link3Method;
	ISP_LINK_CTRL_1 stCtrl1;
	ISP_LINK_CTRL_2 stCtrl2;
	ISPOST_UINT32 ui32Ctrl3;
} ISP_LINK_CTRL, *PISP_LINK_CTRL;

typedef struct _ISP_UV_START_ADDR {
	ISPOST_UINT32 ui32FirstUVAddrMSW;
	ISPOST_UINT32 ui32FirstUVAddrLSW;
	ISPOST_UINT32 ui32NextUVAddrMSW;
	ISPOST_UINT32 ui32NextUVAddrLSW;
} ISP_UV_START_ADDR, *PISP_UV_START_ADDR;

typedef struct _ISP_LINK_INFO {
	ISP_LINK_CTRL_0_LINK2_METHOD stCtrl0Link2Method;
	ISP_LINK_CTRL_0_LINK3_METHOD stCtrl0Link3Method;
	ISP_LINK_CTRL_1 stCtrl1;
	ISP_LINK_CTRL_2 stCtrl2;
	ISPOST_UINT32 ui32Ctrl3;
	ISPOST_UINT32 ui32FirstUVAddrMSW;
	ISPOST_UINT32 ui32FirstUVAddrLSW;
	ISPOST_UINT32 ui32NextUVAddrMSW;
	ISPOST_UINT32 ui32NextUVAddrLSW;
} ISP_LINK_INFO, *PISP_LINK_INFO;

typedef struct _ISPOST_INFO {
	ISPOST_UINT8 ui8IsFirstFrame;
	ISPOST_UINT32 ui32FirstLcSrcImgBufAddr;
	ISPOST_UINT32 ui32NextLcSrcImgBufAddr;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddrMSW;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddr;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddrMask;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddrMSW;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddr;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddrMask;
	ISPOST_UINT32 ui32VsirSrcImgBufAddr;
	ISPOST_UINT32 ui32OvImgBufAddr;
	ISPOST_UINT32 ui32DnRefImgBufAddr;
	ISPOST_UINT32 ui32DnOutImgBufAddr;
	ISPOST_UINT32 ui32UnScaleOutImgBufAddr;
	ISPOST_UINT32 ui32SS0OutImgBufAddr;
	ISPOST_UINT32 ui32SS1OutImgBufAddr;
	ISPOST_CTRL0 stCtrl0;
	LC_INFO stLcInfo;
	VS_INFO stVsInfo;
	//OV_INFO stOvInfo;
	OV_INFO_Q3F stOvInfo;
	DN_INFO stDnInfo;
	UO_INFO stUoInfo;
	SS_INFO stSS0Info;
	SS_INFO stSS1Info;
	V_LINK_INFO stVLinkInfo;
	ISP_LINK_INFO stIspLinkInfo;
} ISPOST_INFO, *PISPOST_INFO;

//---------------------------------------------------------------------------
typedef struct ISPOST_USER
{
	//Ispost control
	ISPOST_UINT32 ui32UpdateFlag;
	ISPOST_UINT32 ui32PrintFlag;
	ISPOST_UINT32 ui32PrintStep;
	ISPOST_UINT32 ui32IspostCount;

	ISPOST_UINT32 ui32FirstLcSrcImgBufAddr;
	ISPOST_UINT32 ui32NextLcSrcImgBufAddr;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddrMSW;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddr;
	ISPOST_UINT32 ui32FirstLcSrcImgBufUvAddrMask;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddrMSW;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddr;
	ISPOST_UINT32 ui32NextLcSrcImgBufUvAddrMask;

	ISPOST_CTRL0 stCtrl0;
	LC_INFO stLcInfo;
	VS_INFO stVsInfo;
	OV_INFO_Q3F stOvInfo;
	DN_INFO stDnInfo;
	UO_INFO stUoInfo;
	SS_INFO stSS0Info;
	SS_INFO stSS1Info;
	V_LINK_INFO stVLinkInfo;
	ISP_LINK_INFO stIspLinkInfo;
} ISPOST_USER;

//---------------------------------------------------------------------------

void ispost_debug(const char* function, int line, const char* format, ...);

#define ISPOST_DEBUG(...) ispost_debug(__FUNCTION__, __LINE__, __VA_ARGS__)

extern int ispost_open(void);
extern int ispost_close(void);
extern int ispost_get_env(void);
extern int ispost_init(ISPOST_USER* pIspostUser);
extern int ispost_update(ISPOST_USER* pIspostUser);
extern int ispost_trigger(ISPOST_USER* pIspostUser);
extern int ispost_wait_complete(ISPOST_USER* pIspostUser);
extern int ispost_full_trigger(ISPOST_USER* pIspostUser);


#ifdef __cplusplus
}
#endif //__cplusplus




#endif //__HLIB_ISPOST_H__
