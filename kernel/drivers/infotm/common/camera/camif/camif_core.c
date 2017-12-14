/***************************************************************************** 
** 
** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** camif v4l2 core file
**
** Revision History: 
** ----------------- 
** v1.0.1	sam@2014/03/31: first version.
** v2.0.1	peter@2016/03/31: for q3f
**
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <media/media-device.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <InfotmMedia.h>
#include <utlz_lib.h>
#include <linux/gpio.h>
#include "../sensors/sen_dev.h"
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/ddk_sensor_driver.h>
#include "camif_core.h"
#include "camif_reg.h"

#define IMAPX_CAMIF_REQ_BUFS_MIN (3)
#define IMAPX_CAMIF_REQ_BUFS_MAX (8)
#define MAX_FRAMEBUFFER_NUM      (4)
#define MAX_ZOOM_ABSOLUT_RATIO	 (20) 
#define DISCARD_FRAME_NUM	 (5)
#define N_CAMERA_SENSORS	 (2) 

#ifdef CONFIG_CAMIF_EXTRA_OCLK
#ifdef CONFIG_IMAPX_Q3EVB_V1_1
#define IMAPX_PCM_MCLK_CFG	(0x0c)
#define IMAPX_PCM_CLKCTL	(0x08)
#define CAMERA_MCLK_PAD          (87) //modify for Q3 gpio sysout_clk
#else
#define CAMERA_MCLK_PAD          (112) //modify for Q3 gpio sysout_clk
#endif
#endif

//declare queue buffer callback function
static int camif_start_streaming(struct vb2_queue *vq, unsigned int count);
static int camif_stop_streaming(struct vb2_queue *vq);
static int camif_queue_setup(struct vb2_queue *vq, 
					const struct v4l2_format *pfmt,
					unsigned int *num_buffers,
					unsigned int *num_planes,
					unsigned int sizes[], 
					void *allocators[]);
static int camif_buffer_prepare(struct vb2_buffer *vb);
static void camif_buffer_queue(struct vb2_buffer *vb);


/* declare file operation callback function */
static volatile int file_opened = 0;
static int imapx_camif_open(struct file *file);
static int imapx_camif_close(struct file *file);
static unsigned int imapx_camif_poll(struct file *file,
					 struct poll_table_struct *wait);	
static int imapx_camif_mmap(struct file *file, struct vm_area_struct *vma);
static struct camif_fmt* imapx_camif_find_format(u32 pixelformat);

//declare video device ioctl operation
static int imapx_camif_ioctl_querycap(struct file *file, void *priv, 
						struct v4l2_capability *cap);
static int imapx_camif_ioctl_enum_input(struct file *file ,void *priv,
						struct v4l2_input *input);
static int imapx_camif_ioctl_s_input(struct file *file, void *priv,
						unsigned int index);
static int imapx_camif_ioctl_g_input(struct file *file, void *priv,
						unsigned int *index);
static int imapx_camif_ioctl_reqbufs(struct file *file, void *priv,
						struct v4l2_requestbuffers *rb);
static int imapx_camif_ioctl_s_std(struct file *file , void *priv,
						v4l2_std_id std);
static int imapx_camif_ioctl_enum_fmt(struct file *file, void *priv,
						struct v4l2_fmtdesc *fmt);
static int imapx_camif_ioctl_try_fmt(struct file *file, void *priv, 
							   struct v4l2_format *fmt);
static int imapx_camif_ioctl_g_fmt(struct file *file, void *priv,
							   struct v4l2_format *fmt);
static int imapx_camif_ioctl_s_fmt(struct file *file, void *priv,
							 struct v4l2_format *fmt);
static int imapx_camif_ioctl_reqbufs(struct file *file, void *priv,
							  struct v4l2_requestbuffers *rb);
static int imapx_camif_ioctl_streamoff(struct file *file, void *priv,
										enum v4l2_buf_type type);
static int imapx_camif_ioctl_streamon(struct file *file, void *priv,
									  enum v4l2_buf_type type);
static int imapx_camif_ioctl_dqbuf(struct file *file, void *priv,
									struct v4l2_buffer *buf);
static int imapx_camif_ioctl_qbuf(struct file *file, void *priv,
									struct v4l2_buffer *buf);
static int imapx_camif_ioctl_enum_framesizes(struct file *file, void *priv,
											struct v4l2_frmsizeenum *fsize);
static int imapx_camif_ioctl_querybuf(struct file *file, void *priv,
									  struct v4l2_buffer *buf);


// formats of camif support
struct camif_fmt imapx_camif_formats[] = {
	// support NV12 YUV420
	{
		.desc		= "YUV 4:2:0",
		.pixelformat = V4L2_PIX_FMT_NV12,
		.bpp		= 12,
		.mbus_code  = V4L2_MBUS_FMT_YUYV8_2X8,
	},
	// support YUYV
    {
        .desc       = "YUV 4:2:2",
        .pixelformat = V4L2_PIX_FMT_YUYV,
        .bpp        = 8,
        .mbus_code  = V4L2_MBUS_FMT_YUYV8_2X8,
    }
	
};

//define pixel format of camif default support
static const struct v4l2_pix_format default_pix_format = 
{
	.width			  = VGA_WIDTH,
	.height			  = VGA_HEIGHT,
	.pixelformat      = V4L2_PIX_FMT_NV12,
	.field			  = V4L2_FIELD_NONE,    
	.bytesperline     = VGA_WIDTH * 3 / 2,         
	.sizeimage		  = VGA_WIDTH * VGA_HEIGHT * 3 / 2, 
};

