#ifndef __I80_H__
#define __I80_H__

#include <linux/types.h>

#define I80IF_BASE_ADDR     (0x3000)
#define I80TRIGCON         (I80IF_BASE_ADDR + 0x00)
#define I80IFCON0          (I80IF_BASE_ADDR + 0x04)
#define I80IFCON1          (I80IF_BASE_ADDR + 0x08)
#define I80CMDCON0         (I80IF_BASE_ADDR + 0x0C)
#define I80CMDCON1         (I80IF_BASE_ADDR + 0x10)
#define I80CMD15           (I80IF_BASE_ADDR + 0x14)
#define I80CMD14           (I80IF_BASE_ADDR + 0x18)
#define I80CMD13           (I80IF_BASE_ADDR + 0x1C)
#define I80CMD12           (I80IF_BASE_ADDR + 0x20)
#define I80CMD11           (I80IF_BASE_ADDR + 0x24)
#define I80CMD10           (I80IF_BASE_ADDR + 0x28)
#define I80CMD09           (I80IF_BASE_ADDR + 0x2C)
#define I80CMD08           (I80IF_BASE_ADDR + 0x30)
#define I80CMD07           (I80IF_BASE_ADDR + 0x34)
#define I80CMD06           (I80IF_BASE_ADDR + 0x38)
#define I80CMD05           (I80IF_BASE_ADDR + 0x3C)
#define I80CMD04           (I80IF_BASE_ADDR + 0x40)
#define I80CMD03           (I80IF_BASE_ADDR + 0x44)
#define I80CMD02           (I80IF_BASE_ADDR + 0x48)
#define I80CMD01           (I80IF_BASE_ADDR + 0x4C)
#define I80CMD00           (I80IF_BASE_ADDR + 0x50)
#define I80MANCON          (I80IF_BASE_ADDR + 0x54)
#define I80MANWDAT         (I80IF_BASE_ADDR + 0x58)
#define I80MANRDAT         (I80IF_BASE_ADDR + 0x5C)

#define I80IF_TRIGCON_TR_VIDEO_OVER     (4)
#define I80IF_TRIGCON_TR_NORMALCMD      (3)
#define I80IF_TRIGCON_TR_AUTOCMD        (2)
#define I80IF_TRIGCON_TR_DATA           (1)
#define I80IF_TRIGCON_NORMAL_CMD_ST     (0)

#define I80IF_IFCON0_DBI_INT             (28)
#define I80IF_IFCON0_CS_SETUP     (23)
#define I80IF_IFCON0_WR_SETUP     (17)
#define I80IF_IFCON0_WR_ACTIVE    (11)
#define I80IF_IFCON0_WR_HOLD      (4) 
#define I80IF_IFCON0_INTR_EN  (2) 
#define I80IF_IFCON0_MAINLCD_CFG        (1) 
#define I80IF_IFCON0_EN                 (0) 

#define I80IF_DBI_COMPATIBLE_EN     (17)    
#define I80IF_IFCON1_PORTTYPE           (15)
#define I80IF_IFCON1_PORTDISTRIBUTED    (13)
#define I80IF_IFCON1_DATASTYLE          (10)
#define I80IF_IFCON1_DATALSBFIRST       (9) 
#define I80IF_IFCON1_DATAUSELOWPORT     (8) 
#define I80IF_IFCON1_DISAUTOCMD         (4) 
#define I80IF_IFCON1_AUTOCMDRATE        (0) 
                                            
#define I80IF_IFCON1_DATA_FORMAT        (8) 
                                            
#define I80IF_CMDCON0_CMD15EN           (30)
#define I80IF_CMDCON0_CMD14EN           (28)
#define I80IF_CMDCON0_CMD13EN           (26)
#define I80IF_CMDCON0_CMD12EN           (24)
#define I80IF_CMDCON0_CMD11EN           (22)
#define I80IF_CMDCON0_CMD10EN           (20)
#define I80IF_CMDCON0_CMD09EN           (18)
#define I80IF_CMDCON0_CMD08EN           (16)
#define I80IF_CMDCON0_CMD07EN           (14)
#define I80IF_CMDCON0_CMD06EN           (12)
#define I80IF_CMDCON0_CMD05EN           (10)
#define I80IF_CMDCON0_CMD04EN           (8) 
#define I80IF_CMDCON0_CMD03EN           (6) 
#define I80IF_CMDCON0_CMD02EN           (4) 
#define I80IF_CMDCON0_CMD01EN           (2) 
#define I80IF_CMDCON0_CMD00EN           (0) 

