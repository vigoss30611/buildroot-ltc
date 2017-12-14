/*
 *  drivers/infotm/common/camera/isp/isp_core.c
 */
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
#include "../sensors/sen_dev.h"
#include "isp_core.h"
#include "isp_reg.h"

#define VGA_WIDTH   640
#define VGA_HEIGHT  480
#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144
#define IMAPX_ISP_REQ_BUFS_MIN  3
#define IMAPX_ISP_REQ_BUFS_MAX  8
#define MAX_FRAMEBUFFER_NUM		4
#define MAX_ZOOM_ABSOLUT_RATIO  20
#define DISCARD_FRAME_NUM       5

static int file_opened = 0;

#if 0
static struct isp_win_size *imapx_isp_find_size(int width, int height)
{
	unsigned i;
	for (i = 0; i < N_IMAPX_ISP_SIZES; i++)
		if (imapx_isp_wins[i].width == width &&
				imapx_isp_wins[i].height == height)
			return imapx_isp_wins + i;
	/* Not found? Then return the first format. */
	pr_info("[ISP] can not find support wins[%d,%d]\n", width, height);
	return NULL;
}
#endif

static struct i2c_board_info *imapx_isp_lookup_sen_dev(struct imapx_vp *vp)
{
	int i;

	for (i = 0; i < N_SUPPORT_SENSORS; i++)
		if (!strcmp(vp->sen_info->name, imap_camera_info[i].type))
			return &imap_camera_info[i];
	return NULL;
}

static  struct isp_fmt *imapx_isp_find_format(u32 pixelformat)
{
	int i;
	for (i = 0; i < N_IMAPX_ISP_FMTS; i++)
		if (imapx_isp_formats[i].pixelformat == pixelformat)
			return &imapx_isp_formats[i];
	/* Not found? Then return the first format. */
	pr_err("[ISP] failed to find support format[%x]\n", pixelformat);
	return imapx_isp_formats;
}

static struct isp_cfg_info *imapx_isp_find_cfg(struct imapx_vp *vp,
		struct v4l2_ctrl *ctrl)
{
	const struct isp_sen_cfg_info *sen_cfg;
	int i;
	/* set a default sensor cfg first*/
	sen_cfg = &isp_sen_cfgs[0];

	for (i = 0; i < N_SEN_CFGS; i++) {
		if (!strcmp(isp_sen_cfgs[i].name, vp->sen_info->name)) {
			sen_cfg = &isp_sen_cfgs[i];
			break;
		}
	}

	for (i = 0; i < ARRAY_SIZE(sen_cfg->ctrls_cfgs); i++) {
		if ((sen_cfg->ctrls_cfgs[i].ctrl_id == ctrl->id) &&
				(sen_cfg->ctrls_cfgs[i].ctrl_val == ctrl->val))
			return &sen_cfg->ctrls_cfgs[i].isp_cfg;
	}
	pr_err("[ISP] can not find isp cfg file\n");
	return NULL;
}

static const struct v4l2_pix_format default_pix_format = {
	.width      = VGA_WIDTH,
	.height     = VGA_HEIGHT,
	.pixelformat    = V4L2_PIX_FMT_NV12,
	.field      = V4L2_FIELD_NONE,
	.bytesperline   = VGA_WIDTH * 3 / 2,
	.sizeimage  = VGA_WIDTH * VGA_HEIGHT * 3 / 2,
};

static const enum v4l2_mbus_pixelcode default_mbus_code
						= V4L2_MBUS_FMT_YUYV8_2X8;

static int imapx_isp_set_power(struct imapx_vp *vp, uint32_t on)
{
	return imapx_isp_sen_power(vp, on);
}

static int imapx_isp_cfg_zoom_absolute(struct imapx_vp *vp, int ratio)
{

	if (ratio < 0 || ratio > MAX_ZOOM_ABSOLUT_RATIO) {
		pr_err("[ISP] Invalid zoom ratio %d, req 0~20!\n", ratio);
		return -1;
	}

	if (ratio == vp->crop.ratio)
		return 0;
	vp->crop.ratio = ratio;

	imapx_isp_scale_enable(vp, 0);
	imapx_isp_crop_enable(vp, 0);

	imapx_isp_crop_cfg(vp, &vp->crop);
	vp->scale.ihres = vp->crop.width;
	vp->scale.ivres = vp->crop.height;
	vp->scale.ohres = vp->outfmt.width;
	vp->scale.ovres = vp->outfmt.height;
	imapx_isp_scale_cfg(vp, &vp->scale);

	imapx_isp_crop_enable(vp, 1);
	imapx_isp_scale_enable(vp, 1);

	return 0;
}

static int  imapx_isp_cfg_effect(struct imapx_vp *vp, struct v4l2_ctrl *ctrl)
{
	struct isp_cfg_info *isp_cfg;
	pr_info("[ISP] %s\n", __func__);
	isp_cfg = imapx_isp_find_cfg(vp, ctrl);
	if (!isp_cfg) {
		pr_err("[ISP] %s:can not find isp cfg file\n", __func__);
		return 0;
	}
	if (isp_cfg->ief_enable) {
		imapx_isp_ief_cfg(vp, &isp_cfg->ief);
		imapx_isp_ief_enable(vp, isp_cfg->ief.enable);
		imapx_isp_ief_enable(vp, isp_cfg->ief.cscen);
	}
	if (isp_cfg->ee_enable) {
		imapx_isp_ee_cfg(vp, &isp_cfg->ee);
		imapx_isp_ee_enable(vp, isp_cfg->ee.enable);
	}
	if (isp_cfg->acc_enable) {
		imapx_isp_acc_cfg(vp, &isp_cfg->acc);
		imapx_isp_acc_enable(vp, isp_cfg->acc.enable);
	}
	if (isp_cfg->acm_enable) {
		imapx_isp_acm_cfg(vp, &isp_cfg->acm);
		imapx_isp_acm_enable(vp, isp_cfg->acm.enable);
	}
	return 0;
}


