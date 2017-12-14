#ifndef __CEVA_DSP_H__
#define __CEVA_DSP_H__

/*
 * DSP System Manager Registers
 */
#define DSP_SYSM_RESET			(0x0)
#define DSP_SYSM_CLK_GATE		(0x4)
#define DSP_SYSM_POW_CTL		( 0x8)
#define DSP_SYSM_BUS_CTL		(0xC)
#define DSP_SYSM_BUS_QOS		(0x10)
#define DSP_SYSM_MEM_PD			(0x14)
#define DSP_SYSM_IF_ISO			(0x18)
#define DSP_SYSM_CLK_RATIO		(0x20)
#define DSP_SYSM_BOOT			(0x24)
#define DSP_SYSM_VECTOR0		(0x28)
#define DSP_SYSM_VECTOR1		(0x2C)
#define DSP_SYSM_VECTOR2		(0x30)
#define DSP_SYSM_VECTOR3		(0x34)
#define DSP_SYSM_VCNTX_EN		(0x38)
#define DSP_SYSM_INT0_SEL		(0x3C)
#define DSP_SYSM_INT1_SEL		(0x40)
#define DSP_SYSM_INT2_SEL		(0x44)
#define DSP_SYSM_INT3_SEL		(0x48)
#define DSP_SYSM_VINT_SEL		(0x4C)
#define DSP_SYSM_NMI_SEL		(0x50)
#define DSP_SYSM_USER_INT_EN		(0x54)
#define DSP_SYSM_MCACHE_INVALID		(0x58)
#define DSP_SYSM_EXTERNAL_WAIT		(0x5C)
#define DSP_SYSM_PSU_WAIT_EN		(0x60)
#define DSP_SYSM_USER0_INPUT		(0x64)
#define DSP_SYSM_USER1_INPUT		(0x68)
#define DSP_SYSM_MODE_SEL		(0x6C)
#define DSP_INT_DEBUG			(0x70)
#define DSP_INT_DEBUG_EN		(0x74)

/*
 * IPC Registers
 */
#define IPC_SEM0S			(0x0)
#define IPC_SEM1S			(0x4)
#define IPC_SEM2S			(0x8)
#define IPC_SEM0C			(0xC)
#define IPC_SEM1C			(0x10)
#define IPC_SEM2C			(0x14)
#define IPC_MCU_MASK0			(0x18)
#define IPC_MCU_MASK1			(0x1C)
#define IPC_MCU_MASK2			(0x20)
#define IPC_CX_MASK0			(0x24)
#define IPC_CX_MASK1			(0x28)
#define IPC_CX_MASK2			(0x2C)
#define IPC_COM0			(0x30)
#define IPC_COM1			(0x34)
#define IPC_COM2			(0x38)
#define IPC_REP0			(0x3C)
#define IPC_REP1			(0x40)
#define IPC_REP2			(0x44)
#define IPC_INT_MASK			(0x48)
#define IPC_STATUS			(0x4C)
#define IPC_MB_MESG0			(0x80)
#define IPC_MB_MESG1			(0x84)
#define IPC_MB_MGST0			(0x88)
#define IPC_MB_MGST1			(0x8C)
#define IPC_DSP_INTS			(0x90)
#define IPC_DSP_ISEL			(0x94)
#define IPC_DSP_INTM			(0x98)
#define IPC_ARM_INTS			(0x9C)
#define IPC_ARM_ISEL			(0xA0)
#define IPC_ARM_INTM			(0xA4)
#define IPC_D2A_HEAD			(0xA8)
#define IPC_D2A_TAIL			(0xAC)
#define IPC_A2D_HEAD			(0xB0)
#define IPC_A2D_TAIL			(0xB4)

#define TODSP_INT			(251)
#define TOARM_INT			(252)
#define GVI_ERR_INT			(254)

#define IPC_INT_REP0_WR_BIT	(3)
#define IPC_MEST0_CNT_MASK	(0xff)
#define IPC_MEST1_CNT_MASK	(0xff)




#define AEC_ERROR_BIT	(31)
#define AEC_INIT_BIT	(30)
#define AEC_RUNNING_BIT	(29)
#define AEC_DONE_BIT	(28)
#define AEC_INTR_BIT	(27)

#define AEC_MODE_BIT	(24)
#define AEC_MODE_MASK	(0x7<<24)

#define AEC_CI_DBG_MODE			(3)
#define AEC_LOGGER_DBG_MODE		(2)
#define AEC_HIGH_PROFILE_MODE	(1)
#define AEC_LOW_PROFILE_MODE	(0)

#define AEC_CMD_NONE			(0x00)

#define AEC_CMD_START_AEC_TX	(0x01)
#define AEC_CMD_START_AEC_RX	(0x02)
#define AEC_CMD_START_AEC_RX_TX	(0x03)
#define AEC_CMD_START_AEC_NONE	(0x04)

#define LITTLE_ENDIAN	(0)
#define BIG_ENDIAN	(1)
#define IMG_INTERNAL	(0)
#define IMG_EXTERNAL	(1)

#define CEVA_TL421	(0)
#define CEVA_TL421_MAJOR	(58)

#ifndef __KERNEL__
#include <stdint.h>
#endif

#define AACENC_IN_SIZE		(4096)
#define AACENC_OUT_SIZE		(1536)

