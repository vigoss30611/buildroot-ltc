#ifndef __TL421_USER_H__
#define __TL421_USER_H__
#ifndef __KERNEL__
#include <stdint.h>
#endif

struct task_info {
	uint32_t src;
	uint32_t dst;
	uint32_t len;
};

struct dsp_format {
	char format[16];
	unsigned int codec;/* 0: decode, 1: encode */
	unsigned int bw;/* bitwidth */
	unsigned int br;/* bitrate */
	unsigned int sr;/* sampling rate */
	int mode;/* 0: mono, 1: stereo */
	int effect;
};

struct dsp_info {
	struct dsp_format in;
	struct dsp_format out;
};

struct aec_codec {
	unsigned int spk_src;
	unsigned int mic_src;
	unsigned int dst;
	unsigned int len;

	unsigned int dbg_dst;
	unsigned int dbg_len;
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
	unsigned long spk_in_phy;
	int8_t *mic_in_virt;
	unsigned long mic_in_phy;
	int8_t *aec_out_virt;
	unsigned long aec_out_phy;
};

struct aec_info {
	struct aec_format fmt;
	struct aec_buf buf_addr;
};

struct aec_dbg_buf {
	int32_t dbg_len;
	int8_t *aec_dbg_out_virt;
	unsigned long aec_dbg_out_phy;
};

struct aec_dbg_format {
#define AEC_NORMAL_MODE 0
#define AEC_LOGGER_MODE 1
#define AEC_CI_MODE     2
	int dbg_mode;
};

struct aec_dbg_info {
	struct aec_dbg_format fmt;
	struct aec_dbg_buf buf_addr;
};

#define TL421_MAGIC	'v'
#define TL421_NODE	"/dev/cevadsp-tl421"
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
#define TL421_AEC_GET_BUF	_IOR(TL421_MAGIC, 10, unsigned long)
#define TL421_AEC_PROCESS	_IOW(TL421_MAGIC, 11, unsigned long)
#define TL421_AEC_RELEASE	_IOW(TL421_MAGIC, 12, unsigned long)
#define TL421_AEC_SET_DBG_FMT	_IOW(TL421_MAGIC, 13, unsigned long)
#define TL421_IOCMAX		13

#endif /* __TL421_USER_H__ */
