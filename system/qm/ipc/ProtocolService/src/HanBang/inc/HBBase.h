#ifndef HB_BASE_H
#define HB_BASE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "GXThread.h"

#define HB_SERVER_PORT			16923
#define HB_AGENT_PORT			18000		//HB default port

#define HB_MAX_USER_NUM			4			//8
#define HB_MAX_AVIDEO_NUM		2

#define HB_MSG_HEADER_LENGTH	16
#define HB_FROM_MSG_MAX_LENGTH	(2*2048)
#define HB_TO_MSG_MAX_LENGTH	(2*2048)

	typedef struct tagHBNetAVideo{
		int				handleID;
		int				videoFd;			//ipc read video stream handle
		int				channelNum;
		int				streamType;			//mainStream, subStream
		int				needIFrame;
		int				frameTickcount;

		int				mediaSocketFd;

		int				runState;
		pthread_t		threadId;
		int				token;				// 1 used      0 not used
	}HBNetAVideo;

	typedef struct tagHBCliecntSession{
		int				handleID;
		int				userID;
		unsigned long   md5Code;
		int				channelNum;

		int				swapSocketfd;				//swap		
		HBNetAVideo		netStreams[HB_MAX_AVIDEO_NUM];

		int				clientFd;			//socketFD
		unsigned int	ipAddr;
		unsigned short	port;

		int				runState;
		pthread_t		threadId;
		int				token;				// 1 used      0 not used
	}HBCliecntSession;

	// test using swap
	typedef struct tagHBClientInfo{
		int					handleID;
		int					clientFd;			
		unsigned int		clientIpAddr;
		unsigned short		clientPort;
		pthread_t			clientThreadId;

		void				*dl_handle;
	}HBClientInfo;

	typedef struct tagHBServiceSession{
		int					handleID;		
		HBCliecntSession	clientSessions[HB_MAX_USER_NUM];
		int					socketfd;
		int					runState;
		pthread_t			threadId;
	}HBServiceSession;


	// userAgent
	typedef struct tagHBUserServerInfo{
		int					enable;
		int					changed;
		char				serverIpaddr[16];
		unsigned short		serverPort;
		char				devSerialNumber[48];
	}HBUserServerInfo;

	typedef struct tagHBUserAgentSession{		// 同时且支持一个服务端
		HBCliecntSession	session;			
		HBUserServerInfo	userAgentInfo;
		int					runState;
		pthread_t			threadId;
		int					runStateCheck;
		pthread_t			threadIdCheck;
	}HBUserAgentSession;


#ifdef __cplusplus
}
#endif
#endif