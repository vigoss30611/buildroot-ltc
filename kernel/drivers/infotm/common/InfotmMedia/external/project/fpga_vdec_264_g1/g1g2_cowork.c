
#include "g1g2_cowork.h"

#include <linux/semaphore.h>
#include <linux/mutex.h>

static int cowork_init = 0;

DEFINE_MUTEX(cowork_lock);
struct semaphore cowork_sem;
static int cowork_user = USER_NONE;
static int cowork_user_count = 0;
static int cowork_waiter_count = 0;
static int sema_inited = 0;

int g1g2_cowork_init()
{
    mutex_lock(&cowork_lock);
    if (cowork_init == 0) {
        if(sema_inited == 0) {
            sema_init(&cowork_sem, 0);
            sema_inited = 1;
        }

        cowork_user = USER_NONE;
        cowork_user_count = 0;
        cowork_waiter_count = 0;
    }

    cowork_init++;
    mutex_unlock(&cowork_lock);

    return 0;
}

int g1g2_cowork_deinit()
{
    mutex_lock(&cowork_lock);
    cowork_init--;
    if (cowork_init == 0) {
        // check 
        //printk("cowork deinit  \n");
        if (cowork_user != USER_NONE || cowork_user_count != 0 || cowork_waiter_count != 0) {
            printk(KERN_ALERT "when deinit, some resource isn't release(user:%d, user count:%d, waiter_count:%d) \n",
                    cowork_user, cowork_user_count, cowork_waiter_count);
        }
        cowork_user = USER_NONE;
        cowork_user_count = 0;
        cowork_waiter_count = 0;
    }

    mutex_unlock(&cowork_lock);

    return 0;
}

int g1g2_cowork_reserve(int user)
{
    int status = USER_UNCHANGE;
    mutex_lock(&cowork_lock);
    if (cowork_user != user) {
        //printk("user %d request reserve hardware(%d), count =%d \n ", user, cowork_user, cowork_user_count);
        if (cowork_user_count  > 0) { 
            cowork_waiter_count++;
            mutex_unlock(&cowork_lock);
            if (down_interruptible(&cowork_sem)) {
                return -1;
            }
            mutex_lock(&cowork_lock);
        }

        cowork_user = user;
        status = USER_CHANGED;
    }


    cowork_user_count++;
    mutex_unlock(&cowork_lock);
    return status;
}


int g1g2_cowork_release(int user)
{
    int user_count = 0;
    mutex_lock(&cowork_lock);
    if (cowork_user == user) {
        cowork_user_count--;
        //printk("user %d release hardware(%d), cowork_waiter_count-:%d \n ", user, cowork_user_count, cowork_waiter_count);
        if (cowork_user_count == 0) {
            cowork_user = USER_NONE;
                while (cowork_waiter_count > 0) {
                    cowork_waiter_count--;
                    up(&cowork_sem);
                }
        }
    }
    user_count = cowork_user_count;

    mutex_unlock(&cowork_lock);

    return user_count;
}

int g1g2_cowork_get_user()
{
    int user = USER_NONE;

    mutex_lock(&cowork_lock);
    //printk("cowork current user %d  \n", cowork_user);
    user = cowork_user;
    mutex_unlock(&cowork_lock);

    return user;
}

EXPORT_SYMBOL(g1g2_cowork_init);
EXPORT_SYMBOL(g1g2_cowork_deinit);
EXPORT_SYMBOL(g1g2_cowork_reserve);
EXPORT_SYMBOL(g1g2_cowork_release);
EXPORT_SYMBOL(g1g2_cowork_get_user);
