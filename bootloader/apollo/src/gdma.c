#include <common.h>
#include <gdma.h>
#include <malloc.h>
#include <div64.h>
#include <linux/types.h>
#include <vstorage.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <linux/byteorder/swab.h>
#include <efuse.h>
#include <asm-arm/arch-imapx800/imapx_base_reg.h>
#include <asm-arm/arch-imapx800/imapx_spiflash.h>
#include <asm-arm/arch-imapx800/imapx_pads.h>
#include <preloader.h>
#include <lowlevel_api.h>
#include <asm-arm/imapx_gdma.h>

//#define BYTE_NUM 8
#define INVALID_DATA 5
#define DESC_BUF_SIZE 0x100
#define ONCE_READ_NUM 65536
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define memcpy irf->memcpy
#define module_enable irf->module_enable
#define pads_chmod irf->pads_chmod
#define pads_pull irf->pads_pull
#define printf irf->printf
#define malloc irf->malloc

#define SPI_TFIFO_EMPTY  (1 << 0)
#define SPI_TFIFO_NFULL  (1 << 1)
#define SPI_RFIFO_NEMPTY (1 << 2)
#define SPI_BUSY         (1 << 4)

static volatile uint32_t __regs = GDMA_BASE_ADDR;
#define BYTES_FOR_SINGLE 8
#define ALNUM          8
#define SECURE_VAL 1
//#define DEBUG
#ifdef DEBUG
#define dma_debug(x...) printf("gdma: " x)
#else
#define dma_debug(x...)
#endif
#define dma_error(x...) printf("gdma: " x)

#define barrier() __asm__ __volatile__("": : :"memory")
#define cpu_relax()			barrier()
#define DBG_BUSY		(1 << 0)
#define true 1
#define false 0

#define readl(a)		(*(volatile unsigned int *)(a))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

enum pl330_reqtype {
	MEMTODEV,
	DEVTOMEM,
	
};

struct _xfer_spec {
	u32 ccr;
	int size;
	int rqtype;
	
};

struct gdma_reqcfg req_cfg_tx;
struct gdma_reqcfg req_cfg_rx;

uint8_t *desc_addr_tx = NULL;
uint8_t *desc_addr_rx = NULL;

static unsigned int _emit_MOV(uint32_t dry_run, uint8_t buf[],
        enum dmamov_dst dst, uint32_t val)                      
{
	if (dry_run)
		return SZ_DMAMOV;

	buf[0] = CMD_DMAMOV;
	buf[1] = dst;
	buf[2] = (val>>0) & 0xff;                                       
	buf[3] = (val>>8) & 0xff;                                       
	buf[4] = (val>>16) & 0xff;                                      
	buf[5] = (val>>24) & 0xff;     
	//*((u32 *)&buf[2]) = val;
	
    dma_debug("\tDMAMOV %s 0x%x\n",              
            dst == SAR ? "SAR" : (dst == DAR ? "DAR" : "CCR"), (uint32_t)val);    

    return SZ_DMAMOV;                                               
}

static uint32_t _emit_FLUSHP(unsigned dry_run, u8 buf[], u8 peri)
{
	if (dry_run)
		return SZ_DMAFLUSHP;

	buf[0] = CMD_DMAFLUSHP;

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	dma_debug("\tDMAFLUSHP %u\n", peri >> 3);

	return SZ_DMAFLUSHP;
}


static uint32_t _emit_LD(uint32_t dry_run, uint8_t buf[], enum gdma_cond cond)
{
    if (dry_run)
        return SZ_DMALD;

    buf[0] = CMD_DMALD;

    if (cond == SINGLE)
        buf[0] |= (0 << 1) | (1 << 0);
    else if (cond == BURST)
        buf[0] |= (1 << 1) | (1 << 0);

    dma_debug("\tDMALD%c\n",
            cond == SINGLE ? 'S' : (cond == BURST ? 'B' : 'A'));

    return SZ_DMALD;
}

static uint32_t _emit_LP(uint32_t dry_run, uint8_t buf[],
        uint32_t loop, uint8_t cnt)                                 
{       
	if (dry_run)
		return SZ_DMALP;

	buf[0] = CMD_DMALP;

	if (loop)
		buf[0] |= (1 << 1);

	cnt--; /* DMAC increments by 1 internally */
	buf[1] = cnt;

    dma_debug("\tDMALP_%c %u\n", loop ? '1' : '0', cnt);

    return SZ_DMALP;
}

