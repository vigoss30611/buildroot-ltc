

#include <sys/types.h>
#include <sys/param.h>

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "libhttpd.h"


#include "cgiinterface.h"
#include "timers.h"

#include "libhttpd.h"
#include "thttpd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifndef HI_ERR_SERVER_PARAM
#define HI_ERR_SERVER_PARAM -2
#endif

//static PTR_FUNC_ICGI_PROC g_pfunSanpShot = 0;
PTR_FUNC_ICGI_PROC g_pfunOnvif = NULL;
PTR_FUNC_ICGI_PROC g_pfunCGIProc = NULL;
PTR_FUNC_ICGI_PROC g_pfunRtspProc = NULL;

//static PTR_FUNC_ICGI_PROC g_pfunBackup = 0;
//static PTR_FUNC_ICGI_PROC g_pfunRestore = 0;
//static PTR_FUNC_ICGI_PROC g_pfunSetUser = 0;
//static PTR_FUNC_ICGI_USERS_PROC g_pfunUserS = 0;
//static cgi_cmd_trans_s* pstru_icgi_cmd=0;

//static PTR_FUNC_ICGI_PROC g_pfunSdformat = 0;


extern int HI_CGI_SignalProc(char* pszCommand, char* query, char* aszDest, char* pszUrl,unsigned int u32Handle);
extern int HI_CGI_ParamProc(httpd_conn* hc);


int HI_ICGI_Register_Proc(HI_S_ICGI_Proc* pstruProc)
{
	if(0 == pstruProc)
	{
	    return -1;
	}
	printf("%s(%d): Register %s process function.\n", __FUNCTION__, __LINE__, pstruProc->pfunType);
	if( strcmp(pstruProc->pfunType, "onvif") == 0)
	{
		g_pfunOnvif = (PTR_FUNC_ICGI_PROC )pstruProc->pfunProc;
	}
	else if( strcmp(pstruProc->pfunType, "cgi") == 0)
	{
		g_pfunCGIProc = (PTR_FUNC_ICGI_PROC )pstruProc->pfunProc;
	}
	else if( strcmp(pstruProc->pfunType, "rtsp") == 0)
	{
		g_pfunRtspProc = (PTR_FUNC_ICGI_PROC )pstruProc->pfunProc;
	}
	
	return 0;
}

int HI_ICGI_DeRegister_Proc(HI_S_ICGI_Proc* pstruProc)
{
	if(!pstruProc)
		return -1;

	if( strcmp(pstruProc->pfunType, "onvif") == 0)
	{
		g_pfunOnvif = NULL;
	}
	else if( strcmp(pstruProc->pfunType, "cgi") == 0)
	{
		g_pfunCGIProc = NULL;
	}
	else if( strcmp(pstruProc->pfunType, "rtsp") == 0)
	{
		g_pfunRtspProc = NULL;
	}

	return 0;
}

void* HI_WEB_Msg_Printf(void* pDest, const char* fmt, va_list pstruArg)
{
    if(NULL == pDest)
    {
        return (void*)HI_ERR_SERVER_PARAM;
    }
    vsnprintf((char*)pDest + strlen((char*)pDest),
            MAX_CNT_CGI_OUT - strlen((char*)pDest), fmt, pstruArg);
    return (void*)0;
}

/* Copies and decodes a string.  It's ok for from and to to be the
** same string.
*/
void HI_CGI_strdecode( char* to, char* from )
{
    if(NULL == to || NULL == from)
        return;

    for ( ; *from != '\0'; ++to, ++from )
	{
	if ( from[0] == '%' && isxdigit( from[1] ) && isxdigit( from[2] ) )
	    {
	    *to = hexit( from[1] ) * 16 + hexit( from[2] );
	    from += 2;
	    }
	else if( (*from) == '+')
	{
        *to = ' ';
	}
    else
    {
	    *to = *from;
    }
	}
    *to = '\0';
}

