#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "QMAPIType.h"
#include "QMAPIErrno.h"
#include "os_Error.h"
#include "dev_tty.h"

#define     DMS_RS_PORT_0                   "/dev/ttyAMA0"
#define     DMS_RS_PORT_1                   "/dev/ttyAMA1"
#define     DMS_RS_PORT_2                   "/dev/ttyAMA2"
#define     DMS_RS_PORT_3                   "/dev/ttyAMA3"


int get_tty_name(int deviceno, char *devName)
{   
    if (devName == NULL)
    {
        return QMAPI_ERR_INVALID_PARAM;
    }
    
    switch (deviceno)
    {
    case COM_PORT_0:
        strcpy(devName, DMS_RS_PORT_0);
        break;
        
    case COM_PORT_1:
        strcpy(devName, DMS_RS_PORT_1);
        break;
        
    case COM_PORT_2:
        strcpy(devName, DMS_RS_PORT_2);
        break;
    case COM_PORT_3:
        strcpy(devName, DMS_RS_PORT_3);
        break;
        
    default:
        return QMAPI_ERR_ILLEGAL_PARAM;
    }
    
    return 0;
}

/* 打开串口设备，返回串口句柄，<0失败错误码，成功返回>=0句柄 */
int DMS_DEV_TTY_Open(int deviceno)
{
    int fd;
    int nRes = 0;
    char csttyName[128];

    nRes = get_tty_name(deviceno, csttyName);
	if(nRes)
	{
		printf("get_tty_name is Failed!\n");
	}
	else
	{
		printf("get_tty_name(), csttyName = %s\n", csttyName);
	}
	
    DMS_Assert(nRes);
    mknod(csttyName,S_IFCHR | S_IRUSR | S_IWUSR,(204 << 8) | (64 + deviceno));
    fd =  open(csttyName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd == -1)
    {
        printf("LDEV Open %s FAILD:%s\n",csttyName,strerror(errno));
        return -1;
    }
    return fd;
}

/* 关闭串口设备 */
int DMS_DEV_TTY_Close(int fd)
{
    if(fd < 0)
    {
        return QMAPI_ERR_INVALID_HANDLE;
    }
    close(fd);

    return QMAPI_SUCCESS;
}

/* 阻塞方式，读入串口数据 */
 int DMS_DEV_TTY_ReadBlock(int fd, void *data, int maxlen)
{
	//printf("-------------------------------------------DMS_DEV_TTY_ReadBlock\n");
    int readlen, fs_sel, bytes;
    fd_set fs_read;
    struct timeval tv_timeout;
    //int   i;
    int recved_len = 0;
    while(1)
    {
        FD_ZERO(&fs_read);
        FD_SET(fd, &fs_read);
        
        tv_timeout.tv_sec = 1;
        tv_timeout.tv_usec = 0;
        fs_sel = select(fd+1, &fs_read, NULL, NULL, &tv_timeout);
        if (fs_sel == 0)
        {
            break;
        }
        if (fs_sel < 0)
        {
            DMS_err();
            return -1;
        }
        
        ioctl(fd, FIONREAD, &bytes);
        
        if (bytes == 0)
        {
        	printf("------------------ioctl(fd, FIONREAD, &bytes); bytes == 0\n");
            break;
        }
        
        if(FD_ISSET(fd, &fs_read))
        {
            readlen = read(fd, data+recved_len, (bytes > maxlen-recved_len) ? maxlen-recved_len : bytes);
            
            if(readlen > 0 )
            {
                recved_len += readlen;
            }
            
            if (recved_len >= maxlen)
            {
                break;
            }
            else
            {
                //  DMS_err();
                continue;
            }
        }   
    }   
    return recved_len;
}

/* 非阻塞方式，读入串口数据 */
 int DMS_DEV_TTY_Read(int fd, void *data, int maxlen)
{
    int readlen = 0;
    int bytes = 0;
    memset(data, 0x00, maxlen);
    ioctl(fd, FIONREAD, &bytes);

    if (bytes == 0)
    {
        return 0;
    }
	//printf("-------------------------------------------DMS_DEV_TTY_Read\n");
    readlen = read(fd, data, (bytes > maxlen) ? maxlen: bytes);

    return readlen;
}

/* 波特率(bps)，0－50；1－75；2－110；3－150；4－300；5－600；6－1200；7－2400；8－4800；9－9600；10－19200；11－38400；12－57600；13－76800；14－115.2k */

const int BaudRate[] = { B50, B75, B110, B150, B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B115200 };

/* 数据有几位 0－5位，1－6位，2－7位，3－8位 */

/* 停止位 0－1位，1－2位  */

/* 校验 0－无校验，1－奇校验，2－偶校验; */

/* 0－无，1－软流控,2-硬流控*/

int convbaud(DWORD  dwBaudRate)
{
    int BaudRate = B9600;
    switch(dwBaudRate)
    {
        case 600:
            BaudRate = B600;
            break;
        case 1200:
            BaudRate = B1200;
            break;
        case 2400:
            BaudRate = B2400;
            break;
        case 4800:
            BaudRate = B4800;
            break;
        case 9600:
            BaudRate = B9600;
            break;
        case 19200:
            BaudRate = B19200;
            break;
        case 38400:
            BaudRate = B38400;
            break;
        case 57600:
            BaudRate = B57600;
            break;
        case 115200:
        default:
            BaudRate = B115200;
            break;
    }

    return BaudRate;
}


/* 阻塞方式，写入串口数据 */
 int DMS_DEV_TTY_Write(int fd, void *data, int len)
{
    int writelen = 0;
    
    writelen = write(fd, data, len);
    if (writelen == len)
    {
        return 0;
    }
    else
    {
        DMS_TRACE(PRINT_LEVEL_TRACE, "fd:%d err:%s\n", fd, strerror(errno));
        tcflush(fd, TCOFLUSH);
        return errno;
    }
    return 0;
}

