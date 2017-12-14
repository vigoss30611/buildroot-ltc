#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <gdma.h>
#include <malloc.h>
#include <div64.h>

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

uint64_t ct = 0;
uint64_t ct1 = 0;
#if 0
static unsigned int _emit_MOV(uint32_t dry_run, uint8_t buf[],
        enum dmamov_dst dst, uint32_t val)                      
{
    if (dry_run) {                                             
		buf[0] = CMD_DMAMOV;                                            
		buf[1] = dst; 
	} else {
		buf[2] = (val>>0) & 0xff;                                       
		buf[3] = (val>>8) & 0xff;                                       
		buf[4] = (val>>16) & 0xff;                                      
		buf[5] = (val>>24) & 0xff;      
	}
    
    dma_debug("\tDMAMOV %s 0x%x\n",              
            dst == SAR ? "SAR" : (dst == DAR ? "DAR" : "CCR"), (uint32_t)val);    

    return SZ_DMAMOV;                                               
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
    if (dry_run) {
	buf[0] = CMD_DMALP;
	if (loop)
	    buf[0] |= (1 << 1); 
    } else {	
	cnt--; /*  DMAC increments by 1 internally */                          
	buf[1] = cnt;
    }

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
#endif
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

    buf[2] = (addr>>0) & 0xff;                                     
    buf[3] = (addr>>8) & 0xff;                                     
    buf[4] = (addr>>16) & 0xff;                                    
    buf[5] = (addr>>24) & 0xff;                                    
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

void set_secure_para(uint8_t secure_flag)
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

void dma_req_configure(struct gdma_reqcfg* req_cfg, uint32_t port_num, uint32_t dst_inc, uint32_t src_inc, uint32_t nonsecure, uint32_t privileged,
        uint32_t insnaccess, uint32_t brst_len, uint32_t brst_size, enum dstcachectrl dcctl, enum srccachectrl scctl, enum byteswap swap)
{                                                                                                                                                       
    req_cfg->port_num = port_num;                                                                                                                       
    req_cfg->dst_inc = dst_inc;                                                                                                                         
    req_cfg->src_inc = src_inc;                                                                                                                         
    req_cfg->nonsecure = nonsecure;                                                                                                                     
    req_cfg->privileged = privileged;                                                                                                                   
    req_cfg->insnaccess = insnaccess;                                                                                                                   
    req_cfg->brst_len = brst_len;                                                                                                                       
    req_cfg->brst_size = brst_size;                                                                                                                     
    req_cfg->dcctl = dcctl;                                                                                                                             
    req_cfg->scctl = scctl;                                                                                                                             
    req_cfg->swap = swap;                                                                                                                               
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

    val = (insn[0] << 16) | (insn[1] << 24);                     
    writel(val, __regs+GDMA_DI0); //#define DBGINST0   0xd08    
    val = (insn[2]<<0) |(insn[3]<<8)|(insn[4]<<16)|(insn[5]<<24);
    writel(val, __regs+GDMA_DI1); //#define DBGINST1   0xd0c    

    /*  Get going */                                              
    writel(0, __regs+GDMA_DC); //#define DBGCMD        0xd04    
}

void dma_clear_result(char *data_addr, unsigned int data_len)
{
    unsigned int tmp;
    unsigned char* pSrc;

    tmp = data_len;
    pSrc = (unsigned char *)data_addr;
    while(tmp)
    {
        *pSrc++= 0;
        tmp--;
    }
}

void gdma_sample_complete(void)
{
    unsigned int val;
    
    val = readl(__regs + GDMA_EIRS);
    while (!val) {
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
#if 0
void gdma_file_descriptor(uint8_t *desc_addr)
{
	uint32_t off = 0, ljmp1 = 0, lcnt1 = 1;
	uint8_t* buf = desc_addr;
	struct _arg_LPEND lpend;

	off+=_emit_MOV(1,&buf[off],SAR,0);
	off+=_emit_MOV(1,&buf[off],DAR,0);
	off+=_emit_MOV(1,&buf[off],CCR,0);

	off += _emit_LP(1, &buf[off], 1, lcnt1); // loop_1 begin
	ljmp1 = off;
	off += _emit_LD(0, &buf[off], ALWAYS);
	lpend.cond = ALWAYS;
	lpend.forever = 0;
	lpend.loop = 1;
	lpend.bjump = off - ljmp1;
	off += _emit_LPEND(0, &buf[off], &lpend); // loop_1 end
	off += _emit_RMB(0, &buf[off]);
	off += _emit_LP(1, &buf[off], 1, lcnt1);
	ljmp1 = off;
	off += _emit_ST(0, &buf[off], ALWAYS);
	lpend.cond = ALWAYS;
	lpend.forever = 0;
	lpend.loop = 1;
	lpend.bjump = off - ljmp1;
	off += _emit_LPEND(0, &buf[off], &lpend); // loop_1 end
	off += _emit_WMB(0, &buf[off]);

	off += _emit_SEV(0, &buf[off], 0);
	off += _emit_END(0, &buf[off]);
}

void gdma_transfer(struct gdma_addr *src, struct gdma_addr *dst, uint32_t data_len,
		struct gdma_reqcfg* req_cfg, uint32_t ev_num, uint8_t *desc_addr)
{
	uint32_t ccr, off = 0;
	uint8_t* buf = desc_addr;
	uint64_t v1;
	v1 = get_ticks();
	writel(1 << ev_num, __regs + GDMA_IE);  //enable event interrupt
	writel(0xffff, __regs + GDMA_IC);
	off+=_emit_MOV(0,&buf[off],SAR,src->byte);
	off+=_emit_MOV(0,&buf[off],DAR,dst->byte);
	ccr = _prepare_ccr(req_cfg);
	off+=_emit_MOV(0,&buf[off],CCR,ccr);
	off += _emit_LP(0, &buf[off], 1, data_len);
	off += 4;
	off += _emit_LP(0, &buf[off], 1, data_len);
	ct1 += get_ticks()- v1;
	v1 = get_ticks();
	dmac_start(req_cfg->port_num, (uint32_t)buf);
	gdma_sample_complete();
	ct += get_ticks() - v1;
}
#endif
#if 0
void dma_fpga_event_desc_test(struct gdma_addr *src, struct gdma_addr *dst, uint32_t data_len, 
        struct gdma_reqcfg* req_cfg, uint32_t ev_num, uint8_t *desc_addr)                                                                  
{                                                                                                                                                                
    uint32_t off = 0, tmp, ccr, ljmp0 = 0, ljmp1, lcnt0, lcnt1;      
    uint8_t* buf = desc_addr;    
    struct _arg_LPEND lpend;    
    int len;
	uint64_t v1;
    len = (!req_cfg->brst_size)? data_len : BYTES_FOR_SINGLE;
	if (src->modbyte)
	  len--;
	if (dst->modbyte)
	  len--;
#if 1
    writel(1 << ev_num, __regs + GDMA_IE);  //enable event interrupt       
	writel(0xffff, __regs + GDMA_IC);
#endif
    off+=_emit_MOV(0,&buf[off],SAR,src->byte);                                                                                                           
    off+=_emit_MOV(0,&buf[off],DAR,dst->byte);                                                                                                           
    ccr = _prepare_ccr(req_cfg);      
    off+=_emit_MOV(0,&buf[off],CCR,ccr);                                                                                                                         

    tmp = BYTE_TO_BURST(data_len, ccr);      
	dma_debug("data_len is %d, tmp is %d\n", data_len, tmp);
    if (src->modbyte) {
        off += _emit_LD(0, &buf[off], ALWAYS);
		off += _emit_RMB(0, &buf[off]);
        tmp -= 1;
    }
	dma_debug("len is %d\n", len);
    do {
		if (tmp == 0)
		  break;
        if(tmp >= 256*8) {                                                                                                                                     
            lcnt1 = 8;                                                                                                                                         
            lcnt0 = 256;                                                                                                                                         
        } else if(tmp > 8) { 
			if (!req_cfg->brst_size)
			    lcnt1 = len;
			else
				lcnt1 = 8;   
            lcnt0 = tmp/lcnt1;      
        } else {
            lcnt1 = tmp;                                             
            lcnt0 = 0;                                               
        }
		dma_debug("lcnt1 is %x, len is %x\n", lcnt1, len);
        if (lcnt1 > len)        
            lcnt1 = len;
        
        if(lcnt0) {                                                  
            off += _emit_LP(0, &buf[off], 0, lcnt0);  // loop_0 begin
            ljmp0 = off;                                             
        }
		dma_debug("lcnt1 is %x, len is %x\n", lcnt1, len);
        off += _emit_LP(0, &buf[off], 1, lcnt1); // loop_1 begin     
        ljmp1 = off;
        off += _emit_LD(0, &buf[off], ALWAYS);
        lpend.cond = ALWAYS;
        lpend.forever = 0;  
        lpend.loop = 1;           
        lpend.bjump = off - ljmp1;                             
        off += _emit_LPEND(0, &buf[off], &lpend); // loop_1 end
        off += _emit_RMB(0, &buf[off]);    

        off += _emit_LP(0, &buf[off], 1, lcnt1);
        ljmp1 = off;
        off += _emit_ST(0, &buf[off], ALWAYS);
        lpend.cond = ALWAYS;
        lpend.forever = 0;  
        lpend.loop = 1;        
        lpend.bjump = off - ljmp1;                
        off += _emit_LPEND(0, &buf[off], &lpend); // loop_1 end
        off += _emit_WMB(0, &buf[off]);                              

        if(lcnt0) {                                                  
            lpend.cond = ALWAYS;                                     
            lpend.forever = 0;                                       
            lpend.loop = 0;                                          
            lpend.bjump = off - ljmp0;                               
            off += _emit_LPEND(0, &buf[off], &lpend);  // loop_0 end 
        }                                                            
        if(lcnt0)                                                    
            tmp -= lcnt0*lcnt1;                                      
        else                                                         
            tmp -= lcnt1;     
    }while(tmp);
    if (src->modbyte) {
        ccr &= ~(0x7f << CCR_SRCBRSTSIZE_SHFT);
		ccr |= (src->modbyte - 1)<< CCR_SRCBRSTLEN_SHFT;
		off+=_emit_MOV(0,&buf[off],CCR,ccr);
        off += _emit_LD(0, &buf[off], ALWAYS);
        off += _emit_RMB(0, &buf[off]);
        off += _emit_ST(0, &buf[off], ALWAYS);
        off += _emit_WMB(0, &buf[off]);
    }
    if (dst->modbyte) {
        ccr &= ~(0x7f << CCR_DSTBRSTSIZE_SHFT);
		ccr |= (dst->modbyte - 1)<< CCR_DSTBRSTLEN_SHFT;
		off+=_emit_MOV(0,&buf[off],CCR,ccr);
        off += _emit_ST(0, &buf[off], ALWAYS);
        off += _emit_WMB(0, &buf[off]);
    }
    /*  DMASEV peripheral/event */                    
    off += _emit_SEV(0, &buf[off], ev_num);          
    off += _emit_END(0, &buf[off]);     
    if(off > 256) {
        dma_error("beyond the pre-malloc in init: %d\n", off);
    } else
		dma_debug("off is %x\n", off);    
	v1 = get_ticks();
    dmac_start(req_cfg->port_num, (uint32_t)buf);
    gdma_sample_complete();
	ct += get_ticks() - v1;
}
#endif
#if 1
#define SINGLE_TO_BURST 8
void gdma_transfer(uint32_t src, uint32_t dst, uint32_t data_len,
        struct gdma_reqcfg* req_cfg, uint32_t ev_num, uint8_t *desc_addr)
{
    struct gdma_desc *buf = (struct gdma_desc *)desc_addr;
    uint32_t ccr, tmp = 0, i;
    uint64_t v1;
    
    writel(1 << ev_num, __regs + GDMA_IE);
    writel(0xffff, __regs + GDMA_IC);
    
	buf->mov[0].addr = dst;
    ccr = _prepare_ccr(req_cfg);
    buf->mov[1].addr = ccr;
    buf->mov[2].addr = src;

    tmp = BYTE_TO_BURST(data_len, ccr);
#if 1
    if (!req_cfg->brst_size) {
	buf->lp[0].lpnum = 0;
        buf->lp[1].lpnum = tmp - 1;
        buf->lp1.lpnum = tmp - 1;
    } else {
        buf->lp[1].lpnum = SINGLE_TO_BURST - 1;
        buf->lp1.lpnum = SINGLE_TO_BURST - 1;
		if (tmp / SINGLE_TO_BURST)
			buf->lp[0].lpnum = tmp / SINGLE_TO_BURST - 1; 
		else
		    buf->lp[0].lpnum = 0;
    }
#endif
#if 1
    if (!req_cfg->brst_size || !(tmp % SINGLE_TO_BURST)) {
        buf->nop[0] = CMD_DMASEV;
        buf->nop[1] = 0;
        buf->nop[2] = CMD_DMAEND;
    } else {
        for (i = 0;i < 3;i++)
            buf->nop[i] = CMD_DMANOP;
        buf->lp1_0.lpnum = tmp % SINGLE_TO_BURST - 1;
        buf->lp1_1.lpnum = tmp % SINGLE_TO_BURST - 1;
    }
#endif
    //v1 = get_ticks();
    dmac_start(req_cfg->port_num, (uint32_t)buf);
    gdma_sample_complete();
    //ct += get_ticks() - v1;
}

void gdma_file_descriptor(uint8_t *desc_addr)
{
    struct gdma_desc *buf = (struct gdma_desc *)desc_addr;
    int i; 

    for (i = 0;i < 3;i++) {
        buf->mov[i].command = CMD_DMAMOV;
        buf->mov[i].attribute = i;
        buf->mov[i].nop[0] = CMD_DMANOP;
        buf->mov[i].nop[1] = CMD_DMANOP;
    }
    for (i = 0;i < 2;i++) {
        buf->lp[i].command = CMD_DMALP | (i << 1);
        buf->lp[i].lpnum = 0;
    }

    buf->emit_ld = CMD_DMALD;
    buf->lpend0.command = (CMD_DMALPEND
            | (1 << 2) | (1 << 4));
    buf->lpend0.lpjmp = SINGLE_LP;
    buf->emit_rmb = CMD_DMARMB;

    buf->lp1.command = CMD_DMALP | (1 << 1);
    buf->lp1.lpnum = 0;
    buf->emit_st = CMD_DMAST;
    buf->lpend1.command = (CMD_DMALPEND
            | (1 << 2) | (1 << 4));
    buf->lpend1.lpjmp = SINGLE_LP;
    buf->emit_wmb = CMD_DMAWMB;
    
    buf->lpend2.command = (CMD_DMALPEND | (1 << 4));
    buf->lpend2.lpjmp = DOUBLE_LP;

    /* second loop for < 1k */
#if 1
    for (i = 0;i < 3;i++)
        buf->nop[i] = CMD_DMANOP;

    buf->lp1_0.command = CMD_DMALP | (1 << 1);
    buf->lp1_0.lpnum = 0;
    buf->emit_ld1 = CMD_DMALD;
    buf->lpend1_0.command = (CMD_DMALPEND
            | (1 << 2) | (1 << 4));
    buf->lpend1_0.lpjmp = SINGLE_LP;
    buf->emit_rmb1 = CMD_DMARMB;

    buf->lp1_1.command = CMD_DMALP | (1 << 1);
    buf->lp1_1.lpnum = 0;
    buf->emit_st1 = CMD_DMAST;
    buf->lpend1_1.command = (CMD_DMALPEND
            | (1 << 2) | (1 << 4));
    buf->lpend1_1.lpjmp = SINGLE_LP;
    buf->emit_wmb1 = CMD_DMAWMB;
#endif
    buf->sev.command = CMD_DMASEV;
    buf->sev.channum = 0;
    buf->emit_end = CMD_DMAEND;
}   
#endif
/* onebyte transfer */
void gdma_onebyte_memcpy(uint32_t src, uint32_t dst, uint32_t data_len,
        struct gdma_reqcfg *req_cfg, uint32_t ev_num, uint8_t *desc_addr)
{
    req_cfg->brst_len = 1;
    req_cfg->brst_size = 0;
    gdma_transfer(src, dst, data_len, req_cfg, 0, desc_addr);
}

void gdma_per256K_memcpy(uint32_t src, uint32_t dst, uint32_t data_len,
        struct gdma_reqcfg *req_cfg, uint32_t ev_num, uint8_t *desc_addr)
{
    int per = data_len / (1024 * 256); 
    int mod = data_len % (1024 * 256);

    while (per)
    {
	gdma_transfer(src, dst, 256 * 1024, req_cfg, 0, desc_addr);
	src += 256 * 1024;
	dst += 256 * 1024;
	per--;
    }
    if (mod) {
	gdma_transfer(src, dst, mod, req_cfg, 0, desc_addr);
    }
}

struct gdma_reqcfg req_cfg;
uint8_t *desc_addr = NULL;
int gdma_memcpy (uint8_t *src_addr, uint8_t *dst_addr, uint32_t data_len)
{  
	uint32_t src, dst; 
	int mod = data_len % 128;

	src = (uint32_t)src_addr;
	dst = (uint32_t)dst_addr;
	if (data_len < 128) {
		//gdma_onebyte_memcpy(src, dst, data_len, &req_cfg, 0, desc_addr);
		memcpy(src_addr, dst_addr, data_len);
		return 0;
	}
	if (src % 8 || dst % 8) {
		printf("gdma src is 0x%x, dst is 0x%x\n", src, dst);
		memcpy(src_addr, dst_addr, data_len);
		return 0;
	}

	req_cfg.brst_len = 16;
	req_cfg.brst_size = 3;
	gdma_per256K_memcpy(src, dst, data_len - mod, &req_cfg, 0, desc_addr);
	if (mod) {
		src += data_len - mod;
		dst += data_len - mod;
		gdma_onebyte_memcpy(src, dst, mod, &req_cfg, 0, desc_addr);
		//memcpy((char *)dst->byte, (char *)src->byte, mod);
		//printf("desc_addr is 0x%x\n", (uint32_t)desc_addr);
		//printf("src is %x, dst is %x, mod is %d\n", src->byte, dst->byte, mod);
	}
	return 0;
}

int gdma_memcpy_align (uint8_t *src_addr, uint8_t *dst_addr, uint32_t data_len, uint32_t align)
{
    int i;

	ct = 0;
	ct1 = 0;
    if (!align)
        return gdma_memcpy(src_addr, dst_addr, data_len);
    if (data_len % align) {
        printf("Transfor len must be integral multiple of the align\n");
        return -1;
    }
    for (i = 0;i < data_len / align;i++) {
        if (gdma_memcpy(src_addr, dst_addr, align) < 0) {
            printf("Memcpy error\n");
            return -1;
        }
        src_addr += align;
        dst_addr += align;
    }
//	printf("The speed is %d KB/s, data_len is 0x%x, time is %d s\n", data_len / (int)lldiv(ct, 1000),
//				data_len, (int)lldiv(ct, 1000));
//	printf("ct1 is %lld\n", ct1);
    return 0;
}

int init_gdma (void)
{
    desc_addr = malloc(0x100);
    if (desc_addr == NULL) {
        dma_error("gdma cannot malloc\n");
        return -1;
    }
    set_secure_para(1);
    dma_req_configure(&req_cfg, 0, 1, 1, SECURE_VAL, 0, 0, 16, 3, DCCTRL0, SCCTRL0, SWAP_NO);
    gdma_file_descriptor(desc_addr);
    return 0;
}
