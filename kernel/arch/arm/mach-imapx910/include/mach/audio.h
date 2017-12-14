#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <linux/i2c.h>

enum ctrl_mode {
	IIC_MODE,
	SPI_MODE,
	NOR_MODE,
};

enum data_mode {
	IIS_MODE,
	PCM_MODE,
	SPDIF_MODE,
	NORMAL_MODE,
};

struct audio_register {
	char codec_name[64];
	char cpu_dai_name[64];
	enum ctrl_mode ctrl;
	enum data_mode data;
	struct platform_device *codec_device;
	struct platform_device *cpu_dai_device;
	struct i2c_board_info *info;
	int (*imapx_audio_cfg_init)(char *codec_name,
			char *cpu_dai_name, enum data_mode data, int enable);
};

extern int imapx_es8328_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable);
extern int imapx_rt5631_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable);
extern int imapx_spdif_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable);
extern int imapx_es8323_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable);
extern int imapx_tqlp9624_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable);

/* tqlp9624 must get clk before reset */
extern void enable_i2s_clk(void);
extern void disable_i2s_clk(void);
extern void resume_i2s_clk(void);

extern int imapx_audio_get_state(void);
#endif
