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
** v4.1.1   leo@2014/03/10: in IM_Looper::request(), fixed the "wait" to ture,
**                      it must be ture, because if not it could break the rule
**                      that request not allow re-enter before replied.
**
*****************************************************************************/ 

#include <InfotmMedia.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"LP_I:"
#define WARNHEAD	"LP_W:"
#define ERRHEAD		"LP_E:"
#define TIPHEAD		"LP_T:"


IM_Looper::IM_Looper() :
	mThread(IM_NULL),
	mRequestSignal(IM_NULL),
	mReplySignal(IM_NULL),
	mInitSignal(IM_NULL),
	mInit(IM_FALSE)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	mRequestSignal = new IM_Signal();
	mReplySignal = new IM_Signal();
	mInitSignal = new IM_Signal();
	mThread = new IM_Thread();
	IM_ASSERT(mRequestSignal);
	IM_ASSERT(mReplySignal);
	IM_ASSERT(mInitSignal);
	IM_ASSERT(mThread);
}

IM_Looper::~IM_Looper()
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_Looper::exit();
	IM_ASSERT(mInit == IM_FALSE);
	IM_SAFE_DELETE(mRequestSignal);
	IM_SAFE_DELETE(mReplySignal);
	IM_SAFE_DELETE(mInitSignal);
	IM_SAFE_DELETE(mThread);
}

IM_RET IM_Looper::create()
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(mInitSignal->reset() == IM_RET_OK);
	IM_RET ret = mThread->create((IM_Thread::THREAD_ENTRY)IM_Looper::threadWrapper, this);
	if(ret != IM_RET_OK){
		IM_ERRMSG((IM_STR("mThread->create() failed, ret=%d"), ret));
		return ret;
	}
	IM_ASSERT(mInitSignal->wait(IM_NULL) == IM_RET_OK);	// threadLoop() will set this signal, before calling onInit().
	return IM_RET_OK;
}

IM_RET IM_Looper::request(IM_INT32 cmd, void *param, IM_BOOL wait, IM_INT32 timeout, IM_BOOL stall)
{
	IM_INFOMSG((IM_STR("%s(cmd=%d, wait=%d, stall=%d)"), IM_STR(_IM_FUNC_), cmd, wait, stall));
	IM_RET ret;
	IM_INT32 flag = 0;

    // force "wait" to true to keep the rule that one command must returned before it has been executed.
    wait = IM_TRUE;

	IM_AutoLock autolck(&mSerialAccess);
	if(mInit == IM_FALSE){
		IM_ERRMSG((IM_STR("looper has not been inited")));
		return IM_RET_FAILED;
	}

	if(wait == IM_FALSE){
		flag |= CMDFLAG_NOREPLY;	// when command don't  need reply, param is ignored.
	}
	if(stall == IM_TRUE){
		flag |= CMDFLAG_STALL;
	}
	mParam = param;
	PACK_CMD(mCmd, cmd, flag);

	IM_ASSERT(IM_RET_OK == mReplySignal->reset());
	IM_ASSERT(IM_RET_OK == mRequestSignal->set());
	if(wait == IM_FALSE){
		return IM_RET_OK;
	}
	ret = mReplySignal->wait(/*&mSerialAccess*/IM_NULL, timeout);
	if(ret != IM_RET_OK){
		IM_ERRMSG((IM_STR("wait reply failed, ret=%d"), ret));
		IM_ASSERT(IM_RET_OK == mRequestSignal->reset());
		return ret;
	}
	IM_INFOMSG((IM_STR("%s(cmd=%d, result=%d)"), IM_STR(_IM_FUNC_), cmd, mResult));
	return mResult;
}

IM_RET IM_Looper::pause(IM_INT32 timeout)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return request(CMD_PAUSE, IM_NULL, IM_TRUE, timeout);
}

