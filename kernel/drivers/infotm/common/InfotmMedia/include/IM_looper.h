/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
** v4.1.1   leo@2014/03/10: remove the no-wait function in request() calling, 
**                      if support this feature, it would break the rule that
**                      request() not allow re-enter before has been replied
**                      completely. meanwhile, set the default wait to false in
**                      run().
**
*****************************************************************************/ 

#ifndef __IM_LOOPER_H__
#define __IM_LOOPER_H__

/**
 * This looper server as a interact platform between client and server,
 * client and server execute in different thread, and client may be not
 * int one thread. a complete seesion include client send a request and
 * server reply this request, it cannot breaken in meanwhile.
 */
class IM_Looper
{
public:
	IM_Looper();
	virtual ~IM_Looper();

	// [31:16] command flag, [15:0] command code
	#define CMD_NONE	0
	#define CMD_PAUSE	1
	#define CMD_EXIT	2
	#define CMD_RUN		3
	#define CMD_USER	0x1000	// user code range [0xFFFF, 0x1000]

	IM_RET create();
	IM_RET pause(IN IM_INT32 timeout=-1);
	IM_RET exit(IN IM_INT32 timeout=-1);
	IM_RET run(IN IM_BOOL wait=IM_TRUE/*MUST BE TRUE*/);	// if wait is IM_FALSE, no need reply.
	IM_RET request(IN IM_INT32 cmd, IN void *param, IN IM_BOOL wait=IM_TRUE/*MUST BE TRUE*/, IN IM_INT32 timeout=-1, IN IM_BOOL stall=IM_TRUE);

protected:
	virtual void *threadLoop();
	virtual IM_RET doRequest(IN IM_INT32 cmd, IN void *param){ return IM_RET_OK; }
	virtual IM_RET loopRoutine() = 0;
	virtual IM_RET onInit(){ return IM_RET_OK; }
	virtual IM_RET onExit(){ return IM_RET_OK; }
	virtual IM_RET setThreadPriority(IM_INT32 prio, IM_INT32 *oldPrio=IM_NULL){
		if(mThread){
			return mThread->setPriority(prio, oldPrio);
		}
		return IM_RET_FAILED;
	}

private:
	// command flag
	#define CMDFLAG_NOREPLY		0x1
	#define CMDFLAG_STALL		0x2	

	#define PACK_CMD(cmd, code, flag)	cmd = (code & 0xFFFF) | (flag << 16);
	#define UNPACK_CMD(cmd, code, flag)	code = cmd & 0xFFFF; flag = cmd >> 16;

	IM_Thread *mThread;
	IM_Signal *mRequestSignal;
	IM_Signal *mReplySignal;
	IM_Signal *mInitSignal;

	IM_Lock mSerialAccess;
	IM_INT32 mCmd;
	void *mParam;
	IM_RET mResult;

	IM_BOOL mInit;

	IM_INT32 getRequest(IM_BOOL waitUntilNewCmd);
	void reply(IM_RET result);
#if (TARGET_SYSTEM == FS_ANDROID)
	static void *threadWrapper(void *me);
#elif (TARGET_SYSTEM == FS_WINCE)
	static IM_INT32 threadWrapper(void *me);
#endif	
};

//
//
//
#include <IM_looper_c.h>


#endif	// __IM_LOOPER_H__
