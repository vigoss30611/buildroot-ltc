
#ifndef __TL_SISSION_H__
#define __TL_SISSION_H__

///Connect Status Right
typedef struct _TL_RECORD_LOGIN_INFO
{
    DWORD       dwUserRight; ///User Login right
    DWORD       dwNetPreviewRight; ///User Network Set right    
    DWORD       LoginID;    ///Ready Login IP Addr
    struct _TL_RECORD_LOGIN_INFO    *pNext;
}TL_RECORD_LOGIN_INFO , *PTL_RECORD_LOGIN_INFO;

///Record already login pthread mutex 
pthread_mutex_t     g_tl_record_login_mutex;
///Record already login User Right 
PTL_RECORD_LOGIN_INFO   g_record_login_info;

unsigned long TL_IpValueFromString(const char *pIp);

int  TL_Video_CheckUser(const char *pUserName,const char *pPsw , const int pUserIp ,  const int pSocketID , const char *pUserMac , int *UserRight , int *NetPreviewRight);

///TL Video New Protocol Server Information Callback
int TL_Server_Info_func(char *pbuffer);

///TL Video New Protocol Channel Information Callback
int TL_Channel_Info_func(char *pbuffer , int TL_Channel);

///TL Video New Protocol Sensor Information Callback
int TL_Sensor_Info_func(char *pbuffer , int TL_Channel,int SensorType);

///TL Video Add New Protocol Login Right Cut
int TL_Client_Login_Right_Cut(DWORD UserID);

///TL Video New Protocol Open Channel Right Check
int TL_User_Open_Data_Channel_Check_Func(int nOpenChannel , int SocketID);

int ReceiveFun(MSG_HEAD *pMsgHead, char *pRecvBuf);

#endif