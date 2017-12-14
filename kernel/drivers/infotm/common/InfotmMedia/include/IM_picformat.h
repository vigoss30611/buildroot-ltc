/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
** v1.3.1	leo@2012/05/15: add hex denote to IM_PIC_FMT_xxx.
**
*****************************************************************************/ 

#ifndef __IM_PICFORMAT_H__
#define __IM_PICFORMAT_H__

// color space
#define IM_PIC_CS_RGB		(0)
#define IM_PIC_CS_RGB_PAL	(1)
#define IM_PIC_CS_YUV		(2)
#define IM_PIC_CS_JPEG		(8)

//
// [31:28] reserved [27:24]cs [23:16]memory size(bits) of per pixel [15:8]bpp [7:0] argb arange format.
//
#define IM_PIC_CS(fmt)		(((fmt) >> 24) & 0xf)
#define IM_PIC_BITS(fmt)	(((fmt) >> 16) & 0xff)
#define IM_PIC_BPP(fmt)		(((fmt) >> 8) & 0xff)

// rgb 1bits
#define IM_PIC_FMT_1BITS_RGB_PAL	((1<<24) | (1<<16) | (1<<8) | 0)	// 0x01010100
// rgb 2bits
#define IM_PIC_FMT_2BITS_RGB_PAL	((1<<24) | (2<<16) | (2<<8) | 0)	// 0x01020200
// rgb 4bits
#define IM_PIC_FMT_4BITS_RGB_PAL	((1<<24) | (4<<16) | (4<<8) | 0)	// 0x01040400
// rgb 8bits
#define IM_PIC_FMT_8BITS_RGB_PAL	((1<<24) | (8<<16) | (8<<8) | 0)	// 0x01080800
#define IM_PIC_FMT_8BITS_ARGB_1232	((0<<24) | (8<<16) | (8<<8) | 1)	// 0x00080800
// rgb 16bits
#define IM_PIC_FMT_16BITS_RGB_565	((0<<24) | (16<<16) | (16<<8) | 0)	// 0x00101000
#define IM_PIC_FMT_16BITS_BGR_565	((0<<24) | (16<<16) | (16<<8) | 1)	// 0x00101001
#define IM_PIC_FMT_16BITS_0RGB_1555	((0<<24) | (16<<16) | (15<<8) | 2)	// 0x00100F02
#define IM_PIC_FMT_16BITS_0BGR_1555	((0<<24) | (16<<16) | (15<<8) | 3)	// 0x00100F03
#define IM_PIC_FMT_16BITS_RGB0_5551	((0<<24) | (16<<16) | (15<<8) | 4)	// 0x00100F04
#define IM_PIC_FMT_16BITS_BGR0_5551	((0<<24) | (16<<16) | (15<<8) | 5)	// 0x00100F05
#define IM_PIC_FMT_16BITS_IRGB_1555	((0<<24) | (16<<16) | (16<<8) | 6)	// 0x00101006
#define IM_PIC_FMT_16BITS_IBGR_1555	((0<<24) | (16<<16) | (16<<8) | 7)	// 0x00101007
#define IM_PIC_FMT_16BITS_RGBI_5551	((0<<24) | (16<<16) | (16<<8) | 8)	// 0x00101008
#define IM_PIC_FMT_16BITS_BGRI_5551	((0<<24) | (16<<16) | (16<<8) | 9)	// 0x00101009
#define IM_PIC_FMT_16BITS_0RGB_4444	((0<<24) | (16<<16) | (12<<8) | 10)	// 0x0010100A
#define IM_PIC_FMT_16BITS_0BGR_4444	((0<<24) | (16<<16) | (12<<8) | 11)	// 0x0010100B
#define IM_PIC_FMT_16BITS_RGB0_4444	((0<<24) | (16<<16) | (12<<8) | 12)	// 0x0010100C
#define IM_PIC_FMT_16BITS_BGR0_4444	((0<<24) | (16<<16) | (12<<8) | 13)	// 0x0010100D
#define IM_PIC_FMT_16BITS_ARGB_1555	((0<<24) | (16<<16) | (16<<8) | 14)	// 0x0010100E
#define IM_PIC_FMT_16BITS_ABGR_1555	((0<<24) | (16<<16) | (16<<8) | 15)	// 0x0010100F
#define IM_PIC_FMT_16BITS_RGBA_5551	((0<<24) | (16<<16) | (16<<8) | 16)	// 0x00101010
#define IM_PIC_FMT_16BITS_BGRA_5551	((0<<24) | (16<<16) | (16<<8) | 17)	// 0x00101011
#define IM_PIC_FMT_16BITS_ARGB_4444	((0<<24) | (16<<16) | (12<<8) | 18)	// 0x00101012
#define IM_PIC_FMT_16BITS_ABGR_4444	((0<<24) | (16<<16) | (12<<8) | 19)	// 0x00101013
#define IM_PIC_FMT_16BITS_RGBA_4444	((0<<24) | (16<<16) | (12<<8) | 20)	// 0x00101014
#define IM_PIC_FMT_16BITS_BGRA_4444	((0<<24) | (16<<16) | (12<<8) | 21)	// 0x00101015
// rgb 24bits
#define IM_PIC_FMT_24BITS_RGB_888	((0<<24) | (24<<16) | (24<<8) | 0)	// 0x00181800
#define IM_PIC_FMT_24BITS_BGR_888	((0<<24) | (24<<16) | (24<<8) | 1)	// 0x00181801
// rgb 32bits
#define IM_PIC_FMT_32BITS_0RGB_8888	((0<<24) | (32<<16) | (24<<8) | 0)	// 0x00201800
#define IM_PIC_FMT_32BITS_0BGR_8888	((0<<24) | (32<<16) | (24<<8) | 1)	// 0x00201801
#define IM_PIC_FMT_32BITS_RGB0_8888	((0<<24) | (32<<16) | (24<<8) | 2)	// 0x00201802
#define IM_PIC_FMT_32BITS_BGR0_8888	((0<<24) | (32<<16) | (24<<8) | 3)	// 0x00201803
#define IM_PIC_FMT_32BITS_ARGB_1565	((0<<24) | (32<<16) | (16<<8) | 4)	// 0x00201004
#define IM_PIC_FMT_32BITS_ARGB_8565	((0<<24) | (32<<16) | (16<<8) | 5)	// 0x00201005
#define IM_PIC_FMT_32BITS_ARGB_1665	((0<<24) | (32<<16) | (17<<8) | 6)	// 0x00201106
#define IM_PIC_FMT_32BITS_ARGB_1666	((0<<24) | (32<<16) | (18<<8) | 7)	// 0x00201207
#define IM_PIC_FMT_32BITS_0RGB_D666	((0<<24) | (32<<16) | (18<<8) | 8)	// 0x00201208
#define IM_PIC_FMT_32BITS_0BGR_D666	((0<<24) | (32<<16) | (18<<8) | 9)	// 0x00201209
#define IM_PIC_FMT_32BITS_RGB0_666D	((0<<24) | (32<<16) | (18<<8) | 10)	// 0x0020120A
#define IM_PIC_FMT_32BITS_BGR0_666D	((0<<24) | (32<<16) | (18<<8) | 11)	// 0x0020120B
#define IM_PIC_FMT_32BITS_ARGB_1887	((0<<24) | (32<<16) | (23<<8) | 12)	// 0x0020170C
#define IM_PIC_FMT_32BITS_ARGB_8888	((0<<24) | (32<<16) | (24<<8) | 13)	// 0x0020180D
#define IM_PIC_FMT_32BITS_ABGR_8888	((0<<24) | (32<<16) | (24<<8) | 14)	// 0x0020180E
#define IM_PIC_FMT_32BITS_RGBA_8888	((0<<24) | (32<<16) | (24<<8) | 15)	// 0x0020180F
#define IM_PIC_FMT_32BITS_BGRA_8888	((0<<24) | (32<<16) | (24<<8) | 16)	// 0x00201810
#define IM_PIC_FMT_32BITS_ARGB_4888	((0<<24) | (32<<16) | (24<<8) | 17)	// 0x00201811
#define IM_PIC_FMT_32BITS_ABGR_4888	((0<<24) | (32<<16) | (24<<8) | 18)	// 0x00201812
#define IM_PIC_FMT_32BITS_RGBA_8884	((0<<24) | (32<<16) | (24<<8) | 19)	// 0x00201813
#define IM_PIC_FMT_32BITS_BGRA_8884	((0<<24) | (32<<16) | (24<<8) | 20)	// 0x00201814
#define IM_PIC_FMT_32BITS_ARGB_1888	((0<<24) | (32<<16) | (24<<8) | 21)	// 0x00201815
#define IM_PIC_FMT_32BITS_ABGR_1888	((0<<24) | (32<<16) | (24<<8) | 22)	// 0x00201816
#define IM_PIC_FMT_32BITS_RGBA_8881	((0<<24) | (32<<16) | (24<<8) | 23)	// 0x00201817
#define IM_PIC_FMT_32BITS_BGRA_8881	((0<<24) | (32<<16) | (24<<8) | 24)	// 0x00201818
// yuv 12bits
#define IM_PIC_FMT_12BITS_YUV420P	((2<<24) | (12<<16) | (12<<8) | 0)	// 0x020C0C00
#define IM_PIC_FMT_12BITS_YUV420SP	((2<<24) | (12<<16) | (12<<8) | 1)	// 0x020C0C01
// yuv 16bits
#define IM_PIC_FMT_16BITS_YUV422P	((2<<24) | (16<<16) | (16<<8) | 0)	// 0x02101000
#define IM_PIC_FMT_16BITS_YUV422SP	((2<<24) | (16<<16) | (16<<8) | 1)	// 0x02101001
#define IM_PIC_FMT_16BITS_YUV422I_YUYV	((2<<24) | (16<<16) | (16<<8) | 2)	// 0x02101002
#define IM_PIC_FMT_16BITS_YUV422I_YVYU	((2<<24) | (16<<16) | (16<<8) | 3)	// 0x02101003
#define IM_PIC_FMT_16BITS_YUV422I_UYVY	((2<<24) | (16<<16) | (16<<8) | 4)	// 0x02101004
#define IM_PIC_FMT_16BITS_YUV422I_VYUY	((2<<24) | (16<<16) | (16<<8) | 5)	// 0x02101005
// yuv 32bits
#define IM_PIC_FMT_32BITS_YUV444I	((2<<24) | (32<<16) | (24<<8) | 0)	// 0x02201800
// jpeg
#define IM_PIC_FMT_JPEG_YUV420P		((8<<24) | (0<<16) | (0<<8) | 0)	// 0x08000000
#define IM_PIC_FMT_JPEG_YUV420SP	((8<<24) | (0<<16) | (0<<8) | 1)	// 0x08000001
#define IM_PIC_FMT_JPEG_YUV422P		((8<<24) | (0<<16) | (0<<8) | 2)	// 0x08000002
#define IM_PIC_FMT_JPEG_YUV422SP	((8<<24) | (0<<16) | (0<<8) | 3)	// 0x08000003
#define IM_PIC_FMT_JPEG_YUV422I_YUYV	((8<<24) | (0<<16) | (0<<8) | 4)	// 0x08000004
#define IM_PIC_FMT_JPEG_YUV422I_YVYU	((8<<24) | (0<<16) | (0<<8) | 5)	// 0x08000005
#define IM_PIC_FMT_JPEG_YUV422I_UYVY	((8<<24) | (0<<16) | (0<<8) | 6)	// 0x08000006
#define IM_PIC_FMT_JPEG_YUV422I_VYUY	((8<<24) | (0<<16) | (0<<8) | 7)	// 0x08000007

//
//
//
typedef struct{
	IM_UINT32	format;		// IM_PIC_FMT_xxx.
	IM_UINT32	width;		// pixel unit.
	IM_UINT32	height;		// pixel unit.
	IM_UINT32	stride;		// y, ycbycr, rgb buffer stride, bytes nuit.
	IM_UINT32	strideCb;	// cb, cbcr buffer stride, bytes nuit.
	IM_UINT32	strideCr;	// cr buffer stride, bytes nuit.
}im_picture_desc_t;


#endif	//__IM_PICFORMAT_H__

