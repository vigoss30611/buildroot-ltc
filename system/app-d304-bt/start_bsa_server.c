
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>

#define LOCAL_NAME_BUFFER_LEN                   32
#define HCI_EVT_CMD_CMPL_LOCAL_NAME_STRING      6
char hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
char hci_read_local_name[] = { 0x01, 0x14, 0x0c, 0x00 };
int uart_fd = -1;
char buffer[1024];
char local_name[LOCAL_NAME_BUFFER_LEN];
struct termios termios;

static void init_uart()
{
	if ((uart_fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY)) == -1) {
			printf("port ttyAMA0 could not be opened, error %d\n", errno);
		}
		
	tcflush(uart_fd, TCIOFLUSH);
	tcgetattr(uart_fd, &termios);

#ifndef __CYGWIN__
	cfmakeraw(&termios);
#else
	termios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                | INLCR | IGNCR | ICRNL | IXON);
	termios.c_oflag &= ~OPOST;
	termios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	termios.c_cflag &= ~(CSIZE | PARENB);
	termios.c_cflag |= CS8;
#endif

	termios.c_cflag |= CRTSCTS;
	tcsetattr(uart_fd, TCSANOW, &termios);
	tcflush(uart_fd, TCIOFLUSH);
	tcsetattr(uart_fd, TCSANOW, &termios);
	tcflush(uart_fd, TCIOFLUSH);
	tcflush(uart_fd, TCIOFLUSH);
	cfsetospeed(&termios, B115200);
	cfsetispeed(&termios, B115200);
	tcsetattr(uart_fd, TCSANOW, &termios);
}

static void hci_send_cmd(char *buf, int len)
{
	write(uart_fd, buf, len);
}

static void read_event(int fd, char *buffer)
{
	int i = 0;
	int len = 3;
	int count;

	while ((count = read(fd, &buffer[i], len)) < len) {
		i += count;
		len -= count;
	}

	i += count;
	len = buffer[2];

	while ((count = read(fd, &buffer[i], len)) < len) {
		i += count;
		len -= count;
	}
}

static void proc_read_local_name()
{
    int i;
    char *p_name;
    hci_send_cmd(hci_read_local_name, sizeof(hci_read_local_name));
    read_event(uart_fd, buffer);
    p_name = &buffer[1+HCI_EVT_CMD_CMPL_LOCAL_NAME_STRING];
    for (i=0; (i < LOCAL_NAME_BUFFER_LEN)||(*(p_name+i) != 0); i++)
        *(p_name+i) = toupper(*(p_name+i));
    strcpy(local_name,p_name);
    printf("chip id = %s\n", local_name);
}
static void get_bt_chip_id()
{
	init_uart();
	proc_read_local_name();
}

void main (int argc, char **argv)
{
	get_bt_chip_id();
	if (!strcmp(local_name, "4343A0"))//bcm6212
	{
		printf("--->start  bcm6212 \n");
		system("bsa_server -d /dev/ttyAMA0 -p /bt/bcm43438a0.hcd -u /tmp/ -k /config/bt/ble_local_keys -all=0 -r 10 &");
	}
	else if (!strcmp(local_name, "BCM43430A1"))//bcm6212a
	{
		printf("--->start  bcm6212A \n");
		system("bsa_server -d /dev/ttyAMA0 -p /bt/bcm43438a1.hcd -u /tmp/ -k /config/bt/ble_local_keys -all=0 -r 10 &");
	}
	else
	{
		printf("get bt id is error,local_name=%s\n",local_name);
	}				
}