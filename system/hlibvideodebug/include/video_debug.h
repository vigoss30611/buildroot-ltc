#ifndef __ISP_DEBUG_H
#define __ISP_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h> /*for clock_gettime */
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/IPUInfo.h>

#include "list.h"

#define BUF_LEN 512

#define _LOG_ERR 0
#define _LOG_WARN 1
#define _LOG_INFO 2
#define _LOG_DBG 3

/*This flag show this funciton has used malloc to alloc buffer*/
#define __malloc__

extern void \
	video_dbg_log(int level, const char* func, const int line, const char * fromat, ...);

#define VDBG_LOG_ERR(fmt, ...) \
	(video_dbg_log(_LOG_ERR, __func__, __LINE__, fmt, ##__VA_ARGS__))

#define VDBG_LOG_DBG(fmt, ...) \
	(video_dbg_log(_LOG_DBG, __func__, __LINE__, fmt, ##__VA_ARGS__))

#define VDBG_LOG_WARN(fmt, ...) \
	(video_dbg_log(_LOG_WARN, __func__, __LINE__, fmt, ##__VA_ARGS__))

#define VDBG_LOG_INFO(fmt, ...) \
	(video_dbg_log(_LOG_INFO, __func__, __LINE__, fmt, ##__VA_ARGS__))

#define SRC_FROM_FILE 0
#define SRC_FROM_API 1
#define SRC_FROM_CMD 2

#define DST_NONE 0
#define DST_VAR 1
#define DST_STRUCT 2

#define CI_N_CONTEXT 2
#define CI_N_IMAGERS 4

#define FELIX_MAX_RTM_VALUES (8)

#define ACTIVE_GASKET_INFO_FILE "/sys/kernel/debug/imgfelix/ActiveGasketInfo"

#define NGAKET_STATUS "Gasket%dStatus"
#define NGAKET_TYPE "Gasket%dType"
#define NGAKET_FRAME_COUNT "Gasket%dFrameCount"

#define NGAKET_MIPI_FIFO_FULL "Gasket%dMipiFifoFull"
#define NGAKET_MIPI_ENABLE_LANES "Gasket%dEnabledLanes"
#define NGAKET_MIPI_CRC_ERROR "Gasket%dMipiCrcError"
#define NGAKET_MIPI_HDR_ERROR "Gasket%dMipiHdrError"
#define NGAKET_MIPI_ECC_ERROR "Gasket%dMipiEccError"
#define NGAKET_MIPI_ECC_CORRECTED "Gasket%dMipiEccCorrected"

#define RTM_INFO_FILE "/sys/kernel/debug/imgfelix/RTMInfo"

#define RTM_NCORE "RTMCore%d"
#define RTM_NCTX_STATUS "RTMCtx%dStatus"
#define RTM_NCTX_LINKEDLISTEMPTYNESS "RTMCtx%dLinkedListEmptyness"
#define RTM_NCTX_NPOS "RTMCtx%dPos%d"

#define LAST_FRAME_INFO_FILE "/sys/kernel/debug/imgfelix/LastFrameInfo"

#define INT_PER_SEC_FILE "/sys/kernel/debug/imgfelix/IntPerSec"
#define N_CTX_INT "Ctx%dInt"
#define N_CTX_IPS "Ctx%dIps"

#define IIF_INFO_FILE "/sys/kernel/debug/imgfelix/IIFInfo"

#define NCTX_IMAGER "Ctx%dImager"
#define NCTX_BAYER_FORMAT "Ctx%deBayerFormat"
#define NCTX_SCALER_BOTTOM_BORDER "Ctx%dScalerBottomBorder"
#define NCTX_BUFF_THRESHOLD "Ctx%dBuffThreshold"
#define NCTX_IMAGER_OFFSET_X "Ctx%dImagerOffsetX"
#define NCTX_IMAGER_OFFSET_Y "Ctx%dImagerOffsetY"
#define NCTX_IMAGER_SIZE_X "Ctx%dImagerSizeRow"
#define NCTX_IMAGER_SIZE_Y "Ctx%dImagerSizeCol"
#define NCTX_BLACK_BORDER_OFFSET "Ctx%dBlackBorderOffset"
#define NCTX_IMAGER_DECIMATION_X "Ctx%dImagerDecimationX"
#define NCTX_IMAGER_DECIMATION_Y "Ctx%dImagerDecimationY"

#define HW_LIST_ELEMENTS_FILE "/sys/kernel/debug/imgfelix/HwListElements"
#define N_CTX_AVAILABLE "Ctx%dAvailable"
#define N_CTX_PENDING "Ctx%dPending"
#define N_CTX_PROCESSED "Ctx%dProcessed"
#define N_CTX_SEND "Ctx%dSend"

#define ENC_SCALER_INFO_FILE "/sys/kernel/debug/imgfelix/EncScalerInfo"

#define DEV_MEM_MAX_USED_FILE "/sys/kernel/debug/imgfelix/DevMemMaxUsed"
#define DEV_MEM_USED_FILE "/sys/kernel/debug/imgfelix/DevMemUsed"

#define DRIVER_N_CONNECTIONS_FILE \
	"/sys/kernel/debug/imgfelix/DriverNConnections"

#define DRIVER_NCTX_ACTIVE_FILE "/sys/kernel/debug/imgfelix/DriverCTX%dActive"
#define DRIVER_NCTX_INT_FILE "/sys/kernel/debug/imgfelix/DriverCTX%dInt"
#define DRIVER_NCTX_INT_DONE_FILE \
	"/sys/kernel/debug/imgfelix/DriverCTX%dInt_DoneAll"
#define DRIVER_NCTX_INT_IGNORE_FILE \
	"/sys/kernel/debug/imgfelix/DriverCTX%dInt_Ignore"
#define DRIVER_NCTX_INT_START_FILE \
	"/sys/kernel/debug/imgfelix/DriverCTX%dInt_Start"
#define DRIVER_NCTX_TRIGGERED_HW_FILE \
	"/sys/kernel/debug/imgfelix/DriverCTX%dTriggeredHW"
#define DRIVER_NCTX_TRIGGERED_SW_FILE \
	"/sys/kernel/debug/imgfelix/DriverCTX%dTriggeredSW"

#define DRIVER_NSERVICED_HARD_INT_FILE \
	"/sys/kernel/debug/imgfelix/DriverNServicedHardInt"
#define DRIVER_NSERVICED_THREAD_INT_FILE \
	"/sys/kernel/debug/imgfelix/DriverNServicedThreadInt"

#define DRIVER_LONGEST_HARD_INIT_US_FILE \
	"/sys/kernel/debug/imgfelix/DriverLongestHardIntUS"
#define DRIVER_LONGEST_THREAD_INIT_US_FILE \
	"/sys/kernel/debug/imgfelix/DriverLongestThreadIntUS"

#define DRIVER_NDG_INT_END_OF_FRAME_FILE \
	"/sys/kernel/debug/imgfelix/DriverIntDG%dInt_EndOfFrame"
#define DRIVER_NDG_INT_ERROR_FILE \
	"/sys/kernel/debug/imgfelix/DriverIntDG%dInt_Error"

#define ISP_CLKRETE_FILE "/sys/module/Felix/parameters/clkRate"
#define ISP_V2500_CLKRATE_FILE "/sys/module/Felix/parameters/ui32ClkRate"

#define ISPOST_HW_VSERION "/sys/kernel/debug/ispost/hw_version"

#define ISPOST_DN_STATUS "/sys/kernel/debug/ispost/dn_status"
#define DN_STAT "Stat"
#define DN_EN "DnEn"
#define DN_REF_Y  "RefY"

#define ISPOST_HW_STATUS "/sys/kernel/debug/ispost/status"
#define ISPOST_COUNT "IspostCount"
#define ISPOST_USER_CTRL0 "UserCtrl0"
#define ISPOST_CTRL0 "Ctrl0"
#define ISPOST_STAT0 "Stat0"

#define CHIP_EFUSE_INFO "/proc/efuse"
#define CHIP_EFUSE_TYPE "TYPE"
#define CHIP_EFUSE_MAC "MAC"

#define DDR_FREQ_CMD "cat /dev/items |grep dram.freq |awk -F ' ' '{print $2;}'"
#define DDR_PORT_R_PRIORITY_EN_CMD "devmem 0x2d00843c 32"
#define DDR_PORT_W_PRIORITY_EN_CMD "devmem 0x2d008440 32"
#define DDR_PORT2_R_PRIORITY_CMD "devmem 0x21a00564 32"
#define DDR_PORT2_W_PRIORITY_CMD "devmem 0x21a00568 32"

#define BOARD_CPU_CMD "cat /dev/items |grep board.cpu |awk -F ' ' '{print $2;}'"

#define BOARD_CPU_APOLLO "apollo"
#define BOARD_CPU_APOLLO2 "apollo2"
#define BOARD_CPU_APOLLO3 "apollo3"

#define CPU_TYPE_APOLLO (0x001)
#define CPU_TYPE_APOLLO2 (0x002)
#define CPU_TYPE_APOLLO3 (0x0003)

#define CSI0_DEMP_FILE_CMD \
	"cat /sys/devices/virtual/misc/ddk_sensor0/sensor_info;" \
	"dmesg | tail -18 > /tmp/csi0_dump.txt"

#define CSI1_DEMP_FILE_CMD \
	"cat /sys/devices/virtual/misc/ddk_sensor1/sensor_info;" \
	"dmesg | tail -18 > /tmp/csi0_dump.txt"

#define CSI0_REG_N_OFFSET_CMD \
	"cat /tmp/csi0_dump.txt | grep 'CSI_REG_OFFSET%x ' " \
	"| awk -F ' ' '{print $4;}' | cut -d '[' -f2 | cut -d ']' -f1"

#define DEBUGFS_MOUNT_CMD "mount -t debugfs none /sys/kernel/debug"

#define SENSOR0_INTERFACE_TYPE_CMD \
	"cat /sys/devices/virtual/misc/ddk_sensor0/sensor_info | grep interface"

#define INTERFACE_MIPI "interface=mipi"

#define SENSOR1_INTERFACE_TYPE_CMD \
	"cat /sys/devices/virtual/misc/ddk_sensor1/sensor_info | grep interface"

#define ISP_DEBUGFS_DIR_PATH "/sys/kernel/debug/imgfelix/"
#define ISPOST_DEBUGFS_DIR_PATH "/sys/kernel/debug/ispost/"

#define CSI2_VERSION_REG 			(0x000)
#define CSI2_N_LANES_REG 			(0x004)
#define CSI2_DPHY_SHUTDOWN_REG 	(0x008)
#define CSI2_DPHYRST_REG			(0x00C)
#define CSI2_RESET_REG				(0x010)
#define CSI2_DPHY_STATE_REG		(0x014)
#define CSI2_DATA_IDS_1_REG 		(0x018)
#define CSI2_DATA_IDS_2_REG		(0x01C)
#define CSI2_ERR1_REG				(0x020)
#define CSI2_ERR2_REG				(0x024)
#define CSI2_MASK1_REG				(0x028)
#define CSI2_MASK2_REG				(0x02C)
#define CSI2_DPHY_TST_CRTL0_REG 	(0x030)
#define CSI2_DPHY_TST_CRTL1_REG 	(0x034)
#define CSIDMAREG_FRAMESIZE 		(0x400)
#define CSIDMAREG_DATATYPE 		(0x404)
#define CSIDMAREG_VSYNCCNT 		(0x408)
#define CSIDMAREG_HBLANK			(0x40c)

#define VIDEO_DEBUG_UE_MODE 0 /* user edition*/
#define VIDEO_DEBUG_PE_MODE 1 /* professional edition */

#define VIDEO_DEBUG_ISP_UE_MODE 2 /* user edition*/
#define VIDEO_DEBUG_ISP_PE_MODE 3 /* professional edition */

#define VIDEO_DEBUG_VENC_MODE 4 /* user edition*/

struct _video_dbg_parser;

typedef void (*line_parser_cb)(struct _video_dbg_parser*parser, char *param, char* val);
typedef void (*parser_cb)(struct _video_dbg_parser*parser, char *val);

typedef struct _video_dbg_parser {
	int id;
	char obj[256];

	void* result;
	int size;

	int src;
	int dst;
	int exist;

	line_parser_cb line_parser_f;
	parser_cb parser_f;

	struct _video_dbg_parser *prev, *next;
} video_dbg_parser;

typedef struct _hwlist_info {
	int available_elements;
	int pending_elements;
	int processed_elements;
	int send_elements;
	int exist;
}hwlist_info;

#define CTX_POS_1D 0
#define CTX_POS_2D 1
#define CTX_POS_RGB 2
#define CTX_POS_ENH0 3
#define CTX_POS_ENH1 4
#define CTX_POS_ENC_L 5
#define CTX_POS_ENC_C 6
#define CTS_POS_DISP 7

typedef struct _rtm_ctx_info {
	uint32_t entries[FELIX_MAX_RTM_VALUES];
	uint32_t ctx_status[CI_N_CONTEXT];
	uint32_t ctx_link_emptyness[CI_N_CONTEXT];
	uint32_t ctx_position[CI_N_CONTEXT][FELIX_MAX_RTM_VALUES];

	uint32_t exist[CI_N_CONTEXT];
}rtm_ctx_info;

typedef struct _ctx_interrupt_info {
	uint32_t active;
	uint32_t int_cnt;
	uint32_t done_all_cnt;
	uint32_t err_ignore_cnt;
	uint32_t start_of_frame_cnt;
	uint32_t hard_cnt;
	uint32_t thread_cnt;

	uint32_t exist;
}ctx_interrupt_info;

typedef struct _interrupt_info {
	uint32_t connections;
	uint32_t serviced_thread_int;
	uint32_t serviced_hard_int;
	uint32_t longest_hard_us;
	uint32_t longest_thread_us;

	uint32_t ctx_int[CI_N_CONTEXT];
	uint32_t int_per_sec[CI_N_CONTEXT];
	uint32_t exist[CI_N_CONTEXT];
}interrupt_info;

typedef struct _gasket_info {
	uint32_t enable;
	char type[32];
	uint32_t frame_cnt;
	uint32_t mipi_fifo_full;
	uint32_t mipi_en_lanes;
	uint32_t mipi_crc_err;
	uint32_t mipi_hdr_err;
	uint32_t mipi_ecc_err;
	uint32_t mipi_ecc_corr;

	uint32_t exist;
}gasket_info;

typedef struct _iif_info {
	uint8_t imager;
	uint16_t scaler_bottom_border;
	char ebayer_format[16];
	uint16_t buff_threshold;
	uint16_t imager_offset_x;
	uint16_t imager_offset_y;
	uint16_t imager_size_row;
	uint16_t imager_size_col;

	uint16_t imager_decimation_x;
	uint16_t imager_decimation_y;
	uint16_t black_border_offset;
}iif_info;

typedef struct _encscaler_info {
	uint16_t output_size_h;
	uint16_t output_size_v;
	uint16_t offset_h;
	uint16_t offset_v;
}encscaler_info;

typedef struct _lastframe_info {
	uint8_t last_config_tag;
	uint8_t last_context_tag;
	uint8_t lastContext_erros;
	uint8_t lastContext_frame;
	uint32_t context_status;
}lastframe_info;

typedef struct _sensor_info {
	char name[128];
	int exposure;
	double gain;
	int gain_min;
	int gain_max;
	int exposure_min;
	int exposure_max;
}sensor_info;

typedef struct isp_mem_info {
	int n_buffer;
	int used_mem;
	int used_max_mem;
}isp_mem_info;

typedef struct _isp_base_info {
	uint32_t clk_rate;
	uint32_t ctx_num;
}isp_base_info;

typedef struct _ddr_info {
	uint32_t clk_rate;
	uint32_t port_wpriority_en;
	uint32_t port_rpriority_en;

	uint32_t port2_wpriority;
	uint32_t port2_rpriority;
}ddr_info;

typedef union _ispostv1_ctrl0 {
	uint32_t ctrl0;
	struct {
		uint32_t u1en:1;
		uint32_t u1rst:1;
		uint32_t u6rev:6;
		uint32_t u1en_lc:1;
		uint32_t u1en_ov:1;
		uint32_t u1en_dn:1;
		uint32_t u1en_ss0:1;
		uint32_t u1en_ss1:1;
		uint32_t u11rev:11;
		uint32_t u1int:1;
		uint32_t u4rev:4;
	};
} ispostv1_ctrl0, *pispostv1_ctrl0;

typedef union _ispostv1_stat0 {
	uint32_t stat0;
	struct {
		uint32_t u1stat0:1;
		uint32_t u1stat_dn:1;
		uint32_t u1stat_ss0:1;
		uint32_t u1stat_ss1:1;
		uint32_t u28rev:28;
	};
} ispostv1_stat0, *pispostv1_stat0;

typedef union _ispostv2_ctrl0 {
	uint32_t ctrl0;
	struct {
		uint32_t u1en:1;      //[0]
		uint32_t u1rst:1;     //[1]
		uint32_t u1en_ilc:1;   //[2]
		uint32_t u1rst_ilc:1;  //[3]
		uint32_t u1en_vs:1;    //[4]
		uint32_t u1rst_vs:1;   //[5]
		uint32_t u1en_ispl:1;  //[6]
		uint32_t u1rst_ispl:1; //[7]
		uint32_t u1en_lc:1;    //[8]
		uint32_t u1en_ov:1;    //[9]
		uint32_t u1en_dn:1;    //[10]
		uint32_t u1en_ss0:1;   //[11]
		uint32_t u1en_ss1:1;   //[12]
		uint32_t u1en_vsrc:1;  //[13]
		uint32_t u1en_vsr:1;   //[14]
		uint32_t u1en_vencl:1; //[15]
		uint32_t u1en_lclb:1;  //[16]
		uint32_t u3rev17:3;   //[17:19]
		uint32_t u1en_uo:1;    //[20]
		uint32_t u1en_dnwr:1;  //[21]
		uint32_t u1rev22:1;   //[22]
		uint32_t u1rst_vencl:1;//[23]
		uint32_t u1Int:1;     //[24]
		uint32_t u1vs_int:1;   //[25]
		uint32_t u1vsfw_int:1; //[26]
		uint32_t u1lb_err_int:1;//[27]
		uint32_t u1rev28:1;   //[28]
		uint32_t u1buf_dn:1;   //[29] software flag
		uint32_t u1buf_ss0:1;  //[30] software flag
		uint32_t u1buf_ss1:1;  //[31] software flag
	};
} ispostv2_ctrl0, *pispostv2_ctrl0;

typedef union _ispostv2_stat0 {
	uint32_t stat0;
	struct {
		uint32_t u1stat0:1;   //[0]
		uint32_t u1stat_uo:1;  //[1]
		uint32_t u1stat_ss0:1; //[2]
		uint32_t u1stat_ss1:1; //[3]
		uint32_t u1stat1:1;   //[4]
		uint32_t u1stat2:1;   //[5]
		uint32_t u1stat_vo:1;  //[6]
		uint32_t u1rev7:1;     //[7]
		uint32_t u1lb_err:1;   //[8]
		uint32_t u23rev:23;   //[9:31]
	};
} ispostv2_stat0, *pispostv2_stat0;

typedef struct _ispost_dn_info {
	uint32_t stat;
	uint32_t en_dn;
	uint32_t ref_y_addr;
} ispost_dn_info;

typedef struct _ispost_hw_status {
	uint32_t ispost_count;
	uint32_t user_ctrl0;
	uint32_t ctrl0;
	uint32_t stat0;
}ispost_hw_status;

typedef struct _ispost_info {
	uint32_t hw_version;
	ispost_dn_info dn;
	ispost_hw_status hw_status;
}ispost_info;

typedef struct _csi_reg_stat {
	uint32_t version;			//0x000
	uint32_t n_lanes;			//0x004
	uint32_t phy_shutdownz;	//0x008
	uint32_t dphy_rstz;		//0x00c
	uint32_t csi2_resetn;		// 0x010
	uint32_t phy_state;		// 0x014
	uint32_t data_ids_1;		//0x018
	uint32_t data_ids_2;		//0x01c
	uint32_t err1;				// 0x020
	uint32_t err2;				//0x024
	uint32_t mark1;			// 0x028
	uint32_t mark2;			//0x02c
	uint32_t phy_test_ctrl0;	// 0x030
	uint32_t phy_test_ctrl1;	// 0x034
	uint32_t img_size;			//0x400
	uint32_t mipi_data_type;	//0x404
	uint32_t vsync_cnt;		//0x408
	uint32_t hblank;			//0x40c
}csi_reg_stat;

typedef struct _chip_efuse_info {
	int type;
	int mac;
	int exist;
}chip_efuse_info;

typedef struct _video_dbg_user_info{
	uint32_t out_w;
	uint32_t out_h;
	uint32_t cap_w;
	uint32_t cap_h;

	int ctx_num;
	int awb_en[CI_N_CONTEXT];
	int ae_en[CI_N_CONTEXT];
	int hw_awb_en[CI_N_CONTEXT];
	int tnm_static_curve[CI_N_CONTEXT];
	int day_mode[CI_N_CONTEXT];
	int mirror_mode[CI_N_CONTEXT];
	uint32_t fps[CI_N_CONTEXT];

	uint32_t isp_freq;
	uint32_t ddr_freq;
	uint32_t isp_ddr_wpriority;
	uint32_t isp_ddr_rpriority;

	uint32_t ispost_dn_en;

	int used_mem;
	int used_max_mem;

	ST_VENC_DBG_INFO vinfo;
}video_dbg_user_info;

typedef struct _video_dbg_info {
	ST_ISP_DBG_INFO isp_info;
	ST_VENC_DBG_INFO vinfo;

	isp_base_info base;
	isp_mem_info mem;

	rtm_ctx_info rtm_ctx;
	interrupt_info isp_int;

	hwlist_info hwlist[CI_N_CONTEXT];
	gasket_info gasket[CI_N_IMAGERS];
	ctx_interrupt_info isp_ctx_int[CI_N_CONTEXT];
	iif_info iif[CI_N_CONTEXT];
	encscaler_info encscaler[CI_N_CONTEXT];
	lastframe_info lastframe[CI_N_CONTEXT];

	ispost_info ispost;
	csi_reg_stat csi;
	ddr_info ddr;
	chip_efuse_info efuse;

	int mode;
	int cpu_type;
}video_dbg_info;

extern int video_dbg_init(int mode);
extern void video_dbg_deinit(void);
extern int video_dbg_update(void);
extern video_dbg_info *video_dgb_get_info(void);
extern video_dbg_user_info* video_dbg_get_user_info(void);

extern void video_dbg_mode_print(void);
extern inline void perf_measure_start(struct timespec *start);
extern inline void perf_measure_stop(const char *title, struct timespec *start);

extern void video_dbg_print_info(int mode);
extern void audio_dbg_print_info(void);

#endif
