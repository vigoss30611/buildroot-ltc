#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <mach/imap-iis.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/dmaengine.h>
#include <asm/dma.h>
#include <mach/imap-iomap.h>
#include <sound/memalloc.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <mach/items.h>
#include <linux/delay.h>
#include "imapx-i2s.h"

#include "imapx-dma.h"
#include "imapx-adc.h"



#define DELAY   1
#define CHECK_RANGE(vol) (vol >1776 && vol<3300)?1:0

struct mic_voltage_device	*mic_vol_dev;

static u32 calcVoltage(u32 adc_value)
{
	u32 temp;
	u32 voltage;

	temp = adc_value >>12;

	if((temp >= LINEAR_ADC_VALUE_MIN)&& (temp <= LINEAR_ADC_VALUE_MAX))
		voltage = (temp - 2)*21/31 + 1776;
	else{
		voltage = 0xffff;
		pr_err("Out of linear range\n");
	}


	pr_err("Measured Voltage is %dmV\n", voltage);

	return voltage;
}


static u32 adc_to_vol(u32 buffer_start, u32 buffer_length, u8 buffer_index)
{

	u8 avg_chunk;
	u32 ret;
	u32 accum = 0;
	u32 skip = 0;
	u32 avg_index = 0;
	u32 adc_value[9] = {0,};
	u32 *pos;	
	/* get the cur buffer addr, left channel first*/	
	if( item_exist("adc.model") &&  item_equal("adc.model","codec",0)){

		if(item_equal("adc.model","micln", 1)){
			pos = (u32*)(buffer_start + buffer_length * buffer_index);
		}
		else if(item_equal("adc.model", "micrn", 1)){
			pos = (u32*)(buffer_start + buffer_length * buffer_index + 4);
		}
	}

	for(avg_chunk=0 ; skip <=buffer_length; pos += 2){

		/* 
		 * duplicate the record buffer 
		 * drop the voltage buffer and save in accum
		 */
		accum += *pos;	
		*pos = 0;

		skip += 8;

		avg_index++;
		if(avg_index ==128){
			adc_value[avg_chunk] = (accum / 128);
			dbg("adc_value[%d] = 0x%08x\n", avg_chunk, adc_value[avg_chunk]);
			accum = 0;
			avg_index = 0;
			avg_chunk++;
		}
	}	

	while(avg_chunk > 0){
		avg_chunk--;
		adc_value[8] += adc_value[avg_chunk];
	}	

	dbg(" sum adc value = 0x%08x\n", adc_value[8]);		   
	
	ret = ( adc_value[8] / 8);
	dbg(" average adc value = 0x%08x\n", ret);		   

	mic_vol_dev->voltage = calcVoltage(ret);

	dbg("%s OUT\n",__func__);	
	return ret;
}




static int imapx_i2s_init(void)
{
	uint32_t imr0, ccr, tcr0, rcr0, tfcr0, rfcr0;

	dbg("%s\n",__func__);
	imr0 = (0x1<<5) | (0x1<<1);
	writel(imr0, imapx_i2s.regs + IMR0);

	/*set fifo*/
	tfcr0 = 0xc;
	rfcr0 = 0x8;
	writel(tfcr0, imapx_i2s.regs + TFCR0);
	writel(rfcr0, imapx_i2s.regs + RFCR0);

	/*set bit mode and gata*/
	ccr = readl(imapx_i2s.regs + CCR);
	tcr0 = readl(imapx_i2s.regs + TCR0);
	rcr0 = readl(imapx_i2s.regs + RCR0);

	/*set format */
	ccr = 0x2 << 3;
#if 1
	tcr0 = 0x04;
	rcr0 = 0x04;
#else
	tcr0 = 0x05;
	rcr0 = 0x05;
#endif
	writel(ccr, imapx_i2s.regs + CCR);
	writel(tcr0, imapx_i2s.regs + TCR0);
	writel(rcr0, imapx_i2s.regs + RCR0);

	/*Disable iis 1,2,3*/
	writel(0, imapx_i2s.regs + RER1);
	writel(0, imapx_i2s.regs + TER1);
	writel(0, imapx_i2s.regs + RER2);
	writel(0, imapx_i2s.regs + TER2);
	writel(0, imapx_i2s.regs + RER3);
	writel(0, imapx_i2s.regs + TER3);


	return 0;
}

static void dma_callback(void *data)
{
	if(mic_vol_dev->api_state){

		mic_vol_dev->dma_pos += mic_vol_dev->buffer_length;

		if (mic_vol_dev->dma_pos >= mic_vol_dev->dma_start + mic_vol_dev->buffer_size)
			mic_vol_dev->dma_pos = mic_vol_dev->dma_start;

		mic_vol_dev->buffer_index = (mic_vol_dev->dma_pos - mic_vol_dev->dma_start)/mic_vol_dev->buffer_length;

	}
	
}



