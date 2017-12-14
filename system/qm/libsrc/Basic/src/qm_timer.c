#include "qm_timer.h"
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <pthread.h>

#include "list.h"
#include "ipc.h"
#include "looper.h"

#define LOGTAG "QM_TIMER"

#define TIMER_PROC_NAME_MAX_LEN     16
#define TIMER_MONITOR_LABEL_MAX_LEN     32

#define DUMP_LBUF_DEF(sz)   \
    char lbuf[sz] = {0};    \
    size_t ttsz = 0;    \
    int len = 0;
#define DUMP_LBUF(exp)  \
    exp;    \
    len = strlen(lbuf); \
    if (len < size) {   \
        memcpy(buf, lbuf, len); \
        buf += len; \
        ttsz += len;    \
    } else {    \
        goto Quit;  \
    }

typedef struct {
    struct listnode node;
    char name[TIMER_PROC_NAME_MAX_LEN];
    void const *p;
    int reload;
    int expire; /*!< 0 means disabled */
    int flag;   /*!< see TIMER_PROC_FLAG_XXX */
    timer_proc_fn_t procfunc;
} timer_proc_t;

typedef struct {
    struct listnode node;
    char label[TIMER_MONITOR_LABEL_MAX_LEN];
    int timeout;
    int duration;
} timer_monitor_t;

#define TIMER_RES_MS    (200)
typedef struct {
    looper_t lp;
    struct listnode procList;   /*!< list of timer_proc_t */
#ifndef __IPC_RELEASE__
    struct listnode monitorList;    /*!< list of timer_monitor_t */
#endif
    pthread_mutex_t lock;
    int inited;
} mtimer_t;

