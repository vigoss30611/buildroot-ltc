/*
 * Setting pull-up/down config for GPIOs
 * Usage:
 *     gpio-pud num_gpio {up,down}
 *
 * Based on example code from Gert van Loo & Dom
 */

#define	BCM2708_PERI_BASE	0x20000000
#define	GPIO_BASE		(BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define	PAGE_SIZE	(4*1024)
#define	BLOCK_SIZE	(4*1024)

int mem_fd;

/* I/O access */
volatile unsigned *gpio;

/* GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y) */
#define	INP_GPIO(g)		*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define	OUT_GPIO(g)		*(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define	SET_GPIO_ALT(g,a)	*(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define	GPIO_SET	*(gpio+7)  /* sets   bits which are 1 ignores bits which are 0 */
#define	GPIO_CLR	*(gpio+10) /* clears bits which are 1 ignores bits which are 0 */

#define	GET_GPIO(g)	(*(gpio+13)&(1<<g)) /* 0 if LOW, (1<<g) if HIGH */

#define	GPIO_PULL	(*(gpio+0x94/4))
#define	GPIO_PULLCLK0	(*(gpio+0x98/4))

#define	CMD_PUD_OFF	0
#define	CMD_PUD_DOWN	1
#define	CMD_PUD_UP	2

static void setup_io();

int main(int argc, char **argv)
{
	int num_gpio;
	int pull_command = -1;

	if (argc != 3) {
print_usage_and_exit:
		fprintf(stderr, "Usage: %s num_gpio {up,down,off}\n", argv[0]);
		exit(1);
	}

	num_gpio = atoi(argv[1]);
	if (0 == strcasecmp("up", argv[2])) {
		pull_command = CMD_PUD_UP;
	} else if (0 == strcasecmp("down", argv[2])) {
		pull_command = CMD_PUD_DOWN;
	} else if (0 == strcasecmp("off", argv[2])) {
		pull_command = CMD_PUD_OFF;
	}

	if ((pull_command < 0) || (num_gpio < 0) || (num_gpio >= 32))
		goto print_usage_and_exit;

	/* Set up gpio pointer for direct register access */
	setup_io();

	GPIO_PULL = pull_command;
	usleep(10000);
	GPIO_PULLCLK0 = (1 << num_gpio);
	usleep(10000);
	GPIO_PULL = CMD_PUD_OFF;
	GPIO_PULLCLK0 = 0;

	return 0;
}

/*
 * Set up a memory regions to access GPIO
 */
static void setup_io(void)
{
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) {
	      printf("can't open /dev/mem \n");
	      exit(-1);
	}

	gpio = mmap(
		NULL,			/* Any adddress in our space will do */
		BLOCK_SIZE,		/* Map length */
		PROT_READ | PROT_WRITE,	/* Enable reading & writting to mapped memory */
		MAP_SHARED,		/* Shared with other processes */
		mem_fd,			/* File to map */
		GPIO_BASE		/* Offset to GPIO peripheral */
	);

	close(mem_fd); /* No need to keep mem_fd open after mmap */

	if (gpio == MAP_FAILED) {
		printf("mmap error %m\n");
		exit(-1);
	}
}
