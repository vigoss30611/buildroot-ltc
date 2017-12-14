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


//#define SPI_CLK 27000000//3000000
#define LED_NUM 147

#define DUMMY_CNT 14
#define SPI_TFIFO_EMPTY  (1 << 0)
#define SPI_TFIFO_NFULL  (1 << 1)
#define SPI_RFIFO_NEMPTY (1 << 2)
#define SPI_BUSY         (1 << 4)

#define FLASH_CMD_WREN  0x6
#define FLASH_CMD_WRDI  0x4
#define FLASH_CMD_RDSR  0x5
#define FLASH_CMD_WRSR  0x1
#define FLASH_CMD_READ  0x3
#define FLASH_CMD_FARD  0xb
#define FLASH_CMD_WRIT  0x2
#define FLASH_CMD_WESR  0x50
#define FLASH_CMD_ERAS  0xc7

#define FLASH_READ_BLOCK 0x1000

#define FLASH_MOD_POLLREDY 0
#define FLASH_MOD_POLLWREN 1
#define FLASH_DUMMY   0x00

#undef writel(v,a)
#undef readl(a)
#define readl(a)			(*(volatile unsigned int *)(a))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

#define readl_temp(a)			(*(volatile unsigned int *)(a))
#define writel_temp(v,a)		(*(volatile unsigned int *)(a) = (v))

#undef philip_printf
#define philip_printf(strings, arg...) do {\
    if(1) { \
        irf->printf("\033[0;44m""%s""\033[0m""\033[0;34m"",%s()""\033[0m""\033[0;32m"",%d:""\033[0m" strings "\n", __FILE__,__FUNCTION__,__LINE__, ##arg); \
    } \
}while(0)


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define memcpy irf->memcpy
#define module_enable irf->module_enable
#define pads_chmod irf->pads_chmod
#define pads_pull irf->pads_pull




struct spi_transfer {
    uint8_t *in;
    uint8_t *out;
    int len;
};
extern struct irom_export *irf;
static int spibytes = 256;

static void gpio_out(int num,int value)
{
    *(unsigned int *)(0x20f00000+num*0x40+0x4) =!!value;  
}
static void gpio_init(int num)
{
    *((unsigned int *)(0x21e0906c+(num/8)*4)) |=1<<(num%8);//GPIO function
    *(unsigned int *)(0x20f00000+num*0x40+0x8) =1;//direct:output
    *(unsigned int *)(0x20f00000+num*0x40+0x4) =1;//high 
}

static void spi_set_bytes(int bytes) {
    spibytes = bytes;
    irf->printf("spi: bytes set to: %d\n", bytes);
}

static int spi_init_clk(void)
{
    int pclk =134000000;// module_get_clock(APB_CLK_BASE),
    int	i, j;
#if 0
    for(i = 2; i < 255; i += 2)
        for(j = 0; j < 256; j++)
            if(pclk / (i * (j + 1)) <= SPI_CLK)
                goto calc_finish;

calc_finish:
    if(i >= 255 || j >= 256) {
        irf->printf("spi: calc clock failed(%d, %d), use default.\n", i, j);
        i = 2; j = 39;
    }
    irf->printf("PCLK: %d, PS: %d, SCR: %d, Fout: %d\n", 
            pclk, i, j, pclk / (i * (j + 1)));
#else
    i=2;
    j=6;
    irf->printf("PCLK: %d, PS: %d, SCR: %d\n", 
            pclk, i, j);
#endif
    writel(0, SSP_CR1_0);
    writel(0, SSP_IMSC_0);
    writel(i, SSP_CPSR_0);
    writel((j << 8) | (3 << 6) | (0 << 4) | (7 << 0), SSP_CR0_0);
    writel((1 << 3) | (0 << 2) | (1 << 1) | (0 << 0), SSP_CR1_0);
    writel(4<<8 | 32, SSP_FIFOTH_0);

    return 0;
}

int spi_init_pads(void) {
    //int xom = get_xom(1);
    int index;

    //if(boot_device() != DEV_FLASH)
    //printf("warning: not spi boot\n");

    //	index = (xom == 4) ? 74: 
    //		((xom == 6)? 27: 121);
    index = 70;
    //irf->printf("xom=%d\n", xom);

    pads_chmod(PADSRANGE(index, (index + 4)), PADS_MODE_CTRL, 0);
    pads_chmod(80, PADS_MODE_CTRL, 0);
    //if(ecfg_check_flag(ECFG_ENABLE_PULL_SPI))

    pads_pull(PADSRANGE(index, (index + 4)), 1, 1);
    pads_pull(80, 1, 0);		/* clk */


    return 0;
}