static uint32_t _emit_RMB(uint32_t dry_run, uint8_t buf[])
{           
    if (dry_run)
        return SZ_DMARMB;

    buf[0] = CMD_DMARMB;

    dma_debug("\tDMARMB\n");

    return SZ_DMARMB;
}

static uint32_t _emit_ST(uint32_t dry_run, uint8_t buf[], enum gdma_cond cond)
{       
    if (dry_run)
        return SZ_DMAST;

    buf[0] = CMD_DMAST; 

    if (cond == SINGLE)
        buf[0] |= (0 << 1) | (1 << 0);
    else if (cond == BURST)
        buf[0] |= (1 << 1) | (1 << 0);

    dma_debug("\tDMAST%c\n",
            cond == SINGLE ? 'S' : (cond == BURST ? 'B' : 'A'));

    return SZ_DMAST;
}

static uint32_t _emit_WMB(uint32_t dry_run, uint8_t buf[])
{       
    if (dry_run)
        return SZ_DMAWMB;

    buf[0] = CMD_DMAWMB;

    dma_debug("\tDMAWMB\n");

    return SZ_DMAWMB;
}

static uint32_t _emit_WFP(uint32_t dry_run, uint8_t buf[], enum gdma_cond cond, int peri)
{
	if (dry_run)
		return SZ_DMAWFP;

	buf[0] = CMD_DMAWFP;

	if (cond == SINGLE)
		buf[0] |= (0 << 1) | (0 << 0);
	else if (cond == BURST)
		buf[0] |= (1 << 1) | (0 << 0);
	else
		buf[0] |= (0 << 1) | (1 << 0);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	dma_debug("\tDMAWFP%c %u\n",
		cond == SINGLE ? 'S' : (cond == BURST ? 'B' : 'P'), peri >> 3);

	return SZ_DMAWFP;
}

static uint32_t _emit_LDP(uint32_t dry_run, uint8_t buf[] ,enum gdma_cond cond, int peri)
{
	if (dry_run)
		return SZ_DMALDP;

	buf[0] = CMD_DMALDP;

	if (cond == BURST)
		buf[0] |= (1 << 1);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	dma_debug("\tDMALDP%c %u\n",
		cond == SINGLE ? 'S' : 'B', peri >> 3);

	return SZ_DMALDP;
}

static uint32_t _emit_STP(uint32_t dry_run, uint8_t buf[], enum gdma_cond cond, int peri)
{
	if (dry_run)
		return SZ_DMASTP;

	buf[0] = CMD_DMASTP;

	if (cond == BURST)
		buf[0] |= (1 << 1);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	dma_debug("\tDMASTP%c %u\n",
		cond == SINGLE ? 'S' : 'B', peri >> 3);

	return SZ_DMASTP;
}


static uint32_t _emit_LPEND(uint32_t dry_run, uint8_t buf[],
        const struct _arg_LPEND *arg)                                 
{                                                                     
    enum gdma_cond cond = arg->cond;                                 
    uint32_t forever = arg->forever;                                  
    uint32_t loop = arg->loop;                                        
    uint8_t bjump = arg->bjump;                                 

    if (dry_run)                                                      
        return SZ_DMALPEND;                                           

    buf[0] = CMD_DMALPEND;                                            

    if (loop)                                                         
        buf[0] |= (1 << 2);                                           

    if (!forever)                                                     
        buf[0] |= (1 << 4);                                           

    if (cond == SINGLE)                                               
        buf[0] |= (0 << 1) | (1 << 0);                                
    else if (cond == BURST)                                           
        buf[0] |= (1 << 1) | (1 << 0);                                

    buf[1] = bjump;                                                   

    dma_debug("\tDMALP%s%c_%c bjmpto_%x\n",      
            forever ? "FE" : "END",                                   
            cond == SINGLE ? 'S' : (cond == BURST ? 'B' : 'A'),       
            loop ? '1' : '0',                                         
            bjump);                                                   

    return SZ_DMALPEND;                                               
}                                                                     

static unsigned int _emit_SEV(unsigned dry_run, unsigned char buf[], unsigned char ev)
{       
    if (dry_run)
        return SZ_DMASEV;                                                             

    buf[0] = CMD_DMASEV;

    ev &= 0x1f;
    ev <<= 3;                                                                         
    buf[1] = ev;

    dma_debug("\tDMASEV %u\n", ev >> 3);                           

    return SZ_DMASEV;
}

