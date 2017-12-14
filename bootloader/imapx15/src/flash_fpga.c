#include <bootlist.h>
#include <asm-arm/arch-apollo3/ssp.h>
#include <malloc.h>
#include <ssp.h>
#include <linux/types.h>
#include <vstorage.h>
#include <asm-arm/io.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <linux/byteorder/swab.h>
#include <efuse.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/arch-apollo3/imapx_spiflash.h>
#include <asm-arm/arch-apollo3/imapx_pads.h>
#include <preloader.h>
#include <lowlevel_api.h>
#include <gdma.h>


//extern 

//local definition 
int32_t spi_read_write_data(spi_protl_set_p  protocal_set);
int32_t otf_config_data_register_func(int32_t reg_addr,int32_t	width,uint8_t wires,uint8_t tx_rx,uint8_t reg_en,int32_t data_count );
int flash_nand_vs_read(uint8_t *buf, long offs, int len, uint32_t extra);
int flash_nand_vs_erase(long offs, int len, uint32_t extra);
int flash_nand_vs_write(uint8_t *buf, long offs, int len, uint32_t extra);
int nand_display_oob(void);

int32_t  (*flash_qe_enable)(void);
int32_t  (*flash_qe_disable)(void);
int		 (*flash_wait_device_status)(int cmd,int status);

int32_t  flash_qe_enable_w25q64(void);
int32_t  flash_qe_disable_w25q64(void);

int32_t  flash_qe_enable_wb(void);
int32_t  flash_qe_disable_wb(void);
int		 flash_wait_device_status_wb(int cmd,int status);

int32_t  flash_qe_enable_mx(void);
int32_t  flash_qe_disable_mx(void);
int		 flash_wait_device_status_mx(int cmd,int status);

int32_t  flash_qe_enable_null(void);
int32_t  flash_qe_disable_null(void);
int		 flash_wait_device_status_pn(int cmd,int status);

int32_t  flash_qe_enable_nand(void);
int32_t  flash_qe_disable_nand(void);
int		 flash_wait_device_status_nand(int cmd,int status);

