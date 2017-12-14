#include <asm/io.h>
#include <preloader.h>
#include <items.h>
#include <training.h>
#include <drama9.h>

#define IMAPX_S2C 
//#define DRAM_DBG 

#ifdef DRAM_DBG
#define dram_dbg(x...) spl_printf(x)
#else
#define dram_dbg(x...)
#endif

#define dram_err(x...) spl_printf(x)

struct drama9_ctrl drama9 = {
    .port_type = A9UMCTL2_DDR2,
    .bl_type = BL8,
    .test_type = NORMAL_TEST,
    .latency_type = A9CL5,
    .register_set = EMPTY,
};

void drama9_module_reset(void)
{
    int reset = 0;

    writel(0x7, EMIF_SYSM_ADDR + DRAM_CORE_RESET);
    writel(0xff, EMIF_SYSM_ADDR + DRAM_PORT_RESET);

    //irf->udelay(LOOP_TO_US(0x10));
        
    if (drama9.port_type == A9UMCTL2_DDR2 ||
        drama9.port_type == A9UMCTL2_DDR3) 
        reset = 1;

    writel(reset, EMIF_SYSM_ADDR + DRAM_CORE_RESET);
    writel(0x0, EMIF_SYSM_ADDR + DRAM_PORT_RESET);
}

void drama9_dll_reset(void)
{
    uint32_t timing;

    writel(0x0, DDR_PHY_ACDLLCR);
    irf->udelay(200);
    writel(0x40000000, DDR_PHY_ACDLLCR);
    while((readl(DDR_PHY_PGSR) & LOCK_PLL) != 4)
        irf->udelay(10);

#ifdef IMAPX_S2C
    if (drama9.port_type == A9DDR3 ||
        drama9.port_type == A9UMCTL2_DDR2)
        timing = 0x22222222;
    else if (drama9.port_type == A9DDR2)
        timing = 0x33333333;
#endif
    writel(timing, DDR_PHY_DX0DQTR);
    writel(timing, DDR_PHY_DX1DQTR);
    writel(timing, DDR_PHY_DX2DQTR);
    writel(timing, DDR_PHY_DX3DQTR);
}

struct drama9_reg umctl2_ddr2[] = {
    SET_DRAM_REQ(DDR_UMCTL2_DFIMISC,  0x00000000),
    SET_DRAM_REQ(DDR_UMCTL2_MSTR,     0x03040000),
    SET_DRAM_REQ(DDR_UMCTL2_RFSHTMG,  0x00080030),
    SET_DRAM_REQ(DDR_UMCTL2_INIT0,    0x00800080),
    SET_DRAM_REQ(DDR_UMCTL2_INIT1,    0x01000101),
    SET_DRAM_REQ(DDR_UMCTL2_INIT3,    0x07530004),
    SET_DRAM_REQ(DDR_UMCTL2_INIT4,    0x00000000),
    SET_DRAM_REQ(DDR_UMCTL2_INIT5,    0x00020001),
    SET_DRAM_REQ(DDR_UMCTL2_RANKCTL,  0x00000669),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG0, 0x0c141b0C),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG1, 0x00060417),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG2, 0x0005070b),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG3, 0x0000400c),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG4, 0x05040305),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG5, 0x05050403),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG6, 0x02020005),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG7, 0x00000202),
    SET_DRAM_REQ(DDR_UMCTL2_DRAMTMG8, 0x00000044),
    SET_DRAM_REQ(DDR_UMCTL2_ZQCTL0,   0x02000040),
    SET_DRAM_REQ(DDR_UMCTL2_ZQCTL1,   0x02000100),
    SET_DRAM_REQ(DDR_UMCTL2_DFITMG0,  0x01030103),
    SET_DRAM_REQ(DDR_UMCTL2_DFITMG1,  0x00000404),
    SET_DRAM_REQ(DDR_UMCTL2_DFIUPD0,  0x00040002),
    SET_DRAM_REQ(DDR_UMCTL2_DFIUPD1,  0x00040005),