static unsigned int _emit_END(unsigned dry_run, unsigned char buf[])
{
    if (dry_run)
        return SZ_DMAEND;

    buf[0] = CMD_DMAEND;

    dma_debug("\tDMAEND\n");

    return SZ_DMAEND;
}

static uint32_t _emit_GO(uint32_t dry_run, uint8_t buf[],
        const struct _arg_GO *arg)                                 
{                                                                  
    uint8_t chan = arg->chan;                                
    uint32_t addr = arg->addr;                                 
    uint32_t ns = arg->ns;                                         

    if (dry_run)                                                   
        return SZ_DMAGO;                                           

    buf[0] = CMD_DMAGO;                                            
    buf[0] |= (ns << 1);                                           

    buf[1] = chan & 0x7;                                           

	*((u32 *)&buf[2]) = addr;
	
    return SZ_DMAGO;                                               
}                                                                  

static uint32_t _prepare_ccr(const struct gdma_reqcfg *rqc) 
{                                                                
    unsigned int ccr = 0;                                        

    if (rqc->src_inc)                                            
        ccr |= CCR_SRCINC;                                       

    if (rqc->dst_inc)                                            
        ccr |= CCR_DSTINC;                                       

    /*  We set same protection levels for Src and DST for now */  
    if (rqc->privileged)                                         
    {                                                            
        ccr |= CCR_SRCPRI;                                       
        ccr |= CCR_DSTPRI;                                       
    }                                                            
    if (rqc->nonsecure)                                          
        ccr |= CCR_SRCNS | CCR_DSTNS;                            
    if (rqc->insnaccess)                                         
        ccr |= CCR_SRCIA | CCR_DSTIA;                            

    ccr |= (((rqc->brst_len - 1) & 0xf) << CCR_SRCBRSTLEN_SHFT); 
    ccr |= (((rqc->brst_len - 1) & 0xf) << CCR_DSTBRSTLEN_SHFT); 

    ccr |= (rqc->brst_size << CCR_SRCBRSTSIZE_SHFT);             
    ccr |= (rqc->brst_size << CCR_DSTBRSTSIZE_SHFT);             

    ccr |= (rqc->dcctl << CCR_SRCCCTRL_SHFT);                    
    ccr |= (rqc->scctl << CCR_DSTCCTRL_SHFT);                    

    ccr |= (rqc->swap << CCR_SWAP_SHFT);                         

    return ccr;                                                  
}

static void set_secure_para(uint8_t secure_flag)
{
    uint8_t val;

    /*  GDMA module reset*/
    module_enable(GDMA_SYSM_ADDR);
    val = readl(GDMA_SYSM_ADDR + GDMA_SC);
    val |= (1<<0);
    writel(val, GDMA_SYSM_ADDR + GDMA_SC);
    if (secure_flag)
        __regs += 0x100000;
}

static void dma_req_configure(struct gdma_reqcfg* p_req_cfg, uint32_t port_num, uint32_t dst_inc, uint32_t src_inc, uint32_t nonsecure, uint32_t privileged,
        uint32_t insnaccess, uint32_t brst_len, uint32_t brst_size, enum dstcachectrl dcctl, enum srccachectrl scctl, enum byteswap swap)
{                                                                                                                                                       
    p_req_cfg->port_num = port_num;                                                                                                                       
    p_req_cfg->dst_inc = dst_inc;                                                                                                                         
    p_req_cfg->src_inc = src_inc;                                                                                                                         
    p_req_cfg->nonsecure = nonsecure;                                                                                                                     
    p_req_cfg->privileged = privileged;                                                                                                                   
    p_req_cfg->insnaccess = insnaccess;                                                                                                                   
    p_req_cfg->brst_len = brst_len;                                                                                                                       
    p_req_cfg->brst_size = brst_size;                                                                                                                     
    p_req_cfg->dcctl = dcctl;                                                                                                                             
    p_req_cfg->scctl = scctl;                                                                                                                             
    p_req_cfg->swap = swap;                                                                                                                               
}

static int _until_dmac_idle()
{
	unsigned long loops = 1155000;  // 5s
	
	do {
		/* Until Manager is Idle */
		if (!(readl(__regs + GDMA_DS) & DBG_BUSY))
			break;

		cpu_relax();
	} while (--loops);

	if (!loops)
		return true;

	return false;
}

