#ifndef __IMAPX800_G2D_HEADER__
#define __IMAPX800_G2D_HEADER__

#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/types.h>

#define width_align (~0xf)

#define SHARPEN_ENABLE		1		/* 1:Enable. 0:Disable */
#define SHARPEN_DENOS_EN	0		/* 1:Enable. 0:Disale */
#define SHARPEN_COEFW				0x0A	/* dex: 0 ~ 100 */
#define SHARPEN_COEFA				0x40
#define SHARPEN_ERRTH				0x6E
#define SHARPEN_HVDTH				0x28

#define G2D_NAME 		"g2d"
#define G2D_SYSFS_LINK_NAME	"g2d"
#define G2D_DRAW_TIMEOUT	(HZ/5)

#define G2D_VIC	16		// 1080P for G2D FB
#define G2D_FB_BPP	2

#define G2D_DEV_MAJOR		246		/* As you wish */
#define G2D_FB_NAME		"g2d_fb"
#define G2D_FB_GET_WIDTH	0x4680
#define G2D_FB_GET_HEIGHT	0x4681
#define G2D_FB_GET_BPP		0x4682



#define P_SHP	"smtv.ids.sharpness"

/*
typedef struct{
	void * vir_addr;
	unsigned int phy_addr;
	unsigned int size;
	unsigned int flag;
}IM_Buffer;

typedef struct{
	IM_Buffer	buffer;
	unsigned int devAddr;
	int pageNum;
	unsigned int  attri;
	void *	privData;
}alc_buffer_t;
*/


struct g2d_params{
	unsigned int	imgFormat;
	unsigned int	brRevs;
	unsigned int	width;
	unsigned int	height;
	unsigned int	fulWidth;
	unsigned int	fulHeight;
	unsigned int	xOffset;
	unsigned int	yOffset;
	unsigned int	phy_addr;
	void *	vir_addr;
	unsigned int	size;
};

struct g2d_alpha_rop{
	unsigned int	ckEn;
	unsigned int	blendEn;
	unsigned int	matchMode;	// G2D_COLORKEY_DIR_MATCH_xxx.
	unsigned int	mask;		// format is RGB888.
	unsigned int	color;		// format is RGB888.
	unsigned int	gAlphaEn;
	unsigned int	gAlphaA;	//(set value)0--255<==>(real value)0--1.0
	unsigned int	gAlphaB;
	unsigned int	arType;	//G2D_AR_TYPE_XXX
	unsigned int	ropCode;
};

struct g2d_sclee{		
	//SCL
	unsigned int	verEnable;
	unsigned int	horEnable;
	unsigned int	vrdMode;
	unsigned int	hrdMode;
	unsigned int	paramType;

	//EE
	unsigned int	eeEnable;
	unsigned int	order;
	unsigned int	coefw;
	unsigned int	coefa;
	unsigned int	rdMode;	
	unsigned int	denosEnable;
	unsigned int	gasMode;
	unsigned int	errTh;
	unsigned int	opMatType;
	unsigned int	hTh;
	unsigned int	vTh;
	unsigned int	d0Th;
	unsigned int	d1Th;
};

struct g2d_dither {
	unsigned int dithEn;
	unsigned int rChannel;
	unsigned int gChannel;
	unsigned int bChannel;
	unsigned int tempoEn;
};

struct g2d {
	struct platform_device *pdev;
//	struct early_suspend early_suspend;
	struct workqueue_struct *wq;
	struct work_struct wk;
	struct completion g2d_done;
	unsigned int regbase_physical;	/* G2D register base physical address */
	unsigned int * regbase_virtual;	/* G2D register base virtual address */
	unsigned int sysmgr_physical;	/* G2D sysmgr register base physical address */
	unsigned int * sysmgr_virtual;	/* G2D sysmgr register base virtual address */
	unsigned int mmu_physical;	/* G2D mmu register base physical address */
	unsigned int * mmu_virtual;	/* G2D mmu register base virtual address */
	unsigned int main_fb_phyaddr;	/* Main framebuffer physical address */
	unsigned int main_fb_size;		/* Main framebuffer size */
//	unsigned int g2d_fb_phyaddr;	/* G2D framebuffer physical address */
	dma_addr_t g2d_fb_phyaddr;	/* G2D framebuffer physical address */
	unsigned int *g2d_fb_viraddr;	/* G2D framebuffer virtual address. Useless */
	unsigned int g2d_fb_size;		/* G2D framebuffer size */
	int g2d_fbx;					/* Current G2D FB[x] that is using */
	int irq;
	void * dmmu_handle;
	unsigned int regVal[456];
	unsigned int regOft[456];
	struct g2d_params src;
	struct g2d_params dst;
	struct g2d_alpha_rop alpharop;
	struct g2d_sclee sclee;
	struct g2d_dither dith;
	dev_t devno;
	struct cdev *cdev;
	struct class * cls;
	struct device * dev;
	int sharpen_en;					/* Sharpen Enable */
	int denos_en;						/* DeNoise Enable */
	int coefw;
	int coefa;
	int errTh;
	int hvdTh;
	int needKeyFlush;
	int needAllFlush;
	int power;
	int g2d_on;
	int idsx;
	int VIC;
	int factor_width;
	int factor_height;
};






/* ----------------------------------------------
 *  --------------- Registers related -----------------
 *  --------------------------------------------- */



//
// Alpha-rop work type
//
#define G2D_AR_TYPE_SCLSRC						0	//scale(src)
#define G2D_AR_TYPE_AFA_SCLSRC_DEST				1	//alpha(scale(src),dst)
#define G2D_AR_TYPE_AFA_SCLSRC_PAT				2	//alpha(scale(src),pat)
#define G2D_AR_TYPE_ROP_SCLSRC_PAT				3	//rop3(scale(src),pat)
#define G2D_AR_TYPE_ROP_SCLSRC_DEST_PAT			4	//rop3(scale(src),dst,pat)
#define G2D_AR_TYPE_ROP_AFA_SCLSRC_DEST_PAT		5	//rop3(alpha(scale(src),dst),pat), has bug, not support
#define G2D_AR_TYPE_ROP_AFA_SCLSRC_PAT_DEST		6	//rop3(alpha(scale(src),pat),dst), has bug, not support
#define G2D_AR_TYPE_AFA_ROP_SCLSRC_PAT_DEST		7	//alpha(rop3(scale(src),pat),dst), has bug, not support
#define G2D_AR_TYPE_AFA_ROP_SCLSRC_DEST_PAT		8	//alpha(rop3(scale(src),dst),pat), has bug, not support
#define G2D_AR_TYPE_ROP_SCLSRC_DEST				9	//rop3(scale(src),dst)

#define G2D_IMAGE_RGB_BPP16_565   5
#define G2D_IMAGE_RGB_BPP24_888   11

#define G2D_MMU_SWITCH_OFFSET 	(0x8)

#define G2D_GLB_INTS		(0xE4)
#define G2DGLBINTS_0VER		(1<<0)

#define G2D_GLB_INTM		(0xE8)
#define G2DGLBINTM_OVER		(1<<0)

#define DMMU_DEV_G2D		(0)

#define ALC_FLAG_PHY_MUST		(1<<2)	// physical and linear requirement.
#define ALC_FLAG_DEVADDR		(1<<12)	// request the devaddr, used for dev-mmu.


/*****************************************************
*  define g2d register number                        *
*****************************************************/
#define rG2D_OUT_BITSWP           0
#define rG2D_IF_XYST              1
#define rG2D_IF_WIDTH             2
#define rG2D_IF_HEIGHT            3
#define rG2D_IF_FULL_WIDTH        4
#define rG2D_IF_ADDR_ST           5
#define rG2D_MEM_ADDR_ST          6
#define rG2D_MEM_LENGTH_ST        7
#define rG2D_MEM_SETDATA32        8
#define rG2D_MEM_SETDATA64        9
#define rG2D_MEM_CODE_LEN         10
#define rG2D_ALU_CNTL             11