static struct  camif_win_size imapx_camif_wins[] = {
	/* QVGA */
	{
		.width  = QVGA_WIDTH,
		.height = QVGA_HEIGHT
	},
	/* VGA  */
	{
		.width  = VGA_WIDTH,
		.height = VGA_HEIGHT
	},
	/* SVGA */
	{
		.width  = SVGA_WIDTH,
		.height = SVGA_HEIGHT
	},
	/* XGA */
	{
		.width  = XGA_WIDTH,
		.height = XGA_HEIGHT
	},
	/* SXGA */
	{
		.width  = SXGA_WIDTH,
		.height = SXGA_HEIGHT
	},
	/* UXGA */
	{
		.width  = UXGA_WIDTH,
		.height = UXGA_HEIGHT
	},
	{
		.width  = CUSTOMIZE_2160,
		.height = CUSTOMIZE_1080
	},
	{
		.width  = CUSTOMIZE_3840,
		.height = CUSTOMIZE_1920
	},
    {
        .width  = CUSTOMIZE_1280,
        .height = CUSTOMIZE_640
    },
	/* end tag*/  
	{
		.width  = 0,
		.height = 0
	}
};
//define bus format
static const enum v4l2_mbus_pixelcode default_mbus_code
					  = V4L2_MBUS_FMT_YUYV8_2X8;
//define buffer queue callback structure 
static const struct vb2_ops imapx_camif_qops ={
	.queue_setup		= camif_queue_setup,
	.buf_prepare		= camif_buffer_prepare,
	.buf_queue			= camif_buffer_queue,
	.start_streaming	= camif_start_streaming,
	.stop_streaming		= camif_stop_streaming,
};

//define camif deivce file operation of callback structure
static const struct v4l2_file_operations imapx_camif_fops = {
	.owner			= THIS_MODULE,
	.open			= imapx_camif_open,
	.release		= imapx_camif_close,
	.poll			= imapx_camif_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap			= imapx_camif_mmap,
};

static const struct v4l2_ioctl_ops imapx_camif_ioctl_ops = {
	.vidioc_querycap			= imapx_camif_ioctl_querycap,
	.vidioc_enum_input			= imapx_camif_ioctl_enum_input,
	.vidioc_g_input				= imapx_camif_ioctl_g_input,
	.vidioc_s_input				= imapx_camif_ioctl_s_input,
	.vidioc_s_std				= imapx_camif_ioctl_s_std,
	.vidioc_enum_fmt_vid_cap    = imapx_camif_ioctl_enum_fmt,
	.vidioc_try_fmt_vid_cap		= imapx_camif_ioctl_try_fmt,
	.vidioc_g_fmt_vid_cap		= imapx_camif_ioctl_g_fmt,
	.vidioc_s_fmt_vid_cap		= imapx_camif_ioctl_s_fmt,
	.vidioc_reqbufs				= imapx_camif_ioctl_reqbufs,
	.vidioc_querybuf			= imapx_camif_ioctl_querybuf,
	.vidioc_qbuf				= imapx_camif_ioctl_qbuf,
	.vidioc_dqbuf				= imapx_camif_ioctl_dqbuf,
	.vidioc_streamon			= imapx_camif_ioctl_streamon,
	.vidioc_streamoff			= imapx_camif_ioctl_streamoff,
	.vidioc_enum_framesizes		= imapx_camif_ioctl_enum_framesizes,
};

//define camif video device structure for v4l2 
static struct video_device imapx_camif_vd = {
	.name		= "imapx-camif",
	.minor		= -1,
	.fops		= &imapx_camif_fops,
	.ioctl_ops  = &imapx_camif_ioctl_ops,
	.release    = video_device_release_empty,	
	
};

static int imapx_camif_ioctl_querycap(struct file *file, void *priv, 
						struct v4l2_capability *cap)
{
	struct imapx_video *vp = video_drvdata(file);

	strcpy(cap->driver, "imapx_camif");
	strcpy(cap->card,	vp->sen_info->name);
	sprintf(cap->bus_info, "%s-%s", vp->sen_info->interface,
							vp->sen_info->name);
	cap->version = 20140331;
	cap->device_caps = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int imapx_camif_ioctl_enum_input(struct file *file ,void *priv,
						struct v4l2_input *input)
{
	struct imapx_video *vp = video_drvdata(file);

	if(input->index || (NULL == vp->sensor))
	{
		return -EINVAL;
	}

	input->type = V4L2_INPUT_TYPE_CAMERA;
	strlcpy(input->name, vp->sensor->name, sizeof(input->name));
	return 0;
}

static int imapx_camif_ioctl_s_input(struct file *file, void *priv,
						unsigned int index)
{
	return (index == 0) ? 0 : (-EINVAL);

}

static int imapx_camif_ioctl_g_input(struct file *file, void *priv,
						unsigned int *index)
{
	*index = 0;
	return 0;
}

static int imapx_camif_ioctl_s_std(struct file *file , void *priv,
						v4l2_std_id std)
{
	return 0;
}

static int imapx_camif_ioctl_enum_fmt(struct file *file, void *priv,
						struct v4l2_fmtdesc *fmt)
{
	if(IMPAX_N_CAMIF_FMTS <= fmt->index)
	{
		return -EINVAL;
	}

	strlcpy(fmt->description, imapx_camif_formats[fmt->index].desc, 
											sizeof(fmt->description));
	fmt->pixelformat = imapx_camif_formats[fmt->index].pixelformat;

	return 0;
}

static int imapx_camif_try_fmt_internal(struct imapx_video *vp,
									struct v4l2_pix_format *upix,
									struct v4l2_pix_format *spix)
{
	struct v4l2_mbus_framefmt mbus_fmt;
	struct camif_fmt *f = imapx_camif_find_format(upix->pixelformat);
	//v4l2 frame size type
	struct v4l2_frmsizeenum sen_sizes;
	int delt_old = 0;
	int delt_new, cur_idx = 0;
	
