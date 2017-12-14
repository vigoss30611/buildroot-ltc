#include <media/v4l2-ctrls.h>          
#include <media/v4l2-ioctl.h>          
#include <media/videobuf2-core.h>      
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-device.h>
#include <linux/v4l2-mediabus.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_data/camera-imapx.h>


#define HOST_PENDING		(1 << 0)
#define HOST_RUNNING        (1 << 1)
#define HOST_STREAMING        (1 << 2)
#define HOST_OFF        (1 << 3)
#define HOST_CONFIG		(1 << 4)

#define isp_write(_host, _off, _val) writel(_val, (_host)->base + (_off))
#define isp_read(_host, _off)    readl((_host)->base + (_off))

/*                                                                   
 * struct isp_fmt - pixel format description                        
 * @bpp:      number of luminance bytes per pixel                    
 */                                                                   
struct isp_fmt {                                                    
	__u8 *desc;                         
	__u32 pixelformat;                  
	int bpp;   /* bits per pixel */    
	enum v4l2_mbus_pixelcode mbus_code; 
};                                                                    

static const struct isp_fmt imapx_isp_formats[] = {
	/* FIXME YUV420 */
	{                                           
		.desc       = "YUV 4:2:0", 
		.pixelformat     =  V4L2_PIX_FMT_NV12,         
		.bpp		=  12,
		.mbus_code		= V4L2_MBUS_FMT_YUYV8_2X8,
	}
};
#define N_IMAPX_ISP_FMTS ARRAY_SIZE(imapx_isp_formats)

struct isp_win_size {
	int width;
	int height;
};

static const struct isp_win_size imapx_isp_wins[] = 
{
	/* QVGA */
	{
		.width = 320,
		.height = 240,
	},
#if 0
	/*  CIF */
	{
		.width = 352,
		.height = 288,
	},
#endif
	/* VGA */
	{
		.width = 640,
		.height = 480,
	},
	/*SVGA */
	{
		.width = 800,
		.height = 600,
	},
	/* XGA */
	{
		.width = 1024,
		.height = 768,
	},
	/* SXGA */
	{
		.width = 1280,
		.height = 1024,
	},
	/* UXGA */
	{
		.width = 1600,
		.height = 1200,
	},
};

#define N_IMAPX_ISP_SIZES ARRAY_SIZE(imapx_isp_wins)


struct isp_addr { 
	u32 y;         
	u32 cb;        
	u32 cr;        
	u32 y_len;
	u32 cb_len;
	u32 cr_len;
};                 

/*
 * struct isp_buffer - host buffer struct description
 * @vb		vb2_buffer, should be first; 
 * @list    add to pending or active queue
 * @index   which DMA buffer      
 */
struct isp_buffer {
	struct vb2_buffer vb;
	struct list_head list;
	struct isp_addr   paddr;
	unsigned int index;
};

struct dma_use_table {
	int ch0;
	int ch1;
	int ch2;
};

/*
 * Scale infomation
 */
struct scale_info {
	int ivres;
	int ihres;
	int ovres;
	int ohres;
	int vscaling;
	int hscaling;
};

/*
 * Crop infomation
 */
struct crop_info {
	int enable;
	int ratio;
	int xstart;
	int ystart;
	int xend;
	int yend;
	int width;
	int height;
};

/*
 *  IE && IECSC infomation
 */
#define ISP_IEF_TYPE_NORMAL         0
#define ISP_IEF_TYPE_GRAY           1
#define ISP_IEF_TYPE_NEGATIVE       2
#define ISP_IEF_TYPE_SOLARIZATION   3
#define ISP_IEF_TYPE_SEPIA          4
#define ISP_IEF_TYPE_EMBOSS         5
#define ISP_IEF_TYPE_SKETCH_SOBEL   6
#define ISP_IEF_TYPE_SKETCH_LP      7
#define ISP_IEF_TYPE_COLOR_SELECT   8

#define ISP_IEF_COLOR_SELECT_MODE_R 0
#define ISP_IEF_COLOR_SELECT_MODE_G 1
#define ISP_IEF_COLOR_SELECT_MODE_B 2


struct ief_csc_matrix {
	int   coef11;
	int   coef12;
	int   coef13;
	int   coef21;
	int   coef22;
	int   coef23;
	int   coef31;
	int   coef32;
	int   coef33;
	int   oft_a; 
	int   oft_b; 
};

struct isp_ief_select_matrix {
	int   mode;
	int   tha; 
	int   thb; 
};

struct ief_info {
	uint8_t enable;
	uint8_t cscen;
	uint32_t type;
	uint32_t coeffcr;
	uint32_t coeffcb;
	struct ief_csc_matrix cscmat;
	struct isp_ief_select_matrix selmat;
};

/*
 *  EE infomation
 */

#define ISP_EE_GAUSS_MODE_0  0
#define ISP_EE_GAUSS_MODE_1  1