static int imapx_isp_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct imapx_vp *vp = container_of(ctrl->handler, struct imapx_vp,
			ctrl_handler);
	int ret;
	switch (ctrl->id) {
		case V4L2_CID_ZOOM_ABSOLUTE:
			ret = imapx_isp_cfg_zoom_absolute(vp, ctrl->val);
			break;
		case V4L2_CID_COLORFX:
		case V4L2_CID_SCENE_MODE:
			ret = imapx_isp_cfg_effect(vp, ctrl);
			break;
		default:
			 return -1;
	}
	return ret;
}

static int imapx_isp_querycap(struct file *file, void *priv,
		struct v4l2_capability *cap)
{
	struct imapx_vp *vp = video_drvdata(file);
	pr_info("[ISP] %s\n", __func__);
	strcpy(cap->driver, "imapx-isp");
	strcpy(cap->card, vp->sen_info->name);
	sprintf(cap->bus_info, "%s-%s-%d", vp->sen_info->interface,
			vp->sen_info->face, vp->sen_info->orientation);
	cap->version = 20140308;
	cap->device_caps = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int imapx_isp_enum_input(struct file *file, void *priv,
		struct v4l2_input *input)
{
	struct imapx_vp *vp = video_drvdata(file);

	pr_info("[ISP] %s\n", __func__);
	if (input->index || vp->sensor == NULL)
		return -EINVAL;

	input->type = V4L2_INPUT_TYPE_CAMERA;
	strlcpy(input->name, vp->sensor->name, sizeof(input->name));
	return 0;
}

static int imapx_isp_s_input(struct file *file, void *priv,
		unsigned int i)
{
	return i == 0 ? 0 : -EINVAL;
}

static int imapx_isp_g_input(struct file *file, void *priv,
		unsigned int *i)
{
	*i = 0;
	return 0;
}


static int imapx_isp_s_std(struct file *filp, void *priv, v4l2_std_id std)
{
	return 0;
}

static int imapx_isp_enum_fmt(struct file *file, void *priv,
		struct v4l2_fmtdesc *fmt)
{

	if (fmt->index >= N_IMAPX_ISP_FMTS)
		return -EINVAL;

	strlcpy(fmt->description, imapx_isp_formats[fmt->index].desc,
			sizeof(fmt->description));
	fmt->pixelformat = imapx_isp_formats[fmt->index].pixelformat;
	return 0;
}



static int imapx_isp_try_fmt_internal(struct imapx_vp *vp,
					struct v4l2_pix_format *upix,
					struct v4l2_pix_format *spix)
{
	struct v4l2_mbus_framefmt mbus_fmt;
	struct isp_fmt  *f = imapx_isp_find_format(upix->pixelformat);
	struct v4l2_frmsizeenum sensizes;
	int delt_old = 0;
	int delt_new, cur_idx = 0;

	int ret;

	if (upix == NULL || vp == NULL || spix == NULL) {
		pr_err("[ISP] error passing NULL Pointer\n");
		return -1;
	}
	sensizes.index = 0;
	*spix = *upix;
/* to find a sensor format*/
	while (!v4l2_subdev_call(vp->sensor, video,
				enum_framesizes, &sensizes)) {
		if (upix->width <= sensizes.discrete.width &&
				upix->height <= sensizes.discrete.height) {

			delt_new = (sensizes.discrete.width - upix->width) +
				(sensizes.discrete.height - upix->height);

			if (delt_new < delt_old || cur_idx == 0) {
				cur_idx++;
				delt_old = delt_new;
				spix->width = sensizes.discrete.width;
				spix->height = sensizes.discrete.height;
			}
		}
		sensizes.index++;
	}
	v4l2_fill_mbus_format(&mbus_fmt, spix, f->mbus_code);
	ret = v4l2_subdev_call(vp->sensor, video, try_mbus_fmt, &mbus_fmt);
	spix->width = mbus_fmt.width;
	spix->height = mbus_fmt.height;
	spix->field = mbus_fmt.field;
	spix->colorspace = mbus_fmt.colorspace;
	spix->bytesperline = ((spix->width+3)&~3); //* f->bpp >> 3;
	spix->sizeimage = spix->height * spix->width *3/2;//spix->bytesperline;
	upix->pixelformat = spix->pixelformat;               
	upix->field = spix->field;                           
	upix->bytesperline = spix->bytesperline; //upix->width * f->bpp >>3;                  
	upix->sizeimage = spix->sizeimage;//upix->bytesperline * upix->height;
	
	pr_info("[ISP] %s bpp = %d spix->bytesperline =%d upix->bytesperline =%d\n",
            __func__,f->bpp, spix->bytesperline,upix->bytesperline);
    
    
    return 0;
}

static int imapx_isp_try_fmt(struct file *file, void *priv,
		struct v4l2_format *fmt)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	struct v4l2_format sfmt;
	int ret;
	pr_info("[ISP] %s\n", __func__);
	//mutex_lock(&host->lock);
	ret = imapx_isp_try_fmt_internal(vp, &fmt->fmt.pix, &sfmt.fmt.pix);
	//mutex_unlock(&host->lock);
	return ret;
}