void spi_fifo_clear(void)
{
    while(readl(SSP_SR_0) & SPI_BUSY);
    while(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
        readl(SSP_DR_0);
        //		printf("c:0x%02x\n", readl(SSP_DR_0));
    }
}
#define min(X, Y)				\
    ({ typeof (X) __x = (X), __y = (Y);	\
     (__x < __y) ? __x : __y; })
/* exchange support:
 *		@tx
 *		@tx rx
 *		@tx tx
 * the first tx is limited to bellow 8 bytes
 */
void spi_exchange(struct spi_transfer *td, int count)
{
    int skip = 0, len, dummy = 0, stat;
    uint8_t *p, ex[8], exlen;
    gpio_out(74,0);
#if 0
    int a;
    for(a = 0; a < count; a++)
        irf->printf("spi: exc(%d): in=0x%p, out=0x%p, len=0x%x\n",
                a, td[a].in, td[a].out, td[a].len);
#endif
    len = skip = td->len;
    p     = td->out;

    if(count > 1) {
        memcpy(ex, td->out, len);
        td++;
        exlen = min(8, len + td->len);

        if(td->out) {
            memcpy(ex + len, td->out, exlen - len);
            p     = td->out + exlen - len;
            len   = td->len + len - exlen;
            skip += td->len;
        } else {
            p = td->in;
            len   = td->len;
            dummy = td->len + len - exlen;
        }

        /* fill tfifo to enable pipe line */
        for(stat = 0; stat < exlen; stat++) {
		//irf->printf("we: %02x\n", ex[stat]);
            writel(ex[stat], SSP_DR_0);
        }

        for(stat = 0; stat < DUMMY_CNT; stat++) {
			writel(0x00, SSP_DR_0);
			writel(0x00, SSP_DR_0);
			writel(0x00, SSP_DR_0);
			writel(0x00, SSP_DR_0);
			dummy-=4;
		}

    }

    /* ok, imediately start the loop */
__loop:
    stat = readl(SSP_RIS_0);

    if(stat & (1 << 2)) {
        if(skip > 3) { skip -= 4;
            readl(SSP_DR_0);
            readl(SSP_DR_0);
            readl(SSP_DR_0);
            readl(SSP_DR_0);
            //irf->printf("rd4\n");
        } else if(skip) {
            //irf->printf("rs:0x%02x\n", readl(SSP_DR_0));
            while(skip) { 
				skip--;
                //irf->printf("rs\n");
                readl(SSP_DR_0); 
			}
        } else {

            if(!td->in)
                irf->printf("spi: unexpected dummy\n");
            else {
                if(len > 3) { 
					len -= 4;
                    *p++ = readl(SSP_DR_0);
                    *p++ = readl(SSP_DR_0);
                    *p++ = readl(SSP_DR_0);
                    *p++ = readl(SSP_DR_0);
					if(dummy>0) {
						dummy-=4;
						writel(FLASH_DUMMY, SSP_DR_0);
						writel(FLASH_DUMMY, SSP_DR_0);
						writel(FLASH_DUMMY, SSP_DR_0);
						writel(FLASH_DUMMY, SSP_DR_0);
					}
                    //irf->printf("r4: %02x %02x %02x %02x\n", *(p - 4), *(p - 3), *(p - 2), *(p - 1));
                } else if(len) {
                    while(len) { 
						len--;
                        *p++ = readl(SSP_DR_0);
                       //irf->printf("r%02x\n", *(p - 1));
                    }
                    //irf->printf("r:0x%02x\n", *(p-1));
                }
                if(!len) goto __finish;
                //irf->printf("r:0x%02x\n", a);
            }
        }
    }

    if(stat & (1 << 3)) {
        if(td->out) {
            if(len > 3) { 
				len -= 4;
                writel(*p++, SSP_DR_0);
                writel(*p++, SSP_DR_0);
                writel(*p++, SSP_DR_0);
                writel(*p++, SSP_DR_0);
				//irf->printf("w4: %02x %02x %02x %02x\n", *(p - 4), *(p - 3), *(p - 2), *(p - 1));
            } else if(len) {
                while(len) { 
					len--;
                    //irf->printf("2%02x\n", *p);
                    writel(*p++, SSP_DR_0); }
            }

            if(td->out && !len)
                goto __finish;
        } else if(dummy > 3) { 
			dummy -= 4;
			//irf->printf("wd4\n");
            writel(FLASH_DUMMY, SSP_DR_0);
            writel(FLASH_DUMMY, SSP_DR_0);
            writel(FLASH_DUMMY, SSP_DR_0);
            writel(FLASH_DUMMY, SSP_DR_0);

        } else if(dummy) {
            while(dummy) { 
				dummy--;
                //printf("wd\n");
                writel(FLASH_DUMMY, SSP_DR_0); }
        }
    }

    /* r time out */
    if(stat & (1 << 1)) {
#if 0
        while(skip) { 
			skip--;
            if(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
                //printf("rts\n");
                readl(SSP_DR_0);
            }
            else
                irf->printf("spi: data missed (s%d)\n", skip);
        }

        while(td->in && len) { 
			len--;
            if(readl(SSP_SR_0) & SPI_RFIFO_NEMPTY) {
                *p++ = readl(SSP_DR_0);
                //printf("rt%02x\n", *(p-1));
            }
            else
                irf->printf("spi: data missed (r%d)\n", len);
        }
#endif

        /* clear SSPRTINTR */
        writel(3, SSP_ICR_0);
        goto __finish;
    }

    goto __loop;

__finish:
    gpio_out(74,1);
    spi_fifo_clear();
}

