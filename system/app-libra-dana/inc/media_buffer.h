/*
 * =====================================================================================
 *
 *       Filename:  media_buffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年06月26日 10时25分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  soyo, soyo.lin@infotmic.com.cn
 *        Company:  infoTM
 *
 * =====================================================================================
 */

#ifndef __MEDIA_BUFFER_H__
#define __MEDIA_BUFFER_H__

#include <pthread.h>

//#define _MEDIA_BUFFER_DEBUG

#ifdef _MEDIA_BUFFER_DEBUG
#define DEBUG_PRINT(args) printf args
#else
#define DEBUG_PRINT(args)
#endif

#define WARN_PRINT(args)  printf args
#define ERROR_PRINT(args) printf args

#define IN
#define OUT

/*media buffer*/
typedef struct {
    int size;
    void *vir_addr;
    void *phy_addr;
    int refs;
    pthread_mutex_t m_lock;
    void* m_handler;
    void *priv;
} media_buffer;

typedef struct {
    media_buffer *(*create)(void);
    int (*destroy)(media_buffer *buffer);
}media_buffer_handle;

media_buffer *media_buffer_copy(media_buffer *buffer); 
media_buffer *media_buffer_release(media_buffer *buffer); 

media_buffer *media_buffer_init(media_buffer *buffer, void *handler);
media_buffer *media_buffer_deinit(media_buffer *buffer);

/*End of media buffer*/


/*media buffer pool*/
#define MAX_POOL_NAME 50

typedef struct media_buffer_element_t {
    int id;
    int used;
    media_buffer buffer;
    struct media_buffer_element_t *next;
} media_buffer_element_t;

typedef struct media_buffer_pool {
    int depth;
    int wait;
    int debug_print;
    int buffer_used;
    int buffer_max_used;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    media_buffer_element_t *buffers_array;
    media_buffer_element_t *buffers_free_head;
    media_buffer_element_t *buffers_free_tail;
    char name[MAX_POOL_NAME];
} media_buffer_pool_t;

int media_buffer_free(media_buffer_pool_t * pool, media_buffer *buffer);
int media_buffer_get(media_buffer_pool_t *pool, media_buffer **buffer);

int media_buffer_has_free(media_buffer_pool_t *pool);

media_buffer_pool_t *media_buffer_pool_init(int depth, const char *name, int debug);
void *media_buffer_pool_deinit(media_buffer_pool_t *pool);

media_buffer *media_buffer_create(media_buffer_pool_t *pool, media_buffer *mediaBuffer, 
        void *vir_addr, void *phy_addr, int size);

media_buffer *media_buffer_create2(media_buffer_pool_t *pool, media_buffer *mediaBuffer, media_buffer_handle *handler,
        void *vir_addr, void *phy_addr, int size);
/* End of media buffer */


#endif
