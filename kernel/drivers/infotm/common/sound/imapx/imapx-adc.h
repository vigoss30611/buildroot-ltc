#define LINEAR_ADC_VALUE_MIN	0x002
#define LINEAR_ADC_VALUE_MAX	0x0072b


extern void *imapx_dmadev_get_ops(void);
extern struct imapx_i2s_info imapx_i2s;
extern struct snd_dma_buffer *dma_buffer_info;
extern void dump_i2s_regs(void);
extern int tqlp9624_mic_adc_set(int channel, int enable);
extern void imapx_snd_rxctrl(int on);
extern int imapx_i2s_set_sysclk(struct snd_soc_dai *dai,int clk_id, unsigned int freq, int format);

#undef DEBUG

#ifdef DEBUG
#define dbg(fmt...)	pr_err(fmt)
#else
#define dbg(fmt...)	
#endif
struct mic_voltage_device{

	struct workqueue_struct *workqueue;
	struct delayed_work work;
	
    wait_queue_head_t adc_wait;

	struct platform_device *pdev;
	struct imapx_dma_ops *dma_ops;
	u8 dma_state;
	u8 api_state;
	/* dma descripton */
	u32 ch;					
	/* dma phy addr */
	u32	dma_phy_start;
	/* dma virtual addr */
	u32 dma_start;
	/* total dma mem size */
	u32 dma_size;
	/* dma virtual addr pointer */
	u32 dma_pos;
	/* dma mem size in use */
	u32 buffer_size;
	/* dma data size for one transfer */
	u32 buffer_length;
	/* buffer count in the dma mem size in use */
	u32 buffer_index;
	/* the result for adc--iis--dma--voltage */
	u32 voltage;
//	void *raw_data;
};


