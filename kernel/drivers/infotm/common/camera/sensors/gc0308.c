/*                                                                  
 * A V4L2 driver for Gcore GC0308 cameras.                     
 *                                                                  
 * Copyright 2006 One Laptop Per Child Association, Inc.  Written   
 * by Jonathan Corbet with substantial inspiration from Mark        
 * McClelland's ovcamchip code.                                     
 *                                                                  
 * Copyright 2006-7 Jonathan Corbet <corbet@lwn.net>                
 *                                                                  
 * This file may be distributed under the terms of the GNU General  
 * Public License, version 2.                                       
 */                                                                 


 #include <linux/init.h>           
 #include <linux/module.h>         
 #include <linux/slab.h>           
 #include <linux/i2c.h>            
 #include <linux/delay.h>          
 #include <linux/videodev2.h>      
 #include <media/v4l2-device.h>    
 #include <media/v4l2-chip-ident.h>
 #include <media/v4l2-ctrls.h>     
 #include <media/v4l2-mediabus.h>  

#define SENSOR_MAX_HEIGHT   480
#define SENSOR_MAX_WIDTH    640

#define SENSOR_MIN_WIDTH    640
#define SENSOR_MIN_HEIGHT   480

struct reginfo {      
	unsigned char reg;
	unsigned char val;  
};

struct gc0308_win_size {
	int width;
	int height;
	struct reginfo *regs;
};


struct gc0308_info {
	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrls;
	struct i2c_client *client;
	enum v4l2_mbus_type bus_type;
	struct mutex		lock;

};

                        

static struct reginfo sensor_init[] = {
	{0xfe,0x80},  	
	{0xfe,0x00},        // set page0
	{0xd2,0x10},  // close AEC
	{0x22,0x55},  // close AWB
	{0x5a,0x56},
	{0x5b,0x40},
	{0x5c,0x4a},			
	{0x22,0x57}, // Open AWB

	{0x01,0x6a},	  // 24M
	{0x02,0x70},
	{0x0f,0x00},

	{0x03,0x01},
	{0x04,0x2c},

	{0xe2,0x00},	//anti-flicker step [11:8]
	{0xe3,0x96},  //anti-flicker step [7:0]

	{0xe4,0x02},  //exp level 1  25fps
	{0xe5,0x58},
	{0xe6,0x02},  //exp level 2  25fps
	{0xe7,0x58},
	{0xe8,0x02},  //exp level 3  25fps
	{0xe9,0x58},  //76
	{0xea,0x05},  //exp level 4  5.00fps
	{0xeb,0xdc},

