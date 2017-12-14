#ifndef __QM_TIMER_H__
#define __QM_TIMER_H__
#include <stdbool.h>

#define TIMER_PROC_FLAG_ONESHOT     (1<<0)  /*!< removed from procList while triggered one time */
#define TIMER_PROC_FLAG_AUTOSTART   (1<<1)  /*!< auto start after timerAddProc() */
#define TIMER_PROC_FLAG_RELOAD      (1<<2)  /*!< auto reload while expired */

/*!< low resolution timer in milliosecond */
typedef void (*timer_proc_fn_t) (void const *p);

int QM_Timer_RegisterProcessor(timer_proc_fn_t procfunc, 
        char const *name, void const *p, int expire, int flag);
int QM_Timer_UnregisterProcessor(timer_proc_fn_t procfunc); 

int QM_Timer_RestartProcessor(timer_proc_fn_t procfunc, bool inProcfunc);
int QM_Timer_StopProcessor(timer_proc_fn_t procfunc, bool inProcfunc);

#endif