static void dmac_start(uint32_t chan, uint32_t insn_addr)
{                                                                
    struct _arg_GO go;                                           
    uint32_t val;                                            
    uint8_t insn[6] = {0, 0, 0, 0, 0, 0};                  

    go.chan = chan;                                              
    go.addr = insn_addr;                                         
    go.ns = 1;                                                   
    _emit_GO(0, insn, &go);     
	
	//printf("channel number:%d, mc_bus:%d, ns:%d\n", chan, insn_addr, go.ns);

    val = (insn[0] << 16) | (insn[1] << 24);                     
    writel(val, __regs + GDMA_DI0); //#define DBGINST0   0xd08    
  	// val = (insn[2]<<0) |(insn[3]<<8)|(insn[4]<<16)|(insn[5]<<24);
    val = *((u32 *)&insn[2]);
    writel(val, __regs + GDMA_DI1); //#define DBGINST1   0xd0c    

	/* If timed out due to halted state-machine */
	if (_until_dmac_idle()) {
		printf("DMAC halted!\n");
		return;
	}
    /*  Get going */                                              
    writel(0, __regs + GDMA_DC); //#define DBGCMD        0xd04    
}

static void gdma_sample_complete(void)
{
    unsigned int val;

    val = readl(__regs + GDMA_EIRS);
    while (val != 0x3) {
        val = readl(__regs + GDMA_FSDM) & 0x1;
        if (val) {
            dma_error("dmac manager fault: 0x%x\n", val);
        }
        val = readl(__regs + GDMA_FSDC);
        if(val) {
            dma_error("dmac channel fault: 0x%x, 0x%x\n", val, readl(__regs + 0x40));
			break;	
        }
        val = readl(__regs + GDMA_EIRS);
        if(val) {
            writel(val, __regs + GDMA_IC);
            break;
        }
    }
}

static inline int _ldst_devtomem(unsigned dry_run, u8 buf[],
		const struct _xfer_spec *pxs, int cyc)
{
	int off = 0;
	
	while (cyc--) {
		if (BRST_LEN(pxs->ccr) == 1) { 
			off += _emit_WFP(dry_run, &buf[off], SINGLE, pxs->rqtype);
			off += _emit_LDP(dry_run, &buf[off], SINGLE, pxs->rqtype);
			off += _emit_STP(dry_run, &buf[off], SINGLE, pxs->rqtype);//ALWAYS
			off += _emit_FLUSHP(dry_run, &buf[off], pxs->rqtype);
		} else {
			off += _emit_WFP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_LDP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_STP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_FLUSHP(dry_run, &buf[off], pxs->rqtype);    
		}
	}

	return off;
}

static inline int _ldst_memtodev(unsigned dry_run, u8 buf[],
		const struct _xfer_spec *pxs, int cyc)
{
	int off = 0;

	while (cyc--) {
		if (BRST_LEN(pxs->ccr) == 1) {
			off += _emit_WFP(dry_run, &buf[off], SINGLE, pxs->rqtype);
			off += _emit_LDP(dry_run, &buf[off], SINGLE, pxs->rqtype);
			off += _emit_STP(dry_run, &buf[off], SINGLE, pxs->rqtype);
			off += _emit_FLUSHP(dry_run, &buf[off], pxs->rqtype);
		} else {
			off += _emit_WFP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_LDP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_STP(dry_run, &buf[off], BURST, pxs->rqtype);
			off += _emit_FLUSHP(dry_run, &buf[off], pxs->rqtype);
		}
	}

	return off;
}

static int _is_valid(u32 ccr)
{
	enum srccachectrl dcctl;
	enum dstcachectrl scctl;

	dcctl = (ccr >> CCR_DSTCCTRL_SHFT) & CCR_DRCCCTRL_MASK;
	scctl = (ccr >> CCR_SRCCCTRL_SHFT) & CCR_SRCCCTRL_MASK;

	if (dcctl == DINVALID1 || dcctl == DINVALID2
			|| scctl == SINVALID1 || scctl == SINVALID2)
		return 0;
	else
		return 1;
}


static int _bursts(unsigned dry_run, u8 buf[],
		const struct _xfer_spec *pxs, int cyc)
{
	int off = 0;

	switch (pxs->rqtype) {
		case MEMTODEV:
			off += _ldst_memtodev(dry_run, &buf[off], pxs, cyc);
			break;
		case DEVTOMEM:
			off += _ldst_devtomem(dry_run, &buf[off], pxs, cyc);
			break;
		default:
			off += 0x40000000; /* Scare off the Client */
			break;
	}

	return off;
}