	{0x05,0x00},
	{0x06,0x00},
	{0x07,0x00},
	{0x08,0x00},
	{0x09,0x01},
	{0x0a,0xe8},
	{0x0b,0x02},
	{0x0c,0x88},
	{0x0d,0x02},
	{0x0e,0x02},
	{0x10,0x22},
	{0x11,0xfd},
	{0x12,0x2a},
	{0x13,0x00},

#if   defined(CONFIG_IG_CAMIF0_MIRR) && defined(CONFIG_IG_CAMIF0_FLIP)
	{0x14,0x13},                                 
#elif defined(CONFIG_IG_CAMIF0_MIRR)
	{0x14,0x11},                                 
#elif defined(CONFIG_IG_CAMIF0_FLIP)
	{0x14,0x12},
#else
	{0x14,0x10},
#endif
	//  0x10:normal ,0x11:IMAGE_H_MIRROR,0x12:IMAGE_V_MIRROR,0x13:IMAGE_HV_MIRROR
	{0x15,0x0a},
	{0x16,0x05},
	{0x17,0x01},
	{0x18,0x44},
	{0x19,0x44},
	{0x1a,0x1e},
	{0x1b,0x00},
	{0x1c,0xc1},
	{0x1d,0x08},
	{0x1e,0x60},
	{0x1f,0x16}, //pad drv ,00 03 13 1f 3f james remarked
	{0x20,0xff},
	{0x21,0xf8},
	{0x22,0x57},
	{0x23,0x00},
	{0x24,0xa2},
	{0x25,0x0f},//output sync_mode       
	{0x26,0x02},//vsync  maybe need changed, value is 0x02
	{0x2f,0x01},
	{0x30,0xf7},
	{0x31,0x50},
	{0x32,0x00},
	{0x39,0x04},
	{0x3a,0x18},
	{0x3b,0x20},
	{0x3c,0x00},
	{0x3d,0x00},
	{0x3e,0x00},
	{0x3f,0x00},
	{0x50,0x10},
	{0x53,0x82},
	{0x54,0x80},
	{0x55,0x80},
	{0x56,0x82},
	{0x8b,0x40},
	{0x8c,0x40},
	{0x8d,0x40},
	{0x8e,0x2e},
	{0x8f,0x2e},
	{0x90,0x2e},
	{0x91,0x3c},
	{0x92,0x50},
	{0x5d,0x12},
	{0x5e,0x1a},
	{0x5f,0x24},
	{0x60,0x07},
	{0x61,0x15},
	{0x62,0x08},
	{0x64,0x03},
	{0x66,0xe8},
	{0x67,0x86},
	{0x68,0xa2},
	{0x69,0x18},
	{0x6a,0x0f},
	{0x6b,0x00},
	{0x6c,0x5f},
	{0x6d,0x8f},
	{0x6e,0x55},
	{0x6f,0x38},
	{0x70,0x15},
	{0x71,0x33},
	{0x72,0xdc},
	{0x73,0x80},
	{0x74,0x02},
	{0x75,0x3f},
	{0x76,0x02},
	{0x77,0x36},
	{0x78,0x88},
	{0x79,0x81},
	{0x7a,0x81},
	{0x7b,0x22},
	{0x7c,0xff},
	{0x93,0x48},
	{0x94,0x00},
	{0x95,0x05},
	{0x96,0xe8},
	{0x97,0x40},
	{0x98,0xf0},
	{0xb1,0x38},
	{0xb2,0x38},
	{0xbd,0x38},
	{0xbe,0x36},
	{0xd0,0xc9},
	{0xd1,0x10},//{0xd2,0x90},
	{0xd3,0x80},
	{0xd5,0xf2},
	{0xd6,0x16},
	{0xdb,0x92},
	{0xdc,0xa5},
	{0xdf,0x23},
	{0xd9,0x00},
	{0xda,0x00},
	{0xe0,0x09},
	{0xec,0x20},
	{0xed,0x04},
	{0xee,0xa0},
	{0xef,0x40},
	{0x80,0x03},
	{0x80,0x03},
	{0x9F,0x10},
	{0xA0,0x20},
	{0xA1,0x38},
	{0xA2,0x4E},
	{0xA3,0x63},
	{0xA4,0x76},
	{0xA5,0x87},
	{0xA6,0xA2},
	{0xA7,0xB8},
	{0xA8,0xCA},
	{0xA9,0xD8},
	{0xAA,0xE3},
	{0xAB,0xEB},
	{0xAC,0xF0},
	{0xAD,0xF8},
	{0xAE,0xFD},
	{0xAF,0xFF},
	{0xc0,0x00},
	{0xc1,0x10},
	{0xc2,0x1C},
	{0xc3,0x30},
	{0xc4,0x43},
	{0xc5,0x54},
	{0xc6,0x65},
	{0xc7,0x75},
	{0xc8,0x93},
	{0xc9,0xB0},
	{0xca,0xCB},
	{0xcb,0xE6},
	{0xcc,0xFF},
	{0xf0,0x02},
	{0xf1,0x01},
	{0xf2,0x01},
	{0xf3,0x30},
	{0xf9,0x9f},
	{0xfa,0x78},
	{0xfe,0x01},  //set page 1
	{0x00,0xf5},
	{0x02,0x1a},
	{0x0a,0xa0},
	{0x0b,0x60},
	{0x0c,0x08},
	{0x0e,0x4c},
	{0x0f,0x39},
	{0x11,0x3f},
	{0x12,0x72},
	{0x13,0x13},
	{0x14,0x42},
	{0x15,0x43},
	{0x16,0xc2},
	{0x17,0xa8},
	{0x18,0x18},
	{0x19,0x40},
	{0x1a,0xd0},
	{0x1b,0xf5},
	{0x70,0x40},
	{0x71,0x58},
	{0x72,0x30},
	{0x73,0x48},
	{0x74,0x20},
	{0x75,0x60},
	{0x77,0x20},
	{0x78,0x32},
	{0x30,0x03},
	{0x31,0x40},
	{0x32,0xe0},
	{0x33,0xe0},
	{0x34,0xe0},
	{0x35,0xb0},
	{0x36,0xc0},
	{0x37,0xc0},
	{0x38,0x04},
	{0x39,0x09},
	{0x3a,0x12},
	{0x3b,0x1C},
	{0x3c,0x28},
	{0x3d,0x31},
	{0x3e,0x44},
	{0x3f,0x57},
	{0x40,0x6C},
	{0x41,0x81},
	{0x42,0x94},
	{0x43,0xA7},
	{0x44,0xB8},
	{0x45,0xD6},
	{0x46,0xEE},
	{0x47,0x0d},
	{0xfe,0x00},  
	{0xd2,0x90}, // Open AEC at last.  //-----------Update the registers 2010/07/07-------------//
	{0xfe,0x00},//set Page0
	{0x10,0x26},                               
	{0x11,0x0d},// fd,modified by mormo 2010/07/06                               
	{0x1a,0x2a},// 1e,modified by mormo 2010/07/06                                  
	{0x1c,0x49}, // c1,modified by mormo 2010/07/06                                 
	{0x1d,0x9a}, // 08,modified by mormo 2010/07/06                                 
	{0x1e,0x61}, // 60,modified by mormo 2010/07/06                                 
	{0x3a,0x20},
	{0x50,0x14},// 10,modified by mormo 2010/07/06                               
	{0x53,0x80},                                
	{0x56,0x80},
	{0x8b,0x20}, //LSC                                 
	{0x8c,0x20},                                
	{0x8d,0x20},                                
	{0x8e,0x14},                                
	{0x8f,0x10},                                
	{0x90,0x14},                                
	{0x94,0x02},                                
	{0x95,0x07},                                
	{0x96,0xe0},                                
	{0xb1,0x40}, // YCPT                                 
	{0xb2,0x40},                                
	{0xb3,0x40},
	{0xb6,0xe0},
	{0xd0,0xcb}, // AECT  c9,modifed by mormo 2010/07/06                                
	{0xd3,0x48}, // 80,modified by mormor 2010/07/06                           
	{0xf2,0x02},                                
	{0xf7,0x12},
	{0xf8,0x0a},
	{0xfe,0x01},//set  Page1
	{0x02,0x20},
	{0x04,0x10},
	{0x05,0x08},
	{0x06,0x20},
	{0x08,0x0a},
	{0x0e,0x44},                                
	{0x0f,0x32},
	{0x10,0x41},                                
	{0x11,0x37},                                
	{0x12,0x22},                                
	{0x13,0x19},                                
	{0x14,0x44},                                
	{0x15,0x44},
	{0x19,0x50},                                
	{0x1a,0xd8}, 
	{0x32,0x10}, 
	{0x35,0x00},                                
	{0x36,0x80},                                
	{0x37,0x00}, //-----------Update the registers end---------//
	{0xfe,0x00},// set back for page1
	{0x9F,0x0E},//-----------GAMMA Select(2)---------------//
	{0xA0,0x1C},
	{0xA1,0x34},
	{0xA2,0x48},
	{0xA3,0x5A},
	{0xA4,0x6B},
	{0xA5,0x7B},
	{0xA6,0x95},
	{0xA7,0xAB},
	{0xA8,0xBF},
	{0xA9,0xCE},
	{0xAA,0xD9},
	{0xAB,0xE4},
	{0xAC,0xEC},
	{0xAD,0xF7},
	{0xAE,0xFD},
	{0xAF,0xFF},
	{0x00,0x00}
};

