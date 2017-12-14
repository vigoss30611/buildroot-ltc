
#include "Ftp.h"
#include <sys/ioctl.h>
#include <errno.h>
static int verbose = 0;
/* connectEx
  5秒连接超时
*/

static int ftpMkdir(FtpClient *pFtp, char *sDir);
static int ftpCWD(FtpClient *pFtp, char *sDir);
static int ftpCmd(FtpClient *pFtp, char *fmt,...);

int connectEx(int fd, struct sockaddr *addr, int nLen)
{
	unsigned long val=1;//
	ioctl(fd, FIONBIO, &val);
	int ret=0;
	if(connect(fd, addr, nLen) == -1){
		struct timeval tv;
		fd_set writefds;
		tv.tv_sec = 15;//15秒超时
		tv.tv_usec = 0;
		FD_ZERO(&writefds);
		FD_SET(fd, &writefds);
		if(select(fd+1,NULL,&writefds,NULL,&tv)>0){
			int error=0;
			socklen_t len=sizeof(int);
			if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)<0)
				ret=-1;
			if(error!=0) ret=-1;
		}else ret=-1;//timeout/error
	}
	val=0;//
	ioctl(fd, FIONBIO, &val);
	return ret;
}
/* ftpDebug
  是否打开调试开关
*/
void ftpDebug(BOOL blOn)
{
	verbose=blOn;
}
void ftpInit(FtpClient *pFtp)
{
	pFtp->ftpio=NULL;
	pFtp->fd_ctrl=-1;
	pFtp->fd_dat=-1;
	strcpy(pFtp->msg, "");
}
/* ftpOpen
   功能:登录ftp服务器
   返回值: 0:成功, -1:失败
 */
int ftpOpen(FtpClient *pFtp, char *sIP, WORD nPort, char *sUser, char *sPass)
{
	pFtp->fd_ctrl = socket(AF_INET,SOCK_STREAM,0); 
	if (pFtp->fd_ctrl == -1) return -1;
	pFtp->ftpio = fdopen(pFtp->fd_ctrl,"r");
	if(!pFtp->ftpio){
		close(pFtp->fd_ctrl);
		pFtp->fd_ctrl=-1;
		return -1;
	}
	int nRet=0;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(nPort); 
	addr.sin_addr.s_addr = inet_addr(sIP);
	if (connectEx(pFtp->fd_ctrl,(struct sockaddr *)&addr,sizeof(addr)) == -1){
		perror("connect:");
		strcpy(pFtp->msg, "-1  Connected failed.");
		goto out;
	}
	nRet = ftpCmd(pFtp,NULL);
	if (nRet != 220)
		goto out;
	nRet = ftpCmd(pFtp,"USER %s",sUser);
	if (nRet != 331)
		goto out;
	nRet = ftpCmd(pFtp,"PASS %s",sPass);
	if (nRet != 230)
		goto out;
	return 0;
out:
	ftpFree(pFtp);
	printf("Errno: %d.\n", nRet);
	return -1;
}
void ftpFree(FtpClient *pFtp)
{
	if(pFtp->fd_dat!=-1){
		close(pFtp->fd_dat);
		pFtp->fd_dat=-1;
		usleep(5000);
	}
	if(pFtp->ftpio){
		fclose(pFtp->ftpio);
		pFtp->ftpio=NULL;
	}
	if(pFtp->fd_ctrl!=-1){
		close(pFtp->fd_ctrl);
		pFtp->fd_ctrl=-1;
	}
}
void ftpClose(FtpClient *pFtp)
{
	if(pFtp->fd_dat!=-1){
		close(pFtp->fd_dat);
		pFtp->fd_dat=-1;
//		usleep(5000);
		int nCount=0;
		struct timeval tv;
		fd_set rfds;
		while(nCount<5){	//最多15秒
			tv.tv_sec = 3;
			tv.tv_usec = 0;
			FD_ZERO(&rfds);
			FD_SET(pFtp->fd_ctrl, &rfds);
			if(select(pFtp->fd_ctrl+1,&rfds,NULL,NULL,&tv)>0){
				if(fgets(pFtp->msg,sizeof(pFtp->msg),pFtp->ftpio)){
					if(verbose) printf("%s", pFtp->msg);
					if(atoi(pFtp->msg)==226) break;
				}
			}
			nCount++;
		}
		//-----------------
	}
}
/* ftpCheckAndMkdir
   检测目录是否存在,如果不存在,则创建目录,并回到'/'
   返回值:0:成功, -1:出错
*/
int ftpCheckAndMkdir(FtpClient *pFtp, char *sFileName)
{
	int nRet=0;
	char *pServerFile=strrchr(sFileName, '/');
	if(pServerFile && pServerFile!=sFileName){
		char sPath[1024]={0};
		strncpy(sPath, sFileName, pServerFile-sFileName+1);//保留最后一个'/'
		nRet=ftpCWD(pFtp, sPath);
		if(nRet==-1){
			char *p=strchr(sPath+1, '/');
			char *p0=sPath;
			do{
				if(p){ *p=0; p++;}
				nRet=ftpMkdir(pFtp, p0);
				if(nRet==-1 && atoi(pFtp->msg)!=550){//创建目录失败
					ftpFree(pFtp);
					return -1;
				}
				ftpCWD(pFtp, p0);
				p0=p;
				p=strchr(p0+1, '/');
			}while(p);
		}
		nRet=ftpCmd(pFtp, "CWD /");
	}
	return nRet;
}
/* ftpMkdir
   创建目录
*/
static int ftpMkdir(FtpClient *pFtp, char *sDir)
{
	int nRet=ftpCmd(pFtp, "MKD %s", sDir);
	return (nRet==250)||(nRet==257)?0:-1;
}

