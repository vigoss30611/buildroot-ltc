#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/profile.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

MODULE_LICENSE("GPL");
#define NETLINK_USER 31
#define PROCESS_MAX_NAME 32
static struct sock *nl_sk = NULL;

struct process_info {
	int pid;
	char name[PROCESS_MAX_NAME];
};
static int nl_sendmsg(struct sock *sk, struct process_info *info)
{
    struct nlmsghdr *nlh = NULL;
    struct sk_buff *nl_skb = NULL;
    int msglen;

    if((NULL == sk) || (NULL == info)) {
    	return -1;
    }

    msglen = sizeof(struct process_info);

    nl_skb = alloc_skb(NLMSG_SPACE(msglen), GFP_ATOMIC);
    if(!nl_skb) {
        printk("nl_skb == NULL, msglen = %d, failed!\n", msglen);
        return -1;
    }

    nlh = nlmsg_put(nl_skb, 0, 0, 0, NLMSG_SPACE(msglen) - NLMSG_HDRLEN, 0);
    NETLINK_CB(nl_skb).portid = 0;
    NETLINK_CB(nl_skb).dst_group = 5;
    memcpy(NLMSG_DATA(nlh), info, msglen);

 	netlink_broadcast(sk, nl_skb, 0, 5, GFP_KERNEL);

    return 0;
}

#if 0
static struct sk_buff *skb = NULL;
static struct nlmsghdr *nlh = NULL;
#define ENDIANNESS ((char)endian_test.l)
static union { char c[4]; unsigned long l; } endian_test __initdata = { { 'l', '?', '?', 'b' } };

struct task_struct *(*orig_copy_process)(unsigned long clone_flags,
					unsigned long stack_start,
					struct pt_regs *regs,
					unsigned long stack_size,
					int __user *parent_tidptr,
					int __user *child_tidptr,
					int pid);

static struct task_struct *my_copy_process(unsigned long clone_flags,
					unsigned long stack_start,
					struct pt_regs *regs,
					unsigned long stack_size,
					int __user *parent_tidptr,
					int __user *child_tidptr,
					int pid)
{
	struct task_struct * ret;
	ret = (*orig_copy_process)(clone_flags,stack_start,regs,stack_size,parent_tidptr,child_tidptr,pid);
	if(!IS_ERR(ret)) {
		printk("---z---\tPID:%d fork %d successed!\n",current->pid,pid);
	} else {
		printk("---z---\tPID:%d fork %d failed!\n",current->pid,pid);
	}
	return ret;

}

void (*orig_profile_task_exit)(struct task_struct * task);

void my_profile_task_exit(struct task_struct * task)
{
    struct process_info info;
    int ret;

	if(strcmp(current->comm, "mdev")) {
		info.pid = current->pid;
		strncpy(info.name, current->comm, strlen(current->comm)+1);
    	ret = nl_sendmsg(nl_sk, &info);
	}
	return;
}

static int replace_fun(unsigned long handle, unsigned long old_fun, unsigned long new_fun)
{
	unsigned char *p = (unsigned char *)handle;
  	int offset = 0;
  	int high;
  	int low;
  	long machinecode;
  	unsigned int orig = 0;
	int i = 0;
#if 0	
	char data = 0;
#endif

	while (1) {
		if (i > 128) {
      		return 0;
    	}
#if 0    	
		if('l' == ENDIANNESS) {
			data = p[0];	
		} else if('b' == ENDIANNESS) {
			data = p[3];
		}
#endif
    	if ((*p & 0xf8) == 0xf8) { //bl instruction code
    		offset = old_fun - (long)p;
    		offset = (offset - 4) & 0x007fffff;
    		high = offset >> 12;
    		low = (offset & 0x00000fff) >> 1;
    		machinecode = ((0xf800 | low) << 16) | (0xf000 | high);

    		if(machinecode == *(unsigned int *)p) {
				break;
    		}
    	}
    	(unsigned char *)p;
    	p++;
   	 	i++;
  	}

  	offset = new_fun - (long)p;
    offset = (offset - 4) & 0x007fffff;
    high = offset >> 12;
    low = (offset & 0x00000fff) >> 1;
    machinecode = ((0xf800 | low) << 16) | (0xf000 | high);
	*(unsigned int *)p = machinecode;

  	return orig;
}

#if 0
ssize_t *sys_call_table = (ssize_t)NULL;

static void get_sys_call_table()
{
	void *swi_addr = (long*)0xffff0008;
	unsigned long offset = 0;
	unsigned long *vector_swi_addr = 0;

	offset = ((*(long *)swi_addr) & 0xfff) + 8;
	vector_swi_addr = *(unsigned long *)(swi_addr + offset);
	while(vector_swi_addr++) {
		if(((*(unsigned long *)vector_swi_addr) & 0xfffff000) == 0xe28f8000) {
			offset = ((*(unsigned long *)vector_swi_addr) & 0xfff) + 8;
			sys_call_table = (void *)vector_swi_addr + offset;
			break;
		}
	}
	return;
}
#endif

static int _init_module(void ) {

	struct netlink_kernel_cfg cfg = {
		.groups = 1,
		.input = NULL,
    };

	orig_profile_task_exit = profile_task_exit;
	replace_fun((long)do_exit, (unsigned long)profile_task_exit, (unsigned long)my_profile_task_exit);

	nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
	if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -1;
    }
	return 0;
}


static void _cleanup_module(void) {

	replace_fun(do_exit, (unsigned long)my_profile_task_exit, (unsigned long)profile_task_exit);

	netlink_kernel_release(nl_sk);

	return;
}
#else
int my_profile_task_exit(struct notifier_block *self, unsigned long val, void *data)
{
	struct process_info info;

	if(strcmp(current->comm, "mdev")) {
		info.pid = current->pid;
		strncpy(info.name, current->comm, strlen(current->comm)+1);
		nl_sendmsg(nl_sk, &info);
	}
	return 0;
}

static struct notifier_block my_task_exit_nb = {
	.notifier_call = my_profile_task_exit,
};

static int _init_module(void ) 
{
	struct netlink_kernel_cfg cfg = {
		.groups = 1,
		.input = NULL,
	};

	profile_event_register(PROFILE_TASK_EXIT, &my_task_exit_nb);

	nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
	if (!nl_sk) {
		printk(KERN_ALERT "Error creating socket.\n");
		return -1;
	}
	return 0;
}

static void _cleanup_module(void) 
{
	profile_event_unregister(PROFILE_TASK_EXIT, &my_task_exit_nb);

	netlink_kernel_release(nl_sk);
}
#endif
module_init(_init_module);
module_exit(_cleanup_module);