void flash_poll(int mod) {
    uint8_t buf[8] = { FLASH_CMD_RDSR, 0 };
    uint32_t i=0,j=0;
    struct spi_transfer td[] = { {NULL, &buf[0], 1},
        {&buf[1], NULL, 1} };

    do {
        if( 30000==i++ )
        {
            j ^=1;i=0;
            gpio_out(LED_NUM,j);
        }
        spi_exchange(td, ARRAY_SIZE(td));
    } while((mod)? !(buf[1] & (1 << 1)): !!(buf[1] & 1));
}

void flash_wren(void) {
    uint8_t tx[8] = { FLASH_CMD_WREN,
        FLASH_CMD_WRDI };
    struct spi_transfer td[] = { {NULL, &tx[0], 1 },
        {NULL, &tx[1], 1} };
    //	spi_exchange(&td[1], 1);
    spi_exchange(&td[0], 1);
    flash_poll(FLASH_MOD_POLLWREN);
}


/****************************uboot_lite adds specially*********************************/
uint8_t flash_ReadWriteByte(uint8_t TxData)
{
    //int retry =0;
    //while( ( readl(SSP_SR_0)&(1<<1) )==0 ) ;
#if 0
    {
        retry++;
        if(retry>20000) 
        {
            irf->printf("wait TNF empty timeout\n");
            return 0;
        }	
    }
#endif
    writel(TxData, SSP_DR_0);
    //retry =0;
    while( ( readl(SSP_SR_0)&(1<<2) )==0 ) ;
#if 0
        {
            retry++;
            if(retry>20000) 
            {
                irf->printf("wait RNE no empty timeout\n");
                return 0;
            }
        }
#endif
        return readl(SSP_DR_0);
}
void flash_vs_read_new ( uint8_t *buf, int offs, int len )
{
    uint8_t *pbuf = buf + len;
    gpio_out(74,0);

    //irf->printf ( "@-->0\n" );
    flash_ReadWriteByte ( FLASH_CMD_FARD );
    flash_ReadWriteByte ( ( uint8_t ) ( offs >> 16 ) );
    flash_ReadWriteByte ( ( uint8_t ) ( offs >> 8 ) );
    flash_ReadWriteByte ( ( uint8_t ) ( offs ) );
    flash_ReadWriteByte ( 0xff ); //dummy

    while ( readl_temp ( SSP_SR_0 ) & 4 )
        readl_temp ( SSP_DR_0 );
    /*
       offs = 0;
       while ( readl_temp ( SSP_SR_0 ) & 4 )  {
       readl_temp ( SSP_DR_0 );
       offs++;
       }
       irf->printf ( "@-->1 %d\n", offs );

       int j = 0;
       for ( offs = 0; offs < 1000000; offs++ ) j += offs;
       irf->printf ( "@-->2 %x\n", j );
       */

    //irf->printf ( "@-->3\n" );

    writel_temp ( 0xff, SSP_DR_0 );
    writel_temp ( 0xff, SSP_DR_0 );
    writel_temp ( 0xff, SSP_DR_0 );
    writel_temp ( 0xff, SSP_DR_0 );

    while (buf<pbuf) {
        while (( readl_temp ( SSP_SR_0 ) & 4 )==0);
        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

        *buf++ = readl_temp ( SSP_DR_0 );
        *buf++ = readl_temp ( SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );
        writel_temp ( 0xff, SSP_DR_0 );

    }

    //irf->printf ( "@-->2 %d\n", pbuf - buf );

    /*
       while(len >=4 )
       {
       while( ( readl_temp(SSP_RIS_0)&(1<<3) )==0 ) ;
       writel_temp(0xff, SSP_DR_0);
       writel_temp(0xff, SSP_DR_0);
       writel_temp(0xff, SSP_DR_0);
       writel_temp(0xff, SSP_DR_0);

       while( ( readl_temp(SSP_RIS_0)&(1<<2) )==0 )  ;

       buf[j++] =readl_temp(SSP_DR_0);
       buf[j++] =readl_temp(SSP_DR_0);
       buf[j++] =readl_temp(SSP_DR_0);
       buf[j++] =readl_temp(SSP_DR_0);

       len -=4;
       }
       for(i=0;i<len;i++)
       buf[j++] =flash_ReadWriteByte(0xff);
       */

    gpio_out(74,1);
}
/*****************************************************************************/