IM_RET IM_Looper::exit(IM_INT32 timeout)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	{
		IM_AutoLock autolck(&mSerialAccess);
		if(mInit == IM_FALSE){
			return IM_RET_OK;
		}
	}

	IM_ASSERT(mInitSignal->reset() == IM_RET_OK);
	if(IM_RET_OK != request(CMD_EXIT, IM_NULL, IM_TRUE, timeout)){
		IM_ERRMSG((IM_STR("request(exit) failed, so kill this thread")));
		mThread->forceTerminate();
		IM_AutoLock autolck(&mSerialAccess);
		onExit();
		mInit = IM_FALSE;
		return IM_RET_OK;
	}
	IM_ASSERT(mInitSignal->wait(IM_NULL) == IM_RET_OK);
	return IM_RET_OK;
}

IM_RET IM_Looper::run(IM_BOOL wait)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return request(CMD_RUN, IM_NULL, wait, -1, IM_FALSE);
}

void IM_Looper::reply(IM_RET result)
{
	IM_INFOMSG((IM_STR("%s(%d)"), IM_STR(_IM_FUNC_), result));
	//IM_AutoLock autolck(&mSerialAccess);
	mResult = result;
	IM_ASSERT(IM_RET_OK == mReplySignal->set());
}

IM_INT32 IM_Looper::getRequest(IM_BOOL waitUntilNewCmd)
{
	IM_INFOMSG((IM_STR("%s(%d)"), IM_STR(_IM_FUNC_), waitUntilNewCmd));
	//IM_AutoLock autolck(&mSerialAccess);
	if(IM_RET_OK == mRequestSignal->wait(/*&mSerialAccess*/IM_NULL, (waitUntilNewCmd==IM_TRUE)?-1:0)){
		IM_INFOMSG((IM_STR("get a request %d"), mCmd));
		IM_INT32 cmd = mCmd;
		mCmd = CMD_NONE;
		return cmd;
	}

	return CMD_NONE;
}

#if (TARGET_SYSTEM == FS_ANDROID)
void *IM_Looper::threadWrapper(void *me)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return ((IM_Looper *)me)->threadLoop();
}
#elif (TARGET_SYSTEM == FS_WINCE)
IM_INT32 IM_Looper::threadWrapper(void *me)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	((IM_Looper *)me)->threadLoop();
	return 0;
}
#endif

void *IM_Looper::threadLoop()
{
	IM_INFOMSG((IM_STR("%s()++"), IM_STR(_IM_FUNC_)));
	IM_INT32 cmd = CMD_NONE;
	IM_INT32 code = CMD_NONE;
	IM_INT32 flag = 0;
	IM_RET result = 0;
	IM_BOOL waitUntilNewCmd = IM_TRUE;

	{
		IM_AutoLock autolck(&mSerialAccess);
		IM_ASSERT(IM_RET_OK == mInitSignal->set());
		if(IM_RET_OK != onInit()){
			IM_ERRMSG((IM_STR("onInit() failed")));
			return IM_NULL;
		}
		mInit = IM_TRUE;
	}

	do{
		cmd = getRequest(waitUntilNewCmd);
		UNPACK_CMD(cmd, code, flag);
		IM_INFOMSG((IM_STR("cmd=%d, flag=0x%x"), code, flag));

		if((code != CMD_NONE) && (code != CMD_RUN)){// actual command
			result = doRequest(code, mParam);
			if(!(flag & CMDFLAG_NOREPLY)){
				reply(result);
			}
			waitUntilNewCmd = (flag & CMDFLAG_STALL)?IM_TRUE:waitUntilNewCmd;
		}else{
			result = loopRoutine();
			if(result == IM_RET_RELAX){
				waitUntilNewCmd = IM_TRUE;
			}else if(result != IM_RET_OK){
				IM_ERRMSG((IM_STR("loopRoutine() failed, result=%d"), result));
				//break;
				waitUntilNewCmd = IM_TRUE;
			}else{
				waitUntilNewCmd = IM_FALSE;
				//IM_Sleep(0);
			}

			if((code == CMD_RUN) && !(flag & CMDFLAG_NOREPLY)){
				reply(IM_RET_OK);
			}
		}
	}while(code != CMD_EXIT);

	IM_AutoLock autolck(&mSerialAccess);
	onExit();
	mInit = IM_FALSE;
	IM_ASSERT(IM_RET_OK == mInitSignal->set());
	IM_INFOMSG((IM_STR("%s()--"), IM_STR(_IM_FUNC_)));

	return IM_NULL;
}


#include "looper_c.c"