void sendReflashToConn( httpd_conn* hc, char* pszUrlAddr)
{
  //  int u32Itmp;
    char aszSendBuf[2000];
    snprintf(aszSendBuf, sizeof(aszSendBuf), "<html><head><title></title>\
        <META http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\
        <META http-equiv=\"Refresh\" content=\"0;URL=%s\"></head><body></body></html>" ,pszUrlAddr);
    //ZQ 2009-02-02
    //add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/plain\r\n\r\n");
    add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n");
    add_response( hc, aszSendBuf );
    httpd_write_response( hc );
}
/*解析字符串到数组*/
int HI_WEB_RequestParse_stru(char* hi_pQueryIn, int* argc1, char** argv1, int maxCnt, int maxLen)
{
    char* pszQuery;
    char* pszAndTemp = NULL;
    char* pszEqualTemp = NULL;
    char* pszMinTemp = NULL;
    int s32Num = *argc1;
    int s32StrLen = 0;

    pszQuery = hi_pQueryIn;
    while( (NULL != pszQuery)&&(*pszQuery != '\0'))
    {
		pszAndTemp = strchr(pszQuery,'&');
        pszEqualTemp = strchr(pszQuery,'=');
        if((pszAndTemp != NULL)&&(pszEqualTemp != NULL))
        {
            if( strlen(pszAndTemp) > strlen(pszEqualTemp) )
            {
                pszMinTemp = pszAndTemp;
            }
            else
            {
                pszMinTemp = pszEqualTemp;
            }
        }
        else if((pszAndTemp != NULL))
        {
            pszMinTemp = pszAndTemp;
        }
        else if((pszEqualTemp != NULL))
        {
            pszMinTemp = pszEqualTemp;
        }
        else if((pszAndTemp == NULL)&&(pszAndTemp == NULL))
        {
            s32StrLen= MIN( strlen(pszQuery), maxLen-1 );
            //strncpy(argv1[s32Num],pszQuery,strlen(pszQuery));
            //*(argv1[s32Num]+strlen(pszQuery))='\0';
            strncpy(argv1[s32Num],pszQuery,s32StrLen);
            *(argv1[s32Num]+s32StrLen)='\0';
            s32Num++;
            break;
        }
        if((strlen(pszQuery)-strlen(pszMinTemp))>0)
        {
            //strncpy(argv1[s32Num],pszQuery,(strlen(pszQuery)-strlen(pszMinTemp)));
            //*(argv1[s32Num]+(strlen(pszQuery)-strlen(pszMinTemp)))='\0';
            s32StrLen= MIN((strlen(pszQuery)-strlen(pszMinTemp)), maxLen-1 );
			strncpy(argv1[s32Num],pszQuery,s32StrLen);
            *(argv1[s32Num]+ s32StrLen )='\0';
            s32Num++;
            if( s32Num >= maxCnt)
            {
                break;
            }
        }
        pszQuery = pszMinTemp+1;      
        pszAndTemp = NULL;
        pszEqualTemp = NULL;
        pszMinTemp = NULL;
    }
    (*argc1) = s32Num;
    return 1;
}

int ANV_CGI_DumpHttpConn(httpd_conn* hc )
{
	
	printf("========= Begin ANV CGI: Dump Http conn 0x%x======== \n", hc); 
	printf("ANV CGI: Dump Http conn initialized:%d\n", hc->initialized);
	printf("ANV CGI: Dump Http conn hs:0x%x\n", hc->hs);
	printf("ANV CGI: Dump Http conn encodedurl:%s\n", hc->encodedurl);
	printf("ANV CGI: Dump Http conn decodedurl:%s\n", hc->decodedurl);
	printf("ANV CGI: Dump Http conn origfilename:%s\n", hc->origfilename);
	printf("ANV CGI: Dump Http conn expnfilename:%s\n", hc->expnfilename);
	printf("ANV CGI: Dump Http conn encodings:%s\n", hc->encodings);
	printf("ANV CGI: Dump Http conn pathinfo:%s\n", hc->pathinfo);
	printf("ANV CGI: Dump Http conn query:%s\n", hc->query);

	
	printf("========= End  ANV CGI: Dump Http conn 0x%x======== \n", hc); 
	return 0;
}