static int imapx_isp_g_fmt(struct file *file, void *priv,
						struct v4l2_format *fmt)
{
	struct imapx_vp *vp = video_drvdata(file);
	fmt->fmt.pix = vp->outfmt;
	return 0;
}

static int imapx_isp_s_fmt(struct file *file, void *priv,
		struct v4l2_format *fmt)
{
	struct imapx_vp *vp = video_drvdata(file);
	int ret;
	struct v4l2_format sfmt;
	struct v4l2_mbus_framefmt mbus_fmt;
	struct isp_fmt *f = imapx_isp_find_format(fmt->fmt.pix.pixelformat);

	pr_info("[ISP] %s\n", __func__);
	ret = imapx_isp_try_fmt_internal(vp, &fmt->fmt.pix, &sfmt.fmt.pix);
	vp->outfmt = fmt->fmt.pix;
	vp->payload = vp->outfmt.sizeimage;
	vp->infmt = sfmt.fmt.pix;
	vp->mbus_code = f->mbus_code;
	v4l2_fill_mbus_format(&mbus_fmt, &vp->infmt, vp->mbus_code);
	ret = v4l2_subdev_call(vp->sensor, video, s_mbus_fmt, &mbus_fmt);
	if (ret) {
		pr_err("[ISP] call sensor 's_mbus_fmt' failed with %d\n", ret);
		return ret;
	}

	pr_err("[ISP] Outfmt: win(%d, %d)\n",
			vp->outfmt.width, vp->outfmt.height);
	pr_err("[ISP] Infmt:  win(%d, %d)\n",
			vp->infmt.width, vp->infmt.height);

	ret = imapx_isp_set_fmt(vp);
	if (ret) {
		pr_err("[ISP] Config isp fmt failed with %d\n", ret);
		return ret;
	}
	return 0;
}


static int imapx_isp_reqbufs(struct file *file, void *priv,
		struct v4l2_requestbuffers *rb)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	int ret = 0;

	pr_info("[ISP] %s-memoymode-%d\n", __func__, rb->memory);
	if (rb->count)
		rb->count =  max_t(u32, IMAPX_ISP_REQ_BUFS_MIN, rb->count);
	ret = vb2_reqbufs(&vp->vb_queue, rb);
	if (ret < 0)
		return ret;
	if (rb->count && rb->count < IMAPX_ISP_REQ_BUFS_MIN) {
		rb->count = 0;
		vb2_reqbufs(&vp->vb_queue, rb);
		ret = -ENOMEM;
	}
	if (rb->memory == V4L2_MEMORY_DMABUF) {
		host->dmmu_hdl = vb2_imapx_dmmu_init(11);
		if (host->dmmu_hdl) {
			ret = vb2_imapx_dmmu_enable(host->dmmu_hdl, 1);
			if (ret)
				pr_err("[ISP] can not enable dmmu! exit errno %d\n", ret);
		} else
			ret = -1;
	}

	return ret;
}

static int imapx_isp_querybuf(struct file *file, void *priv,
							struct v4l2_buffer *buf)
{
	struct imapx_vp *vp = video_drvdata(file);
	return vb2_querybuf(&vp->vb_queue, buf);
}

static int imapx_isp_qbuf(struct file *file, void *priv,
							struct v4l2_buffer *buf)
{
	struct imapx_vp *vp = video_drvdata(file);
	return vb2_qbuf(&vp->vb_queue, buf);
}

static int imapx_isp_dqbuf(struct file *file, void *priv,
							struct v4l2_buffer *buf)
{
	struct imapx_vp *vp = video_drvdata(file);
	return vb2_dqbuf(&vp->vb_queue, buf, file->f_flags & O_NONBLOCK);
}

static int imapx_isp_streamon(struct file *file, void *priv,
		enum v4l2_buf_type type)
{
	struct imapx_vp *vp = video_drvdata(file);
	int ret;

	pr_info("[ISP] %s\n", __func__);
	ret = vb2_streamon(&vp->vb_queue, type);
	vp->state = HOST_RUNNING | HOST_STREAMING;
	return ret;
}

static int imapx_isp_streamoff(struct file *file, void *priv,
		enum v4l2_buf_type type)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	struct isp_buffer	*buf;
	unsigned long flags;
	int ret;

	pr_info("[ISP] %s\n", __func__);
	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	ret = vb2_streamoff(&vp->vb_queue, type);
	spin_lock_irqsave(&host->slock, flags);
	vp->state &= ~(HOST_RUNNING | HOST_STREAMING);
	/* Release unused buffers */
	while (!list_empty(&vp->pending_buf_q)) {
		buf = isp_pending_queue_pop(vp);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	while (!list_empty(&vp->active_buf_q)) {
		buf = isp_active_queue_pop(vp);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&host->slock, flags);
	return ret;
}

static int imapx_isp_enum_framesizes(struct file *file, void *priv,
		struct v4l2_frmsizeenum *fsize)
{
	struct imapx_vp *vp = video_drvdata(file);
	int i;