/* 设置串口参数 */
 int DMS_DEV_TTY_SetAttr(int fd, DEV_INTER_SERIAL_ATTR *attr)
{
    struct termios termios_old, termios_new;
    int     baudrate;//, tmp;
    char    databit, stopbit, parity, fctl;
/*  DMS_TRACE(PRINT_LEVEL_DEBUG, "tty fd:%d\n", fd);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty byDataBit:%d\n", attr->byDataBit);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty byFlowcontrol:%d\n", attr->byFlowcontrol);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty byMode:%d\n", attr->byMode);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty byParity:%d\n", attr->byParity);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty byStopBit:%d\n", attr->byStopBit);
    DMS_TRACE(PRINT_LEVEL_DEBUG, "tty dwBaudRate:%lu\n", attr->dwBaudRate);
*/
    if( fd < 0 )
    {
        return QMAPI_ERR_INVALID_HANDLE;
    }
    
    bzero(&termios_old, sizeof(termios_old));
    bzero(&termios_new, sizeof(termios_new));
    
    cfmakeraw(&termios_new);
    tcgetattr(fd, &termios_old);
/*  DMS_TRACE(PRINT_LEVEL_TRACE, "c_iflag:%lu c_oflag:%lu c_cflag:%lu c_lflag:%lu c_line:%lu\n", 
        termios_old.c_iflag, termios_old.c_oflag, termios_old.c_cflag, termios_old.c_lflag, termios_old.c_line);
    for(i=0; i<NCCS; i++)
    {
        DMS_TRACE(PRINT_LEVEL_TRACE, "%d c_cc %d\n", termios_old.c_cc[i]);
    }
*/  // baudrates
    baudrate = convbaud(attr->dwBaudRate);
//  DMS_TRACE(PRINT_LEVEL_TRACE, " tty baudrate:%d  dwBaudRate:%d\n", baudrate, attr->dwBaudRate);
    cfsetispeed(&termios_new, baudrate);        
    cfsetospeed(&termios_new, baudrate);        
    termios_new.c_cflag |= CLOCAL;          
    termios_new.c_cflag |= CREAD;           
    fctl = attr->byFlowcontrol;
    switch (fctl)
    {
    case 0:
        termios_new.c_cflag &= ~CRTSCTS;                // no flow control
        break;
        
    case 1:
        termios_new.c_iflag |= IXON | IXOFF |IXANY;     //software flow control
        break;
        
    case 2:
        termios_new.c_cflag |= CRTSCTS;             // hardware flow control
        break;
    }
    
    // data bits
    termios_new.c_cflag &= ~CSIZE;      
    databit = attr->byDataBit;
    switch (databit)
    {
    case 0:
        termios_new.c_cflag |= CS5;
        break;
        
    case 1:
        termios_new.c_cflag |= CS6;
        break;
        
    case 2:
        termios_new.c_cflag |= CS7;
        break;
    case 3:
        termios_new.c_cflag |= CS8;
        break;
    default:
        termios_new.c_cflag |= CS8;
        break;
    }
    
    // parity check
    parity = attr->byParity;
    switch (parity)
    {
    case 0:
        termios_new.c_cflag &= ~PARENB;     // no parity check
        termios_new.c_iflag &= ~INPCK;      /* Enable parity checking */
        break;
    case 1:
        termios_new.c_cflag |= PARENB;          // odd check
        termios_new.c_cflag |= PARODD;
        termios_new.c_iflag |= INPCK;           /* Disnable parity checking */ 
        break;
    case 2:
        termios_new.c_cflag |= PARENB;          // even check
        termios_new.c_cflag &= ~PARODD;
        termios_new.c_iflag |= INPCK;           /* Disnable parity checking */ 
        break;
    }
    
    // stop bits
    stopbit = attr->byStopBit;
    switch (stopbit)
    {
    case 0:
        termios_new.c_cflag &= ~CSTOPB;     // 1 stop bits
        break;
    case 1:
        termios_new.c_cflag |= CSTOPB;          // 2 stop bits
        break;
    }
    
    //termios_new.c_iflag &=~(IXON | IXOFF | IXANY);
    //termios_new.c_iflag &=~(INLCR | IGNCR | ICRNL);
    
    termios_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    
    //other attributions default
    termios_new.c_oflag &= ~OPOST;          
    termios_new.c_cc[VMIN]  = 1;                /* define the minimum bytes data to be readed*/         
    termios_new.c_cc[VTIME] = 1;                // 设置超时 unit: (1/10)second
    
    tcflush(fd, TCIFLUSH);          
    //tmp = tcsetattr(fd, TCSANOW, &termios_new); // TCSANOW
    tcsetattr(fd, TCSANOW, &termios_new); // TCSANOW
    tcgetattr(fd, &termios_old);

/*  DMS_TRACE(PRINT_LEVEL_TRACE, "c_iflag:%lu c_oflag:%lu c_cflag:%lu c_lflag:%lu c_line:%lu\n", 
        termios_old.c_iflag, termios_old.c_oflag, termios_old.c_cflag, termios_old.c_lflag, termios_old.c_line);
    for(i=0; i<NCCS; i++)
    {
        DMS_TRACE(PRINT_LEVEL_TRACE, "%d c_cc %d\n", termios_old.c_cc[i]);
    }
*/
    return 0;
}

/* 获取串口参数 */
 int DMS_DEV_TTY_GetAttr(int fd, DEV_INTER_SERIAL_ATTR *attr)
{
    return 0;
}