static struct reginfo sensor_qcif[] = {
	{0xfe, 0x01},
	{0x54, 0x33},// 1/3 = 213*160
	{0x55, 0x00},
	{0x56, 0x00},
	{0x57, 0x00},
	{0x58, 0x00},
	{0x59, 0x00},
	{0xfe, 0x00},
	
	{0x46, 0x80},
	{0x47, 0x08},
	{0x48, 0x14},
	{0x49, 0x00},//144
	{0x4a, 0x90},
	{0x4b, 0x00},//176
	{0x4c, 0xB0}, 
	{0x00, 0x00},
};

static struct reginfo sensor_qvga[] = {
	{0xfe, 0x01},
	{0x54, 0x22},// 1/2
	{0x55, 0x03},
	{0x56, 0x00},
	{0x57, 0x00},
	{0x58, 0x00},
	{0x59, 0x00},
	{0xfe, 0x00},
	
	{0x46, 0x80},
	{0x47, 0x00},
	{0x48, 0x00},
	{0x49, 0x00},//240
	{0x4a, 0xf0},
	{0x4b, 0x01},//320
	{0x4c, 0x40},
	{0x00, 0x00},
};

static struct reginfo sensor_cif[] = {
	{0xfe, 0x01},
	{0x53, 0x83},
	{0x54, 0x55},// 3/5=384*288
	{0x55, 0x03},  
	{0x56, 0x00},
	{0x57, 0x24},
	{0x58, 0x00},
	{0x59, 0x24},
	{0xfe, 0x00},
	
	{0x46, 0x80},
	{0x47, 0x00},
	{0x48, 0x10},
	{0x49, 0x01},//288
	{0x4a, 0x20},
	{0x4b, 0x01},//352
	{0x4c, 0x60},
	{0x00, 0x00},	
};

static struct reginfo sensor_vga[] = {                                   
	{0xfe, 0x01}, 	  
	{0x54, 0x11}, //subsample 
	{0x55, 0x01},   
	{0x56, 0x00}, 		 
	{0x57, 0x00}, 		 
	{0x58, 0x00}, 		 
	{0x59, 0x00}, 		 
	{0xfe, 0x00},  
	
	{0x46, 0x80},
	{0x47, 0x00},
	{0x48, 0x00},
	{0x49, 0x01},//480
	{0x4a, 0xe0},
	{0x4b, 0x02},//640
	{0x4c, 0x80},
	{0x00, 0x00},
};

static  struct reginfo  ev_neg4_regs[] = {
	{0xb5,0xc0},
	{0xd3,0x30},
	{0x00,0x00},
};

static  struct reginfo  ev_neg3_regs[] = {
	{0xb5,0xd0},
	{0xd3,0x38},
	{0x00,0x00},
};

static  struct reginfo  ev_neg2_regs[] = {
	{0xb5,0xe0},
	{0xd3,0x40},
	{0x00,0x00},
};