	if(NULL == upix || NULL == vp || NULL == spix){
		pr_err("%s pass NULL pointer\n", __func__);
		return -1;
	}

	sen_sizes.index = 0;
	*spix = *upix;
	//to find a sensor format
	while(!v4l2_subdev_call(vp->sensor, video, enum_framesizes, &sen_sizes)){
		if(upix->width <= sen_sizes.discrete.width && 
				upix->height <= sen_sizes.discrete.height){
			delt_new = (sen_sizes.discrete.width - upix->width) + 
				(sen_sizes.discrete.height - upix->height);
			if(delt_new < delt_old || cur_idx == 0)
			{
				delt_old = delt_new;
				spix->width = sen_sizes.discrete.width;
				spix->height = sen_sizes.discrete.height;
				cur_idx++;

			}
		}	
		sen_sizes.index++;
			
	}
	v4l2_fill_mbus_format(&mbus_fmt, spix, f->mbus_code);
	v4l2_subdev_call(vp->sensor, video, try_mbus_fmt, &mbus_fmt);
	spix->width  = mbus_fmt.width;
	spix->height = mbus_fmt.height;
	spix->field  = mbus_fmt.field;
	spix->colorspace = mbus_fmt.colorspace;

	if(vp->sen_info->mode == 0x1e) { //for yuv422: bpp is 16, but raw is 8
	    f->bpp = 16;
    }
	spix->bytesperline = spix->width * f->bpp >> 3;
	spix->sizeimage = spix->height * spix->bytesperline;
	upix->pixelformat = spix->pixelformat;               
	upix->field = spix->field;                           
	upix->bytesperline = upix->width * f->bpp >>3;                  
	upix->sizeimage = upix->bytesperline * upix->height;
	return 0;
}

static int imapx_camif_ioctl_try_fmt(struct file *file, void *priv, 
							   struct v4l2_format *fmt)
{
	struct imapx_video *vp = video_drvdata(file);
	struct imapx_camif *host = vp->camif_dev;
	struct v4l2_format sfmt; 

	int ret = 0;
	mutex_lock(&(host->lock));
	ret = imapx_camif_try_fmt_internal(vp, &fmt->fmt.pix, &sfmt.fmt.pix);
	mutex_unlock(&(host->lock));
	return ret;	
}

static int imapx_camif_ioctl_g_fmt(struct file *file, void *priv,
							   struct v4l2_format *fmt)
{
	struct imapx_video *vp = video_drvdata(file);
	fmt->fmt.pix = vp->outfmt;
	return 0;
}

static int imapx_camif_ioctl_s_fmt(struct file *file, void *priv,
							 struct v4l2_format *fmt)
{
	struct imapx_video *vp = video_drvdata(file);
	struct v4l2_format sfmt;
	struct v4l2_mbus_framefmt mbus_fmt;
	struct camif_fmt *f;
	int ret = 0;
	f = imapx_camif_find_format(fmt->fmt.pix.pixelformat);
	ret = imapx_camif_try_fmt_internal(vp, &(fmt->fmt.pix), &(sfmt.fmt.pix));
	vp->outfmt = fmt->fmt.pix;
	vp->payload = vp->outfmt.sizeimage;
	vp->infmt = sfmt.fmt.pix;
	vp->mbus_code = f->mbus_code;
	v4l2_fill_mbus_format(&mbus_fmt, &vp->infmt, vp->mbus_code);
	ret = v4l2_subdev_call(vp->sensor, video, s_mbus_fmt, &mbus_fmt);
	if(ret){
		pr_err("camif call sensor 's_mbus_fmt' fail with %d\n",ret);
		return ret;
	}

	pr_db("camif Outfmt: win(%d, %d)\n", vp->outfmt.width, vp->outfmt.height);
	pr_db("camif Infmt:  win(%d, %d)\n", vp->infmt.width, vp->infmt.height);
	pr_db("camif driver v4l2 payload = %d:\n",vp->payload);
	ret = imapx_camif_set_fmt(vp);
	if(ret) {
		pr_err("camif set camif fmt fail with %d\n",ret);
		return ret;
	}
	return 0;

}

static int imapx_camif_ioctl_enum_framesizes(struct file *file, void *priv,
											struct v4l2_frmsizeenum *fsize)
{
	struct imapx_video *vp = video_drvdata(file);
	int i;
	struct v4l2_frmsizeenum sen_sizes;
	for(i = 0; (vp->pwin_sizes[i].width) != 0; i++)
	{
		sen_sizes.index = i;
		if(!v4l2_subdev_call(vp->sensor, video, enum_framesizes, &sen_sizes)){
			if((vp->pwin_sizes[fsize->index].width <= 
					sen_sizes.discrete.width) &&
			   (vp->pwin_sizes[fsize->index].height <=
					sen_sizes.discrete.height))
			{
				fsize->discrete.width =
								vp->pwin_sizes[fsize->index].width;
				fsize->discrete.height = 
								vp->pwin_sizes[fsize->index].height;
				return 0;
			}
		}
	}

	return -1;
}

static int imapx_camif_ioctl_reqbufs(struct file *file, void *priv,
							  struct v4l2_requestbuffers *rb)
{
	int ret = 0;
	struct imapx_video *vp = video_drvdata(file);
	//struct imapx_camif *host = vp->camif_dev;

	if(rb->count){
		rb->count = max_t(u32, IMAPX_CAMIF_REQ_BUFS_MIN, rb->count);
	}

