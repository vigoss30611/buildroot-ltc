#include <pthread.h>
pthread_mutex_t enc_mutex = PTHREAD_MUTEX_INITIALIZER;

void EncHwLock(void)
{
	pthread_mutex_lock(&enc_mutex);
}

void EncHwUnlock(void)
{
	pthread_mutex_unlock(&enc_mutex);
}