static  struct reginfo  ev_neg1_regs[] = {
	{0xb5,0xf0},
	{0xd3,0x48},
	{0x00,0x00},
};

static  struct reginfo  ev_zero_regs[] = {
	{0xb5,0x00},    
	{0xd3,0x48},//50
	{0x00,0x00},
};

static  struct reginfo  ev_pos1_regs[] = {
	{0xb5,0x10},
	{0xd3,0x60},
	{0x00,0x00},
};

static  struct reginfo  ev_pos2_regs[] = {
	{0xb5,0x20}, 
	{0xd3,0x70}, 
	{0x00,0x00},
};

static  struct reginfo  ev_pos3_regs[] = {
	{0xb5,0x30}, 
	{0xd3,0x80}, 
	{0x00,0x00},
};

static  struct reginfo  ev_pos4_regs[] = {
	{0xb5,0x40},
	{0xd3,0x90},
	{0x00,0x00},
};

static  struct reginfo  wb_auto_regs[] = { 
	{0x5a,0x57},//for AWB can adjust back                 
	{0x5b,0x40},                                          
	{0x5c,0x4a},                                          
	{0x22,0x57},                                                                              
	{0x00,0x00},
};                                                        

static  struct reginfo  wb_incandescent_regs[] = {
	{0x22,0x55},                                         
	{0x5a,0x46},                                         
	{0x5b,0x40},                                         
	{0x5c,0x6c},                                         
	{0x00,0x00},
};                                                       

static  struct reginfo  wb_flourscent_regs[] = {
	{0x22,0x55},
	{0x5a,0x50},
	{0x5b,0x40},
	{0x5c,0x45},
	{0x00,0x00},
};                                                       

static  struct reginfo  wb_daylight_regs[] = {
	{0x22,0x55},
	{0x5a,0x50},
	{0x5b,0x40},
	{0x5c,0x48},
	{0x00,0x00},
};

static  struct reginfo  wb_cloudy_regs[] = {
	{0x22,0x55},                                           
	{0x5a,0x5c},                                           
	{0x5b,0x40},                                           
	{0x5c,0x40},                                             
	{0x00,0x00},
};

static  struct reginfo  scence_night60hz_regs[] = {
	{0xb5,0x1a},          //add ae                                     
	{0xd3,0x70},                                              
	{0x5a,0x57},
	{0x5b,0x40},                                          
	{0x5c,0x4a},                                          
	{0x22,0x57}, 
	{0xb1,0x40},
	{0xb2,0x40},
	{0x64,0x03},
	{0x00,0x00},
};

static  struct reginfo  scence_auto_regs[] = {
	{0xb5,0x00},                                               
	{0xd3,0x48}, 
	{0x5a,0x57},
	{0x5b,0x40},                                          
	{0x5c,0x4a},                                          
	{0x22,0x57}, //awb
	{0xb1,0x40},
	{0xb2,0x40},
	{0x64,0x03},
	{0x00,0x00},
};


static  struct reginfo  scence_sunset60hz_regs[] = {
	{0xb5,0x00},                                               
	{0xd3,0x48}, 
    {0x22,0x55},  //set wb                                       
	{0x5a,0x46},                                         
	{0x5b,0x40},                                         
	{0x5c,0x6c},                                         
	{0xb1,0x40},
	{0xb2,0x40},
	{0x64,0x03},
	{0x00,0x00},
};

static struct reginfo  scence_party_indoor_regs [] = {
	{0xb5, 0x00}, 
	{0x5a, 0x57},
	{0x5b, 0x40},                                          
	{0x5c, 0x4a},                                          
	{0x22, 0x57}, //awb
	{0xd3, 0x60},
	{0xb1, 0x44},
	{0xb2, 0x44},
	{0x64, 0x03},
	{0x00, 0x00}
};

static struct reginfo scence_sports_regs [] = {
	{0xb5,0x00},                                               
	{0xd3,0x48}, 
	{0x5a,0x57},
	{0x5b,0x40},                                          
	{0x5c,0x4a},                                          
	{0x22,0x57}, //awb
	{0xb1,0x40},
	{0xb2,0x40},
	{0x64,0x01},
	{0x00,0x00},
};

static  struct reginfo  colorfx_none_regs[] = {
	{0x23,0x00},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	{0x77,0x65},
	            
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},
	
	{0x00,0x00}
};

static  struct reginfo  colorfx_mono_regs[] = {
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	            
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},

	{0x00,0x00}
};

static  struct reginfo  colorfx_negative_regs[] = {
	{0x23,0x01},
	{0x2d,0x0a},
	{0x20,0x7f},
	{0xd2,0x90},
	{0x73,0x00},
	{0x77,0x54},
	            
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},

	{0x00,0x00}
};

static  struct reginfo  colorfx_sepia_regs[] = {
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0xd0},
	{0xbb,0x28},

	{0x00,0x00}
};

static  struct reginfo  colorfx_sepiagreen_regs[] = {
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x77,0x88},
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0xc0},
	{0xbb,0xc0},

	{0x00,0x00}
};