	ret = vb2_reqbufs(&(vp->vb_queue), rb);
	if(0 > ret){
		pr_err("vb2_reqbufs fail \n");
		return ret;
	}

	if(rb->count && (rb->count < IMAPX_CAMIF_REQ_BUFS_MIN)){
		rb->count = 0;
		vb2_reqbufs(&(vp->vb_queue), rb);
		ret = -ENOMEM;
	}
	return ret;
}

static int imapx_camif_ioctl_querybuf(struct file *file, void *priv,
									  struct v4l2_buffer *buf)
{
	struct imapx_video *vp = video_drvdata(file);
	return vb2_querybuf(&(vp->vb_queue), buf);
}

static int imapx_camif_ioctl_qbuf(struct file *file, void *priv,
									struct v4l2_buffer *buf)
{
	struct imapx_video *vp = video_drvdata(file);
	return vb2_qbuf(&(vp->vb_queue), buf);

}

static int imapx_camif_ioctl_dqbuf(struct file *file, void *priv,
									struct v4l2_buffer *buf)
{
	struct imapx_video *vp = video_drvdata(file);
	return vb2_dqbuf(&(vp->vb_queue), buf, (file->f_flags & O_NONBLOCK));

}

static int imapx_camif_ioctl_streamon(struct file *file, void *priv,
									  enum v4l2_buf_type type) 
{
	struct imapx_video *vp = video_drvdata(file);
	int ret = 0;
	ret = vb2_streamon(&(vp->vb_queue), type);
	vp->state = HOST_RUNNING | HOST_STREAMING;
	return ret;
}

static int imapx_camif_ioctl_streamoff(struct file *file, void *priv,
										enum v4l2_buf_type type)
{
	struct imapx_video *vp = video_drvdata(file);
	struct imapx_camif *host = vp->camif_dev;
	struct camif_buffer	*buf;
	unsigned long flags;
	int ret = 0;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE){ 
		return -EINVAL; 
	}	
	ret = vb2_streamoff(&(vp->vb_queue), type);
	spin_lock_irqsave(&(host->slock), flags);
	vp->state &= ~(HOST_RUNNING | HOST_STREAMING);
	/*  Release unused buffers */                        
	while (!list_empty(&(vp->pending_buf_q))) {           
	    buf = camif_pending_queue_pop(vp);              
		vb2_buffer_done(&(buf->vb), VB2_BUF_STATE_ERROR); 
	}

	while (!list_empty(&(vp->active_buf_q))) {            
	    buf = camif_active_queue_pop(vp);               
		vb2_buffer_done(&(buf->vb), VB2_BUF_STATE_ERROR);
	}	
	spin_unlock_irqrestore(&(host->slock), flags);
	return ret;


}

static int imapx_camif_open(struct file *file)
{
	struct imapx_video *vp = video_drvdata(file);
	struct imapx_camif *host = vp->camif_dev;
	int ret = 0;

	pr_db("camif driver run here:%s\n", __func__);
	if(file_opened){
		pr_err("camif device has been opened,exit\n");
		return -1;
	}
	
	//set power clk gate
	if((host->pdata)->init){
		(host->pdata)->init();
	}

	if (host->sen_info[0].wraper_use) {

        #ifdef CONFIG_ARCH_Q3F
                camif_dvp_wrapper_probe();
                camif_dvp_wrapper_init(0x02,
                        host->sen_info[0].wwidth,
                        host->sen_info[0].wheight,
                        host->sen_info[0].whdly,
                        host->sen_info[0].wvdly);
        #endif

     }
	//set camif clk 
	//imapx_camif_enable_clk(host);
	
	//initalize video device structre data element
	vp->buf_index = 0;
	vp->state = HOST_PENDING;
	vp->pwin_sizes = imapx_camif_wins;
	file->private_data = host;
	
	//set sensor power
	//imapx_camif_set_sensor_power(vp, 1);
	ddk_sensor_power_up(vp->sen_info, 0, 1);
	
	if (!strcmp(vp->sen_info->interface, "camif-csi"))
	{
#ifdef CONFIG_ARCH_Q3F
	    struct clk * pclk = NULL;
	    int ret = 0;
	    pclk = clk_get_sys("imap-mipi-dphy-pix", "imap-mipi-dsi");
	    if (IS_ERR(pclk)) {
	        pr_err("get mipi pixel clock structure error\n");
	        goto err_open;
	    }
	    ret = clk_set_rate(pclk, vp->sen_info->mipi_pixclk);
        if (0 > ret){
           pr_err("Fail to set mipi_pixclk output %d clock\n",  vp->sen_info->mipi_pixclk);
           goto err_open;
        }

	    csi_core_init(vp->sen_info->lanes, vp->sen_info->csi_freq);
	    csi_iditoipi_open(vp->sen_info->wwidth,  vp->sen_info->wheight,  vp->sen_info->mode);
	    if (vp->sen_info->mode > 0x10) {
	        writel(readl(IO_ADDRESS(SYSMGR_CAMIF_BASE + 0x20)) | 0x04, IO_ADDRESS(SYSMGR_CAMIF_BASE + 0x20));
	        pr_db("ok in camif idi to ipi configure.mode = %d\n",  vp->sen_info->mode);
	    }
#endif
	}
	/* initalize sensor */
	ret = v4l2_subdev_call(vp->sensor, core, init, 0);
	if(ret){
		pr_err("camif callback sensor init fail,return :%d\n",ret);
		goto err_open;
	}
    //initalize camif 
    imapx_camif_host_init(vp);	

	host->cur_vp = vp;
	file_opened++;
	return 0;
err_open:
	//imapx_camif_set_sensor_power(vp, 0);
	//imapx_camif_remove_clk(host);
    ddk_sensor_power_up(vp->sen_info, 0, 0);
	if(host->pdata){
		(host->pdata)->deinit();
	}

	return ret;

}