int ANV_CGI_Proc( httpd_conn* hc )
{
	char* pCommand=NULL;
	char szCommand[MAX_CGI_INTER] = {0};
	int Rtn = 0;    
	int commandFlag = 0;
	
	if( NULL == hc->origfilename)
	{
		DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[Error]the origfile name is null \n");
		return 0;
	}
	
	strcpy(szCommand, hc->origfilename);
	
	if(0 == strncmp(hc->origfilename, "onvif", strlen("onvif")) ||!strncmp(hc->origfilename, "Subscription", strlen("Subscription")))
	{
		Rtn = 0;
		if(NULL != g_pfunOnvif)
		{
		   	Rtn = g_pfunOnvif(hc);
		}
		else
		{
		 	printf("<%s>(%d):g_pfunOnvif is NULL\n", __FILE__, __LINE__);
		}
        //Aobaozi add 2012-12-11 9:41:18 这里让thttpd直接关闭onvif的连接。

		if(Rtn<=0)
			return -1;
		
		return Rtn;
	}
	else if(0 == strncmp(hc->origfilename, "ROH/channel", strlen("ROH/channel")))
	{
		Rtn = 0;
		if(NULL != g_pfunRtspProc)
		{
		   	Rtn = g_pfunRtspProc(hc);
		}
		else
		{
		 	printf("<%s>(%d):g_pfunRtspProc is NULL\n", __FILE__, __LINE__);
		}
        //Aobaozi add 2012-12-11 9:41:18 这里让thttpd直接关闭onvif的连接。

		if(Rtn<=0)
			return -1;
		
		return Rtn;
	}
	else
	{
		printf("szCommand:%s hc->conn_fd:%d g_pfunCGIProc:%d\n",szCommand,hc->conn_fd,g_pfunCGIProc);
		if(NULL != g_pfunCGIProc)
		{
		   	Rtn = g_pfunCGIProc(hc);
		}
		else
		{
		 	printf("<%s>(%d):g_pfunCGIProc is NULL\n", __FILE__, __LINE__);
		}
		return Rtn;
	}
#if 0	
	else
	{
       printf("proc icgi\n");
        int i;
        //int ret; 
        unsigned int u32Handle = 0;
        int u32RtnNum = 0;
        int argc = 0;
        int u32Itmp2;
        char* argv[MAX_NUM_CMD]={0};
        char* pszAllMalloc = NULL;
        
        char pszCurrentUrlOrig[MAX_LEN_URL]={0};  
        char pszRequest[MAX_CNT_CMD]={0}; 
        char query[MAX_CNT_CGI]={0};
        char aszDest[MAX_CNT_CGI_OUT] = {0};
        char method[8] = {0};
        
        CGI_CMD_CmdProc    pFunCgiCmdProc     = NULL;
        
        pszAllMalloc = (char *)malloc(MAX_NUM_CMD*MAX_CNT_EVERYCMD*sizeof(char));
        if(pszAllMalloc == NULL)
        {
            DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[error] malloc the argv size is fail\n");
            return 0;
        }
        for(i=0;i<MAX_NUM_CMD;i++)
        {
            argv[i] = pszAllMalloc + (i*MAX_CNT_EVERYCMD);
        }  
 //       u32RtnNum = HI_OUT_Register(HI_WEB_Msg_Printf, aszDest, &u32Handle);

        for(i=0;i<MAX_LEN;i++)
        {
            if( 0 ==strcmp(pstru_icgi_cmd[i].cgi,szCommand))
            {
                commandFlag = 1;
                strncpy(pszRequest,pstru_icgi_cmd[i].cmd,strlen(pstru_icgi_cmd[i].cmd));
                pFunCgiCmdProc = pstru_icgi_cmd[i].cmdProc;
                strncpy(method,pstru_icgi_cmd[i].method,strlen(pstru_icgi_cmd[i].method));
                //printf("find success\n");
                break;
            }
        }

        if( 1 != commandFlag )
        {
            DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[error]invalide interface\n");
           // printf("invalide interface\n");
            goto result;
        }
        if( hc->method == METHOD_GET )
        {
            if( hc->query[0] != '\0' )
            {
                if( strlen(hc->query) >= MAX_CNT_CGI )
                {
                    *(hc->query + MAX_CNT_CGI) = '\0';
                }
                HI_CGI_strdecode( query, hc->query );
                strncat(pszRequest,query,strlen(query));   
            }
            strcpy(aszMethod,"GET");
            aszMethod[strlen("GET")]='\0';
        }
        else if(hc->method == METHOD_POST)
        {
            strcpy(aszMethod,"POST");
            aszMethod[strlen("POST")]='\0';
            int c,retnum,cpynum=0,r;
            char buf[MAX_CNT_CGI]={0};
            c=hc->read_idx - hc->checked_idx;
            //debug 存在字符串长度越界的漏洞,当hc->contentlength大于buf的大小时
            if(c>0)
            {
                strncpy(buf, &(hc->read_buf[hc->checked_idx]), MIN(c,MAX_CNT_CGI-1));  
                cpynum = c;
               // printf("the buf is %s \n",buf);
            }
            while ( ( c < hc->contentlength ) && ( c < MAX_CNT_CGI-1 ) )
            {
//                if(hc->ssl_flag)   
 //                   r = SSL_read(hc->ssl, buf+c, MIN( hc->contentlength - c, MAX_CNT_CGI-1 - c ));
 //               else
                	r = read( hc->conn_fd, buf+c, MIN( hc->contentlength - c, MAX_CNT_CGI-1 - c ));
            	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
            	{
            	    sleep( 1 );
            	    continue;
            	}
            	if ( r <= 0 )
            	{
                    break;
            	}
            	c += r;
            }
            //retnum = read(hc->conn_fd,buf+cpynum,hc->contentlength - c);
            retnum=strlen(buf);
            if(retnum>0)
            {
                //printf("the post content is %s\n",buf);
                HI_CGI_strdecode(query, buf);
                strncat(pszRequest,query,strlen(query));
            }
            else
            {
               // printf("have not the string in content\n");
            }
        }
        else
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERROR, "[error]invalide request method \n" );
            goto result;
        }
        DEBUG_CGI_PRINT("CGI",LOG_LEVEL_INFO,"[info]the request is %s %s",hc->query,query);
        *(pszRequest+strlen(pstru_icgi_cmd[i].cmd)+strlen(query))='\0';
        if(pszRequest[0]!='\0')
        {
            HI_LOG_ACCESS("[%s] %s %s %s\n",httpd_ntoa( &hc->client_addr ),aszMethod,szCommand,pszRequest);
           // HI_LOG_ACCESS("%s %s %s\n",hc->method,szCommand,pszRequest);
        }
        else  
        {
            HI_LOG_ACCESS("[%s] %s %s \n",httpd_ntoa( &hc->client_addr ),aszMethod,szCommand);
           // HI_LOG_ACCESS("%s %s \n",hc->method,szCommand);
        }
     //   printf("the request is %s \n",pszRequest);
        if(  pszRequest[0] != '\0' )
        {
            u32RtnNum = HI_WEB_RequestParse_stru( pszRequest, &argc, argv,MAX_NUM_CMD,MAX_CNT_EVERYCMD);
            if( u32RtnNum == 0 )
            {
                DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]request string parse fail \n" );
                goto result;
            }
        }
        if(0 == strcmp(method,CGI_METHOD_SET))
        {
            for( u32Itmp2 = 0; u32Itmp2< argc; u32Itmp2++ )
            {
                if( 0 == strcmp(argv[u32Itmp2], CGI_URL_PATH_FLAG ) )
                {
                    strncpy( pszCurrentUrlOrig, argv[u32Itmp2+1],
                             MIN( MAX_LEN_URL-1,strlen(argv[u32Itmp2+1])) );
                    *(pszCurrentUrlOrig+strlen(argv[u32Itmp2+1]))='\0';
                    for( i=u32Itmp2; i<argc; i++ )
                    {
                        argv[i] = argv[i+2];
                    }
                    argc = argc - 2;
                    break;
                }
            }
        }
        
        if(pFunCgiCmdProc != NULL)
        {
            u32RtnNum = pFunCgiCmdProc( u32Handle,argc,(const char **)argv);
            if( -1 == u32RtnNum)
            {
                DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]cmd proc is fail errorcode %d", u32RtnNum );
                //goto result;
            }
            else
            {
                u32RtnNum = 1;
            }
        }
        if( 0 == strcmp(method,CGI_METHOD_GET) )
        {
        	
        	#if 0
            #if  defined (HI_LINUX_SUPPORT_UCLIBC)
            send_mime(
                hc, 200, "OK", "", "", "text/html; charset=%s", (off_t) -1,
                hc->sb.st_mtime );
            #else
            send_mime(
                hc, 200, "OK", "", "", "text/html; charset=%s", (off_t) -1,
                hc->sb.st_mtim.tv_sec );
            #endif
            
          #endif
	    //ZQ 2009-02-02
	    //add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/plain\r\n\r\n");
	    add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n");
	    add_response( hc, aszDest );
            httpd_write_response( hc );
        }
        else if( 0 == strcmp(method,CGI_METHOD_SET) )
        {
            if( pszCurrentUrlOrig[0] != '\0' )
            {
                sendReflashToConn( hc, pszCurrentUrlOrig);
            }
            else
            {
              //  printf("have not the url address\n");
            }
        }
        else if( 0 == strcmp(method, CGI_METHOD_COMMAND ))
        {
            sendReflashToConn( hc, hc->referer);
        }
        DEBUG_CGI_PRINT("CGI", LOG_LEVEL_ERR, "[info]the request %s return is %s the method:%s \n",szCommand,aszDest,method);
      result:       
        aszDest[0] = '\0';