static inline int _loop(unsigned dry_run, u8 buf[],
		unsigned long *bursts, 
		const struct _xfer_spec *pxs)		
{
	int cyc, cycmax, szlp, szlpend, szbrst, off;
	unsigned lcnt0, lcnt1, ljmp0, ljmp1;
	struct _arg_LPEND lpend;

	/* Max iterations possibile in DMALP is 256 */
	if (*bursts >= 256*256) {
		lcnt1 = 256;
		lcnt0 = 256;
		cyc = *bursts / lcnt1 / lcnt0;
	} else if (*bursts > 256) {
		lcnt1 = 256;
		lcnt0 = *bursts / lcnt1;
		cyc = 1;
	} else {
		lcnt1 = *bursts;
		lcnt0 = 0;
		cyc = 1;
	}

	szlp = _emit_LP(1, buf, 0, 0);
	szbrst = _bursts(1, buf, pxs, 1);

	lpend.cond = ALWAYS;
	lpend.forever = 0;
	lpend.loop = 0;
	lpend.bjump = 0;
	szlpend = _emit_LPEND(1, buf, &lpend);

	if (lcnt0) {
		szlp *= 2;
		szlpend *= 2;
	}

	/*
	 * Max bursts that we can unroll due to limit on the
	 * size of backward jump that can be encoded in DMALPEND
	 * which is 8-bits and hence 255
	 */
	cycmax = (255 - (szlp + szlpend)) / szbrst;
	cyc = (cycmax < cyc) ? cycmax : cyc;

	off = 0;
	
	if (lcnt0) {
		off += _emit_LP(dry_run, &buf[off], 0, lcnt0);
		ljmp0 = off;
	}

	off += _emit_LP(dry_run, &buf[off], 1, lcnt1);
	ljmp1 = off;
	
	off += _bursts(dry_run, &buf[off], pxs, cyc);
	
	lpend.cond = ALWAYS;
	lpend.forever = 0;
	lpend.loop = 1;
	lpend.bjump = off - ljmp1;
	off += _emit_LPEND(dry_run, &buf[off], &lpend);

	if (lcnt0) {
		lpend.cond = ALWAYS;
		lpend.forever = 0;
		lpend.loop = 0;
		lpend.bjump = off - ljmp0;
		off += _emit_LPEND(dry_run, &buf[off], &lpend);
	}

	*bursts = lcnt1 * cyc;
	if (lcnt0)
		*bursts *= lcnt0;

	return off;
}

static inline int _setup_loops(unsigned dry_run, u8 buf[],
		struct _xfer_spec *pxs)
{
	unsigned long c, bursts, least;		// BYTE_TO_BURST(x->bytes, ccr);
	int off = 0;
	bursts = BYTE_TO_BURST(pxs->size, pxs->ccr);

	while (bursts) {
		c = bursts;
		off += _loop(dry_run, &buf[off], &c, pxs);		
		bursts -= c;
	}

	bursts = BYTE_TO_BURST(pxs->size, pxs->ccr);
	least = pxs->size - BURST_TO_BYTE(bursts, pxs->ccr);

	if (least) {
		pxs->ccr &= ~(0xf << CCR_SRCBRSTLEN_SHFT);
		pxs->ccr &= ~(0xf << CCR_DSTBRSTLEN_SHFT);
		off += _emit_MOV(dry_run, &buf[off], CCR, pxs->ccr);
		while (least) {
			c = least;
			off += _loop(dry_run, &buf[off], &c, pxs);
			least -= c;
		}
	}
	return off;
}

static void clear_desc_buf(uint8_t *desc_addr)
{	
	int i = DESC_BUF_SIZE;
	uint8_t* buf = desc_addr;    
	
	while(i--)
		*(buf + i) = 0x0;
}

static void gdma_transfer(struct gdma_addr *src, struct gdma_addr *dst, uint32_t len, 
        struct gdma_reqcfg* p_req_cfg, uint32_t ev_num, uint8_t *desc_addr, int peri)                                                                  
{                                                                                                                                                                 
    uint8_t* buf = desc_addr;    
	struct _xfer_spec xs;	

	int dry_run, off, ccr;

	if (src->modbyte)
	  len--;
	if (dst->modbyte)
	  len--;