static int imapx_dma_init(void)
{
	struct imapx_dma_info dma_info;
	struct imapx_dma_prep_info dma_config_info;

	dbg("%s\n", __func__);
	

	/* requeset dma resource */
	dma_info.cap = DMA_CYCLIC;
	dma_info.direction = DMA_DEV_TO_MEM;

	dma_info.width = 4;		
	dma_info.fifo = IMAP_IIS0_BASE + RXDMA;
	
	if(mic_vol_dev->ch == 0){

		mic_vol_dev->ch = mic_vol_dev->dma_ops->request(IMAPX_I2S_MASTER_RX, &dma_info);
	}

	if(!(mic_vol_dev->dma_state &= ST_RUNNING)){
		mic_vol_dev->dma_ops->flush(mic_vol_dev->ch);

		/* config dma param */
		dma_config_info.cap = DMA_LLI;
		dma_config_info.direction = DMA_DEV_TO_MEM;
		dma_config_info.fp = dma_callback;

		dma_config_info.fp_param = mic_vol_dev;
		dma_config_info.buf = mic_vol_dev->dma_phy_start;
		dma_config_info.period = mic_vol_dev->buffer_length;
		dma_config_info.len =  mic_vol_dev->buffer_size;

		mic_vol_dev->dma_ops->prepare(mic_vol_dev->ch, &dma_config_info);
	}

	return 0;
}



int	mic_voltage_api(char on_off) 
{
	if(on_off){
		
		dbg("start micphone adc voltage collection\n");
		/* turn on i2s clk */
		imapx_i2s_set_sysclk(NULL, 256, 48000, SNDRV_PCM_FORMAT_S24_LE);
		/* set codec param */
		if(item_exist("adc.model") && item_equal("adc.model", "codec", 0)){

			if(item_equal("adc.model", "micln", 1))
				tqlp9624_mic_adc_set(1, on_off);
			else if(item_equal("adc.model", "micrn", 1))
				tqlp9624_mic_adc_set(2, on_off);
		}
		/* init the dma channel */
		imapx_dma_init();

		/* init the i2s channel */
		imapx_i2s_init();

		/* start dma */
		if(!(mic_vol_dev->dma_state &= ST_RUNNING)){

			dbg("%s: api start\n",__func__);

			mic_vol_dev->dma_ops->trigger(mic_vol_dev->ch);
			/* start i2s */
			imapx_snd_rxctrl(on_off);

			mic_vol_dev->api_state = 1;
		}


		/* schedule workqueue */
//      cancel_delayed_work(&mic_vol_dev->work);

		queue_delayed_work(mic_vol_dev->workqueue, &mic_vol_dev->work, DELAY*HZ);

	}else{
		dbg("stop micphone adc voltage\n");

		cancel_delayed_work(&mic_vol_dev->work);
		flush_workqueue(mic_vol_dev->workqueue);

		if(mic_vol_dev->ch !=0 && !(mic_vol_dev->dma_state &= ST_RUNNING)){
			dbg("%s: api stop\n",__func__);

		if(item_exist("adc.model") && item_equal("adc.model", "codec", 0)){

				if(item_equal("adc.model", "micln", 1))
					tqlp9624_mic_adc_set(1, on_off);
				else if(item_equal("adc.modle", "micrn", 1))
					tqlp9624_mic_adc_set(2, on_off);
			}

			imapx_snd_rxctrl(on_off);
			mic_vol_dev->dma_ops->stop(mic_vol_dev->ch);
		}

		mic_vol_dev->api_state = 0;
        mic_vol_dev->voltage = 1776;

	}

	
	return 0;
}
EXPORT_SYMBOL(mic_voltage_api);


u32 get_lmic_voltage(void)
{
#if 1
    int ret=0;
    int voltage;
    dbg("%s IN\n",__func__); 
    mic_voltage_api(1);
    dbg("before adc wait queue\n");
    dbg("mic_vol_dev->voltage = %d\n",mic_vol_dev->voltage);
    ret = wait_event_interruptible(mic_vol_dev->adc_wait, CHECK_RANGE(mic_vol_dev->voltage)); 
    
    dbg("after adc wait queue\n");
    voltage = mic_vol_dev->voltage;

    mic_voltage_api(0);

    dbg("%s OUT\n",__func__); 

    return voltage;

#endif
}
EXPORT_SYMBOL(get_lmic_voltage);


static void adc_value_update(struct work_struct *work)
{
	struct mic_voltage_device *mvd = container_of(work,struct mic_voltage_device, work.work);

	dbg("%s\n",__func__);
#if 1
	if(!(mic_vol_dev->dma_state &= ST_RUNNING) && mic_vol_dev->ch==0 && mic_vol_dev->api_state==1){
		/* restart dma */
		cancel_delayed_work(&mic_vol_dev->work);
		dbg("%s:restart api\n",__func__);
		//flush_workqueue(mic_vol_dev->workqueue);

		mic_voltage_api(1);
	}	
#endif
#if 1
	dbg(" buffer_start =0x%08x\nbuffer_pos=0x%08x\nbuffer_length=%d\nbuffer_index=%d\n",	\
			mvd->dma_start, \
			mvd->dma_pos, \
			mvd->buffer_length,	\
			mvd->buffer_index);
#endif
		#if 1
		adc_to_vol(mic_vol_dev->dma_start, mic_vol_dev->buffer_length, mic_vol_dev->buffer_index);
		#else
		adc_to_vol((u32)mic_vol_dev->raw_data, mic_vol_dev->buffer_length, 0);
		#endif
            
        if(CHECK_RANGE(mic_vol_dev->voltage)){
            dbg("wakeup adc wait queue\n");
            wake_up_interruptible(&mic_vol_dev->adc_wait);
        }
    	cancel_delayed_work(&mic_vol_dev->work);
		queue_delayed_work(mic_vol_dev->workqueue, &mic_vol_dev->work,DELAY*HZ);
}