//        HI_OUT_Deregister(u32Handle);
        if(pszAllMalloc != NULL)
        {
            free(pszAllMalloc);
        }
        pszAllMalloc = NULL;
        return u32RtnNum; 
    }
#endif	
}

#if 0
int HI_CGI_ParamProc(httpd_conn* hc)
{
    char pszBuf[MAX_CNT_CGI];
    char* pszBegin;
    int ret=0;
    
    char aszDest[MAX_CNT_CGI_OUT] = {0};
    unsigned int u32Handle;
    char* pszCmd;
//    char* pszCommand;
    char* pszAnd;
    char aszQuery[MAX_CNT_CGI] = {0};
    char aszCommand[MAX_CGI_INTER] = {0};
    char aszUrl[MAX_LEN_URL];
    char aszMethod[16]={0};
    
    pszBuf[0]='\0';
    aszDest[0] = '\0';
    aszUrl[0] = '\0';
    aszQuery[0] = '\0';
    aszCommand[0] = '\0';

    /*step1 read the request string*/
    if( hc->method == METHOD_GET )
    {
        if( hc->query[0] != '\0' )
        {
            if( strlen(hc->query) >= MAX_CNT_CGI )
            {
                *(hc->query + MAX_CNT_CGI) = '\0';
                DEBUG_CGI_PRINT( "CGI", 1, "[error]the request query is too long \n" );
            }
            strncpy(pszBuf,hc->query,strlen(hc->query)); 
            pszBuf[strlen(hc->query)] = '\0';
        }
        strcpy(aszMethod,"GET");
        aszMethod[strlen("GET")]='\0';
     }
    else if(hc->method == METHOD_POST)
    {
        strcpy(aszMethod,"POST");
        aszMethod[strlen("POST")]='\0';
        int c,cpynum=0,r;
       // char buf[MAX_CNT_CGI]={0};
        c=hc->read_idx - hc->checked_idx;
        if(c>0)
        {
            strncpy(pszBuf, &(hc->read_buf[hc->checked_idx]), MIN(c,MAX_CNT_CGI-1));  
            cpynum = c;
           // printf("the buf is %s \n",buf);
        }
        if( hc->contentlength >=MAX_CNT_CGI )
        {
        //    printf("the request is too long %s %d\n",__FILE__,__LINE__);
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request content is too long\n" );
        }
        
        while ( ( c < hc->contentlength ) && ( c < MAX_CNT_CGI-1 ) )
        {
//            if(hc->ssl_flag)   
//                r = SSL_read(hc->ssl, pszBuf+c, MIN( hc->contentlength - c, MAX_CNT_CGI-1 - c ));
//            else                
            	r = read( hc->conn_fd, pszBuf+c, MIN( hc->contentlength - c, MAX_CNT_CGI-1 - c ));
            
        	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
        	{
        	    sleep( 1 );
        	    continue;
        	}
        	if ( r <= 0 )
        	{
                break;
        	}
        	c += r;
        }
        pszBuf[MIN(hc->contentlength,MAX_CNT_CGI-1)]='\0';
      //  printf("the buffer is %s\n",pszBuf);
    }
    else
    {
        DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]invalide request method \n" );
        ret = -1;
        goto result;
    }

    /*step2 decode the request */
    if( pszBuf[0] == '\0')
    {
        ret = -1;
        goto result;
    }
    HI_CGI_strdecode(pszBuf, pszBuf);
   // printf("the request buffer is %s ---\n",pszBuf);
