#ifndef __LIB_TL421_H__
#define __LIB_TL421_H__

int ceva_tl421_open(void *dev);
int ceva_tl421_close(void *handle);
int ceva_tl421_set_format(void *handle, struct codec_info *info);
int ceva_tl421_get_format(void *handle, struct codec_info *info);
int ceva_tl421_encode(void *handle, char *dst, char *src, uint32_t len);
int ceva_tl421_decode(void *handle, char *dst, char *src, int len);
float ceva_tl421_get_comp_ratio(void *handle, int mode, void *data);
int ceva_tl421_convert(void *handle, char *dst, char *src, int len);
int ceva_tl421_iotest(void);

int ceva_tl421_aec_set_format(void *handle, struct aec_info *info);
int ceva_tl421_aec_close(void *handle, struct aec_info *info);
int ceva_tl421_aec_process(void *handle, struct aec_codec *aec_addr);

int ceva_tl421_aec_dbg_open(void *handle, struct aec_dbg_info *info);
int ceva_tl421_aec_dbg_close(void *handle, struct aec_dbg_info *info);

#endif
