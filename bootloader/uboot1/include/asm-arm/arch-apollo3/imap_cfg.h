;;-----------------------------------------------------------------------------
#define SDRAM               0x1
#define DDR                 0x2
#define MSDRAM              0x3
#define MDDR                0x4
#define DDR2                0x7
#define PASDR_TEST          0x5
#define EMPTY               0x6
#define SYNOPSYS_DDR2       0x8
#define SYNOPSYS            0x9
#define DRAM_SET_ADD        0xa
#define DENALI_DDR2			0xb
#define SYNOPSYS_DDR3       0xC
#define SYNOPSYS_LPDDR2     0xD
#define SYNOPSYS_MDDR       0xE
;;-----------------------------------------------------------------------------
#define SIZE_32M            0x1
#define SIZE_64M            0x2
;;-----------------------------------------------------------------------------
#define PORT_A              0x1
#define PORT_B              0x2

#define CL2				    0x2
#define CL3				    0x3
#define CL4				    0x4
#define CL5				    0x5
#define CL6				    0x6
#define CL7				    0x7
#define CL8				    0x8

#define P1KB_X8				0x1
#define P2KB_X16			0x2


;;-----------------------------------------------------------------------------
;; STEP1: CLOCK TYPE
;;-----------------------------------------------------------------------------

#define NO_CLK_DIV_MODE     0x1
#define CLK_DIV_MODE		0x2
#define CLK_DIV_MODE_1_3_6  0x3
#define CA9_CLK_DIV_MODE_1_3_6 0x4
#define CLK_DEFAULT         0x5

#define CLK_TYPE_MODE       NO_CLK_DIV_MODE
#define DIV_TYPE_MODE       CLK_DEFAULT

;;-----------------------------------------------------------------------------
;; STEP1: CONFIG PA/PB MEMORY TYPE
;;-----------------------------------------------------------------------------
#define REGISTER_SET        EMPTY
#define FPGA_TYPE           EMPTY
#define PORTA_TYPE          SYNOPSYS_DDR3
#define PORTB_TYPE          EMPTY
#define LATENCY_TYPE		CL6
#define PAGE_TYPE			P1KB_X8

;;-----------------------------------------------------------------------------
;; STEP2: CONFIG PA/PB MEMORY SIZE
;;-----------------------------------------------------------------------------
#define PORTA_SIZE          SIZE_64M
#define PORTB_SIZE          SIZE_64M
;;-----------------------------------------------------------------------------
;; STEP3: CONFIG the location of STACK
;;-----------------------------------------------------------------------------
#define STACK_BASEADDR      PORT_A
;;-----------------------------------------------------------------------------
;; STEP4: IF it used to be WINCE, set WINCE_ENABLE to 1, else to 0
;; NOTE: remember to define SLEEPDATA_BASE_PHYSICAL/SLEEPDATA_BASE_VIRTUAL
;;       in WINCE startup.s
;;-----------------------------------------------------------------------------
#define WINCE_ENABLE        0

;;-----------------------------------------------------------------------------
;; STEP5: define _ISR_STARTADDRESS in imap_h/imap_cfg.h
;;-----------------------------------------------------------------------------

;;-----------------------------------------------------------------------------
;; if test for rtl set RTL_TEST flag as 1
;;-----------------------------------------------------------------------------
#define RTL_TEST            1

;;-----------------------------------------------------------------------------
;; if test for mmu set MMU_ENABLE flag as 1
;;-----------------------------------------------------------------------------
;; the mmu config is in imapx_system.inc and imapx_system.h
extern void cfg_mmu_table(void);