extern struct irom_export *irf; 
//#define writel(v,a)   (*(volatile unsigned int *)(a) = (v))
//#define readl(a)			(*(volatile unsigned int *)(a))
#define spi_debug(format,args...)  \
	if(spi_log_state)\
		irf->printf(format,##args)
#define ARRAY_SIZE(x)    (sizeof(x)/sizeof(x[0]))

const flash_dev_info_t flash_dev_list[] = {

	{ MX25L25635E, SIZE_256M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ W25Q64, SIZE_64M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ W25Q128, SIZE_128M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ W25Q256, SIZE_256M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ PN25F128B, SIZE_128M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ XT25F128A, SIZE_128M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ GD25Q127C, SIZE_128M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
	{ GD25Q64C, SIZE_64M, FLASH_ERASE_SIZE, FLASH_PAGE_SIZE },
};

const flash_mfr_info_t flash_mfr_list[] = {
	{
		.jedec_mfr = MFR_MACRONIX,
		.flash_qe_enable_func = flash_qe_enable_mx,
		.flash_qe_disable_func = flash_qe_disable_mx,
		.flash_wait_device_status_func = flash_wait_device_status_mx,
	},

	{
		.jedec_mfr = MFR_WINBOND,
		.flash_qe_enable_func = flash_qe_enable_wb,
		.flash_qe_disable_func = flash_qe_disable_wb,
		.flash_wait_device_status_func = flash_wait_device_status_wb,
	},

	{
		.jedec_mfr = MFR_PN,
		.flash_qe_enable_func = flash_qe_enable_null,
		.flash_qe_disable_func = flash_qe_disable_null,
		.flash_wait_device_status_func = flash_wait_device_status_pn,
	},

	{
		.jedec_mfr = MFR_XT,
		.flash_qe_enable_func = flash_qe_enable_null,
		.flash_qe_disable_func = flash_qe_disable_null,
		.flash_wait_device_status_func = flash_wait_device_status_pn,
	},

	{
		.jedec_mfr = MFR_GIGADEVICE,
		.flash_qe_enable_func = flash_qe_enable_wb,
		.flash_qe_disable_func = flash_qe_disable_wb,
		.flash_wait_device_status_func = flash_wait_device_status_wb,
	},

};

spi_manager_glob spi_manager_g;
static uint32_t FLASH_SECTOR_SIZE = 256;
static int spi_log_state = 0;

//#define     NAND_TIME_TEST 1
#define		COMPILE_FOR_RTL		0
//#define NAND_TIME_TEST

enum pads_modules {
	/* start from a none-zero number */
	PADS_M_I2C = 10,
	PADS_M_SPI,
	PADS_M_MMC0,
	PADS_M_MMC1,
	PADS_M_MMC2,
    PADS_M_UART,
    PADS_M_GMAC,
    PADS_M_PWM0,
    PADS_M_PWM1,
    PADS_M_PWM2,
    PADS_M_PWM3,
    PADS_M_PWM4,
};
struct pads_modules_bundle {
	enum pads_modules m;	/* modules index */
	uint32_t id;			/* pads indexs */
	uint32_t pt;			/* pull pattern */
    uint32_t break_point[2];
};

static struct pads_modules_bundle bundle_list[] = {
        {PADS_M_SPI , PADSRANGE(70, 74), 0x00b, {80}},
        {0, 0, 0, {0}},
};

static void gpio_out(int num,int value)
{
    *(unsigned int *)(0x20f00000+num*0x40+0x4) =!!value;
}

static void gpio_init(int num)
{
    *((unsigned int *)(0x21e0906c+(num/8)*4)) |= 1<<(num%8);//GPIO function
    *(unsigned int *)(0x20f00000+num*0x40+0x8) = 1;//direct:output
    *(unsigned int *)(0x20f00000+num*0x40+0x4) = 1;//high
}


static int pads_init_module(enum pads_modules id)
{
	int i,j,index;

	writel(readl(PAD_SYSM_ADDR)|(0x1<<2), PAD_SYSM_ADDR);
	spi_debug("PAD_SYSM_ADDR = 0x%x\n", readl(PAD_SYSM_ADDR));

	for(i = 0; bundle_list[i].m; i++) {
		if (bundle_list[i].m == id) {
			irf->pads_chmod(bundle_list[i].id, 0, 0);
			irf->pads_pull(bundle_list[i].id, 1, 1);
            for( j = 0 ; j < ARRAY_SIZE(bundle_list[i].break_point); j++) 
            { 
                index = bundle_list[i].break_point[j];
                if( index > 0 ){ 
                    irf->pads_chmod(index, 0, 0);
					irf->pads_pull(index, 1, 1);
                }
            }
		}
	}

	gpio_init(74);
	gpio_out(74, 1);
	return 0;
}



void ssp_log_open(void)
{
	spi_log_state = ~spi_log_state;
}

/*
 * Swab bits.exg:
 * 00001-->10000
 * 00111-->11100
 */
static int swab_bits(int data,int bits)
{
    int     i = 0;
    int     val = 0 ;
	if(	bits <= 1 )
		return SPI_OPERATE_FAILED;
    for( i = 0 ; i < bits ; i ++)
    {
        val |= ((data>>(bits-1-i))&0x1)<<i;
    }
    return val;
}

void ssp_manager_set_wire(uint32_t wires)
{
	spi_manager_g.g_wires = wires;
}


void ssp_manager_set_rwmode(uint32_t rw_mode)
{
	spi_manager_g.rw_mode = rw_mode;
	if ( rw_mode < SPI_POLLING || rw_mode >= SPI_MAX )
		spi_manager_g.rw_mode = SPI_POLLING;
	if ( rw_mode == SPI_DMA  )
		spi_manager_g.rw_mode = SPI_POLLING;
}

int32_t ssp_manager_set_flash_type(uint32_t flash_type)
{
	if( spi_manager_g.inited ){
		spi_manager_g.flash_type = flash_type;
		if( flash_type == SPI_NAND_FLASH )
			spi_manager_g.flash_sector_size = 2*1024;
		else
			spi_manager_g.flash_sector_size = 64*1024;

		return SPI_SUCCESS;
	}else{
		return SPI_OPERATE_FAILED;
	}
}

void ssp_manager_init(void)
{
	irf->memset( &spi_manager_g, 0x0 ,sizeof(spi_manager_g));

	spi_manager_g.g_size			= 0;
	spi_manager_g.g_page_size		= FLASH_PAGE_SIZE;
	spi_manager_g.g_erase_size		= FLASH_ERASE_SIZE;
	spi_manager_g.g_addr_width		= 3;
//	spi_manager_g.g_addr_width		= 4;
	spi_manager_g.g_wires			= 4;//p_mgr->g_wires;
//	spi_manager_g.g_wires			= 1;//p_mgr->g_wires;
	spi_manager_g.inited			= 1;
//	spi_manager_g.rw_mode			= SPI_POLLING;//p_mgr->rw_mode;
	spi_manager_g.rw_mode			= SPI_DMA;//p_mgr->rw_mode;

	ssp_manager_set_flash_type(SPI_NOR_FLASH);
	//ssp_manager_set_wire(1);
}

void display_data(uint8_t* buf,int buf_len)                                 
{                                                                               
#if 1
	int         i = 0;                                                          
	spi_debug("\n");

	spi_debug("==============display data===============\n");
//	for ( i = 0 ; i < buf_len ; i ++ ) {
	for ( i = 0 ; i < 50 ; i ++ ) {
		if( (i%16) == 0 )
			spi_debug("\n");
		spi_debug("0x%x ",buf[i]);
		//spi_debug("%-4d",buf[i]);
	}
	spi_debug("\n");
#endif
}

void otf_config_set_dma_enable(int val) {

	writel(val, SSP_DMACTL_OTF);
	spi_debug("SSP_DR0_OTF = 0x%x\n", readl(SSP_DMACTL_OTF));
}

void otf_config_all_data_en(dctrl0, dctrl1, dctrl2, dctrl3, dctrl4, fdctrl)
{
	writel(dctrl0,SSP_DCTL0_OTF);
	writel(dctrl1,SSP_DCTL1_OTF);
	writel(dctrl2,SSP_DCTL2_OTF);
	writel(dctrl3,SSP_DCTL3_OTF);
	writel(dctrl4,SSP_DCTL4_OTF);
	writel(fdctrl,SSP_FDCTL_OTF);
}

void otf_config_all_data_dis(void)
{
	writel(0x0,SSP_DCTL0_OTF);
	writel(0x0,SSP_DCTL1_OTF);
	writel(0x0,SSP_DCTL2_OTF);
	writel(0x0,SSP_DCTL3_OTF);
	writel(0x0,SSP_DCTL4_OTF);
	writel(0x0,SSP_FDCTL_OTF);
}

int otf_config_phase_polarity(int32_t	value)
{
	int32_t				reg_val = 0;
	//Read reg.
	reg_val = readl( SSP_CTL_OTF );
	//Set the two bits to zero.
	reg_val &= SSP_CLOCK_POL_PH_ZERO_OTF; 
	reg_val |= value;
	writel( reg_val , SSP_CTL_OTF);
	spi_debug("SSP_CTL_OTF = 0x%x\n", readl(SSP_CTL_OTF ));
	return SPI_SUCCESS;
}

int32_t otf_config_rx_threshold(int32_t threshold)
{
	int32_t			val = 0;
	val = readl( SSP_FIFOTHD_OTF );
	val &= SSP_RX_FIFO_DEPTH_CLEAR;
	val |= (threshold&0xff)<<8;
	writel( val , SSP_FIFOTHD_OTF ); 
	val = readl( SSP_FIFOTHD_OTF );
	spi_debug("SSP_FIFOTHD_OT = 0x%x\n", val);
	return SPI_SUCCESS;
}

int32_t otf_config_tx_threshold(int32_t threshold)
{
	int32_t			val = 0;
	val = readl( SSP_FIFOTHD_OTF );  
	val &= SSP_TX_FIFO_DEPTH_CLEAR;
	val |= threshold&0xff;
	writel( val , SSP_FIFOTHD_OTF ); 
	val = readl( SSP_FIFOTHD_OTF );
	spi_debug("SSP_FIFOTHD_OT = 0x%x\n", val);
	return SPI_SUCCESS;
}

int32_t otf_config_tx_fifo_timeout_count(uint32_t count)
{
	int32_t			val = 0;
	if ( count < 0 )
		return SPI_OPERATE_FAILED;
	val = count&0xffffffff;
	writel( val , SSP_TXTICNT_OTF );
	return SPI_SUCCESS;
}

int32_t otf_config_data_register_func(int32_t reg_addr,int32_t	width,
		uint8_t wires,uint8_t tx_rx,uint8_t reg_en,int32_t data_count )
{
	int32_t				val = 0;
	val |= (width)<<8;
	switch(wires)
	{
		case SSP_MODULE_ONE_WIRE_NEW:
			{
				val &= (~(0x3<<4));
				break;
			}
		case SSP_MODULE_TWO_WIRE:
			{
				val &= (~(0x3<<4));
				val |= 0x1<<4;
				break;
			}
		case SSP_MODULE_FOUR_WIRE:
			{
				val &= (~(0x3<<4));
				val |= 0x2<<4;
				break;
			}
		default:
			break;
	}
	val |= (tx_rx) <<1;
	val |= (reg_en)<<0;
	if ( data_count > 0 )
		val |= (data_count-1)<<16;
//	writel( val , reg_addr );
	//spi_debug("Had written data_reg[0x%x] value:[0x%x]!\n",reg_addr,readl(reg_addr));
//	return SPI_SUCCESS;
	return val;
}

int32_t otf_status_is_busy(void)
{
	int32_t		status =0 ;

	status = readl(SSP_FIFOSTAT_OTF)&SSP_BUSY ? 1: 0;
	if ( status & SSP_BUSY )
		spi_debug("Controller is busy reg:[0x%x],bit4!\n",readl(SSP_FIFOSTAT_OTF));
	else
		spi_debug("Controller is not busy reg:[0x%x],bit4!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

int32_t otf_status_recv_fifo_empty(void)
{
	int32_t		status = 0;
	status = readl(SSP_FIFOSTAT_OTF)&SSP_RFE_OTF ? 1 : 0;
#if 1
	if ( status & SSP_RFE_OTF )
		spi_debug("recv fifo is empty reg:[0x%x],bit3!\n",readl(SSP_FIFOSTAT_OTF));
#endif
	return status;
}

int32_t otf_status_transmit_fifo_empty(void)
{
	int32_t		status = 0;
	status = (readl(SSP_FIFOSTAT_OTF)&SSP_TFE_OTF) ? 1 : 0;
	if ( status )
		spi_debug("tx fifo is empty reg:[0x%x],bit1!\n",readl(SSP_FIFOSTAT_OTF));
	else
		spi_debug("tx fifo is not empty reg:[0x%x],bit1!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

int32_t otf_status_transmit_fifo_full(void)
{
	int32_t		status = 0;
	status = (readl(SSP_FIFOSTAT_OTF)& SSP_TFF_OTF) ? 1 : 0;
	if ( status )
		spi_debug("tx fifo is full reg:[0x%x],bit1!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

//============for 1/2/4
/*
 *(1)Reg: SSP interrupt mask set or clear reg 
 *(2)Receive overrun interrupt 
 */
int otf_set_intr_recv_overrun(int set_clear)
{
	int32_t			reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	//Clear reg,bit[0]
	reg_val &= SSP_ROR_INTR_CLEAR_OTF;
	reg_val |= SSP_ROR_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}

/*
 *(1)Reg: SSP interrupt mask set or clear reg 
 *(2)Receive fifo interrupt 
 */
int otf_set_intr_recv_fifo(int set_clear)
{
	int32_t			reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	//Clear reg,bit[2]
	reg_val &= SSP_RF_INTR_CLEAR_OTF;
	reg_val |= SSP_RF_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}


/*
 *(1)Reg: SSP interrupt mask set or clear reg 
 *(2)Transmit fifo interrupt 
 */
int otf_set_intr_transmit_fifo(int set_clear)
{
	int32_t			reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	reg_val &= SSP_TF_INTR_CLEAR_OTF;
	reg_val |= SSP_TF_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}

int otf_set_intr_transmit_timeout(int set_clear)
{
	int32_t			reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	reg_val &= SSP_TT_INTR_CLEAR_OTF;
	reg_val |= SSP_TT_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}

int otf_clear_intr(int mask)
{
	int32_t			reg_val = 0;
	//Read reg
	reg_val = readl(SSP_INTRCLR_OTF);
	reg_val &= ~(mask);
	writel(reg_val , SSP_INTRCLR_OTF);
	spi_debug("SSP_INTRCLR_OTF= 0x%x\n", readl(SSP_INTRCLR_OTF));
	return SPI_SUCCESS;
}


/*
 *(1)Set clock of the module spi
 *(2)Set initialized state of the module
 *(3)
 */
int otf_module_init(char*  spi_father_clk)
{
	uint32_t					apb_clock = 0;
	uint32_t					spi_clock = 0;
	uint32_t					pll_clock;
	uint32_t					reg_val = 0;
	uint32_t					i = 0;

	if ( NULL == spi_father_clk )	
	{
		spi_debug("param is null!\n");
		return SPI_OPERATE_FAILED;
	}
	if( COMPILE_FOR_RTL )
		spi_log_state = 1;

	irf->module_enable(OTF_SYSM_ADDR);
	spi_debug("OTF_SYSM_ADDR+0x1c = 0x%x\n", readl(OTF_SYSM_ADDR+0x1c));
	writel(0x03, SYS_CLOCK_GATE_EN(OTF_SYSM_ADDR));
	spi_debug("OTF_SYSM_ADDR+0x4 = 0x%x\n", readl(OTF_SYSM_ADDR+0x4));
	//Set multiple wire core active 
	reg_val = readl(SSP_MODULE_WIRES_EN);	
	spi_debug("SPI_EN reg val:[0x%x]!\n",reg_val);
	reg_val |= 0x1;
	writel(reg_val,SSP_MODULE_WIRES_EN);
	spi_debug("SPI_EN reg val:[0x%x],enable!\n",readl(SSP_MODULE_WIRES_EN));

	apb_clock = irf->module_get_clock(APB_CLK_BASE);
	//pll_clock = irf->module_get_clock("vpll");
	irf->module_set_clock ( SSP_CLK_BASE, PLL_E, 4);
	pll_clock = 118000000;//118MHz

	//disable ssp module at the beginning
	writel( SSP_OTF_MODULE_DIS , SSP_EN_OTF);
	//disable ssp dma at the beginning
	otf_config_set_dma_enable(SSP_RECV_FIFO_DMA_DISABLE_OTF | SSP_TRANS_FIFO_DMA_DISABLE_OTF);
	//set timeout time to maxium
	otf_config_tx_fifo_timeout_count(0xffffffff);
	//disable all data ctrl register
	otf_config_all_data_dis();
	//clear ssp interruption at the beginning
	otf_clear_intr(SSP_ALL_INTR_STATE_CLEAR_OTF);
	//Set FIFO depth
	otf_config_rx_threshold(16);
	otf_config_tx_threshold(0);
	//Set FIFO width

	if ( pll_clock < 0 )
	{
		spi_debug("Get father [%s]'c clock failed!\n", spi_father_clk );
		return SPI_OPERATE_FAILED;
	}

	if(spi_manager_g.rw_mode == SPI_DMA) {
		i = 4;
	}
	else {
		i = 84;
	}

	spi_clock = pll_clock/i;
	spi_debug("APB:[%d],PLLCLK:[%d], PS:[%d], SPICLK:[%d]!\n",apb_clock,pll_clock,i,spi_clock);
	//Set clock prescaler
	writel( i, SSP_CFDF_OTF);
	spi_debug("SSP_CFDF_OTF= 0x%x\n", readl(SSP_CFDF_OTF));
	otf_config_phase_polarity(SSP_CLOCK_POL1_PH1_FOR_OTF);
	return 0;
}

/*
 *(1)Set clock of the module spi
 *(2)Set initialized state of the module
 *(3)
 */
int ssp_module_disable()
{


	writel(0x01, SYS_SYNCRST_CLK_EN(SSP_SYSM_ADDR));
	writel(0x3f, SYS_SOFT_RESET(SSP_SYSM_ADDR));

	writel(0x00, SYS_BUS_ISOLATION_R(SSP_SYSM_ADDR));
	writel(0x00, SYS_CLOCK_GATE_EN(SSP_SYSM_ADDR));

	writel(0x00, SYS_SYNCRST_CLK_EN(SSP_SYSM_ADDR));
	writel(0x00, SYS_SOFT_RESET(SSP_SYSM_ADDR));

	return 0;
}

/*
 *(1)Must use one line.
 * */
int32_t  flash_enter_4byte(void)
{
	//spi_debug("========Write enable start======\n");
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = FLASH_CMD_EN_4B;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );
	//spi_debug("========Write enable over======\n");
	return SPI_SUCCESS;
}

/*
 *(1)Must use one line.
 * */
int32_t  flash_exit_4byte(void)
{
	//spi_debug("========Write enable start======\n");
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = FLASH_CMD_EX_4B;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );
	//spi_debug("========Write enable over======\n");
	return SPI_SUCCESS;
}

/*
 *(1)Must use one line.
 * */
uint32_t  flash_read_id(void) {

	static uint32_t				jedec = 0;

	uint8_t				buf[3];
	uint32_t				i;

	if(jedec)
		return jedec;

	//spi_debug("========Read JEDEC start======\n");
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = FLASH_CMD_RD_JEDEC;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 3;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	jedec = (buf[0] << 16) | (buf[1] << 8) | (buf[2]);
	spi_debug("jedec = 0x%x\n", jedec);
	if(spi_manager_g.flash_type == SPI_NAND_FLASH) {
		flash_qe_enable = flash_qe_enable_nand;
		flash_qe_disable = flash_qe_disable_nand;
		flash_wait_device_status = flash_wait_device_status_nand;
	}
	else {
		for(i = 0; i < ARRAY_SIZE(flash_mfr_list); i++) {
			if(flash_mfr_list[i].jedec_mfr == JEDEC_MFR(jedec)) {
				spi_manager_g.flash_device = jedec;
				flash_qe_enable = flash_mfr_list[i].flash_qe_enable_func;
				flash_qe_disable = flash_mfr_list[i].flash_qe_disable_func;
				flash_wait_device_status = flash_mfr_list[i].flash_wait_device_status_func;
				spi_debug("flash device found! jedec = 0x%x\n", spi_manager_g.flash_device);
				break;
			}
		}
		if(i == ARRAY_SIZE(flash_mfr_list))
		{ 
			irf->printf("flash device not found! use default here!\n");
			spi_manager_g.flash_device = jedec;
			flash_qe_enable = flash_mfr_list[0].flash_qe_enable_func;
			flash_qe_disable = flash_mfr_list[0].flash_qe_disable_func;
			flash_wait_device_status = flash_mfr_list[0].flash_wait_device_status_func;
			//while(1);
		}
	}

	//a freak chip request different operation from others made by winbone company
	if(jedec == W25Q64) {
		flash_qe_enable = flash_qe_enable_w25q64;
		flash_qe_disable = flash_qe_disable_w25q64;
	}

	for(i = 0; i < ARRAY_SIZE(flash_dev_list); i++) {
		if(flash_dev_list[i].jedec == jedec) {
			spi_manager_g.g_size = flash_dev_list[i].size;
			spi_manager_g.g_page_size = flash_dev_list[i].page_size;
			spi_manager_g.g_erase_size = flash_dev_list[i].erase_size;
			if(flash_dev_list[i].size > SIZE_128M) {
				spi_manager_g.g_addr_width = 4;
				flash_enter_4byte();
			}
			else {
				spi_manager_g.g_addr_width = 3;
			}
		}

	}
	spi_debug("jedec=0x%x, size=%d, page_size=%d, erase_size=%d, addr_width=%d\n", spi_manager_g.flash_device, spi_manager_g.g_size, spi_manager_g.g_page_size, spi_manager_g.g_erase_size, spi_manager_g.g_addr_width);
	//spi_debug("========Read JEDEC over======\n");
	return jedec;
}

/*
 *(1)Must use one line.
 * */
int32_t  flash_write_enable(void)
{
	//spi_debug("========Write enable start======\n");
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = FLASH_CMD_WR_EN ;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );
	//spi_debug("========Write enable over======\n");
	return SPI_SUCCESS;
}
/*
 *(1)Must use 1/2 wires
 *(2)Can't use 4 wire
 * */
int32_t  flash_qe_enable_mx(void)
{
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg = 0;
	int					flash_config_reg = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//2.read status register
	flash_status_reg = flash_wait_device_status(FLASH_CMD_RD_STAT_REG, FLASH_STAT_REG_RD);
	flash_config_reg = flash_wait_device_status(FLASH_CMD_RD_CFG_REG, FLASH_CFG_REG_RD);
//		spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	//tmp!!!!!!!!!!error
	if( flash_status_reg == 0xff)
		flash_status_reg = 0;

	if ( flash_status_reg & (0x1<<6))
		return 0;

	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG,FLASH_DEV_WREN);
	//3.enable qe
	flash_status_reg |= (0x1 << 6);
	//4.write status register
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG;
	spi_trans.commands[1] = flash_status_reg;
	spi_trans.commands[2] = flash_config_reg;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register
	flash_status_reg = flash_wait_device_status(FLASH_CMD_RD_STAT_REG,FLASH_STAT_REG_QE_EN);
		//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

/*
 *(1)Must use 1/2 wires
 *(2)Can't use 4 wire
 * */
int32_t  flash_qe_enable_w25q64(void)
{
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg1 = 0;
	int					flash_status_reg2 = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//2.read status register1 & register2
	flash_status_reg1 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG,FLASH_STAT_REG_RD);
	spi_debug(">>>>>flash status register1:[0x%x]\n",flash_status_reg1);
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_RD);
	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	//tmp!!!!!!!!!!error
	if( flash_status_reg1 == 0xff)
		flash_status_reg1 = 0;
	if( flash_status_reg2 == 0xff)
		flash_status_reg2 = 0;

	//already enable qe
	if ( flash_status_reg2 & (0x1<<1))
		return 0;

	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG_1,FLASH_DEV_WREN);
	//3.enable qe
	flash_status_reg2 |= (0x1 << 1);
	//4.write status register 2
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG;
	spi_trans.commands[1] = flash_status_reg1;
	spi_trans.commands[2] = flash_status_reg2;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_QE_EN);
	//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

/*
 *(1)Must use 1/2 wires
 *(2)Can't use 4 wire
 * */
int32_t  flash_qe_enable_wb(void)
{
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg2 = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//2.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_RD);
	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	//tmp!!!!!!!!!!error
	if( flash_status_reg2 == 0xff)
		flash_status_reg2 = 0;

	if ( flash_status_reg2 & (0x1<<1))
		return 0;

	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG_1,FLASH_DEV_WREN);
	//3.enable qe
	flash_status_reg2 |= (0x1 << 1);
	//4.write status register 2
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG_2;
	spi_trans.commands[1] = flash_status_reg2;
	spi_trans.cmd_count = 2;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_QE_EN);
			//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

int32_t  flash_qe_enable_null(void) {
	return SPI_SUCCESS;
}
/*
 *(1)Must use 1/2 wires
 *(2)Can't use 4 wire
 * */
int32_t  flash_qe_enable_nand(void)
{
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg2 = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_GET_FEATURE_ND,FLASH_DEV_GET_REG_B0_ND);
	flash_status_reg2 |= (0x1 << 0 );
	spi_trans.commands[0] = FLASH_CMD_SET_FEATURE_ND;
	spi_trans.commands[1] = FLASH_FEATURE_FEAT_ADDR_ND;
	spi_trans.commands[2] = flash_status_reg2;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_GET_FEATURE_ND,FLASH_DEV_GET_REG_B0_ND);