#ifdef IMAPX_S2C
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP0, 0x00001414),
#endif
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP1, 0x00070707),
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP2, 0x00000000),
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP3, 0x0F000000),
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP4, 0x00000f0f),
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP5, 0x06060606),
#ifdef IMAPX_S2C
    SET_DRAM_REQ(DDR_UMCTL2_ADDRMAP6, 0x0F0F0606),
#endif
    SET_DRAM_REQ(DDR_UMCTL2_ODTMAP,   0x00000000),
    SET_DRAM_REQ(DDR_UMCTL2_SCHED,    0x7fc81001),
    SET_DRAM_REQ(DDR_UMCTL2_PERFHPR0, 0xffff00c8),
    SET_DRAM_REQ(DDR_UMCTL2_PERFHPR1, 0x040000c8),
    SET_DRAM_REQ(DDR_UMCTL2_PERFLPR0, 0xffff00ff),
    SET_DRAM_REQ(DDR_UMCTL2_PERFLPR1, 0x040000c9),
    SET_DRAM_REQ(DDR_UMCTL2_PERFWR0,  0xffff00c9),
    SET_DRAM_REQ(DDR_UMCTL2_PERFWR1,  0x040000ca),
    SET_DRAM_REQ(DDR_UMCTL2_DBG0,     0x66666600),
    SET_DRAM_REQ(DDR_UMCTL2_PCCFG,    0xffffff00),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_0,  0xffffd083),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_0,  0xffff4093),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_1,  0xffffd082),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_1,  0xffff4092),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_2,  0xffff4081),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_2,  0xffff4091),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_3,  0xffff4084),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_3,  0xffff4094),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_4,  0xffff4085),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_4,  0xffff4095),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_5,  0xffff4086),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_5,  0xffff4096),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_6,  0xffff4087),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_6,  0xffff4097),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGR_7,  0xffff4088),
    SET_DRAM_REQ(DDR_UMCTL2_PCFGW_7,  0xffff4098),
};

void drama9_umctl_reg(void)
{
    int i;

    if (drama9.bl_type == BL4) {
        umctl2_ddr2[UMCTL2_MSTR].value = 0x03020000;
        if (drama9.latency_type == A9CL4)
            umctl2_ddr2[UMCTL2_INIT3].value = 0x7420004;
    } else if (drama9.bl_type == BL8 && drama9.latency_type == A9CL4)
        umctl2_ddr2[UMCTL2_INIT3].value = 0x7430004; 

    if (drama9.test_type == BISTLP_TEST) {
        umctl2_ddr2[UMCTL2_RFSHTMG].value = 0x00000030;
        umctl2_ddr2[UMCTL2_INIT0].value = 0x00010001;
        umctl2_ddr2[UMCTL2_INIT1].value = 0x00010101;
    } else if (drama9.test_type == SIMULATION_TEST) {
        umctl2_ddr2[UMCTL2_RFSHTMG].value = 0x00620030;
        umctl2_ddr2[UMCTL2_INIT0].value = 0x00010001;
        umctl2_ddr2[UMCTL2_INIT1].value = 0x00010101;
    }

    if (drama9.latency_type == A9CL4) {
        umctl2_ddr2[UMCTL2_DRAMTMG0].value = 0x0b141b0c;
        umctl2_ddr2[UMCTL2_DRAMTMG1].value = 0x00060416;
        umctl2_ddr2[UMCTL2_DRAMTMG2].value = 0x0004070a;
        umctl2_ddr2[UMCTL2_DRAMTMG4].value = 0x04040304;
        umctl2_ddr2[UMCTL2_DFITMG0].value = 0x01020102;

    }

    for (i = 0; i < ARRAY_SIZE(umctl2_ddr2); i++) 
        writel(umctl2_ddr2[i].value, umctl2_ddr2[i].reg);
}