static int imapx_camif_close(struct file *file)
{
	struct imapx_video *vp = video_drvdata(file);
	struct imapx_camif *host = vp->camif_dev;
	struct camif_buffer *buf;
	int ret = 0;

	mutex_lock(&(host->lock));
	if(!file_opened){
		pr_err("camif device hasn't been opened, exit\n");
		mutex_unlock(&(host->lock));
		return -1;
	}
	
	vp->state &= ~(HOST_RUNNING | HOST_STREAMING);
	if((vp->vb_queue).streaming){
		vb2_streamoff(&(vp->vb_queue), (vp->vb_queue).type);
	}

	imapx_camif_host_deinit(vp);
	//imapx_camif_set_sensor_power(vp, 0); need fixed
	ddk_sensor_power_up(vp->sen_info, 0, 0);
	if (!strcmp(vp->sen_info->interface, "camif-csi")) {
#ifdef CONFIG_ARCH_Q3F
	    csi_core_close();
#endif
	}
	while(!list_empty(&(vp->pending_buf_q))){
		buf = camif_pending_queue_pop(vp);
		vb2_buffer_done(&(buf->vb), VB2_BUF_STATE_ERROR);
	}
	
	while(!list_empty(&(vp->active_buf_q))){
		buf = camif_active_queue_pop(vp);
		vb2_buffer_done(&(buf->vb), VB2_BUF_STATE_ERROR);
	}
	
	//imapx_camif_remove_clk(host);
	if(host->pdata){
		(host->pdata)->deinit();
	}
	host->cur_vp = NULL;
	file_opened--;
	mutex_unlock(&(host->lock));
	pr_db("camif driver run here:%s\n", __func__);

	return ret;

}

static unsigned int imapx_camif_poll(struct file *file,
						struct poll_table_struct *wait)
{
	struct imapx_video *vp = video_drvdata(file);
	struct imapx_camif *host = vp->camif_dev;
	int ret =0 ;
	mutex_lock(&(host->lock));
	ret = vb2_poll(&(vp->vb_queue), file, wait);
	mutex_unlock(&(host->lock));
	return ret;
}

static int imapx_camif_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct imapx_video *vp = video_drvdata(file);
	return vb2_mmap(&(vp->vb_queue), vma);
}

static int imapx_camif_set_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret =0;
#if 0	
	struct imapx_video *vp = container_of(ctrl->handler, 
						struct imapx_video, ctrl_handler);
	switch(ctrl->id){
		//need to add operation
	};
#endif	
	return ret;
}

static const struct v4l2_ctrl_ops imapx_camif_ctrl_ops = {
	.s_ctrl = imapx_camif_set_ctrl,
};

static void imapx_camif_prepare_addr(struct imapx_video *vp, 
									struct camif_buffer *buf)
{
	switch((vp->outfmt).pixelformat)
	{
		case V4L2_PIX_FMT_NV12:
        {
            (buf->paddr).y = vb2_dma_contig_plane_dma_addr(&(buf->vb), 0);
            (buf->paddr).y_len = (vp->outfmt.width)*(vp->outfmt.height);
            (vp->dmause).ch2 = 1;
            (buf->paddr).cb =(u32)((buf->paddr).y + (buf->paddr).y_len);
            (buf->paddr).cb_len =((buf->paddr).y_len) >> 1;
            (vp->dmause).ch1 = 1;
            (buf->paddr).cr = 0;
            (buf->paddr).cr_len = 0;
            (vp->dmause).ch0 = 0;

            break;
        }
		case V4L2_PIX_FMT_YUYV:
		{
		    (buf->paddr).y = vb2_dma_contig_plane_dma_addr(&(buf->vb), 0);
		    //printk("in prepare address w = %d, h =%d\n", vp->outfmt.width, vp->outfmt.height);
		    //(vp->outfmt.width)*(vp->outfmt.height) << 1; for transfer raw data ,so not << 1
            (buf->paddr).y_len = ((vp->outfmt.width)*(vp->outfmt.height)) << 1;
            pr_db(" (buf->paddr).y_len = %x\n", (buf->paddr).y_len );
            (vp->dmause).ch2 = 1;
            (buf->paddr).cb =0;
            (buf->paddr).cb_len =0;
            (vp->dmause).ch1 = 0;
            (buf->paddr).cr = 0;
            (buf->paddr).cr_len = 0;
            (vp->dmause).ch0 = 0;
		    break;
		}
		default:
			break;
	}

	return ;
}

static int camif_buffer_prepare(struct vb2_buffer *vb)
{
	struct imapx_video *vp = vb2_get_drv_priv(vb->vb2_queue);
	if(vb2_plane_size(vb, 0) < vp->payload){
		pr_err("vb2 buffer less than payload\n");
		return -EINVAL;
	}

	vb2_set_plane_payload(vb, 0, vp->payload);

	return 0;
}

static struct camif_fmt* imapx_camif_find_format(u32 pixelformat)
{
	int i;
	for(i = 0; i < IMPAX_N_CAMIF_FMTS; i++){
		if(imapx_camif_formats[i].pixelformat == pixelformat){
			return &imapx_camif_formats[i];
		}
	}
	
	pr_db("camif hasn't find the current format[%x]\n", pixelformat);
	return (&imapx_camif_formats[0]);
}

