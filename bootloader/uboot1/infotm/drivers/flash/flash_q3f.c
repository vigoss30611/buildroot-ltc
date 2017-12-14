#include <ssp.h>
#include <asm/arch/ssp.h>
#include <asm/arch/imapx_base_reg.h>
#include <asm/arch/imapx_spiflash.h>
#include <asm/arch/imapx_pads.h>
#include <gdma.h>
#include <lowlevel_api.h>
#include <asm-arm/io.h>
#include <cpuid.h>

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

enum pads_modules id = PADS_M_SPI;
spi_manager_glob spi_manager_g;
#define SPI_CS 74
#define	COMPILE_FOR_RTL		0

static int spi_log_state = 0;
#define spi_debug(format,args...)  \
	if(spi_log_state)\
		printf(format,##args)

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

int pads_init_module()
{
	int i,j,index;

	writel(readl(PAD_SYSM_ADDR)|(0x1<<2), PAD_SYSM_ADDR);
	spi_debug("PAD_SYSM_ADDR = 0x%x\n", readl(PAD_SYSM_ADDR));

	for(i = 0; bundle_list[i].m; i++) {
		if (bundle_list[i].m == id) {
			pads_chmod(bundle_list[i].id, 0, 0);
			//pads_pull(bundle_list[i].id, 1, 1);

	            for( j = 0 ; j < ARRAY_SIZE(bundle_list[i].break_point); j++)
	            {
	                index = bundle_list[i].break_point[j];
	                if( index > 0 ){
	                    pads_chmod(index, 0, 0);
				pads_pull(index, 1, 1);
	                }
	            }
		}
	}
	gpio_init(SPI_CS);
	gpio_out(SPI_CS, 1);

	return 0;
}

/*
 * Swab bits.exg:
 * 00001-->10000
 * 00111-->11100
 */
static int swab_bits(int data,int bits)
{
    int i = 0;
    int val = 0 ;
    if(bits <= 1)
	  return SPI_OPERATE_FAILED;

    for( i = 0 ; i < bits ; i ++)
    {
        val |= ((data>>(bits-1-i))&0x1)<<i;
    }
    return val;
}

static void otf_config_set_dma_enable(int val) {
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

static int otf_config_phase_polarity(int32_t	value)
{
	int32_t reg_val = 0;
	//Read reg.
	reg_val = readl( SSP_CTL_OTF );
	//Set the two bits to zero.
	reg_val &= SSP_CLOCK_POL_PH_ZERO_OTF;
	reg_val |= value;
	writel( reg_val , SSP_CTL_OTF);
	spi_debug("SSP_CTL_OTF = 0x%x\n", readl(SSP_CTL_OTF ));
	return SPI_SUCCESS;
}

static int32_t otf_config_rx_threshold(int32_t threshold)
{
	int32_t val = 0;
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
	int32_t val = 0;
	val = readl( SSP_FIFOTHD_OTF );
	val &= SSP_TX_FIFO_DEPTH_CLEAR;
	val |= threshold&0xff;
	writel( val , SSP_FIFOTHD_OTF );
	val = readl( SSP_FIFOTHD_OTF );
	spi_debug("SSP_FIFOTHD_OT = 0x%x\n", val);
	return SPI_SUCCESS;
}

static int32_t otf_config_tx_fifo_timeout_count(uint32_t count)
{
	int32_t val = 0;
	if ( count < 0 )
		return SPI_OPERATE_FAILED;
	val = count&0xffffffff;
	writel( val , SSP_TXTICNT_OTF );
	return SPI_SUCCESS;
}

static int32_t otf_status_transmit_fifo_full(void)
{
	int32_t status = 0;
	status = (readl(SSP_FIFOSTAT_OTF)& SSP_TFF_OTF) ? 1 : 0;
	if ( status )
		spi_debug("tx fifo is full reg:[0x%x],bit1!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

static int32_t otf_config_data_register_func(int32_t reg_addr,int32_t	width,
		uint8_t wires,uint8_t tx_rx,uint8_t reg_en,int32_t data_count )
{
	int32_t val = 0;
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

static int32_t otf_status_is_busy(void)
{
	int32_t status =0 ;
	status = readl(SSP_FIFOSTAT_OTF)&SSP_BUSY ? 1: 0;
	if ( status & SSP_BUSY )
		spi_debug("Controller is busy reg:[0x%x],bit4!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

static int32_t otf_status_recv_fifo_empty(void)
{
	int32_t status = 0;
	status = readl(SSP_FIFOSTAT_OTF)&SSP_RFE_OTF ? 1 : 0;
#if 1
	if ( status & SSP_RFE_OTF )
		spi_debug("recv fifo is empty reg:[0x%x],bit3!\n",readl(SSP_FIFOSTAT_OTF));
#endif
	return status;
}

static int32_t otf_status_transmit_fifo_empty(void)
{
	int32_t status = 0;
	status = (readl(SSP_FIFOSTAT_OTF)&SSP_TFE_OTF) ? 1 : 0;
	if ( status )
		spi_debug("tx fifo is empty reg:[0x%x],bit1!\n",readl(SSP_FIFOSTAT_OTF));
	return status;
}

//============for 1/2/4
/*
 *(1)Reg: SSP interrupt mask set or clear reg
 *(2)Receive overrun interrupt
 */
static int otf_set_intr_recv_overrun(int set_clear)
{
	int32_t reg_val = 0;
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
static int otf_set_intr_recv_fifo(int set_clear)
{
	int32_t reg_val = 0;
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
static int otf_set_intr_transmit_fifo(int set_clear)
{
	int32_t reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	reg_val &= SSP_TF_INTR_CLEAR_OTF;
	reg_val |= SSP_TF_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}

static int otf_set_intr_transmit_timeout(int set_clear)
{
	int32_t reg_val = 0;
	//Read reg
	reg_val = readl( SSP_INTRMASK_OTF );
	reg_val &= SSP_TT_INTR_CLEAR_OTF;
	reg_val |= SSP_TT_INTR_UNMASK_OTF;
	writel( reg_val , SSP_INTRMASK_OTF);
	spi_debug("SSP_INTRMASK_OTF= 0x%x\n", readl(SSP_INTRMASK_OTF));
	return SPI_SUCCESS;
}

static int otf_clear_intr(int mask)
{
	int32_t reg_val = 0;
	//Read reg
	reg_val = readl(SSP_INTRCLR_OTF);
	reg_val &= ~(mask);
	writel(reg_val , SSP_INTRCLR_OTF);
	spi_debug("SSP_INTRCLR_OTF= 0x%x\n", readl(SSP_INTRCLR_OTF));
	return SPI_SUCCESS;
}

static void ssp_manager_set_rwmode(uint32_t rw_mode)
{
	spi_manager_g.rw_mode = rw_mode;
	if ( rw_mode < SPI_POLLING || rw_mode >= SPI_MAX )
		spi_manager_g.rw_mode = SPI_POLLING;
	if ( rw_mode == SPI_DMA  )
		spi_manager_g.rw_mode = SPI_POLLING;
}

static int32_t ssp_manager_set_flash_type(uint32_t flash_type)
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
/*
 *(1)Set clock of the module spi
 *(2)Set initialized state of the module
 *(3)
 */
static int ssp_module_disable()
{
	writel(0x01, SYS_SYNCRST_CLK_EN(SSP_SYSM_ADDR));
	writel(0x3f, SYS_SOFT_RESET(SSP_SYSM_ADDR));

	writel(0x00, SYS_BUS_ISOLATION_R(SSP_SYSM_ADDR));
	writel(0x00, SYS_CLOCK_GATE_EN(SSP_SYSM_ADDR));

	writel(0x00, SYS_SYNCRST_CLK_EN(SSP_SYSM_ADDR));
	writel(0x00, SYS_SOFT_RESET(SSP_SYSM_ADDR));

	return 0;
}

static void ssp_manager_init(void)
{
	memset( &spi_manager_g, 0x0 ,sizeof(spi_manager_g));

	spi_manager_g.g_wires			= 4;
	spi_manager_g.rw_mode			= SPI_DMA;

	spi_manager_g.inited			= 1;
	ssp_manager_set_flash_type(SPI_NOR_FLASH);
}

/*
 *(1)Set clock of the module spi
 *(2)Set initialized state of the module
 *(3)
 */
int otf_module_init()
{
	uint32_t	apb_clock = 0;
	uint32_t	spi_clock = 0;
	uint32_t	pll_clock;
	uint32_t	reg_val = 0;
	uint32_t	i = 0;
	int ret;

	ssp_manager_init();

	ssp_module_disable();
	module_enable(OTF_SYSM_ADDR);
	spi_debug("OTF_SYSM_ADDR+0x1c = 0x%x\n", readl(OTF_SYSM_ADDR+0x1c));
	writel(0x03, SYS_CLOCK_GATE_EN(OTF_SYSM_ADDR));
	spi_debug("OTF_SYSM_ADDR+0x4 = 0x%x\n", readl(OTF_SYSM_ADDR+0x4));
	//Set multiple wire core active
	reg_val = readl(SSP_MODULE_WIRES_EN);
	spi_debug("SPI_EN reg val:[0x%x]!\n",reg_val);
	reg_val |= 0x1;
	writel(reg_val,SSP_MODULE_WIRES_EN);
	spi_debug("SPI_EN reg val:[0x%x],enable!\n",readl(SSP_MODULE_WIRES_EN));

	apb_clock = module_get_clock(APB_CLK_BASE);
	//pll_clock = module_get_clock("vpll");
	//module_set_clock ( SSP_CLK_BASE, PLL_E, 4);  //xiongbiao
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
	otf_config_tx_threshold(16);
	//Set FIFO width

	if (pll_clock < 0)
	{
		spi_debug("Get father spi'c clock failed!\n" );
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

	//gpio_switch_func_by_module("ssp0",GPIO_FUNC_MODE_MUX0);
	pads_init_module();
	//spi_init_pads();
	spi_debug("pads_init_module =ok\n");

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

int32_t spi_read_write_data(spi_protl_set_p  protocal_set)
{
	//int8_t	protocal[16] ;//[CMD,ADDR(3bytes),DUMMY)
	int32_t	i = 0;
	int32_t	read_count = 0;
	int32_t	write_count = 0;
	//int32_t	 write_gen_clk_count = 0;
	//int32_t	 protocal_len = 0;
	int32_t	val = 0;
	int32_t	wires = 0;
	int32_t	module_wires = 0;
	uint8_t	cmd_value = 0;

	uint32_t	dctrl0 = 0;
	uint32_t	dctrl1 = 0;
	uint32_t	dctrl2 = 0;
	uint32_t	fdctrl = 0;
	uint8_t*	rd_buf = protocal_set->read_buf;
	int32_t	rd_len = protocal_set->read_count;
	uint8_t* wr_buf = protocal_set->write_buf;
	int32_t  wr_len = protocal_set->write_count;
	int32_t  timeout = 0;

	struct gdma_addr src;
	struct gdma_addr dst;

	if ( NULL == protocal_set )
		return SPI_OPERATE_FAILED;

	gpio_out(SPI_CS, 0);
	spi_debug("Use new ip core...\n");
	while( otf_status_is_busy());
	while( !otf_status_transmit_fifo_empty() );
	while( !otf_status_recv_fifo_empty() );

	//1.Read data to buf
	wires = protocal_set->module_lines;
	for ( i = 0 ; i < protocal_set->cmd_count ; i ++ )
		val |= protocal_set->commands[i] << (protocal_set->cmd_count-1-i)*8;
	//if(protocal_set->cmd_count > 0)
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
		dctrl2 = otf_config_data_register_func( SSP_DCTL2_OTF,protocal_set->dummy_count*8-1,protocal_set->dummy_lines,
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
					//display_data(rd_buf,rd_len);
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
				//	display_data(wr_buf,wr_len);
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
	else if (protocal_set->cmd_type == CMD_ONLY) {
		otf_config_all_data_en(dctrl0, dctrl1, dctrl2, 0, 0, fdctrl);
		writel( SSP_OTF_MODULE_EN , SSP_EN_OTF);
	}
	otf_config_tx_fifo_timeout_count(0x0);
	//end
	while( otf_status_is_busy());
	while( !otf_status_transmit_fifo_empty() );
	otf_config_all_data_dis();
	//disable spi
	writel( SSP_OTF_MODULE_DIS , SSP_EN_OTF);
	gpio_out(SPI_CS, 1);

	return SPI_SUCCESS;
}