static  struct reginfo  colorfx_sepiablue_regs[] = {
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x50},
	{0xbb,0xe0},

	{0x00,0x00}
};

static struct reginfo antibanding_50hz_regs[] = {
	{0x01,0xfa},
	{0x02,0x70},
	{0x0f,0x01},
	{0xe2,0x00},
	{0xe3,0x64},
	{0xe4,0x02},
	{0xe5,0x58},
	{0xe6,0x03},
	{0xe7,0xe8},
	{0xe8,0x05},
	{0xe9,0x14},
	{0xea,0x07},
	{0xeb,0xd0},
	{0x00,0x00}
};

static struct reginfo antibanding_60hz_regs[] = {
	{0x01,0x32},
	{0x02,0x70},
	{0x0f,0x01},
	{0xe2,0x00},
	{0xe3,0x32},
	{0xe4,0x02},
	{0xe5,0x58},
	{0xe6,0x02},
	{0xe7,0xbc},
	{0xe8,0x03},
	{0xe9,0x52},
	{0xea,0x04},
	{0xeb,0xb0},
	{0x00,0x00}
};

static  struct reginfo  sensor_out_yuv[] = {
	{0x00,0x00}
};

struct gc0308_win_size gc0308_win_sizes[] = { 	
    /* QCIF */
	{                                                      
		.width = 176,                                      
		.height = 144,                                     
		.regs  = sensor_qcif,                              
	},   
	/* QVGA */
	{                                                      
		.width = 320,                                      
		.height = 240,                                     
		.regs  = sensor_qvga,                              
	},

	/* CIF */
	{                                                      
		.width = 352,                                      
		.height = 288,                                     
		.regs  = sensor_cif,                              
	},    
	/* VGA */                                             
	{                                                      
		.width = 640,                                      
		.height = 480,                                     
		.regs  = sensor_vga,                              
	}
};                                                         


/* only one fixed colorspace per pixelcode */ 
struct gc0308_data_fmt {                       
	enum v4l2_mbus_pixelcode code;            
	enum v4l2_colorspace colorspace;          
	struct reginfo	*regs;
};


struct gc0308_data_fmt gc0308_data_fmts[] = { 
	{
		.code = V4L2_MBUS_FMT_YUYV8_2X8, 
		.colorspace = V4L2_COLORSPACE_JPEG,
		.regs	= sensor_out_yuv, //FIXME 
	},
};                                                          

static struct gc0308_data_fmt *sensor_find_datafmt(             
		enum v4l2_mbus_pixelcode code, struct gc0308_data_fmt *fmt, 
		int n)                                                           
{                                                                    
	int i;                                                           
	for (i = 0; i < n; i++)                                          
		if (fmt[i].code == code)                                     
			return &fmt[i];                                          

	return fmt;                                                     
}      

static struct gc0308_win_size *sensor_find_win(             
		struct v4l2_mbus_framefmt *mf, struct gc0308_win_size *win, 
		int n)                                                           
{                                                                    
	int i;                                                           
	for (i = 0; i < n; i++)                                          
		if (win[i].width == mf->width && win[i].height == mf->height)                                     
			return &win[i];                                          

	return win;                                                     
}                                                               

static struct gc0308_info * to_sensor(struct v4l2_subdev *sd)            
{                                                                           
	return container_of(sd, struct gc0308_info, sd);
}                                                                           

static int gc0308_write(struct i2c_client *client, uint8_t reg, uint8_t val)
{
	int err,cnt;                            
	u8 buf[2];                              
	struct i2c_msg msg[1];                  

	buf[0] = reg & 0xFF;                    
	buf[1] = val;                           

	msg->addr = client->addr;               
	msg->flags = I2C_M_NOSTART;             
	msg->buf = buf;                         
	msg->len = sizeof(buf);                 
	cnt = 3;      
	err = -EAGAIN;
	err = i2c_transfer(client->adapter, msg, 1);
	if(1 != err) {
		pr_err("GC0308 write reg(0x%x, val:0x%x) failed, ecode=%d \
			\n",	reg, val, err);

		return err;
	}

	return 0;
}

static uint8_t gc0308_read(struct i2c_client *client, uint8_t reg) 
{
	int err ,cnt;
	struct i2c_msg msg[2];
	char buf= 0xff;

	msg[0].addr = client->addr;                
	msg[0].flags = I2C_M_NOSTART;              
	msg[0].buf = &reg;                          
	msg[0].len = 1;                  

	msg[1].addr = client->addr;               
	msg[1].flags = I2C_M_RD;    
	msg[1].buf = &buf; 
	msg[1].len = 1;                           

	cnt = 1;
	err = -EAGAIN;
	err = i2c_transfer(client->adapter, msg, 2); 
	if(err != 2)
		pr_err("GC0308 read reg(0x%x)failed!\n", reg);

	return buf;
}

static int gc0308_write_array(struct i2c_client *client,
										struct reginfo *regarray)
{
	int i = 0, err;
	