static int camif_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct imapx_video *vp = vb2_get_drv_priv(vq);
	struct imapx_camif *host = vp->camif_dev;
	unsigned long flags;
	pr_db("camif driver run here:%s\n", __func__);
	vp->cur_frame_index = 0;
	vp->frame_sequence = 0;
	
	spin_lock_irqsave(&(host->slock), flags);
	//enable camif interrupt		
	//imapx_camif_unmask_irq(vp, INT_MASK_ALL);		
	v4l2_subdev_call(vp->sensor, video, s_stream, 1);
	imapx_camif_enable_capture(vp);
	spin_unlock_irqrestore(&(host->slock), flags);

	return 0;

}

static int camif_stop_streaming(struct vb2_queue *vq)
{
	struct imapx_video *vp = vb2_get_drv_priv(vq);
	pr_db("camif driver run here:%s\n", __func__);
	v4l2_subdev_call(vp->sensor, video, s_stream, 0);
	imapx_camif_disable_capture(vp);
	return 0;
}

static int camif_queue_setup(struct vb2_queue *vq, 
					const struct v4l2_format *pfmt,
					unsigned int *num_buffers,
					unsigned int *num_planes,
					unsigned int sizes[], 
					void *allocators[])
{
	struct imapx_video *vp	 = vb2_get_drv_priv(vq);
	struct imapx_camif *host = vp->camif_dev;
	const struct v4l2_pix_format *pix = NULL;
	const struct camif_fmt *f = NULL;
			
	pr_db("camif %s\n", __func__);
	*num_planes = 1;
	
	// FIXME only one plane support now
	if(pfmt)
	{
		pix = &((pfmt->fmt).pix);
		f = imapx_camif_find_format((pfmt->fmt).pix.pixelformat);
		sizes[0] = pix->width * pix->height * f->bpp >> 3;
	}
	else{
		sizes[0] = vp->outfmt.sizeimage; 
	}
	allocators[0] = host->alloc_ctx;
    
	return 0;	
								 
}

static void  camif_buffer_queue(struct vb2_buffer *vb)
{
	struct imapx_video *vp = vb2_get_drv_priv(vb->vb2_queue);
	struct imapx_camif *host = vp->camif_dev;
	struct camif_buffer *buf = container_of(vb, struct camif_buffer, vb);
	unsigned long flags;

	spin_lock_irqsave(&(host->slock), flags);
	//camif prepare buffer
	imapx_camif_prepare_addr(vp, buf);
	
	if(!(vp->state & HOST_STREAMING) && ((vp->active_buffers) < 2)){
		buf->index = vp->buf_index;
		imapx_camif_cfg_addr(vp, buf, buf->index);
		imapx_camif_cfg_addr(vp, buf, (buf->index) + 2);
		vp->buf_index = !(vp->buf_index); //0/1
		camif_active_queue_add(vp, buf);
	}
	else{
		camif_pending_queue_add(vp, buf);
	}

	spin_unlock_irqrestore(&(host->slock), flags);
	
	return ;

}


static struct i2c_board_info *imapx_camif_lookup_sensor_dev(struct imapx_video
	*vp)
{
	int i;
	for(i = 0; i < N_SUPPORT_SENSORS; i++)
	{
	    pr_db("imap_camera_info[i].type = %s ### vp->sen_info->name = %s \n",
	            imap_camera_info[i].type , vp->sen_info->name);
		if(0 == strcmp((vp->sen_info)->name, imap_camera_info[i].type)){

			return &imap_camera_info[i];
		}
		
	}
	return NULL;
}


static irqreturn_t imapx_camif_irq_handler(int irq, void *priv)
{
	struct imapx_camif *host = priv;
	struct imapx_video *vp = host->cur_vp;
	unsigned int status = camif_read(host, CAMIF_CICPTSTATUS);
    unsigned int H = 0, W = 0, sum;
    //comment of codes for debug use.
    //pr_db("camif drvier interrupt status:%x\n",status);
	//imapx_camif_dump_regs(vp);
	//csi2_dump_regs();
    sum=  camif_read(host, CAMIF_INPUT_SUMPIXS);
    H = (camif_read(host, CAMIF_ACTIVE_SIZE) >> 16) & 0xFFFF;
    W = (camif_read(host, CAMIF_ACTIVE_SIZE)) & 0xFFFF;
    pr_db("Get input active sum pixels : %d\n", sum);
    pr_db("Get input active picture : width = %d height = %d\n", W, H);

	if((status & (0x01 << CICPTSTATUS_C_PATH_DMA_SUCCESS)) &&
	        (!(status & (1 << CICPTSTATUS_C_PATH_FIFO_DIRTY_STATUS))))
	{
	    //dvp_wrapper_debug();
		//discard firstly some frames
		if(DISCARD_FRAME_NUM > vp->cur_frame_index){
			camif_write(host, CAMIF_CICPTSTATUS, status);
			vp->cur_frame_index++;
			return IRQ_HANDLED;
		}

		if(!list_empty(&(vp->pending_buf_q)) && (HOST_RUNNING & vp->state)
				&&(!list_empty(&(vp->active_buf_q)))){
			unsigned int index;
			struct camif_buffer *vbuf;
			struct timeval *tv;
			struct timespec ts;
			index = (camif_read(host, CAMIF_CH2DMACC1) >> 28) & 0x03;
			index = ((index != 0)?(index -1):3) & 0x01;
			ktime_get_ts(&ts);
			vbuf = camif_active_queue_peek(vp, index);
			if(!WARN_ON(NULL == vbuf)){
				tv = &(vbuf->vb.v4l2_buf.timestamp);
				tv->tv_sec = ts.tv_sec;
				tv->tv_usec = ts.tv_nsec/NSEC_PER_USEC;
				vbuf->vb.v4l2_buf.sequence = vp->frame_sequence++;

				vb2_buffer_done(&(vbuf->vb), VB2_BUF_STATE_DONE);

				vbuf = camif_pending_queue_pop(vp);
				vbuf->index = index;
				imapx_camif_cfg_addr(vp, vbuf, index);
				imapx_camif_cfg_addr(vp, vbuf, index + 2);
				camif_active_queue_add(vp, vbuf);
			}
		}


	}
	else
	{
	    //csi2_dump_regs();
	    printk("camif drvier interrupt status:%x\n",status);
	}

	camif_write(host, CAMIF_CICPTSTATUS, status);	
	return IRQ_HANDLED;
}