/* ftpCWD
   进入服务器的指定目录
   返回值:0:成功, -1:失败
*/
static int ftpCWD(FtpClient *pFtp, char *sDir)
{
	return ftpCmd(pFtp, "CWD %s", sDir)==250?0:-1;
}
static int ftpCmd(FtpClient *pFtp, char *fmt,...)
{ 
	va_list vp; 
	int err,len; 
	if (fmt) 
	{ 
		va_start(vp,fmt); 
		len = vsprintf(pFtp->msg,fmt,vp); 
		pFtp->msg[len++] = '\r'; 
		pFtp->msg[len++]='\n'; 
		write(pFtp->fd_ctrl,pFtp->msg,len); 
		if (verbose){
			pFtp->msg[len]='\0';
			printf("%s", pFtp->msg);
		}
	} 
	do 
	{ 
		if (fgets(pFtp->msg,sizeof(pFtp->msg),pFtp->ftpio) == NULL) 
			return -1; 
		if (verbose) puts(pFtp->msg); 
	}while(pFtp->msg[3] == '-'); 
	//printf("%s:%d  ============msg:\n%s\n===========\n", __FILE__, __LINE__, pFtp->msg);
	sscanf(pFtp->msg,"%d",&err); 
	return err;
}
BOOL ftpGetDataPort(FtpClient *pFtp, DWORD *dwIp, WORD *wPort)
{
	int nRet = ftpCmd(pFtp, "PASV");
	if(nRet==227){//227 Entering Passive Mode (172,20,50,35,4,3)
		char *lpstr=strchr(pFtp->msg+4, '(');
		if(!lpstr) return FALSE;
		int n1,n2,n3,n4,n5,n6;
		DWORD dwReturn = 0;
		sscanf(lpstr+1,"%d,%d,%d,%d,%d,%d",&n1,&n2,&n3,&n4,&n5,&n6);
		dwReturn = (n1 << 24) | (n2 << 16) | (n3 << 8) | n4;
		if(dwIp)
			*dwIp = dwReturn;
		if(wPort)
			*wPort = (n5 << 8) | n6;
		return TRUE;
	}//port
	return FALSE;
}
int ftpGetDataFD(FtpClient *pFtp, char *filename)
{
	struct sockaddr_in addr;
	DWORD dwIP=0;
	WORD wPort=0;
	int sct, nRet;
	if(ftpGetDataPort(pFtp, &dwIP, &wPort)){//PASV
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(dwIP);
		addr.sin_port = htons(wPort);
		sct = socket(AF_INET,SOCK_STREAM,0);
		if(sct<0){
			close(sct);
			return -1;
		}
		//printf("PASV connect %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(wPort));
		if(connect(sct,(struct sockaddr *)&addr,sizeof(struct sockaddr_in)) == -1){
			perror("connect");
			close(sct);
			return -1;
		}
		nRet = ftpCmd(pFtp,"STOR %s",filename);
		//125:打开数据连接,开始传输   150:打开连接
		if (nRet == 150 || nRet == 125) return sct;
		close(sct);
	}else{//PORT
		socklen_t tmp=sizeof(struct sockaddr_in);
		int iSockFtpData = socket(AF_INET,SOCK_STREAM,0);
		getsockname(pFtp->fd_ctrl,(struct sockaddr *)&addr,&tmp);
		addr.sin_port = 0;
		if(bind(iSockFtpData,(struct sockaddr *)&addr,sizeof(addr)) == -1){
			goto err;
		}
		if(listen(iSockFtpData,1) == -1){
			goto err;
		}
		tmp = sizeof(addr);
		getsockname(iSockFtpData,(struct sockaddr *)&addr,&tmp);
        
        {
        unsigned char *c = (unsigned char *)&addr.sin_addr;
		unsigned char *p = (unsigned char *)&addr.sin_port;
		nRet = ftpCmd(pFtp,"PORT %d,%d,%d,%d,%d,%d", c[0],c[1],c[2],c[3],p[0],p[1]);
        }
        if (nRet != 200) goto err;
		nRet = ftpCmd(pFtp,"STOR %s",filename);
		if (nRet != 150) goto err;
		tmp = sizeof(addr);
		printf("PORT Listen %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		sct = accept(iSockFtpData,(struct sockaddr *)&addr,&tmp);
		if (sct != -1){
			close(iSockFtpData);
			return sct;
		}
		err:
			close(iSockFtpData);
	}
	return -1;
}
int ftpPutFile(char *ip, WORD nPort, char *user, char *pass, char *localfile, char *serverfile, BOOL blNeedREST)
{
	FtpClient ftp;
	int nRet, fd;
	int nOffset=0;
	ftpInit(&ftp);
	//1. 打开本地文件
	fd = open(localfile,O_RDONLY,0644);
 	if (fd == -1){
 		printf("Open(%s) error...\n", localfile);
 		return -1;
 	}
 	//2. 登录ftp服务器
	nRet=ftpOpen(&ftp, ip, nPort, user, pass);
	if(nRet<0)
	{
		close(fd);
		return -1;
	}
/*
	char *pServerFile=strrchr(serverfile, '/');
	if(pServerFile && pServerFile!=serverfile){
		char sPath[1024]={0};
		strncpy(sPath, serverfile, pServerFile-serverfile);
		ftpCmd(&ftp, "CWD %s", sPath);
		pServerFile++;
	}else */
	char *pServerFile=serverfile;
//	ftpCmd(&ftp, "PWD", NULL);

    nRet = ftpCheckAndMkdir(&ftp, pServerFile);
    if (nRet==-1) goto out;

	nRet = ftpCmd(&ftp, "TYPE I");
	if (nRet != 200 && nRet!=220) goto out;
	if(blNeedREST){//检测重服务器文件大小
		nRet = ftpCmd(&ftp, "SIZE %s", pServerFile);
		if(nRet==213) nOffset=atoi(ftp.msg+4);
		if(nOffset>0){
			nRet=ftpCmd(&ftp, "REST %d", nOffset);
			if(nRet==350)
				lseek(fd, nOffset, SEEK_SET);
			else
				nOffset=0;//忽略seek
		}
	}
	ftp.fd_dat=ftpGetDataFD(&ftp, pServerFile);
	if(ftp.fd_dat>0){
        char buf[4096]={0};
        nRet=read(fd, buf, sizeof(buf));
		while(nRet!=0)
		{
			write(ftp.fd_dat,buf,nRet);
			//printf("send %d(%s)\n", nRet, buf);
			nRet=read(fd, buf, sizeof(buf));
		}
		ftpClose(&ftp);
	}
	close(fd);//关闭文件
	ftpFree(&ftp);
	return nRet;
out:
	close(fd);
	ftpFree(&ftp);
	return -1;
}
/* ftpCheck()
   检测ftp服务器是否支持文件读写
   返回值:TRUE:成功, FALSE:失败
 */
BOOL ftpCheck(char *sIP, WORD nPort, char *user, char *pass)
{
	FtpClient ftp;
	int nRet;
	char sNewBlock[32]={"abcdefghijklmnopqrstuvwxyz"};
	int nSize=0;
    
    ftpInit(&ftp);
	nRet=ftpOpen(&ftp, sIP, nPort, user, pass);
	if(nRet!=0) goto Err1;
	printf("Check Login     OK\n");
	nRet = ftpCmd(&ftp, "TYPE I");
	if (nRet != 200 && nRet!=220) goto Err1;
	nRet=ftpCmd(&ftp, "REST 100");
	if(nRet!=350) goto Err1;
	printf("Check REST      OK\n");
	ftpCmd(&ftp, "REST 0");	//回到头部
	nRet=ftpCmd(&ftp, "MKD _TMP");
	if(nRet!=250 && nRet!=257) goto Err1;
	nRet=ftpCmd(&ftp, "RMD _TMP");
	if(nRet!=250){
		printf("RMD Error:%s\n", ftp.msg);
	}
	ftp.fd_dat=ftpGetDataFD(&ftp, "_tmpchk.tmp");
	if(ftp.fd_dat<0) goto Err1;
	ftpStreamWrite(&ftp, sNewBlock, strlen(sNewBlock));
	ftpFree(&ftp);
	nRet=ftpOpen(&ftp, sIP, nPort, user, pass);//重新登录
	nRet=ftpStreamStart(&ftp, "_tmpchk.tmp", 10);//定位
	if(nRet>=0){
		ftpStreamWrite(&ftp, "XX", 2);
	}
	ftpFree(&ftp);
	nRet=ftpOpen(&ftp, sIP, nPort, user, pass);//重新登录
	nRet = ftpCmd(&ftp, "SIZE %s", "_tmpchk.tmp");
	
    if(nRet==213) nSize=atoi(ftp.msg+4);
	if(nSize!=strlen(sNewBlock)){
		printf("Failed to rewrite...\n");;
	}
	nRet=ftpCmd(&ftp, "DELE %s", "_tmpchk.tmp");
	if(nRet==550) printf("Failed to delete file..\n");
	ftpFree(&ftp);
	return nSize==strlen(sNewBlock)?TRUE:FALSE;
Err1:
	printf("Message:%s\n", ftp.msg);
	ftpFree(&ftp);
	return FALSE;
}
#ifdef DEBUG
int testUploadStream(char *ip, WORD nPort, char *user, char *pass, char *serverfile)
{
	FtpClient ftp;
	int fd;
	int nRet;
	//1. 打开本地文件
	char localfile[]="/mnt/hgfs/myvm/libsrc_4199.rar";//test
	fd = open(localfile,O_RDONLY,0644);
	if (fd == -1){
		printf("Open(%s) error...\n", localfile);
		return -1;
	}
	ftpInit(&ftp);
	//2. 登录ftp服务器
	nRet=ftpOpen(&ftp, ip, nPort, user, pass);
	if(nRet<0){
		close(fd);
		return -1;
	}
	//-----------------
	if(ftpCheckAndMkdir(&ftp, serverfile)==-1){
		ftpFree(&ftp);
		return -1;
	}
	//-----------------
	nRet=ftpStreamStart(&ftp, serverfile, 0);
	if(nRet>=0){
		char buf[4096]={0};
		nRet=read(fd, buf, sizeof(buf));
		while(nRet!=0)
		{
			if(ftpStreamWrite(&ftp, buf,nRet)<0) break;//出错了
			nRet=read(fd, buf, sizeof(buf));
		}
		ftpStreamStop(&ftp);
	}
	ftpFree(&ftp);//完全关闭
	//-----------------
	nRet=ftpOpen(&ftp, ip, nPort, user, pass);//重新登录
	nRet=ftpStreamStart(&ftp, serverfile, 10);//定位
	if(nRet>=0){
		char sNewBlock[16]={0};
		int i;
		for(i=0; i<10; i++){
			sprintf(sNewBlock, "NewBlock%03d\r\n", i);
			ftpStreamWrite(&ftp, sNewBlock, strlen(sNewBlock));
		}
		ftpStreamStop(&ftp);
	}
	//-----------------
	close(fd);//关闭文件
	ftpFree(&ftp);
	return nRet<0?-1:0;
}
#endif
/* ftpStreamStart()
   获取服务器中文件信息,并建立socket对通讯,等待写入数据
   nOffset<0,则先检测服务器上文件大小,数据追加到时文件末尾
   返回值: 已存在文件大小, 出错则返回-1
 */
int ftpStreamStart(FtpClient *pFtp, char *sSrvFile, int nOffset)
{
	int nRet = ftpCmd(pFtp, "TYPE I");
	if (nRet != 200 && nRet!=220) goto out;
	if(nOffset<0){//检测重服务器文件大小
		nRet = ftpCmd(pFtp, "SIZE %s", sSrvFile);
		if(nRet==213) nOffset=atoi(pFtp->msg+4);
	}
	if(nOffset>0){
		nRet=ftpCmd(pFtp, "REST %d", nOffset);
		if(nRet!=350) nOffset=0;//从0开始
	}
	//----------------
	pFtp->fd_dat=ftpGetDataFD(pFtp, sSrvFile);
	if(pFtp->fd_dat>0) return nOffset;
out:
	return -1;
}
/* ftpStreamWrite()
   往ftp服务器写入数据
 */
int ftpStreamWrite(FtpClient *pFtp, char *Buf, int nLen)
{
	if(pFtp->fd_dat<0) return -1;//已关闭
	int nRet, nWrite=0;
	struct timeval tv;
	fd_set writefds;
	do{
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		FD_ZERO(&writefds);
		FD_SET(pFtp->fd_dat, &writefds);
		nRet=select(pFtp->fd_dat+1,NULL,&writefds,NULL,&tv);
		if(nRet>0){
			nRet=write(pFtp->fd_dat, Buf+nWrite, nLen-nWrite);
			if(nRet<0) break;//出错了
			nWrite+=nRet;
		}else{
			return -1;//出错了
		}
	}while(nWrite<nLen);
//	printf("send %d/%d(%s)\n", nRet,nLen, Buf);
	return nRet;
}
/* ftpStreamStop()
   关闭数据传输socket
 */
void ftpStreamStop(FtpClient *pFtp)
{
	ftpClose(pFtp);
}