static int  mic_vol_probe(struct platform_device *pdev)
{
	struct snd_dma_buffer *buf;
	struct workqueue_struct *workqueue;

	dbg("%s\n",__func__);

	/* requeset dma buffer */
	buf = dma_buffer_info;
	

	mic_vol_dev->dma_ops = imapx_dmadev_get_ops();

	INIT_DELAYED_WORK(&mic_vol_dev->work, adc_value_update);
    init_waitqueue_head(&mic_vol_dev->adc_wait); 

	workqueue= create_singlethread_workqueue("micphone-adc");	

	if(workqueue == NULL)
		return -ENOMEM;

	mic_vol_dev->workqueue = workqueue;
	mic_vol_dev->dma_state &= ~ST_RUNNING;
	mic_vol_dev->api_state = 0;

    mic_vol_dev->voltage = 0;

	/* dma description */
	mic_vol_dev->ch = 0;
	/* get dma buf info */
	mic_vol_dev->dma_start = (u32)buf->area;
	mic_vol_dev->dma_phy_start = buf->addr;
    mic_vol_dev->dma_size = buf->bytes;	
	mic_vol_dev->dma_pos = (u32)buf->area;
	
	mic_vol_dev->buffer_index = 0;
	mic_vol_dev->buffer_length = 0x2000;
	mic_vol_dev->buffer_size= 0x6000;
	dbg("dma start phy addr=0x%x\ndam_start=0x%x\ndma size =0x%x\n",	\
			mic_vol_dev->dma_phy_start,	\
			mic_vol_dev->dma_start,	\
			mic_vol_dev->dma_size); 
	/*	
	mic_vol_dev->raw_data = kzalloc(mic_vol_dev->buffer_length, GFP_KERNEL);
	if(mic_vol_dev->raw_data == NULL)
		return -ENOMEM;
	*/
	return 0;
}

void mic_vol_remove(struct platform_device *dev){
	flush_workqueue(mic_vol_dev->workqueue);
	destroy_workqueue(mic_vol_dev->workqueue);
}

static struct platform_driver mic_voltage_driver = {
	.driver = {
		.name = "mic-voltage-adc",
		.owner = THIS_MODULE,
	},
	.probe = mic_vol_probe,
	.remove = mic_vol_remove
};



static int mvd_proc_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t mvd_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos) 
{
	char ubuf[4]={0,};

	if(count >= sizeof(ubuf))
		count=sizeof(ubuf);

	if(copy_from_user(ubuf,buffer,count))
		return -EFAULT;

	ubuf[count]='\0';
	dbg("ubuf=%s\n", ubuf);
	if(!strncmp(ubuf, "on", 2))	
		mic_voltage_api(1);
	else if(!strncmp(ubuf, "off", 3))
		mic_voltage_api(0);

	return count;

}



static const struct file_operations mic_vol_device_fops = {

	.owner = THIS_MODULE,
	.open = mvd_proc_open,
	.write = mvd_proc_write,
};

static int __init imapx_mic_adc_init(void)
{
	int ret;
	struct proc_dir_entry *entry;

	mic_vol_dev = kmalloc(sizeof(struct mic_voltage_device),GFP_KERNEL);
	
	if(mic_vol_dev == NULL)
		return -ENOMEM;
#if 1
	entry = proc_create_data("mvd",S_IRUGO|S_IWUSR,NULL,&mic_vol_device_fops,NULL);

	if(entry == NULL)
		return -1;
#endif

	mic_vol_dev->ch = 0;
	mic_vol_dev->pdev = platform_device_register_simple("mic-voltage-adc",-1, NULL, 0);
	if(IS_ERR(mic_vol_dev->pdev)){
		dbg("register pdev fail\n");
		goto out;
	}
	if(item_equal("adc.model","codec",0))
		ret =platform_driver_register(&mic_voltage_driver);
	else
		goto out;

	if(ret<0){
		dbg("regiter pdev driver fail\n");
		goto out1;
	}
	return ret;

out:
	pr_err("item adc.model Not exist\n");
	platform_device_unregister(mic_vol_dev->pdev);
	return 0;
out1:
	platform_driver_unregister(&mic_voltage_driver);
	return -1;

}

static void __exit imapx_mic_adc_exit(void)
{
	platform_device_unregister(mic_vol_dev->pdev);
	platform_driver_unregister(&mic_voltage_driver);
	remove_proc_entry("mvd",NULL);
	kfree(mic_vol_dev);
}

late_initcall(imapx_mic_adc_init);
module_exit(imapx_mic_adc_exit);
MODULE_LICENSE("GPL");

