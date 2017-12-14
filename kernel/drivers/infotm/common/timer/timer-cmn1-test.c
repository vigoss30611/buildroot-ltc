/*
    This program is only for cmn timer 1 test, it must not be included in a formal release.
*/
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/timer-cmn1.h>

#if 1
#define MAX_DELTA_RANGE_NUM     16
enum{
    ISR_STATISTIC_ID = 0,
    TASKLET_STATISTIC_ID,
};
struct cmn1_statistic_struct{
    long    tick_num;
    int     usec_period;
    int     work_finished;
    long    cmn1_int_counter;
    int     cmn1_delta_counter[MAX_DELTA_RANGE_NUM];
    int     delta_us[MAX_DELTA_RANGE_NUM];
    struct timeval cmn1_last_timval;
    struct timeval cmn1_curr_timval;
};
static struct cmn1_statistic_struct cmn1_timer_statistic[2];

static void cmn1_timer_statistic_init(int index, long msec, long tick_num)
{
    static int delta_us[MAX_DELTA_RANGE_NUM] = {
        -1,     30,     60,     100,
        150,    200,    400,    600,
        1000,   2000,   5000,   10000,
        20000,  30000,  50000,  -1};

    memset(&cmn1_timer_statistic[index], 0, sizeof(struct cmn1_statistic_struct));
    memcpy(cmn1_timer_statistic[index].delta_us, delta_us, sizeof(delta_us));
    cmn1_timer_statistic[index].tick_num = tick_num;
    cmn1_timer_statistic[index].usec_period = msec*1000;

    #ifndef  TEST_CMN1_TIMER_TASKLET
    if(index == TASKLET_STATISTIC_ID){
        cmn1_timer_statistic[index].work_finished = 1;
        cmn1_timer_statistic[index].tick_num = 0;
    }
    #endif

    return;
}
static void cmn1_timer_statistic_proc(unsigned long index)
{
    int i = 0;
    struct cmn1_statistic_struct* pctrl = &cmn1_timer_statistic[index];
    
    if(pctrl->cmn1_int_counter > pctrl->tick_num){
        pctrl->work_finished = 1;
        return;
    }

    pctrl->cmn1_last_timval = pctrl->cmn1_curr_timval;
    do_gettimeofday(&pctrl->cmn1_curr_timval);
    if(pctrl->cmn1_int_counter > 0){
        long delta_timval = (pctrl->cmn1_curr_timval.tv_sec-pctrl->cmn1_last_timval.tv_sec)*1000000
            + (pctrl->cmn1_curr_timval.tv_usec-pctrl->cmn1_last_timval.tv_usec);
        if(delta_timval > pctrl->usec_period)
            delta_timval -= pctrl->usec_period;
        else
            delta_timval = pctrl->usec_period - delta_timval;

        for(i=(MAX_DELTA_RANGE_NUM-2);i>=0;i--){
            if(delta_timval > pctrl->delta_us[i]){
                pctrl->cmn1_delta_counter[i]++;
                break;
            }
        }
    }

    pctrl->cmn1_int_counter++;
}
static void cmn1_timer_statistic_show(int index)
{
    int     i = 0;
    long    sum_delta = 0;
    long    tmp_percent = 0;
    struct cmn1_statistic_struct* pctrl = &cmn1_timer_statistic[index];

    if(pctrl->tick_num <= 0)
        return;

    printk(KERN_ERR "~~~~ cmn1_statistic show #%d  Expect:(%ld) Real:(%ld) latest:(%ld, %ld)~~~~\n", index, 
        pctrl->tick_num, pctrl->cmn1_int_counter, pctrl->cmn1_curr_timval.tv_sec, pctrl->cmn1_curr_timval.tv_usec);
    for(i=(MAX_DELTA_RANGE_NUM-2);i>=0;i--){
        sum_delta += pctrl->cmn1_delta_counter[i];
        tmp_percent = sum_delta*10000/pctrl->tick_num;
        printk(KERN_ERR "delta:(%5d~%5d us) %6d, above_all:%2ld.%02ld %%\n", pctrl->delta_us[i], pctrl->delta_us[i+1],
            pctrl->cmn1_delta_counter[i], tmp_percent/100, tmp_percent%100);
    }
}
static inline bool cmn1_timer_statistic_running(int index)
{
    return cmn1_timer_statistic[index].work_finished? false:true;
}
static irqreturn_t jz_timer_interrupt(int irq, void *dev_id)
{
    cmn1_timer_clear_int();
    cmn1_timer_statistic_proc(0);
    return IRQ_HANDLED;
}
/**/
static void cmn1_timer_test_case1(long interval_msec, long tick_num)
{
    long run_step_irq = 0;

    cmn1_timer_init();
    run_step_irq = cmn1_timer_get_irq();
    request_irq(run_step_irq, jz_timer_interrupt, 0, "jz_timer_interrupt", NULL);
    
    cmn1_timer_set_period(interval_msec);
    cmn1_timer_statistic_init(ISR_STATISTIC_ID, interval_msec, tick_num);
    cmn1_timer_enable();

    while(cmn1_timer_statistic_running(ISR_STATISTIC_ID)){
        msleep(20);
    }

    cmn1_timer_disable();
    free_irq(cmn1_timer_get_irq(), NULL);

    cmn1_timer_statistic_show(ISR_STATISTIC_ID);
}
static void cmn1_timer_test_case2(long interval_msec, long tick_num)
{
    cmn1_timer_statistic_init(ISR_STATISTIC_ID, interval_msec, tick_num);
    cmn1_timer_statistic_init(TASKLET_STATISTIC_ID, interval_msec, tick_num);
    
    cmn1_timer_register_handler(cmn1_timer_statistic_proc, ISR_STATISTIC_ID);
    #ifdef  TEST_CMN1_TIMER_TASKLET
    cmn1_timer_register_tasklet(cmn1_timer_statistic_proc, TASKLET_STATISTIC_ID);
    #endif
    cmn1_timer_set_period(interval_msec);
    cmn1_timer_enable();

    while(cmn1_timer_statistic_running(ISR_STATISTIC_ID) 
        || cmn1_timer_statistic_running(TASKLET_STATISTIC_ID)
        ){
        msleep(20);
    }
    cmn1_timer_disable();
    cmn1_timer_deregister_handler();
    
    cmn1_timer_statistic_show(ISR_STATISTIC_ID);
    cmn1_timer_statistic_show(TASKLET_STATISTIC_ID);
    
}
static int cmn1_timer_test_thread(void *d)
{
    long    i=0, tick_num = 0;
    struct _test_period{
        long    inter_msec; //interrupt interval, in ms
        long    duration;   //test duration, in second
    };
    #define     TEST_CASE_NUM       8
    struct _test_period test_period[TEST_CASE_NUM] = {
            {1, 10}, {10, 60}, {10, 600}, {10, 1800}, {10, 9000}, {0,0}
    };

    ssleep(6);

    for(i=0;i<TEST_CASE_NUM;i++){
        if(0 >= test_period[i].inter_msec)
            break;

        tick_num = test_period[i].duration*1000/test_period[i].inter_msec;
        cmn1_timer_test_case1(test_period[i].inter_msec, tick_num);
        cmn1_timer_test_case2(test_period[i].inter_msec, tick_num);
    }
    return 0;
}
static int __init cmn1_timer_test(void)
{
    struct task_struct  *thread = NULL;
    
    cmn1_timer_init();
    thread = kthread_run(cmn1_timer_test_thread, NULL, "cmn1_timer_test");
    if (IS_ERR(thread)) {
    }

    return 0;
}
late_initcall(cmn1_timer_test);
#endif