	pr_info("[ISP] %s\n", __func__);
	if (fsize->index == 0) {
		struct v4l2_frmsizeenum sensizes;
		sensizes.index = 0;
		while (!v4l2_subdev_call(vp->sensor, video,
					enum_framesizes, &sensizes) &&
				(sensizes.index < N_IMAPX_ISP_SIZES)) {
			vp->sensor_wins[sensizes.index].width = sensizes.discrete.width;
			vp->sensor_wins[sensizes.index].height = sensizes.discrete.height;
			/*
			   pr_info("[ISP]--get sen framesize--%d-(%d,%d)\n", sensizes.index,
			   sensizes.discrete.width, sensizes.discrete.height);
			   */
			sensizes.index++;
		}
		vp->sensor_wins[sensizes.index].width = 0;
		vp->sensor_wins[sensizes.index].height = 0;
	}

	for (i = 0; vp->sensor_wins[i].width != 0; i++) {
		/* zoom in */
		if (imapx_isp_wins[fsize->index].width <= vp->sensor_wins[i].width &&
				imapx_isp_wins[fsize->index].height <= vp->sensor_wins[i].height) {
			fsize->discrete.width = imapx_isp_wins[fsize->index].width;
			fsize->discrete.height = imapx_isp_wins[fsize->index].height;
			fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;

			pr_info("[ISP]--enum framesize(%d) %d,%d\n", fsize->index,
					fsize->discrete.width, fsize->discrete.height);
			return 0;
		}
	}
	return -1;
}

int imapx_isp_enum_frameintervals (struct file *file, void *fh,
        struct v4l2_frmivalenum *fival)
{
	struct imapx_vp *vp = video_drvdata(file);
	int i;

    pr_info("[ISP] %s\n", __func__);
    return v4l2_subdev_call(vp->sensor, video,
            enum_frameintervals, fival);

}

static const struct v4l2_ctrl_ops imapx_isp_ctrl_ops = {
	.s_ctrl = imapx_isp_s_ctrl,
};

static const struct v4l2_ioctl_ops imapx_isp_ioctl_ops = {
	.vidioc_querycap    = imapx_isp_querycap,
	.vidioc_enum_input  = imapx_isp_enum_input,
	.vidioc_g_input     = imapx_isp_g_input,
	.vidioc_s_input     = imapx_isp_s_input,
	.vidioc_s_std       = imapx_isp_s_std,
	.vidioc_enum_fmt_vid_cap = imapx_isp_enum_fmt,
	.vidioc_try_fmt_vid_cap = imapx_isp_try_fmt,
	.vidioc_g_fmt_vid_cap   = imapx_isp_g_fmt,
	.vidioc_s_fmt_vid_cap   = imapx_isp_s_fmt,
	.vidioc_reqbufs     = imapx_isp_reqbufs,
	.vidioc_querybuf    = imapx_isp_querybuf,
	.vidioc_qbuf        = imapx_isp_qbuf,
	.vidioc_dqbuf       = imapx_isp_dqbuf,
	.vidioc_streamon    = imapx_isp_streamon,
	.vidioc_streamoff   = imapx_isp_streamoff,
	.vidioc_enum_framesizes = imapx_isp_enum_framesizes,
    .vidioc_enum_frameintervals = imapx_isp_enum_frameintervals,    
};                                                            

static int imapx_isp_enable_clk(struct imapx_isp *host)
{
	int ret = 0;
	unsigned long clkrate;
	host->busclk = clk_get_sys("imap-isp", "imap-isp");
	if (IS_ERR(host->busclk)) {
		pr_err("[ISP] Failed to get clk 'imap-isp'\n");
		return PTR_ERR(host->busclk);
	}

	clkrate = utlz_arbitrate_request(200000000, ARB_ISP);
	ret = clk_set_rate(host->busclk, clkrate);
	if (ret < 0) {
		pr_err("[ISP] Failed to set clk 'imap-isp' to 200MHZ\n");
		return -1;
	}
	clk_prepare_enable(host->busclk);

	host->oclk = clk_get_sys("imap-camo", "imap-camo");
	if (IS_ERR(host->oclk)) {
		pr_err("[ISP] Failed to get clk 'imap-camo'\n");
		return PTR_ERR(host->oclk);
	}
	ret = clk_set_rate(host->oclk, 24000000);
	if (ret < 0) {
		pr_err("[ISP] Failed to set clk 'imap-camo' to 24MHZ\n");
		return -1;
	}
	clk_prepare_enable(host->oclk);

	host->osdclk = clk_get_sys("imap-isp-osd", "imap-isp-osd");
	if (IS_ERR(host->osdclk)) {
		pr_err("[ISP] Failed to get clk 'isp-osd'\n");
		return PTR_ERR(host->osdclk);
	}
	ret = clk_set_rate(host->osdclk, 72000000);
	if (ret < 0) {
		pr_err("[ISP] Failed to set clk 'imap-isd-osd' to 96MHZ\n");
		return -1;
	}
	clk_prepare_enable(host->osdclk);

	return 0;
}

static int imapx_isp_remove_clk(struct imapx_isp *host)
{

	if (host->busclk)
		clk_disable_unprepare(host->busclk);
	utlz_arbitrate_clear(ARB_ISP);

	if (host->oclk)
		clk_disable_unprepare(host->oclk);

	if (host->osdclk)
		clk_disable_unprepare(host->osdclk);

	return 0;
}