static mtimer_t sTimer = {
    .inited = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

static void * timerProcessor(void *p)
{
    int req;
    mtimer_t *timer = (mtimer_t *)p;
    struct listnode *node, *l;
    timer_proc_t *proc;
    timer_monitor_t *monitor;
    bool relax;

    ipclog_debug("enter timer processor\n");

    /*!< loop of timer processing */
    lp_lock(&timer->lp);
    lp_set_state(&timer->lp, STATE_IDLE);
    while (lp_get_req(&timer->lp) != REQ_QUIT) {
        /*!< wait for start/quit requst when in idle state */
        if (lp_get_state(&timer->lp) == STATE_IDLE) {
            req = lp_wait_request_l(&timer->lp);
            if (req == REQ_START) {
                /*!< do business */
                ipclog_debug("timer start to do things\n");
            } else if (req == REQ_QUIT) {
                goto Quit;
            }
        } else {
            /*!< check stop request */
            if (lp_check_request_l(&timer->lp) == REQ_STOP) {
                continue;
            }
        }

        /*!< timed check per-proc */
        lp_unlock(&timer->lp);
        usleep((TIMER_RES_MS - 5) * 1000);
        lp_lock(&timer->lp);

        /*!< must locked to access proc and to call procfunc, because it may be delete by timerDelProc() */
        relax = true;
        list_for_each(node, &timer->procList) {
            proc = node_to_item(node, timer_proc_t, node);
            if (proc->expire > 0) {
                proc->expire -= TIMER_RES_MS;
                if (proc->expire <= 0) {
                    /*!< must before call procfunc, because procfunc will call timerStopProc() that will set expire to 0 */
                    if (proc->flag & TIMER_PROC_FLAG_RELOAD) {
                        proc->expire = proc->reload;
                        relax = false;
                    }

                    assert(proc->procfunc);
                    //ipclog_debug("+timer proc [%s]\n", proc->name);
                    proc->procfunc(proc->p);
                    //ipclog_debug("-timer proc [%s]\n", proc->name);

                    if (proc->flag & TIMER_PROC_FLAG_ONESHOT) {
                        l = node->next;
                        list_remove(node);
                        free((void *)proc);
                        node = l;
                    }
                } else {
                    relax = false;
                }
            }
        }

#ifndef __IPC_RELEASE__
        list_for_each(node, &timer->monitorList) {
            monitor = node_to_item(node, timer_monitor_t, node);
            monitor->duration += TIMER_RES_MS;
            if (monitor->duration > monitor->timeout) {
                ipclog_alert("%s timedout (%d)\n", monitor->label, monitor->duration);
            }
        }
        relax = false;
#endif

        if (relax) {
            ipclog_debug("timer idle for nothing to do\n");
            lp_set_state(&timer->lp, STATE_IDLE);
        }

        /*!< for requestion checking */
        lp_unlock(&timer->lp);
        usleep(0);
        lp_lock(&timer->lp);
    }

Quit:
    lp_unlock(&timer->lp);
    ipclog_debug("leave timer processor\n");
    return NULL;
}

static void timerReset(void);
static int timerInit(void)
{
    mtimer_t *timer = &sTimer;

    ipclog_debug("timerInit() \n");

    //memset((void *)timer, 0, sizeof(mtimer_t));
    list_init(&timer->procList);
#ifndef __IPC_RELEASE__
    list_init(&timer->monitorList);
#endif
    if (lp_init(&timer->lp, "timer", timerProcessor, (void *)timer) != 0) {
        ipclog_ooo("create timer looper failed\n");
        return -1;
    }
    
    ipclog_debug("timerInit() done\n");

    return 0;
}

static void timerDeinit(void)
{
    mtimer_t *timer = &sTimer;

    ipclog_debug("timerDeinit() \n");

    /*!< stop and deinit looper */
    timerReset();
    assert(list_empty(&timer->procList));
#ifndef __IPC_RELEASE__
    assert(list_empty(&timer->monitorList));
#endif
    lp_deinit(&timer->lp);
    ipclog_debug("timerDeinit() done\n");
}

#ifndef __IPC_RELEASE__
static void timerAddMonitor(char const *prefix, char const *name, int timeout/* millisecond unit */)
{
    mtimer_t *timer = &sTimer;
    timer_monitor_t *monitor;

    assert(prefix);
    assert(name);
    assert(strlen(prefix) + strlen(name) < TIMER_MONITOR_LABEL_MAX_LEN);
    assert(timeout > 0);

    monitor = (timer_monitor_t *)malloc(sizeof(timer_monitor_t));
    if (!monitor) {
        ipclog_ooo("malloc timer_monitor_t failed\n");
        ipc_panic("out of memory");
    }

    sprintf(monitor->label, "%s-%s", prefix, name);
    monitor->timeout = timeout;
    monitor->duration = 0;
    lp_lock(&timer->lp);
    list_add_tail(&timer->monitorList, &monitor->node);
    lp_unlock(&timer->lp);
}

static void timerDelMonitor(char const *prefix, char const *name)
{
    mtimer_t *timer = &sTimer;
    struct listnode *node, *l;
    timer_monitor_t *monitor;
    char label[TIMER_MONITOR_LABEL_MAX_LEN];

    assert(prefix);
    assert(name);
    assert(strlen(prefix) + strlen(name) < TIMER_MONITOR_LABEL_MAX_LEN);

    sprintf(label, "%s-%s", prefix, name);
    lp_lock(&timer->lp);
    list_for_each_safe(node, l, &timer->monitorList) {
        monitor = node_to_item(node, timer_monitor_t, node);
        if (!strcmp(monitor->label, label)) {
            list_remove(node);
            free((void *)monitor);
            break;
        }
    }
    lp_unlock(&timer->lp);
}
#else
#define timerAddMonitor(prefix, name, timeout)
#define timerDelMonitor(prefix, name)
#endif

#define TMI(prefix, name, timeout)      timerAddMonitor(prefix, name, timeout)
#define TMO(prefix, name)               timerDelMonitor(prefix, name)

static void timerAddProc(timer_proc_fn_t procfunc, char const *name, void const *p, int expire, int flag)
{
    mtimer_t *timer = &sTimer;
    timer_proc_t *proc;

    ipclog_debug("timerAddProc(%p, %s, %p, %d, 0x%x)\n", procfunc, name, p, expire, flag);
    assert(procfunc);
    assert(name && (strlen(name) < (TIMER_PROC_NAME_MAX_LEN - 1)));

    proc = (timer_proc_t *)malloc(sizeof(timer_proc_t));
    if (!proc) {
        ipclog_ooo("malloc timer_proc_t failed\n");
        ipc_panic("out of memory");
    }

    strcpy(proc->name, name);
    proc->p = p;
    proc->reload = expire;
    proc->flag = flag;
    proc->procfunc = procfunc;
    lp_lock(&timer->lp);
    list_add_tail(&timer->procList, &proc->node);
    if (proc->flag & TIMER_PROC_FLAG_AUTOSTART) {
        proc->expire = expire;
        lp_start(&timer->lp, true);
        ipclog_debug("start timer proc %s\n", proc->name);
    } else {
        proc->expire = 0;
    }
    lp_unlock(&timer->lp);
    ipclog_debug("add timer proc %s\n", proc->name);
}

static void timerDelProc(timer_proc_fn_t procfunc)
{
    mtimer_t *timer = &sTimer;
    struct listnode *node, *l;
    timer_proc_t *proc;

    ipclog_debug("timerDelProc(%p)\n", procfunc);
    assert(procfunc);

    lp_lock(&timer->lp);
    list_for_each_safe(node, l, &timer->procList) {
        proc = node_to_item(node, timer_proc_t, node);
        if (proc->procfunc == procfunc) {
            list_remove(node);
            free((void *)proc);
            ipclog_debug("delete timer proc %s\n", proc->name);
            break;
        }
    }
    lp_unlock(&timer->lp);
}

static int timerProcIsEmpty(void)
{
    mtimer_t *timer = &sTimer;
  
    return list_empty(&timer->procList); 
}

static void timerRestartProc(timer_proc_fn_t procfunc, bool inProcfunc)
{
    mtimer_t *timer = &sTimer;
    struct listnode *node;
    timer_proc_t *proc;

    //ipclog_debug("timerRestartProc(%p, %d)\n", procfunc, inProcfunc);
    assert(procfunc);

    if (!inProcfunc) {
        lp_lock(&timer->lp);
    }
    list_for_each(node, &timer->procList) {
        proc = node_to_item(node, timer_proc_t, node);
        if (proc->procfunc == procfunc) {
            //ipclog_debug("start timer proc %s\n", proc->name);
            proc->expire = proc->reload;
            lp_start(&timer->lp, true);
            break;
        }
    }
    if (!inProcfunc) {
        lp_unlock(&timer->lp);
    }
}

static void timerStopProc(timer_proc_fn_t procfunc, bool inProcfunc)
{
    mtimer_t *timer = &sTimer;
    struct listnode *node;
    timer_proc_t *proc;

    ipclog_debug("timerStopProc(%p, %d)\n", procfunc, inProcfunc);
    assert(procfunc);

    if (!inProcfunc) {
        lp_lock(&timer->lp);
    }
    list_for_each(node, &timer->procList) {
        proc = node_to_item(node, timer_proc_t, node);
        if (proc->procfunc == procfunc) {
            ipclog_debug("stop timer proc %s\n", proc->name);
            proc->expire = 0;
            break;
        }
    }
    if (!inProcfunc) {
        lp_unlock(&timer->lp);
    }
}

static void timerReset(void)
{
    mtimer_t *timer = &sTimer;

    ipclog_debug("timerReset()\n");

    lp_stop(&timer->lp, false);
}

static size_t timerDump(char *buf, size_t size)
{
    mtimer_t *timer = &sTimer;
    struct listnode *node;
    timer_proc_t *proc;
    timer_monitor_t *monitor;

    ipclog_debug("timerDump(%p, %d)\n", buf, size);

    DUMP_LBUF_DEF(128);

    lp_lock(&timer->lp);
    DUMP_LBUF(sprintf(lbuf, "----- timerDump -----\n"));
    DUMP_LBUF(sprintf(lbuf, "proc list:\n"));
    list_for_each(node, &timer->procList) {
        proc = node_to_item(node, timer_proc_t, node);
        DUMP_LBUF(sprintf(lbuf, "name %s, p %p, reload %d, expire %d, flag 0x%x, procfunc %p\n",
                proc->name, proc->p, proc->reload, proc->expire, proc->flag, proc->procfunc));
    }

    DUMP_LBUF(sprintf(lbuf, "monitor list:\n"));
    list_for_each(node, &timer->monitorList) {
        monitor = node_to_item(node, timer_monitor_t, node);
        DUMP_LBUF(sprintf(lbuf, "label %s, timeout %d, duration %d\n",
                monitor->label, monitor->timeout, monitor->duration));
    }
Quit:
    lp_unlock(&timer->lp);
    return ttsz;
}

static int qm_timer_register_processor(timer_proc_fn_t procfunc, 
        char const *name, void const *p, int expire, int flag) 
{
    mtimer_t *tm = &sTimer;
    pthread_mutex_lock(&tm->lock);
    if (!tm->inited) {
        timerInit();
        tm->inited = 1;
    }
    
    timerAddProc(procfunc, name, p, expire, flag);
    pthread_mutex_unlock(&tm->lock);
    return 0;
}

static int qm_timer_restart_processor(timer_proc_fn_t procfunc, bool inProcfunc) 
{
    mtimer_t *tm = &sTimer;
    pthread_mutex_lock(&tm->lock);
    if (!tm->inited) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&tm->lock);
        return 0;
    }

    timerRestartProc(procfunc, inProcfunc);
    pthread_mutex_unlock(&tm->lock);

    return 0;
}