//	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

int32_t  flash_qe_disable_nand(void) {
	return 0;
}

int32_t  flash_qe_disable_mx(void)
{
	return 0;
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg = 0;
	int					flash_config_reg = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//1.write enable 
	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG, FLASH_DEV_WREN);
	
	//2.read status register 2
	flash_status_reg = flash_wait_device_status(FLASH_CMD_RD_STAT_REG, FLASH_STAT_REG_RD);
	flash_config_reg = flash_wait_device_status(FLASH_CMD_RD_CFG_REG, FLASH_CFG_REG_RD);
	//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	if( flash_status_reg == 0xff )
		flash_status_reg = 0;

	//3.disable qe
	flash_status_reg &= (~(0x1<<6));
	//4.write status register 2
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG;
	spi_trans.commands[1] = flash_status_reg;
	spi_trans.commands[2] = flash_config_reg;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register 2
	flash_status_reg = flash_wait_device_status(FLASH_CMD_RD_STAT_REG,FLASH_STAT_REG_QE_DIS);
//	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

int32_t  flash_qe_disable_w25q64(void)
{
	return 0;
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg1 = 0;
	int					flash_status_reg2 = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//1.write enable
	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG_1,FLASH_DEV_WREN);
	//2.read status register1 & register2
	flash_status_reg1 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG,FLASH_STAT_REG_RD);
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_RD);
	//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	if( flash_status_reg1 == 0xff )
		flash_status_reg1 = 0;

	if( flash_status_reg2 == 0xff )
		flash_status_reg2 = 0;

	//3.disable qe
	flash_status_reg2 &= (~(0x1<<1));
	//4.write status register 2
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG;
	spi_trans.commands[1] = flash_status_reg1;
	spi_trans.commands[2] = flash_status_reg2;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_QE_DIS);
	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