static int imapx_camif_register_video_node(struct imapx_camif *host, int idx)
{
	struct imapx_video *vp = &(host->vp[idx]);
	struct i2c_adapter *adapter;
	struct i2c_board_info *sensor_dev;
	int ret = 0;

	if(!((host->sen_info[idx]).exist)){
		pr_warn("camif sensor%d not exist\n", idx);
		return -ENXIO;
	}
	//get sen_info from camif 
	vp->sen_info = &(host->sen_info[idx]);

	pr_db("name=%s\ninterface=%s\ni2c_addr=0x%x\nchn=%d\nlanes=%d\ncsi_freq=%d\nmclk_src=%s\n"
	            "mclk=%d\nwraper_use=%d\nwwidth=%d\nwheight=%d\nwhdly=%d\nwvdly=%dimager=%d\n \
	            mipi_pixclk = %d\n", vp->sen_info->name, vp->sen_info->interface, vp->sen_info->i2c_addr,\
	            vp->sen_info->chn, vp->sen_info->lanes, vp->sen_info->csi_freq \
	            ,vp->sen_info->mclk_src, vp->sen_info->mclk, \
	            vp->sen_info->wraper_use, vp->sen_info->wwidth, vp->sen_info->wheight, \
	            vp->sen_info->whdly, vp->sen_info->wvdly, vp->sen_info->imager, vp->sen_info->mipi_pixclk);

	sensor_dev = imapx_camif_lookup_sensor_dev(vp);
	if(NULL == sensor_dev){
        pr_err("lookup sensor device failed:\n");
        return -ENXIO;
    }

	adapter = i2c_get_adapter(vp->sen_info->chn);
	if(NULL == adapter){                                    
		pr_err("camif fail to get i2c adapter %d\n",
		     vp->sen_info->chn);
		return -ENXIO;
	}
	
	vp->outfmt = vp->infmt = default_pix_format;
	vp->mbus_code = default_mbus_code;
	vp->state = HOST_PENDING;
	//should add pending /active buffer queue--->need to add
	INIT_LIST_HEAD(&(vp->pending_buf_q));
	INIT_LIST_HEAD(&(vp->active_buf_q));		
	sprintf((vp->v4l2_dev).name, "%s", (vp->sen_info)->name);
	ret = v4l2_device_register(NULL, &(vp->v4l2_dev));
	if(ret)
	{
		pr_err("camif unable to register v4l2 device\n");
		goto put_adatpter;
	}
	
	//connect sub sensor device and register i2c sub sensor device 
	vp->sensor = v4l2_i2c_new_subdev_board(&(vp->v4l2_dev), 
				adapter, sensor_dev, NULL);
	if(!(vp->sensor))
	{
		pr_err("camif connect to subdev:%s fail", (vp->sen_info)->name);
		goto unregister_v4l2_dev;
	}

	/* register vb2 buf ops */
	vp->vb_queue.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vp->vb_queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	vp->vb_queue.mem_ops  = &vb2_dma_contig_memops;
	vp->vb_queue.ops	  = &imapx_camif_qops;
	vp->vb_queue.drv_priv = vp;
	vp->vb_queue.timestamp_type  = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	vp->vb_queue.buf_struct_size = sizeof(struct camif_buffer);
	
	ret = 0;
	ret = vb2_queue_init(&(vp->vb_queue));
	if(ret){
		pr_err("camif vb2 queue init error, return %d\n", ret);	
		goto unregister_v4l2_dev;
	}

	/* register ctrl_handler */
	ret = v4l2_ctrl_handler_init(&(vp->ctrl_handler), 1);
	if(ret){
		pr_err("camif unable to init v4l2 ctrl handler\n");
		goto vb2_queue_err;
	}
//as do zoom in hal layer, driver don't add this contrl
#if 0
	vp->zoom_absolute = v4l2_ctrl_new_std(&(vp->ctrl_handler),
					 &imapx_camif_ctrl_ops,
					 V4L2_CID_ZOOM_ABSOLUTE, 0, MAX_ZOOM_ABSOLUT_RATIO,
					 1,	0);
#endif
	ret = v4l2_ctrl_add_handler(&(vp->ctrl_handler),
					 (vp->sensor)->ctrl_handler, NULL);
	if(0 > ret){
		pr_err("camif v4l2 add handle error, return %d\n", ret);
		goto vb2_queue_err;
	}

	/* register v4l2 device */
	vp->vdev			  = imapx_camif_vd;
	vp->vdev.ctrl_handler = &(vp->ctrl_handler);
	vp->vdev.v4l2_dev     = &(vp->v4l2_dev);
	vp->vdev.lock		  = &(host->lock);
	ret = video_register_device(&(vp->vdev), VFL_TYPE_GRABBER, -1);
	if(ret){
		pr_err("camif fail to register v4l2 device:%d\n", ret);
		goto vb2_queue_err;
	}
	video_set_drvdata(&(vp->vdev), vp);
	vp->camif_dev = host;
	/* detect subdev sensor: first power up */
	//no need to detect : for speed up boot time
#if 0
	ddk_sensor_power_up(host->sen_info, idx, 1);

	if(v4l2_subdev_call(vp->sensor, core, init, 1)){
		pr_err("camif detect %s fail.\n", (vp->sen_info)->name);
		goto release_video_device;
	}
	/*after detect subdev sensor: power down */
	//imapx_camif_set_sensor_power(vp, 0);
	ddk_sensor_power_up(host->sen_info, idx, 0);
	//vp->camif_dev = host; //move upforward
#endif    

	return 0;
#if 0
release_video_device:
	//imapx_camif_set_sensor_power(vp, 0);
    //ddk_sensor_power_up(host->sen_info, idx, 0);
	video_unregister_device(&(vp->vdev));
#endif
vb2_queue_err:
	vb2_queue_release(&(vp->vb_queue));
unregister_v4l2_dev:
	v4l2_device_unregister(&(vp->v4l2_dev));
put_adatpter:
	i2c_put_adapter(adapter);

	return -1;
}