#define rG2D_SRC_XYST             12
#define rG2D_SRC_SIZE             13
#define rG2D_SRC_SIZE_1           14
#define rG2D_SRC_FULL_WIDTH       15
#define rG2D_DEST_XYST            16
#define rG2D_DEST_SIZE            17
#define rG2D_DEST_SIZE_1          18
#define rG2D_DEST_FULL_WIDTH      19
#define rG2D_PAT_SIZE             20
#define rG2D_SRC_BITSWP           21
#define rG2D_SRC_FORMAT           22
#define rG2D_SRC_WIDOFFSET        23
#define rG2D_SRC_RGB              24
#define rG2D_SRC_COEF1112         25
#define rG2D_SRC_COEF1321         26
#define rG2D_SRC_COEF2223         27
#define rG2D_SRC_COEF3132         28
#define rG2D_SRC_COEF33_OFT       29
#define rG2D_DEST_BITSWP          30
#define rG2D_DEST_FORMAT          31
#define rG2D_DEST_WIDOFFSET       32
#define rG2D_ALPHAG               33
#define rG2D_CK_KEY               34
#define rG2D_CK_MASK              35
#define rG2D_CK_CNTL              36
#define rG2D_Y_BITSWP             37
#define rG2D_UV_BITSWP            38
#define rG2D_YUV_OFFSET           39
#define rG2D_Y_ADDR_ST            40
#define rG2D_UV_ADDR_ST           41
#define rG2D_DEST_ADDR_ST         42
#define rG2D_DRAW_CNTL            43
#define rG2D_DRAW_START           44
#define rG2D_DRAW_END             45
#define rG2D_DRAW_MASK            46
#define rG2D_DRAW_MASK_DATA       47
#define rG2D_LUT_READY            48
#define rG2D_SCL_IFSR             49
#define rG2D_SCL_OFSR             50
#define rG2D_SCL_OFOFT            51
#define rG2D_SCL_SCR              52
#define rG2D_EE_CNTL              53
#define rG2D_EE_COEF              54
#define rG2D_EE_THRE              55
#define rG2D_DIT_FRAME_SIZE       56
#define rG2D_DIT_DI_CNTL          57
#define rG2D_EE_MAT0              58
#define rG2D_EE_MAT1              59
#define rG2D_EE_MAT2              60
#define rG2D_EE_MAT3              61
#define rG2D_EE_MAT4              62
#define rG2D_SCL_HSCR0_0          63
#define rG2D_SCL_HSCR0_1          64
#define rG2D_SCL_HSCR0_2          65
#define rG2D_SCL_HSCR0_3          66
#define rG2D_SCL_HSCR0_4          67
#define rG2D_SCL_HSCR0_5          68
#define rG2D_SCL_HSCR0_6          69
#define rG2D_SCL_HSCR0_7          70
#define rG2D_SCL_HSCR0_8          71
#define rG2D_SCL_HSCR0_9          72
#define rG2D_SCL_HSCR0_10         73
#define rG2D_SCL_HSCR0_11         74
#define rG2D_SCL_HSCR0_12         75
#define rG2D_SCL_HSCR0_13         76
#define rG2D_SCL_HSCR0_14         77
#define rG2D_SCL_HSCR0_15         78
#define rG2D_SCL_HSCR0_16         79
#define rG2D_SCL_HSCR0_17         80
#define rG2D_SCL_HSCR0_18         81
#define rG2D_SCL_HSCR0_19         82
#define rG2D_SCL_HSCR0_20         83
#define rG2D_SCL_HSCR0_21         84
#define rG2D_SCL_HSCR0_22         85
#define rG2D_SCL_HSCR0_23         86
#define rG2D_SCL_HSCR0_24         87
#define rG2D_SCL_HSCR0_25         88
#define rG2D_SCL_HSCR0_26         89
#define rG2D_SCL_HSCR0_27         90
#define rG2D_SCL_HSCR0_28         91
#define rG2D_SCL_HSCR0_29         92
#define rG2D_SCL_HSCR0_30         93
#define rG2D_SCL_HSCR0_31         94
#define rG2D_SCL_HSCR0_32         95
#define rG2D_SCL_HSCR0_33         96
#define rG2D_SCL_HSCR0_34         97
#define rG2D_SCL_HSCR0_35         98
#define rG2D_SCL_HSCR0_36         99
#define rG2D_SCL_HSCR0_37         100
#define rG2D_SCL_HSCR0_38         101
#define rG2D_SCL_HSCR0_39         102
#define rG2D_SCL_HSCR0_40         103
#define rG2D_SCL_HSCR0_41         104
#define rG2D_SCL_HSCR0_42         105
#define rG2D_SCL_HSCR0_43         106
#define rG2D_SCL_HSCR0_44         107
#define rG2D_SCL_HSCR0_45         108
#define rG2D_SCL_HSCR0_46         109
#define rG2D_SCL_HSCR0_47         110
#define rG2D_SCL_HSCR0_48         111
#define rG2D_SCL_HSCR0_49         112
#define rG2D_SCL_HSCR0_50         113
#define rG2D_SCL_HSCR0_51         114
#define rG2D_SCL_HSCR0_52         115
#define rG2D_SCL_HSCR0_53         116
#define rG2D_SCL_HSCR0_54         117
#define rG2D_SCL_HSCR0_55         118
#define rG2D_SCL_HSCR0_56         119
#define rG2D_SCL_HSCR0_57         120
#define rG2D_SCL_HSCR0_58         121
#define rG2D_SCL_HSCR0_59         122
#define rG2D_SCL_HSCR0_60         123
#define rG2D_SCL_HSCR0_61         124
#define rG2D_SCL_HSCR0_62         125
#define rG2D_SCL_HSCR0_63         126
#define rG2D_SCL_HSCR1_0          127
#define rG2D_SCL_HSCR1_1          128
#define rG2D_SCL_HSCR1_2          129
#define rG2D_SCL_HSCR1_3          130
#define rG2D_SCL_HSCR1_4          131
#define rG2D_SCL_HSCR1_5          132
#define rG2D_SCL_HSCR1_6          133
#define rG2D_SCL_HSCR1_7          134
#define rG2D_SCL_HSCR1_8          135
#define rG2D_SCL_HSCR1_9          136
#define rG2D_SCL_HSCR1_10         137
#define rG2D_SCL_HSCR1_11         138
#define rG2D_SCL_HSCR1_12         139
#define rG2D_SCL_HSCR1_13         140
#define rG2D_SCL_HSCR1_14         141
#define rG2D_SCL_HSCR1_15         142
#define rG2D_SCL_HSCR1_16         143
#define rG2D_SCL_HSCR1_17         144
#define rG2D_SCL_HSCR1_18         145
#define rG2D_SCL_HSCR1_19         146
#define rG2D_SCL_HSCR1_20         147
#define rG2D_SCL_HSCR1_21         148
#define rG2D_SCL_HSCR1_22         149
#define rG2D_SCL_HSCR1_23         150
#define rG2D_SCL_HSCR1_24         151
#define rG2D_SCL_HSCR1_25         152
#define rG2D_SCL_HSCR1_26         153
#define rG2D_SCL_HSCR1_27         154
#define rG2D_SCL_HSCR1_28         155
#define rG2D_SCL_HSCR1_29         156
#define rG2D_SCL_HSCR1_30         157
#define rG2D_SCL_HSCR1_31         158
#define rG2D_SCL_HSCR1_32         159
#define rG2D_SCL_HSCR1_33         160
#define rG2D_SCL_HSCR1_34         161
#define rG2D_SCL_HSCR1_35         162
#define rG2D_SCL_HSCR1_36         163
#define rG2D_SCL_HSCR1_37         164
#define rG2D_SCL_HSCR1_38         165
#define rG2D_SCL_HSCR1_39         166
#define rG2D_SCL_HSCR1_40         167
#define rG2D_SCL_HSCR1_41         168
#define rG2D_SCL_HSCR1_42         169
#define rG2D_SCL_HSCR1_43         170
#define rG2D_SCL_HSCR1_44         171
#define rG2D_SCL_HSCR1_45         172
#define rG2D_SCL_HSCR1_46         173
#define rG2D_SCL_HSCR1_47         174
#define rG2D_SCL_HSCR1_48         175
#define rG2D_SCL_HSCR1_49         176
#define rG2D_SCL_HSCR1_50         177
#define rG2D_SCL_HSCR1_51         178
#define rG2D_SCL_HSCR1_52         179
#define rG2D_SCL_HSCR1_53         180
#define rG2D_SCL_HSCR1_54         181
#define rG2D_SCL_HSCR1_55         182
#define rG2D_SCL_HSCR1_56         183
#define rG2D_SCL_HSCR1_57         184
#define rG2D_SCL_HSCR1_58         185
#define rG2D_SCL_HSCR1_59         186
#define rG2D_SCL_HSCR1_60         187
#define rG2D_SCL_HSCR1_61         188
#define rG2D_SCL_HSCR1_62         189
#define rG2D_SCL_HSCR1_63         190
#define rG2D_SCL_HSCR2_0          191
#define rG2D_SCL_HSCR2_1          192
#define rG2D_SCL_HSCR2_2          193
#define rG2D_SCL_HSCR2_3          194
#define rG2D_SCL_HSCR2_4          195
#define rG2D_SCL_HSCR2_5          196
#define rG2D_SCL_HSCR2_6          197
#define rG2D_SCL_HSCR2_7          198
#define rG2D_SCL_HSCR2_8          199
#define rG2D_SCL_HSCR2_9          200
#define rG2D_SCL_HSCR2_10         201
#define rG2D_SCL_HSCR2_11         202
#define rG2D_SCL_HSCR2_12         203
#define rG2D_SCL_HSCR2_13         204
#define rG2D_SCL_HSCR2_14         205
#define rG2D_SCL_HSCR2_15         206
#define rG2D_SCL_HSCR2_16         207
#define rG2D_SCL_HSCR2_17         208
#define rG2D_SCL_HSCR2_18         209
#define rG2D_SCL_HSCR2_19         210
#define rG2D_SCL_HSCR2_20         211
#define rG2D_SCL_HSCR2_21         212
#define rG2D_SCL_HSCR2_22         213
#define rG2D_SCL_HSCR2_23         214
#define rG2D_SCL_HSCR2_24         215
#define rG2D_SCL_HSCR2_25         216
#define rG2D_SCL_HSCR2_26         217
#define rG2D_SCL_HSCR2_27         218
#define rG2D_SCL_HSCR2_28         219
#define rG2D_SCL_HSCR2_29         220
#define rG2D_SCL_HSCR2_30         221
#define rG2D_SCL_HSCR2_31         222
#define rG2D_SCL_HSCR2_32         223
#define rG2D_SCL_HSCR2_33         224
#define rG2D_SCL_HSCR2_34         225
#define rG2D_SCL_HSCR2_35         226
#define rG2D_SCL_HSCR2_36         227
#define rG2D_SCL_HSCR2_37         228
#define rG2D_SCL_HSCR2_38         229
#define rG2D_SCL_HSCR2_39         230
#define rG2D_SCL_HSCR2_40         231
#define rG2D_SCL_HSCR2_41         232
#define rG2D_SCL_HSCR2_42         233
#define rG2D_SCL_HSCR2_43         234
#define rG2D_SCL_HSCR2_44         235
#define rG2D_SCL_HSCR2_45         236
#define rG2D_SCL_HSCR2_46         237
#define rG2D_SCL_HSCR2_47         238
#define rG2D_SCL_HSCR2_48         239
#define rG2D_SCL_HSCR2_49         240
#define rG2D_SCL_HSCR2_50         241
#define rG2D_SCL_HSCR2_51         242
#define rG2D_SCL_HSCR2_52         243
#define rG2D_SCL_HSCR2_53         244
#define rG2D_SCL_HSCR2_54         245
#define rG2D_SCL_HSCR2_55         246
#define rG2D_SCL_HSCR2_56         247
#define rG2D_SCL_HSCR2_57         248
#define rG2D_SCL_HSCR2_58         249
#define rG2D_SCL_HSCR2_59         250
#define rG2D_SCL_HSCR2_60         251
#define rG2D_SCL_HSCR2_61         252
#define rG2D_SCL_HSCR2_62         253
#define rG2D_SCL_HSCR2_63         254
#define rG2D_SCL_HSCR3_0          255
#define rG2D_SCL_HSCR3_1          256
#define rG2D_SCL_HSCR3_2          257
#define rG2D_SCL_HSCR3_3          258
#define rG2D_SCL_HSCR3_4          259
#define rG2D_SCL_HSCR3_5          260
#define rG2D_SCL_HSCR3_6          261
#define rG2D_SCL_HSCR3_7          262
#define rG2D_SCL_HSCR3_8          263
#define rG2D_SCL_HSCR3_9          264
#define rG2D_SCL_HSCR3_10         265
#define rG2D_SCL_HSCR3_11         266
#define rG2D_SCL_HSCR3_12         267
#define rG2D_SCL_HSCR3_13         268
#define rG2D_SCL_HSCR3_14         269
#define rG2D_SCL_HSCR3_15         270
#define rG2D_SCL_HSCR3_16         271
#define rG2D_SCL_HSCR3_17         272
#define rG2D_SCL_HSCR3_18         273
#define rG2D_SCL_HSCR3_19         274
#define rG2D_SCL_HSCR3_20         275
#define rG2D_SCL_HSCR3_21         276
#define rG2D_SCL_HSCR3_22         277
#define rG2D_SCL_HSCR3_23         278
#define rG2D_SCL_HSCR3_24         279
#define rG2D_SCL_HSCR3_25         280
#define rG2D_SCL_HSCR3_26         281
#define rG2D_SCL_HSCR3_27         282
#define rG2D_SCL_HSCR3_28         283
#define rG2D_SCL_HSCR3_29         284
#define rG2D_SCL_HSCR3_30         285
#define rG2D_SCL_HSCR3_31         286
#define rG2D_SCL_HSCR3_32         287
#define rG2D_SCL_HSCR3_33         288
#define rG2D_SCL_HSCR3_34         289
#define rG2D_SCL_HSCR3_35         290
#define rG2D_SCL_HSCR3_36         291
#define rG2D_SCL_HSCR3_37         292
#define rG2D_SCL_HSCR3_38         293
#define rG2D_SCL_HSCR3_39         294
#define rG2D_SCL_HSCR3_40         295
#define rG2D_SCL_HSCR3_41         296
#define rG2D_SCL_HSCR3_42         297
#define rG2D_SCL_HSCR3_43         298
#define rG2D_SCL_HSCR3_44         299
#define rG2D_SCL_HSCR3_45         300
#define rG2D_SCL_HSCR3_46         301
#define rG2D_SCL_HSCR3_47         302
#define rG2D_SCL_HSCR3_48         303
#define rG2D_SCL_HSCR3_49         304
#define rG2D_SCL_HSCR3_50         305
#define rG2D_SCL_HSCR3_51         306
#define rG2D_SCL_HSCR3_52         307
#define rG2D_SCL_HSCR3_53         308
#define rG2D_SCL_HSCR3_54         309
#define rG2D_SCL_HSCR3_55         310
#define rG2D_SCL_HSCR3_56         311
#define rG2D_SCL_HSCR3_57         312
#define rG2D_SCL_HSCR3_58         313
#define rG2D_SCL_HSCR3_59         314
#define rG2D_SCL_HSCR3_60         315
#define rG2D_SCL_HSCR3_61         316
#define rG2D_SCL_HSCR3_62         317
#define rG2D_SCL_HSCR3_63         318
#define rG2D_SCL_VSCR0_0          319
#define rG2D_SCL_VSCR0_1          320
#define rG2D_SCL_VSCR0_2          321
#define rG2D_SCL_VSCR0_3          322
#define rG2D_SCL_VSCR0_4          323
#define rG2D_SCL_VSCR0_5          324
#define rG2D_SCL_VSCR0_6          325
#define rG2D_SCL_VSCR0_7          326
#define rG2D_SCL_VSCR0_8          327
#define rG2D_SCL_VSCR0_9          328
#define rG2D_SCL_VSCR0_10         329
#define rG2D_SCL_VSCR0_11         330
#define rG2D_SCL_VSCR0_12         331
#define rG2D_SCL_VSCR0_13         332
#define rG2D_SCL_VSCR0_14         333
#define rG2D_SCL_VSCR0_15         334
#define rG2D_SCL_VSCR0_16         335
#define rG2D_SCL_VSCR0_17         336
#define rG2D_SCL_VSCR0_18         337
#define rG2D_SCL_VSCR0_19         338
#define rG2D_SCL_VSCR0_20         339
#define rG2D_SCL_VSCR0_21         340
#define rG2D_SCL_VSCR0_22         341
#define rG2D_SCL_VSCR0_23         342
#define rG2D_SCL_VSCR0_24         343
#define rG2D_SCL_VSCR0_25         344
#define rG2D_SCL_VSCR0_26         345
#define rG2D_SCL_VSCR0_27         346
#define rG2D_SCL_VSCR0_28         347
#define rG2D_SCL_VSCR0_29         348
#define rG2D_SCL_VSCR0_30         349
#define rG2D_SCL_VSCR0_31         350
#define rG2D_SCL_VSCR0_32         351
#define rG2D_SCL_VSCR0_33         352
#define rG2D_SCL_VSCR0_34         353
#define rG2D_SCL_VSCR0_35         354
#define rG2D_SCL_VSCR0_36         355
#define rG2D_SCL_VSCR0_37         356
#define rG2D_SCL_VSCR0_38         357
#define rG2D_SCL_VSCR0_39         358
#define rG2D_SCL_VSCR0_40         359
#define rG2D_SCL_VSCR0_41         360
#define rG2D_SCL_VSCR0_42         361
#define rG2D_SCL_VSCR0_43         362
#define rG2D_SCL_VSCR0_44         363
#define rG2D_SCL_VSCR0_45         364
#define rG2D_SCL_VSCR0_46         365
#define rG2D_SCL_VSCR0_47         366
#define rG2D_SCL_VSCR0_48         367
#define rG2D_SCL_VSCR0_49         368
#define rG2D_SCL_VSCR0_50         369
#define rG2D_SCL_VSCR0_51         370
#define rG2D_SCL_VSCR0_52         371
#define rG2D_SCL_VSCR0_53         372
#define rG2D_SCL_VSCR0_54         373
#define rG2D_SCL_VSCR0_55         374
#define rG2D_SCL_VSCR0_56         375
#define rG2D_SCL_VSCR0_57         376
#define rG2D_SCL_VSCR0_58         377
#define rG2D_SCL_VSCR0_59         378
#define rG2D_SCL_VSCR0_60         379
#define rG2D_SCL_VSCR0_61         380
#define rG2D_SCL_VSCR0_62         381
#define rG2D_SCL_VSCR0_63         382
#define rG2D_SCL_VSCR1_0          383
#define rG2D_SCL_VSCR1_1          384
#define rG2D_SCL_VSCR1_2          385
#define rG2D_SCL_VSCR1_3          386
#define rG2D_SCL_VSCR1_4          387
#define rG2D_SCL_VSCR1_5          388
#define rG2D_SCL_VSCR1_6          389
#define rG2D_SCL_VSCR1_7          390
#define rG2D_SCL_VSCR1_8          391
#define rG2D_SCL_VSCR1_9          392
#define rG2D_SCL_VSCR1_10         393
#define rG2D_SCL_VSCR1_11         394
#define rG2D_SCL_VSCR1_12         395
#define rG2D_SCL_VSCR1_13         396
#define rG2D_SCL_VSCR1_14         397
#define rG2D_SCL_VSCR1_15         398
#define rG2D_SCL_VSCR1_16         399
#define rG2D_SCL_VSCR1_17         400
#define rG2D_SCL_VSCR1_18         401
#define rG2D_SCL_VSCR1_19         402
#define rG2D_SCL_VSCR1_20         403
#define rG2D_SCL_VSCR1_21         404
#define rG2D_SCL_VSCR1_22         405
#define rG2D_SCL_VSCR1_23         406
#define rG2D_SCL_VSCR1_24         407
#define rG2D_SCL_VSCR1_25         408
#define rG2D_SCL_VSCR1_26         409
#define rG2D_SCL_VSCR1_27         410
#define rG2D_SCL_VSCR1_28         411
#define rG2D_SCL_VSCR1_29         412
#define rG2D_SCL_VSCR1_30         413
#define rG2D_SCL_VSCR1_31         414
#define rG2D_SCL_VSCR1_32         415
#define rG2D_SCL_VSCR1_33         416
#define rG2D_SCL_VSCR1_34         417
#define rG2D_SCL_VSCR1_35         418
#define rG2D_SCL_VSCR1_36         419
#define rG2D_SCL_VSCR1_37         420
#define rG2D_SCL_VSCR1_38         421
#define rG2D_SCL_VSCR1_39         422
#define rG2D_SCL_VSCR1_40         423
#define rG2D_SCL_VSCR1_41         424
#define rG2D_SCL_VSCR1_42         425
#define rG2D_SCL_VSCR1_43         426
#define rG2D_SCL_VSCR1_44         427
#define rG2D_SCL_VSCR1_45         428
#define rG2D_SCL_VSCR1_46         429
#define rG2D_SCL_VSCR1_47         430
#define rG2D_SCL_VSCR1_48         431
#define rG2D_SCL_VSCR1_49         432
#define rG2D_SCL_VSCR1_50         433
#define rG2D_SCL_VSCR1_51         434
#define rG2D_SCL_VSCR1_52         435
#define rG2D_SCL_VSCR1_53         436
#define rG2D_SCL_VSCR1_54         437
#define rG2D_SCL_VSCR1_55         438
#define rG2D_SCL_VSCR1_56         439
#define rG2D_SCL_VSCR1_57         440
#define rG2D_SCL_VSCR1_58         441
#define rG2D_SCL_VSCR1_59         442
#define rG2D_SCL_VSCR1_60         443
#define rG2D_SCL_VSCR1_61         444
#define rG2D_SCL_VSCR1_62         445
#define rG2D_SCL_VSCR1_63         446
#define rG2D_DIT_DM_COEF_0003     447
#define rG2D_DIT_DM_COEF_0407     448
#define rG2D_DIT_DM_COEF_1013     449
#define rG2D_DIT_DM_COEF_1417     450
#define rG2D_DIT_DM_COEF_2023     451
#define rG2D_DIT_DM_COEF_2427     452
#define rG2D_DIT_DM_COEF_3033     453
#define rG2D_DIT_DM_COEF_3437     454
#define rG2D_DIT_DE_COEF          455

#define	MEMREGNUM				(rG2D_ALU_CNTL+1)
#define	KEYREGNUM				(rG2D_DIT_DI_CNTL+1)
#define	G2DREGNUM				(rG2D_DIT_DE_COEF+1)