static int imapx_isp_open(struct file *file)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	int ret;


	pr_err("[ISP] %s\n", __func__);
	if (file_opened) {
		pr_err("[ISP] device has been opend, exit\n");
		return -1;
	}
	if (host->pdata->init)
		host->pdata->init();
	imapx_isp_enable_clk(host);

	vp->buf_index = 0;
	vp->state = HOST_PENDING;
	file->private_data = host;
	memset(&vp->crop, 0x00, sizeof(struct crop_info));
	memset(&vp->scale, 0x00, sizeof(struct scale_info));
	ret = imapx_isp_set_power(vp, 1);
	imapx_isp_host_init(vp);
	/* here we init sensor first */
	ret = v4l2_subdev_call(vp->sensor, core, init, 0);
	if (ret) {
		pr_err("[ISP] Call sensor 'init' failed with\n");
		goto err_open;
	}
	host->cur_vp = vp;
	file_opened++;
	return 0;
err_open:
	imapx_isp_set_power(vp, 0);
	imapx_isp_remove_clk(host);
	if (host->pdata)
		host->pdata->deinit();
	return ret;
}

static int imapx_isp_close(struct file *file)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	struct isp_buffer	*buf;
	int	ret, retry = 10;

	pr_err("[ISP] %s\n", __func__);
	mutex_lock(&host->lock);
	if (!file_opened) {
		pr_err("[ISP] device is not opened, exit\n");
		mutex_unlock(&host->lock);
		return -1;
	}

	vp->state &= ~(HOST_RUNNING | HOST_STREAMING);
	if (vp->vb_queue.streaming) {
		vb2_streamoff(&vp->vb_queue, vp->vb_queue.type);
		pr_warn("[ISP] abort to exit!\n");
	}

	imapx_isp_host_deinit(vp);
	ret = imapx_isp_set_power(vp, 0);
	if (host->dmmu_hdl) {
		while (retry && vb2_imapx_dmmu_enable(host->dmmu_hdl, 0)) {
			/* Make sure dmmu is disabled here*/
			retry--;
			msleep(20);
		}
		vb2_imapx_dmmu_deinit(host->dmmu_hdl);
		host->dmmu_hdl = NULL;
	}
	while (!list_empty(&vp->pending_buf_q)) {
		buf = isp_pending_queue_pop(vp);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	while (!list_empty(&vp->active_buf_q)) {
		buf = isp_active_queue_pop(vp);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	imapx_isp_remove_clk(host);
	if (host->pdata)
		host->pdata->deinit();
	host->cur_vp = NULL;
	file_opened--;
	mutex_unlock(&host->lock);
	return ret;
}


static unsigned int imapx_isp_poll(struct file *file,
		struct poll_table_struct *wait)
{
	struct imapx_vp *vp = video_drvdata(file);
	struct imapx_isp *host = vp->isp_dev;
	int ret;
	mutex_lock(&host->lock);
	ret = vb2_poll(&vp->vb_queue, file, wait);
	mutex_unlock(&host->lock);
	return ret;
}

static int imapx_isp_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct imapx_vp *vp = video_drvdata(file);
	return vb2_mmap(&vp->vb_queue, vma);
}

static int queue_setup(struct vb2_queue *vq,
			const struct v4l2_format *pfmt, unsigned int *num_buffers,
			unsigned int *num_planes, unsigned int sizes[],
			void *allocators[])
{
	struct imapx_vp *vp = vb2_get_drv_priv(vq);
	struct imapx_isp *host = vp->isp_dev;
	const struct v4l2_pix_format *pix = NULL;
	struct isp_fmt *f = NULL;

	pr_info("[ISP] %s\n", __func__);
	*num_planes = 1;
	/*
	 * FIXME only one plane support now
	 */
	if (pfmt) {
		pix = &pfmt->fmt.pix;
		f = imapx_isp_find_format(pfmt->fmt.pix.pixelformat);
		sizes[0] = pix->width * pix->height * f->bpp >> 3;
	} else
		sizes[0] = vp->outfmt.sizeimage;

	allocators[0] = host->alloc_ctx;
	return 0;
}


static int buffer_prepare(struct vb2_buffer *vb)
{
	struct imapx_vp *vp = vb2_get_drv_priv(vb->vb2_queue);
	if (vb2_plane_size(vb, 0) < vp->payload) {
		pr_err("[ISP] vb2 buffer less than payload\n");
		return -EINVAL;
	}
	vb2_set_plane_payload(vb, 0, vp->payload);
	return 0;
}


static void imapx_isp_prepare_addr(struct imapx_vp *vp,
								struct isp_buffer *buf)
{
	switch (vp->outfmt.pixelformat) {
		case V4L2_PIX_FMT_NV12:
			buf->paddr.y = vb2_dma_contig_plane_dma_addr(&buf->vb, 0);
			buf->paddr.y_len = vp->outfmt.width * vp->outfmt.height;
			vp->dmause.ch0 = 1;
			buf->paddr.cb = (u32)(buf->paddr.y + (buf->paddr.y_len));
			buf->paddr.cb_len = buf->paddr.y_len >> 1;
			vp->dmause.ch1 = 1;
			buf->paddr.cr = 0;
			buf->paddr.cr_len = 0;
			vp->dmause.ch2 = 0;
			break;
		default:
			break;
	}
#if 0
	pr_err("[ISP]- Y-%x-%d, CB-%x-%d, CR-%x-%d\n",
					buf->paddr.y, buf->paddr.y_len,
					buf->paddr.cb, buf->paddr.cb_len,
					buf->paddr.cr, buf->paddr.cr_len);
#endif
	return;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct imapx_vp *vp = vb2_get_drv_priv(vb->vb2_queue);
	struct imapx_isp *host = vp->isp_dev;
	struct isp_buffer *buf = container_of(vb, struct isp_buffer, vb);
	unsigned long flags;

	spin_lock_irqsave(&host->slock, flags);
	imapx_isp_prepare_addr(vp, buf);