#define I80IF_CMDCON1_CMD15RS           (30)
#define I80IF_CMDCON1_CMD14RS           (28)
#define I80IF_CMDCON1_CMD13RS           (26)
#define I80IF_CMDCON1_CMD12RS           (24)
#define I80IF_CMDCON1_CMD11RS           (22)
#define I80IF_CMDCON1_CMD10RS           (20)
#define I80IF_CMDCON1_CMD09RS           (18)
#define I80IF_CMDCON1_CMD08RS           (16)
#define I80IF_CMDCON1_CMD07RS           (14)
#define I80IF_CMDCON1_CMD06RS           (12)
#define I80IF_CMDCON1_CMD05RS           (10)
#define I80IF_CMDCON1_CMD04RS           (8) 
#define I80IF_CMDCON1_CMD03RS           (6) 
#define I80IF_CMDCON1_CMD02RS           (4) 
#define I80IF_CMDCON1_CMD01RS           (2) 
#define I80IF_CMDCON1_CMD00RS           (0) 

#define I80IF_MANCON_SIG_DOE            (6)
#define I80IF_MANCON_SIG_RS             (5)
#define I80IF_MANCON_SIG_CS0            (4)
#define I80IF_MANCON_SIG_CS1            (3)
#define I80IF_MANCON_SIG_RD             (2)
#define I80IF_MANCON_SIG_WR             (1)
#define I80IF_MANCON_EN                 (0)

enum i80_data_format{
	I80IF_BUS18_xx_xx_x_x   =   0x180,

	I80IF_BUS9_1x_xx_0_x    =   0x0c0,
	I80IF_BUS9_1x_xx_1_x    =   0x0c2,
	I80IF_BUS9_0x_xx_0_x    =   0x080,
	I80IF_BUS9_0x_xx_1_x    =   0x082,

	I80IF_BUS16_10_11_x_x   =   0x14C,
	I80IF_BUS16_00_11_x_x   =   0x10C,
	I80IF_BUS16_11_11_x_x   =   0x16C,
	I80IF_BUS16_01_11_x_x   =   0x12C,
	I80IF_BUS16_10_10_0_0   =   0x148,
	I80IF_BUS16_10_10_0_1   =   0x149,
	I80IF_BUS16_10_10_1_0   =   0x14A,
	I80IF_BUS16_10_10_1_1   =   0x14B,
	I80IF_BUS16_10_01_1_0   =   0x146,
	I80IF_BUS16_10_01_1_1   =   0x147,
	I80IF_BUS16_10_01_0_0   =   0x144,
	I80IF_BUS16_10_01_0_1   =   0x145,
	I80IF_BUS16_00_10_0_0   =   0x108,
	I80IF_BUS16_00_10_0_1   =   0x109,
	I80IF_BUS16_00_10_1_0   =   0x10A,
	I80IF_BUS16_00_10_1_1   =   0x10B,
	I80IF_BUS16_00_01_1_0   =   0x106,
	I80IF_BUS16_00_01_1_1   =   0x107,
	I80IF_BUS16_00_01_0_0   =   0x104,
	I80IF_BUS16_00_01_0_1   =   0x105,
	I80IF_BUS16_11_10_0_0   =   0x168,
	I80IF_BUS16_11_10_0_1   =   0x169,
	I80IF_BUS16_11_10_1_0   =   0x16A,
	I80IF_BUS16_11_10_1_1   =   0x16B,
	I80IF_BUS16_11_01_1_1   =   0x167,
	I80IF_BUS16_11_01_0_1   =   0x165,
	I80IF_BUS16_11_01_1_0   =   0x166,
	I80IF_BUS16_01_10_0_0   =   0x128,
	I80IF_BUS16_01_10_0_1   =   0x129,
	I80IF_BUS16_01_10_1_0   =   0x12A,
	I80IF_BUS16_01_10_1_1   =   0x12B,
	I80IF_BUS16_01_01_1_0   =   0x126,
	I80IF_BUS16_01_01_1_1   =   0x127,
	I80IF_BUS16_01_01_0_0   =   0x124,
	I80IF_BUS16_01_01_0_1   =   0x125,
	I80IF_BUS8_0x_11_0_x    =   0x00C,
	I80IF_BUS8_0x_11_1_x    =   0x00D,
	I80IF_BUS8_1x_11_0_x    =   0x04C,
	I80IF_BUS8_1x_11_1_x    =   0x04E,
	I80IF_BUS8_0x_10_0_1    =   0x009,
	I80IF_BUS8_0x_10_0_0    =   0x008,
	I80IF_BUS8_0x_10_1_1    =   0x00B,
	I80IF_BUS8_0x_10_1_0    =   0x00A,
	I80IF_BUS8_1x_10_0_1    =   0x049,
	I80IF_BUS8_1x_10_0_0    =   0x048,
	I80IF_BUS8_1x_10_1_1    =   0x04B,
	I80IF_BUS8_1x_10_1_0    =   0x04A,
	I80IF_BUS8_0x_01_0_1    =   0x005,
	I80IF_BUS8_0x_01_0_0    =   0x004,
	I80IF_BUS8_0x_01_1_1    =   0x007,
	I80IF_BUS8_0x_01_1_0    =   0x006,
	I80IF_BUS8_0x_00_1_1    =   0x003,
	I80IF_BUS8_0x_00_1_0    =   0x002,
	I80IF_BUS8_0x_00_0_1    =   0x001,
	I80IF_BUS8_0x_00_0_0    =   0x000,
	I80IF_BUS8_1x_01_0_1    =   0x045,
	I80IF_BUS8_1x_01_0_0    =   0x044,
	I80IF_BUS8_1x_01_1_1    =   0x047,
	I80IF_BUS8_1x_01_1_0    =   0x046,
	I80IF_BUS8_1x_00_1_1    =   0x043,
	I80IF_BUS8_1x_00_1_0    =   0x042,
	I80IF_BUS8_1x_00_0_1    =   0x041,
	I80IF_BUS8_1x_00_0_0    =   0x040,
}; 

