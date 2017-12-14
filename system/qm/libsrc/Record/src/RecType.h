#ifndef _RECORD_TYPE_H_
#define _RECORD_TYPE_H_

//#define DEBUG_REC_PTHREAD_MUTEX

#if 0

#define REC_DEBUG(str, arg...)		printf("REC_DEBUG::[FILE]:%s  [FUNCTION]:%s [LINE]:%d "str, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define REC_TRACE(str, arg...)		printf("REC_TRACE::[FILE]:%s  [FUNCTION]:%s [LINE]:%d "str, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define REC_ERR(str, arg...)		printf("REC_ERR::[FILE]:%s  [FUNCTION]:%s [LINE]:%d "str, __FILE__, __FUNCTION__, __LINE__, ##arg)

#else
#define X6_TRACE(fmt, arg...) {}
#define REC_TRACE(fmt, arg...) {}
#define REC_DEBUG(fmt, arg...) {}
//Error,±ÿ–Î¥Ú”°
#define REC_ERR(str, arg...)		printf("REC_ERR::[FILE]:%s  [FUNCTION]:%s [LINE]:%d "str, __FILE__, __FUNCTION__, __LINE__, ##arg)

#endif

#ifdef DEBUG_REC_PTHREAD_MUTEX
#define REC_PTHREAD_MUTEX_LOCK(mutex) \
{							\
	pthread_t pthread_id = pthread_self();		\
	REC_DEBUG("Thread 0x%X before "#mutex" 0x%X\n", (int)pthread_id, (int)mutex); \
	pthread_mutex_lock(mutex); \
	REC_DEBUG("Thread 0x%X after "#mutex" 0x%X\n", (int)pthread_id, (int)mutex);  \
}
#define REC_PTHREAD_MUTEX_UNLOCK(mutex) \
{							\
	pthread_t pthread_id = pthread_self();		\
	pthread_mutex_unlock(mutex); \
	REC_DEBUG("Thread 0x%X Leave "#mutex" 0x%X\n", (int)pthread_id, (int)mutex); \
}
#else
#define REC_PTHREAD_MUTEX_LOCK(mutex) (pthread_mutex_lock(mutex))
#define REC_PTHREAD_MUTEX_UNLOCK(mutex) (pthread_mutex_unlock(mutex))
#endif
#endif
