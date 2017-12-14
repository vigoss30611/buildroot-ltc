#include <linux/clk-private.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/debugfs.h>


static struct clk *c = NULL;

static int clk_name_show(struct seq_file *s, void *data)
{
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}
	printk("%s\n", c->name);
	return 0;
}

static int clk_name_open(struct inode *inode, struct file *file)
{
	return single_open(file, clk_name_show, inode->i_private);
}

static int clk_name_write(struct file *file, const char __user *data,
					size_t count, loff_t *ppos)
{
	char buf[64];
	size_t buf_size;
	int i;
	
	buf_size = min(count, (sizeof(buf)-1));
	if (copy_from_user(buf, data, buf_size))
		return -EFAULT;
	buf[buf_size - 1] = '\0';
	
	for (i = 0; i < clk_num; i++) {
		if (!strncmp(clks[i]->name, buf, buf_size)) {
			c = clks[i];
			break;
		}
	}
	return count;
}

static const struct file_operations clk_name_fops = {
	.open = clk_name_open,
	.write = clk_name_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int clk_enable_show(struct seq_file *s, void *data)
{
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}
	printk("%s\n", c->enable_count ? "enable" : "disable");
	return 0;
}

static int clk_enable_open(struct inode *inode, struct file *file)
{
	return single_open(file, clk_enable_show, inode->i_private);
}

static int clk_enable_write(struct file *file, const char __user *data,
					size_t count, loff_t *ppos)
{
	char buf[32];
	size_t buf_size;
	
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}

	buf_size = min(count, (sizeof(buf)-1));
	if (copy_from_user(buf, data, buf_size))
		return -EFAULT;
	buf[buf_size - 1] = '\0';
	
	if (!strcmp(buf, "enable"))
		clk_prepare_enable(c);
	else if (!strcmp(buf, "disable"))
		clk_disable_unprepare(c);

	return count;
}

static const struct file_operations clk_enable_fops = {
	.open = clk_enable_open,
	.write = clk_enable_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int clk_freq_show(struct seq_file *s, void *data)
{
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}
	printk("%ld\n", clk_get_rate(c));
	return 0;
}

static int clk_freq_open(struct inode *inode, struct file *file)
{
	return single_open(file, clk_freq_show, inode->i_private);
}

static int clk_freq_write(struct file *file, const char __user *data,
					size_t count, loff_t *ppos)
{
	char buf[32];
	size_t buf_size;
	int value;
	
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}

	buf_size = min(count, (sizeof(buf)-1));
	if (copy_from_user(buf, data, buf_size))
		return -EFAULT;
	buf[buf_size - 1] = 0;
	
	value = simple_strtoull(buf, NULL, 10);	
	if (value)
		clk_set_rate(c, value);

	return count;
}

static const struct file_operations clk_freq_fops = {
	.open = clk_freq_open,
	.write = clk_freq_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int clk_state_show(struct seq_file *s, void *data)
{
	if (!c) {
		printk("you need input clk name first\n");
		return 0;
	}

	printk("clock				enable_cnt  prepare_cnt  rate\n");	
	printk("----------------------------------------------------------------\n");
	printk("%s					%d	%d	%ld\n", c->name,
			c->enable_count, c->prepare_count, clk_get_rate(c));	
	return 0;
}

static int clk_state_open(struct inode *inode, struct file *file)
{
	return single_open(file, clk_state_show, inode->i_private);
}

static const struct file_operations clk_state_fops = {
	.open = clk_state_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
 * 1. set clk name: echo apb_pclk > name
 * 2. set enable: echo enable or disable > enable
 *    set freq: echo 115200 > freq
 * 3. read current clk state: cat state
 */

int clk_create_debugfs(struct dentry *parent)
{
	struct dentry *cfg;
	struct dentry *d;

	if (!parent)
		return -ENOMEM;

	cfg = debugfs_create_dir("configure", parent);
	if (!cfg)
		return -ENOMEM;

	d = debugfs_create_file("name", S_IRUGO | S_IWUGO, cfg, NULL,
				&clk_name_fops);
	if (!d)
		return -ENOMEM;

	d = debugfs_create_file("enable", S_IRUGO | S_IWUGO, cfg, NULL,
				&clk_enable_fops);
	if (!d)
		return -ENOMEM;

	d = debugfs_create_file("freq", S_IRUGO | S_IWUGO, cfg, NULL,
				&clk_freq_fops);
	if (!d)
		return -ENOMEM;

	d = debugfs_create_file("state", S_IRUGO, cfg, NULL,
				&clk_state_fops);
	if (!d)
		return -ENOMEM;
	return 0;
}
EXPORT_SYMBOL(clk_create_debugfs);