struct dev_cfg {
	uint32_t addr;
	uint32_t data;
	int delay;
};

enum I80_TYPE {
	I80_CMD,
	I80_PARA,
	I80_DELAY,
};

struct dev_init {
	enum I80_TYPE type;
	uint32_t data;
};

enum I80_POLARITY_TYPE {
	I80_ACTIVE_LOW,
	I80_ACTIVE_HIGH,
};

enum I80_INT_TYPE {
	NO_INT,
	FRAME_OVER,
	NOR_CMD_OVER,
	FRAME_OR_CMD_OVER,
};

enum MAIN_LCD {
	CS0_MAIN_LCD,
	CS1_MAIN_LCD,
};

enum I80_AUTO_TYPE {
	AUTOCMD_BEFORE_VIDEO,
	VIDEO_DATA_ONLY,
};

enum AUTO_CMD_RATE {
	AUTO_RATE_PER_1,
	AUTO_RATE_PER_2,
	AUTO_RATE_PER_4,
	AUTO_RATE_PER_6,
	AUTO_RATE_PER_8,
	AUTO_RATE_PER_10,
	AUTO_RATE_PER_12,
	AUTO_RATE_PER_14,
	AUTO_RATE_PER_16,
	AUTO_RATE_PER_18,
	AUTO_RATE_PER_20,
	AUTO_RATE_PER_22,
	AUTO_RATE_PER_24,
	AUTO_RATE_PER_26,
	AUTO_RATE_PER_28,
	AUTO_RATE_PER_30,
};

enum SIGNAL_CTRL {
	I80_MAN_EN,
	I80_MAN_WR,
	I80_MAN_RD,
	I80_MAN_CS1,
	I80_MAN_CS0,
	I80_MAN_RS,
	I80_MAN_DOE,
};

enum I80_CMDTYPE {
	CMD_DISABLE,
	CMD_NORMAL_ENABLE,
	CMD_AUTO_ENABLE,
	CMD_NORMAL_AUTO,
};

enum I80_DATA_TYPE {
	I80_8_WIRE_MODE,
	I80_9_WIRE_MODE,
	I80_10_WIRE_MODE,
	I80_11_WIRE_MODE,
};

struct i80_polarity {
	int rd_n;
	int wr_n;
	int rs_n;
	int cs1_n;
	int cs0_n;
};

struct i80_cycles {
	int cs_setup;
	int wr_setup;
	int wr_act;
	int wr_hold;
};

struct i80_data {
	int porttype;
	int datastyle;
	int datalsb;
	int datauselowport;
	int portdistributed;
};

struct i80_autocmd {
	enum I80_AUTO_TYPE type;
	enum AUTO_CMD_RATE rate;
};

struct i80_cmd {
	int num;
	int cmd;
	int rs_ctrl;
	enum I80_CMDTYPE type;
};

struct i80_dev {
	char *name;
	struct list_head link;
	int (*config)(uint8_t *mem);
	int (*open)(void);
	int (*enable)(int en);
	int (*close)(void);
	int (*write)(uint32_t *mem, int length, int mode);
	int (*display_manual)(char *mem);
};

extern int i80_init(int num);
extern void panel_enable(int en);
extern void i80_enable(int flags);
extern void i80_set_polarity(struct i80_polarity *polarity);
extern void i80_set_clock_cycle(struct i80_cycles *cycle);
extern void i80_set_int_mask(enum I80_INT_TYPE type);
extern void i80_set_date_structure(struct i80_data *data);
extern void i80_mannual_write_once(int data);
extern int i80_mannual_read_once(void);
extern void i80_set_autocmd(struct i80_autocmd *cmd);
extern void i80_set_cmd(struct i80_cmd *cmd);
extern void i80_mannual_ctrl(enum SIGNAL_CTRL ctrl, int value);
extern void i80_mannual_init(void);
extern void i80_mannual_deinit(void);
extern int i80_wait_idle(void);
extern void i80_trigger_normal_cmd(int flags);
extern void i80_default_cfg(int format);
/* I80 interface */
extern int i80_register(struct i80_dev *udev);
extern int i80_dev_cfg(uint8_t *mem, int format);
extern int i80_close(void);
extern int i80_open(char *name);
extern int i80_mannual_write(uint32_t *mem, int length, int mode);
extern int tft3p3469_init(void);
extern int st7789s_init(void);
extern int st7567_init(void);
#endif
