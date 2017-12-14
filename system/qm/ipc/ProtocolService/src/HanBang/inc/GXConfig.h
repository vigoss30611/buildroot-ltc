#ifndef GXCONFIG_H
#define GXCONFIG_H
#include <stdio.h>
#include <string.h>

#define GX_OPEN_DEBUG  1

#if GX_OPEN_DEBUG
#define GX_DEBUG(msg...) fprintf(stderr, "[ %s, %d ]=> ", __FILE__, __LINE__);printf(msg);printf("\n")
#define GX_ASSERT(exp...) assert(exp)
#else
#define GX_DEBUG(msg...)  (void)(msg)
#define GX_ASSERT(exp...) (void)(exp)
#endif



typedef enum tagGXWorkState
{
	kGXWorkInit,
	kGXWorkRun,
	kGXWorkClose,
	kGXWorkError,
	kGXWorkRelease,
}GXWORKSTATE;

enum tagGXErrorCode
{
	kGXErrorCodeOK = 0,
	kGXErrorCodeParam = 200,
	kGXErrorCodePacket,
	kGXErrorCodeSend,
	kGXErrorCodeSync,
	kGXErrorCodeMsgtype,
	kGXErrorCodeMalloc,
	kGXErrorCodeLogin,
	kGXErrorCodeRegister,
	kGXErrorCodeFail,
	kGXErrorCodeOther,
};


#endif //GXConfig_h
