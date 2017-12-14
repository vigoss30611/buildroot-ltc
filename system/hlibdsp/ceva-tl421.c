#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <ceva/tl421-user.h>

#define log(x...) printf("dsplib: " x)

static int fd = -1;
static int after_1stfr_enc = 0;

int  ceva_get_acc_framelen(char* buffer)
{
	if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0))
		return ((buffer[3] & 0x3) << 11) | (buffer[4] << 3) | (buffer[5] >> 5);
	else
		return 0;
}

int ceva_tl421_open(void *dev)
{
	int ver;

	if (fd < 0)
		fd = open(TL421_NODE, O_RDWR);
	else
		return 0;

	if (fd < 0) {
		log("failed to open %s\n", TL421_NODE);
		return -1;
	}
	
	after_1stfr_enc = 0;

	ver = ioctl(fd, TL421_GET_VER, NULL);
	if (ver != TL421_VER) {
		log("dsplib version is %d, but dsp driver version is %d\n",
				TL421_VER, ver);
		close(fd);
		return -1;
	}

	return 0;
}

int ceva_tl421_close(void *handle)
{

	if (fd < 0)
		return 0;
	else {
		log("closing %s\n", TL421_NODE);
		close(fd);
	}

	fd = -1;
	after_1stfr_enc = 0;

	return 0;
}

int ceva_tl421_set_format(void *handle, struct dsp_format *info)
{
	int ret;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, TL421_SET_FMT, info);
	if (ret < 0) {
		log("failed to set format\n");
		return ret;
	}

	return 0;
}

int ceva_tl421_aec_dbg_open(void *handle, struct aec_dbg_info *info)
{
	int ret = 0;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, TL421_AEC_SET_DBG_FMT, info);
	if (ret < 0) {
		log("aec failed to set dbg format\n");
		return ret;
	}

	ret = mmap(0, info->buf_addr.dbg_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, info->buf_addr.aec_dbg_out_phy);
	if(ret == MAP_FAILED) {
		log("couldn't mmap memory space for logger_out: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	info->buf_addr.aec_dbg_out_virt = (short *)ret;
	log("aec_dbg: mmap phy_addr(0x%p) to virt_addr(0x%x)\n", info->buf_addr.aec_dbg_out_phy, info->buf_addr.aec_dbg_out_virt);


	return 0;

out_unmap_dsp:
	if(info->buf_addr.aec_dbg_out_virt)
		munmap(info->buf_addr.aec_dbg_out_virt, info->buf_addr.dbg_len);
	return ret;
}

int ceva_tl421_aec_dbg_close(void *handle, struct aec_dbg_info *info) {

	if(info) {
		if(info->buf_addr.aec_dbg_out_virt)
			munmap(info->buf_addr.aec_dbg_out_virt, info->buf_addr.dbg_len);
	}
	return 0;
}

int ceva_tl421_aec_set_format(void *handle, struct aec_info *info)
{
	int ret;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, TL421_AEC_SET_FMT, info);
	if (ret < 0) {
		log("aec failed to set format\n");
		return ret;
	}

	ret = mmap(0, info->buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, info->buf_addr.spk_in_phy);
	if(ret == MAP_FAILED) {
		log("couldn't mmap memory space for spk: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	info->buf_addr.spk_in_virt = (short *)ret;

	ret = mmap(0, info->buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, info->buf_addr.mic_in_phy);
	if(ret == MAP_FAILED) {
		log("couldn't mmap memory space for mic: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	info->buf_addr.mic_in_virt = (short *)ret;

	ret = mmap(0, info->buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, info->buf_addr.aec_out_phy); // they share the same buffer because they don't used simultaneously
	if(ret == MAP_FAILED) {
		log("couldn't mmap memory space for mout/sout: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	info->buf_addr.aec_out_virt = (short *)ret;

	return 0;

out_unmap_dsp:

	if(info->buf_addr.spk_in_virt)
		munmap(info->buf_addr.spk_in_virt, info->buf_addr.data_len);
	if(info->buf_addr.mic_in_virt)
		munmap(info->buf_addr.mic_in_virt, info->buf_addr.data_len);
	if(info->buf_addr.aec_out_virt)
		munmap(info->buf_addr.aec_out_virt, info->buf_addr.data_len);
	return ret;
}

int ceva_tl421_aec_close(void *handle, struct aec_info *info) {

	if(info) {
		if(info->buf_addr.spk_in_virt) {
			munmap(info->buf_addr.spk_in_virt, info->buf_addr.data_len);
		}
		if(info->buf_addr.mic_in_virt) {
			munmap(info->buf_addr.mic_in_virt, info->buf_addr.data_len);
		}
		if(info->buf_addr.aec_out_virt) {
			munmap(info->buf_addr.aec_out_virt, info->buf_addr.data_len);
		}
	}

	ioctl(fd, TL421_AEC_RELEASE, NULL);
	ceva_tl421_close(NULL);
	return 0;
}

int ceva_tl421_get_format(void *handle, struct codec_info *info)
{
	int ret;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, TL421_GET_FMT, info);
	if (ret < 0) {
		log("failed to get format\n");
		return ret;
	}

	return 0;
}

int ceva_tl421_encode(void *handle, char *dst, char *src, uint32_t len)
{
	int32_t ret;
	struct task_info info;
	
	if (fd < 0)
		return -1;

	info.src = (uint32_t)src;
	info.dst = (uint32_t)dst;
	info.len = len;

	ret = ioctl(fd, TL421_ENCODE, &info);
	if (ret < 0)
		log("failed to encode\n");

	if(after_1stfr_enc == 0){
		after_1stfr_enc = 1;
		/*First encode request will output 2 frames, return the size of first frame, omit the second frame*/
		ret = ceva_get_acc_framelen(dst);
	}
	return ret;
}

int ceva_tl421_decode(void *handle, char *dst, char *src, int len)
{
	int ret;
	struct task_info info;
	
	if (fd < 0)
		return -1;

	info.src = (uint32_t)src;
	info.dst = (uint32_t)dst;
	info.len = len;

	ret = ioctl(fd, TL421_DECODE, &info);
	if (ret < 0)
		log("failed to decode\n");

	return ret;
}

int ceva_tl421_aec_process(void *handle, struct aec_codec *aec_addr)
{
	int ret;

	if (fd < 0)
		return -1;

	ret = ioctl(fd, TL421_AEC_PROCESS, aec_addr);
	if (ret < 0)
		log("failed to process aec\n");

	return ret;
}

float ceva_tl421_get_comp_ratio(void *handle, int mode, void *data)
{
	return -1;
}

int ceva_tl421_convert(void *handle, char *dst, char *src, int len)
{
	return -1;
}

int ceva_tl421_iotest(void)
{
	int ret;
	struct task_info info;

	if (fd < 0)
		return -1;

	info.src = 0x12345678;
	info.dst = 0x87654321;
	info.len = 100;

	ret = ioctl(fd, TL421_IOTEST, &info);
	if (ret < 0) {
		log("failed to iotest\n");
		return ret;
	}

	return 0;
}