extern phys_addr_t dsp_reserve_start;

struct dsp_firmware {
	char name[64];/* firmware name */
	uint32_t *dst;/* destination */
	int len;/* length */
	int size;/* bit width */
	int endian;/* big or little */
};

struct dsp_attribute {
	char format[16];
	unsigned int codec;/* 0: decode, 1: encode */
	unsigned int bw;/* bitwidth */
	unsigned int br;/* bitrate */
	unsigned int sr;/* sampling rate */
	int mode;/* 0: mono, 1: stereo */
	int effect;/* de-noise, AEC, AGC */
};

struct audio_codec {
	uint32_t src;
	uint32_t dst;
	uint32_t len;
	int flag;/* 0: all output samples, other: user defined */
};
struct aec_codec {
	unsigned int spk_src;
	unsigned int mic_src;
	unsigned int dst;
	unsigned int len;

	unsigned int dbg_dst;
	unsigned int dbg_len;
};

struct firmware_info {
	char name[64];
	uint32_t off;
	int endian;
	int loc;
	ssize_t size;
	u8 *data;
	int load;
};

struct image_info {
	uint16_t *start;
	uint32_t off;
	int len;
	int endian;
	int loc;
};

struct codec_format {
	int type;
	int effect;
	int channel;
	int sr;//sample_rate
	int bw;//bitwidth
	int br;//bit_rate
};

struct codec_info {
	struct codec_format in;
	struct codec_format out;
};

struct codec_phys {
	struct audio_codec *ac;
	uint32_t p_ac;
	uint8_t *in;
	uint32_t p_in;
	uint8_t *out;
	uint32_t p_out;
};

struct aec_format {
	int channel;
	int sample_rate;
	int bitwidth;
	int sample_size;
	int effect;
};

struct aec_buf {

	int32_t data_len;
	int8_t *spk_in_virt;
	uint32_t spk_in_phy;
	int8_t *mic_in_virt;
	uint32_t mic_in_phy;
	int8_t *aec_out_virt;
	uint32_t aec_out_phy;
};

struct aec_info {
	struct aec_format fmt;
	struct aec_buf buf_addr;
};

struct aec_dbg_format {
#define AEC_NO_DBG			(0)
#define AEC_LOGGER_DBG		(1)
#define AEC_CI_DBG			(2)
	int dbg_mode;
};

struct aec_dbg_buf {
	int32_t dbg_len;
	int8_t *aec_dbg_out_virt;
	uint32_t aec_dbg_out_phy;
};

struct aec_dbg_info {
	struct aec_dbg_format fmt;
	struct aec_dbg_buf buf_addr;
};

struct aec_ctl {
	struct aec_format fmt;
	struct aec_buf buf_addr;
	struct aec_dbg_info dbg;
	int timeout;
	wait_queue_head_t dsp_respond;
	unsigned int irq;
	uint32_t status;
};

struct mem_region {
	struct resource *res;
	void __iomem *base;
};

struct dsp_mem_region {
	char        name[32];
	uint32_t    start;
	uint32_t    size;

	struct resource *res;
	void __iomem *iobase;
};

struct dsp_device {
	unsigned long in_use;
	struct codec_info *info;
	struct codec_info *pre_info;
	//struct dsp_attribute *attr;
	//struct dsp_attribute *old_attr;
	struct codec_phys *dec;
	struct codec_phys *enc;
	//struct codec_info info;
	struct platform_device *pdev;

	struct mem_region sysm;
	struct mem_region ptcm;
	struct mem_region ptcm_ext;
	struct mem_region dtcm;
	struct mem_region dtcm_ext;
	struct mem_region ipc;

	struct clk *clk;
	struct aec_ctl aec;
};

#define DECODE_MODE	(0)
#define ENCODE_MODE	(1)

#define TL421_MAGIC	'v'
#define TL421_NODE	"/dev/ceva-tl421"
#define TL421_VER	(421)

#define TL421_SET_FMT		_IOW(TL421_MAGIC, 1, unsigned long)
#define TL421_GET_FMT		_IOR(TL421_MAGIC, 2, unsigned long)
#define TL421_ENCODE		_IOW(TL421_MAGIC, 3, unsigned long)
#define TL421_DECODE		_IOW(TL421_MAGIC, 4, unsigned long)
#define TL421_COMP_RATIO	_IOR(TL421_MAGIC, 5, unsigned long)
#define TL421_CONVERT		_IOW(TL421_MAGIC, 6, unsigned long)
#define TL421_GET_VER		_IOR(TL421_MAGIC, 7, unsigned long)
#define TL421_IOTEST		_IOW(TL421_MAGIC, 8, unsigned long)
#define TL421_AEC_SET_FMT	_IOW(TL421_MAGIC, 9, unsigned long)
#define TL421_AEC_GET_BUF	_IOW(TL421_MAGIC, 10, unsigned long)
#define TL421_AEC_PROCESS	_IOW(TL421_MAGIC, 11, unsigned long)
#define TL421_AEC_RELEASE	_IOW(TL421_MAGIC, 12, unsigned long)
#define TL421_AEC_SET_DBG_FMT	_IOW(TL421_MAGIC, 13, unsigned long)
#define TL421_IOCMAX	13

#endif /* __CEVA_DSP_H__ */
