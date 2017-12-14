
#ifndef _BOARD_H_
#define _BOARD_H_

extern void board_scan_arch(void);
extern void board_init_i2c(void);

extern int board_binit(void);
extern int board_reset(void);
extern int board_shut(void);
extern int board_bootst(void);
extern int board_acon(void);

extern int a5pv10_binit(void);
extern int a5pv10_reset(void);
extern int a5pv10_shut(void);
extern int a5pv10_bootst(void);
extern int a5pv10_acon(void);

extern int a5pv20_binit(void);
extern int a5pv20_reset(void);
extern int a5pv20_shut(void);
extern int a5pv20_bootst(void);
extern int a5pv20_acon(void);

extern int a9pv10_binit(void);
extern int a9pv10_reset(void);
extern int a9pv10_shut(void);
extern int a9pv10_bootst(void);
extern int a9pv10_acon(void);

extern int q3pv10_binit(void);
extern int q3pv10_reset(void);
extern int q3pv10_shut(void);
extern int q3pv10_bootst(void);
extern int q3pv10_acon(void);

extern int q3f_fpga_binit(void);
extern int q3f_fpga_reset(void);
extern int q3f_fpga_shut(void);
extern int q3f_fpga_bootst(void);
extern int q3f_fpga_acon(void);

extern int apollo3_fpga_binit(void);
extern int apollo3_fpga_reset(void);
extern int apollo3_fpga_shut(void);
extern int apollo3_fpga_bootst(void);
extern int apollo3_fpga_acon(void);
#endif

