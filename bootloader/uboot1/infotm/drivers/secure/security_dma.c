#include <common.h>
#include <security_dma.h>

/*
 * dma start
 */
void dma_start(uint32_t dma)
{   
    rSEC_DMA_START(dma) = 1 << DMA_START_BIT;
}

/*
 * dma reset
 */
uint32_t dma_reset(uint32_t dma)
{
    uint32_t ret = 0;
	uint64_t t = 0;
    
    rSEC_DMA_START(dma) = 1 << DMA_RESET_BIT;
    
    t = get_ticks();
    ret = rSEC_DMA_START(dma);
    while(ret)
    {
    	ret = rSEC_DMA_START(dma);
		if(get_ticks() > t + 2000)
		   return 1;
    }

	return 0;
}

/*
 * dma get status value
 * 
 * bit : the bit of the status
 */
uint32_t dma_get_status(uint32_t dma, uint32_t bit)
{
    uint32_t readdata = 0;
    
    readdata = rSEC_DMA_STATUS(dma);
    if(readdata & (1 << bit)){
    	rSEC_DMA_STATUS(dma) = 1 << bit;
    	return 1;
    }
    else
    	return 0;
}

/*
 * dma sdma init
 * 
 * here , len should be less than 16bit
 */
uint32_t dma_sdma_init(uint32_t dma, uint32_t add, uint32_t len)
{
    uint32_t ret = 0;

	ret = rSEC_DMA_ADMA_CFG(dma);
    ret |= (SDMA_MODE << ADMA_MODE) | (ENABLE << ADMA_ENABLE);
    rSEC_DMA_ADMA_CFG(dma) = ret;

    rSEC_DMA_SDMA_CTRL0(dma) = 0x04000001;

	if(len >= 0x10000)
	    return 1;
    // set length
    ret = 0;
    ret |= len & 0xffff;
    rSEC_DMA_SDMA_CTRL1(dma) = ret;

    // set addr
    rSEC_DMA_SDMA_ADDR0(dma) = add;
    
    return 0;
}

/*
 * security cke control
 *
 * module_cke : SEC_BLCK_CKE , SEC_HASH_CKE, SEC_PKA_CKE
 * mode       : ENABLE or DISABLE
 */
void security_cke_control(uint32_t module_cke, uint32_t mode)
{
    uint32_t ret = 0;

	ret = rSEC_CG_CFG;
	ret &= ~(1 << module_cke);
	ret |= mode << module_cke;
	rSEC_CG_CFG = ret;
}

