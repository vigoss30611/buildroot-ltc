/* For AllWinner android platform.
 *
 * mir3da.c - Linux kernel modules for 3-Axis Accelerometer
 *
 * Copyright (C) 2011-2013 MiraMEMS Sensing Technology Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/input-polldev.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
//#include <mach/system.h>
#include <mach/hardware.h>
#include "mir3da_core.h"
#include "mir3da_cust.h"
#include <linux/workqueue.h>
#include <linux/kthread.h>  /* thread */
#include <linux/gpio.h>
#include <linux/input.h>
#include <mach/items.h>
#include <linux/kobject.h>

#define MIR3DA_DRV_NAME                 	"mir3da"
#define MIR3DA_INPUT_DEV_NAME     	MIR3DA_DRV_NAME          
#define MIR3DA_MISC_NAME                	MIR3DA_DRV_NAME

#define POLL_INTERVAL                   	50 

#define OFFSET_STRING_LEN               26
#define COLLIDE	0x2

#define MI_DATA(format, ...)         	if(DEBUG_DATA&Log_level){printk(KERN_ERR MI_TAG format "\n", ## __VA_ARGS__);}
#define MI_MSG(format, ...)             if(DEBUG_MSG&Log_level){printk(KERN_ERR MI_TAG format "\n", ## __VA_ARGS__);}
#define MI_ERR(format, ...)             if(DEBUG_ERR&Log_level){printk(KERN_ERR MI_TAG format "\n", ## __VA_ARGS__);}
#define MI_FUN                          if(DEBUG_FUNC&Log_level){printk(KERN_ERR MI_TAG "%s is called, line: %d\n", __FUNCTION__,__LINE__);}
#define MI_ASSERT(expr)                 \
	if (!(expr)) {\
		printk(KERN_ERR "Assertion failed! %s,%d,%s,%s\n",\
			__FILE__, __LINE__, __func__, #expr);\
	}

/*----------------------------------------------------------------------------*/
#define ALOG(fmt,args...) printk(fmt,##args)
#define LOG(fmt,args...) ALOG(KERN_INFO "cpia2: "fmt,##args)
#define G_SENSOR_GPIO_DEFAULT 141

struct mir3da_data
{
	int acc_irq_enable;
	int acc_det_count;
	int acc_det_status;
	u32 acc_int_handle;
	u32 acc_irq_number;
	int gs_suspend_en; 
	
	//struct input_polled_dev	*mir3da_idev;
	struct input_dev *mir3da_idev;
	struct device			*hwmon_dev;
	MIR_HANDLE				mir_handle;

	struct work_struct  acc_int_work;
	struct workqueue_struct *input_acc_int_work;
	
	struct task_struct * acc_gs_poll_task;
	struct i2c_client *this_client;
	struct kobject *mir3da_kobj;
};

struct work_info
{
    char        tst1[20];
    char        tst2[20];
    char        buffer[OFFSET_STRING_LEN];
    struct      workqueue_struct *wq;
    struct      delayed_work read_work;
    struct      delayed_work write_work;
    struct      completion completion;
    int         len;
    int         rst;
};

struct general_op_s ops_handle;

static struct mir3da_data *mir3da_data_ptr = NULL;
static unsigned int   delayMs = 50;
static unsigned char  twi_id = 0;
static int mir3da_auto_detect = 0;
static int g_level_flag = 1;
extern int gSensorEnable;

extern MIR_HANDLE mir3da_core_init(PLAT_HANDLE handle);
extern int Log_level;

static struct work_info m_work_info = {{0}};
#if 0
/*----------------------------------------------------------------------------*/
static void sensor_write_work( struct work_struct *work )
{
    struct work_info*   pWorkInfo;
    struct file         *filep;
    u32                 orgfs;
    int                 ret;   

    orgfs = get_fs();
    set_fs(KERNEL_DS);

    pWorkInfo = container_of((struct delayed_work*)work, struct work_info, write_work);
    if (pWorkInfo == NULL){            
            MI_ERR("get pWorkInfo failed!");       
            return;
    }
    
    filep = filp_open(OffsetFileName, O_RDWR|O_CREAT, 0600);
    if (IS_ERR(filep)){
        MI_ERR("write, sys_open %s error!!.\n", OffsetFileName);
        ret =  -1;
    }
    else{   
        filep->f_op->write(filep, pWorkInfo->buffer, pWorkInfo->len, &filep->f_pos);
        filp_close(filep, NULL);
        ret = 0;        
    }
    
    set_fs(orgfs);   
    pWorkInfo->rst = ret;
    complete( &pWorkInfo->completion );
}
/*----------------------------------------------------------------------------*/
static void sensor_read_work( struct work_struct *work )
{
    u32 orgfs;
    struct file *filep;
    int ret; 
    struct work_info* pWorkInfo;
        
    orgfs = get_fs();
    set_fs(KERNEL_DS);
    
    pWorkInfo = container_of((struct delayed_work*)work, struct work_info, read_work);
    if (pWorkInfo == NULL){            
        MI_ERR("get pWorkInfo failed!");       
        return;
    }

    filep = filp_open(OffsetFileName, O_RDONLY, 0600);
    if (IS_ERR(filep)){
        MI_ERR("read, sys_open %s error!!.\n",OffsetFileName);
        set_fs(orgfs);
        ret =  -1;
    }
    else{
        filep->f_op->read(filep, pWorkInfo->buffer,  sizeof(pWorkInfo->buffer), &filep->f_pos);
        filp_close(filep, NULL);    
        set_fs(orgfs);
        ret = 0;
    }

    pWorkInfo->rst = ret;
    complete(&(pWorkInfo->completion));
}

/*----------------------------------------------------------------------------*/
static int sensor_sync_read(u8* offset)
{
	return 0;
}
/*----------------------------------------------------------------------------*/
static int sensor_sync_write(u8* off)
{
    int err = 0;
    struct work_info* pWorkInfo = &m_work_info;

	return 0;
	
    init_completion( &pWorkInfo->completion );
    
    sprintf(m_work_info.buffer, "%x,%x,%x,%x,%x,%x,%x,%x,%x\n", off[0],off[1],off[2],off[3],off[4],off[5],off[6],off[7],off[8]);
    
    pWorkInfo->len = sizeof(m_work_info.buffer);
        
    //queue_delayed_work( pWorkInfo->wq, &pWorkInfo->write_work, msecs_to_jiffies(0) );
    err = wait_for_completion_timeout( &pWorkInfo->completion, msecs_to_jiffies( 2000 ) );
    if ( err == 0 ){
        MI_ERR("wait_for_completion_timeout TIMEOUT");
        return -1;
    }

    if (pWorkInfo->rst != 0){
        MI_ERR("work_info.rst  not equal 0");
        return pWorkInfo->rst;
    }
    
    return 0;
}
/*----------------------------------------------------------------------------*/
static int check_califolder_exist(void)
{
    u32     orgfs = 0;
    struct  file *filep;

	return 1;

    orgfs = get_fs();
    set_fs(KERNEL_DS);
	
    filep = filp_open(OffsetFolerName, O_CREAT|O_RDONLY, 0600);
    if (IS_ERR(filep)) {
        MI_ERR("%s read, sys_open %s error!!.\n",__func__,OffsetFolerName);
        set_fs(orgfs);
        return 0;
    }

    filp_close(filep, NULL);    
    set_fs(orgfs); 

    return 1;
}

/*----------------------------------------------------------------------------*/
static int support_fast_auto_cali(void)
{
#if MIR3DA_SUPPORT_FAST_AUTO_CALI
    return 1;
#else
    return 0;
#endif
}
/*----------------------------------------------------------------------------*/
static void report_abs(void)
{
    short x=0,y=0,z=0;
    MIR_HANDLE handle = mir3da_data_ptr->mir_handle;
        
    if (mir3da_read_data(handle, &x,&y,&z) != 0) {
        MI_ERR("MIR3DA data read failed!\n");
        return;
    }
    
    input_report_abs(mir3da_data_ptr->mir3da_idev->input, ABS_X, x);
    input_report_abs(mir3da_data_ptr->mir3da_idev->input, ABS_Y, y);
    input_report_abs(mir3da_data_ptr->mir3da_idev->input, ABS_Z, z);
    input_sync(mir3da_data_ptr->mir3da_idev->input);

	printk("report_abs - x:%d, y:%d, z:%d\n", x, y, z);
		
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_delay_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", delayMs);
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_delay_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int interval = 0;   

    interval = simple_strtoul(buf, NULL, 10);    
    
    if (interval < 0 || interval > 1000)
        return -EINVAL;

    if ((interval <=30)&&(interval > 10)){
        interval = 10;
    }

    delayMs = interval;
        
    return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_primary_offset_show(struct device *dev,
                   struct device_attribute *attr, char *buf){    
    MIR_HANDLE   handle = mir3da_data_ptr->mir_handle;
    int x=0,y=0,z=0;
   
    mir3da_get_primary_offset(handle,&x,&y,&z);

	return sprintf(buf, "x=%d ,y=%d ,z=%d\n",x,y,z);

}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_uevent_show(struct device *dev,
                   struct device_attribute *attr, char *buf){
	return sprintf(buf, "%s\n", "MiraMEMS");
}
/*----------------------------------------------------------------------------*/

static void report_abs(void)
{
    short x=0,y=0,z=0;
    MIR_HANDLE handle = mir3da_data_ptr->mir_handle;
        
    if (mir3da_read_data(handle, &x,&y,&z) != 0) {
        MI_ERR("MIR3DA data read failed!\n");
        return;
    }
    
    input_report_abs(mir3da_data_ptr->mir3da_idev, ABS_X, x);
    input_report_abs(mir3da_data_ptr->mir3da_idev, ABS_Y, y);
    input_report_abs(mir3da_data_ptr->mir3da_idev, ABS_Z, z);
    input_sync(mir3da_data_ptr->mir3da_idev);

	printk("report_abs - x:%d, y:%d, z:%d\n", x, y, z);
		
}
/*----------------------------------------------------------------------------*/
static void mir3da_dev_poll(struct input_polled_dev *dev)
{
	return ;
}

#endif

static int DoI2cScanDetectDev( void )
{
	struct i2c_adapter *adapter=NULL;
	__u16 i2cAddr= 0x26;	//slave addr
	__u8 subByteAddr=0x0;	//fix to read 0
	char databuf[8];
	int ret;

#define I2C_DETECT_TRY_SIZE  5
	int i,i2cDetecCnt=0;

	struct i2c_msg I2Cmsgs[2] = {
		{
			.addr	= i2cAddr,
			.flags	= 0,
			.len	= 1,
			.buf	= &subByteAddr,
		},
		{
			.addr	= i2cAddr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= databuf,
		}
	};

	printk("A%s\n",__FUNCTION__);

	if(item_exist("sensor.grivaty")){
		if(item_equal("sensor.grivaty.model", "mir3da", 0)){
			
			//get i2c handle
			int i2c_id = item_integer("sensor.grivaty.ctrl", 1);
			adapter = i2c_get_adapter(i2c_id);

            printk("=======stk i2c id : i2c.%d=======\n", i2c_id);
			if (!adapter) {
				printk("****** get_adapter error! ******\n");
				return 0;
			}
			printk("C%s\n",__FUNCTION__);
			
			//scan i2c
			i2cDetecCnt=0;
			for(i=0; i<I2C_DETECT_TRY_SIZE; i++)
			{
				ret = i2c_transfer(adapter,I2Cmsgs,2);
				printk("D-%s,ret=%d\n", __FUNCTION__, ret);
				if(ret>0)
					i2cDetecCnt++;
			}
			LOG("E-%s,thd=%d,%d\n", __FUNCTION__, (I2C_DETECT_TRY_SIZE/2),i2cDetecCnt);
			if(i2cDetecCnt<=(I2C_DETECT_TRY_SIZE/2)){
				return 0;
			}else {
                mir3da_auto_detect = 1;
				return 1;
			}
		}
	}
	return 0;
}


/*----------------------------------------------------------------------------*/
static int get_address(PLAT_HANDLE handle)
{
    if(NULL == handle){
        MI_ERR("chip init failed !\n");
		    return -1;
    }
			
	return ((struct i2c_client *)handle)->addr; 		
}


/*----------------------------------------------------------------------------*/
static long mir3da_misc_ioctl( struct file *file,unsigned int cmd, unsigned long arg)
{
    void __user     *argp = (void __user *)arg;
    int             err = 0;
    int             interval = 0;
    char            bEnable = 0;
    short           xyz[3] = {0};
    MIR_HANDLE      handle = mir3da_data_ptr->mir_handle;
	
#if MIR3DA_OFFSET_TEMP_SOLUTION
	int 			z_dir = 0;
#endif
	
    if(_IOC_DIR(cmd) & _IOC_READ){
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if(_IOC_DIR(cmd) & _IOC_WRITE){
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }

    if(err){
        return -EFAULT;
    }

    switch (cmd){      
        case MIR3DA_ACC_IOCTL_GET_DELAY:
            interval = POLL_INTERVAL;
            if (copy_to_user(argp, &interval, sizeof(interval)))
                return -EFAULT;
            break;

        case MIR3DA_ACC_IOCTL_SET_DELAY:
            if (copy_from_user(&interval, argp, sizeof(interval)))
                return -EFAULT;
            if (interval < 0 || interval > 1000)
                return -EINVAL;
            if((interval <=30)&&(interval > 10)){
                interval = 10;
            }
            delayMs = interval;
            break;

        case MIR3DA_ACC_IOCTL_SET_ENABLE:
            if (copy_from_user(&bEnable, argp, sizeof(bEnable)))
                return -EFAULT;

            err = mir3da_set_enable(handle, bEnable);
            if (err < 0)
                return EINVAL;
            break;

        case MIR3DA_ACC_IOCTL_GET_ENABLE:        
            err = mir3da_get_enable(handle, &bEnable);
            if (err < 0){
                return -EINVAL;
            }

            if (copy_to_user(argp, &bEnable, sizeof(bEnable)))
                    return -EINVAL;            
            break;

#if MIR3DA_OFFSET_TEMP_SOLUTION
        case MIR3DA_ACC_IOCTL_CALIBRATION:
            if(copy_from_user(&z_dir, argp, sizeof(z_dir)))
                return -EFAULT;
            
            if(mir3da_calibrate(handle, z_dir)) {
                return -EFAULT;
            } 

            if(copy_to_user(argp, &z_dir, sizeof(z_dir)))
                return -EFAULT;
            break;        

        case MIR3DA_ACC_IOCTL_UPDATE_OFFSET:
            manual_load_cali_file(handle);
            break;
#endif 

        case MIR3DA_ACC_IOCTL_GET_COOR_XYZ:

            if(mir3da_read_data(handle, &xyz[0],&xyz[1],&xyz[2]))
                return -EFAULT;        

            if(copy_to_user((void __user *)arg, xyz, sizeof(xyz)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
static const struct file_operations mir3da_misc_fops = {
        .owner = THIS_MODULE,
        .unlocked_ioctl = mir3da_misc_ioctl,
};

static struct miscdevice misc_mir3da = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = MIR3DA_MISC_NAME,
        .fops = &mir3da_misc_fops,
};


/*----------------------------------------------------------------------------*/
static ssize_t mir3da_axis_data_show(struct device *dev,
           struct device_attribute *attr, char *buf)
{
    int result;
    short x,y,z;
    int count = 0;
    MIR_HANDLE      handle = mir3da_data_ptr->mir_handle;
    
    result = mir3da_read_data(handle, &x, &y, &z);
    if (result == 0)
        count += sprintf(buf+count, "x= %d;y=%d;z=%d\n", x,y,z);
    else
        count += sprintf(buf+count, "reading failed!");
	
    return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_reg_data_store(struct device *dev,
           struct device_attribute *attr, const char *buf, size_t count)
{
    int                 addr, data;
    int                 result;
    MIR_HANDLE          handle = mir3da_data_ptr->mir_handle;
        
    sscanf(buf, "0x%x, 0x%x\n", &addr, &data);
    
    result = mir3da_register_write(handle, addr, data);
    
    MI_ASSERT(result==0);

    return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_reg_data_show(struct device *dev,
           struct device_attribute *attr, char *buf)
{
    MIR_HANDLE          handle = mir3da_data_ptr->mir_handle;
        
    return mir3da_get_reg_data(handle, buf);
}

#if MIR3DA_OFFSET_TEMP_SOLUTION
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_offset_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t count = 0;

#if MIR3DA_OFFSET_TEMP_SOLUTION 
    if(bLoad==FILE_EXIST)
    	count += sprintf(buf,"%s", m_work_info.buffer);   
    else
    	count += sprintf(buf,"%s","Calibration file not exist!\n");
#endif

    return count;
}     
#endif

/*----------------------------------------------------------------------------*/
#if FILTER_AVERAGE_ENHANCE
static ssize_t mir3da_average_enhance_show(struct device *dev,
                   struct device_attribute *attr, char *buf)
{
    int                             ret = 0;
    struct mir3da_filter_param_s    param = {0};

    ret = mir3da_get_filter_param(&param);
    ret |= sprintf(buf, "%d %d %d\n", param.filter_param_l, param.filter_param_h, param.filter_threhold);

    return ret;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_average_enhance_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{ 
    int                             ret = 0;
    struct mir3da_filter_param_s    param = {0};
    
    sscanf(buf, "%d %d %d\n", &param.filter_param_l, &param.filter_param_h, &param.filter_threhold);
    
    ret = mir3da_set_filter_param(&param);
    
    return count;
}
#endif 
/*----------------------------------------------------------------------------*/
#if MIR3DA_OFFSET_TEMP_SOLUTION
int bCaliResult = -1;
static ssize_t mir3da_calibrate_show(struct device *dev,struct device_attribute *attr,char *buf)
{
    int ret;       

    ret = sprintf(buf, "%d\n", bCaliResult);   
    return ret;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_calibrate_store(struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
    s8              z_dir = 0;
    MIR_HANDLE      handle = mir3da_data_ptr->mir_handle;
       
    z_dir = simple_strtol(buf, NULL, 10);
    bCaliResult = mir3da_calibrate(handle, z_dir);
    
    return count;
}
#endif
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_log_level_show(struct device *dev,
                   struct device_attribute *attr, char *buf)
{
    int ret;

    ret = sprintf(buf, "%d\n", Log_level);

    return ret;
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_log_level_store(struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
    Log_level = simple_strtoul(buf, NULL, 10);    

    return count;
}

/*----------------------------------------------------------------------------*/
static ssize_t mir3da_version_show(struct device *dev,
                   struct device_attribute *attr, char *buf){    

	return sprintf(buf, "%s_%s\n", DRI_VER, CORE_VER);
}
/*----------------------------------------------------------------------------*/
static ssize_t mir3da_vendor_show(struct device *dev,
                   struct device_attribute *attr, char *buf){
	return sprintf(buf, "%s\n", "MiraMEMS");
}

static ssize_t mir3da_sensitivity_show(struct device *dev,
                   struct device_attribute *attr, char *buf)
{
	
	return sprintf(buf, "%d\n", g_level_flag);
}


static ssize_t mir3da_sensitivity_store(struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
	int level;
	MIR_HANDLE	 handle = mir3da_data_ptr->mir_handle;

	level = simple_strtoul(buf, NULL, 10); 
	if(level>0 && level<4){
		g_level_flag = level;
		mir3da_set_sensitivity(handle, level);
	}
	
	return count;
}

static ssize_t mir3da_enable_show(struct device *dev,
                   struct device_attribute *attr, char *buf)
{
    int             ret;

	ret = sprintf(buf, "%d\n", mir3da_data_ptr->acc_irq_enable);

    return ret;
}

/*----------------------------------------------------------------------------*/
static ssize_t mir3da_enable_store(struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
    int             ret = 0;
    unsigned long   enable;
	
    if (buf == NULL){
        return -1;
    }

    enable = simple_strtoul(buf, NULL, 10);    
	
	if(enable && !mir3da_data_ptr->acc_irq_enable){
		//enable_irq(mir3da_data_ptr->acc_irq_number);
		mir3da_data_ptr->acc_irq_enable = enable;
		gSensorEnable = mir3da_data_ptr->acc_irq_enable;
		printk("open irq\n");
	}else if(!enable && mir3da_data_ptr->acc_irq_enable){
		//disable_irq(mir3da_data_ptr->acc_irq_number);
		mir3da_data_ptr->acc_irq_enable = enable;
		gSensorEnable = mir3da_data_ptr->acc_irq_enable;
		printk("close irq\n");
	}

    return count;
}

/*----------------------------------------------------------------------------*/
static DEVICE_ATTR(enable,          S_IRUGO | S_IWUGO,  mir3da_enable_show,              mir3da_enable_store);
//static DEVICE_ATTR(delay,      S_IRUGO | S_IWUGO,  mir3da_delay_show,                    mir3da_delay_store);
static DEVICE_ATTR(axis_data,       S_IRUGO |S_IWUGO,    mir3da_axis_data_show,          NULL);
static DEVICE_ATTR(reg_data,        S_IWUGO | S_IRUGO,  mir3da_reg_data_show,             mir3da_reg_data_store);
static DEVICE_ATTR(log_level,       S_IWUGO | S_IRUGO,  mir3da_log_level_show,            mir3da_log_level_store);
#if MIR3DA_OFFSET_TEMP_SOLUTION
static DEVICE_ATTR(offset,          S_IWUGO | S_IRUGO,  mir3da_offset_show,                NULL);
static DEVICE_ATTR(calibrate_miraGSensor,       S_IWUGO | S_IRUGO,  mir3da_calibrate_show, mir3da_calibrate_store);
#endif
#if FILTER_AVERAGE_ENHANCE
static DEVICE_ATTR(average_enhance, S_IWUGO | S_IRUGO,  mir3da_average_enhance_show,       mir3da_average_enhance_store);
#endif 
//static DEVICE_ATTR(primary_offset,  S_IRUGO,            mir3da_primary_offset_show,            NULL);
static DEVICE_ATTR(version,         S_IRUGO,            mir3da_version_show,            NULL);
static DEVICE_ATTR(vendor,          S_IRUGO,            mir3da_vendor_show,             NULL); 
static DEVICE_ATTR(sensitivity,     S_IWUGO | S_IRUGO,  mir3da_sensitivity_show,     mir3da_sensitivity_store); 
/*----------------------------------------------------------------------------*/
static struct attribute *mir3da_attributes[] = { 
    &dev_attr_enable.attr,
 //   &dev_attr_delay.attr,
    &dev_attr_axis_data.attr,
    &dev_attr_reg_data.attr,
    &dev_attr_log_level.attr,
#if MIR3DA_OFFSET_TEMP_SOLUTION
    &dev_attr_offset.attr,    
    &dev_attr_calibrate_miraGSensor.attr,
#endif
#if FILTER_AVERAGE_ENHANCE
    &dev_attr_average_enhance.attr,
#endif /* ! FILTER_AVERAGE_ENHANCE */
//    &dev_attr_primary_offset.attr,  
    &dev_attr_version.attr,
    &dev_attr_vendor.attr,
    &dev_attr_sensitivity.attr,
    NULL
};

static const struct attribute_group mir3da_attr_group = {
    .attrs  = mir3da_attributes,
};

/*----------------------------------------------------------------------------*/
int i2c_smbus_read(PLAT_HANDLE handle, u8 addr, u8 *data)
{
    int                 res = 0;
    struct i2c_client   *client = (struct i2c_client*)handle;
    
    *data = i2c_smbus_read_byte_data(client, addr);
    
    return res;
}
/*----------------------------------------------------------------------------*/
int i2c_smbus_read_block(PLAT_HANDLE handle, u8 addr, u8 count, u8 *data)
{
    int                 res = 0;
    struct i2c_client   *client = (struct i2c_client*)handle;
    
    res = i2c_smbus_read_i2c_block_data(client, addr, count, data);
    
    return res;
}
/*----------------------------------------------------------------------------*/
int i2c_smbus_write(PLAT_HANDLE handle, u8 addr, u8 data)
{
    int                 res = 0;
    struct i2c_client   *client = (struct i2c_client*)handle;
    
    res = i2c_smbus_write_byte_data(client, addr, data);
    
    return res;
}
/*----------------------------------------------------------------------------*/
void msdelay(int ms)
{
    mdelay(ms);
}

#if MIR3DA_OFFSET_TEMP_SOLUTION
MIR_GENERAL_OPS_DECLARE(ops_handle, i2c_smbus_read, i2c_smbus_read_block, i2c_smbus_write, sensor_sync_write, sensor_sync_read, check_califolder_exist, get_address, support_fast_auto_cali,msdelay, printk, sprintf);
#else
MIR_GENERAL_OPS_DECLARE(ops_handle, i2c_smbus_read, i2c_smbus_read_block, i2c_smbus_write, NULL, NULL, NULL,get_address,NULL,msdelay, printk, sprintf);
#endif


static irqreturn_t input_acc_irq_handler(int irq, void *dev_id)
{	
	queue_work(mir3da_data_ptr->input_acc_int_work, &mir3da_data_ptr->acc_int_work);
	return IRQ_HANDLED;
}

static void int_acc_work_func(struct work_struct *work)
{
	static int count = 0 ;

	count++;
	if(count%2 == 0){
		input_report_key(mir3da_data_ptr->mir3da_idev, KEY_SPORT, 0);
	}else{
		input_report_key(mir3da_data_ptr->mir3da_idev, KEY_SPORT, 1);
	}
	input_sync(mir3da_data_ptr->mir3da_idev);

	if(count >= 100)
		count = 0;
	printk("G-sensor Detected collision, Reporting uevent\n");
}

static int mir3da_irq_setup(struct i2c_client *client)
{
	int error;
	int gpio = -1;

	if(item_exist("sensor.grivaty.int")){
		gpio = item_integer("sensor.grivaty.int", 1);
	}else{
		gpio = G_SENSOR_GPIO_DEFAULT;
	}

	if (gpio_request(gpio, "GENT")){
		MI_ERR("[mir3da]%s:gpio_request() failed\n",__func__);
		return -1;
	}
	
	gpio_direction_input(gpio);
	mir3da_data_ptr->acc_irq_number = gpio_to_irq(gpio);

	if ( mir3da_data_ptr->acc_irq_number < 0 ){
		MI_ERR("[mir3da]%s:gpio_to_irq() failed\n",__func__);
		return -1;
	}
	
	client->irq = mir3da_data_ptr->acc_irq_number;
	printk(KERN_INFO "[mir3da]%s: irq # = %d\n", __func__, mir3da_data_ptr->acc_irq_number);
	if(mir3da_data_ptr->acc_irq_number < 0)
		MI_ERR("[mir3da]%s: irq number was not specified!\n", __func__);
	
	gSensorEnable = mir3da_data_ptr->acc_irq_enable = 1;
	
	error = request_irq(client->irq, input_acc_irq_handler, IRQF_TRIGGER_RISING, "mir3da-mems", mir3da_data_ptr);
	if (error < 0){
		MI_ERR("[mir3da]%s: request_irq(%d) failed for (%d)\n", __func__, client->irq, error);
		return -1;
	}
	
	return client->irq;
}
/*----------------------------------------------------------------------------*/
static int mir3da_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int  result, error;
    struct input_dev    *idev;

    if(mir3da_install_general_ops(&ops_handle)){
        MI_ERR("Install ops failed !\n");
        goto err_detach_client;
    }

    INIT_WORK(&mir3da_data_ptr->acc_int_work, int_acc_work_func);
    mir3da_data_ptr->input_acc_int_work = create_singlethread_workqueue("acc_int_work");
    if (!mir3da_data_ptr->input_acc_int_work){
        MI_ERR("Creat input_acc_int_work  workqueue failed.");
        return -ENOMEM;
    }

    m_work_info.wq = create_singlethread_workqueue( "oo" );
    if(NULL==m_work_info.wq) {
        MI_ERR("Failed to create workqueue !");
        goto err_detach_client;
    }
	
    /* Initialize the MIR3DA chip */
    mir3da_data_ptr->mir_handle = mir3da_core_init((PLAT_HANDLE)client);
    if(NULL == mir3da_data_ptr->mir_handle){
        MI_ERR("chip init failed !\n");
        goto err_detach_client;        
    }

    mir3da_data_ptr->hwmon_dev = hwmon_device_register(&client->dev);
    MI_ASSERT(!(IS_ERR(mir3da_data_ptr->hwmon_dev)));

	mir3da_data_ptr->mir3da_idev = input_allocate_device();
	if (!mir3da_data_ptr->mir3da_idev) {
        MI_ERR("alloc poll device failed!\n");
        result = -ENOMEM;
        goto err_hwmon_device_unregister;
    }

	    /* Misc device interface Register */
    result = misc_register(&misc_mir3da);
    if (result) {
        MI_ERR("%s: mir3da_dev register failed", __func__);
        goto err_free_device;
    }
	
	idev = mir3da_data_ptr->mir3da_idev;

	input_set_capability(idev, EV_KEY, KEY_SPORT);

	idev->name = MIR3DA_INPUT_DEV_NAME;   
	idev->phys = "mir3da/input2";
	
	idev->id.bustype = BUS_HOST;
	idev->dev.parent = &client->dev;
	idev->id.vendor = 0x000A;
	idev->id.product = 0x000A;
	idev->id.version = 0x0A00;
	set_bit(EV_KEY, idev->evbit);

    result = input_register_device(idev);
    if (result) {
        MI_ERR("register poll device failed!\n");
        goto err_misc_deregister; 
	}
		
	mir3da_data_ptr->mir3da_kobj = kobject_create_and_add("attribute", &misc_mir3da.this_device->kobj);
	if (!mir3da_data_ptr->mir3da_kobj){		
		MI_ERR("kobject create or add failed!\n");
		result = -ENOMEM;
		goto err_unregister_device;
	}
    
    /* Sys Attribute Register */
    result = sysfs_create_group(mir3da_data_ptr->mir3da_kobj, &mir3da_attr_group);
    if (result) {
        MI_ERR("create device file failed!\n");
        result = -EINVAL;
		kobject_put(mir3da_data_ptr->mir3da_kobj);
        goto err_del_kobject;
    }
	
	error = mir3da_irq_setup(client);
	if(!error){
		goto err_remove_sysfs_group;
	}
	
    return result;
    
err_remove_sysfs_group:
    sysfs_remove_group(&idev->dev.kobj, &mir3da_attr_group);   
err_del_kobject:
	kobject_put(mir3da_data_ptr->mir3da_kobj);
	kobject_del(mir3da_data_ptr->mir3da_kobj);
err_unregister_device:
    input_unregister_device(idev);
err_misc_deregister:
	misc_deregister(&misc_mir3da);
err_free_device:
    input_free_device(idev);
err_hwmon_device_unregister:
    hwmon_device_unregister(&client->dev);    
err_detach_client:
	flush_workqueue(m_work_info.wq);
    destroy_workqueue(m_work_info.wq);
    return result;
}
/*----------------------------------------------------------------------------*/
static int mir3da_remove(struct i2c_client *client)
{
    MIR_HANDLE      handle = mir3da_data_ptr->mir_handle;

    mir3da_set_enable(handle, 0);
    sysfs_remove_group(&mir3da_data_ptr->mir3da_idev->dev.kobj, &mir3da_attr_group);
	kobject_put(mir3da_data_ptr->mir3da_kobj);
	kobject_del(mir3da_data_ptr->mir3da_kobj);
	input_unregister_device(mir3da_data_ptr->mir3da_idev);
    misc_deregister(&misc_mir3da);
    input_free_device(mir3da_data_ptr->mir3da_idev);
	hwmon_device_unregister(mir3da_data_ptr->hwmon_dev); 
    flush_workqueue(m_work_info.wq);
    destroy_workqueue(m_work_info.wq);
   
    return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static int mir3da_resume(struct i2c_client *client)
{
	int result = 0;
	MIR_HANDLE handle = mir3da_data_ptr->mir_handle;

	MI_FUN;

	result = mir3da_chip_resume(handle);
	if(result) {
		MI_ERR("%s:chip resume fail!!\n",__func__);
		return -EINVAL;
	}	

	result = mir3da_set_enable(handle, 1);
	if(result) {
		MI_ERR("%s:set enable fail!!\n",__func__);
		return -EINVAL;
	}

	mir3da_close_interrupt(handle);	  

	mir3da_data_ptr->mir3da_idev->open(mir3da_data_ptr->mir3da_idev);

	return result;
}
/*----------------------------------------------------------------------------*/
static int mir3da_detect(struct i2c_client *new_client,
		       struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = new_client->adapter;

	printk("%s:bus[%d] addr[0x%x]\n",__func__, adapter->nr, new_client->addr);

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	if(twi_id == adapter->nr){			
		if(mir3da_install_general_ops(&ops_handle)){
			MI_ERR("Install ops failed !\n");
			return -ENODEV;	
		}

		if(mir3da_module_detect((PLAT_HANDLE)new_client)) {
			MI_ERR("Can't find Mir3da gsensor!!");
		}else{
			MI_ERR("'Find Mir3da gsensor!!");	
			strlcpy(info->type, MIR3DA_DRV_NAME, I2C_NAME_SIZE);
			return 0;
		}
	}

	return  -ENODEV;
}

/*
static int mir3da_shutdown(struct i2c_client *new_client)
{
	int res = 0;

	//res |= mir3da_register_mask_write(new_client, NSA_REG_POWERMODE_BW, 0xFF, 0xDE);

	return res;
}
*/
	
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id mir3da_id[] = {
    { MIR3DA_DRV_NAME, 0 },
    { }
};

MODULE_DEVICE_TABLE(i2c, mir3da_id);
/*----------------------------------------------------------------------------*/
static struct i2c_driver mir3da_driver = {
    .class		= I2C_CLASS_HWMON,
    .driver = {
        .name    = MIR3DA_DRV_NAME,
        .owner    = THIS_MODULE,
    },

    .probe    	= mir3da_probe,
    .resume    	= mir3da_resume,
    .detect		= mir3da_detect,    
    .remove    	= mir3da_remove,
   // .shutdown   = mir3da_shutdown,
    .id_table 	= mir3da_id,
};
/*----------------------------------------------------------------------------*/
static int __init mir3da_init(void)
{    
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	mir3da_data_ptr = kzalloc(sizeof(struct mir3da_data), GFP_KERNEL);
	if (!mir3da_data_ptr){
		printk(KERN_ERR "[stk8312]%s:memory allocation error\n", __func__);
		return -1;
	}

	mir3da_data_ptr->acc_int_handle = 0;
	mir3da_data_ptr->gs_suspend_en = 0;
	mir3da_data_ptr->acc_det_status = 0;
	mir3da_data_ptr->acc_det_count = 0;
	mir3da_data_ptr->acc_irq_number = 0;

    MI_FUN;	
	printk("===================================================================\n");
	
	if(item_exist("sensor.grivaty") && (item_equal("sensor.grivaty.model", "mir3da", 0) || DoI2cScanDetectDev())){
		memset(&info, 0, sizeof(struct i2c_board_info));
		info.addr = 0x27;
		strlcpy(info.type, "mir3da", I2C_NAME_SIZE);
		adapter = i2c_get_adapter(item_integer("sensor.grivaty.ctrl", 1));
		if (!adapter) {
			printk("[stk8312]*******get_adapter error!\n");
		}

		mir3da_data_ptr->this_client = client = i2c_new_device(adapter, &info);

		return i2c_add_driver(&mir3da_driver);
	}
	else {
		printk("[mir3da]%s: G-sensor is not mir3da or exist.\n", __func__);
		return -1;
	}
}
/*----------------------------------------------------------------------------*/
static void __exit mir3da_exit(void)
{
    MI_FUN;
	
    i2c_del_driver(&mir3da_driver);
	kfree(mir3da_data_ptr);
}
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("MiraMEMS <lschen@miramems.com>");
MODULE_DESCRIPTION("MIR3DA 3-Axis Accelerometer driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

module_init(mir3da_init);
module_exit(mir3da_exit);