int flash_vs_read(uint8_t *buf, int offs, int len, int extra)
{
    uint8_t tx[8] = {0, 0, 0, FLASH_CMD_FARD };
    int remain = len, *d = (int *)&tx[4];
    struct spi_transfer td[] = {
        {NULL, &tx[3], 5 },
        {buf,  NULL, 0 }
    }, *p;

    //	printf("spi: r (b=%p, o=0x%llx, l=0x%x)\n", buf, offs, len);

    //for(p = &td[1]; remain;) {
    p = &td[1];
    *d = offs;
    *d = __swab32(*d);
    *d >>= 8;

    //p->len = (remain < FLASH_READ_BLOCK)?
    //	remain: FLASH_READ_BLOCK;
    p->len =len;

    spi_exchange(td, ARRAY_SIZE(td));

#if 0
	int i;
	for(i = 0; i < (len > 20? 20: len); i++) {
		irf->printf("buf[%d] = 0x%x\n", i, buf[i]);
	}
#endif

#if 0
    remain -= p->len;
    offs   += p->len;
    p->in  += p->len;

}
#endif
return len;
}

int flash_vs_write(uint8_t *buf, int offs, int len, int extra)
{
    uint8_t tx[8] = { FLASH_CMD_WREN, 0, 0, FLASH_CMD_WRIT };
    int remain = len, *d = (int *)&tx[4];
    struct spi_transfer td[] = {
        {NULL, &tx[4], 4 },
        {NULL, buf,    0 },
    }, *p;

    //	printf("spi: w (b=%p, o=0x%llx, l=0x%x)\n", buf, offs, len);
    //flash_wren();
    for(p = &td[1]; remain;) {
        *d = offs;
        *d = __swab32(*d);
        *d &= ~0xff;
        *d |= FLASH_CMD_WRIT;
        p->len = (remain < spibytes) ? remain: spibytes;

        flash_wren();
        spi_exchange(td, ARRAY_SIZE(td));
        flash_poll(FLASH_MOD_POLLREDY);

        remain -= p->len;
        offs   += p->len;
        p->out += p->len;
    }

    return len;
}

int flash_vs_erase(loff_t offs, uint32_t len)
{
    uint8_t tx[8];
    struct spi_transfer td[2];

    tx[0] = FLASH_CMD_WRSR;
    tx[1] = 0;
    tx[2] = FLASH_CMD_ERAS;

    td[0].in = NULL;
    td[0].out = &tx[0];
    td[0].len = 2;

    td[1].in = NULL;
    td[1].out = &tx[2];
    td[1].len = 1;
    /* clear sr */
    //	printf("clear sr\n");
#if 1
    flash_wren();
    spi_exchange(&td[0], 1);
    flash_poll(FLASH_MOD_POLLREDY);
#endif
#if 0
    /* clear sr: alt */
    //	printf("clear sr: alt\n");
    spi_exchange(&td[2], 1);
    spi_exchange(&td[0], 1);
    flash_poll(FLASH_MOD_POLLREDY);
#endif

    /* erase */
    flash_wren();
    spi_exchange(&td[1], 1);
    flash_poll(FLASH_MOD_POLLREDY);
    gpio_out(LED_NUM,1);
    return 0;
}
int flash_vs_reset(void) {
    module_enable(SSP_SYSM_ADDR);
    spi_init_clk();
    spi_init_pads();
    spi_fifo_clear();
    gpio_init(74);//for Q3F spi cs,gpio74
    gpio_init(LED_NUM);
    return 0;
}

int flash_vs_align(void) {
    return 256;
}

