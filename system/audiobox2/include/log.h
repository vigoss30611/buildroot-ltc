#ifndef __AUDIOBOX_LOG_H__
#define __AUDIOBOX_LOG_H__

//#define  AUDIOBOX_DEBUG

#ifdef AUDIOBOX_DEBUG
#define AUD_DBG(arg...)		printf(arg) 
#else
#define AUD_DBG(arg...)
#endif
#define AUD_ERR(arg...)     printf(arg)
#define AUD_INFO(arg...)     printf(arg)
#endif