void drama9_phy_done(void)
{
    while ((readl(DDR_PHY_PGSR) & 0x3) != 0x3)
        irf->udelay(10);
}

struct drama9_reg phy_reg[] = {
    SET_DRAM_REQ(DDR_PHY_PGCR,  0x018C2E02),
    SET_DRAM_REQ(DDR_PHY_DCR,   0x0000000a),
    SET_DRAM_REQ(DDR_PHY_MR0,   0x00000653),
    SET_DRAM_REQ(DDR_PHY_MR1,   0x00000005),
    SET_DRAM_REQ(DDR_PHY_MR2,   0x00000000),
    SET_DRAM_REQ(DDR_PHY_MR3,   0x00000000),
    SET_DRAM_REQ(DDR_PHY_DTPR0, 0x2e72556e),
    SET_DRAM_REQ(DDR_PHY_DTPR1, 0x09220050),
    SET_DRAM_REQ(DDR_PHY_DTPR2, 0x1001a0c8),
    SET_DRAM_REQ(DDR_PHY_PTR0,  0x0020051b),
};

void drama9_phy_init(void)
{
    int i;
        
    if (drama9.bl_type == BL8 && drama9.latency_type == A9CL4)
        phy_reg[PHY_MR0].value = 0x643;
    else if (drama9.bl_type == BL4 && drama9.latency_type == A9CL4)
        phy_reg[PHY_MR0].value = 0x642;
        
    if (drama9.latency_type == A9CL4)
        phy_reg[PHY_DTPR0].value = 0x2c72446e;

    for (i = 0; i < ARRAY_SIZE(phy_reg); i++)
        writel(phy_reg[i].value, phy_reg[i].reg);

    drama9_phy_done();
    writel(0x00040001, DDR_PHY_PIR);
    drama9_phy_done();

    drama9_phy_done();
    writel(0x00000011, DDR_PHY_PIR);
    drama9_phy_done();
}

struct drama9_reg upctl_reg[] = {
    SET_DRAM_REQ(DDR_UPCTL_TOGCNT1U,    0x00000032),
    SET_DRAM_REQ(DDR_UPCTL_TINIT,       0x000000c8),
    SET_DRAM_REQ(DDR_UPCTL_TRSTH,       0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TOGCNT100N,  0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_MCFG,        0x00060000),
    SET_DRAM_REQ(DDR_PHY_PGCR,          0x018c2e02),
    SET_DRAM_REQ(DDR_PHY_DCR,           0x0000000a),
    SET_DRAM_REQ(DDR_PHY_MR0,           0x00000642),
    SET_DRAM_REQ(DDR_PHY_MR1,           0x00000005),
    SET_DRAM_REQ(DDR_PHY_MR2,           0x00000000),
    SET_DRAM_REQ(DDR_PHY_MR3,           0x00000000),
    SET_DRAM_REQ(DDR_PHY_DTPR0,         0x1e4b444a),
    SET_DRAM_REQ(DDR_PHY_DTPR1,         0x09220050),
    SET_DRAM_REQ(DDR_PHY_DTPR2,         0x1001a0c8),
    SET_DRAM_REQ(DDR_PHY_PTR0,          0x0020051b),
};

void drama9_upctl_ddr2_reg(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(upctl_reg); i++)
        writel(upctl_reg[i].value, upctl_reg[i].reg);

    drama9_phy_done();
    writel(0x00040001, DDR_PHY_PIR);
    drama9_phy_done();
}

void drama9_power_up(void)
{
    if (drama9.register_set == DRAM_SET_ADD) {
        while ((readl(DDR_UPCTL_DFISTSTAT0) & 0x1) != 0x1)
            irf->udelay(10);
    }

    writel(0x1, DDR_UPCTL_POWCTL);
    while ((readl(DDR_UPCTL_POWSTAT) & 0x1) != 0x1)
        irf->udelay(10);
}