/*************************************************
*  define g2d register id(bits field)            *
**************************************************/
#define G2D_OUT_BITSWP_FORMAT_BPP       0 
#define G2D_OUT_BITSWP_BITADR           1 
#define G2D_OUT_BITSWP_HFWSWP           2 
#define G2D_OUT_BITSWP_BYTSWP           3 
#define G2D_OUT_BITSWP_HFBSWP           4 
#define G2D_OUT_BITSWP_BIT2SWP          5 
#define G2D_OUT_BITSWP_BITSWP           6 
#define G2D_IF_XYST_YSTART              7 
#define G2D_IF_XYST_XSTART              8 
#define G2D_IF_WIDTH_1                  9 
#define G2D_IF_WIDTH                    10 
#define G2D_IF_HEIGHT_1                 11 
#define G2D_IF_HEIGHT                   12 
#define G2D_IF_FULL_WIDTH_1             13 
#define G2D_IF_FULL_WIDTH               14 
#define G2D_IF_ADDR_ST                  15 
#define G2D_MEM_ADDR_ST                 16 
#define G2D_MEM_LENGTH                  17
#define G2D_MEM_SETDATA32               18
#define G2D_MEM_SETDATA64               19
#define G2D_MEM_CODE_LEN_SET_LEN        20
#define G2D_MEM_CODE_LEN_CODE           21
#define G2D_ALU_CNTL_DRAW_EN            22 
#define G2D_ALU_CNTL_MEM_SEL            23 
#define G2D_ALU_CNTL_ROT_CODE           24 
#define G2D_ALU_CNTL_ROT_SEL            25 
#define G2D_SRC_XYST_SOFTRST            26
#define G2D_SRC_XYST_YSTART             27
#define G2D_SRC_XYST_XSTART             28
#define G2D_SRC_SIZE_HEIGHT             29
#define G2D_SRC_SIZE_WIDTH              30
#define G2D_SRC_SIZE_1_HEIGHT           31
#define G2D_SRC_SIZE_1_WIDTH            32
#define G2D_SRC_FULL_WIDTH_1            33
#define G2D_SRC_FULL_WIDTH              34
#define G2D_DEST_XYST_YSTART            35
#define G2D_DEST_XYST_XSTART            36
#define G2D_DEST_SIZE_HEIGHT            37
#define G2D_DEST_SIZE_WIDTH             38
#define G2D_DEST_SIZE_1_HEIGHT          39
#define G2D_DEST_SIZE_1_WIDTH           40
#define G2D_DEST_FULL_WIDTH_1           41
#define G2D_DEST_FULL_WIDTH             42
#define G2D_PAT_SIZE_HEIGHT             43
#define G2D_PAT_SIZE_WIDTH              44
#define G2D_SRC_BITSWP_BITADR           45
#define G2D_SRC_BITSWP_HFWSWP           46
#define G2D_SRC_BITSWP_BYTSWP           47
#define G2D_SRC_BITSWP_HFBSWP           48
#define G2D_SRC_BITSWP_BIT2SWP          49
#define G2D_SRC_BITSWP_BITSWP           50
#define G2D_SRC_FORMAT_PAL              51
#define G2D_SRC_FORMAT_BPP              52
#define G2D_SRC_WIDOFFSET               53
#define G2D_SRC_RGB_CSC_BYPASSS         54 
#define G2D_SRC_RGB_YUVFMT              55 
#define G2D_SRC_RGB_BRREV               56 
#define G2D_SRC_COEF1112_COEF12         57 
#define G2D_SRC_COEF1112_COEF11         58 
#define G2D_SRC_COEF1321_COEF21         59 
#define G2D_SRC_COEF1321_COEF13         60 
#define G2D_SRC_COEF2322_COEF23         61 
#define G2D_SRC_COEF2322_COEF22         62 
#define G2D_SRC_COEF3132_COEF32         63 
#define G2D_SRC_COEF3132_COEF31         64 
#define G2D_SRC_COEF33_OFT_OFTA         65 
#define G2D_SRC_COEF33_OFT_OFTB         66 
#define G2D_SRC_COEF33_OFT_COEF33       67 
#define G2D_DEST_BITSWP_BITADR          68 
#define G2D_DEST_BITSWP_HFWSWP          69 
#define G2D_DEST_BITSWP_BYTSWP          70 
#define G2D_DEST_BITSWP_HFBSWP          71 
#define G2D_DEST_BITSWP_BIT2SWP         72 
#define G2D_DEST_BITSWP_BITSWP          73 
#define G2D_DEST_FORMAT_PAL             74 
#define G2D_DEST_FORMAT_BPP             75 
#define G2D_DEST_WIDOFFSET_BRREV        76 
#define G2D_DEST_WIDOFFSET_OFFSET       77 
#define G2D_ALPHAG_GB                   78 
#define G2D_ALPHAG_GA                   79 
#define G2D_ALPHAG_GSEL                 80 
#define G2D_CK_KEY                      81 
#define G2D_CK_MASK                     82 
#define G2D_CK_CNTL_ROP_CODE            83 
#define G2D_CK_CNTL_AR_CODE             84 
#define G2D_CK_CNTL_CK_DIR              85 
#define G2D_CK_CNTL_CK_ABLD_EN          86 
#define G2D_CK_CNTL_CK_EN               87 
#define G2D_Y_BITSWP_BITADR             88 
#define G2D_Y_BITSWP_HFWSWP             89 
#define G2D_Y_BITSWP_BYTSWP             90 
#define G2D_Y_BITSWP_HFBSWP             91 
#define G2D_Y_BITSWP_BIT2SWP            92 
#define G2D_Y_BITSWP_BITSWP             93 
#define G2D_UV_BITSWP_BITADR            94 
#define G2D_UV_BITSWP_HFWSWP            95 
#define G2D_UV_BITSWP_BYTSWP            96 
#define G2D_UV_BITSWP_HFBSWP            97 
#define G2D_UV_BITSWP_BIT2SWP           98 
#define G2D_UV_BITSWP_BITSWP            99 
#define G2D_YUV_OFFSET_UV               100 
#define G2D_YUV_OFFSET_Y                101 
#define G2D_Y_ADDR_ST                   102 
#define G2D_UV_ADDR_ST                  103 
#define G2D_DEST_ADDR_ST                104
#define G2D_DRAW_CNTL_FOOT              105
#define G2D_DRAW_CNTL_WIDTH             106
#define G2D_DRAW_CNT_CODE               107
#define G2D_DRAW_START_Y                108
#define G2D_DRAW_START_X                109
#define G2D_DRAW_END_Y                  110
#define G2D_DRAW_END_X                  111
#define G2D_DRAW_MASK                   112
#define G2D_DRAW_MASK_DATA              113
#define G2D_LUT_READY_PAT               114
#define G2D_LUT_READY_DEST_MONO         115
#define G2D_LUT_READY_SRC_MONO          116
#define G2D_SCL_IFSR_IVRES              117
#define G2D_SCL_IFSR_IHRES              118
#define G2D_SCL_OFSR_OVRES              119
#define G2D_SCL_OFSR_OHRES              120
#define G2D_SCL_OFOFT_OVOFFSET          121
#define G2D_SCL_OFOFT_OHOFFSET          122
#define G2D_SCL_SCR_VROUND              123
#define G2D_SCL_SCR_VPASSBY             124
#define G2D_SCL_SCR_VSCALING            125
#define G2D_SCL_SCR_HROUND              126
#define G2D_SCL_SCR_HPASSBY             127
#define G2D_SCL_SCR_HSCALING            128
#define G2D_EE_CNTL_FIRST               129
#define G2D_EE_CNTL_DENOISE_EN          130
#define G2D_EE_CNTL_ROUND               131
#define G2D_EE_CNTL_BYPASS              132
#define G2D_EE_COEF_ERR                 133
#define G2D_EE_COEF_COEFA               134
#define G2D_EE_COEF_COEFW               135
#define G2D_EE_THRE_H                   136
#define G2D_EE_THRE_V                   137
#define G2D_EE_THRE_D0                  138
#define G2D_EE_THRE_D1                  139
#define G2D_DIT_FRAME_SIZE_VRES         140
#define G2D_DIT_FRAME_SIZE_HRES         141
#define G2D_DIT_DI_CNTL_RNB             142
#define G2D_DIT_DI_CNTL_GNB             143
#define G2D_DIT_DI_CNTL_BNB             144
#define G2D_DIT_DI_CNTL_BYPASS          145
#define G2D_DIT_DI_CNTL_TEMPO           146
#define G2D_EE_MAT0_HM00                147
#define G2D_EE_MAT0_HM01                148
#define G2D_EE_MAT0_HM02                149
#define G2D_EE_MAT0_HM10                150
#define G2D_EE_MAT0_HM11                151
#define G2D_EE_MAT0_HM12                152
#define G2D_EE_MAT0_HM20                153
#define G2D_EE_MAT0_HM21                154
#define G2D_EE_MAT0_HM22                155
#define G2D_EE_MAT1_VM00                156
#define G2D_EE_MAT1_VM01                157
#define G2D_EE_MAT1_VM02                158
#define G2D_EE_MAT1_VM10                159
#define G2D_EE_MAT1_VM11                160
#define G2D_EE_MAT1_VM12                161
#define G2D_EE_MAT1_VM20                162
#define G2D_EE_MAT1_VM21                163
#define G2D_EE_MAT1_VM22                164
#define G2D_EE_MAT2_D0M00               165
#define G2D_EE_MAT2_D0M01               166
#define G2D_EE_MAT2_D0M02               167
#define G2D_EE_MAT2_D0M10               168
#define G2D_EE_MAT2_D0M11               169
#define G2D_EE_MAT2_D0M12               170
#define G2D_EE_MAT2_D0M20               171
#define G2D_EE_MAT2_D0M21               172
#define G2D_EE_MAT2_D0M22               173
#define G2D_EE_MAT3_D1M00               174
#define G2D_EE_MAT3_D1M01               175
#define G2D_EE_MAT3_D1M02               176
#define G2D_EE_MAT3_D1M10               177
#define G2D_EE_MAT3_D1M11               178
#define G2D_EE_MAT3_D1M12               179
#define G2D_EE_MAT3_D1M20               180
#define G2D_EE_MAT3_D1M21               181
#define G2D_EE_MAT3_D1M22               182
#define G2D_EE_MAT4_GAM11_HIGH          183
#define G2D_EE_MAT4_GAM00               184
#define G2D_EE_MAT4_GAM01               185
#define G2D_EE_MAT4_GAM02               186
#define G2D_EE_MAT4_GAM10               187
#define G2D_EE_MAT4_GAM11_LOW           188
#define G2D_EE_MAT4_GAM12               189
#define G2D_EE_MAT4_GAM20               190
#define G2D_EE_MAT4_GAM21               191
#define G2D_EE_MAT4_GAM22               192
#define G2D_SCL_HSCR0_0                 193
#define G2D_SCL_HSCR0_1                 194
#define G2D_SCL_HSCR0_2                 195
#define G2D_SCL_HSCR0_3                 196
#define G2D_SCL_HSCR0_4                 197
#define G2D_SCL_HSCR0_5                 198
#define G2D_SCL_HSCR0_6                 199
#define G2D_SCL_HSCR0_7                 200
#define G2D_SCL_HSCR0_8                 201
#define G2D_SCL_HSCR0_9                 202
#define G2D_SCL_HSCR0_10                203
#define G2D_SCL_HSCR0_11                204
#define G2D_SCL_HSCR0_12                205
#define G2D_SCL_HSCR0_13                206
#define G2D_SCL_HSCR0_14                207
#define G2D_SCL_HSCR0_15                208
#define G2D_SCL_HSCR0_16                209
#define G2D_SCL_HSCR0_17                210
#define G2D_SCL_HSCR0_18                211
#define G2D_SCL_HSCR0_19                212
#define G2D_SCL_HSCR0_20                213
#define G2D_SCL_HSCR0_21                214
#define G2D_SCL_HSCR0_22                215
#define G2D_SCL_HSCR0_23                216
#define G2D_SCL_HSCR0_24                217
#define G2D_SCL_HSCR0_25                218
#define G2D_SCL_HSCR0_26                219
#define G2D_SCL_HSCR0_27                220
#define G2D_SCL_HSCR0_28                221
#define G2D_SCL_HSCR0_29                222
#define G2D_SCL_HSCR0_30                223
#define G2D_SCL_HSCR0_31                224
#define G2D_SCL_HSCR0_32                225
#define G2D_SCL_HSCR0_33                226
#define G2D_SCL_HSCR0_34                227
#define G2D_SCL_HSCR0_35                228
#define G2D_SCL_HSCR0_36                229
#define G2D_SCL_HSCR0_37                230
#define G2D_SCL_HSCR0_38                231
#define G2D_SCL_HSCR0_39                232
#define G2D_SCL_HSCR0_40                233
#define G2D_SCL_HSCR0_41                234
#define G2D_SCL_HSCR0_42                235
#define G2D_SCL_HSCR0_43                236
#define G2D_SCL_HSCR0_44                237
#define G2D_SCL_HSCR0_45                238
#define G2D_SCL_HSCR0_46                239
#define G2D_SCL_HSCR0_47                240
#define G2D_SCL_HSCR0_48                241
#define G2D_SCL_HSCR0_49                242
#define G2D_SCL_HSCR0_50                243
#define G2D_SCL_HSCR0_51                244
#define G2D_SCL_HSCR0_52                245
#define G2D_SCL_HSCR0_53                246
#define G2D_SCL_HSCR0_54                247
#define G2D_SCL_HSCR0_55                248
#define G2D_SCL_HSCR0_56                249
#define G2D_SCL_HSCR0_57                250
#define G2D_SCL_HSCR0_58                251
#define G2D_SCL_HSCR0_59                252
#define G2D_SCL_HSCR0_60                253
#define G2D_SCL_HSCR0_61                254
#define G2D_SCL_HSCR0_62                255
#define G2D_SCL_HSCR0_63                256
#define G2D_SCL_HSCR1_0                 257
#define G2D_SCL_HSCR1_1                 258
#define G2D_SCL_HSCR1_2                 259
#define G2D_SCL_HSCR1_3                 260
#define G2D_SCL_HSCR1_4                 261
#define G2D_SCL_HSCR1_5                 262
#define G2D_SCL_HSCR1_6                 263
#define G2D_SCL_HSCR1_7                 264
#define G2D_SCL_HSCR1_8                 265
#define G2D_SCL_HSCR1_9                 266
#define G2D_SCL_HSCR1_10                267
#define G2D_SCL_HSCR1_11                268
#define G2D_SCL_HSCR1_12                269
#define G2D_SCL_HSCR1_13                270
#define G2D_SCL_HSCR1_14                271
#define G2D_SCL_HSCR1_15                272
#define G2D_SCL_HSCR1_16                273
#define G2D_SCL_HSCR1_17                274
#define G2D_SCL_HSCR1_18                275
#define G2D_SCL_HSCR1_19                276
#define G2D_SCL_HSCR1_20                277
#define G2D_SCL_HSCR1_21                278
#define G2D_SCL_HSCR1_22                279
#define G2D_SCL_HSCR1_23                280
#define G2D_SCL_HSCR1_24                281
#define G2D_SCL_HSCR1_25                282
#define G2D_SCL_HSCR1_26                283
#define G2D_SCL_HSCR1_27                284
#define G2D_SCL_HSCR1_28                285
#define G2D_SCL_HSCR1_29                286
#define G2D_SCL_HSCR1_30                287
#define G2D_SCL_HSCR1_31                288
#define G2D_SCL_HSCR1_32                289
#define G2D_SCL_HSCR1_33                290
#define G2D_SCL_HSCR1_34                291
#define G2D_SCL_HSCR1_35                292
#define G2D_SCL_HSCR1_36                293
#define G2D_SCL_HSCR1_37                294
#define G2D_SCL_HSCR1_38                295
#define G2D_SCL_HSCR1_39                296
#define G2D_SCL_HSCR1_40                297
#define G2D_SCL_HSCR1_41                298
#define G2D_SCL_HSCR1_42                299
#define G2D_SCL_HSCR1_43                300
#define G2D_SCL_HSCR1_44                301
#define G2D_SCL_HSCR1_45                302
#define G2D_SCL_HSCR1_46                303
#define G2D_SCL_HSCR1_47                304
#define G2D_SCL_HSCR1_48                305
#define G2D_SCL_HSCR1_49                306
#define G2D_SCL_HSCR1_50                307
#define G2D_SCL_HSCR1_51                308
#define G2D_SCL_HSCR1_52                309
#define G2D_SCL_HSCR1_53                310
#define G2D_SCL_HSCR1_54                311
#define G2D_SCL_HSCR1_55                312
#define G2D_SCL_HSCR1_56                313
#define G2D_SCL_HSCR1_57                314
#define G2D_SCL_HSCR1_58                315
#define G2D_SCL_HSCR1_59                316
#define G2D_SCL_HSCR1_60                317
#define G2D_SCL_HSCR1_61                318
#define G2D_SCL_HSCR1_62                319
#define G2D_SCL_HSCR1_63                320
#define G2D_SCL_HSCR2_0                 321
#define G2D_SCL_HSCR2_1                 322
#define G2D_SCL_HSCR2_2                 323
#define G2D_SCL_HSCR2_3                 324
#define G2D_SCL_HSCR2_4                 325
#define G2D_SCL_HSCR2_5                 326
#define G2D_SCL_HSCR2_6                 327
#define G2D_SCL_HSCR2_7                 328
#define G2D_SCL_HSCR2_8                 329
#define G2D_SCL_HSCR2_9                 330
#define G2D_SCL_HSCR2_10                331
#define G2D_SCL_HSCR2_11                332
#define G2D_SCL_HSCR2_12                333
#define G2D_SCL_HSCR2_13                334
#define G2D_SCL_HSCR2_14                335
#define G2D_SCL_HSCR2_15                336
#define G2D_SCL_HSCR2_16                337
#define G2D_SCL_HSCR2_17                338
#define G2D_SCL_HSCR2_18                339
#define G2D_SCL_HSCR2_19                340
#define G2D_SCL_HSCR2_20                341
#define G2D_SCL_HSCR2_21                342
#define G2D_SCL_HSCR2_22                343
#define G2D_SCL_HSCR2_23                344
#define G2D_SCL_HSCR2_24                345
#define G2D_SCL_HSCR2_25                346
#define G2D_SCL_HSCR2_26                347
#define G2D_SCL_HSCR2_27                348
#define G2D_SCL_HSCR2_28                349
#define G2D_SCL_HSCR2_29                350
#define G2D_SCL_HSCR2_30                351
#define G2D_SCL_HSCR2_31                352
#define G2D_SCL_HSCR2_32                353
#define G2D_SCL_HSCR2_33                354
#define G2D_SCL_HSCR2_34                355
#define G2D_SCL_HSCR2_35                356
#define G2D_SCL_HSCR2_36                357
#define G2D_SCL_HSCR2_37                358
#define G2D_SCL_HSCR2_38                359
#define G2D_SCL_HSCR2_39                360
#define G2D_SCL_HSCR2_40                361
#define G2D_SCL_HSCR2_41                362
#define G2D_SCL_HSCR2_42                363
#define G2D_SCL_HSCR2_43                364
#define G2D_SCL_HSCR2_44                365
#define G2D_SCL_HSCR2_45                366
#define G2D_SCL_HSCR2_46                367
#define G2D_SCL_HSCR2_47                368
#define G2D_SCL_HSCR2_48                369
#define G2D_SCL_HSCR2_49                370
#define G2D_SCL_HSCR2_50                371
#define G2D_SCL_HSCR2_51                372
#define G2D_SCL_HSCR2_52                373
#define G2D_SCL_HSCR2_53                374
#define G2D_SCL_HSCR2_54                375
#define G2D_SCL_HSCR2_55                376
#define G2D_SCL_HSCR2_56                377
#define G2D_SCL_HSCR2_57                378
#define G2D_SCL_HSCR2_58                379
#define G2D_SCL_HSCR2_59                380
#define G2D_SCL_HSCR2_60                381
#define G2D_SCL_HSCR2_61                382
#define G2D_SCL_HSCR2_62                383
#define G2D_SCL_HSCR2_63                384
#define G2D_SCL_HSCR3_0                 385
#define G2D_SCL_HSCR3_1                 386
#define G2D_SCL_HSCR3_2                 387
#define G2D_SCL_HSCR3_3                 388
#define G2D_SCL_HSCR3_4                 389
#define G2D_SCL_HSCR3_5                 390
#define G2D_SCL_HSCR3_6                 391
#define G2D_SCL_HSCR3_7                 392
#define G2D_SCL_HSCR3_8                 393
#define G2D_SCL_HSCR3_9                 394
#define G2D_SCL_HSCR3_10                395
#define G2D_SCL_HSCR3_11                396
#define G2D_SCL_HSCR3_12                397
#define G2D_SCL_HSCR3_13                398
#define G2D_SCL_HSCR3_14                399
#define G2D_SCL_HSCR3_15                400
#define G2D_SCL_HSCR3_16                401
#define G2D_SCL_HSCR3_17                402
#define G2D_SCL_HSCR3_18                403
#define G2D_SCL_HSCR3_19                404
#define G2D_SCL_HSCR3_20                405
#define G2D_SCL_HSCR3_21                406
#define G2D_SCL_HSCR3_22                407
#define G2D_SCL_HSCR3_23                408
#define G2D_SCL_HSCR3_24                409
#define G2D_SCL_HSCR3_25                410
#define G2D_SCL_HSCR3_26                411
#define G2D_SCL_HSCR3_27                412
#define G2D_SCL_HSCR3_28                413
#define G2D_SCL_HSCR3_29                414
#define G2D_SCL_HSCR3_30                415
#define G2D_SCL_HSCR3_31                416
#define G2D_SCL_HSCR3_32                417
#define G2D_SCL_HSCR3_33                418
#define G2D_SCL_HSCR3_34                419
#define G2D_SCL_HSCR3_35                420
#define G2D_SCL_HSCR3_36                421
#define G2D_SCL_HSCR3_37                422
#define G2D_SCL_HSCR3_38                423
#define G2D_SCL_HSCR3_39                424
#define G2D_SCL_HSCR3_40                425
#define G2D_SCL_HSCR3_41                426
#define G2D_SCL_HSCR3_42                427
#define G2D_SCL_HSCR3_43                428
#define G2D_SCL_HSCR3_44                429
#define G2D_SCL_HSCR3_45                430
#define G2D_SCL_HSCR3_46                431
#define G2D_SCL_HSCR3_47                432
#define G2D_SCL_HSCR3_48                433
#define G2D_SCL_HSCR3_49                434
#define G2D_SCL_HSCR3_50                435
#define G2D_SCL_HSCR3_51                436
#define G2D_SCL_HSCR3_52                437
#define G2D_SCL_HSCR3_53                438
#define G2D_SCL_HSCR3_54                439
#define G2D_SCL_HSCR3_55                440
#define G2D_SCL_HSCR3_56                441
#define G2D_SCL_HSCR3_57                442
#define G2D_SCL_HSCR3_58                443
#define G2D_SCL_HSCR3_59                444
#define G2D_SCL_HSCR3_60                445
#define G2D_SCL_HSCR3_61                446
#define G2D_SCL_HSCR3_62                447
#define G2D_SCL_HSCR3_63                448
#define G2D_SCL_VSCR0_0                 449
#define G2D_SCL_VSCR0_1                 450
#define G2D_SCL_VSCR0_2                 451
#define G2D_SCL_VSCR0_3                 452
#define G2D_SCL_VSCR0_4                 453
#define G2D_SCL_VSCR0_5                 454
#define G2D_SCL_VSCR0_6                 455
#define G2D_SCL_VSCR0_7                 456
#define G2D_SCL_VSCR0_8                 457
#define G2D_SCL_VSCR0_9                 458
#define G2D_SCL_VSCR0_10                459
#define G2D_SCL_VSCR0_11                460
#define G2D_SCL_VSCR0_12                461
#define G2D_SCL_VSCR0_13                462
#define G2D_SCL_VSCR0_14                463
#define G2D_SCL_VSCR0_15                464
#define G2D_SCL_VSCR0_16                465
#define G2D_SCL_VSCR0_17                466
#define G2D_SCL_VSCR0_18                467
#define G2D_SCL_VSCR0_19                468
#define G2D_SCL_VSCR0_20                469
#define G2D_SCL_VSCR0_21                470
#define G2D_SCL_VSCR0_22                471
#define G2D_SCL_VSCR0_23                472
#define G2D_SCL_VSCR0_24                473
#define G2D_SCL_VSCR0_25                474
#define G2D_SCL_VSCR0_26                475
#define G2D_SCL_VSCR0_27                476
#define G2D_SCL_VSCR0_28                477
#define G2D_SCL_VSCR0_29                478
#define G2D_SCL_VSCR0_30                479
#define G2D_SCL_VSCR0_31                480
#define G2D_SCL_VSCR0_32                481
#define G2D_SCL_VSCR0_33                482
#define G2D_SCL_VSCR0_34                483
#define G2D_SCL_VSCR0_35                484
#define G2D_SCL_VSCR0_36                485
#define G2D_SCL_VSCR0_37                486
#define G2D_SCL_VSCR0_38                487
#define G2D_SCL_VSCR0_39                488
#define G2D_SCL_VSCR0_40                489
#define G2D_SCL_VSCR0_41                490
#define G2D_SCL_VSCR0_42                491
#define G2D_SCL_VSCR0_43                492
#define G2D_SCL_VSCR0_44                493
#define G2D_SCL_VSCR0_45                494
#define G2D_SCL_VSCR0_46                495
#define G2D_SCL_VSCR0_47                496
#define G2D_SCL_VSCR0_48                497
#define G2D_SCL_VSCR0_49                498
#define G2D_SCL_VSCR0_50                499
#define G2D_SCL_VSCR0_51                500
#define G2D_SCL_VSCR0_52                501
#define G2D_SCL_VSCR0_53                502
#define G2D_SCL_VSCR0_54                503
#define G2D_SCL_VSCR0_55                504
#define G2D_SCL_VSCR0_56                505
#define G2D_SCL_VSCR0_57                506
#define G2D_SCL_VSCR0_58                507
#define G2D_SCL_VSCR0_59                508
#define G2D_SCL_VSCR0_60                509
#define G2D_SCL_VSCR0_61                510
#define G2D_SCL_VSCR0_62                511
#define G2D_SCL_VSCR0_63                512
#define G2D_SCL_VSCR1_0                 513
#define G2D_SCL_VSCR1_1                 514
#define G2D_SCL_VSCR1_2                 515
#define G2D_SCL_VSCR1_3                 516
#define G2D_SCL_VSCR1_4                 517
#define G2D_SCL_VSCR1_5                 518
#define G2D_SCL_VSCR1_6                 519
#define G2D_SCL_VSCR1_7                 520
#define G2D_SCL_VSCR1_8                 521
#define G2D_SCL_VSCR1_9                 522
#define G2D_SCL_VSCR1_10                523
#define G2D_SCL_VSCR1_11                524
#define G2D_SCL_VSCR1_12                525
#define G2D_SCL_VSCR1_13                526
#define G2D_SCL_VSCR1_14                527
#define G2D_SCL_VSCR1_15                528
#define G2D_SCL_VSCR1_16                529
#define G2D_SCL_VSCR1_17                530
#define G2D_SCL_VSCR1_18                531
#define G2D_SCL_VSCR1_19                532
#define G2D_SCL_VSCR1_20                533
#define G2D_SCL_VSCR1_21                534
#define G2D_SCL_VSCR1_22                535
#define G2D_SCL_VSCR1_23                536
#define G2D_SCL_VSCR1_24                537
#define G2D_SCL_VSCR1_25                538
#define G2D_SCL_VSCR1_26                539
#define G2D_SCL_VSCR1_27                540
#define G2D_SCL_VSCR1_28                541
#define G2D_SCL_VSCR1_29                542
#define G2D_SCL_VSCR1_30                543
#define G2D_SCL_VSCR1_31                544
#define G2D_SCL_VSCR1_32                545
#define G2D_SCL_VSCR1_33                546
#define G2D_SCL_VSCR1_34                547
#define G2D_SCL_VSCR1_35                548
#define G2D_SCL_VSCR1_36                549
#define G2D_SCL_VSCR1_37                550
#define G2D_SCL_VSCR1_38                551
#define G2D_SCL_VSCR1_39                552
#define G2D_SCL_VSCR1_40                553
#define G2D_SCL_VSCR1_41                554
#define G2D_SCL_VSCR1_42                555
#define G2D_SCL_VSCR1_43                556
#define G2D_SCL_VSCR1_44                557
#define G2D_SCL_VSCR1_45                558
#define G2D_SCL_VSCR1_46                559
#define G2D_SCL_VSCR1_47                560
#define G2D_SCL_VSCR1_48                561
#define G2D_SCL_VSCR1_49                562
#define G2D_SCL_VSCR1_50                563
#define G2D_SCL_VSCR1_51                564
#define G2D_SCL_VSCR1_52                565
#define G2D_SCL_VSCR1_53                566
#define G2D_SCL_VSCR1_54                567
#define G2D_SCL_VSCR1_55                568
#define G2D_SCL_VSCR1_56                569
#define G2D_SCL_VSCR1_57                570
#define G2D_SCL_VSCR1_58                571
#define G2D_SCL_VSCR1_59                572
#define G2D_SCL_VSCR1_60                573
#define G2D_SCL_VSCR1_61                574
#define G2D_SCL_VSCR1_62                575
#define G2D_SCL_VSCR1_63                576
#define G2D_DIT_DM_COEF_0003_COEF00     577
#define G2D_DIT_DM_COEF_0003_COEF01     578
#define G2D_DIT_DM_COEF_0003_COEF02     579
#define G2D_DIT_DM_COEF_0003_COEF03     580
#define G2D_DIT_DM_COEF_0407_COEF04     581
#define G2D_DIT_DM_COEF_0407_COEF05     582
#define G2D_DIT_DM_COEF_0407_COEF06     583
#define G2D_DIT_DM_COEF_0407_COEF07     584
#define G2D_DIT_DM_COEF_1013_COEF10     585
#define G2D_DIT_DM_COEF_1013_COEF11     586
#define G2D_DIT_DM_COEF_1013_COEF12     587
#define G2D_DIT_DM_COEF_1013_COEF13     588
#define G2D_DIT_DM_COEF_1417_COEF14     589
#define G2D_DIT_DM_COEF_1417_COEF15     590
#define G2D_DIT_DM_COEF_1417_COEF16     591
#define G2D_DIT_DM_COEF_1417_COEF17     592
#define G2D_DIT_DM_COEF_2023_COEF20     593
#define G2D_DIT_DM_COEF_2023_COEF21     594
#define G2D_DIT_DM_COEF_2023_COEF22     595
#define G2D_DIT_DM_COEF_2023_COEF23     596
#define G2D_DIT_DM_COEF_2427_COEF24     597
#define G2D_DIT_DM_COEF_2427_COEF25     598
#define G2D_DIT_DM_COEF_2427_COEF26     599
#define G2D_DIT_DM_COEF_2427_COEF27     600
#define G2D_DIT_DM_COEF_3033_COEF30     601
#define G2D_DIT_DM_COEF_3033_COEF31     602
#define G2D_DIT_DM_COEF_3033_COEF32     603
#define G2D_DIT_DM_COEF_3033_COEF33     604
#define G2D_DIT_DM_COEF_3437_COEF34     605
#define G2D_DIT_DM_COEF_3437_COEF35     606
#define G2D_DIT_DM_COEF_3437_COEF36     607
#define G2D_DIT_DM_COEF_3437_COEF37     608
#define G2D_DIT_DE_COEF_COEF12          609
#define G2D_DIT_DE_COEF_COEF20          610
#define G2D_DIT_DE_COEF_COEF21          611
#define G2D_DIT_DE_COEF_COEF22          612

