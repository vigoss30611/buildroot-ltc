#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#ifdef SUPPORT_EMAIL_FUNC
 
#include "smtpc.h"
#include "debug.h"
//#include "commondatatype.h"
//#include "commonfunction.h"
//#include "snapjpgmanager.h"
//#include "commondefine.h"
#include "email.h"
//#include "TLNVSDK.h"
#include "QMAPINetSdk.h"
 

int MallocAndCopyString(char **to, char *from, int *pLen)
{
    if((to == NULL) || (from == NULL) || (pLen == NULL))
    {
        return -1;
    }
    if(strlen(from) == 0)
    {
        return -1;
    }
    if(*pLen < (int)(strlen(from) + 1))/*字符串结束标志*/
    {
        if(*to)
        {
            free(*to);
        }
        *pLen = strlen(from) + 1;
        *to = (char*)malloc(*pLen);
        if(*to == NULL)
        {
            *pLen = 0;
            return -1;
        }
    }
    strcpy(*to, from);

    return 0;
}

/*SMTP服务器*/
static char *pSmtpServerName = NULL;
static int       SmtpServerNameLen = 0;
/*收件人地址*/
static char *pRecipName = NULL;
static char *pRecipNameCc = NULL;
static char *pRecipNameBcc = NULL;
static int RecipNameLen = 0;
static int RecipNameCcLen = 0;
static int RecipNameBccLen = 0;
/*发件人邮箱地址及密码*/
static char *pSendName = NULL;
static int SendNameLen = 0;
static char *pSendPwd = NULL;
static int SendPwdLen = 0;
static char pSubject[256];
static char pText[256];
static SMTPC_CONF_LoginType_E FirstLoginType = AuthLogin;

int nEMailStatus = EMAIL_ST_NoRUN;

/*********************************************
1) 修改新界面支持抄送到第2个邮箱,
只要收件人1正确，抄送邮箱地址错
误不用退出
**********************************************/
int Hisi_SendEmail(QMAPI_NET_EMAIL_PARAM *lpEmail, char *lpData,int njpeglen,char *lpfilename,char *lsubject,char *lbody)
{
    SMTPC_INF_MailContent_s struMc;
    SMTPC_CONF_Configure_s  struConf;
    PTR_SMTPC_INF_MailContent_s mc  = &struMc;
    PTR_SMTPC_CONF_Configure_s conf = &struConf;

    

    int SendAuthTimes = 0;
    if(lpEmail->wServicePort == 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        return -1;
    }
    if(MallocAndCopyString(&pSmtpServerName, (char*)lpEmail->csSmtpServer,&SmtpServerNameLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        return -1;
    }
    if(MallocAndCopyString(&pRecipName, lpEmail->stToAddrList[0].csAddress,&RecipNameLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        return -1;
    }
    if(MallocAndCopyString(&pRecipNameCc, lpEmail->stCcAddrList[0].csAddress,&RecipNameCcLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        //return -1;
    }
	if(MallocAndCopyString(&pRecipNameBcc, lpEmail->stBccAddrList[0].csAddress,&RecipNameBccLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        //return -1;
    }	
    if(MallocAndCopyString(&pSendName, lpEmail->stSendAddrList.csAddress,&SendNameLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        return -1;
    }
    if(MallocAndCopyString(&pSendPwd, lpEmail->csEmailPass,&SendPwdLen) != 0)
    {
        printf("%s %d Param Error\n",__FUNCTION__,__LINE__);
        nEMailStatus = EMAIL_ST_ParamError;
        return -1;
    }

	memset(pSubject, 0, sizeof(pSubject));
	memset(pText, 0, sizeof(pText));
	memcpy(pSubject, lsubject, 256);
    memcpy(pText, lbody, 256);

    /*附件*/
    char pAttachment[512];
    memset(pAttachment,0,512);
    if(lpfilename)
    {
    	strcat(pAttachment, "/");
        strcat(pAttachment, lpfilename);
        strcat(pAttachment, ";");
    }

    if(pAttachment[0])
    {
        pAttachment[strlen(pAttachment) - 1] = '\0';
    }
    memset(&struMc, 0, sizeof(struMc));
    memset(&struConf, 0, sizeof(struConf));
    mc->From = pSendName;
    mc->To = pRecipName;
    mc->Text = pText;
    mc->Subject = pSubject;
	if(lpData != NULL && njpeglen !=0)
	{
    	mc->Attachment = pAttachment;
	}
	else
	{
		mc->Attachment = NULL;
	}
    mc->Cc = pRecipNameCc;
    mc->Bc = pRecipNameBcc;		
    mc->Charset = NULL;

    conf->Server = pSmtpServerName;
    conf->Port = lpEmail->wServicePort;
    conf->LoginType = AuthLogin;

    if(lpEmail->wEncryptionType)
    {
        DEBUG_INFO("SendEmail have ssl\n");
        conf->SslType = SSL_TYPE_STARTTLS;
    }
    else
    {
        DEBUG_INFO("SendEmail no ssl\n");
        conf->SslType = SSL_TYPE_NOSSL;
    }
    conf->User      = pSendName;
    conf->Passwd    = pSendPwd;
    conf->RespTimeout = 15;
    conf->ConnTimeout = 3;
SEND_AGAIN:
    if(SendAuthTimes == 0)
        nEMailStatus = EMAIL_ST_FirstSend;
    else if(SendAuthTimes == 1)
        nEMailStatus = EMAIL_ST_SecondSend;
    else
        nEMailStatus = EMAIL_ST_ThirdSend;
    SendAuthTimes ++;/*表示已经发送过的次数*/
    conf->LoginType = FirstLoginType;/*登陆验证方式*/
    if(HI_SMTPC_SendMail(mc, conf, lpData, njpeglen) != 0)
    {
        /*发送失败，那么换一个验证方式继续发送*/
        switch(FirstLoginType)
        {
        case AuthLogin:
            FirstLoginType = AuthLoginPlain;
            nEMailStatus = EMAIL_ST_LoginFail;
            DEBUG_INFO("USE AUTHLOGIN SEND EMAIL FAILED\n");
            break;
        case AuthLoginPlain:
            FirstLoginType = CramLogin;
            nEMailStatus = EMAIL_ST_LoginFail;
            DEBUG_INFO("USE AUTHLOGINPLAIN SEND EMAIL FAILED\n");
            break;
        case CramLogin:
        default:
            FirstLoginType = AuthLogin;
            nEMailStatus = EMAIL_ST_LoginFail;
            DEBUG_INFO("USE CRAMLOGIN SEND EMAIL FAILED\n");
            break;
        }
        if(SendAuthTimes < 3)
        {
            goto SEND_AGAIN;
        }
        return -1;
    }
	
    printf("%s %d SendMail Success Use %d Mode\n",__FUNCTION__,__LINE__,FirstLoginType);
    nEMailStatus = EMAIL_ST_SendSuccess;
    return 0;
} 

int nSleepTime = 0;

int SRDK_GetEmailStatus(int *lpSleepTime)
{
    if(lpSleepTime)
        *lpSleepTime = nSleepTime;
    return nEMailStatus;
} 

void SRDK_ResetEmailSleepTime()
{
    nSleepTime = 0;
}



