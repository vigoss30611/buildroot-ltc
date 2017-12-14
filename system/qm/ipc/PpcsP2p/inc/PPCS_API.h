#ifndef _PPCS_API___INC_H_
#define _PPCS_API___INC_H_

#define LINUX

#ifdef WIN32DLL
#ifdef PPPP_API_EXPORTS
#define PPPP_API_API __declspec(dllexport)
#else
#define PPPP_API_API __declspec(dllimport)
#endif
#endif //// #ifdef WIN32DLL

#ifdef LINUX
#include <netinet/in.h>
#define PPPP_API_API 
#endif //// #ifdef LINUX

#ifdef _ARC_COMPILER
#include "net_api.h"
#define PPPP_API_API 
#endif //// #ifdef _ARC_COMPILER

#include "PPCS_Type.h"
#include "PPCS_Error.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct{	
	CHAR_8 bFlagInternet;		// Internet Reachable? 1: YES, 0: NO
	CHAR_8 bFlagHostResolved;	// P2P Server IP resolved? 1: YES, 0: NO
	CHAR_8 bFlagServerHello;	// P2P Server Hello? 1: YES, 0: NO
	CHAR_8 NAT_Type;			// NAT type, 0: Unknow, 1: IP-Restricted Cone type,   2: Port-Restricted Cone type, 3: Symmetric 
	CHAR_8 MyLanIP[16];		// My LAN IP. If (bFlagInternet==0) || (bFlagHostResolved==0) || (bFlagServerHello==0), MyLanIP will be "0.0.0.0"
	CHAR_8 MyWanIP[16];		// My Wan IP. If (bFlagInternet==0) || (bFlagHostResolved==0) || (bFlagServerHello==0), MyWanIP will be "0.0.0.0"
} st_PPCS_NetInfo;

typedef struct{	
	int  Skt;					// Sockfd
	struct sockaddr_in RemoteAddr;	// Remote IP:Port
	struct sockaddr_in MyLocalAddr;	// My Local IP:Port
	struct sockaddr_in MyWanAddr;	// My Wan IP:Port
	UINT_32 ConnectTime;				// Connection build in ? Sec Before
	CHAR_8 DID[24];					// Device ID
	CHAR_8 bCorD;						// I am Client or Device, 0: Client, 1: Device
	CHAR_8 bMode;						// Connection Mode: 0: P2P, 1:Relay Mode
	CHAR_8 Reserved[2];				
} st_PPCS_Session;

PPPP_API_API UINT_32 PPCS_GetAPIVersion(void);
PPPP_API_API INT_32 PPCS_QueryDID(const CHAR_8* DeviceName, CHAR_8* DID, INT_32 DIDBufSize);
PPPP_API_API INT_32 PPCS_Initialize(CHAR_8 *Parameter);
PPPP_API_API INT_32 PPCS_DeInitialize(void);
PPPP_API_API INT_32 PPCS_NetworkDetect(st_PPCS_NetInfo *NetInfo, UINT_16 UDP_Port);
PPPP_API_API INT_32 PPCS_NetworkDetectByServer(st_PPCS_NetInfo *NetInfo, UINT_16 UDP_Port, CHAR_8 *ServerString);
PPPP_API_API INT_32 PPCS_Share_Bandwidth(CHAR_8 bOnOff);
PPPP_API_API INT_32 PPCS_Listen(const CHAR_8 *MyID, const UINT_32 TimeOut_Sec, UINT_16 UDP_Port, CHAR_8 bEnableInternet, const CHAR_8* APILicense);
PPPP_API_API INT_32 PPCS_Listen_Break(void);
PPPP_API_API INT_32 PPCS_LoginStatus_Check(CHAR_8* bLoginStatus);
PPPP_API_API INT_32 PPCS_Connect(const CHAR_8 *TargetID, CHAR_8 bEnableLanSearch, UINT_16 UDP_Port);
PPPP_API_API INT_32 PPCS_ConnectByServer(const CHAR_8 *TargetID, CHAR_8 bEnableLanSearch, UINT_16 UDP_Port, CHAR_8 *ServerString);
PPPP_API_API INT_32 PPCS_Connect_Break(void);
PPPP_API_API INT_32 PPCS_Check(INT_32 SessionHandle, st_PPCS_Session *SInfo);
PPPP_API_API INT_32 PPCS_Close(INT_32 SessionHandle);
PPPP_API_API INT_32 PPCS_ForceClose(INT_32 SessionHandle);
PPPP_API_API INT_32 PPCS_Write(INT_32 SessionHandle, UCHAR_8 Channel, CHAR_8 *DataBuf, INT_32 DataSizeToWrite);
PPPP_API_API INT_32 PPCS_Read(INT_32 SessionHandle, UCHAR_8 Channel, CHAR_8 *DataBuf, INT_32 *DataSize, UINT_32 TimeOut_ms);
PPPP_API_API INT_32 PPCS_Check_Buffer(INT_32 SessionHandle, UCHAR_8 Channel, UINT_32 *WriteSize, UINT_32 *ReadSize);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif ////#ifndef _PPCS_API___INC_H_