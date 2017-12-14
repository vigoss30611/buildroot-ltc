#ifndef __EMAIL_H__
#define __EMAIL_H__

#include "QMAPINetSdk.h"
void StartSendEmailThread();

//#ifdef SUPPORT_EMAIL_FUNC

#define EMAIL_ST_NoRUN          0x0100
#define EMAIL_ST_NoData         0x0101
#define EMAIL_ST_ParamError     0x0102
#define EMAIL_ST_FirstSend      0x0103
#define EMAIL_ST_SecondSend     0x0104
#define EMAIL_ST_ThirdSend      0x0105
#define EMAIL_ST_LoginFail      0x0106
#define EMAIL_ST_SendSuccess    0x0107

int SRDK_GetEmailStatus(int *lpSleepTime);
void SRDK_ResetEmailSleepTime();

int Hisi_SendEmail(QMAPI_NET_EMAIL_PARAM *lpEmail, char *lpData,int njpeglen,char *lpfilename,char *lsubject,char *lbody);


//#endif

#endif/*__EMAIL_H__*/