	if (!(vp->state & HOST_STREAMING) && vp->active_buffers < 2) {
		buf->index = vp->buf_index;
		imapx_isp_cfg_addr(vp, buf, buf->index);
		imapx_isp_cfg_addr(vp, buf, buf->index + 2);
		vp->buf_index = !vp->buf_index;
		isp_active_queue_add(vp, buf);
	} else
		isp_pending_queue_add(vp, buf);
	spin_unlock_irqrestore(&host->slock, flags);
}

static int start_streaming(struct vb2_queue *vq, unsigned int count)
{

	struct imapx_vp *vp = vb2_get_drv_priv(vq);
	struct imapx_isp *host = vp->isp_dev;
	unsigned long flags;
	pr_err("[ISP] %s\n", __func__);
	vp->cur_frame_index = 0;
	vp->frame_sequence = 0;
	spin_lock_irqsave(&host->slock, flags);

	imapx_isp_scale_enable(vp, 0);
	imapx_isp_crop_enable(vp, 0);
	imapx_isp_crop_cfg(vp, &vp->crop);
	vp->scale.ihres = vp->crop.width;
	vp->scale.ivres = vp->crop.height;
	vp->scale.ohres = vp->outfmt.width;
	vp->scale.ovres = vp->outfmt.height;
	imapx_isp_scale_cfg(vp, &vp->scale);
	imapx_isp_crop_enable(vp, 1);
	imapx_isp_scale_enable(vp, 1);

	imapx_isp_unmask_irq(vp, INT_MASK_ALL);
	v4l2_subdev_call(vp->sensor, video, s_stream, 1);
	imapx_isp_enable_capture(vp);
	spin_unlock_irqrestore(&host->slock, flags);
	return 0;
}

static int stop_streaming(struct vb2_queue *vq)
{
	struct imapx_vp *vp = vb2_get_drv_priv(vq);

	pr_err("[ISP] %s\n", __func__);
	imapx_isp_disable_capture(vp);
	return 0;
}


static const struct vb2_ops imapx_isp_qops = {
	.queue_setup     = queue_setup,
	.buf_prepare     = buffer_prepare,
	.buf_queue	     = buffer_queue,
	.start_streaming = start_streaming,
	.stop_streaming  = stop_streaming,
};



static const struct v4l2_file_operations imapx_isp_fops = {
	.owner      = THIS_MODULE,
	.open       = imapx_isp_open,
	.release    = imapx_isp_close,
	.poll       = imapx_isp_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap       = imapx_isp_mmap,
};

static struct video_device imapx_isp_vd = {
	.name       = "imapx-isp",
	.minor      = -1,
	.fops       = &imapx_isp_fops,
	.ioctl_ops  = &imapx_isp_ioctl_ops,
	.release    = video_device_release_empty, /* Check this */
};

static irqreturn_t imapx_isp_irq_handler(int irq, void *priv)
{
	struct imapx_isp *host = priv;
	struct imapx_vp *vp = host->cur_vp;
	uint32_t status = isp_read(host, ISP_GLB_INTS);
	if (status & INT_STAT_SYNC) {
		/* Discard  some frames */
		if (vp->cur_frame_index < DISCARD_FRAME_NUM) {
			/*
			pr_info("[ISP] discard %dth frame\n", vp->cur_frame_index);
			*/
			isp_write(host, ISP_GLB_INTS, status);
			vp->cur_frame_index++;
			return IRQ_HANDLED;
		}

		if (list_empty(&vp->pending_buf_q))
			pr_err("[ISP] pending_buf_q empty\n");
		if (list_empty(&vp->active_buf_q))
			pr_err("[ISP] active_buf_q  empty\n");
		if (!list_empty(&vp->pending_buf_q) && (vp->state & HOST_RUNNING) &&
				!list_empty(&vp->active_buf_q)) {
			unsigned int index;
			struct isp_buffer *vbuf;
			struct timeval *tv;
			struct timespec ts;
			index = (isp_read(host, ISP_CH0DMA_CC1) >> 28) & 0x3;
			index = ((index != 0) ? (index - 1) : 3) & 0x1;
			ktime_get_ts(&ts);
			vbuf = isp_active_queue_peek(vp, index);
			if (!WARN_ON(vbuf == NULL)) {
				tv = &vbuf->vb.v4l2_buf.timestamp;
				tv->tv_sec = ts.tv_sec;
				tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
				vbuf->vb.v4l2_buf.sequence = vp->frame_sequence++;
				vb2_buffer_done(&vbuf->vb, VB2_BUF_STATE_DONE);
				/* Set up an empty buffer at the DMA engine */
				vbuf = isp_pending_queue_pop(vp);
				vbuf->index = index;
				imapx_isp_cfg_addr(vp, vbuf, index);
				imapx_isp_cfg_addr(vp, vbuf, index + 2);
				isp_active_queue_add(vp, vbuf);
			}
		}
	}
	isp_write(host, ISP_GLB_INTS, status);

	return IRQ_HANDLED;
}