//    ret = HI_OUT_Register(HI_WEB_Msg_Printf, aszDest, &u32Handle);

    /*step 4 recycle run every cmd*/
    pszBegin = pszBuf;
    while( *pszBegin != '\0' )
    {
        pszCmd = strstr( pszBegin,DIVIDE_CMD );
        
        /*error input not include the "cmd="*/
        if ( pszCmd == NULL )
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]invalide request cmd \n" );
            ret = -1;
            goto result;
        }
        
        pszCmd = pszCmd + strlen(DIVIDE_CMD);

        if( *pszCmd == '\0' )
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request cmd is null\n" );
            //ret = -1;
            goto result;
        }
        
        pszAnd = strchr( pszCmd, '&');
        
        
        /*the input is only "cmd=<value>"*/
        if( pszAnd == NULL )
        {
          //  printf("the input is cmd=%s",pszCmd);
            if( strlen(pszCmd) >= MAX_CGI_INTER )
            {
                DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"the cmd is too long\n");
                ret = -1;
                goto result;
            }
            strcpy( aszCommand, pszCmd );
            aszCommand[strlen(pszCmd)]='\0';
            aszQuery[0]='\0';
            HI_LOG_ACCESS("[%s] %s %s cmd=%s \n",httpd_ntoa( &hc->client_addr ),aszMethod,"param.cgi",aszCommand);

            {
                ret = HI_CGI_SignalProc(aszCommand, aszQuery, aszDest, (char*)aszUrl,u32Handle);
                if( ret < 0 )
                {
                  
                    DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request %s is fail\n",aszCommand);
                }
            }
            goto result;
        }
        /*the input is only "cmd=<value>&"*/
        if( *(pszAnd+1) == '\0' )
        {
            if( (strlen(pszCmd)-strlen(pszAnd)) >= MAX_CGI_INTER )
            {
                DEBUG_CGI_PRINT("CGI",1,"the cmd is too long\n");
                ret = -1;
                goto result;
            }
            strncpy( aszCommand, pszCmd, (strlen(pszCmd)-strlen(pszAnd)) );
            aszCommand[(strlen(pszCmd)-strlen(pszAnd))]='\0';
            aszQuery[0]='\0';
            HI_LOG_ACCESS("[%s] %s %s cmd=%s\n",httpd_ntoa( &hc->client_addr ),aszMethod,"param.cgi",aszCommand);

            {
                ret = HI_CGI_SignalProc(aszCommand, aszQuery, aszDest, aszUrl,u32Handle);
                if( ret < 0 )
                {
                    DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request %s is failed\n",aszCommand);
                }
            }
            goto result;
        }
        /*the input is "cmd=&dda"*/
        if( pszAnd == pszCmd )
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request cmd is null\n" );
           // ret = -1;
            pszBegin = pszAnd+1;
            continue;
        }
        
        /*the input is "cmd=<command>&-bps=1000..."*/
        if( (strlen(pszCmd)-strlen(pszAnd)) >= MAX_CGI_INTER )
        {
            DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"the cmd is too long\n");
            pszBegin = pszAnd+1;
            continue;
        }
        strncpy( aszCommand, pszCmd, (strlen(pszCmd)-strlen(pszAnd)) );
        aszCommand[(strlen(pszCmd)-strlen(pszAnd))]='\0';
        pszCmd = NULL;
        pszAnd ++;
        
        pszCmd = strstr( pszAnd, DIVIDE_CMD);
        /*the input is "cmd=<command>&-bps=1000"*/
        if( pszCmd ==NULL )
        {
            strncpy(aszQuery,pszAnd,MIN(strlen(pszAnd),MAX_CNT_CGI));
            aszQuery[MIN(strlen(pszAnd),MAX_CNT_CGI)] = '\0';
            *pszBegin = '\0';
        }
        /*the input is "cmd=<command>&-bps=1000&-cmd=<command2>..."*/
        else
        {
            strncpy(aszQuery,pszAnd,MIN((strlen(pszAnd)-strlen(pszCmd)),MAX_CNT_CGI));
            aszQuery[MIN((strlen(pszAnd)-strlen(pszCmd)),MAX_CNT_CGI)] = '\0';
            pszBegin = pszCmd;
        }  
        HI_LOG_ACCESS("[%s] %s %s cmd=%s %s \n",httpd_ntoa( &hc->client_addr ),aszMethod,"param.cgi",aszCommand,aszQuery);
        //HI_LOG_ACCESS("%s %s cmd=%s %s \n",hc->method,PARAM_INTERFACE,aszCommand,aszQuery);

         {
            ret = HI_CGI_SignalProc(aszCommand, aszQuery, aszDest, aszUrl,u32Handle);
            if( ret < 0 )
            {
               // printf("[error]the request %s is fail\n",aszCommand);
                DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]the request %s is fail\n",aszCommand);
            }   
        }
      //  printf("the result is %s \n",aszDest);
    }
    