static int qm_timer_stop_processor(timer_proc_fn_t procfunc, bool inProcfunc) 
{
    mtimer_t *tm = &sTimer;
    pthread_mutex_lock(&tm->lock);
    if (!tm->inited) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&tm->lock);
        return 0;
    }

    timerStopProc(procfunc, inProcfunc);
    pthread_mutex_unlock(&tm->lock);
    return 0;
}

static int qm_timer_unregister_processor(timer_proc_fn_t procfunc)
{
    mtimer_t *tm = &sTimer;
    pthread_mutex_lock(&tm->lock);
    if (!tm->inited) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&tm->lock);
        return 0;
    }

    timerDelProc(procfunc);

    if (timerProcIsEmpty()) {
        timerDeinit();
        tm->inited = 0;
    }
    pthread_mutex_unlock(&tm->lock);

    return 0;
}


int QM_Timer_RegisterProcessor(timer_proc_fn_t procfunc, 
        char const *name, void const *p, int expire, int flag) 
{
    return qm_timer_register_processor(procfunc, name, p, expire, flag);
}

int QM_Timer_RestartProcessor(timer_proc_fn_t procfunc, bool inProcfunc) 
{
    return qm_timer_restart_processor(procfunc, inProcfunc);
}

int QM_Timer_StopProcessor(timer_proc_fn_t procfunc, bool inProcfunc) 
{
    return qm_timer_stop_processor(procfunc, inProcfunc);
}

int QM_Timer_UnregisterProcessor(timer_proc_fn_t procfunc)
{
    return qm_timer_unregister_processor(procfunc);
} 