static void imapx_camif_unregister_video_node(struct imapx_camif *host,
											 int idx)
{
	struct imapx_video *vp = &(host->vp[idx]);
	if(!host->sen_info[idx].exist){
		pr_err("camif sensor %d: not exist\n",idx);
		return;
	}
	ddk_sensor_power_up(host->sen_info, idx, 0);

	if(video_is_registered(&(vp->vdev))){
		v4l2_ctrl_handler_free(vp->vdev.ctrl_handler);
		video_unregister_device(&(vp->vdev));
	}

	v4l2_device_unregister(&(vp->v4l2_dev));

}


static int imapx_camif_probe(struct platform_device *pdev)
{
	struct device *dev = &(pdev->dev);
	//board-->sensor infomation & init/deinit 
	struct imapx_cam_board *pboard = (pdev->dev).platform_data;
	struct imapx_camif *host = NULL;
	struct resource *res = NULL;
	int ret = 0, idx = 0;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(NULL == res){
		pr_err("camif get resource error\n");
		return -ENXIO;
	}

	host = devm_kzalloc(dev, sizeof(struct imapx_camif), GFP_KERNEL);
	if(!host){
		return -ENOMEM;
	}
	memset(host, 0, sizeof(struct imapx_camif));	
	host->dev = dev;
	host->cur_vp = NULL;
	
	host->pdata = pboard;
	imapx_pad_init("camif");
	if((host->pdata)->init){
		(host->pdata)->init();
	}
	else{
		pr_err("No way to init camif interface\n");
		return -ENXIO;
	}

	//ioremap device memory to kernel
	host->base = devm_ioremap_resource(dev, res);
	if(IS_ERR(host->base)){
		dev_err(&(pdev->dev), "camif io_memory remap failed\n");
		return PTR_ERR(host->base);
	}

	//register irq
	host->irq = platform_get_irq(pdev, 0);
	if(0 >= host->irq){
		dev_err(&(pdev->dev), "get irq failed:%d\n", host->irq);
		return -ENXIO;
	}
	
	ret = devm_request_irq(dev, host->irq, imapx_camif_irq_handler,
			0, dev_name(dev), host);
	if(0 > ret){
		dev_err(&(pdev->dev), "fail to install IRQ:%d\n", ret);
		return ret;
	}
	
	host->alloc_ctx = vb2_dma_contig_init_ctx(dev);
	/*register /dev/videoX */
	for(idx = 0; idx < MAX_SENSOR_NUM; idx++){
        ret = get_sensor_board_info(&(host->sen_info[idx]), idx);
        if(-2 == ret) {
           pr_err("ddk sensor%d interface item not exist!\n", idx);
           continue;
        }
        //if interface is dvp or mipi , then for isp use not camif
        if (!strcmp(host->sen_info[idx].interface, "dvp") || !strcmp(host->sen_info[idx].interface, "mipi") )
           continue;

        imapx_camif_register_video_node(host, idx);
	}

	mutex_init(&(host->lock));
	spin_lock_init(&(host->slock));
	platform_set_drvdata(pdev, host);
	if(host->pdata){
		host->pdata->deinit();
	}

	return 0;
}


static int imapx_camif_remove(struct platform_device *pdev)
{
	struct imapx_camif *host = platform_get_drvdata(pdev);
	int idx;

	//release irq
	free_irq(host->irq, host);
	//release video device
	for(idx = 0; idx < MAX_SENSOR_NUM; idx++){
		imapx_camif_unregister_video_node(host,idx);			
	}

	vb2_dma_contig_cleanup_ctx(host->alloc_ctx);

	kfree(host);
	return 0;
	//
}

static int imapx_camif_plat_suspend(struct device *dev)
{
	//TODO
	return 0;
}

static int imapx_camif_plat_resume(struct device *dev)
{
	//TODO
	return 0;
}
//define camif power manager operation
SIMPLE_DEV_PM_OPS(imapx_camif_pm_ops, imapx_camif_plat_suspend, 
		imapx_camif_plat_resume);
//define platform driver structure
static struct platform_driver imapx_camif_driver = {
		.probe	= imapx_camif_probe,
		.remove = imapx_camif_remove,
		.driver = {                    //device driver structure
			.name  = "imapx-camif",
			.owner = THIS_MODULE,
			.pm    = &imapx_camif_pm_ops,
		},
};

static int __init imapx_camif_init(void)
{
	return platform_driver_register(&imapx_camif_driver);
}

static void __exit imapx_camif_exit(void)
{
	platform_driver_unregister(&imapx_camif_driver);
}

module_init(imapx_camif_init);
module_exit(imapx_camif_exit);


MODULE_AUTHOR("sam");
MODULE_DESCRIPTION("imapx camif driver");
MODULE_LICENSE("GPL2");








