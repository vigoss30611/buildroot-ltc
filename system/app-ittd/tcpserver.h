//******************************************************************************/
//* File: tcpserver.h                                                          */
//******************************************************************************/

#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include <pthreadcc.h>
#include <socketcc.h>
#include <vector>
#include <qsdk/tcpcommand.h>

class TCPServiceThread;

//******************************************************************************/
//* TCPServer Class.                                                           */
//******************************************************************************/
class TCPServer : public ThreadBase
{
protected:
    	MutualExclusion  	m_mutexAccept;
        TCPServerSocket     *m_pcListeningSocket;

        Condition           m_condWakeThread;

        bool                m_bServerTerminate;
        int                 m_iThreadCount;
        int 				m_iMaxThreads;


        virtual void *      Initialise();
        virtual void *      Execute();
        TCPSocket*         	AcceptClient();

public:
                            TCPServer(int iPortNumber, int iBackLog, int iMaxThreads);
                            ~TCPServer();
		void                Start();
		void 				Stop();
        void	 			DeleteClient(TCPServiceThread* pcServiceThread);
        void 				DumpClient();
};

//******************************************************************************/
//* TCPServiceThread Class.                                                    */
//******************************************************************************/
class TCPServiceThread : public ThreadBase
{
protected:
        TCPServer*          m_pcServer;
        TCPSocket*          m_pcClientSocket;
        bool                m_bServiceTerminate;
        int                 m_iThreadID;
        TCPCmdData          m_CmdData;
        bool                m_bIttdDebugInfoEnable;
        bool                m_bH26XDebugInfoEnable;
        VIDEOBOX_CTRL       m_stVideoboxCtrl;
        RE_MOUNT_DEV_CTRL   m_stReMountDevCtrl;

        virtual void *      Initialise();
        virtual void *      Execute();
        virtual void        CleanUp();

public:
        enum KillThreadExceptions { errFatalError, errTooManyThreads, errClientError, errServerDown };

                            TCPServiceThread(TCPSocket* pcClientSocket, int iThreadID, TCPServer* pcServer);
        virtual             ~TCPServiceThread();
        int                 GetThreadID() { return m_iThreadID; }
        TCPSocket*          GetSocket() { return m_pcClientSocket; }
        void                ServiceClient();
        void                Start();
        void                Stop();

        int                 SendCommand(unsigned int nMod, unsigned int nDir, unsigned int nType, unsigned int uiDataSize, UTLONGLONG utVal);
        int                 SendDataCommand(TCPCommand* pCmd, void* pData, unsigned int uiDataSize);
        int                 ReceiveFile(TCPCmdData* pTcp, char* pszFilename);
        int                 SendFile(unsigned int nMod, char* pszFilename, void* pData, unsigned int uiDataSize);
        void                DumpCommandData(TCPCmdData* pTcp, unsigned int uiSize);
        void                ParseCommand(TCPCmdData* pTcp, unsigned int uiSize);
        void                ParseGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize);
        void                ParseIspCommand(TCPCmdData* pTcp, unsigned int uiSize);
        void                ParseIspostCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 GetIspostCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 SetIspostCommand(TCPCmdData* pTcp, unsigned int uiSize);
        void                ParseH26XCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 GetH26XCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 SetH26XCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 GetGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize);
        int                 SetGeneralCommand(TCPCmdData* pTcp, unsigned int uiSize);
};

#endif //__TCPSERVER_H__