struct drama9_reg rest_upctl[] = {
    SET_DRAM_REQ(DDR_UPCTL_TREFI,       0x00000067),
    SET_DRAM_REQ(DDR_UPCTL_TMRD,        0x00000002),
    SET_DRAM_REQ(DDR_UPCTL_TRFC,        0x00000032),
    SET_DRAM_REQ(DDR_UPCTL_TRP,         0x00010005),
    SET_DRAM_REQ(DDR_UPCTL_TRTW,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TAL,         0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCL,         0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TCWL,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TRAS,        0x0000000d),
    SET_DRAM_REQ(DDR_UPCTL_TRC,         0x00000012),
    SET_DRAM_REQ(DDR_UPCTL_TRCD,        0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TRRD,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TRTP,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TWR,         0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TWTR,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TEXSR,       0x000000c8),
    SET_DRAM_REQ(DDR_UPCTL_TXP,         0x00000003),
    SET_DRAM_REQ(DDR_UPCTL_TXPDLL,      0x00000003),
    SET_DRAM_REQ(DDR_UPCTL_TZQCS,       0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TZQCSI,      0x00000002),
    SET_DRAM_REQ(DDR_UPCTL_TDQS,        0x00000002),
    SET_DRAM_REQ(DDR_UPCTL_TCKSRE,      0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCKSRX,      0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCKE,        0x00000003),
    SET_DRAM_REQ(DDR_UPCTL_TMOD,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TRSTL,       0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TZQCL,       0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TMRR,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCKESR,      0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TDPD,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_SCFG,        0x00000420),
};

struct drama9_reg ddr3_reset_upctl[] = {                    
    SET_DRAM_REQ(DDR_UPCTL_TREFI,       0x00000030),
    SET_DRAM_REQ(DDR_UPCTL_TMRD,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TRFC,        0x00000048),
    SET_DRAM_REQ(DDR_UPCTL_TRP,         0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TRTW,        0x00000006),
    SET_DRAM_REQ(DDR_UPCTL_TAL,         0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCL,         0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TCWL,        0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TRAS,        0x0000000F),
    SET_DRAM_REQ(DDR_UPCTL_TRC,         0x00000014),
    SET_DRAM_REQ(DDR_UPCTL_TRCD,        0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TRRD,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TRTP,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TWR,         0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TWTR,        0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TEXSR,       0x00000200),
    SET_DRAM_REQ(DDR_UPCTL_TXP,         0x0000000A),
    SET_DRAM_REQ(DDR_UPCTL_TXPDLL,      0x00000003),
    SET_DRAM_REQ(DDR_UPCTL_TZQCS,       0x00000040),
    SET_DRAM_REQ(DDR_UPCTL_TZQCSI,      0x00000002),
    SET_DRAM_REQ(DDR_UPCTL_TDQS,        0x00000002),
    SET_DRAM_REQ(DDR_UPCTL_TCKSRE,      0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TCKSRX,      0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_TCKE,        0x00000003),
    SET_DRAM_REQ(DDR_UPCTL_TMOD,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TRSTL,       0x00000010),
    SET_DRAM_REQ(DDR_UPCTL_TZQCL,       0x00000200),
    SET_DRAM_REQ(DDR_UPCTL_TMRR,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_TCKESR,      0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_TDPD,        0x00000000),
    SET_DRAM_REQ(DDR_UPCTL_SCFG,        0x00000420),
};                                                  


/* Configure rest of uPCTL Program all timing T* registers */
void drama9_reset_upctl(void)
{
    int i;

    if (drama9.port_type == A9DDR2) {
        for (i = 0; i < ARRAY_SIZE(rest_upctl); i++)
            writel(rest_upctl[i].value, rest_upctl[i].reg);
    } else if (drama9.port_type == A9DDR3) {
        for (i = 0; i < ARRAY_SIZE(ddr3_reset_upctl); i++)
            writel(ddr3_reset_upctl[i].value, ddr3_reset_upctl[i].reg);
    }
}