result:

 //   DEBUG_CGI_PRINT( "CGI", 1, "[info]the request return is %s \n", aszDest);
    if( aszUrl[0] != '\0' )
    {
        sendReflashToConn( hc, aszUrl);
    }
    //else if(aszDest != '\0')
    if((aszDest != '\0')&&(hc->method == METHOD_GET))
    {
       // printf("\n send string is %s\n",aszDest);
    #if 0
        #if  defined (HI_LINUX_SUPPORT_UCLIBC)
        send_mime(
            hc, 200, "OK", "", "", "text/html; charset=%s", (off_t) -1,
            hc->sb.st_mtime );
        #else
        send_mime(
            hc, 200, "OK", "", "", "text/html; charset=%s", (off_t) -1,
            hc->sb.st_mtim.tv_sec );
        #endif
    #endif
	//ZQ 2009-02-02
	//add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/plain\r\n\r\n");
	add_response(hc, "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n");
        add_response( hc, aszDest );
        httpd_write_response( hc );
    }    
    
    aszDest[0] = '\0';
 //   HI_OUT_Deregister(u32Handle);
    return ret;
}
int HI_CGI_SignalProc(char* pszCommand, char* query, char* aszDest, char* pszUrl,unsigned int u32Handle)
{
    int ret = 0;

   // printf("WWWWWWWWWW signal proc %s  %s  WWWWWWWWWWWWWWWWWWWWWWWWW",pszCommand,query);
    DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_NOTICE, "[info]the request is %s %s \n", pszCommand,query);
    if(strlen(pszCommand)< (MAX_CGI_INTER-strlen(CGI_POSTFIX)))  
    {
        strcat(pszCommand,CGI_POSTFIX);
    }
    else
    {
        DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[error] the cmd is too long\n");
        ret = -1;
        goto result;
    } 
