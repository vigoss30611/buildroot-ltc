#include <imapx_iic.h>
#include <asm-arm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/arch-apollo3/imapx_pads.h>
#include <asm-arm/types.h>
#define IIC_CLK	     320000
static uint8_t iicreadapistate;

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#define udelay irf->udelay
#define module_enable irf->module_enable
#define module_get_clock irf->module_get_clock
#define pads_chmod irf->pads_chmod
#define pads_pull irf->pads_pull
#endif
#define PADS_MODE_CTRL			0
uint8_t iic_init(uint8_t iicindex, uint8_t iicmode, uint8_t devaddr, uint8_t intren, uint8_t isrestart, uint8_t ignoreack)
{
	int ret;
	printf("enter func %s \n", __func__);

	iicreadapistate = READ_DATA_FIN;	

	module_enable(I2C_SYSM_ADDR);
	
        if(IROM_IDENTITY == IX_CPUID_X15)
	{
		printf("cpu is imapx15\n");
		pads_chmod(PADSRANGE(43, 44), PADS_MODE_CTRL, 0);
		pads_pull(PADSRANGE(43, 44), 1, 1);
	}
	else if(IROM_IDENTITY == IX_CPUID_X9 || IROM_IDENTITY == IX_CPUID_X9_NEW)
	{
		printf("cpu is imapxa9\n");
		pads_chmod(PADSRANGE(54, 55), PADS_MODE_CTRL, 0);
		if(IROM_IDENTITY == IX_CPUID_X9)
			pads_pull(PADSRANGE(54, 55), 1, 1);
		if(IROM_IDENTITY == IX_CPUID_X9_NEW)
			pads_pull(PADSRANGE(54, 55), 1, 3);
	}
	else if(IROM_IDENTITY == IX_CPUID_Q3)
	{
		//printf("cpu is q3\n");
		pads_chmod(PADSRANGE(75, 76), PADS_MODE_CTRL, 0);
		pads_pull(PADSRANGE(75, 76), 1, 1);
	}
	else
	{
		printf("cpu is apollo3\n");
		pads_chmod(PADSRANGE(95, 96), PADS_MODE_CTRL, 0);
		pads_pull(PADSRANGE(95, 96), 1, 1);
	}

#if 0
	temp_reg = readl(GPCCON);                   
	temp_reg &= ~(0x3<<6 | 0x3<<4 | 0x3<<2 | 0x3);
	writel(temp_reg,GPCCON);                    
		                                              
	temp_reg = readl(GPCCON);                   
	temp_reg |= (0x2<<6 | 0x2<<4 | 0x2<<2 | 0x2);
	writel(temp_reg, GPCCON);                   
#endif


	ret = iicreg_init(iicindex,iicmode,devaddr,intren,isrestart,ignoreack);

	return ret;
}
uint8_t iicreg_init(uint8_t iicindex, uint8_t iicmode, uint8_t devaddr, uint8_t intren, uint8_t isrestart, uint8_t ignoreack)
{
	uint32_t temp_reg, scl_h, scl_l;

	if(iicindex != 0 && iicindex != 1) return FALSE;
	writel(0x0,IC_ENABLE);
	writel((FASTMODE | MASTER_MODE | SLAVE_DISABLE),IC_CON);
	//printf("IC_CON: 0x%x \n", readl(IC_CON));

	// values of scl_h and scl_l come from i2c driver in linux kernel.
	scl_h = 0x78;
	scl_l = 0x104;
	writel(scl_h, IC_FS_SCL_HCNT);
	writel(scl_l, IC_FS_SCL_LCNT);
	//printf("IC_FS_SCL_HCNT: 0x%x \n", readl(IC_FS_SCL_HCNT));
	//printf("IC_FS_SCL_LCNT: 0x%x \n", readl(IC_FS_SCL_LCNT));
	
	temp_reg = readl(IC_CON);
	temp_reg |= IC_RESTART_EN;
	writel(temp_reg,IC_CON);
	writel(0x0,IC_INTR_MASK);      
	writel(devaddr>>1,IC_TAR);
	writel(0x1,IC_ENABLE);

	return TRUE;
}