void drama9_cmd_done(void)
{
    int upctl_cmd;

    upctl_cmd = readl(DDR_UPCTL_MCMD);
    while(upctl_cmd != 0x80000000) {
        irf->udelay(10);
        upctl_cmd = readl(DDR_UPCTL_MCMD);
    }
}

struct drama9_reg ddr2_mem[] = {
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f00000),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f00001),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f40003),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f60003),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f20043),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f07523),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f00001),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80F00002),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f00002),
    SET_DRAM_REQ(DDR_UPCTL_MCMD, 0x80f06523),
};

/* memory initialization procedure */
void drama9_ddr2_mem_init(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(ddr2_mem); i++) {
        drama9_cmd_done();
        writel(ddr2_mem[i].value, ddr2_mem[i].reg);
        drama9_cmd_done();
    }
    //irf->udelay(LOOP_TO_US(0xd000));
    writel(0x80f23843, DDR_UPCTL_MCMD);
    drama9_cmd_done();
    writel(0x80f20043, DDR_UPCTL_MCMD);
    drama9_cmd_done();
}

struct drama9_reg upctl_refine[] = {
    SET_DRAM_REQ_ABLE(DDR_UPCTL_PPCFG,          0x00000000,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITCTRLDELAY,  0x00000002,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFIODTCFG,      0x05050000,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFIODTCFG,      0x0d0d0d0d,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFIODTCFG1,     0x06060100,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFIODTRANKMAP,  0x00008421,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYWRDATA,  0x00000001,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYWRLAT,   0x00000003,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYRDLAT,   0x00000001,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYUPDTYPE0,0x00000010,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYUPDTYPE1,0x00000010,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYUPDTYPE2,0x00000010,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYUPDTYPE3,0x00000010,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITCTRLUPDMIN, 0x00000010,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITCTRLUPDMAX, 0x00000040,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITCTRLUPDDLY, 0x00000008,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFIUPDCFG,      0x00000003,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITREFMSKI,    0x00000000,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITCTRLUPDI,   0x00000000,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFISTCFG0,      0x00000007,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFISTCFG1,      0x00000003,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITDRAMCLKEN,  0x00000002,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITDRAMCLKDIS, 0x00000002,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFISTCFG2,      0x00000003,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFISTPARCLR,    0x00000000,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFISTPARLOG,    0x00000000,     DISABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFILPCFG0,      0x00070101,     ENABLE),
    /* ecc */
    SET_DRAM_REQ_ABLE(DDR_UPCTL_ECCCFG,         0x00000000,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITPHYWRLAT,   0x00000003,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UPCTL_DFITRDDATAEN,   0x00000002,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0,         0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x4,   0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x8,   0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0xc,   0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x10,  0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x14,  0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x18,  0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_PCFG_0 + 0x1c,  0x00100130,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_CCFG,           0x00000018,     ENABLE),
    SET_DRAM_REQ_ABLE(DDR_UMCTL_DCFG,           0x00000112,     ENABLE),
};

/* Configure uPCTL to refine configuration */
void drama9_ddr2_refine(void)
{
    int i;
    /*
     * uPCTL moves to Config state and sets STAT.ctl_stat = Config when completed
     * [2:0] init 0, CFG 1, GO 2, SLEEP 3, WAKEUP 4
     */
    writel(0x1, DDR_UPCTL_SCTL);
    while ((readl(DDR_UPCTL_STAT) & 0x7) != 0x1)
        irf->udelay(10);
    writel(0x00060000, DDR_UPCTL_MCFG);
        
    for (i = 0; i < ARRAY_SIZE(upctl_refine); i++) {
        if (drama9.register_set == DRAM_SET_ADD)
            writel(upctl_refine[i].value, upctl_refine[i].reg);
        else {
            if (upctl_refine[i].flags == ENABLE)
                writel(upctl_refine[i].value, upctl_refine[i].reg);
        }
    }

    /* write GO into SCTL state_cmd register and poll STAT ctl_stat = Access */
    writel(0x00000002, DDR_UPCTL_SCTL);
    while ((readl(DDR_UPCTL_STAT) & 0x7) != 3)
        irf->udelay(10);
}

