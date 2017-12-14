#ifndef _XC9080_COMMON_H
#define _XC9080_COMMON_H

#include <linux/types.h>
#include <linux/i2c.h>

#define XC9080_ID_HIGH		0x60
#define XC9080_ID_LOW		0x71

#define HM2131_ID_HIGH	0x31
#define HM2131_ID_LOW	0x21

#define XC9080_CHIP_ADDR (0x36 >> 1)
#define HM2131_CHIP_ADDR (0x48 >> 1)

#ifndef XC9080_BYPASS_OFF
#define XC9080_BYPASS_OFF	0
#endif

#ifndef XC9080_BYPASS_ON
#define XC9080_BYPASS_ON	1
#endif

#define XC9080_1920_1080_RAW10	1
#define XC9080_2160_1080_RAW10	2
#define XC9080_3840_1920_RAW10	3
#define XC9080_1920_1080_YUV	4
#define XC9080_2160_1080_YUV	5
#define XC9080_2160_1080_YUV_CROP	6
#define XC9080_2160_1080_YUV_SCALE	7

struct reginfo {
	uint16_t reg;
	uint8_t val;
};

extern struct reginfo hm2131_raw10[];
extern struct reginfo hm2131_yuv[];
extern struct reginfo xc9080_1920_1080_raw10[];
extern struct reginfo xc9080_3840_1920_raw10[];
extern struct reginfo xc9080_2160_1080_raw10[];
extern struct reginfo xc9080_1920_1080_yuv[];
extern struct reginfo xc9080_2160_1080_yuv[];
extern struct reginfo xc9080_2160_1080_yuv_crop[];
extern struct reginfo xc9080_2160_1080_yuv_scale[];
extern struct reginfo sensor_qcif[];
extern struct reginfo sensor_qvga[];
extern struct reginfo sensor_cif[];
extern struct reginfo sensor_vga[];
extern struct reginfo  ev_neg4_regs[];
extern struct reginfo  ev_neg3_regs[];
extern struct reginfo  ev_neg2_regs[];
extern struct reginfo  ev_neg1_regs[];
extern struct reginfo  ev_zero_regs[];
extern struct reginfo  ev_pos1_regs[];
extern struct reginfo  ev_pos2_regs[];
extern struct reginfo  ev_pos3_regs[];
extern struct reginfo  ev_pos4_regs[];
extern struct reginfo  wb_auto_regs[];
extern struct reginfo  wb_incandescent_regs[];
extern struct reginfo  wb_flourscent_regs[];
extern struct reginfo  wb_daylight_regs[];
extern struct reginfo  wb_cloudy_regs[];
extern struct reginfo  scence_night60hz_regs[];
extern struct reginfo  scence_auto_regs[];
extern struct reginfo  scence_sunset60hz_regs[];
extern struct reginfo  scence_party_indoor_regs[];
extern struct reginfo scence_sports_regs[];
extern struct reginfo  colorfx_none_regs[];
extern struct reginfo  colorfx_mono_regs[];
extern struct reginfo  colorfx_negative_regs[];
extern struct reginfo  colorfx_sepia_regs[];
extern struct reginfo  colorfx_sepiagreen_regs[];
extern struct reginfo  colorfx_sepiablue_regs[];
extern struct reginfo antibanding_50hz_regs[];
extern struct reginfo antibanding_60hz_regs[];
extern struct reginfo sensor_out_yuv[];
extern struct reginfo sensor_2160_1080[];
extern struct reginfo  sensor_1440_720[];
extern struct reginfo xc9080_bypass_on[];
extern struct reginfo xc9080_bypass_off[];
extern struct reginfo xc9080_mipi_enable[];
extern struct reginfo xc9080_mipi_disable[];

extern const int hm2131_raw10_array_size;
extern const int hm2131_yuv_array_size;
extern const int xc9080_1920_1080_raw10_array_size;
extern const int xc9080_3840_1920_raw10_array_size;
extern const int xc9080_2160_1080_raw10_array_size;
extern const int xc9080_1920_1080_yuv_array_size;
extern const int xc9080_2160_1080_yuv_array_size;
extern const int xc9080_2160_1080_yuv_crop_array_size;
extern const int xc9080_2160_1080_yuv_scale_array_size;

extern int xc9080_write(struct i2c_client *client, uint16_t reg, uint8_t val);
extern int hm2131_write(struct i2c_client *client, uint16_t reg, uint8_t val);
extern int xc9080_read(struct i2c_client *client, uint16_t reg, uint8_t *val);
extern int hm2131_read(struct i2c_client *client, uint16_t reg, uint8_t *val);
extern int xc9080_write_array(struct i2c_client *client, struct reginfo *regarray);
extern int xc9080_write_array2(struct i2c_client *client, struct reginfo *regs, int size);
extern int hm2131_write_array(struct i2c_client *client, struct reginfo *regarray);
extern int hm2131_write_array2(struct i2c_client *client, struct reginfo *regs, int size);
extern int hm2131_get_register(struct i2c_client *client, struct reginfo *regarray);
extern int xc9080_get_register(struct i2c_client *client, struct reginfo *regarray);
extern int hm2131_get_id(struct i2c_client *client);
extern int xc9080_get_id(struct i2c_client *client);
extern int xc9080_bypass_change(struct i2c_client *client, int on_off);
extern int xc9080_bypass_change(struct i2c_client *client, int on_off);

#endif /*_XC9080_COMMON_H*/