	while((regarray[i].reg != 0) || (regarray[i].val != 0))                     
	{                                                                              
		err = gc0308_write(client, regarray[i].reg, regarray[i].val);              
		if (err != 0)                                                              
		{                                                                       
			printk(KERN_ERR "gc0308 write failed current i = %d\n",i);
			return err;                                                            
		}                                                                          
		i++;                                                                       
	}                                                                              
	return 0;
}


#if 0
static const struct v4l2_subdev_pad_ops gc0308_pad_ops = { 
	.enum_mbus_code = gc0308_enum_mbus_code,               
	.enum_frame_size = gc0308_enum_frame_sizes,            
	.get_fmt = gc0308_get_fmt,                             
	.set_fmt = gc0308_set_fmt,                             
};                                                         
#endif

static int gc0308_g_chip_ident(struct v4l2_subdev *sd,                    
		struct v4l2_dbg_chip_ident *chip)                                 
{                                                                         
	struct i2c_client *client = v4l2_get_subdevdata(sd);                  
	return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_GC0308, 0);
}                                                                         


static int gc0308_reset(struct v4l2_subdev *sd, u32 val)
{                                                       
	return 0;                                           
}                                                       

static int is_sensor_gc0308(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	u8 sen = gc0308_read(client, 0x0);
	pr_info("[gc0308] Sensor ID:%x,req:0x9b\n",sen);
	return (sen == 0x9b) ? 1 : 0;
}

static int gc0308_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	/*
	 * val - 1: Just read sensor ID
	 *     - 0: init sensor
	 */
	if(val) 
		return !is_sensor_gc0308(sd);
	else
		return gc0308_write_array(client, sensor_init);
}	

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int gc0308_g_register(struct v4l2_subdev *sd,
							struct v4l2_dbg_register *reg)
{
	pr_info("[gc0308] %s\n", __func__);
	return 0;
}

static int gc0308_s_register(struct v4l2_subdev *sd, 
							const struct v4l2_dbg_register *reg)
{
	pr_info("[gc0308] %s\n", __func__);
	return 0;
}

#endif