/*
 * some things least to do for init ddr
 */

void drama9_umctl2_ddr2_least(void)
{
    uint32_t umctl2_stat;
    uint32_t phy_pgsr;

    writel(0x00000001, DDR_UMCTL2_DFIMISC);
    umctl2_stat = readl(DDR_UMCTL2_STAT);
    while ((umctl2_stat & 0x3) != 0x1) {
        umctl2_stat = readl(DDR_UMCTL2_STAT);
        irf->udelay(10);
    }
    writel(0x1, DDR_UMCTL2_RFSHCTL3);
    writel(0x0, DDR_UMCTL2_PWRCTL);
    writel(0x0, DDR_UMCTL2_DFIMISC);

    if (drama9.test_type == SIMULATION_TEST) {
        writel(0x7fFFF000, DDR_PHY_DTAR);
        writel(0x81, DDR_PHY_PIR);
        phy_pgsr = readl(DDR_PHY_PGSR);
        while ((phy_pgsr & 0x10) != 0x10) {
                irf->udelay(10);
                phy_pgsr = readl(DDR_PHY_PGSR);
        }
    }
    writel(0x0, DDR_UMCTL2_RFSHCTL3);
}

struct drama9_reg ddr3_reg[] = {
    SET_DRAM_REQ(DDR_UPCTL_TOGCNT1U,    0x00000032),
    SET_DRAM_REQ(DDR_UPCTL_TINIT,       0x000000f0),
    SET_DRAM_REQ(DDR_UPCTL_TRSTH,       0x00000200),
    SET_DRAM_REQ(DDR_UPCTL_TOGCNT100N,  0x00000005),
    SET_DRAM_REQ(DDR_UPCTL_MCFG,        0x00060021),
    SET_DRAM_REQ(DDR_UPCTL_DFIODTCFG,   0x05050000),
    SET_DRAM_REQ(DDR_UPCTL_DFITPHYWRLAT,0x00000004),
    SET_DRAM_REQ(DDR_UPCTL_DFITRDDATAEN,0x00000003),
    SET_DRAM_REQ(DDR_PHY_PGCR,          0x018C2E02),
    SET_DRAM_REQ(DDR_PHY_DCR,           0x0000000B),
    SET_DRAM_REQ(DDR_PHY_MR0,           0x00000210),
    SET_DRAM_REQ(DDR_PHY_MR1,           0x00000006),
    SET_DRAM_REQ(DDR_PHY_MR2,           0x00000000),
    SET_DRAM_REQ(DDR_PHY_MR3,           0x00000000),
    SET_DRAM_REQ(DDR_PHY_DTPR0,         0x2a905590),
    SET_DRAM_REQ(DDR_PHY_DTPR1,         0x007c18a4),
    SET_DRAM_REQ(DDR_PHY_DTPR2,         0x10022a00),
    SET_DRAM_REQ(DDR_PHY_PTR0,          0x0020051b),
};

void drama9_upctl_ddr3_reg(void)
{
    int i;
       
    if (drama9.bl_type == BL4)
        ddr3_reg[4].value = 0x00060020;
        
    ddr3_reg[6].value = ddr3_reg[6].value | (
            drama9.latency_type / 2);
    ddr3_reg[7].value = ddr3_reg[7].value + 
        drama9.latency_type - 1;

    for (i = 0; i < ARRAY_SIZE(ddr3_reg); i++)
        writel(ddr3_reg[i].value, ddr3_reg[i].reg);

    drama9_phy_done();
    writel(0x00040001, DDR_PHY_PIR);
    drama9_phy_done();
}

