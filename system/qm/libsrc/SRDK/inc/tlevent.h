/******************************************************************************
******************************************************************************/

#ifndef _NVSEVENT_H
#define _TLVIDEOEVENT_H

//#include "etilib/etiLocal.h"
//#include "etilib/etiTypes.h"
//#include "etiPlayerEvent.h"


#include "QMAPIType.h"
 
typedef struct playerEvent *nvsEvent_t;

typedef enum 
{

    NVSpepNone     = 0,    // Priority not specified.
    NVSpepHighest,         // Highest priority.
    NVSpepHigh,            // High priority.
    NVSpepLow,             // Low priority.
    NVSpepLowest,          // Lowest Priority.

} nvsEventPriority_t;

typedef enum 
{

    NVSPLAYEREVENT_OK,                     // No error, operation succeeded.
    NVSPLAYEREVENT_NOT_OPEN,               // API function on invalid instance handle.
    NVSPLAYEREVENT_INVALID_PARM,           // Invalid parameter passed to API function.
    NVSPLAYEREVENT_INSUFFICIENT_MEMORY,    // Failed to allocate memory.
    NVSPLAYEREVENT_INVALID_EVENT_TYPE,     // Event type is not valid.
    NVSPLAYEREVENT_EVENT_NOT_CREATED,      // Event has not been created.
    NVSPLAYEREVENT_NO_SETUP,               // Setup has not been done yet.
    NVSPLAYEREVENT_OSAL_ERROR,             // OSAL returned an error.
    NVSPLAYEREVENT_BSPMAP_ERROR,           // BSP MAP library returned an error.
    NVSPLAYEREVENT_EVENT_EXISTS,           // The event already exists.

} nvsEventError_t;

typedef struct 
{
    nvsEventPriority_t        priority;           // Priority for the event handler.
    UInt                      maxNrofEvents;      // Maximum number of events.
    UInt                      maxNrofDataEvents;  // Maximum number of events with associated data.
} nvsEventSetup_t;

typedef enum
{
	None						= 6000,        		// No event.
	setAlarm						= None+1,        	// alarm process.
    setAudiotalk					= None+2,        	// audio talk process.
    setTty						= None+3,        	// set yuntai with tty.
    setCodec						= None+4,         	//user set encoder config.
    setPPPoe						= None+5,
    setSysNet						= None+6,
    setDDNS						= None+7,
    setUserInfo					= None+8,
    setvideoAttr					= None+9,
    setTtyUpdate					= None+10,
    setSysDefault					= None+11,          //user set nvs system para.
    setSysUpdate					= None+12,
    setSysTime					= None+13,
    setSysRestart					= None+14,  
    getSysPara					= None+15,         //user get nvs system para.
    sysAlarm						= None+16,
    setAutoCapIp					= None+17,
    setSaveSysParam				= None+18,
	setSpecifyHostInfo				= None+19,
	
	setAlarmOutput				= None+20,			// [zhb][add][2006-04-10]
	setRtpParams					= None+21,			// [zhb][add][2006-04-10]
	setEmailParams				= None+22,			// [zhb][add][2006-04-10]
	setUdpAlarm					= None+23,			// [zhb][add][2006-04-10][USA]
	setPersonContactInfo			= None+24,			// [zhb][add][2006-04-10][USA]	
	setPersonPsw					= None+25,			// [zhb][add][2006-04-10][USA]	
	setHomePos					= None+26,			// [zhb][add][2006-04-10][USA]	
	setTw2824Register			= None+27,

	setTL_OSD			=	0x10000

} nvsEventType_t;

typedef struct 
{
    nvsEventType_t          eventType;  // Event type.
    UInt                    dataSize;   // Size of associated data.
    Pointer                 data;       // Data associated with event.
} nvsEventDescription_t;

typedef Void (*nvsEventHandler_t)   (nvsEventDescription_t * event, Pointer data);

typedef struct 
{
    nvsEventType_t               eventType;  // Event to be created.
    nvsEventHandler_t            handler;    // Event handler.
    Pointer                      data;       // Generic data pointer. This data pointer will be
                                        // passed to the handler when the event is triggered.
} nvsEventCreate_t;

#endif

