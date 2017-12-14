
#ifndef _ARM_IMAP_CMN_TIMER_1_H_
#define _ARM_IMAP_CMN_TIMER_1_H_

/* Get irq number for cmn timer 1 */
extern long cmn1_timer_get_irq(void);
/* clear timer interrupt status register */
extern void cmn1_timer_clear_int(void);
/* set cmn timer 1 reload value, in msec.*/
extern int  cmn1_timer_set_period(long msec);
/* enable cmn timer 1 */
extern int  cmn1_timer_enable(void);
/* disable cmn timer 1 */
extern void cmn1_timer_disable(void);
/* init cmn timer 1 */
extern void cmn1_timer_init(void);


/* register IRQ handler callback function for cmn timer 1 */
extern long cmn1_timer_register_handler(void (*func)(unsigned long),unsigned long para);
/* deregister IRQ handler callback function for cmn timer 1, and remove irq setup*/
extern void cmn1_timer_deregister_handler(void);
#ifdef  TEST_CMN1_TIMER_TASKLET
/* register IRQ handelr tasklet function for cmn timer 1 */
extern long cmn1_timer_register_tasklet(void (*func)(unsigned long),unsigned long para);\
/* deregister IRQ handelr tasklet function for cmn timer 1 */
extern void cmn1_timer_deregister_tasklet(void);
#endif

#endif /* _ARM_IMAP_CMN_TIMER_H_ */