struct drama9_reg ddr3_mem[] = {
    SET_DRAM_REQ(DDR_UPCTL_MCMD,        0x86f00000),
    SET_DRAM_REQ(DDR_UPCTL_MCMD,        0x80f40003),
    SET_DRAM_REQ(DDR_UPCTL_MCMD,        0x80f20043),
    SET_DRAM_REQ(DDR_UPCTL_MCMD,        0x80f03103),
    SET_DRAM_REQ(DDR_UPCTL_MCMD,        0x80f00005),
};

void drama9_ddr3_mem_init(void)
{
    int i;

    if (drama9.latency_type == A9CL7 ||
           drama9.latency_type == A9CL8)
       ddr3_mem[1].value = 0x80f40083;
    if (drama9.latency_type == A9CL5 &&
           drama9.bl_type == BL4)
       ddr3_mem[3].value = 0x80f03123;
    if (drama9.latency_type > A9CL5)
       ddr3_mem[3].value = ddr3_mem[3].value + 0x2000 +
           (drama9.latency_type - 1) * 0x100;

    for (i = 0; i < ARRAY_SIZE(ddr3_mem); i++)
        writel(ddr3_mem[i].value, ddr3_mem[i].reg);
}

struct drama9_reg ddr3_refine[] = {
    SET_DRAM_REQ(DDR_UPCTL_PPCFG,               0x00000000),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0,              0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0 + 0x4,        0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0 + 0x8,        0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0 + 0xc,        0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0 + 0x10,       0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_PCFG_0 + 0x14,       0x01101030),
    SET_DRAM_REQ(DDR_UMCTL_CCFG,                0x00000018),
    SET_DRAM_REQ(DDR_UMCTL_DCFG,                0x00000111),
};

void drama9_ddr3_refine(void)
{
    int i;

    writel(0x1, DDR_UPCTL_SCTL);
    while ((readl(DDR_UPCTL_STAT) & 0x7) != 0x1)
            irf->udelay(10);
    writel(0x00060000, DDR_UPCTL_MCFG);
        
    for (i = 0; i < ARRAY_SIZE(ddr3_refine); i++)
        writel(ddr3_refine[i].value, ddr3_refine[i].reg);

    /* write GO into SCTL state_cmd register and poll STAT ctl_stat = Access */
    writel(0x00000002, DDR_UPCTL_SCTL);
    while ((readl(DDR_UPCTL_STAT) & 0x7) != 3)
        irf->udelay(10);
}

/*
 * uPCTL and Memory Initialization with PUBL
 */
void drama9_ctrlmem_init(void)
{
    if (drama9.port_type == A9UMCTL2_DDR2) {
        drama9_umctl_reg(); 
        /* Deassert soft reset signal RST.soft_rstb */
        writel(0x0, EMIF_SYSM_ADDR + DRAM_CORE_RESET);
        drama9_phy_init();
        drama9_umctl2_ddr2_least();
    } else if (drama9.port_type == A9DDR2) {
        drama9_upctl_ddr2_reg();
        drama9_power_up();
        drama9_reset_upctl();
        drama9_ddr2_mem_init();
        drama9_ddr2_refine();
    } else if (drama9.port_type == A9DDR3) {
        drama9_upctl_ddr3_reg(); 
        drama9_power_up();
        drama9_reset_upctl();
        drama9_ddr3_mem_init();
        drama9_ddr3_refine();
    } else
        dram_err("Now, Not support this ddr %d\n", drama9.port_type);
}

void drama9_init(void)
{
    // reset module
    drama9_module_reset();
    // clock dll reset
    drama9_dll_reset();
    // uPCTL and Memory Initialization with PUBL
    drama9_ctrlmem_init();
}