static int gc0308_set_exposure(struct i2c_client *client,
								struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case -4:
			ret = gc0308_write_array(client, ev_neg4_regs);
			break;
		case -3:
			ret = gc0308_write_array(client, ev_neg3_regs);
			break;
		case -2:
			ret = gc0308_write_array(client, ev_neg2_regs);
			break;
		case -1:
			ret = gc0308_write_array(client, ev_neg1_regs);
			break;
		case 0:
			ret = gc0308_write_array(client, ev_zero_regs);
			break;
		case 1:
			ret = gc0308_write_array(client, ev_pos1_regs);
			break;
		case 2:
			ret = gc0308_write_array(client, ev_pos2_regs);
			break;
		case 3:
			ret = gc0308_write_array(client, ev_pos3_regs);
			break;
		case 4:
			ret = gc0308_write_array(client, ev_pos4_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0308_set_wb(struct i2c_client *client,  struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_WHITE_BALANCE_AUTO: 
			ret = gc0308_write_array(client, wb_auto_regs);
			break;
		case V4L2_WHITE_BALANCE_INCANDESCENT:
			ret = gc0308_write_array(client, wb_incandescent_regs);
			break;
		case V4L2_WHITE_BALANCE_FLUORESCENT:
			ret = gc0308_write_array(client, wb_flourscent_regs);
			break;
		case V4L2_WHITE_BALANCE_DAYLIGHT:
			ret = gc0308_write_array(client, wb_daylight_regs);
			break;
		case V4L2_WHITE_BALANCE_CLOUDY:
			ret = gc0308_write_array(client, wb_cloudy_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0308_set_scence(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_SCENE_MODE_NONE:
			ret = gc0308_write_array(client, scence_auto_regs); 
			break;
		case V4L2_SCENE_MODE_NIGHT:
			ret = gc0308_write_array(client, scence_night60hz_regs);
			break;
		case V4L2_SCENE_MODE_SUNSET:
			ret = gc0308_write_array(client, scence_sunset60hz_regs);
			break;
		case V4L2_SCENE_MODE_PARTY_INDOOR:
			ret = gc0308_write_array(client, scence_party_indoor_regs);
			break;
		case V4L2_SCENE_MODE_SPORTS:
			ret = gc0308_write_array(client, scence_sports_regs);
			break;
		default:
			break;
	}
	return ret;

}

static int gc0308_set_colorfx(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_COLORFX_NONE:
			ret = gc0308_write_array(client, colorfx_none_regs);
			break;
		case V4L2_COLORFX_BW:
			ret = gc0308_write_array(client, colorfx_mono_regs);
			break;
		case V4L2_COLORFX_SEPIA:
			ret = gc0308_write_array(client, colorfx_sepia_regs);
			break;
		case V4L2_COLORFX_NEGATIVE:
			ret = gc0308_write_array(client, colorfx_negative_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0308_set_powlinefreq(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED:
		case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
		case V4L2_CID_POWER_LINE_FREQUENCY_AUTO:
			ret = gc0308_write_array(client, antibanding_60hz_regs);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
			ret = gc0308_write_array(client, antibanding_50hz_regs);
			break;
		default:
			break;
	}
	return -1;
}
static int gc0308_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = &container_of(ctrl->handler, struct gc0308_info, ctrls)->sd;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = -1;
	switch (ctrl->id) {
		case V4L2_CID_EXPOSURE_ABSOLUTE:
			ret = gc0308_set_exposure(client, ctrl);
			break;	
		case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
			ret = gc0308_set_wb(client, ctrl);
			break;
		case V4L2_CID_SCENE_MODE:
			ret = gc0308_set_scence(client, ctrl);
			break;
		case V4L2_CID_COLORFX:
			ret = gc0308_set_colorfx(client, ctrl);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY:
			ret = gc0308_set_powlinefreq(client, ctrl);
			break;
		default:
		   break;
	}
	return ret; 
}

static int gc0308_enable(struct v4l2_subdev *sd, int enable)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    pr_info("in gc0308 enable = %d ***************\n", enable);

    return 0;
}


static struct v4l2_ctrl_ops gc0308_ctrl_ops = {
	.s_ctrl = gc0308_s_ctrl,
};

static const struct v4l2_subdev_core_ops gc0308_core_ops = { 
	.g_chip_ident = gc0308_g_chip_ident,                     
	.reset = gc0308_reset,                                   
	.init = gc0308_init,
#ifdef CONFIG_VIDEO_ADV_DEBUG                                
	.g_register = gc0308_g_register,                         
	.s_register = gc0308_s_register,                         
#endif                                                       
};                                                           



static int gc0308_enum_mbus_fmt(struct v4l2_subdev *sd, unsigned int index,
							enum v4l2_mbus_pixelcode *code)
{
	pr_info("[gc0308] %s\n",__func__);
	if (index >= ARRAY_SIZE(gc0308_data_fmts))
		return -EINVAL;                         
	*code = gc0308_data_fmts[index].code;     
	return 0;                                   
}


static int gc0308_try_mbus_fmt_internal(struct gc0308_info *sensor,
										struct v4l2_mbus_framefmt *mf,
										struct gc0308_data_fmt **ret_fmt,
										struct gc0308_win_size  **ret_size)
{
	int ret = 0;                             

	if(ret_fmt != NULL)
		*ret_fmt = sensor_find_datafmt(mf->code, gc0308_data_fmts,
				ARRAY_SIZE(gc0308_data_fmts));        
	if(ret_size != NULL)
		*ret_size = sensor_find_win(mf, gc0308_win_sizes,
				ARRAY_SIZE(gc0308_win_sizes));        

	return ret;
}

static int gc0308_try_mbus_fmt(struct v4l2_subdev *sd, 
									struct v4l2_mbus_framefmt *mf)
{

	struct gc0308_info *sensor = to_sensor(sd);                  
	pr_info("[gc0308] %s\n",__func__);
	return gc0308_try_mbus_fmt_internal(sensor, mf, NULL, NULL);
}

static int gc0308_s_mbus_fmt(struct v4l2_subdev *sd, 
								struct v4l2_mbus_framefmt *mf)
{
	struct gc0308_data_fmt *ovfmt; 
	struct gc0308_win_size *wsize;      
	struct i2c_client *client =  v4l2_get_subdevdata(sd);
	struct gc0308_info *sensor = to_sensor(sd);                  
	int ret;                                    

	pr_info("[gc0308] %s\n",__func__);
	ret = gc0308_try_mbus_fmt_internal(sensor, mf, &ovfmt, &wsize);
	if(ovfmt->regs)
		ret = gc0308_write_array(client, ovfmt->regs);
	if(wsize->regs)
		ret = gc0308_write_array(client, wsize->regs);
	return ret;


}

static int gc0308_g_mbus_fmt(struct v4l2_subdev *sd,
								struct v4l2_mbus_framefmt *mf)
{
	pr_info("[gc0308] %s\n",__func__);
	return 0;
}

static int gc0308_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{                                                                              
	pr_info("[gc0308] %s\n",__func__);
	return 0;                              
}

static int gc0308_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{                                                                              
	pr_info("[gc0308] %s\n",__func__);
	return 0;                              
}


static int gc0308_enum_framesizes(struct v4l2_subdev *sd,
		        struct v4l2_frmsizeenum *fsize)                  
{
	if(fsize->index >=  ARRAY_SIZE(gc0308_win_sizes))
		            return -1;
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE; 
	fsize->discrete.width = gc0308_win_sizes[fsize->index].width;       
	fsize->discrete.height = gc0308_win_sizes[fsize->index].height;     
	return 0;
}


/*  
struct v4l2_fract {
	__u32   numerator;
	__u32   denominator;
};
*/

static unsigned int gc0308_numerator = 1;
static unsigned int gc0308_denominator = 15;   

static int gc0308_g_frame_interval(struct v4l2_subdev *sd,
        struct v4l2_subdev_frame_interval *interval)
{

    pr_info("[gc0308] %s\n",__func__);
    if(interval == NULL)
        return -1;

    interval->interval.numerator = 1;
    interval->interval.denominator = 15;
    
    return 0;                              
}

static	int gc0308_s_frame_interval(struct v4l2_subdev *sd,
        struct v4l2_subdev_frame_interval *interval)
{

    pr_info("[gc0308] %s\n",__func__);

    if(interval == NULL)
        return -1;

    gc0308_numerator = interval->interval.numerator;
    gc0308_denominator = interval->interval.denominator;

    
    return 0;                              
}

static	int gc0308_enum_frameintervals(struct v4l2_subdev *sd, 
        struct v4l2_frmivalenum *fival)
{

    pr_info("[gc0308] %s\n",__func__);
    if(fival == NULL)
        return -1;
    
    if(fival->index != 0)
       return -1; 

	fival->type = V4L2_FRMSIZE_TYPE_DISCRETE; 
    fival->discrete.numerator = 1;
    fival->discrete.denominator = 15;
 
	return 0;                              
}


static const struct v4l2_subdev_video_ops gc0308_video_ops = { 
    .enum_mbus_fmt = gc0308_enum_mbus_fmt,                     
    .try_mbus_fmt = gc0308_try_mbus_fmt,                       
    .s_mbus_fmt = gc0308_s_mbus_fmt,                           
	.g_mbus_fmt = gc0308_g_mbus_fmt,
	.s_parm = gc0308_s_parm,                                   
	.g_parm = gc0308_g_parm,                                   
	.enum_framesizes = gc0308_enum_framesizes,
    .enum_frameintervals = gc0308_enum_frameintervals,
    .s_frame_interval = gc0308_s_frame_interval,
    .g_frame_interval = gc0308_g_frame_interval,
    .s_stream = gc0308_enable,
};                                                             

static const struct v4l2_subdev_ops gc0308_subdev_ops = { 
	.core = &gc0308_core_ops,                             
	//.pad = &gc0308_pad_ops,                               
	.video = &gc0308_video_ops,                           
};                                                        

static const struct i2c_device_id gc0308_id[] = { 
	{ "gc0308_demo", 0},                   
	{}                                           
};                                                
MODULE_DEVICE_TABLE(i2c, gc0308_id);              

static int gc0308_probe(struct i2c_client *client,
		            const struct i2c_device_id *id)       
{
	struct v4l2_subdev *sd;
	struct gc0308_info *sensor;
	int ret;

	pr_info("[gc0308] %s\n", __func__);
	
	sensor = devm_kzalloc(&client->dev, sizeof(*sensor), GFP_KERNEL);
	if (!sensor)       
		return -ENOMEM;

	mutex_init(&sensor->lock);
	sensor->client = client;
	sd = &sensor->sd;
	ret = v4l2_ctrl_handler_init(&sensor->ctrls, 4);
	if (ret < 0)                                             
		return ret;                                          
	v4l2_ctrl_new_std(&sensor->ctrls, &gc0308_ctrl_ops,
			V4L2_CID_EXPOSURE_ABSOLUTE, -4, 4, 1, 0);                                         

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0308_ctrl_ops,
			V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, 9, ~0x14e,
			V4L2_WHITE_BALANCE_AUTO);                                         
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0308_ctrl_ops,
			V4L2_CID_SCENE_MODE, V4L2_SCENE_MODE_TEXT, ~0x1f01,
			V4L2_SCENE_MODE_NONE);                                         

/*
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0308_ctrl_ops,
			V4L2_CID_COLORFX, V4L2_COLORFX_EMBOSS, ~0xf,
			V4L2_COLORFX_NONE);                                         
*/
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0308_ctrl_ops, 
			V4L2_CID_POWER_LINE_FREQUENCY, V4L2_CID_POWER_LINE_FREQUENCY_AUTO,
		   	~0xf, V4L2_CID_POWER_LINE_FREQUENCY_AUTO);

	sensor->sd.ctrl_handler = &sensor->ctrls;                    
	v4l2_i2c_subdev_init(sd, client, &gc0308_subdev_ops);
	strlcpy(sd->name, "gc0308_demo", sizeof(sd->name));    

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE |  
		V4L2_SUBDEV_FL_HAS_EVENTS;        
	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &sensor->pad, 0);
	if (ret < 0)                                             
		return ret;                                          

	return 0;
}

static int gc0308_remove(struct i2c_client *client)
{
	return 0;
}
static struct i2c_driver gc0308_driver = {
	.driver = {                           
		.owner  = THIS_MODULE,            
		.name   = "gc0308_demo",               
	},                                    
	.probe      = gc0308_probe,           
	.remove     = gc0308_remove,          
	.id_table   = gc0308_id,              
};                                        

module_i2c_driver(gc0308_driver);         

MODULE_AUTHOR("peter"); 
MODULE_DESCRIPTION("GC0308 CMOS Image Sensor driver");       
MODULE_LICENSE("GPL");                                              


