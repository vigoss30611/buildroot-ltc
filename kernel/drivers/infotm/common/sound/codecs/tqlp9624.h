#ifndef __TQLP9624_H__
#define __TQLP9624_H__

#include <linux/regulator/consumer.h>

#define RACDR           0x0000
#define MCSSR           0x0004
#define RSTR0           0x0400
#define LTHR1           0x0404
#define ADCSR2          0x0408
#define DACSR3          0x040c
#define I2SC0R12        0x0430
#define I2SC1R13        0x0434
#define I2SC2R14        0x0438
#define SPCR17          0x0444
#define PC0R21          0x0454
#define PC1R22          0x0458
#define PC2R23          0x045c
#define PC3R24          0x0460
#define PC4R25          0x0464
#define PC5R26          0x0468
#define MC0R29          0x0474
#define MC2R31          0x047c
#define MC4R33          0x0484
#define RLCHMVCR36      0x0490
#define RRCHMVCR37      0x0494
#define PGALCHVCR38     0x0498
#define PGARCHVCR39     0x049C
#define PLCHMVCR52      0x04D0
#define PRCHMVCR53      0x04D4
#define SHDLCHVCR56     0x04E0
#define SHDRCHVCR57     0x04E4
#define RLCHISR71       0x051C
#define RRCHISR72       0x0520
#define ACLINEOE0R      0x0554
#define ACLINEOE1R      0x0558
#define PMLCHCR89       0x0564
#define PMRCHCR90       0x0568
#define POPSC1R162      0x0688
#define POPSC1R163      0x068C
#define POPSCS1LR       0x0690
#define POPSCS1HR       0x0694
#define POPSCS2LR       0x0698
#define POPSCS2HR       0x069C
#define POPSCS3LR       0x06A0
#define POPSCS3HR       0x06A4
#define POPSCS4LR       0x06A8
#define POPSCS4HR       0x06AC
#define POPSCS5LR       0x06B0
#define POPSCS5HR       0x06B4
#define POPSCS6LR       0x06B8
#define POPSCS6HR       0x06BC
#define POPSCS7LR       0x06C0
#define POPSCS7HR       0x06C4
#define POPSCS8LR       0x06C8
#define POPSCS8HR       0x06CC
#define POPSCS9LR       0x06D0
#define POPSCS9HR       0x06D4
#define POPSCS10LR      0x06D8
#define POPSCS10HR      0x06DC
#define POPSCS11LR      0x06E0
#define POPSCS11HR      0x06E4
#define POPSCS12LR      0x06E8
#define POPSCS12HR      0x06EC
#define POPSCS13LR      0x06F0
#define POPSCS13HR      0x06F4
#define POPSCS14LR      0x06F8
#define POPSCS14HR      0x06FC
#define POPSCS15LR      0x0700
#define POPSCS15HR      0x0704
#define POPSCS16LR      0x0708
#define POPSCS16HR      0x070C
#define ALCC0R200       0x0720
#define ALCC1R201       0x0724
#define ALCC2R202       0x0728
#define ALCRDCR203      0x072C
#define ALCRUCR204      0x0730
#define ALCMASGR205     0x0734
#define DNGCR206        0x0738
#define DTR210          0x0748
#define ADCHPMR211      0x074C
#define DACMR212        0x0750
#define SRR213          0x0754
#define DAAT1R240       0x07C0
#define DAAT2R241       0x07C4
#define DAAT3R242       0x07C8
#define DAAT4R243       0x07CC
#define PAT0R248        0x07E0
#define PAT1R249        0x07E4
#define PAT2R250        0x07E8
#define PAT3R251        0x07EC

#define HEADSET_L_MUTE_SHIFT	4
#define HEADSET_R_MUTE_SHIFT	5

#define PGA_L_MUTE_SHIFT    3
#define PGA_R_MUTE_SHIFT    2

#define RECORD_L_MUTE_SHIFT    3
#define RECORD_R_MUTE_SHIFT    2

#define BYPASS_L_MUTE_SHIFT    3
#define BYPASS_R_MUTE_SHIFT    2

#define PWR_MICBIAS_BIT		1

/* for acodec sys register */
#define ACODEC_INTF_CTRL        0x20
#define ACODEC_SOFT_RST         0x0

#define DEFAULT_REG     (-1)
#define DEFAULT_VAL     (-1)

#define CLOCK_SEL_256   0
#define CLOCK_SEL_384   1

enum CLK_MODE {
	CLK_IIS_MODE,
	CLK_SYS_MODE,
};

enum REG_CTRL_MODE {
	CTRL_SET_REG,
	CTRL_SET_LATCH,
	CTRL_SET_SLEEP,
};

struct set_reg {
	uint32_t reg;
	uint32_t val;
	int      mode;
};

enum SAMPLE_FREQ_SEL {
	SAMPLE_FREQ_8000,
	SAMPLE_FREQ_11025,
	SAMPLE_FREQ_12000,
	SAMPLE_FREQ_16000,
	SAMPLE_FREQ_22025,
	SAMPLE_FREQ_24000,
	SAMPLE_FREQ_32000,
	SAMPLE_FREQ_44100,
	SAMPLE_FREQ_48000,
	SAMPLE_FREQ_96000,
	SAMPLE_FREQ_192000,
};

struct sample_freq {
	int freq;
	int sample_sel;
};

#define SET_REG_VAL(_reg, _val, _mode)   \
{\
	.reg        = _reg,                 \
	.val        = _val,                 \
	.mode       = _mode,                \
}

struct tqlp9624_priv {
	void __iomem *base;
	struct regulator *regulator;
	struct codec_cfg *cfg;
	struct clk *clk;
	int clk_sel;
	int sample_rate;
	int spken;
	int hdmien;
};

#endif
