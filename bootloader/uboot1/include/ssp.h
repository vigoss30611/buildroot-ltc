/*
 * =====================================================================================
 *
 *       Filename:  ssp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/20/2015 03:35:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =================================================================================*/

#ifndef __COMMON_SSP_H__ 
#define __COMMON_SSP_H__
#include <linux/types.h>


typedef int  bool;
#define true    1
#define false   0

typedef struct spi_transfer_set{                                                
	uint8_t     commands[4]; //4 bytes for command                              
	int8_t      cmd_count;   //commands's count                                 
	enum        cmd_type_{                                                      
		CMD_ONLY,                                                               
		CMD_READ_DATA , 
		CMD_WRITE_DATA ,
	}cmd_type;                                                                  
	uint8_t     addr[4];     //4 bytes for addr                                 
	int8_t      addr_count;  //addr's count  
	int8_t      addr_lines;  //addr's line
	int8_t      dummy;                                                          
	int8_t      dummy_count;   
	int8_t      dummy_lines;
	/*                                                                          
	 *(1)SSP_MODULE_ONE_WIRE                                                    
	 *(2)SSP_MODULE_TWO_WIRE                                                    
	 *(3)SSP_MODULE_FOUR_WIRE                                                   
	 *(4)SSP_MODULE_QPI_WIRE                                                    
	 */                                                                         
	int8_t      module_lines;                                                   
	uint8_t*    read_buf;                                                       
	int32_t     read_count;                                                     
	uint32_t        flash_addr;  
	uint8_t*    write_buf;                                                       
	int32_t     write_count;  	
}spi_protl_set,*spi_protl_set_p;  

#define MFR_MACRONIX 0x00c2
#define MFR_WINBOND 0x00ef
#define MFR_GIGADEVICE 0x00c8
#define JEDEC_MFR(jedec) ((jedec) >> 16)

#define MX25L25635E 0xc22019
#define MX25L12835F 0xc22018
#define W25Q128 0xef4018
#define W25Q64 0xef4017

#define MX25L256_SIZE (256*1024*1024)
#define W25Q128_SIZE (128*1024*1024)
#define MX25L128_SIZE (128*1024*1024)

typedef enum flash_dev {
	FLASH_MX25L256,
	FLASH_W25Q128,
} flash_dev_t;


typedef struct spi_manager_{                                                    
	uint8_t     g_wires;                                                            
	enum	flash_types{
		SPI_NOR_FLASH,
		SPI_NAND_FLASH,
	}flash_type;

	flash_dev_t flash_device;
	uint32_t		flash_sector_size;
	/*                                                                          
	 *(1)0xa,for q3,by arm.                                                     
	 *(2)0xb,for q3e,designed by xiaoran.cai                                    
	 */                                                                         
	enum    read_mode_def{                                                      
		SPI_POLLING = 100,                                                      
		SPI_INTR,                                                               
		SPI_DMA,                                                                
		SPI_MAX,
	}rw_mode;                                                                 
	uint8_t     inited;                                                             

}spi_manager_glob,*spi_manager_glob_p;

typedef struct flash_dev_info {
	uint32_t jedec;
	flash_dev_t dev_id;
	int32_t (*flash_qe_enable_func)(void);
	int32_t (*flash_qe_disable_func)(void);
	int	    (*flash_wait_device_status_func)(int cmd,int status);

}flash_dev_info_t;


# if 0
extern int otf_set_intr_recv_overrun(int set_clear) ;
extern int otf_set_intr_recv_fifo(int set_clear) ;
extern int otf_set_intr_transmit_fifo(int set_clear) ;
extern int otf_set_intr_transmit_timeout(int set_clear);
extern int otf_get_raw_intr_recv_overrun(void) ;
extern int otf_get_raw_intr_recv_fifo(void);
extern int otf_get_raw_intr_trans_fifo(void);
extern int otf_get_raw_intr_trans_timeout(void);
extern int otf_get_masked_intr_recv_overun(void);
extern void otf_clear_intr_transmit_timeout(void)  ;
extern int32_t otf_status_is_busy(void);
extern int32_t otf_status_recv_fifo_full(void);
extern int32_t otf_status_transmit_fifo_empty(void);
extern int32_t otf_status_is_busy(void);
extern int32_t otf_status_recv_fifo_full(void);
extern int32_t otf_status_transmit_fifo_empty(void);
extern int32_t otf_status_is_busy(void);
extern int32_t otf_status_recv_fifo_full(void);
extern int32_t otf_status_transmit_fifo_empty(void);
extern int otf_get_masked_intr_recv_fifo(void) ;
extern int otf_get_masked_intr_trans_fifo(void) ;
extern int otf_get_masked_intr_trans_timeout(void);
extern void otf_clear_intr_recv_overun(void);
extern int32_t otf_status_recv_fifo_empty(void);
extern int32_t otf_status_transmit_fifo_full(void) ;
extern int32_t otf_config_tx_fifo_timeout_count(uint32_t count);
extern int32_t otf_config_data_register_func(int32_t reg_addr,int32_t  width,          
		           uint8_t wires,uint8_t tx_rx,uint8_t reg_en,int32_t data_count ) ;
extern void ssp_manager_set(spi_manager_glob_p p_mgr);
extern void ssp_manager_init(void);
extern void ssp_manager_set_wire(uint32_t wires);
extern void ssp_manager_set_rwmode(uint32_t rw_mode);
extern int32_t  flash_write_enable(void);
extern int flash_wait_device_status(int cmd,int status);
extern int32_t  flash_write_disable(void);
extern int32_t  flash_qe_enable(void);
extern int32_t  flash_qe_disable(void);
extern int32_t flash_read_status_reg2(void);

extern int32_t  spi_transfer_init(spi_protl_set_p  spi_trans_p);
extern int32_t spi_read_write_data(spi_protl_set_p  protocal_set);
extern void ssp_log_open(void);
extern bool nand_check_bad_block(int blk);
extern int nand_update_bbt(int blk);
extern int nand_erase_blk0(void);
extern int nand_display_oob(void);
extern int32_t spi_transfer_init(spi_protl_set_p  spi_trans_p);
#endif

#endif