#define ISP_ROUND_MINUS     0
#define ISP_ROUND_NEAREST   1

struct isp_ee_thr_matrix {
	uint32_t   hth; 
	uint32_t   vth; 
	uint32_t   d0th;
	uint32_t   d1th;
};

struct ee_info {
	uint8_t enable;
	uint32_t           coefw;  
	uint32_t           coefa;  
	uint32_t           rdmode; 
	uint8_t             gasen;  
	uint32_t           gasmode;
	uint32_t           errth;  
	struct isp_ee_thr_matrix   thrmat; 
};

/*
 * ACC infomation
 *
 *
 */
#define ISP_ACC_LUTB_CONTROL_MODE_NORMAL    0
#define ISP_ACC_LUTB_CONTROL_MODE_CPU       1

struct isp_acc_co_matrix {
	uint32_t coefa;
	uint32_t coefb;
	uint32_t coefc;
	uint32_t coefd;
};

struct isp_acc_ro_matrix {
	uint32_t roahi;
	uint32_t roalo;
	uint32_t robhi;
	uint32_t roblo;
	uint32_t rochi;
	uint32_t roclo;
};

struct acc_info {
	uint8_t enable;	uint32_t coefe;
	uint32_t rdmode;
	uint32_t lutapbs;
	uint32_t histround;
	uint32_t histframe;
	struct isp_acc_co_matrix comat;
	struct isp_acc_ro_matrix romat;
};


/*
 * ACM infomation
 */
struct isp_acm_thr_matrix {
	uint32_t tha;
	uint32_t thb;
	uint32_t thc;
};

struct isp_acm_coef_matrix {
	uint32_t       coefp;
	uint32_t       coefr;
	uint32_t       coefg;
	uint32_t       coefb;
	uint32_t       m0r;
	uint32_t       m1r;
	uint32_t       m2r;
	uint32_t       m3r;
	uint32_t       m4r;
	uint32_t       m0g;
	uint32_t       m1g;
	uint32_t       m2g;
	uint32_t       m3g;
	uint32_t       m4g;
	uint32_t       m0b;
	uint32_t       m1b;
	uint32_t       m2b;
	uint32_t       m3b;
	uint32_t       m4b;
};

struct acm_info {
	uint8_t enable;
	uint32_t rdmode;
	uint32_t ths;
	struct isp_acm_thr_matrix thrmat;
	struct isp_acm_coef_matrix comat;
};

struct isp_cfg_info {
	/*TODO only support 4 now */
	uint8_t	ief_enable;
	uint8_t	ee_enable;
	uint8_t	acc_enable;
	uint8_t	acm_enable;		
	struct ief_info	ief;
	struct ee_info	ee;
	struct acc_info	acc;
	struct acm_info acm;
};

struct ctrl_isp_cfg_info {
	uint32_t ctrl_id;
	uint32_t ctrl_val;
	struct isp_cfg_info isp_cfg;
};

struct isp_sen_cfg_info {
	char name[64];
	struct ctrl_isp_cfg_info ctrls_cfgs[24];
};

static const struct isp_sen_cfg_info  isp_sen_cfgs[] = {
	[0] = {
		.name = "gc0308_demo",
#include "sensor_cfg/gc0308_demo_cfg.h"
	},
	[1] = {
		.name = "gc2035_demo",
#include "sensor_cfg/gc2035_demo_cfg.h"
	},
};
#define N_SEN_CFGS ARRAY_SIZE(isp_sen_cfgs)


/* struct imapx_vp - iMap ISP video proccesing  structure
 * @isp_dev			 pointer to de imapx_isp structure @pending_buf_q
 * pending(empty) buffer queue
 * @active_buf_q	 active(being written) buffer queue
 * @vb_queue		 videobuf2 buffer queue	
 * 
 *
 *
 */
struct imapx_vp {
	struct imapx_isp *isp_dev;
	struct v4l2_device v4l2_dev;
	struct video_device vdev;
	struct list_head    pending_buf_q;
	struct list_head    active_buf_q; 
	struct vb2_queue    vb_queue;
	struct v4l2_subdev  *sensor;
	enum v4l2_mbus_pixelcode mbus_code;   
	struct v4l2_ctrl_handler ctrl_handler;
	struct v4l2_ctrl    *ctrl_colorfx;
	struct v4l2_ctrl    *zoom_absolute;
	struct v4l2_mbus_framefmt   mbus_fmt;
	struct isp_win_size	sensor_wins[N_IMAPX_ISP_SIZES + 1];
	struct sensor_info  *sen_info;
	struct v4l2_pix_format infmt; 
	struct v4l2_pix_format outfmt;   
	struct regulator *dvdd_rg;
	struct regulator *iovdd_rg;
	struct dma_use_table  dmause;
	struct scale_info	scale;
	struct crop_info	crop;
	struct isp_cfg_info  isp_cfg;		
	unsigned int        payload;
	unsigned int        frame_sequence;
	int					active_buffers;
	int					pending_buffers;
	int					state;
	int					buf_index;
	int					cur_frame_index;
};