int32_t  flash_qe_disable_wb(void)
{
	return 0;
	//spi_debug("========Write enable start======\n");
	int					flash_status_reg2 = 0;
	spi_protl_set       spi_trans;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	//1.write enable
	flash_write_enable();
	flash_wait_device_status(FLASH_CMD_RD_STAT_REG_1,FLASH_DEV_WREN);
	//2.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_RD);
	//spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);

	if( flash_status_reg2 == 0xff )
		flash_status_reg2 = 0;

	//3.disable qe
	flash_status_reg2 &= (~(0x1<<1));
	//4.write status register 2
	spi_trans.commands[0] = FLASH_CMD_WR_STAT_REG_2;
	spi_trans.commands[1] = flash_status_reg2;
	spi_trans.cmd_count = 2;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	//5.read status register 2
	flash_status_reg2 = flash_wait_device_status(FLASH_CMD_RD_STAT_REG_2,FLASH_STAT_REG2_QE_DIS);
	spi_debug(">>>>>flash status register2:[0x%x]\n",flash_status_reg2);
	return SPI_SUCCESS;
}

int32_t  flash_qe_disable_null(void) {
	return SPI_SUCCESS;
}

int flash_wait_device_status_nand(int cmd,int status)
{
	spi_protl_set			spi_trans;
	uint8_t				buf[4];
	uint8_t					val = 0;

	if ( status < 0 )
	{
		spi_debug("Param err,value=[0x%x]!\n",status);
		return SPI_OPERATE_FAILED;
	}
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = (uint8_t)cmd;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	switch(status)
	{
		case FLASH_DEV_GET_REG_B0_ND:
			{
				//for nand flash,read status reg
				spi_trans.commands[0] = (uint8_t)cmd;
				spi_trans.commands[1] = FLASH_FEATURE_FEAT_ADDR_ND;
				spi_trans.cmd_count	  = 2;
				spi_trans.cmd_type    = CMD_READ_DATA;
				irf->memset(buf,0x0,2);
				spi_trans.read_buf	  = buf;
				spi_trans.read_count  = 1;
				spi_read_write_data( &spi_trans );
				spi_debug("read flash reg B0,val:[0x%x]\n",buf[0]);
				return buf[0];
			}
		case FLASH_DEV_RDY_ND:
			{
				//for nand flash,wait dev to be ready
				spi_trans.commands[0] = (uint8_t)cmd;
				spi_trans.commands[1] = FLASH_FEATURE_STAT_ADDR_ND;
				spi_trans.cmd_count = 2;
				spi_trans.cmd_type = CMD_READ_DATA;
				while(1){
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					if( !(buf[0] & (0x1<<0)) )
						break;
					spi_debug("Wait flash to be ready,val:[0x%x],try again\n",buf[0]);
					irf->udelay(10*1000);
				}
				break;
			}

		default:
			break;
	}
	spi_debug("========wait dev status over,status:[0x%x]======\n",buf[0]);
	return SPI_SUCCESS;
}

/*
 *(1)Read status reg
 *(2)reg:
 *		FLASH_CMD_RD_STAT_REG_1
 *		FLASH_CMD_RD_STAT_REG_2
 *(3)Only support 1 wire(SPI MODE) or QPI mode in spec w25q128.
 */