#define	G2D_LAST_REG_ID                 G2D_DIT_DE_COEF_COEF22






static const unsigned int regOffsetTable[] = {
	
	/*rG2D_OUT_BITSWP			*/0x0054,
	/*rG2D_IF_XYST				*/0x0058,  
	/*rG2D_IF_WIDTH 				*/0x0078,  
	/*rG2D_IF_HEIGHT				*/0x007C,  
	/*rG2D_IF_FULL_WIDTH			*/0x0080,  
	/*rG2D_IF_ADDR_ST			*/0x0094,  
	/*rG2D_MEM_ADDR_ST			*/0x0098,  
	/*rG2D_MEM_LENGTH_ST		*/0x009C,  
	/*rG2D_MEM_SETDATA32		*/0x00A0,  
	/*rG2D_MEM_SETDATA64		*/0x00A4,  
	/*rG2D_MEM_CODE_LEN 		*/0x00A8,  
	/*rG2D_ALU_CNTL 				*/0x006C,  
	
	/*rG2D_SRC_XYST 				*/0x0000,  
	/*rG2D_SRC_SIZE 				*/0x0004,  
	/*rG2D_SRC_SIZE_1			*/0x0008,  
	/*rG2D_SRC_FULL_WIDTH		*/0x000C,  
	/*rG2D_DEST_XYST				*/0x0010,  
	/*rG2D_DEST_SIZE				*/0x0014,  
	/*rG2D_DEST_SIZE_1			*/0x0018,  
	/*rG2D_DEST_FULL_WIDTH		*/0x001C,  
	/*rG2D_PAT_SIZE 				*/0x0020,  
	/*rG2D_SRC_BITSWP			*/0x0024,  
	/*rG2D_SRC_FORMAT			*/0x0028,  
	/*rG2D_SRC_WIDOFFSET		*/0x002C,  
	/*rG2D_SRC_RGB				*/0x0030,  
	/*rG2D_SRC_COEF1112 		*/0x0034,  
	/*rG2D_SRC_COEF1321 		*/0x0038,	
	/*rG2D_SRC_COEF2223 		*/0x003C,  
	/*rG2D_SRC_COEF3132 		*/0x0040,  
	/*rG2D_SRC_COEF33_OFT		*/0x0044,  
	/*rG2D_DEST_BITSWP			*/0x0048,  
	/*rG2D_DEST_FORMAT			*/0x004C,  
	/*rG2D_DEST_WIDOFFSET		*/0x0050,  
	/*rG2D_ALPHAG				*/0x005C,  
	/*rG2D_CK_KEY				*/0x0060,  
	/*rG2D_CK_MASK				*/0x0064,  
	/*rG2D_CK_CNTL				*/0x0068,  
	/*rG2D_Y_BITSWP 				*/0x0070,  
	/*rG2D_UV_BITSWP				*/0x0074,  
	/*rG2D_YUV_OFFSET			*/0x0084,	
	/*rG2D_Y_ADDR_ST				*/0x0088,  
	/*rG2D_UV_ADDR_ST			*/0x008C,  
	/*rG2D_DEST_ADDR_ST 		*/0x0090,  
	/*rG2D_DRAW_CNTL			*/0x00AC,  
	/*rG2D_DRAW_START			*/0x00B0,  
	/*rG2D_DRAW_END 			*/0x00B4,  
	/*rG2D_DRAW_MASK			*/0x00B8,  
	/*rG2D_DRAW_MASK_DATA		*/0x00BC,  
	/*rG2D_LUT_READY				*/0x00C0,
	/*rG2D_SCL_IFSR 				*/0x0100,
	   
	/*rG2D_SCL_OFSR 				*/0x0104,
	   
	/*rG2D_SCL_OFOFT				*/0x0108,
	   
	/*rG2D_SCL_SCR				*/0x010C,
	/*rG2D_EE_CNTL				*/0x011C,
	/*rG2D_EE_COEF				*/0x0134,
	/*rG2D_EE_THRE				*/0x0138,
										
	/*rG2D_DIT_FRAME_SIZE		*/0x0800,
	/*rG2D_DIT_DI_CNTL			*/0x0804,
	/*rG2D_EE_MAT0				*/0x0120,
	/*rG2D_EE_MAT1				*/0x0124,
	/*rG2D_EE_MAT2				*/0x0128,
	/*rG2D_EE_MAT3				*/0x012C,
	/*rG2D_EE_MAT4				*/0x0130,
	/*rG2D_SCL_HSCR0_0			*/0x0200,
	/*rG2D_SCL_HSCR0_1			*/0x0204,
	/*rG2D_SCL_HSCR0_2			*/0x0208,
	/*rG2D_SCL_HSCR0_3			*/0x020C,
	/*rG2D_SCL_HSCR0_4			*/0x0210,
	/*rG2D_SCL_HSCR0_5			*/0x0214,
	/*rG2D_SCL_HSCR0_6			*/0x0218,
	/*rG2D_SCL_HSCR0_7			*/0x021C,
	/*rG2D_SCL_HSCR0_8			*/0x0220,
	/*rG2D_SCL_HSCR0_9			*/0x0224,
	/*rG2D_SCL_HSCR0_10 		*/0x0228,
	/*rG2D_SCL_HSCR0_11 		*/0x022C,
	/*rG2D_SCL_HSCR0_12 		*/0x0230,
	/*rG2D_SCL_HSCR0_13 		*/0x0234,
	/*rG2D_SCL_HSCR0_14 		*/0x0238,
	/*rG2D_SCL_HSCR0_15 		*/0x023C,
	/*rG2D_SCL_HSCR0_16 		*/0x0240,
	/*rG2D_SCL_HSCR0_17 		*/0x0244,
	/*rG2D_SCL_HSCR0_18 		*/0x0248,
	/*rG2D_SCL_HSCR0_19 		*/0x024C,
	/*rG2D_SCL_HSCR0_20 		*/0x0250,
	/*rG2D_SCL_HSCR0_21 		*/0x0254,
	/*rG2D_SCL_HSCR0_22 		*/0x0258,
	/*rG2D_SCL_HSCR0_23 		*/0x025C,
	/*rG2D_SCL_HSCR0_24 		*/0x0260,
	/*rG2D_SCL_HSCR0_25 		*/0x0264,
	/*rG2D_SCL_HSCR0_26 		*/0x0268,
	/*rG2D_SCL_HSCR0_27 		*/0x026C,
	/*rG2D_SCL_HSCR0_28 		*/0x0270,
	/*rG2D_SCL_HSCR0_29 		*/0x0274,
	/*rG2D_SCL_HSCR0_30 		*/0x0278,
	/*rG2D_SCL_HSCR0_31 		*/0x027C,
	/*rG2D_SCL_HSCR0_32 		*/0x0280,
	/*rG2D_SCL_HSCR0_33 		*/0x0284,
	/*rG2D_SCL_HSCR0_34 		*/0x0288,
	/*rG2D_SCL_HSCR0_35 		*/0x028C,
	/*rG2D_SCL_HSCR0_36 		*/0x0290,
	/*rG2D_SCL_HSCR0_37 		*/0x0294,
	/*rG2D_SCL_HSCR0_38 		*/0x0298,
	/*rG2D_SCL_HSCR0_39 		*/0x029C,
	/*rG2D_SCL_HSCR0_40 		*/0x02A0,
	/*rG2D_SCL_HSCR0_41 		*/0x02A4,
	/*rG2D_SCL_HSCR0_42 		*/0x02A8,
	/*rG2D_SCL_HSCR0_43 		*/0x02AC,
	/*rG2D_SCL_HSCR0_44 		*/0x02B0,
	/*rG2D_SCL_HSCR0_45 		*/0x02B4,
	/*rG2D_SCL_HSCR0_46 		*/0x02B8,
	/*rG2D_SCL_HSCR0_47 		*/0x02BC,
	/*rG2D_SCL_HSCR0_48 		*/0x02C0,
	/*rG2D_SCL_HSCR0_49 		*/0x02C4,
	/*rG2D_SCL_HSCR0_50 		*/0x02C8,
	/*rG2D_SCL_HSCR0_51 		*/0x02CC,
	/*rG2D_SCL_HSCR0_52 		*/0x02D0,
	/*rG2D_SCL_HSCR0_53 		*/0x02D4,
	/*rG2D_SCL_HSCR0_54 		*/0x02D8,
	/*rG2D_SCL_HSCR0_55 		*/0x02DC,
	/*rG2D_SCL_HSCR0_56 		*/0x02E0,
	/*rG2D_SCL_HSCR0_57 		*/0x02E4,
	/*rG2D_SCL_HSCR0_58 		*/0x02E8,
	/*rG2D_SCL_HSCR0_59 		*/0x02EC,
	/*rG2D_SCL_HSCR0_60 		*/0x02F0,
	/*rG2D_SCL_HSCR0_61 		*/0x02F4,
	/*rG2D_SCL_HSCR0_62 		*/0x02F8,
	/*rG2D_SCL_HSCR0_63 		*/0x02FC,
	/*rG2D_SCL_HSCR1_0			*/0x0300,
	/*rG2D_SCL_HSCR1_1			*/0x0304,
	/*rG2D_SCL_HSCR1_2			*/0x0308,
	/*rG2D_SCL_HSCR1_3			*/0x030C,
	/*rG2D_SCL_HSCR1_4			*/0x0310,
	/*rG2D_SCL_HSCR1_5			*/0x0314,
	/*rG2D_SCL_HSCR1_6			*/0x0318,
	/*rG2D_SCL_HSCR1_7			*/0x031C,
	/*rG2D_SCL_HSCR1_8			*/0x0320,
	/*rG2D_SCL_HSCR1_9			*/0x0324,
	/*rG2D_SCL_HSCR1_10 		*/0x0328,
	/*rG2D_SCL_HSCR1_11 		*/0x032C,
	/*rG2D_SCL_HSCR1_12 		*/0x0330,
	/*rG2D_SCL_HSCR1_13 		*/0x0334,
	/*rG2D_SCL_HSCR1_14 		*/0x0338,
	/*rG2D_SCL_HSCR1_15 		*/0x033C,
	/*rG2D_SCL_HSCR1_16 		*/0x0340,
	/*rG2D_SCL_HSCR1_17 		*/0x0344,
	/*rG2D_SCL_HSCR1_18 		*/0x0348,
	/*rG2D_SCL_HSCR1_19 		*/0x034C,
	/*rG2D_SCL_HSCR1_20 		*/0x0350,
	/*rG2D_SCL_HSCR1_21 		*/0x0354,
	/*rG2D_SCL_HSCR1_22 		*/0x0358,
	/*rG2D_SCL_HSCR1_23 		*/0x035C,
	/*rG2D_SCL_HSCR1_24 		*/0x0360,
	/*rG2D_SCL_HSCR1_25 		*/0x0364,
	/*rG2D_SCL_HSCR1_26 		*/0x0368,
	/*rG2D_SCL_HSCR1_27 		*/0x036C,
	/*rG2D_SCL_HSCR1_28 		*/0x0370,
	/*rG2D_SCL_HSCR1_29 		*/0x0374,
	/*rG2D_SCL_HSCR1_30 		*/0x0378,
	/*rG2D_SCL_HSCR1_31 		*/0x037C,
	/*rG2D_SCL_HSCR1_32 		*/0x0380,
	/*rG2D_SCL_HSCR1_33 		*/0x0384,
	/*rG2D_SCL_HSCR1_34 		*/0x0388,
	/*rG2D_SCL_HSCR1_35 		*/0x038C,
	/*rG2D_SCL_HSCR1_36 		*/0x0390,
	/*rG2D_SCL_HSCR1_37 		*/0x0394,
	/*rG2D_SCL_HSCR1_38 		*/0x0398,
	/*rG2D_SCL_HSCR1_39 		*/0x039C,
	/*rG2D_SCL_HSCR1_40 		*/0x03A0,
	/*rG2D_SCL_HSCR1_41 		*/0x03A4,
	/*rG2D_SCL_HSCR1_42 		*/0x03A8,
	/*rG2D_SCL_HSCR1_43 		*/0x03AC,
	/*rG2D_SCL_HSCR1_44 		*/0x03B0,
	/*rG2D_SCL_HSCR1_45 		*/0x03B4,
	/*rG2D_SCL_HSCR1_46 		*/0x03B8,
	/*rG2D_SCL_HSCR1_47 		*/0x03BC,
	/*rG2D_SCL_HSCR1_48 		*/0x03C0,
	/*rG2D_SCL_HSCR1_49 		*/0x03C4,
	/*rG2D_SCL_HSCR1_50 		*/0x03C8,
	/*rG2D_SCL_HSCR1_51 		*/0x03CC,
	/*rG2D_SCL_HSCR1_52 		*/0x03D0,
	/*rG2D_SCL_HSCR1_53 		*/0x03D4,
	/*rG2D_SCL_HSCR1_54 		*/0x03D8,
	/*rG2D_SCL_HSCR1_55 		*/0x03DC,
	/*rG2D_SCL_HSCR1_56 		*/0x03E0,
	/*rG2D_SCL_HSCR1_57 		*/0x03E4,
	/*rG2D_SCL_HSCR1_58 		*/0x03E8,
	/*rG2D_SCL_HSCR1_59 		*/0x03EC,
	/*rG2D_SCL_HSCR1_60 		*/0x03F0,
	/*rG2D_SCL_HSCR1_61 		*/0x03F4,
	/*rG2D_SCL_HSCR1_62 		*/0x03F8,
	/*rG2D_SCL_HSCR1_63 		*/0x03FC,
	/*rG2D_SCL_HSCR2_0			*/0x0400,
	/*rG2D_SCL_HSCR2_1			*/0x0404,
	/*rG2D_SCL_HSCR2_2			*/0x0408,
	/*rG2D_SCL_HSCR2_3			*/0x040C,
	/*rG2D_SCL_HSCR2_4			*/0x0410,
	/*rG2D_SCL_HSCR2_5			*/0x0414,
	/*rG2D_SCL_HSCR2_6			*/0x0418,
	/*rG2D_SCL_HSCR2_7			*/0x041C,
	/*rG2D_SCL_HSCR2_8			*/0x0420,
	/*rG2D_SCL_HSCR2_9			*/0x0424,
	/*rG2D_SCL_HSCR2_10 		*/0x0428,
	/*rG2D_SCL_HSCR2_11 		*/0x042C,
	/*rG2D_SCL_HSCR2_12 		*/0x0430,
	/*rG2D_SCL_HSCR2_13 		*/0x0434,
	/*rG2D_SCL_HSCR2_14 		*/0x0438,
	/*rG2D_SCL_HSCR2_15 		*/0x043C,
	/*rG2D_SCL_HSCR2_16 		*/0x0440,
	/*rG2D_SCL_HSCR2_17 		*/0x0444,
	/*rG2D_SCL_HSCR2_18 		*/0x0448,
	/*rG2D_SCL_HSCR2_19 		*/0x044C,
	/*rG2D_SCL_HSCR2_20 		*/0x0450,
	/*rG2D_SCL_HSCR2_21 		*/0x0454,
	/*rG2D_SCL_HSCR2_22 		*/0x0458,
	/*rG2D_SCL_HSCR2_23 		*/0x045C,
	/*rG2D_SCL_HSCR2_24 		*/0x0460,
	/*rG2D_SCL_HSCR2_25 		*/0x0464,
	/*rG2D_SCL_HSCR2_26 		*/0x0468,
	/*rG2D_SCL_HSCR2_27 		*/0x046C,
	/*rG2D_SCL_HSCR2_28 		*/0x0470,
	/*rG2D_SCL_HSCR2_29 		*/0x0474,
	/*rG2D_SCL_HSCR2_30 		*/0x0478,
	/*rG2D_SCL_HSCR2_31 		*/0x047C,
	/*rG2D_SCL_HSCR2_32 		*/0x0480,
	/*rG2D_SCL_HSCR2_33 		*/0x0484,
	/*rG2D_SCL_HSCR2_34 		*/0x0488,
	/*rG2D_SCL_HSCR2_35 		*/0x048C,
	/*rG2D_SCL_HSCR2_36 		*/0x0490,
	/*rG2D_SCL_HSCR2_37 		*/0x0494,
	/*rG2D_SCL_HSCR2_38 		*/0x0498,
	/*rG2D_SCL_HSCR2_39 		*/0x049C,
	/*rG2D_SCL_HSCR2_40 		*/0x04A0,
	/*rG2D_SCL_HSCR2_41 		*/0x04A4,
	/*rG2D_SCL_HSCR2_42 		*/0x04A8,
	/*rG2D_SCL_HSCR2_43 		*/0x04AC,
	/*rG2D_SCL_HSCR2_44 		*/0x04B0,
	/*rG2D_SCL_HSCR2_45 		*/0x04B4,
	/*rG2D_SCL_HSCR2_46 		*/0x04B8,
	/*rG2D_SCL_HSCR2_47 		*/0x04BC,
	/*rG2D_SCL_HSCR2_48 		*/0x04C0,
	/*rG2D_SCL_HSCR2_49 		*/0x04C4,
	/*rG2D_SCL_HSCR2_50 		*/0x04C8,
	/*rG2D_SCL_HSCR2_51 		*/0x04CC,
	/*rG2D_SCL_HSCR2_52 		*/0x04D0,
	/*rG2D_SCL_HSCR2_53 		*/0x04D4,
	/*rG2D_SCL_HSCR2_54 		*/0x04D8,
	/*rG2D_SCL_HSCR2_55 		*/0x04DC,
	/*rG2D_SCL_HSCR2_56 		*/0x04E0,
	/*rG2D_SCL_HSCR2_57 		*/0x04E4,
	/*rG2D_SCL_HSCR2_58 		*/0x04E8,
	/*rG2D_SCL_HSCR2_59 		*/0x04EC,
	/*rG2D_SCL_HSCR2_60 		*/0x04F0,
	/*rG2D_SCL_HSCR2_61 		*/0x04F4,
	/*rG2D_SCL_HSCR2_62 		*/0x04F8,
	/*rG2D_SCL_HSCR2_63 		*/0x04FC,
	/*rG2D_SCL_HSCR3_0			*/0x0500,
	/*rG2D_SCL_HSCR3_1			*/0x0504,
	/*rG2D_SCL_HSCR3_2			*/0x0508,
	/*rG2D_SCL_HSCR3_3			*/0x050C,
	/*rG2D_SCL_HSCR3_4			*/0x0510,
	/*rG2D_SCL_HSCR3_5			*/0x0514,
	/*rG2D_SCL_HSCR3_6			*/0x0518,
	/*rG2D_SCL_HSCR3_7			*/0x051C,
	/*rG2D_SCL_HSCR3_8			*/0x0520,
	/*rG2D_SCL_HSCR3_9			*/0x0524,
	/*rG2D_SCL_HSCR3_10 		*/0x0528,
	/*rG2D_SCL_HSCR3_11 		*/0x052C,
	/*rG2D_SCL_HSCR3_12 		*/0x0530,
	/*rG2D_SCL_HSCR3_13 		*/0x0534,
	/*rG2D_SCL_HSCR3_14 		*/0x0538,
	/*rG2D_SCL_HSCR3_15 		*/0x053C,
	/*rG2D_SCL_HSCR3_16 		*/0x0540,
	/*rG2D_SCL_HSCR3_17 		*/0x0544,
	/*rG2D_SCL_HSCR3_18 		*/0x0548,
	/*rG2D_SCL_HSCR3_19 		*/0x054C,
	/*rG2D_SCL_HSCR3_20 		*/0x0550,
	/*rG2D_SCL_HSCR3_21 		*/0x0554,
	/*rG2D_SCL_HSCR3_22 		*/0x0558,
	/*rG2D_SCL_HSCR3_23 		*/0x055C,
	/*rG2D_SCL_HSCR3_24 		*/0x0560,
	/*rG2D_SCL_HSCR3_25 		*/0x0564,
	/*rG2D_SCL_HSCR3_26 		*/0x0568,
	/*rG2D_SCL_HSCR3_27 		*/0x056C,
	/*rG2D_SCL_HSCR3_28 		*/0x0570,
	/*rG2D_SCL_HSCR3_29 		*/0x0574,
	/*rG2D_SCL_HSCR3_30 		*/0x0578,
	/*rG2D_SCL_HSCR3_31 		*/0x057C,
	/*rG2D_SCL_HSCR3_32 		*/0x0580,
	/*rG2D_SCL_HSCR3_33 		*/0x0584,
	/*rG2D_SCL_HSCR3_34 		*/0x0588,
	/*rG2D_SCL_HSCR3_35 		*/0x058C,
	/*rG2D_SCL_HSCR3_36 		*/0x0590,
	/*rG2D_SCL_HSCR3_37 		*/0x0594,
	/*rG2D_SCL_HSCR3_38 		*/0x0598,
	/*rG2D_SCL_HSCR3_39 		*/0x059C,
	/*rG2D_SCL_HSCR3_40 		*/0x05A0,
	/*rG2D_SCL_HSCR3_41 		*/0x05A4,
	/*rG2D_SCL_HSCR3_42 		*/0x05A8,
	/*rG2D_SCL_HSCR3_43 		*/0x05AC,
	/*rG2D_SCL_HSCR3_44 		*/0x05B0,
	/*rG2D_SCL_HSCR3_45 		*/0x05B4,
	/*rG2D_SCL_HSCR3_46 		*/0x05B8,
	/*rG2D_SCL_HSCR3_47 		*/0x05BC,
	/*rG2D_SCL_HSCR3_48 		*/0x05C0,
	/*rG2D_SCL_HSCR3_49 		*/0x05C4,
	/*rG2D_SCL_HSCR3_50 		*/0x05C8,
	/*rG2D_SCL_HSCR3_51 		*/0x05CC,
	/*rG2D_SCL_HSCR3_52 		*/0x05D0,
	/*rG2D_SCL_HSCR3_53 		*/0x05D4,
	/*rG2D_SCL_HSCR3_54 		*/0x05D8,
	/*rG2D_SCL_HSCR3_55 		*/0x05DC,
	/*rG2D_SCL_HSCR3_56 		*/0x05E0,
	/*rG2D_SCL_HSCR3_57 		*/0x05E4,
	/*rG2D_SCL_HSCR3_58 		*/0x05E8,
	/*rG2D_SCL_HSCR3_59 		*/0x05EC,
	/*rG2D_SCL_HSCR3_60 		*/0x05F0,
	/*rG2D_SCL_HSCR3_61 		*/0x05F4,
	/*rG2D_SCL_HSCR3_62 		*/0x05F8,
	/*rG2D_SCL_HSCR3_63 		*/0x05FC,
	/*rG2D_SCL_VSCR0_0			*/0x0600,
	/*rG2D_SCL_VSCR0_1			*/0x0604,
	/*rG2D_SCL_VSCR0_2			*/0x0608,
	/*rG2D_SCL_VSCR0_3			*/0x060C,
	/*rG2D_SCL_VSCR0_4			*/0x0610,
	/*rG2D_SCL_VSCR0_5			*/0x0614,
	/*rG2D_SCL_VSCR0_6			*/0x0618,
	/*rG2D_SCL_VSCR0_7			*/0x061C,
	/*rG2D_SCL_VSCR0_8			*/0x0620,
	/*rG2D_SCL_VSCR0_9			*/0x0624,
	/*rG2D_SCL_VSCR0_10 		*/0x0628,
	/*rG2D_SCL_VSCR0_11 		*/0x062C,
	/*rG2D_SCL_VSCR0_12 		*/0x0630,
	/*rG2D_SCL_VSCR0_13 		*/0x0634,
	/*rG2D_SCL_VSCR0_14 		*/0x0638,
	/*rG2D_SCL_VSCR0_15 		*/0x063C,
	/*rG2D_SCL_VSCR0_16 		*/0x0640,
	/*rG2D_SCL_VSCR0_17 		*/0x0644,
	/*rG2D_SCL_VSCR0_18 		*/0x0648,
	/*rG2D_SCL_VSCR0_19 		*/0x064C,
	/*rG2D_SCL_VSCR0_20 		*/0x0650,
	/*rG2D_SCL_VSCR0_21 		*/0x0654,
	/*rG2D_SCL_VSCR0_22 		*/0x0658,
	/*rG2D_SCL_VSCR0_23 		*/0x065C,
	/*rG2D_SCL_VSCR0_24 		*/0x0660,
	/*rG2D_SCL_VSCR0_25 		*/0x0664,
	/*rG2D_SCL_VSCR0_26 		*/0x0668,
	/*rG2D_SCL_VSCR0_27 		*/0x066C,
	/*rG2D_SCL_VSCR0_28 		*/0x0670,
	/*rG2D_SCL_VSCR0_29 		*/0x0674,
	/*rG2D_SCL_VSCR0_30 		*/0x0678,
	/*rG2D_SCL_VSCR0_31 		*/0x067C,
	/*rG2D_SCL_VSCR0_32 		*/0x0680,
	/*rG2D_SCL_VSCR0_33 		*/0x0684,
	/*rG2D_SCL_VSCR0_34 		*/0x0688,
	/*rG2D_SCL_VSCR0_35 		*/0x068C,
	/*rG2D_SCL_VSCR0_36 		*/0x0690,
	/*rG2D_SCL_VSCR0_37 		*/0x0694,
	/*rG2D_SCL_VSCR0_38 		*/0x0698,
	/*rG2D_SCL_VSCR0_39 		*/0x069C,
	/*rG2D_SCL_VSCR0_40 		*/0x06A0,
	/*rG2D_SCL_VSCR0_41 		*/0x06A4,
	/*rG2D_SCL_VSCR0_42 		*/0x06A8,
	/*rG2D_SCL_VSCR0_43 		*/0x06AC,
	/*rG2D_SCL_VSCR0_44 		*/0x06B0,
	/*rG2D_SCL_VSCR0_45 		*/0x06B4,
	/*rG2D_SCL_VSCR0_46 		*/0x06B8,
	/*rG2D_SCL_VSCR0_47 		*/0x06BC,
	/*rG2D_SCL_VSCR0_48 		*/0x06C0,
	/*rG2D_SCL_VSCR0_49 		*/0x06C4,
	/*rG2D_SCL_VSCR0_50 		*/0x06C8,
	/*rG2D_SCL_VSCR0_51 		*/0x06CC,
	/*rG2D_SCL_VSCR0_52 		*/0x06D0,
	/*rG2D_SCL_VSCR0_53 		*/0x06D4,
	/*rG2D_SCL_VSCR0_54 		*/0x06D8,
	/*rG2D_SCL_VSCR0_55 		*/0x06DC,
	/*rG2D_SCL_VSCR0_56 		*/0x06E0,
	/*rG2D_SCL_VSCR0_57 		*/0x06E4,
	/*rG2D_SCL_VSCR0_58 		*/0x06E8,
	/*rG2D_SCL_VSCR0_59 		*/0x06EC,
	/*rG2D_SCL_VSCR0_60 		*/0x06F0,
	/*rG2D_SCL_VSCR0_61 		*/0x06F4,
	/*rG2D_SCL_VSCR0_62 		*/0x06F8,
	/*rG2D_SCL_VSCR0_63 		*/0x06FC,
	/*rG2D_SCL_VSCR1_0			*/0x0700,
	/*rG2D_SCL_VSCR1_1			*/0x0704,
	/*rG2D_SCL_VSCR1_2			*/0x0708,
	/*rG2D_SCL_VSCR1_3			*/0x070C,
	/*rG2D_SCL_VSCR1_4			*/0x0710,
	/*rG2D_SCL_VSCR1_5			*/0x0714,
	/*rG2D_SCL_VSCR1_6			*/0x0718,
	/*rG2D_SCL_VSCR1_7			*/0x071C,
	/*rG2D_SCL_VSCR1_8			*/0x0720,
	/*rG2D_SCL_VSCR1_9			*/0x0724,
	/*rG2D_SCL_VSCR1_10 		*/0x0728,
	/*rG2D_SCL_VSCR1_11 		*/0x072C,
	/*rG2D_SCL_VSCR1_12 		*/0x0730,
	/*rG2D_SCL_VSCR1_13 		*/0x0734,
	/*rG2D_SCL_VSCR1_14 		*/0x0738,
	/*rG2D_SCL_VSCR1_15 		*/0x073C,
	/*rG2D_SCL_VSCR1_16 		*/0x0740,
	/*rG2D_SCL_VSCR1_17 		*/0x0744,
	/*rG2D_SCL_VSCR1_18 		*/0x0748,
	/*rG2D_SCL_VSCR1_19 		*/0x074C,
	/*rG2D_SCL_VSCR1_20 		*/0x0750,
	/*rG2D_SCL_VSCR1_21 		*/0x0754,
	/*rG2D_SCL_VSCR1_22 		*/0x0758,
	/*rG2D_SCL_VSCR1_23 		*/0x075C,
	/*rG2D_SCL_VSCR1_24 		*/0x0760,
	/*rG2D_SCL_VSCR1_25 		*/0x0764,
	/*rG2D_SCL_VSCR1_26 		*/0x0768,
	/*rG2D_SCL_VSCR1_27 		*/0x076C,
	/*rG2D_SCL_VSCR1_28 		*/0x0770,
	/*rG2D_SCL_VSCR1_29 		*/0x0774,
	/*rG2D_SCL_VSCR1_30 		*/0x0778,
	/*rG2D_SCL_VSCR1_31 		*/0x077C,
	/*rG2D_SCL_VSCR1_32 		*/0x0780,
	/*rG2D_SCL_VSCR1_33 		*/0x0784,
	/*rG2D_SCL_VSCR1_34 		*/0x0788,
	/*rG2D_SCL_VSCR1_35 		*/0x078C,
	/*rG2D_SCL_VSCR1_36 		*/0x0790,
	/*rG2D_SCL_VSCR1_37 		*/0x0794,
	/*rG2D_SCL_VSCR1_38 		*/0x0798,
	/*rG2D_SCL_VSCR1_39 		*/0x079C,
	/*rG2D_SCL_VSCR1_40 		*/0x07A0,
	/*rG2D_SCL_VSCR1_41 		*/0x07A4,
	/*rG2D_SCL_VSCR1_42 		*/0x07A8,
	/*rG2D_SCL_VSCR1_43 		*/0x07AC,
	/*rG2D_SCL_VSCR1_44 		*/0x07B0,
	/*rG2D_SCL_VSCR1_45 		*/0x07B4,
	/*rG2D_SCL_VSCR1_46 		*/0x07B8,
	/*rG2D_SCL_VSCR1_47 		*/0x07BC,
	/*rG2D_SCL_VSCR1_48 		*/0x07C0,
	/*rG2D_SCL_VSCR1_49 		*/0x07C4,
	/*rG2D_SCL_VSCR1_50 		*/0x07C8,
	/*rG2D_SCL_VSCR1_51 		*/0x07CC,
	/*rG2D_SCL_VSCR1_52 		*/0x07D0,
	/*rG2D_SCL_VSCR1_53 		*/0x07D4,
	/*rG2D_SCL_VSCR1_54 		*/0x07D8,
	/*rG2D_SCL_VSCR1_55 		*/0x07DC,
	/*rG2D_SCL_VSCR1_56 		*/0x07E0,
	/*rG2D_SCL_VSCR1_57 		*/0x07E4,
	/*rG2D_SCL_VSCR1_58 		*/0x07E8,
	/*rG2D_SCL_VSCR1_59 		*/0x07EC,
	/*rG2D_SCL_VSCR1_60 		*/0x07F0,
	/*rG2D_SCL_VSCR1_61 		*/0x07F4,
	/*rG2D_SCL_VSCR1_62 		*/0x07F8,
	/*rG2D_SCL_VSCR1_63 		*/0x07FC,
	/*rG2D_DIT_DM_COEF_0003 	*/0x0808,
	/*rG2D_DIT_DM_COEF_0407 	*/0x080C,
	/*rG2D_DIT_DM_COEF_1013 	*/0x0810,
	/*rG2D_DIT_DM_COEF_1417 	*/0x0814,
	/*rG2D_DIT_DM_COEF_2023 	*/0x0818,
	/*rG2D_DIT_DM_COEF_2427 	*/0x081C,
	/*rG2D_DIT_DM_COEF_3033 	*/0x0820,
	/*rG2D_DIT_DM_COEF_3437 	*/0x0824,
	/*rG2D_DIT_DE_COEF			*/0x0828,
};