static int imapx_isp_register_video_node(struct imapx_isp *host, int idx)
{

	struct imapx_vp *vp = &host->vp[idx];
	struct i2c_adapter *adapter;
	struct i2c_board_info *sen_dev;
	int ret = 0;

	if (!host->pdata->sen_info[idx].exist) {
		pr_warn("[ISP] sensor%d not exist\n", idx);
		return -ENXIO;
	}

	vp->sen_info = &host->pdata->sen_info[idx];
	pr_info("[ISP] Sensor Info:\n"
			"exist:		%d\n"
			"name:		%s\n"
			"ctrl_bus:	%s\n"
			"interface:	%s\n"
			"face:		%s\n"
			"pw_iovdd:	%s\n"
			"pw_dvdd:	%s\n"
			"bus_chn:	%d\n"
			"pdown_pin:	%d\n"
			"reset_pin:	%d\n"
			"orientation:	%d\n"
			"addr:		%d\n"
			"mclk:		%d\n",
			vp->sen_info->exist, vp->sen_info->name,
			vp->sen_info->ctrl_bus, vp->sen_info->interface,
			vp->sen_info->face, vp->sen_info->iovdd,
			vp->sen_info->dvdd, vp->sen_info->bus_chn,
			vp->sen_info->pdown_pin, vp->sen_info->reset_pin,
			vp->sen_info->orientation, vp->sen_info->addr,
			vp->sen_info->mclk);

	sen_dev = imapx_isp_lookup_sen_dev(vp);
	if (!sen_dev) {
		pr_err("[ISP] can not find driver for %s\n", vp->sen_info->name);
		return -ENXIO;
	}

	adapter = i2c_get_adapter(vp->sen_info->bus_chn);
	if (adapter == NULL) {
		pr_err("[ISP] failed to get i2c adapter %d\n",
				vp->sen_info->bus_chn);
		return -ENXIO;
	}

	if (vp->sen_info->iovdd[0]) {
		vp->iovdd_rg = regulator_get(NULL, vp->sen_info->iovdd);
		if (!vp->iovdd_rg) {
			pr_err("[ISP] get sensor %s IOVDD regulator err\n",
					vp->sen_info->name);
			goto put_adatpter;
		}
	}
	if (vp->sen_info->dvdd[0]) {
		vp->dvdd_rg = regulator_get(NULL, vp->sen_info->dvdd);
		if (!vp->dvdd_rg) {
			pr_err("[ISP] get sensor %s DVDD regulator err\n",
					vp->sen_info->name);
			goto release_iovdd;
		}
	}

	vp->outfmt = vp->infmt = default_pix_format;
	vp->mbus_code = default_mbus_code;
	vp->state = HOST_PENDING;
	INIT_LIST_HEAD(&vp->pending_buf_q);
	INIT_LIST_HEAD(&vp->active_buf_q);

	sprintf(vp->v4l2_dev.name, "%s", vp->sen_info->face);
	ret = v4l2_device_register(NULL, &vp->v4l2_dev);
	if (ret) {
		pr_err("[ISP] Unable to register v4l2 device\n");
		goto release_dvdd;
	}

	/*
	 * connect subdev
	 */
	vp->sensor = v4l2_i2c_new_subdev_board(&vp->v4l2_dev, adapter,
			sen_dev, NULL);

	if (!vp->sensor) {
		pr_err("[ISP] No driver for %s\n", vp->sen_info->name);
		goto unregister_v4l2_dev;
	}

	/*
	 *  register vb2 buf ops
	 */
	vp->vb_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vp->vb_queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	vp->vb_queue.mem_ops = &vb2_dma_contig_memops;
	vp->vb_queue.ops	= &imapx_isp_qops;
	vp->vb_queue.drv_priv = vp;
	vp->vb_queue.timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	vp->vb_queue.buf_struct_size = sizeof(struct isp_buffer);
	ret = vb2_queue_init(&vp->vb_queue);
	if (ret) {
		pr_err("[ISP] vb2 queue init err return %d\n", ret);
		goto unregister_v4l2_dev;
	}
	/*
	 * register ctrl_handler
	 */
	ret = v4l2_ctrl_handler_init(&vp->ctrl_handler, 2);
	if (ret) {
		pr_err("[ISP] Unable to init v4l2 ctrl handler\n");
		goto vb2_queue_err;
	}
	vp->zoom_absolute = v4l2_ctrl_new_std(&vp->ctrl_handler,
			&imapx_isp_ctrl_ops, V4L2_CID_ZOOM_ABSOLUTE,
			0, MAX_ZOOM_ABSOLUT_RATIO, 1, 0);
	/*
	v4l2_ctrl_new_std_menu(&vp->ctrl_handler, &imapx_isp_ctrl_ops,
			V4L2_CID_SCENE_MODE, V4L2_SCENE_MODE_SUNSET, ~0x1fcd,
			V4L2_SCENE_MODE_NONE);
			*/
	vp->ctrl_colorfx = v4l2_ctrl_new_std_menu(&vp->ctrl_handler,
			&imapx_isp_ctrl_ops, V4L2_CID_COLORFX,
			V4L2_COLORFX_SOLARIZATION,
			~0x263f, V4L2_COLORFX_NONE);

	ret = v4l2_ctrl_add_handler(&vp->ctrl_handler,
			vp->sensor->ctrl_handler, NULL);
	if (ret < 0) {
		pr_err("[ISP] v4l2 add handler err %d\n", ret);
		goto vb2_queue_err;
	}
	/*
	 *  Register v4l2 device
	 */
	vp->vdev = imapx_isp_vd;
	vp->vdev.ctrl_handler = &vp->ctrl_handler;
	vp->vdev.v4l2_dev = &vp->v4l2_dev;
	vp->vdev.lock = &host->lock;
	ret = video_register_device(&vp->vdev, VFL_TYPE_GRABBER, -1);
	if (ret) {
		pr_err("[ISP]--failed to register v4l2 device: %d\n", ret);
		goto vb2_queue_err;
	}
	video_set_drvdata(&vp->vdev, vp);

	/*
	 * detect sensor
	 */

	imapx_isp_sen_power(vp, 1);
	if (v4l2_subdev_call(vp->sensor, core, init, 1)) {
		pr_err("[ISP] %s detect failed!\n", vp->sen_info->name);
		goto release_video_device;
	}
	imapx_isp_sen_power(vp, 0);

	vp->isp_dev = host;
	return 0;
release_video_device:
	imapx_isp_sen_power(vp, 0);
	video_unregister_device(&vp->vdev);
vb2_queue_err:
	vb2_queue_release(&vp->vb_queue);
unregister_v4l2_dev:
	v4l2_device_unregister(&vp->v4l2_dev);
release_dvdd:
	regulator_put(vp->dvdd_rg);
release_iovdd:
	regulator_put(vp->iovdd_rg);
put_adatpter:
	i2c_put_adapter(adapter);
	return -1;
}