int flash_wait_device_status_wb(int cmd,int status)
{
	spi_protl_set			spi_trans;
	uint8_t				buf[4];
	uint8_t					val = 0;
	uint8_t					write_enable_flag = 0x1<<1;	
	uint8_t					dev_ready_flag = 0x1<<0;	


	if ( status < 0 )
	{
		spi_debug("Param err,value=[0x%x]!\n",status);
		return SPI_OPERATE_FAILED;
	}
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = (uint8_t)cmd;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	switch(status)
	{
		case FLASH_DEV_WREN:
			{
				while(1)
				{
					//flash_write_enable();
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//write enable && ready 
					if ( ((buf[0] & write_enable_flag)) && ( ! (buf[0] & dev_ready_flag) ) )
					{
						spi_debug("write enable,status:[0x%x]!\n",buf[0]);
						break;
					}
					//tmp
					if (!((buf[0] & write_enable_flag)));
						flash_write_enable();

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				val = buf[0];
				return val;
				//break;
			}

		case FLASH_DEV_RDY:
			{
				while(1) {
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//ready
					if ( !(buf[0] & dev_ready_flag) )
					{
						spi_debug("ready,status:[0x%x]!\n",buf[0]);
						break;
					}

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				break;
			}

		case FLASH_STAT_REG_RD:
		case FLASH_STAT_REG2_RD:
			{
				irf->memset(buf,0x0,2);
				spi_trans.read_buf = buf;
				spi_read_write_data( &spi_trans );
				val = buf[0];
				return val;
			}
		case FLASH_STAT_REG2_QE_EN:
			{
				while(1){
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					if(buf[0] & (0x1<<1))
						break;
					spi_debug("Wait flash status reg2:[0x%x] QE as 1\n",buf[0]);
					irf->udelay(1*1000*1000);
				}
				val = buf[0];
				return val;
			}

		case FLASH_STAT_REG2_QE_DIS:
			{
				while(1){
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					if( !(buf[0] & (0x1<<1)) )
						break;
					spi_debug("Wait flash status reg2:[0x%x] QE as 0\n",buf[0]);
					irf->udelay(1*1000*1000);
				}
				val = buf[0];
				return val;
			}

		default:
			break;
	}
	spi_debug("========wait dev status over,status:[0x%x]======\n",buf[0]);
	return SPI_SUCCESS;
}

/*
 *(1)Read status reg
 *(2)reg:
 *		FLASH_CMD_RD_STAT_REG_1
 *		FLASH_CMD_RD_STAT_REG_2
 *(3)Only support 1 wire(SPI MODE) or QPI mode in spec w25q128.
 */
int flash_wait_device_status_mx(int cmd,int status)
{
	spi_protl_set			spi_trans ;
	uint8_t				buf[4];
	uint8_t					val = 0;
	uint8_t					write_enable_flag = 0x1<<1;
	uint8_t					dev_ready_flag = 0x1<<0;


	if ( status < 0 )
	{
		spi_debug("Param err,value=[0x%x]!\n",status);
		return SPI_OPERATE_FAILED;
	}
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = (uint8_t)cmd;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	switch(status)
	{
		case FLASH_DEV_WREN:
			{
				while(1)
				{
					//flash_write_enable();
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//write enable && ready
					if ( ((buf[0] & write_enable_flag)) && ( ! (buf[0] & dev_ready_flag) ) )
					{
						spi_debug("write enable,status:[0x%x]!\n",buf[0]);
						break;
					}
					//tmp
					if (!((buf[0] & write_enable_flag)));
						flash_write_enable();

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				break;
			}
		case FLASH_DEV_RDY:
			{
				while(1) {
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//ready
					if (!(buf[0] & dev_ready_flag) )
					{
						spi_debug("ready,status:[0x%x]!\n",buf[0]);
						break;
					}

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				break;
			}
		case FLASH_CFG_REG_RD:
		case FLASH_STAT_REG_RD:
			{
				irf->memset(buf,0x0,2);
				spi_trans.read_buf = buf;
				spi_read_write_data( &spi_trans );
				val = buf[0];
				return val;
			}
		case FLASH_STAT_REG_QE_EN:
			{
				while(1){
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					if(buf[0] & (0x1<<6))
						break;
					spi_debug("Wait flash status reg2:[0x%x] QE as 1\n",buf[0]);
					irf->udelay(1*1000*1000);
				}
				val = buf[0];
				return val;
			}

		case FLASH_STAT_REG_QE_DIS:
			{
				while(1){
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					if( !(buf[0] & (0x1<<6)) )
						break;
					spi_debug("Wait flash status reg2:[0x%x] QE as 0\n",buf[0]);
					irf->udelay(1*1000*1000);
				}
				val = buf[0];
				return val;
			}

		default:
			break;
	}
	spi_debug("========wait dev status over,status:[0x%x]======\n",buf[0]);
	return SPI_SUCCESS;
}

/*
 *(1)Read status reg
 *(2)reg:
 *		FLASH_CMD_RD_STAT_REG_1
 *		FLASH_CMD_RD_STAT_REG_2
 *		FLASH_CMD_RD_STAT_REG_3
 *(3)Only support 1 wire(SPI MODE) or QPI mode in spec w25q128.
 */
int flash_wait_device_status_pn(int cmd,int status)
{
	spi_protl_set			spi_trans;
	uint8_t				buf[4];
	uint8_t					val = 0;
	uint8_t					write_enable_flag = 0x1<<1;	
	uint8_t					dev_ready_flag = 0x1<<0;	


	if ( status < 0 )
	{
		spi_debug("Param err,value=[0x%x]!\n",status);
		return SPI_OPERATE_FAILED;
	}
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = (uint8_t)cmd;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	switch(status)
	{
		case FLASH_DEV_WREN:
			{
				while(1)
				{
					//flash_write_enable();
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//write enable && ready 
					if ( ((buf[0] & write_enable_flag)) && ( ! (buf[0] & dev_ready_flag) ) )
					{
						spi_debug("write enable,status:[0x%x]!\n",buf[0]);
						break;
					}
					//tmp
					if (!((buf[0] & write_enable_flag)));
						flash_write_enable();

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				val = buf[0];
				return val;
				//break;
			}

		case FLASH_DEV_RDY:
			{
				while(1) {
					irf->memset(buf,0x0,2);
					spi_trans.read_buf = buf;
					spi_read_write_data( &spi_trans );
					//ready
					if ( !(buf[0] & dev_ready_flag) )
					{
						spi_debug("ready,status:[0x%x]!\n",buf[0]);
						break;
					}

					spi_debug("flash status:[0x%x]!\n",buf[0]);
					irf->udelay(50);
				}
				break;
			}

		case FLASH_STAT_REG_RD:
		case FLASH_STAT_REG2_RD:
		case FLASH_STAT_REG3_RD:
			{
				irf->memset(buf,0x0,2);
				spi_trans.read_buf = buf;
				spi_read_write_data( &spi_trans );
				val = buf[0];
				return val;
			}

		//there is no QE bit in STAT REG0~2
		default:
			break;
	}
	spi_debug("========wait dev status over,status:[0x%x]======\n",buf[0]);
	return SPI_SUCCESS;
}

/*
 *(1) Wait until device ready
 *(2)reg:
 *		FLASH_CMD_RD_STAT_REG_1
 *		FLASH_CMD_RD_STAT_REG_2
 *(3)Only support 1 wire(SPI MODE) or QPI mode in spec w25q128.
 */
int flash_wait_ready(void) {

	if(!flash_wait_device_status)
		return -1;

	flash_wait_device_status(FLASH_CMD_RD_STAT_REG, FLASH_DEV_RDY);
	return 0;

}


int nand_read_page_to_cache(int page_offs,int line_mode)
{
	short					blk_addr = 0;//for nand
	short					page_addr= 0;//for nand
	struct spi_transfer_set	spi_trans ;
	//for nand flash,read to cache first
	//cmd+addr: 0x13+24bit
	//blk_addr	= irf->chufa(page_offs,spi_manager_g.nd_fls_array_org.pages_per_blk);
	//page_addr	= irf->quyu(page_offs,spi_manager_g.nd_fls_array_org.pages_per_blk);
	blk_addr	= page_offs>>6;
	page_addr	= page_offs - blk_addr<<6;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = FLASH_CMD_RD2CACHE_ND;
	spi_trans.commands[1] = 0;//dummy 

	*((short*)(spi_trans.commands+2)) |= ((page_addr & 0x3f)<<8);
	*((short*)(spi_trans.commands+2)) |= swab_bits(swab_bits(blk_addr & 0x3,2),8)<<8;
	*((short*)(spi_trans.commands+2)) |= ((blk_addr&0x3fc)>>2 );
	spi_trans.cmd_type	= CMD_ONLY;
	spi_trans.cmd_count = 4;
	if( line_mode == SSP_MODULE_ONE_WIRE )
		spi_trans.module_lines = SSP_MODULE_ONE_WIRE;
	else
		spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data(&spi_trans);
	flash_wait_device_status(FLASH_CMD_GET_FEATURE_ND,FLASH_DEV_RDY_ND);
	return SPI_SUCCESS;

}
/*
 *(1)extra:high 8,addr lines
 *(2)extra:next 8,modules lines
 *(3)offs must be absolute addr. 
 *(4)addr = page_id*256
 *(5)If you want to divice one nand block into 8 frame,channel_in_sector is 8
 *   if channel_in_sector is 0,one nand block is one frame
 */
int flash_nand_vs_read(uint8_t *buf, long offs, int len, uint32_t extra)
{
	uint8_t					line_mode = 0;
	uint8_t					dma_mode  = 0;
	struct spi_transfer_set	spi_trans ;
	uint32_t				sectors	  = 0;
	uint32_t				rest_in_sector = 0;
	uint32_t				i = 0;
	int						page_offs= 0;
	//uint32_t				channel_in_sector = 8;
	uint32_t				channel_in_sector = 0;
	uint32_t				channels = 0;

	uint32_t				channel_size = 0;
	uint32_t				channel_rest = 0;
	int						jedec = 0;

	spi_debug("flash_nand_vs_read\n");

	if ( spi_manager_g.inited != 1) {
		spi_debug("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}

	jedec = flash_read_id();
	FLASH_SECTOR_SIZE = spi_manager_g.flash_sector_size;
	line_mode = spi_manager_g.g_wires;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	switch(line_mode) {
		/* Four lines */
		case 4:
			spi_trans.commands[0] = FLASH_CMD_RD_QUAD;
			spi_trans.module_lines = SSP_MODULE_FOUR_WIRE;
			break;

		/* Two lines */
		case 2:
			spi_trans.commands[0] = FLASH_CMD_RD_DUAL;
			spi_trans.module_lines = SSP_MODULE_TWO_WIRE;
			break;

		/* One lines */
		case 1:
			spi_trans.commands[0] = FLASH_CMD_RD_FT;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;

		/* One lines */
		default:
			spi_trans.commands[0] = FLASH_CMD_RD_FT;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;
	}
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA; 
	spi_trans.addr_count = spi_manager_g.g_addr_width;
	spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_trans.dummy_count = 1;
	spi_trans.dummy_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_trans.read_buf = buf;
	spi_trans.read_count = FLASH_SECTOR_SIZE;
	spi_trans.flash_addr = offs ;

	if( spi_manager_g.flash_type == SPI_NAND_FLASH ) {
		spi_trans.addr_count = 2;
		spi_trans.dummy_count = 1;
	}

	//sector size
	spi_debug("read %d from spi, FLASH_SECTOR_SIZE = %d\n", len, FLASH_SECTOR_SIZE);
	sectors = ((uint32_t)len) / FLASH_SECTOR_SIZE;
	rest_in_sector = ((uint32_t)len) - (sectors * FLASH_SECTOR_SIZE);

	if(spi_trans.module_lines == SSP_MODULE_FOUR_WIRE) {
			flash_qe_enable();
	}

	spi_debug("Sectors:[%d], rest_in_sector:[%d]\n",sectors, rest_in_sector);
	for ( i = 0 ; i < sectors ; i ++ )
	{
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}

		if( spi_manager_g.flash_type == SPI_NAND_FLASH )
		{
			int		tmp = 0;

			page_offs = offs;
			page_offs += i;
			spi_trans.addr[0] = 0; //wrap addr
			spi_trans.addr[1] = 0; //column addr
			nand_read_page_to_cache(page_offs,SSP_MODULE_ONE_WIRE_NEW);
		}

		spi_debug("Read [%d]bytes from addr:[0x%x] !\n",spi_trans.read_count,spi_trans.flash_addr);
		spi_debug("Sector:[%d] is handling...!\n",i);
		//read buffer
		if( 0 == channel_in_sector ){

			spi_read_write_data(&spi_trans);
			spi_trans.flash_addr += FLASH_SECTOR_SIZE;
		}else{

			/* To make channel_rest to be a zero value, you must carefully choose the channel_in_sector */
			channel_size = FLASH_SECTOR_SIZE / channel_in_sector;
			channel_rest = FLASH_SECTOR_SIZE - (channel_size * channel_in_sector); 
			for( channels = 0; channels < channel_in_sector ; channels ++ )
			{
				spi_trans.read_count = channel_size;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_size;
				spi_trans.addr[0]    = (1+channels);
			}
			if(!channel_rest) {
				spi_trans.read_count = channel_rest;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_rest;
			}
		}

		spi_trans.read_count = FLASH_SECTOR_SIZE;
	}
	if ( rest_in_sector )
	{
		uint32_t		rest_in_channel = 0;
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}
		if( spi_manager_g.flash_type == SPI_NAND_FLASH )
		{
			int		tmp = 0;

			page_offs = offs;
			page_offs += i;
			spi_trans.addr[0] = 0; //wrap addr
			spi_trans.addr[1] = 0; //column addr
			nand_read_page_to_cache(page_offs,line_mode);
		}

		//read buffer
		spi_trans.read_count = rest_in_sector;
		spi_debug("Read [%d]bytes from addr:[0x%x] with [%d] lines!\n",spi_trans.read_count,spi_trans.flash_addr,spi_trans.module_lines);
		if( 0 == channel_in_sector ){
			spi_read_write_data(&spi_trans);
		}else{
			uint32_t				t = 0;
			channel_in_sector = rest_in_sector/channel_size;
			rest_in_channel   = rest_in_sector%channel_size;
			for( t = 0 ; t < channel_in_sector ; t ++ )
			{
				spi_trans.read_count = channel_size;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_size;
				spi_trans.addr[0] = (t+1);
			}
			if( rest_in_channel ){
				spi_trans.read_count = rest_in_channel;
				spi_read_write_data(&spi_trans);
			}
		}
		
	}
	flash_qe_disable();
	display_data(buf,len);
	return len;
}

/*
 *(1)extra:high 8,addr lines
 *(2)extra:next 8,modules lines
 *(3)offs must be absolute addr.
 *(4)addr = page_id*256
 *(5)If you want to divice one nand block into 8 frame,channel_in_sector is 8
 *   if channel_in_sector is 0,one nand block is one frame
 */
int flash_nand_vs_write(uint8_t *buf, long offs, int len, uint32_t extra)
{
	uint8_t					line_mode = 0;
	uint8_t					dma_mode  = 0;
	struct spi_transfer_set	spi_trans ;
	uint32_t				page_size	  = 0;
	uint32_t				pages	  = 0;
	uint32_t				rest_in_page = 0;
	uint32_t				i = 0;
	int						page_offs= 0;
	int						length = len;

	int						jedec = 0;

	if ( spi_manager_g.inited != 1) {
		spi_debug("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}

	jedec = flash_read_id();
	page_size = spi_manager_g.g_page_size;
	line_mode = spi_manager_g.g_wires;
	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	switch(line_mode) {
		/* Four lines */
		case 4:
			spi_trans.module_lines = SSP_MODULE_FOUR_WIRE;
			if(JEDEC_MFR(jedec) == MFR_MACRONIX) {
				spi_trans.commands[0] = FLASH_CMD_4PP;
				spi_trans.addr_lines = SSP_MODULE_FOUR_WIRE;
			}
			else if(JEDEC_MFR(jedec) == MFR_WINBOND || JEDEC_MFR(jedec) == MFR_XT || JEDEC_MFR(jedec) == MFR_PN || JEDEC_MFR(jedec) == MFR_GIGADEVICE) {
				spi_trans.commands[0] = FLASH_CMD_PP_QUAD;
				spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			}
			else {
				spi_debug("unknown flash chip\n");
				return SPI_OPERATE_FAILED;
			}
			break;

		/* One lines */
		case 1:
			spi_trans.commands[0] = FLASH_CMD_PP;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;

		/* One lines */
		default:
			spi_trans.commands[0] = FLASH_CMD_PP;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;
	}
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_WRITE_DATA;
	spi_trans.addr_count = spi_manager_g.g_addr_width;
	spi_trans.dummy_count = 0;
	spi_trans.flash_addr = offs ;
	spi_trans.write_buf = buf;

	spi_debug("write %d to spi, page_size = %d\n", length, page_size);

	if(spi_trans.module_lines == SSP_MODULE_FOUR_WIRE) {
		flash_qe_enable();
	}

	page_offs = offs & (page_size - 1);
	if(page_offs) {

		spi_trans.write_count = length < page_size ? length : page_size - page_offs;
		spi_debug("page_offs = %d, write_count = %d\n", page_offs, spi_trans.write_count);
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		length -= spi_trans.write_count;
		spi_trans.flash_addr += spi_trans.write_count;
	}

	//page size
	pages = ((uint32_t)length) / page_size;
	rest_in_page = ((uint32_t)length) - (pages * page_size);


	spi_debug("Pages:[%d], rest_in_page:[%d]\n",pages, rest_in_page);
	for ( i = 0 ; i < pages ; i ++ )
	{

		spi_trans.write_count = page_size;
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}

		spi_debug("Write [%d]bytes from addr[0x%x] to addr:[0x%x] !\n",spi_trans.write_count, spi_trans.write_buf, spi_trans.flash_addr);
		spi_debug("Page:[%d] is handling...!\n",i);
		//read buffer

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		spi_trans.flash_addr += page_size;
	}

	if ( rest_in_page )
	{
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}

		//write buffer
		spi_debug("Write [%d]bytes from addr[0x%x] to addr:[0x%x] !\n",spi_trans.write_count, spi_trans.write_buf, spi_trans.flash_addr);

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
	}

	flash_qe_disable();
	display_data(buf,len);
	return len;
}


/*
 *(1)offs: erase begining addr, must be 4K aligned
 *(2)len: erase total size, must be 4K aligned
 *(3)extra: reserved
 */
int flash_nand_vs_erase(long offs, int len, uint32_t extra)
{
	struct spi_transfer_set	spi_trans ;
	uint32_t				sector_size	  = 0;
	uint32_t				sectors	  = 0;
	uint32_t				i = 0;

	uint32_t				jedec = 0;

#if 1
	if(offs %  spi_manager_g.g_erase_size || len % spi_manager_g.g_erase_size) {
		spi_debug("Erase addr and len must be %d aligned!\n", spi_manager_g.g_erase_size);
		return SPI_OPERATE_FAILED;
	}
#endif

	if ( spi_manager_g.inited != 1) {
		spi_debug("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}

	jedec = flash_read_id();
	if(offs == 0x0 && len >= spi_manager_g.g_size) {

		spi_debug("Erase whole chip !\n");

		flash_wait_ready();
		flash_write_enable();
		irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
		spi_trans.commands[0] = FLASH_CMD_ERAS_CHIP;
		spi_trans.cmd_count = 1;
		spi_trans.cmd_type = CMD_ONLY;
		spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
		spi_read_write_data( &spi_trans );

		return SPI_SUCCESS;

	}

	//sector size
	sector_size = spi_manager_g.g_erase_size;
	sectors = ((uint32_t)len) / sector_size;
#if 0
	if(sectors <= 0) {
		sectors = 1;
	}
#endif
	spi_debug("Erase %d from spi, sector_size = %d\n", len, sector_size);

	irf->memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = FLASH_CMD_ERAS_SECT;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.flash_addr = offs;
	spi_trans.addr_count = spi_manager_g.g_addr_width;
	spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_trans.dummy_count= 0;
	spi_trans.dummy_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_debug("Sectors:[%d]\n",sectors);
	for ( i = 0 ; i < sectors ; i ++ )
	{
		if(spi_trans.addr_count == 3) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 0 )&0xff;
			spi_trans.addr[3] = 0;
		}
		else if(spi_trans.addr_count == 4) {

			spi_trans.addr[0] = (spi_trans.flash_addr >> 24)&0xff;
			spi_trans.addr[1] = (spi_trans.flash_addr >> 16)&0xff;
			spi_trans.addr[2] = (spi_trans.flash_addr >> 8 )&0xff;
			spi_trans.addr[3] = (spi_trans.flash_addr >> 0 )&0xff;
		}

		spi_debug("Erase [%d]bytes from addr:[0x%x] !\n", sector_size, spi_trans.flash_addr);
		spi_debug("Sector:[%d] is handling...!\n",i);

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		spi_trans.flash_addr += sector_size;
	}

	return SPI_SUCCESS;
}

int flash_nand_vs_reset(void)
{
	int ret;
	spi_debug("Flash device reset...!\n");
	ssp_manager_init();
	/*
	 *(1)Author:summer
	 *(2)Date:2015-05-13
	 *(3)We should write command to flash device,let it reset
	 */
	if ( spi_manager_g.inited != 1 )
	{
		spi_debug("spi manager not initialized yet!\n");
		return -1;
	}
	ssp_module_disable();
	otf_module_init(SSP_CLK_SRC);
	//gpio_switch_func_by_module("ssp0",GPIO_FUNC_MODE_MUX0);
	pads_init_module(11);

	if ( spi_manager_g.rw_mode == SPI_INTR ) {
		otf_set_intr_recv_overrun(SSP_ROR_INTR_UNMASK_OTF);
		otf_set_intr_recv_fifo(SSP_RF_INTR_UNMASK_OTF);
		otf_set_intr_transmit_fifo(SSP_TF_INTR_UNMASK_OTF);
		otf_set_intr_transmit_timeout(SSP_TT_INTR_UNMASK_OTF);
	}
	else if ( spi_manager_g.rw_mode == SPI_DMA ) {
		otf_set_intr_recv_overrun(SSP_ROR_INTR_CLEAR_OTF);
		otf_set_intr_recv_fifo(SSP_RF_INTR_CLEAR_OTF);
		otf_set_intr_transmit_fifo(SSP_TT_INTR_CLEAR_OTF);
		otf_set_intr_transmit_timeout(SSP_TF_INTR_CLEAR_OTF);
		//enable ssp dma
		otf_config_set_dma_enable(SSP_RECV_FIFO_DMA_ENABLE_OTF | SSP_TRANS_FIFO_DMA_ENABLE_OTF);
		otf_config_rx_threshold(16);
		otf_config_tx_threshold(16);

		ret = init_gdma();
		if(ret < 0){
			spi_debug("dma init error!\n");
			while(1);
		}
	}
	else {
		otf_set_intr_recv_overrun(SSP_ROR_INTR_CLEAR_OTF);
		otf_set_intr_recv_fifo(SSP_RF_INTR_CLEAR_OTF);
		otf_set_intr_transmit_fifo(SSP_TT_INTR_CLEAR_OTF);
		otf_set_intr_transmit_timeout(SSP_TF_INTR_CLEAR_OTF);
	}
	return 0;
}


int flash_nand_vs_align(void)
{
	return 2048;
}

int32_t spi_read_write_data(spi_protl_set_p  protocal_set)
{
	int8_t			protocal[16] ;//[CMD,ADDR(3bytes),DUMMY)
	int32_t			i = 0;
	int32_t			read_count = 0;
	int32_t			write_count = 0;
	int32_t			write_gen_clk_count = 0;
	int32_t			protocal_len = 0;
	int32_t					val = 0;
	int32_t					wires = 0;
	int32_t			module_wires = 0;
	uint8_t			cmd_value = 0;

	uint32_t			dctrl0 = 0;
	uint32_t			dctrl1 = 0;
	uint32_t			dctrl2 = 0;
	uint32_t			fdctrl = 0;
	uint8_t*			rd_buf = protocal_set->read_buf;
	int32_t			    rd_len = protocal_set->read_count;
	uint8_t*			wr_buf = protocal_set->write_buf;
	int32_t			    wr_len = protocal_set->write_count;
	int32_t			timeout = 0;

	struct gdma_addr src;
	struct gdma_addr dst;

	if ( NULL == protocal_set )
		return SPI_OPERATE_FAILED;

	gpio_out(74, 0);
	spi_debug("Use new ip core...\n");
	while( otf_status_is_busy());
	while( !otf_status_transmit_fifo_empty() );
	while( !otf_status_recv_fifo_empty() );

	//1.Read data to buf
	wires = protocal_set->module_lines;
	for ( i = 0 ; i < protocal_set->cmd_count ; i ++ )
		val |= protocal_set->commands[i] << (protocal_set->cmd_count-1-i)*8;
	///if(protocal_set->cmd_count > 0)
	//	spi_debug("dr0 = 0x%x\n", val);
	writel( val,SSP_DR0_OTF );

	val = 0;
	for ( i = 0 ; i < protocal_set->addr_count ; i ++)
		val |= protocal_set->addr[i] << (protocal_set->addr_count-1-i)*8; 
	//if(protocal_set->addr_count > 0)
	//	spi_debug("dr1 = 0x%x\n", val);
	writel( val, SSP_DR1_OTF );

	val = 0;
	for ( i = 0 ; i < protocal_set->dummy_count ; i ++ )
		val |= protocal_set->dummy << (protocal_set->dummy_count-1-i)*8;
	//if(protocal_set->dummy_count > 0)
	//	spi_debug("dr2 = 0x%x\n", val);
	writel( val, SSP_DR2_OTF );
	otf_config_tx_fifo_timeout_count(0xffffffff);

	module_wires = protocal_set->module_lines;
	cmd_value = protocal_set->commands[0];

	spi_debug("Will use [%d] wire to read/write...\n",module_wires);


	//cmd write  
	if ( protocal_set->cmd_count > 0 )
		dctrl0 = otf_config_data_register_func( SSP_DCTL0_OTF,protocal_set->cmd_count*8-1,SSP_MODULE_ONE_WIRE_NEW,
				SSP_TX_DIR_OTF,SSP_DR_EN_OTF,0);

	//addr write
	if ( protocal_set->addr_count > 0 )
		dctrl1 = otf_config_data_register_func( SSP_DCTL1_OTF,protocal_set->addr_count*8-1,protocal_set->addr_lines,
				SSP_TX_DIR_OTF,SSP_DR_EN_OTF,0);

	//dummy write
	if ( protocal_set->dummy_count > 0)
		dctrl2 = otf_config_data_register_func( SSP_DCTL2_OTF,protocal_set->dummy_count*8-1, protocal_set->dummy_lines,
				SSP_TX_DIR_OTF,SSP_DR_EN_OTF,0);

	spi_debug("cmd = 0x%x, cmd_count = %d, addr_count = %d, addr_lines = %d, dummy_count = %d, dummy_lines = %d, module_lines = %d\n", protocal_set->commands[0], protocal_set->cmd_count, protocal_set->addr_count, protocal_set->addr_lines, protocal_set->dummy_count, protocal_set->dummy_lines, protocal_set->module_lines);

	//rx fifo 
	if ( protocal_set->cmd_type == CMD_READ_DATA ){
		if ( protocal_set->read_count > 0 )
		{
			fdctrl = otf_config_data_register_func( SSP_FDCTL_OTF,SSP_DR_8BITS_OTF,module_wires, SSP_RX_DIR_OTF,SSP_DR_EN_OTF, (protocal_set->read_count));
		}
	}
	//tx fifo
	if ( protocal_set->cmd_type == CMD_WRITE_DATA ){
		if ( protocal_set->write_count > 0 )
		{
			spi_debug("protocal_set->write_count = 0x%x\n", protocal_set->write_count);
			fdctrl = otf_config_data_register_func( SSP_FDCTL_OTF,SSP_DR_8BITS_OTF,module_wires, SSP_TX_DIR_OTF,SSP_DR_EN_OTF, (protocal_set->write_count));
		}
	}
	//read data 
	if ( protocal_set->cmd_type == CMD_READ_DATA )
	{
		//(1)Polling read data
		if( spi_manager_g.rw_mode == SPI_POLLING ||  rd_len < 32)
		{
			spi_debug("dctrl0 = 0x%x, dctrl1 = 0x%x, dctrl2 = 0x%x, fdctrl = 0x%x\n",  dctrl0, dctrl1, dctrl2, fdctrl);

			//enable spi
			otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
			writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);

			//FIFO WIDTH IS 32
			timeout = 0xFFFFFF;
			while(read_count < protocal_set->read_count){

				if ( !otf_status_recv_fifo_empty())
				{
				//	*((int8_t*)protocal_set->read_buf) = __swab32(readl( SSP_FDR_OTF )); 
					while (readl(SSP_RXFIFO_DCNT_OTF)) {
						*((int8_t*)protocal_set->read_buf++) = readl( SSP_FDR_OTF ); 

						spi_debug("read_buf[%d] = 0x%x\n", read_count, *(protocal_set->read_buf - 1));
						read_count = protocal_set->read_buf - rd_buf;
						if( read_count >= protocal_set->read_count ) {
							spi_debug("%s: %d, read finished\n", __func__, __LINE__);
							break;
						}
					}

					timeout = 0xFFFFFF;
				}
				if(timeout-- <= 0) {
					spi_debug("%s: %d, timeout, read_count = %d, protocal_set->read_count = %d\n", __func__, __LINE__, read_count, protocal_set->read_count);
					display_data(rd_buf,rd_len);
					break;
				}
			}
		}
		else if (spi_manager_g.rw_mode == SPI_DMA) {

			spi_debug("Using DMA to receive data.\n");
			src.byte = SSP_FDR_OTF;
			src.modbyte = 0;
			dst.byte = protocal_set->read_buf;
			dst.modbyte = 0;

			/*start transfer*/
			dma_peri_transfer_otf((struct gdma_addr*)&src, (struct gdma_addr*)&dst, (protocal_set->read_count), dctrl0, dctrl1, dctrl2, fdctrl, 1);
			protocal_set->read_buf += protocal_set->read_count;

		}
		else {
			//enable spi
			otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
			writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);
		}
		spi_debug("Read data from fifo over...\n");
	}
	else if (protocal_set->cmd_type == CMD_ONLY) {
		otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
		writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);
	}
	else if ( protocal_set->cmd_type == CMD_WRITE_DATA ){
		//(1)Polling write data
		if( spi_manager_g.rw_mode == SPI_POLLING ||  wr_len < 32)
		{

			spi_debug("dctrl0 = 0x%x, dctrl1 = 0x%x, dctrl2 = 0x%x, fdctrl = 0x%x\n",  dctrl0, dctrl1, dctrl2, fdctrl);
			//enable spi
			otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
			writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);

			//FIFO WIDTH IS 32
			timeout = 0xFFFFFF;
			while(write_count < protocal_set->write_count) {

				if ( !otf_status_transmit_fifo_full())
				{
				//	*((int8_t*)protocal_set->read_buf) = __swab32(readl( SSP_FDR_OTF )); 
					while (readl(SSP_TXFIFO_DCNT_OTF) < 64) {
						writel( *((int8_t*)protocal_set->write_buf++), SSP_FDR_OTF );

						spi_debug("write_buf[%d] = 0x%x, fifo cnt = %d\n", write_count, *((int8_t*)protocal_set->write_buf - 1), readl(SSP_TXFIFO_DCNT_OTF));
						write_count = protocal_set->write_buf - wr_buf;
						if( write_count >= protocal_set->write_count ) {
							spi_debug("%s: %d, write finished\n", __func__, __LINE__);
							break;
						}
					}

					timeout = 0xFFFFFF;
				}
				if(timeout-- <= 0) {
					spi_debug("%s: %d, timeout, write_count = %d, protocal_set->write_count = %d\n", __func__, __LINE__, write_count, protocal_set->write_count);
					display_data(wr_buf,wr_len);
					break;
				}
			}

		}
		else if (spi_manager_g.rw_mode == SPI_DMA) {

			spi_debug("Using DMA to transmit data.\n");
			src.byte = protocal_set->write_buf;
			src.modbyte = 0;
			dst.byte = SSP_FDR_OTF;
			dst.modbyte = 0;

			spi_debug("dctrl0 = 0x%x, dctrl1 = 0x%x, dctrl2 = 0x%x, fdctrl = 0x%x\n",  dctrl0, dctrl1, dctrl2, fdctrl);
			/*start transfer*/
			dma_peri_transfer_otf((struct gdma_addr*)&src, (struct gdma_addr*)&dst, (protocal_set->write_count), dctrl0, dctrl1, dctrl2, fdctrl, 0);
			protocal_set->write_buf += protocal_set->write_count;

		}
		else {
			//enable spi
			otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
			writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);
		}
		spi_debug("Write data from fifo over...\n");
	}
	otf_config_tx_fifo_timeout_count(0x0);
	//end 
	while( otf_status_is_busy());
	while( !otf_status_transmit_fifo_empty() );
	otf_config_all_data_dis();
	//disable spi
	writel( SSP_OTF_MODULE_DIS , SSP_EN_OTF);
	gpio_out(74, 1);

	return SPI_SUCCESS;
}