/*********************************
*  g2d register bits field info  *
*   { REGNUM, BITS, POSITION }   *
*********************************/
static const unsigned int g2dRegSpec[G2D_LAST_REG_ID + 1][3] = {

	/*register id					  */ /*register num,	   BITS,POS*/	/*[hiBit:loBit]*/
	/*G2D_OUT_BITSWP_FORMAT_BPP 	  */{rG2D_OUT_BITSWP	  ,  5, 16},	//[20:16]
	/*G2D_OUT_BITSWP_BITADR 		  */{rG2D_OUT_BITSWP	  ,  6,  5},	//[10:5]
	/*G2D_OUT_BITSWP_HFWSWP 		  */{rG2D_OUT_BITSWP	  ,  1,  4},	//[4:4]
	/*G2D_OUT_BITSWP_BYTSWP 		  */{rG2D_OUT_BITSWP	  ,  1,  3},	//[3:3]
	/*G2D_OUT_BITSWP_HFBSWP 		  */{rG2D_OUT_BITSWP	  ,  1,  2},	//[2:2]
	/*G2D_OUT_BITSWP_BIT2SWP		  */{rG2D_OUT_BITSWP	  ,  1,  1},	//[1:1]
	/*G2D_OUT_BITSWP_BITSWP 		  */{rG2D_OUT_BITSWP	  ,  1,  0},	//[0:0]
	/*G2D_IF_XYST_YSTART			  */{rG2D_IF_XYST		  , 12, 16},	//[27:16]
	/*G2D_IF_XYST_XSTART			  */{rG2D_IF_XYST		  , 12,  0},	//[11:0]
	/*G2D_IF_WIDTH_1				  */{rG2D_IF_WIDTH		  , 12, 16},	//[27:16]
	/*G2D_IF_WIDTH					  */{rG2D_IF_WIDTH		  , 12,  0},	//[11:0]
	/*G2D_IF_HEIGHT_1				  */{rG2D_IF_HEIGHT 	  , 12, 16},	//[27:16]
	/*G2D_IF_HEIGHT 				  */{rG2D_IF_HEIGHT 	  , 12,  0},	//[11:0]
	/*G2D_IF_FULL_WIDTH_1			  */{rG2D_IF_FULL_WIDTH   , 12, 16},	//[27:16]
	/*G2D_IF_FULL_WIDTH 			  */{rG2D_IF_FULL_WIDTH   , 12,  0},	//[11:0]
	/*G2D_IF_ADDR_ST				  */{rG2D_IF_ADDR_ST	  , 32,  0},	//[31:0]
	/*G2D_MEM_ADDR_ST				  */{rG2D_MEM_ADDR_ST	  , 32,  0},	//[31:0]
	/*G2D_MEM_LENGTH				  */{rG2D_MEM_LENGTH_ST   , 32,  0},	//[31:0]
	/*G2D_MEM_SETDATA32 			  */{rG2D_MEM_SETDATA32   , 32,  0},	//[31:0]
	/*G2D_MEM_SETDATA64 			  */{rG2D_MEM_SETDATA64   , 32,  0},	//[31:0]
	/*G2D_MEM_CODE_LEN_SET_LEN		  */{rG2D_MEM_CODE_LEN	  ,  3,  4},	//[6:4]
	/*G2D_MEM_CODE_LEN_CODE 		  */{rG2D_MEM_CODE_LEN	  ,  2,  0},	//[1:0]
	/*G2D_ALU_CNTL_DRAW_EN			  */{rG2D_ALU_CNTL		  ,  1,  5},	//[5:5]
	/*G2D_ALU_CNTL_MEM_SEL			  */{rG2D_ALU_CNTL		  ,  1,  4},	//[4:4]
	/*G2D_ALU_CNTL_ROT_CODE 		  */{rG2D_ALU_CNTL		  ,  3,  1},	//[3:1]
	/*G2D_ALU_CNTL_ROT_SEL			  */{rG2D_ALU_CNTL		  ,  1,  0},	//[0:0]
	/*G2D_SRC_XYST_SOFTRST			  */{rG2D_SRC_XYST		  ,  1, 30},	//[30:30]
	/*G2D_SRC_XYST_YSTART			  */{rG2D_SRC_XYST		  , 12, 16},	//[27:16]
	/*G2D_SRC_XYST_XSTART			  */{rG2D_SRC_XYST		  , 12,  0},	//[11:0]
	/*G2D_SRC_SIZE_HEIGHT			  */{rG2D_SRC_SIZE		  , 12, 16},	//[27:16]
	/*G2D_SRC_SIZE_WIDTH			  */{rG2D_SRC_SIZE		  , 12,  0},	//[11:0]
	/*G2D_SRC_SIZE_1_HEIGHT 		  */{rG2D_SRC_SIZE_1	  , 12, 16},	//[27:16]
	/*G2D_SRC_SIZE_1_WIDTH			  */{rG2D_SRC_SIZE_1	  , 12,  0},	//[11:0]
	/*G2D_SRC_FULL_WIDTH_1			  */{rG2D_SRC_FULL_WIDTH  , 12, 16},	//[27:16]
	/*G2D_SRC_FULL_WIDTH			  */{rG2D_SRC_FULL_WIDTH  , 12,  0},	//[11:0]
	/*G2D_DEST_XYST_YSTART			  */{rG2D_DEST_XYST 	  , 12, 16},	//[27:16]
	/*G2D_DEST_XYST_XSTART			  */{rG2D_DEST_XYST 	  , 12,  0},	//[11:0]
	/*G2D_DEST_SIZE_HEIGHT			  */{rG2D_DEST_SIZE 	  , 12, 16},	//[27:16]
	/*G2D_DEST_SIZE_WIDTH			  */{rG2D_DEST_SIZE 	  , 12,  0},	//[11:0]
	/*G2D_DEST_SIZE_1_HEIGHT		  */{rG2D_DEST_SIZE_1	  , 12, 16},	//[27:16]
	/*G2D_DEST_SIZE_1_WIDTH 		  */{rG2D_DEST_SIZE_1	  , 12,  0},	//[11:0]
	/*G2D_DEST_FULL_WIDTH_1 		  */{rG2D_DEST_FULL_WIDTH , 12, 16},	//[27:16]
	/*G2D_DEST_FULL_WIDTH			  */{rG2D_DEST_FULL_WIDTH , 12,  0},	//[11:0]
	/*G2D_PAT_SIZE_HEIGHT			  */{rG2D_PAT_SIZE		  ,  4, 16},	//[19:16]
	/*G2D_PAT_SIZE_WIDTH			  */{rG2D_PAT_SIZE		  ,  4,  0},	//[3:0]
	/*G2D_SRC_BITSWP_BITADR 		  */{rG2D_SRC_BITSWP	  ,  6,  5},	//[10:5]
	/*G2D_SRC_BITSWP_HFWSWP 		  */{rG2D_SRC_BITSWP	  ,  1,  4},	//[4:4]
	/*G2D_SRC_BITSWP_BYTSWP 		  */{rG2D_SRC_BITSWP	  ,  1,  3},	//[3:3]
	/*G2D_SRC_BITSWP_HFBSWP 		  */{rG2D_SRC_BITSWP	  ,  1,  2},	//[2:2]
	/*G2D_SRC_BITSWP_BIT2SWP		  */{rG2D_SRC_BITSWP	  ,  1,  1},	//[1:1]
	/*G2D_SRC_BITSWP_BITSWP 		  */{rG2D_SRC_BITSWP	  ,  1,  0},	//[0:0]
	/*G2D_SRC_FORMAT_PAL			  */{rG2D_SRC_FORMAT	  ,  3,  8},	//[10:8]
	/*G2D_SRC_FORMAT_BPP			  */{rG2D_SRC_FORMAT	  ,  5,  0},	//[4:0]
	/*G2D_SRC_WIDOFFSET 			  */{rG2D_SRC_WIDOFFSET   ,  6,  0},	//[5:0]
	/*G2D_SRC_RGB_CSC_BYPASSS		  */{rG2D_SRC_RGB		  ,  1,  4},	//[4:4]
	/*G2D_SRC_RGB_YUVFMT			  */{rG2D_SRC_RGB		  ,  3,  1},	//[3:1]
	/*G2D_SRC_RGB_BRREV 			  */{rG2D_SRC_RGB		  ,  1,  0},	//[0:0]
	/*G2D_SRC_COEF1112_COEF12		  */{rG2D_SRC_COEF1112	  , 13, 16},	//[28:16]
	/*G2D_SRC_COEF1112_COEF11		  */{rG2D_SRC_COEF1112	  , 13,  0},	//[12:0]
	/*G2D_SRC_COEF1321_COEF21		  */{rG2D_SRC_COEF1321	  , 13, 16},	//[28:16]
	/*G2D_SRC_COEF1321_COEF13		  */{rG2D_SRC_COEF1321	  , 13,  0},	//[12:0]
	/*G2D_SRC_COEF2322_COEF23		  */{rG2D_SRC_COEF2223	  , 13, 16},	//[28:16]
	/*G2D_SRC_COEF2322_COEF22		  */{rG2D_SRC_COEF2223	  , 13,  0},	//[12:0]
	/*G2D_SRC_COEF3132_COEF32		  */{rG2D_SRC_COEF3132	  , 13, 16},	//[28:16]
	/*G2D_SRC_COEF3132_COEF31		  */{rG2D_SRC_COEF3132	  , 13,  0},	//[12:0]
	/*G2D_SRC_COEF33_OFT_OFTA		  */{rG2D_SRC_COEF33_OFT  ,  6, 24},	//[29:24]
	/*G2D_SRC_COEF33_OFT_OFTB		  */{rG2D_SRC_COEF33_OFT  ,  6, 16},	//[21:16]
	/*G2D_SRC_COEF33_OFT_COEF33 	  */{rG2D_SRC_COEF33_OFT  , 13,  0},	//[12:0]
	/*G2D_DEST_BITSWP_BITADR		  */{rG2D_DEST_BITSWP	  ,  6,  5},	//[10:5]
	/*G2D_DEST_BITSWP_HFWSWP		  */{rG2D_DEST_BITSWP	  ,  1,  4},	//[4:4]
	/*G2D_DEST_BITSWP_BYTSWP		  */{rG2D_DEST_BITSWP	  ,  1,  3},	//[3:3]
	/*G2D_DEST_BITSWP_HFBSWP		  */{rG2D_DEST_BITSWP	  ,  1,  2},	//[2:2]
	/*G2D_DEST_BITSWP_BIT2SWP		  */{rG2D_DEST_BITSWP	  ,  1,  1},	//[1:1]
	/*G2D_DEST_BITSWP_BITSWP		  */{rG2D_DEST_BITSWP	  ,  1,  0},	//[0:0]
	/*G2D_DEST_FORMAT_PAL			  */{rG2D_DEST_FORMAT	  ,  3,  8},	//[10:8]
	/*G2D_DEST_FORMAT_BPP			  */{rG2D_DEST_FORMAT	  ,  5,  0},	//[4:0]
	/*G2D_DEST_WIDOFFSET_BRREV		  */{rG2D_DEST_WIDOFFSET  ,  1,  6},	//[6:6]
	/*G2D_DEST_WIDOFFSET_OFFSET 	  */{rG2D_DEST_WIDOFFSET  ,  6,  0},	//[5:0]
	/*G2D_ALPHAG_GB 				  */{rG2D_ALPHAG		  ,  8, 24},	//[31:24]
	/*G2D_ALPHAG_GA 				  */{rG2D_ALPHAG		  ,  8,  8},	//[15:8]
	/*G2D_ALPHAG_GSEL				  */{rG2D_ALPHAG		  ,  1,  0},	//[0:0]
	/*G2D_CK_KEY					  */{rG2D_CK_KEY		  , 24,  0},	//[23:0]
	/*G2D_CK_MASK					  */{rG2D_CK_MASK		  , 24,  0},	//[23:0]
	/*G2D_CK_CNTL_ROP_CODE			  */{rG2D_CK_CNTL		  , 10, 16},	//[25:16]
	/*G2D_CK_CNTL_AR_CODE			  */{rG2D_CK_CNTL		  ,  4,  4},	//[7:4]
	/*G2D_CK_CNTL_CK_DIR			  */{rG2D_CK_CNTL		  ,  1,  2},	//[2:2]
	/*G2D_CK_CNTL_CK_ABLD_EN		  */{rG2D_CK_CNTL		  ,  1,  1},	//[1:1]
	/*G2D_CK_CNTL_CK_EN 			  */{rG2D_CK_CNTL		  ,  1,  0},	//[0:0]
	/*G2D_Y_BITSWP_BITADR			  */{rG2D_Y_BITSWP		  ,  6,  5},	//[10:5]
	/*G2D_Y_BITSWP_HFWSWP			  */{rG2D_Y_BITSWP		  ,  1,  4},	//[4:4]
	/*G2D_Y_BITSWP_BYTSWP			  */{rG2D_Y_BITSWP		  ,  1,  3},	//[3:3]
	/*G2D_Y_BITSWP_HFBSWP			  */{rG2D_Y_BITSWP		  ,  1,  2},	//[2:2]
	/*G2D_Y_BITSWP_BIT2SWP			  */{rG2D_Y_BITSWP		  ,  1,  1},	//[1:1]
	/*G2D_Y_BITSWP_BITSWP			  */{rG2D_Y_BITSWP		  ,  1,  0},	//[0:0]
	/*G2D_UV_BITSWP_BITADR			  */{rG2D_UV_BITSWP 	  ,  6,  5},	//[10:5]
	/*G2D_UV_BITSWP_HFWSWP			  */{rG2D_UV_BITSWP 	  ,  1,  4},	//[4:4]
	/*G2D_UV_BITSWP_BYTSWP			  */{rG2D_UV_BITSWP 	  ,  1,  3},	//[3:3]
	/*G2D_UV_BITSWP_HFBSWP			  */{rG2D_UV_BITSWP 	  ,  1,  2},	//[2:2]
	/*G2D_UV_BITSWP_BIT2SWP 		  */{rG2D_UV_BITSWP 	  ,  1,  1},	//[1:1]
	/*G2D_UV_BITSWP_BITSWP			  */{rG2D_UV_BITSWP 	  ,  1,  0},	//[0:0]
	/*G2D_YUV_OFFSET_UV 			  */{rG2D_YUV_OFFSET	  ,  6,  8},	//[13:8]
	/*G2D_YUV_OFFSET_Y				  */{rG2D_YUV_OFFSET	  ,  6,  0},	//[5:0]
	/*G2D_Y_ADDR_ST 				  */{rG2D_Y_ADDR_ST 	  , 32,  0},	//[31:0]
	/*G2D_UV_ADDR_ST				  */{rG2D_UV_ADDR_ST	  , 32,  0},	//[31:0]
	/*G2D_DEST_ADDR_ST				  */{rG2D_DEST_ADDR_ST	  , 32,  0},	//[31:0]
	/*G2D_DRAW_CNTL_FOOT			  */{rG2D_DRAW_CNTL 	  ,  1,  7},	//[7:7]
	/*G2D_DRAW_CNTL_WIDTH			  */{rG2D_DRAW_CNTL 	  ,  4,  3},	//[6:3]
	/*G2D_DRAW_CNT_CODE 			  */{rG2D_DRAW_CNTL 	  ,  2,  1},	//[2:1]
	/*G2D_DRAW_START_Y				  */{rG2D_DRAW_START	  , 12, 16},	//[27:16]
	/*G2D_DRAW_START_X				  */{rG2D_DRAW_START	  , 12,  0},	//[11:0]
	/*G2D_DRAW_END_Y				  */{rG2D_DRAW_END		  , 12, 16},	//[27:16]
	/*G2D_DRAW_END_X				  */{rG2D_DRAW_END		  , 12,  0},	//[11:0]
	/*G2D_DRAW_MASK 				  */{rG2D_DRAW_MASK 	  , 32,  0},	//[31:0]
	/*G2D_DRAW_MASK_DATA			  */{rG2D_DRAW_MASK_DATA  , 32,  0},	//[31:0]
	/*G2D_LUT_READY_PAT 			  */{rG2D_LUT_READY 	  ,  1,  2},	//[2:2]
	/*G2D_LUT_READY_DEST_MONO		  */{rG2D_LUT_READY 	  ,  1,  1},	//[1:1]
	/*G2D_LUT_READY_SRC_MONO		  */{rG2D_LUT_READY 	  ,  1,  0},	//[0:0]
	/*G2D_SCL_IFSR_IVRES			  */{rG2D_SCL_IFSR		  , 12, 16},	//[27:16]
	/*G2D_SCL_IFSR_IHRES			  */{rG2D_SCL_IFSR		  , 12,  0},	//[11:0]
	/*G2D_SCL_OFSR_OVRES			  */{rG2D_SCL_OFSR		  , 12, 16},	//[27:16]
	/*G2D_SCL_OFSR_OHRES			  */{rG2D_SCL_OFSR		  , 12,  0},	//[11:0]
	/*G2D_SCL_OFOFT_OVOFFSET		  */{rG2D_SCL_OFOFT 	  , 12, 16},	//[27:16]
	/*G2D_SCL_OFOFT_OHOFFSET		  */{rG2D_SCL_OFOFT 	  , 12,  0},	//[11:0]
	/*G2D_SCL_SCR_VROUND			  */{rG2D_SCL_SCR		  ,  1, 31},	//[31:31]
	/*G2D_SCL_SCR_VPASSBY			  */{rG2D_SCL_SCR		  ,  1, 30},	//[30:30]
	/*G2D_SCL_SCR_VSCALING			  */{rG2D_SCL_SCR		  , 14, 16},	//[29:16]
	/*G2D_SCL_SCR_HROUND			  */{rG2D_SCL_SCR		  ,  1, 15},	//[15:15]
	/*G2D_SCL_SCR_HPASSBY			  */{rG2D_SCL_SCR		  ,  1, 14},	//[14:14]
	/*G2D_SCL_SCR_HSCALING			  */{rG2D_SCL_SCR		  , 14,  0},	//[13:0]
	/*G2D_EE_CNTL_FIRST 			  */{rG2D_EE_CNTL		  ,  1,  3},	//[3:3]
	/*G2D_EE_CNTL_DENOISE_EN		  */{rG2D_EE_CNTL		  ,  1,  2},	//[2:2]
	/*G2D_EE_CNTL_ROUND 			  */{rG2D_EE_CNTL		  ,  1,  1},	//[1:1]
	/*G2D_EE_CNTL_BYPASS			  */{rG2D_EE_CNTL		  ,  1,  0},	//[0:0]
	/*G2D_EE_COEF_ERR				  */{rG2D_EE_COEF		  , 12, 16},	//[27:16]
	/*G2D_EE_COEF_COEFA 			  */{rG2D_EE_COEF		  ,  8,  8},	//[15:8]
	/*G2D_EE_COEF_COEFW 			  */{rG2D_EE_COEF		  ,  8,  0},	//[7:0]
	/*G2D_EE_THRE_H 				  */{rG2D_EE_THRE		  ,  8, 24},	//[31:24]
	/*G2D_EE_THRE_V 				  */{rG2D_EE_THRE		  ,  8, 16},	//[23:16]
	/*G2D_EE_THRE_D0				  */{rG2D_EE_THRE		  ,  8,  8},	//[15:8]
	/*G2D_EE_THRE_D1				  */{rG2D_EE_THRE		  ,  8,  0},	//[7:0]
	/*G2D_DIT_FRAME_SIZE_VRES		  */{rG2D_DIT_FRAME_SIZE  , 12, 16},	//[27:16]
	/*G2D_DIT_FRAME_SIZE_HRES		  */{rG2D_DIT_FRAME_SIZE  , 12,  0},	//[11:0]
	/*G2D_DIT_DI_CNTL_RNB			  */{rG2D_DIT_DI_CNTL	  ,  3,  9},	//[11:9]
	/*G2D_DIT_DI_CNTL_GNB			  */{rG2D_DIT_DI_CNTL	  ,  3,  6},	//[8:6]
	/*G2D_DIT_DI_CNTL_BNB			  */{rG2D_DIT_DI_CNTL	  ,  3,  3},	//[5:3]
	/*G2D_DIT_DI_CNTL_BYPASS		  */{rG2D_DIT_DI_CNTL	  ,  1,  2},	//[2:2]
	/*G2D_DIT_DI_CNTL_TEMPO 		  */{rG2D_DIT_DI_CNTL	  ,  1,  0},	//[0:0]
	/*G2D_EE_MAT0_HM00				  */{rG2D_EE_MAT0		  ,  3, 24},	//[26:24]
	/*G2D_EE_MAT0_HM01				  */{rG2D_EE_MAT0		  ,  3, 21},	//[23:21]
	/*G2D_EE_MAT0_HM02				  */{rG2D_EE_MAT0		  ,  3, 18},	//[20:18]
	/*G2D_EE_MAT0_HM10				  */{rG2D_EE_MAT0		  ,  3, 15},	//[17:15]
	/*G2D_EE_MAT0_HM11				  */{rG2D_EE_MAT0		  ,  3, 12},	//[14:12]
	/*G2D_EE_MAT0_HM12				  */{rG2D_EE_MAT0		  ,  3,  9},	//[11:9]
	/*G2D_EE_MAT0_HM20				  */{rG2D_EE_MAT0		  ,  3,  6},	//[8:6]
	/*G2D_EE_MAT0_HM21				  */{rG2D_EE_MAT0		  ,  3,  3},	//[5:3]
	/*G2D_EE_MAT0_HM22				  */{rG2D_EE_MAT0		  ,  3,  0},	//[2:0]
	/*G2D_EE_MAT1_VM00				  */{rG2D_EE_MAT1		  ,  3, 24},	//[26:24]
	/*G2D_EE_MAT1_VM01				  */{rG2D_EE_MAT1		  ,  3, 21},	//[23:21]
	/*G2D_EE_MAT1_VM02				  */{rG2D_EE_MAT1		  ,  3, 18},	//[20:18]
	/*G2D_EE_MAT1_VM10				  */{rG2D_EE_MAT1		  ,  3, 15},	//[17:15]
	/*G2D_EE_MAT1_VM11				  */{rG2D_EE_MAT1		  ,  3, 12},	//[14:12]
	/*G2D_EE_MAT1_VM12				  */{rG2D_EE_MAT1		  ,  3,  9},	//[11:9]
	/*G2D_EE_MAT1_VM20				  */{rG2D_EE_MAT1		  ,  3,  6},	//[8:6]
	/*G2D_EE_MAT1_VM21				  */{rG2D_EE_MAT1		  ,  3,  3},	//[5:3]
	/*G2D_EE_MAT1_VM22				  */{rG2D_EE_MAT1		  ,  3,  0},	//[2:0]
	/*G2D_EE_MAT2_D0M00 			  */{rG2D_EE_MAT2		  ,  3, 24},	//[26:24]
	/*G2D_EE_MAT2_D0M01 			  */{rG2D_EE_MAT2		  ,  3, 21},	//[23:21]
	/*G2D_EE_MAT2_D0M02 			  */{rG2D_EE_MAT2		  ,  3, 18},	//[20:18]
	/*G2D_EE_MAT2_D0M10 			  */{rG2D_EE_MAT2		  ,  3, 15},	//[17:15]
	/*G2D_EE_MAT2_D0M11 			  */{rG2D_EE_MAT2		  ,  3, 12},	//[14:12]
	/*G2D_EE_MAT2_D0M12 			  */{rG2D_EE_MAT2		  ,  3,  9},	//[11:9]
	/*G2D_EE_MAT2_D0M20 			  */{rG2D_EE_MAT2		  ,  3,  6},	//[8:6]
	/*G2D_EE_MAT2_D0M21 			  */{rG2D_EE_MAT2		  ,  3,  3},	//[5:3]
	/*G2D_EE_MAT2_D0M22 			  */{rG2D_EE_MAT2		  ,  3,  0},	//[2:0]
	/*G2D_EE_MAT3_D1M00 			  */{rG2D_EE_MAT3		  ,  3, 24},	//[26:24]
	/*G2D_EE_MAT3_D1M01 			  */{rG2D_EE_MAT3		  ,  3, 21},	//[23:21]
	/*G2D_EE_MAT3_D1M02 			  */{rG2D_EE_MAT3		  ,  3, 18},	//[20:18]
	/*G2D_EE_MAT3_D1M10 			  */{rG2D_EE_MAT3		  ,  3, 15},	//[17:15]
	/*G2D_EE_MAT3_D1M11 			  */{rG2D_EE_MAT3		  ,  3, 12},	//[14:12]
	/*G2D_EE_MAT3_D1M12 			  */{rG2D_EE_MAT3		  ,  3,  9},	//[11:9]
	/*G2D_EE_MAT3_D1M20 			  */{rG2D_EE_MAT3		  ,  3,  6},	//[8:6]
	/*G2D_EE_MAT3_D1M21 			  */{rG2D_EE_MAT3		  ,  3,  3},	//[5:3]
	/*G2D_EE_MAT3_D1M22 			  */{rG2D_EE_MAT3		  ,  3,  0},	//[2:0]
	/*G2D_EE_MAT4_GAM11_HIGH		  */{rG2D_EE_MAT4		  ,  1, 27},	//[27:27]
	/*G2D_EE_MAT4_GAM00 			  */{rG2D_EE_MAT4		  ,  3, 25},	//[26:24]
	/*G2D_EE_MAT4_GAM01 			  */{rG2D_EE_MAT4		  ,  3, 22},	//[23:21]
	/*G2D_EE_MAT4_GAM02 			  */{rG2D_EE_MAT4		  ,  3, 19},	//[20:18]
	/*G2D_EE_MAT4_GAM10 			  */{rG2D_EE_MAT4		  ,  3, 16},	//[17:15]
	/*G2D_EE_MAT4_GAM11_LOW 		  */{rG2D_EE_MAT4		  ,  3, 12},	//[14:12]
	/*G2D_EE_MAT4_GAM12 			  */{rG2D_EE_MAT4		  ,  3,  9},	//[11:9]
	/*G2D_EE_MAT4_GAM20 			  */{rG2D_EE_MAT4		  ,  3,  6},	//[8:6]
	/*G2D_EE_MAT4_GAM21 			  */{rG2D_EE_MAT4		  ,  3,  3},	//[5:3]
	/*G2D_EE_MAT4_GAM22 			  */{rG2D_EE_MAT4		  ,  3,  0},	//[2:0]
	/*G2D_SCL_HSCR0_0				  */{rG2D_SCL_HSCR0_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_1				  */{rG2D_SCL_HSCR0_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_2				  */{rG2D_SCL_HSCR0_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_3				  */{rG2D_SCL_HSCR0_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_4				  */{rG2D_SCL_HSCR0_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_5				  */{rG2D_SCL_HSCR0_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_6				  */{rG2D_SCL_HSCR0_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_7				  */{rG2D_SCL_HSCR0_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_8				  */{rG2D_SCL_HSCR0_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_9				  */{rG2D_SCL_HSCR0_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_10				  */{rG2D_SCL_HSCR0_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_11				  */{rG2D_SCL_HSCR0_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_12				  */{rG2D_SCL_HSCR0_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_13				  */{rG2D_SCL_HSCR0_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_14				  */{rG2D_SCL_HSCR0_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_15				  */{rG2D_SCL_HSCR0_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_16				  */{rG2D_SCL_HSCR0_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_17				  */{rG2D_SCL_HSCR0_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_18				  */{rG2D_SCL_HSCR0_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_19				  */{rG2D_SCL_HSCR0_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_20				  */{rG2D_SCL_HSCR0_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_21				  */{rG2D_SCL_HSCR0_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_22				  */{rG2D_SCL_HSCR0_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_23				  */{rG2D_SCL_HSCR0_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_24				  */{rG2D_SCL_HSCR0_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_25				  */{rG2D_SCL_HSCR0_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_26				  */{rG2D_SCL_HSCR0_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_27				  */{rG2D_SCL_HSCR0_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_28				  */{rG2D_SCL_HSCR0_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_29				  */{rG2D_SCL_HSCR0_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_30				  */{rG2D_SCL_HSCR0_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_31				  */{rG2D_SCL_HSCR0_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_32				  */{rG2D_SCL_HSCR0_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_33				  */{rG2D_SCL_HSCR0_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_34				  */{rG2D_SCL_HSCR0_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_35				  */{rG2D_SCL_HSCR0_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_36				  */{rG2D_SCL_HSCR0_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_37				  */{rG2D_SCL_HSCR0_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_38				  */{rG2D_SCL_HSCR0_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_39				  */{rG2D_SCL_HSCR0_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_40				  */{rG2D_SCL_HSCR0_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_41				  */{rG2D_SCL_HSCR0_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_42				  */{rG2D_SCL_HSCR0_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_43				  */{rG2D_SCL_HSCR0_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_44				  */{rG2D_SCL_HSCR0_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_45				  */{rG2D_SCL_HSCR0_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_46				  */{rG2D_SCL_HSCR0_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_47				  */{rG2D_SCL_HSCR0_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_48				  */{rG2D_SCL_HSCR0_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_49				  */{rG2D_SCL_HSCR0_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_50				  */{rG2D_SCL_HSCR0_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_51				  */{rG2D_SCL_HSCR0_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_52				  */{rG2D_SCL_HSCR0_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_53				  */{rG2D_SCL_HSCR0_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_54				  */{rG2D_SCL_HSCR0_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_55				  */{rG2D_SCL_HSCR0_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_56				  */{rG2D_SCL_HSCR0_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_57				  */{rG2D_SCL_HSCR0_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_58				  */{rG2D_SCL_HSCR0_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_59				  */{rG2D_SCL_HSCR0_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_60				  */{rG2D_SCL_HSCR0_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_61				  */{rG2D_SCL_HSCR0_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_62				  */{rG2D_SCL_HSCR0_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR0_63				  */{rG2D_SCL_HSCR0_63	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_0				  */{rG2D_SCL_HSCR1_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_1				  */{rG2D_SCL_HSCR1_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_2				  */{rG2D_SCL_HSCR1_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_3				  */{rG2D_SCL_HSCR1_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_4				  */{rG2D_SCL_HSCR1_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_5				  */{rG2D_SCL_HSCR1_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_6				  */{rG2D_SCL_HSCR1_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_7				  */{rG2D_SCL_HSCR1_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_8				  */{rG2D_SCL_HSCR1_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_9				  */{rG2D_SCL_HSCR1_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_10				  */{rG2D_SCL_HSCR1_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_11				  */{rG2D_SCL_HSCR1_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_12				  */{rG2D_SCL_HSCR1_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_13				  */{rG2D_SCL_HSCR1_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_14				  */{rG2D_SCL_HSCR1_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_15				  */{rG2D_SCL_HSCR1_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_16				  */{rG2D_SCL_HSCR1_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_17				  */{rG2D_SCL_HSCR1_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_18				  */{rG2D_SCL_HSCR1_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_19				  */{rG2D_SCL_HSCR1_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_20				  */{rG2D_SCL_HSCR1_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_21				  */{rG2D_SCL_HSCR1_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_22				  */{rG2D_SCL_HSCR1_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_23				  */{rG2D_SCL_HSCR1_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_24				  */{rG2D_SCL_HSCR1_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_25				  */{rG2D_SCL_HSCR1_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_26				  */{rG2D_SCL_HSCR1_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_27				  */{rG2D_SCL_HSCR1_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_28				  */{rG2D_SCL_HSCR1_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_29				  */{rG2D_SCL_HSCR1_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_30				  */{rG2D_SCL_HSCR1_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_31				  */{rG2D_SCL_HSCR1_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_32				  */{rG2D_SCL_HSCR1_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_33				  */{rG2D_SCL_HSCR1_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_34				  */{rG2D_SCL_HSCR1_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_35				  */{rG2D_SCL_HSCR1_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_36				  */{rG2D_SCL_HSCR1_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_37				  */{rG2D_SCL_HSCR1_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_38				  */{rG2D_SCL_HSCR1_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_39				  */{rG2D_SCL_HSCR1_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_40				  */{rG2D_SCL_HSCR1_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_41				  */{rG2D_SCL_HSCR1_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_42				  */{rG2D_SCL_HSCR1_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_43				  */{rG2D_SCL_HSCR1_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_44				  */{rG2D_SCL_HSCR1_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_45				  */{rG2D_SCL_HSCR1_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_46				  */{rG2D_SCL_HSCR1_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_47				  */{rG2D_SCL_HSCR1_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_48				  */{rG2D_SCL_HSCR1_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_49				  */{rG2D_SCL_HSCR1_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_50				  */{rG2D_SCL_HSCR1_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_51				  */{rG2D_SCL_HSCR1_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_52				  */{rG2D_SCL_HSCR1_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_53				  */{rG2D_SCL_HSCR1_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_54				  */{rG2D_SCL_HSCR1_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_55				  */{rG2D_SCL_HSCR1_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_56				  */{rG2D_SCL_HSCR1_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_57				  */{rG2D_SCL_HSCR1_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_58				  */{rG2D_SCL_HSCR1_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_59				  */{rG2D_SCL_HSCR1_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_60				  */{rG2D_SCL_HSCR1_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_61				  */{rG2D_SCL_HSCR1_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_62				  */{rG2D_SCL_HSCR1_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR1_63				  */{rG2D_SCL_HSCR1_63	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_0				  */{rG2D_SCL_HSCR2_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_1				  */{rG2D_SCL_HSCR2_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_2				  */{rG2D_SCL_HSCR2_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_3				  */{rG2D_SCL_HSCR2_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_4				  */{rG2D_SCL_HSCR2_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_5				  */{rG2D_SCL_HSCR2_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_6				  */{rG2D_SCL_HSCR2_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_7				  */{rG2D_SCL_HSCR2_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_8				  */{rG2D_SCL_HSCR2_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_9				  */{rG2D_SCL_HSCR2_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_10				  */{rG2D_SCL_HSCR2_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_11				  */{rG2D_SCL_HSCR2_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_12				  */{rG2D_SCL_HSCR2_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_13				  */{rG2D_SCL_HSCR2_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_14				  */{rG2D_SCL_HSCR2_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_15				  */{rG2D_SCL_HSCR2_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_16				  */{rG2D_SCL_HSCR2_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_17				  */{rG2D_SCL_HSCR2_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_18				  */{rG2D_SCL_HSCR2_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_19				  */{rG2D_SCL_HSCR2_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_20				  */{rG2D_SCL_HSCR2_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_21				  */{rG2D_SCL_HSCR2_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_22				  */{rG2D_SCL_HSCR2_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_23				  */{rG2D_SCL_HSCR2_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_24				  */{rG2D_SCL_HSCR2_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_25				  */{rG2D_SCL_HSCR2_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_26				  */{rG2D_SCL_HSCR2_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_27				  */{rG2D_SCL_HSCR2_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_28				  */{rG2D_SCL_HSCR2_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_29				  */{rG2D_SCL_HSCR2_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_30				  */{rG2D_SCL_HSCR2_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_31				  */{rG2D_SCL_HSCR2_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_32				  */{rG2D_SCL_HSCR2_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_33				  */{rG2D_SCL_HSCR2_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_34				  */{rG2D_SCL_HSCR2_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_35				  */{rG2D_SCL_HSCR2_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_36				  */{rG2D_SCL_HSCR2_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_37				  */{rG2D_SCL_HSCR2_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_38				  */{rG2D_SCL_HSCR2_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_39				  */{rG2D_SCL_HSCR2_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_40				  */{rG2D_SCL_HSCR2_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_41				  */{rG2D_SCL_HSCR2_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_42				  */{rG2D_SCL_HSCR2_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_43				  */{rG2D_SCL_HSCR2_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_44				  */{rG2D_SCL_HSCR2_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_45				  */{rG2D_SCL_HSCR2_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_46				  */{rG2D_SCL_HSCR2_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_47				  */{rG2D_SCL_HSCR2_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_48				  */{rG2D_SCL_HSCR2_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_49				  */{rG2D_SCL_HSCR2_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_50				  */{rG2D_SCL_HSCR2_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_51				  */{rG2D_SCL_HSCR2_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_52				  */{rG2D_SCL_HSCR2_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_53				  */{rG2D_SCL_HSCR2_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_54				  */{rG2D_SCL_HSCR2_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_55				  */{rG2D_SCL_HSCR2_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_56				  */{rG2D_SCL_HSCR2_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_57				  */{rG2D_SCL_HSCR2_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_58				  */{rG2D_SCL_HSCR2_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_59				  */{rG2D_SCL_HSCR2_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_60				  */{rG2D_SCL_HSCR2_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_61				  */{rG2D_SCL_HSCR2_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_62				  */{rG2D_SCL_HSCR2_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR2_63				  */{rG2D_SCL_HSCR2_63	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_0				  */{rG2D_SCL_HSCR3_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_1				  */{rG2D_SCL_HSCR3_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_2				  */{rG2D_SCL_HSCR3_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_3				  */{rG2D_SCL_HSCR3_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_4				  */{rG2D_SCL_HSCR3_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_5				  */{rG2D_SCL_HSCR3_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_6				  */{rG2D_SCL_HSCR3_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_7				  */{rG2D_SCL_HSCR3_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_8				  */{rG2D_SCL_HSCR3_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_9				  */{rG2D_SCL_HSCR3_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_10				  */{rG2D_SCL_HSCR3_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_11				  */{rG2D_SCL_HSCR3_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_12				  */{rG2D_SCL_HSCR3_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_13				  */{rG2D_SCL_HSCR3_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_14				  */{rG2D_SCL_HSCR3_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_15				  */{rG2D_SCL_HSCR3_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_16				  */{rG2D_SCL_HSCR3_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_17				  */{rG2D_SCL_HSCR3_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_18				  */{rG2D_SCL_HSCR3_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_19				  */{rG2D_SCL_HSCR3_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_20				  */{rG2D_SCL_HSCR3_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_21				  */{rG2D_SCL_HSCR3_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_22				  */{rG2D_SCL_HSCR3_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_23				  */{rG2D_SCL_HSCR3_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_24				  */{rG2D_SCL_HSCR3_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_25				  */{rG2D_SCL_HSCR3_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_26				  */{rG2D_SCL_HSCR3_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_27				  */{rG2D_SCL_HSCR3_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_28				  */{rG2D_SCL_HSCR3_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_29				  */{rG2D_SCL_HSCR3_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_30				  */{rG2D_SCL_HSCR3_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_31				  */{rG2D_SCL_HSCR3_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_32				  */{rG2D_SCL_HSCR3_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_33				  */{rG2D_SCL_HSCR3_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_34				  */{rG2D_SCL_HSCR3_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_35				  */{rG2D_SCL_HSCR3_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_36				  */{rG2D_SCL_HSCR3_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_37				  */{rG2D_SCL_HSCR3_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_38				  */{rG2D_SCL_HSCR3_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_39				  */{rG2D_SCL_HSCR3_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_40				  */{rG2D_SCL_HSCR3_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_41				  */{rG2D_SCL_HSCR3_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_42				  */{rG2D_SCL_HSCR3_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_43				  */{rG2D_SCL_HSCR3_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_44				  */{rG2D_SCL_HSCR3_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_45				  */{rG2D_SCL_HSCR3_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_46				  */{rG2D_SCL_HSCR3_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_47				  */{rG2D_SCL_HSCR3_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_48				  */{rG2D_SCL_HSCR3_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_49				  */{rG2D_SCL_HSCR3_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_50				  */{rG2D_SCL_HSCR3_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_51				  */{rG2D_SCL_HSCR3_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_52				  */{rG2D_SCL_HSCR3_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_53				  */{rG2D_SCL_HSCR3_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_54				  */{rG2D_SCL_HSCR3_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_55				  */{rG2D_SCL_HSCR3_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_56				  */{rG2D_SCL_HSCR3_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_57				  */{rG2D_SCL_HSCR3_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_58				  */{rG2D_SCL_HSCR3_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_59				  */{rG2D_SCL_HSCR3_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_60				  */{rG2D_SCL_HSCR3_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_61				  */{rG2D_SCL_HSCR3_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_62				  */{rG2D_SCL_HSCR3_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_HSCR3_63				  */{rG2D_SCL_HSCR3_63	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_0				  */{rG2D_SCL_VSCR0_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_1				  */{rG2D_SCL_VSCR0_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_2				  */{rG2D_SCL_VSCR0_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_3				  */{rG2D_SCL_VSCR0_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_4				  */{rG2D_SCL_VSCR0_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_5				  */{rG2D_SCL_VSCR0_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_6				  */{rG2D_SCL_VSCR0_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_7				  */{rG2D_SCL_VSCR0_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_8				  */{rG2D_SCL_VSCR0_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_9				  */{rG2D_SCL_VSCR0_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_10				  */{rG2D_SCL_VSCR0_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_11				  */{rG2D_SCL_VSCR0_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_12				  */{rG2D_SCL_VSCR0_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_13				  */{rG2D_SCL_VSCR0_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_14				  */{rG2D_SCL_VSCR0_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_15				  */{rG2D_SCL_VSCR0_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_16				  */{rG2D_SCL_VSCR0_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_17				  */{rG2D_SCL_VSCR0_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_18				  */{rG2D_SCL_VSCR0_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_19				  */{rG2D_SCL_VSCR0_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_20				  */{rG2D_SCL_VSCR0_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_21				  */{rG2D_SCL_VSCR0_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_22				  */{rG2D_SCL_VSCR0_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_23				  */{rG2D_SCL_VSCR0_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_24				  */{rG2D_SCL_VSCR0_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_25				  */{rG2D_SCL_VSCR0_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_26				  */{rG2D_SCL_VSCR0_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_27				  */{rG2D_SCL_VSCR0_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_28				  */{rG2D_SCL_VSCR0_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_29				  */{rG2D_SCL_VSCR0_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_30				  */{rG2D_SCL_VSCR0_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_31				  */{rG2D_SCL_VSCR0_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_32				  */{rG2D_SCL_VSCR0_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_33				  */{rG2D_SCL_VSCR0_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_34				  */{rG2D_SCL_VSCR0_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_35				  */{rG2D_SCL_VSCR0_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_36				  */{rG2D_SCL_VSCR0_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_37				  */{rG2D_SCL_VSCR0_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_38				  */{rG2D_SCL_VSCR0_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_39				  */{rG2D_SCL_VSCR0_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_40				  */{rG2D_SCL_VSCR0_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_41				  */{rG2D_SCL_VSCR0_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_42				  */{rG2D_SCL_VSCR0_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_43				  */{rG2D_SCL_VSCR0_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_44				  */{rG2D_SCL_VSCR0_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_45				  */{rG2D_SCL_VSCR0_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_46				  */{rG2D_SCL_VSCR0_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_47				  */{rG2D_SCL_VSCR0_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_48				  */{rG2D_SCL_VSCR0_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_49				  */{rG2D_SCL_VSCR0_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_50				  */{rG2D_SCL_VSCR0_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_51				  */{rG2D_SCL_VSCR0_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_52				  */{rG2D_SCL_VSCR0_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_53				  */{rG2D_SCL_VSCR0_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_54				  */{rG2D_SCL_VSCR0_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_55				  */{rG2D_SCL_VSCR0_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_56				  */{rG2D_SCL_VSCR0_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_57				  */{rG2D_SCL_VSCR0_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_58				  */{rG2D_SCL_VSCR0_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_59				  */{rG2D_SCL_VSCR0_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_60				  */{rG2D_SCL_VSCR0_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_61				  */{rG2D_SCL_VSCR0_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_62				  */{rG2D_SCL_VSCR0_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR0_63				  */{rG2D_SCL_VSCR0_63	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_0				  */{rG2D_SCL_VSCR1_0	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_1				  */{rG2D_SCL_VSCR1_1	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_2				  */{rG2D_SCL_VSCR1_2	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_3				  */{rG2D_SCL_VSCR1_3	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_4				  */{rG2D_SCL_VSCR1_4	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_5				  */{rG2D_SCL_VSCR1_5	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_6				  */{rG2D_SCL_VSCR1_6	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_7				  */{rG2D_SCL_VSCR1_7	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_8				  */{rG2D_SCL_VSCR1_8	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_9				  */{rG2D_SCL_VSCR1_9	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_10				  */{rG2D_SCL_VSCR1_10	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_11				  */{rG2D_SCL_VSCR1_11	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_12				  */{rG2D_SCL_VSCR1_12	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_13				  */{rG2D_SCL_VSCR1_13	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_14				  */{rG2D_SCL_VSCR1_14	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_15				  */{rG2D_SCL_VSCR1_15	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_16				  */{rG2D_SCL_VSCR1_16	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_17				  */{rG2D_SCL_VSCR1_17	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_18				  */{rG2D_SCL_VSCR1_18	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_19				  */{rG2D_SCL_VSCR1_19	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_20				  */{rG2D_SCL_VSCR1_20	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_21				  */{rG2D_SCL_VSCR1_21	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_22				  */{rG2D_SCL_VSCR1_22	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_23				  */{rG2D_SCL_VSCR1_23	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_24				  */{rG2D_SCL_VSCR1_24	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_25				  */{rG2D_SCL_VSCR1_25	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_26				  */{rG2D_SCL_VSCR1_26	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_27				  */{rG2D_SCL_VSCR1_27	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_28				  */{rG2D_SCL_VSCR1_28	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_29				  */{rG2D_SCL_VSCR1_29	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_30				  */{rG2D_SCL_VSCR1_30	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_31				  */{rG2D_SCL_VSCR1_31	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_32				  */{rG2D_SCL_VSCR1_32	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_33				  */{rG2D_SCL_VSCR1_33	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_34				  */{rG2D_SCL_VSCR1_34	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_35				  */{rG2D_SCL_VSCR1_35	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_36				  */{rG2D_SCL_VSCR1_36	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_37				  */{rG2D_SCL_VSCR1_37	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_38				  */{rG2D_SCL_VSCR1_38	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_39				  */{rG2D_SCL_VSCR1_39	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_40				  */{rG2D_SCL_VSCR1_40	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_41				  */{rG2D_SCL_VSCR1_41	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_42				  */{rG2D_SCL_VSCR1_42	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_43				  */{rG2D_SCL_VSCR1_43	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_44				  */{rG2D_SCL_VSCR1_44	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_45				  */{rG2D_SCL_VSCR1_45	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_46				  */{rG2D_SCL_VSCR1_46	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_47				  */{rG2D_SCL_VSCR1_47	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_48				  */{rG2D_SCL_VSCR1_48	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_49				  */{rG2D_SCL_VSCR1_49	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_50				  */{rG2D_SCL_VSCR1_50	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_51				  */{rG2D_SCL_VSCR1_51	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_52				  */{rG2D_SCL_VSCR1_52	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_53				  */{rG2D_SCL_VSCR1_53	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_54				  */{rG2D_SCL_VSCR1_54	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_55				  */{rG2D_SCL_VSCR1_55	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_56				  */{rG2D_SCL_VSCR1_56	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_57				  */{rG2D_SCL_VSCR1_57	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_58				  */{rG2D_SCL_VSCR1_58	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_59				  */{rG2D_SCL_VSCR1_59	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_60				  */{rG2D_SCL_VSCR1_60	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_61				  */{rG2D_SCL_VSCR1_61	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_62				  */{rG2D_SCL_VSCR1_62	  , 12,  0},	//[11:0]
	/*G2D_SCL_VSCR1_63				  */{rG2D_SCL_VSCR1_63	  , 12,  0},	//[11:0]
	/*G2D_DIT_DM_COEF_0003_COEF00	  */{rG2D_DIT_DM_COEF_0003,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_0003_COEF01	  */{rG2D_DIT_DM_COEF_0003,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_0003_COEF02	  */{rG2D_DIT_DM_COEF_0003,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_0003_COEF03	  */{rG2D_DIT_DM_COEF_0003,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_0407_COEF04	  */{rG2D_DIT_DM_COEF_0407,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_0407_COEF05	  */{rG2D_DIT_DM_COEF_0407,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_0407_COEF06	  */{rG2D_DIT_DM_COEF_0407,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_0407_COEF07	  */{rG2D_DIT_DM_COEF_0407,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_1013_COEF10	  */{rG2D_DIT_DM_COEF_1013,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_1013_COEF11	  */{rG2D_DIT_DM_COEF_1013,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_1013_COEF12	  */{rG2D_DIT_DM_COEF_1013,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_1013_COEF13	  */{rG2D_DIT_DM_COEF_1013,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_1417_COEF14	  */{rG2D_DIT_DM_COEF_1417,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_1417_COEF15	  */{rG2D_DIT_DM_COEF_1417,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_1417_COEF16	  */{rG2D_DIT_DM_COEF_1417,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_1417_COEF17	  */{rG2D_DIT_DM_COEF_1417,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_2023_COEF20	  */{rG2D_DIT_DM_COEF_2023,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_2023_COEF21	  */{rG2D_DIT_DM_COEF_2023,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_2023_COEF22	  */{rG2D_DIT_DM_COEF_2023,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_2023_COEF23	  */{rG2D_DIT_DM_COEF_2023,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_2427_COEF24	  */{rG2D_DIT_DM_COEF_2427,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_2427_COEF25	  */{rG2D_DIT_DM_COEF_2427,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_2427_COEF26	  */{rG2D_DIT_DM_COEF_2427,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_2427_COEF27	  */{rG2D_DIT_DM_COEF_2427,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_3033_COEF30	  */{rG2D_DIT_DM_COEF_3033,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_3033_COEF31	  */{rG2D_DIT_DM_COEF_3033,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_3033_COEF32	  */{rG2D_DIT_DM_COEF_3033,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_3033_COEF33	  */{rG2D_DIT_DM_COEF_3033,  5,  0},	//[4:0]
	/*G2D_DIT_DM_COEF_3437_COEF34	  */{rG2D_DIT_DM_COEF_3437,  5, 24},	//[28:24]
	/*G2D_DIT_DM_COEF_3437_COEF35	  */{rG2D_DIT_DM_COEF_3437,  5, 16},	//[20:16]
	/*G2D_DIT_DM_COEF_3437_COEF36	  */{rG2D_DIT_DM_COEF_3437,  5,  8},	//[12:8]
	/*G2D_DIT_DM_COEF_3437_COEF37	  */{rG2D_DIT_DM_COEF_3437,  5,  0},	//[4:0]
	/*G2D_DIT_DE_COEF_COEF12		  */{rG2D_DIT_DE_COEF	  ,  5, 24},	//[28:24]
	/*G2D_DIT_DE_COEF_COEF20		  */{rG2D_DIT_DE_COEF	  ,  5, 16},	//[20:16]
	/*G2D_DIT_DE_COEF_COEF21		  */{rG2D_DIT_DE_COEF	  ,  5,  8},	//[12:8]
	/*G2D_DIT_DE_COEF_COEF22		  */{rG2D_DIT_DE_COEF	  ,  5,  0},	//[4:0]

};


static const unsigned int regMask[33] = { 0x00000000,
    0x00000001, 0x00000003, 0x00000007, 0x0000000F,
    0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
    0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
    0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
    0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
    0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
    0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
    0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};


//for G2D_EE_GAUSS_MODE_0(use default mode)
static int GASMat0[9] = {
	 2,  4,  2,
	 4,  8,  4,
	 2,  4,  2
};

#if 0
static int GASMat1[9] = {
	4,  4,  4,
	4,  0,  4,
	4,  4,  4
};
#endif

/*{coef00,coef01,coef02,coef10,coef11,coef12,coef20,coef21,coef22}*/
//for G2D_EE_OPMAT_TYPE_0(use default coef)
static int OpMat0[4*9] = {
	-1, -2, -1,  0,  0,  0,  1,  2,  1, //HMat
	-1,  0,  1, -2,  0,  2, -1,  0,  1, //VMat
	-2, -1,  0, -1,  0,  1,  0,  1,  2,	//D0Mat
	 0, -1, -2,  1,  0, -1,  2,  1,  0	//D1Mat
};


static const int scl_params[6*64] = { 
	/*HCOEF0*/
	0,
	0,
	0,
	0,
	0,
	0,
	-100,
	-112,
	0,
	0,
	-141,
	-149,
	0,
	0,
	-166,
	-169,
	-172,
	0,
	0,
	-175,
	-175,
	0,
	-171,
	-169,
	-166,
	-162,
	-158,
	-154,
	-149,
	-144,
	-139,
	-133,
	-128,
	-122,
	-116,
	-110,
	-104,
	-97,
	-91,
	-86,
	-80,
	-74,
	-68,
	-62,
	-56,
	-51,
	-45,
	-41,
	-37,
	-32,
	-28,
	-24,
	-20,
	-17,
	-14,
	0,
	-9,
	-7,
	-5,
	-3,
	0,
	-1,
	-1,
	0,

	/*HCOEF1*/
	2047,
	2047,
	2047,
	2047,
	2047,
	2047,
	2008,
	1993,
	2047,
	2047,
	1937,
	1916,
	2047,
	2047,
	1839,
	1809,
	1780,
	2047,
	2047,
	1680,
	1644,
	2047,
	1569,
	1531,
	1491,
	1450,
	1408,
	1366,
	1325,
	1282,
	1239,
	1195,
	1152,
	1108,
	1064,
	1020,
	976,
	932,
	889,
	846,
	803,
	760,
	718,
	677,
	635,
	594,
	554,
	516,
	477,
	440,
	403,
	368,
	332,
	298,
	265,
	21,
	203,
	174,
	145,
	118,
	21,
	67,
	43,
	21,

	/*HCOEF2*/
	1,
	1,
	1,
	1,
	1,
	1,
	145,
	174,
	1,
	1,
	266,
	298,
	1,
	1,
	403,
	440,
	477,
	1,
	1,
	594,
	635,
	1,
	718,
	760,
	803,
	846,
	889,
	933,
	976,
	1020,
	1064,
	1108,
	1152,
	1195,
	1239,
	1282,
	1325,
	1367,
	1408,
	1450,
	1491,
	1531,
	1569,
	1607,
	1644,
	1680,
	1714,
	1747,
	1780,
	1809,
	1839,
	1865,
	1891,
	1916,
	1938,
	2047,
	1977,
	1993,
	2008,
	2019,
	2047,
	2037,
	2044,
	2047,

	/*HCOEF3*/
	0,
	0,
	0,
	0,
	0,
	0,
	-5,
	-7,
	0,
	0,
	-14,
	-17,
	0,
	0,
	-28,
	-32,
	-37,
	0,
	0,
	-51,
	-56,
	0,
	-68,
	-74,
	-80,
	-86,
	-91,
	-97,
	-104,
	-110,
	-116,
	-122,
	-128,
	-133,
	-139,
	-144,
	-149,
	-154,
	-158,
	-162,
	-166,
	-169,
	-171,
	-174,
	-175,
	-175,
	-175,
	-174,
	-172,
	-169,
	-166,
	-161,
	-155,
	-149,
	-141,
	-20,
	-123,
	-112,
	-100,
	-86,
	-20,
	-55,
	-38,
	-20,

	/*VCOEF0*/
	2047,
	2016,
	1984,
	1952,
	1920,
	1888,
	1856,
	1824,
	1792,
	1760,
	1728,
	1696,
	1664,
	1632,
	1600,
	1568,
	1536,
	1504,
	1472,
	1440,
	1408,
	1376,
	1344,
	1312,
	1280,
	1248,
	1216,
	1184,
	1152,
	1120,
	1088,
	1056,
	1024,
	992,
	960,
	928,
	896,
	864,
	832,
	800,
	768,
	736,
	704,
	672,
	640,
	608,
	576,
	544,
	512,
	480,
	448,
	416,
	384,
	352,
	320,
	288,
	256,
	224,
	192,
	160,
	128,
	96,
	64,
	32,

	/*VCOEF1*/
	1,
	32,
	64,
	96,
	128,
	160,
	192,
	224,
	256,
	288,
	320,
	352,
	384,
	416,
	448,
	480,
	512,
	544,
	576,
	608,
	640,
	672,
	704,
	736,
	768,
	800,
	832,
	864,
	896,
	928,
	960,
	992,
	1024,
	1056,
	1088,
	1120,
	1152,
	1184,
	1216,
	1248,
	1280,
	1312,
	1344,
	1376,
	1408,
	1440,
	1472,
	1504,
	1536,
	1568,
	1600,
	1632,
	1664,
	1696,
	1728,
	1760,
	1792,
	1824,
	1856,
	1888,
	1920,
	1952,
	1984,
	2016,
};


//for G2D_DITH_COEF_TYPE_0(default coef)
static unsigned int dithMat0[36] = {
0x00, 0x10, 0x08, 0x18,	//coef00, coef01,coef02, coef03
0x02, 0x12, 0x0a, 0x1a,	//coef04, coef05,coef06, coef07
0x0c, 0x1c, 0x04, 0x14,	//coef10, coef11,coef12, coef13
0x0e, 0x1e, 0x06, 0x16,	//coef14, coef15,coef16, coef17
0x03, 0x13, 0x0b, 0x1b,	//coef20, coef21,coef22, coef23
0x01, 0x11, 0x09, 0x19,	//coef24, coef25,coef26, coef27
0x0f, 0x1f, 0x07, 0x17,	//coef30, coef31,coef32, coef33
0x0d, 0x1d, 0x05, 0x15,	//coef34, coef35,coef36, coef37
0x1c, 0x0c, 0x14, 0x04	//decoef12,decoef20,decoef21,decoef22
};


#endif