	dry_run = 0;
	off = 0;

    writel(1 << ev_num, __regs + GDMA_IE);  //enable event interrupt       
	writel(0xffff, __regs + GDMA_IC);

	ccr = _prepare_ccr(p_req_cfg);     

	if (!_is_valid(ccr)) {
		return ;
	}
	
	off+=_emit_FLUSHP(0,&buf[off],peri);
	off+=_emit_MOV(0,&buf[off],CCR,ccr);	

    off+=_emit_MOV(0,&buf[off],SAR,src->byte);                                                                                                           
    off+=_emit_MOV(0,&buf[off],DAR,dst->byte);        
	
    if (src->modbyte) {
        off += _emit_LD(0, &buf[off], ALWAYS);
		off += _emit_RMB(0, &buf[off]);
        len -= 1;
    }

	dma_debug("len:%d, brst_size:%d\n", len, p_req_cfg->brst_size);
	xs.size = len;
	xs.rqtype = peri;
	xs.ccr = ccr;

	off += _setup_loops(dry_run, &buf[off], &xs);

	off += _emit_SEV(dry_run, &buf[off], ev_num);
	off += _emit_END(dry_run, &buf[off]);

    dmac_start(p_req_cfg->port_num, (uint32_t)buf);
    gdma_sample_complete();
}

static void dma_sync_transfer(struct gdma_addr *src, struct gdma_addr *dst, uint32_t len, 
        struct gdma_reqcfg* p_req_cfg_tx, uint32_t tx_ev_num, uint8_t *tx_desc_addr, int tx_peri, 
        struct gdma_reqcfg* p_req_cfg_rx, uint32_t rx_ev_num, uint8_t *rx_desc_addr, int rx_peri)                                                                  
{
	int dry_run, tx_off, rx_off, tx_ccr, rx_ccr;
	uint8_t* tx_buf = tx_desc_addr;  
	uint8_t* rx_buf = rx_desc_addr;
	
	struct _xfer_spec tx_xs;	
	struct _xfer_spec rx_xs;

	dry_run = 0;
	tx_off = 0;
	rx_off = 0;

	writel(1 << tx_ev_num | 1 << rx_ev_num, __regs + GDMA_IE);  //enable event interrupt       
	writel(0xffff, __regs + GDMA_IC);

	tx_ccr = _prepare_ccr(p_req_cfg_tx);     
	rx_ccr = _prepare_ccr(p_req_cfg_rx);

	if (!_is_valid(tx_ccr)) {
		return ;
	}

	if (!_is_valid(rx_ccr)) {
		return ;
	}

	/* init tx channel */
	tx_off+=_emit_FLUSHP(0,&tx_buf[tx_off],tx_peri);
	tx_off+=_emit_MOV(0,&tx_buf[tx_off],CCR,tx_ccr);	

	tx_off+=_emit_MOV(0,&tx_buf[tx_off],SAR,dst->byte + 0x200000);                                                                                                           
	tx_off+=_emit_MOV(0,&tx_buf[tx_off],DAR,src->byte);        

	tx_xs.size = len;
	tx_xs.rqtype = tx_peri;
	tx_xs.ccr = tx_ccr;

	tx_off += _setup_loops(dry_run, &tx_buf[tx_off], &tx_xs);

	tx_off += _emit_SEV(dry_run, &tx_buf[tx_off], tx_ev_num);
	tx_off += _emit_END(dry_run, &tx_buf[tx_off]);

	/* init rx channel */
	rx_off+=_emit_FLUSHP(0,&rx_buf[rx_off],rx_peri);
	rx_off+=_emit_MOV(0,&rx_buf[rx_off],CCR,rx_ccr);	

	rx_off+=_emit_MOV(0,&rx_buf[rx_off],SAR,src->byte);                                                                                                           
	rx_off+=_emit_MOV(0,&rx_buf[rx_off],DAR,dst->byte);        

	rx_xs.size = len;
	rx_xs.rqtype = rx_peri;
	rx_xs.ccr = rx_ccr;

	rx_off += _setup_loops(dry_run, &rx_buf[rx_off], &rx_xs);

	rx_off += _emit_SEV(dry_run, &rx_buf[rx_off], rx_ev_num);
	rx_off += _emit_END(dry_run, &rx_buf[rx_off]);
	
	dmac_start(p_req_cfg_tx->port_num, (uint32_t)tx_buf); //start tx channel
	dmac_start(p_req_cfg_rx->port_num, (uint32_t)rx_buf); //start rx channel
	
