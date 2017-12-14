#include <common.h>
#include <asm/io.h>
#include <jtag.h>

void ss_jtag_en(int en) {
	writel(!!en, CPU_SYSM_ADDR + 0xb8);
}

