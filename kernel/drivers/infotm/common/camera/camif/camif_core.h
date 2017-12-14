/***************************************************************************** 
** 
** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** camif structure header file
**
** Revision History: 
** ----------------- 
** v1.0.1	sam@2014/03/31: first version.
**
*****************************************************************************/
#ifndef _CAMIF_CORE_H
#define _CAMIF_CORE_H

#include <media/v4l2-ctrls.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-device.h>
#include <linux/v4l2-mediabus.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_data/camera-imapx.h>
#include "../sensors/power_up.h"
//define v4l2 process statues
#define HOST_PENDING			(1 << 0)
#define HOST_RUNNING			(1 << 1)
#define HOST_STREAMING          (1 << 2)
#define HOST_OFF				(1 << 3)
#define HOST_CONFIG				(1 << 4)

#define VGA_WIDTH	(640)
#define VGA_HEIGHT  (480)
#define QCIF_WIDTH  (176)
#define QCIF_HEIGHT	(144)
#define QVGA_WIDTH  (320)
#define QVGA_HEIGHT (240)
#define SVGA_WIDTH  (800)
#define SVGA_HEIGHT (600)
#define XGA_WIDTH	(1024)
#define XGA_HEIGHT  (768)
#define SXGA_WIDTH  (1280)
#define SXGA_HEIGHT (1024)
#define UXGA_WIDTH  (1920)
#define UXGA_HEIGHT (1080)
#define CIF_WIDTH	(352)
#define CIF_HEIGHT	(288)
#define CUSTOMIZE_2160	(2160)
#define CUSTOMIZE_1080	(1080)
#define CUSTOMIZE_3840	(3840)
#define CUSTOMIZE_1920	(1920)
#define CUSTOMIZE_1280  (1280)
#define CUSTOMIZE_640  (640)

//define camif w/r register operation
#define camif_write(_host, _off, _val) writel((_val), (((_host)->base) + \
(_off)))
#define camif_read(_host, _off) readl((((host)->base) + (_off)))

struct camif_addr{
	u32 y;
	u32 cb;
	u32 cr;
	u32 y_len;
	u32 cb_len;
	u32 cr_len;
};

struct camif_fmt{
	__u8  *desc;
	__u32 pixelformat;
	int bpp;
	enum v4l2_mbus_pixelcode mbus_code;
};

struct camif_buffer{
	struct vb2_buffer vb;
	struct camif_addr paddr;
	struct list_head list;
	unsigned int index;
};

struct dma_use_table{
	int ch0;
	int ch1;
	int ch2;	
};

struct camif_win_size{
	int width;
	int height;
};


struct imapx_video {
	struct imapx_camif		    *camif_dev;
	struct v4l2_device			v4l2_dev;
	struct video_device			vdev;
	struct vb2_queue		    vb_queue;
	struct v4l2_subdev			*sensor;
	enum   v4l2_mbus_pixelcode	mbus_pixelcode;
	struct v4l2_ctrl_handler    ctrl_handler;
	struct v4l2_ctrl            *zoom_absolute;
	enum v4l2_mbus_pixelcode	mbus_code;
	struct v4l2_mbus_framefmt   *mbus_framefmt;
	struct sensor_info			*sen_info;
	struct camif_win_size		*pwin_sizes;
	//image format information
	struct v4l2_pix_format		infmt;
	struct v4l2_pix_format		outfmt;
	struct regulator			*dvdd_rg;
	struct regulator			*iovdd_rg;
	unsigned int				payload;  // for image size 	
	struct dma_use_table        dmause;
	struct list_head			pending_buf_q;
	struct list_head			active_buf_q;
	unsigned int				active_buffers;
	unsigned int				pending_buffers;
	int							state;
	int                         buf_index;
	unsigned int				cur_frame_index;
	unsigned int				frame_sequence;

};

// camif structure 
struct imapx_camif {
	// the board information (two sensors)
    struct sensor_info sen_info[MAX_SENSOR_NUM];
	struct imapx_cam_board *pdata;
	//someone device 
	struct imapx_video   vp[MAX_SENSOR_NUM];
	struct imapx_video   *cur_vp;
	struct device        *dev;
	struct mutex	     lock;
	spinlock_t			 slock;
	int					 irq;	
	void   __iomem		 *base;
	struct vb2_alloc_ctx *alloc_ctx;
    unsigned long busclk_rate;

};


#define IMPAX_N_CAMIF_FMTS (ARRAY_SIZE(imapx_camif_formats))

static inline void camif_active_queue_add(struct imapx_video *vp,
										struct camif_buffer *buf)
{
	list_add_tail(&(buf->list), &(vp->active_buf_q));
	vp->active_buffers++;
}

static inline  struct camif_buffer* camif_active_queue_pop(
									struct imapx_video *vp)
{
	struct camif_buffer *buf = list_first_entry(&(vp->active_buf_q),
										struct camif_buffer, list);
	list_del(&(buf->list));
	vp->active_buffers--;
	return buf;
}

static inline struct camif_buffer * camif_active_queue_peek(
									struct imapx_video *vp, int index)
{
	struct camif_buffer *tmp, *buf;
	if(WARN_ON(list_empty(&(vp->active_buf_q)))){
		pr_err("camif active queue is empty in %s\n", __func__);
		return NULL;
	}

	list_for_each_entry_safe(buf, tmp, &(vp->active_buf_q), list){
		if(buf->index == index){
			list_del(&(buf->list));
			vp->active_buffers--;
			return buf;
		}
	}

	return NULL;
}

static inline void camif_pending_queue_add(struct imapx_video *vp,
										struct camif_buffer *buf)
{
	list_add_tail(&(buf->list), &(vp->pending_buf_q));
}

static inline struct camif_buffer* camif_pending_queue_pop(
									struct imapx_video *vp)
{
	struct camif_buffer *buf = list_first_entry(&(vp->pending_buf_q),
									struct camif_buffer, list );
	list_del(&(buf->list));
	return buf;
}


#endif