static void imapx_isp_unregister_video_node(struct imapx_isp *host, int idx)
{
	struct imapx_vp *vp = &host->vp[idx];


	if (!host->pdata->sen_info[idx].exist) {
		pr_warn("[ISP] sensor%d not exist\n", idx);
		return;
	}

	if (vp->dvdd_rg)
		regulator_put(vp->dvdd_rg);
	if (vp->iovdd_rg)
		regulator_put(vp->iovdd_rg);

	if (video_is_registered(&vp->vdev)) {
		v4l2_ctrl_handler_free(vp->vdev.ctrl_handler);
		video_unregister_device(&vp->vdev);
	}
	v4l2_device_unregister(&vp->v4l2_dev);
}

static int imapx_isp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct imapx_cam_board *pdata = pdev->dev.platform_data;
	struct imapx_isp *host;
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	int ret = 0, idx;

	pr_err("[ISP] infoTM imapx Image signal processor driver\n");

	host = devm_kzalloc(dev, sizeof(struct imapx_isp), GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	memset(host, 0x00, sizeof(struct imapx_isp));

	host->dev = dev;
	host->cur_vp = NULL;
	host->dmmu_hdl = NULL;

	/* config GPIO mode and init ISP power*/
	host->pdata = pdata;
	if (host->pdata->init)
		host->pdata->init();
	else {
		dev_err(&pdev->dev, "[ISP] No way to init ISP controller\n");
		return -ENXIO;
	}

	/*io_base*/
	host->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(host->base)) {
		dev_err(&pdev->dev, "[ISP] failed to map io base\n");
		return PTR_ERR(host->base);
	}
	/*request irq*/
	host->irq = platform_get_irq(pdev, 0);
	if (host->irq <= 0) {
		dev_err(&pdev->dev, "[ISP] failed to get irq %d\n", host->irq);
		return -ENXIO;
	}
	ret = devm_request_irq(dev, host->irq, imapx_isp_irq_handler,
			0, dev_name(dev), host);
	if (ret < 0) {
		dev_err(&pdev->dev, "[ISP] failed to install IRQ: %d\n", ret);
		return ret;
	}

	/* enable host&sensor clock */
	if (imapx_isp_enable_clk(host) < 0) {
		dev_err(&pdev->dev, "[ISP] failed to enable clock\n");
		return -ENXIO;
	}
	/*
	 * Tell V4L2
	 */

	host->alloc_ctx = vb2_dma_contig_init_ctx(dev);
    /* Register /dev/videoX */
	for (idx = 0; idx < IMAPX_MAX_SENSOR_NUM; idx++)
		imapx_isp_register_video_node(host, idx);
	mutex_init(&host->lock);
	spin_lock_init(&host->slock);
	platform_set_drvdata(pdev, host);
	imapx_isp_remove_clk(host);
	if (host->pdata)
		host->pdata->deinit();

	return 0;
}


static int imapx_isp_remove(struct platform_device *pdev)
{
	struct imapx_isp *host = platform_get_drvdata(pdev);
	int idx;

	free_irq(host->irq, host);
	for (idx = 0; idx < IMAPX_MAX_SENSOR_NUM; idx++)
		imapx_isp_unregister_video_node(host, idx);
	vb2_dma_contig_cleanup_ctx(host->alloc_ctx);
	imapx_isp_remove_clk(host);
	kfree(host);
	return 0;
}

static int imapx_isp_plat_suspend(struct device *dev)
{
	pr_err("[ISP] %s\n", __func__);
	return 0;
}

static int imapx_isp_plat_resume(struct device *dev)
{
	pr_err("[ISP] %s\n", __func__);
	return 0;
}


SIMPLE_DEV_PM_OPS(imapx_isp_pm_ops,
		imapx_isp_plat_suspend, imapx_isp_plat_resume);

static struct platform_driver imapx_isp_driver = {
	.probe      = imapx_isp_probe,
	.remove     = imapx_isp_remove,
	.driver = {
		.name   = "imapx-isp",
		.owner  = THIS_MODULE,
//		.pm = &imapx_isp_pm_ops,
	}
};

static int __init imapx_isp_init(void)
{
	return platform_driver_register(&imapx_isp_driver);
}
module_init(imapx_isp_init);

static void __exit imapx_isp_exit(void)
{
	platform_driver_unregister(&imapx_isp_driver);
}
module_exit(imapx_isp_exit);
MODULE_AUTHOR("peter");
MODULE_DESCRIPTION("imapx ISP driver");
MODULE_LICENSE("GPL2");