#if 0    
    if( 0 == strcmp(pszCommand,UPDATA_USERINFO_INTERFACE) )
    {
        ret = 0;
        if(NULL != g_pfunUserS)
        {
            ret = g_pfunUserS(pszCommand, query, aszDest, pszUrl, u32Handle);
        }
        else
        {
            printf("<%s>(%d):g_pfunUserS is NULL\n", __FILE__, __LINE__);
        }
        
        return ret;
    }
  #endif  
    int argc = 0;
    int u32Itmp2;
    char* argv[MAX_NUM_CMD]={0};
    char* pszAllMalloc = NULL;
    CGI_CMD_CmdProc    pFunCgiCmdProc     = NULL;
    
    int commandFlag = 0;
    int i;
    char pszRequest[MAX_CNT_CMD]={0}; 
    char method[8];
    
    pszAllMalloc = (char *)malloc(MAX_NUM_CMD*MAX_CNT_EVERYCMD*sizeof(char));
    if(pszAllMalloc == NULL)
    {
        DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[error] malloc the argv size is fail\n");
        return 0;
    }
    for(i=0;i<MAX_NUM_CMD;i++)
    {
        argv[i] = pszAllMalloc + (i*MAX_CNT_EVERYCMD);
    } 
    
   
    for(i=0;i<MAX_LEN;i++)
    {
        if( 0 ==strcmp(pstru_icgi_cmd[i].cgi,pszCommand))
        {
            commandFlag = 1;
            strncpy(pszRequest,pstru_icgi_cmd[i].cmd,strlen(pstru_icgi_cmd[i].cmd));
            pFunCgiCmdProc = pstru_icgi_cmd[i].cmdProc;
            strncpy(method,pstru_icgi_cmd[i].method,strlen(pstru_icgi_cmd[i].method));
            break;
        }
    }
    if( 1 != commandFlag )
    {
        DEBUG_CGI_PRINT("CGI",LOG_LEVEL_ERR,"[error]invalide interface\n");
        ret = -1;
        goto result;
    }

    /*通过icgi来调用脚本执行cgi*/
    if(0 == strcmp(method, CGI_METHOD_CGI))
    {
        ret = HI_WEB_RequestParse_stru( pszRequest, &argc, argv,MAX_NUM_CMD,MAX_CNT_EVERYCMD);
        if( ret == 0 )
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]request string parse fail \n" );
            goto result;
        }
        if((query[0]!='\0')&&( argc < MAX_NUM_CMD ))
        {
            //strncpy(argv[argc],query,MIN(MAX_CNT_EVERYCMD-1,strlen(query)));
            snprintf(argv[argc],MAX_CNT_EVERYCMD,"\"%s\"",query);
            //argv[argc][strlen()]='\0';
            argc++;
        }
        printf(" the cgi request is ");
        for(i=0;i<argc;i++)
        {
            printf(" %s ",argv[i]);
        }
       
        if(pFunCgiCmdProc != NULL)
        {
            ret = pFunCgiCmdProc( u32Handle,argc,(const char **)argv);
            if( -1 == ret)
            {
                DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]cmd proc is fail errorcode %d", ret );
                //goto result;
            }
            else
            {
                ret = 1;
            }
        }
        goto result;
    }

    /*单个的icgi请求的处理*/
    strncat(pszRequest,query,strlen(query)); 
    if(  pszRequest[0] != '\0' )
    {
        ret = HI_WEB_RequestParse_stru( pszRequest, &argc, argv,MAX_NUM_CMD,MAX_CNT_EVERYCMD);
        if( ret == 0 )
        {
            DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]request string parse fail \n" );
            goto result;
        }
    }
     
   // if(0 == strcmp(method,CGI_METHOD_SET))
    {
            for( u32Itmp2 = 0; u32Itmp2< argc; u32Itmp2++ )
            {
                if( 0 == strcmp(argv[u32Itmp2], CGI_URL_PATH_FLAG ) )
                {
                    strncpy( pszUrl, argv[u32Itmp2+1],
                             MIN( MAX_LEN_URL-1,strlen(argv[u32Itmp2+1])) );
                    *(pszUrl+strlen(argv[u32Itmp2+1]))='\0';
                    for( i=u32Itmp2; i<argc; i++ )
                    {
                        argv[i] = argv[i+2];
                    }
                    argc = argc - 2;
                    break;
                }
            }
        }
        if(pFunCgiCmdProc != NULL)
        {
            ret = pFunCgiCmdProc( u32Handle,argc,(const char **)argv);
            if( -1 == ret)
            {
                DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[error]cmd proc is fail errorcode %d", ret );
                //goto result;
            }
            else
            {
                ret = 1;
            }
          //  printf("the ret is %d %s\n",ret,aszDest);
        }
        if(*pszUrl!= '\0')
        {
         //   printf("the url is %s\n",pszUrl);
        }
result:
    if(aszDest != NULL)
    {
        DEBUG_CGI_PRINT( "CGI", LOG_LEVEL_ERR, "[info]the request %s return is %s \n", pszCommand,aszDest);
        //printf(" \n return %s \n",aszDest);
    }
    if(pszAllMalloc != NULL)
    {
        free(pszAllMalloc);
    }
    pszAllMalloc = NULL;
    return ret; 
    
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