	gdma_sample_complete(); //wait tx and rx channel transfer complete
}

void dma_peri_flush_fifo(struct gdma_addr *src_addr, struct gdma_addr *dst_addr)
{
	struct gdma_addr temp;

	temp.byte = dst_addr->byte + 0x200000;
	temp.modbyte = 0;

	req_cfg_tx.brst_len = 0;
	req_cfg_rx.brst_len = 0;

	while(readl(SSP_SR_0) & SPI_BUSY);
    while(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
        gdma_transfer(src_addr, &temp, ONCE_READ_NUM, &req_cfg_rx, 0, desc_addr_rx, 1);
    } 
}

void dma_spi_send_cmd(struct gdma_addr *src_addr, struct gdma_addr *dst_addr, 
			uint8_t *ex, uint8_t ex_len, uint32_t *data_len)
{
	int i;
	struct gdma_addr temp;

	temp.byte = &ex[0];
	temp.modbyte = 0;
	
	req_cfg_tx.brst_len = 0;
	req_cfg_rx.brst_len = 0;
	for(i=0; i<ex_len; i++){
		gdma_transfer(&temp, src_addr, 1, &req_cfg_tx, 1, desc_addr_tx, 0);	
		temp.byte += 1;
	}
	
	temp.byte = dst_addr->byte + 0x200000;
	temp.modbyte = 0;

	req_cfg_rx.brst_len = 4;
	gdma_transfer(src_addr, &temp, INVALID_DATA, &req_cfg_rx, 0, desc_addr_rx, 1);

	req_cfg_rx.brst_len = 4;
	gdma_transfer(src_addr, dst_addr, (ex_len - INVALID_DATA), &req_cfg_rx, 0, desc_addr_rx, 1);
	dst_addr->byte += (ex_len - INVALID_DATA);
	*data_len -= (ex_len - INVALID_DATA);
}

int dma_peri_transfer(struct gdma_addr *src_addr, struct gdma_addr *dst_addr, uint32_t data_len)
{
	uint32_t i, j; 	
	unsigned long  len = 0;

	if(data_len > ONCE_READ_NUM)	{ 		
	
		i = data_len / ONCE_READ_NUM;
		j = data_len % ONCE_READ_NUM;
		req_cfg_tx.brst_len = 4;
		req_cfg_rx.brst_len = 4;
		
		while(i--){
			dma_sync_transfer(src_addr, dst_addr, ONCE_READ_NUM, &req_cfg_tx, 1, desc_addr_tx, 0, &req_cfg_rx, 0, desc_addr_rx, 1);
			dst_addr->byte += ONCE_READ_NUM;
			len += ONCE_READ_NUM;
		}
		
		dma_sync_transfer(src_addr, dst_addr, j, &req_cfg_tx, 1, desc_addr_tx, 0, &req_cfg_rx, 0, desc_addr_rx, 1);
		dst_addr->byte += j;
		len += j;
	}else{
		req_cfg_tx.brst_len = 4;
		req_cfg_rx.brst_len = 4;
		
		dma_sync_transfer(src_addr, dst_addr, data_len, &req_cfg_tx, 1, desc_addr_tx, 0, &req_cfg_rx, 0, desc_addr_rx, 1);
		dst_addr->byte += data_len;
		len += data_len;
	}
	
	return len;
}

int init_gdma(void)
{
    desc_addr_tx = malloc(DESC_BUF_SIZE);
    if (desc_addr_tx == NULL) {
        dma_error("gdma cannot malloc\n");
        return -1;
    }
	
	desc_addr_rx = malloc(DESC_BUF_SIZE);
		if (desc_addr_rx == NULL) {
			dma_error("gdma cannot malloc\n");
			return -1;
	}
		
	clear_desc_buf(desc_addr_tx);
	clear_desc_buf(desc_addr_rx);

    set_secure_para(0);

    dma_req_configure(&req_cfg_tx, IMAPX_SSP0_TX, 0, 1, SECURE_VAL, 0, 0, 4, 0, SCCTRL0, DCCTRL0, SWAP_NO);
    dma_req_configure(&req_cfg_rx, IMAPX_SSP0_RX, 1, 0, SECURE_VAL, 0, 0, 4, 0, SCCTRL0, DCCTRL0, SWAP_NO);
	
    return 0;
}