uint8_t iic_writeread(uint8_t iicindex, uint8_t subAddr, uint8_t * data)
{
	if(iicreadapistate == WRITE_ADDR_FIN || iicreadapistate == READ_DATA_FLS)
		goto READ_BEGIN;
	if(iicreg_write(iicindex, &subAddr, 1, NULL, 0, 0)) {
		iicreadapistate = WRITE_ADDR_FIN;
READ_BEGIN:
		if(iicreg_read(iicindex, data, 1))
		{
			iicreadapistate = READ_DATA_FIN;
			return TRUE;
		}
		else
		{
			iicreadapistate = READ_DATA_FLS;
			return FALSE;
		}
	}
	else
	{
		iicreadapistate = WRITE_ADDR_FLS;
		return FALSE;
	}
}

uint8_t iic_read_func(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num)
{
	if(iicreadapistate == WRITE_ADDR_FIN || iicreadapistate == READ_DATA_FLS) 
		goto READ_BEGIN;
	if(iicreg_write(iicindex, subAddr, addr_len, NULL, 0, 1)) {
		iicreadapistate = WRITE_ADDR_FIN;
READ_BEGIN:
		if(iicreg_read(iicindex, data, data_num)) {
			iicreadapistate = READ_DATA_FIN;
			return TRUE;
		} else {
			iicreadapistate = READ_DATA_FLS;
			return FALSE;
		}
	} else {
		iicreadapistate = WRITE_ADDR_FLS;
		return FALSE;
	}
}


uint8_t iicreg_write(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num, uint8_t isstop) {
	uint32_t temp_reg;
	int i;
	int cnt = 0;
	uint32_t num = addr_len + data_num;
	if(iicindex != 0 && iicindex != 1) return FALSE;
	temp_reg = readl(IC_RAW_INTR_STAT);
	if(temp_reg & TX_ABORT)
	{
		readl(IC_TX_ABRT_SOURCE);
		readl(IC_CLR_TX_ABRT);
		return FALSE;
	}
	temp_reg = readl(IC_STATUS);
	if(temp_reg & (0x1<<2)) {
		for(i=0;i<num;i++)
		{
		        while(1) {
		                temp_reg = readl(IC_STATUS);
		                if(temp_reg & (0x1<<1))
		                {
				        if (addr_len) {
//					    printf(":02%x\n", *subAddr);
	 			        	writel(*subAddr,IC_DATA_CMD);
	                                	subAddr++;
						addr_len--;
					} else if (data_num) {
					    	writel(*data, IC_DATA_CMD);
						data++;
						data_num--;
					}
	                        	break;
			         }
		       }
		}
	
		while(1) {
		        temp_reg = readl(IC_STATUS);
		        if (temp_reg & (0x1<<2))
		        {
				if (cnt >= 1000) {
					printf("########### iicreg_write timeout ###########\n");
					return 0;
				}

				if (isstop) {
		                	while(1) {
		                        	temp_reg = readl(IC_RAW_INTR_STAT);
		                        	if (temp_reg & (0x1<<9)) {
		                                	readl(IC_CLR_STOP_DET);
		                                	udelay(6500);
		                                	return TRUE;
		                        	}
					}
		                } else 
					return TRUE;

				//udelay(1000);
				cnt++;
			}
		}
	}
	return 0;
}

uint8_t iicreg_read(uint8_t iicindex, uint8_t *data, uint32_t num) {
	uint32_t temp_reg;
	int readnum = 0;
	int writenum = 0;
    int cnt = 0;

	if(iicindex != 0 && iicindex != 1) return FALSE;
	writenum = num;
	temp_reg = readl(IC_STATUS);
	while(temp_reg & (0x1<<3))
	{
		temp_reg = readl(IC_DATA_CMD);
	}
	while (1) {
	    temp_reg = readl(IC_STATUS);
	    if (temp_reg & (0x1 << 1) && writenum) {
	        writel(0x1<<8,IC_DATA_CMD);
		writenum--;
		}
	    if (temp_reg & (0x1 << 3)) {
		*data = readl(IC_DATA_CMD);
		data++;
		readnum++;
	    }
	    if (readnum == num)
		break;

        //udelay(1000);
        cnt++;
        if(cnt >= 1000)
            return FALSE;
#if 0
		to = get_ticks();
	        while(1) {
	        	temp_reg = readl(IC_STATUS);
		        if (temp_reg & (0x1<<3)) {
				data[i]= readl(IC_DATA_CMD);
      	                        break;
		        }

			tn = get_ticks();
			if ((tn - to) > 600000) {
				return false;
			}

		}
#endif
	}

	return TRUE;
}