struct imapx_isp {
	struct imapx_cam_board   *pdata;
	struct imapx_vp		vp[IMAPX_MAX_SENSOR_NUM];
	struct imapx_vp		*cur_vp;
	struct device           *dev;
	struct mutex		lock;
	struct vb2_alloc_ctx        *alloc_ctx;
	spinlock_t          slock;
	struct clk          *busclk;
	struct clk          *oclk;  
	struct clk          *osdclk;  
	struct clk          *mipicon_clk;  
	struct clk          *mipipix_clk; 
	int					irq;
	void				*dmmu_hdl;
	void __iomem        *base; 
};

static inline void isp_active_queue_add(struct imapx_vp *vp,       
		struct isp_buffer *buf)                      
{                                                                    
	list_add_tail(&buf->list, &vp->active_buf_q);                    
	vp->active_buffers++;                                            
}                                                                    

static inline struct isp_buffer *isp_active_queue_pop(           
		struct imapx_vp *vp)                             
{                                                                    
	struct isp_buffer *buf = list_first_entry(&vp->active_buf_q,   
			struct isp_buffer, list);                
	list_del(&buf->list);                                            
	vp->active_buffers--;                                            
	return buf;                                                      
}                                                                    

static inline struct isp_buffer *isp_active_queue_peek(          
		struct imapx_vp *vp, int index)                       
{                                                                    
	struct isp_buffer *tmp, *buf;                                  

	if (WARN_ON(list_empty(&vp->active_buf_q))) {
		pr_err("[ISP] %s:active queue ie empty\n", __func__);
		return NULL;                                                 
	}
	list_for_each_entry_safe(buf, tmp, &vp->active_buf_q, list) {    
		if (buf->index == index) {                                   
			list_del(&buf->list);                                    
			vp->active_buffers--;                                    
			return buf;                                              
		}                                                            
	}                                                                
	return NULL;                                                     
}                                                                    

static inline void isp_pending_queue_add(struct imapx_vp *vp,      
		struct isp_buffer *buf)                     
{                                                                    
	list_add_tail(&buf->list, &vp->pending_buf_q);                   
}                                                                    


static inline struct isp_buffer *isp_pending_queue_pop(         
		struct imapx_vp *vp)                            
{                                                                   
	struct isp_buffer *buf = list_first_entry(&vp->pending_buf_q, 
			struct isp_buffer, list);               
	list_del(&buf->list);                                           
	return buf;                                                     
}                                                                   

void imapx_isp_set_bit(struct imapx_isp *host, uint32_t addr,
		int bits, int offset, uint32_t value);
uint32_t imapx_isp_get_bit(struct imapx_isp *host,uint32_t addr,
		int bits, int offset);                               

int imapx_isp_set_fmt(struct imapx_vp *vp);
void imapx_isp_host_init(struct imapx_vp *vp);
void imapx_isp_host_deinit(struct imapx_vp *vp);
void imapx_isp_cfg_addr(struct imapx_vp *vp, struct isp_buffer *buf, int index);
void imapx_isp_enable_capture(struct imapx_vp *vp);
void imapx_isp_disable_capture(struct imapx_vp *vp);
void imapx_isp_mask_irq(struct imapx_vp *vp, uint32_t mbit);
void imapx_isp_unmask_irq(struct imapx_vp *vp, uint32_t mbit);
int imapx_isp_sen_power(struct imapx_vp *vp, uint32_t on);
/*
 * Crop
 */
int imapx_isp_crop_enable(struct imapx_vp *vp, int enable);
int imapx_isp_crop_cfg(struct imapx_vp *vp, struct crop_info *crop);
/*
 * scale
 */

int imapx_isp_scale_enable(struct imapx_vp *vp, int enable);
int imapx_isp_scale_cfg(struct imapx_vp *vp, struct scale_info *scale);

/* IEF */
int imapx_isp_ief_enable(struct imapx_vp *vp, int enable);
int imapx_isp_ief_cfg(struct imapx_vp *vp, struct ief_info *ief);
/* EE */
int imapx_isp_ee_enable(struct imapx_vp *vp, int enable);
int imapx_isp_ee_cfg(struct imapx_vp *vp, struct ee_info *ief);
/* ACC */
int imapx_isp_acc_enable(struct imapx_vp *vp, int enable);
int imapx_isp_acc_cfg(struct imapx_vp *vp, struct acc_info *acc);
/* ACM */
int imapx_isp_acm_enable(struct imapx_vp *vp, int enable);
int imapx_isp_acm_cfg(struct imapx_vp *vp, struct acm_info *acm);
