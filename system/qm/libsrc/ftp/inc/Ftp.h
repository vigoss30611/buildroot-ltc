#ifndef _FTP_STREAM_RECORD_H_
#define _FTP_STREAM_RECORD_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
/*
typedef int BOOL;
typedef unsigned int DWORD;
typedef unsigned short WORD;
*/
#include "QMAPIType.h"
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct{
	int fd_ctrl;
	int fd_dat;
	FILE *ftpio;
	char msg[512];
}FtpClient;

/* ftpDebug
  是否打开调试开关
*/
void ftpDebug(BOOL blOn);

/* connectEx
  5秒连接超时
*/
int connectEx(int fd, struct sockaddr *addr, int nLen);

/* ftpInit
   初始化FtpClient结构
 */
void ftpInit(FtpClient *pFtp);
/* ftpOpen
   返回值: 0:成功, -1:失败
 */
int ftpOpen(FtpClient *pFtp, char *sIP, WORD nPort, char *sUser, char *sPass);
/* ftpFree()
   关闭ftp所有连接
 */
void ftpFree(FtpClient *pFtp);
/* ftpClose()
   只关闭ftp数据连接socket,以备重新连接
 */
void ftpClose(FtpClient *pFtp);
/* ftpCheckAndMkdir
   检测目录是否存在,如果不存在,则创建目录,并回到'/'
   sFileName:如 /dir1/dir2/dir3/file.dat,侧自动在ftp服务器上建立/dir1/dir2/dir3目录,以备上传该文件
   返回值:0:成功, -1:出错
*/
int ftpCheckAndMkdir(FtpClient *pFtp, char *sFileName);

//------------------------------------------------------------------
/* ftpCheck()
   检测ftp服务器是否支持文件回写及创建目录
   返回值: TRUE-支持, FALSE-不支持
 */
BOOL ftpCheck(char *sIP, WORD nPort, char *user, char *pass);
/* ftpStreamStart()
   获取服务器中文件信息,并建立socket对通讯,等待写入数据
   nOffset<0,则先检测服务器上文件大小,数据追加到时文件末尾
   返回值: 文件写入点(已存在文件大小或者是nOffset位置), 出错则返回-1
 */
int ftpStreamStart(FtpClient *pFtp, char *sSrvFile, int nOffset);
/* ftpStreamWrite()
   往ftp服务器写入数据
   返回值: 实际写入字节数
 */
int ftpStreamWrite(FtpClient *pFtp, char *Buf, int nLen);
/* ftpStreamStop()
   关闭与ftp服务器的连接
 */
void ftpStreamStop(FtpClient *pFtp);

#ifdef DEBUG
/* testUploadStream()
   返回值: 0:成功, -1:出错
   上传示例
 */
int testUploadStream(char *ip, WORD nPort, char *user, char *pass, char *serverfile);
#endif

/* FtpPutFile
   文件上传
   char *localfile-本地文件名
   char *serverfile-服务器上文件名
   BOOL blNeedREST-FALE:从头上传, TRUE:断点继传
   返回值:0:上传成功, -1:上传失败
*/

int ftpPutFile(char *ip, WORD nPort, char *user, char *pass, char *localfile, char *serverfile, BOOL blNeedREST);

#endif

