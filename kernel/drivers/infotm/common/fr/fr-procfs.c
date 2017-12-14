#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static int total;

static void *l_start(struct seq_file *m, loff_t * pos)
{
  struct fr *fr = NULL;
  if(!list_empty(&list_of_frs) && !*pos) {
    fr = list_first_entry(&list_of_frs, struct fr, node);
	seq_printf(m, "                   GETGET  WAIT"
			"    DEAL    PUTPUT  FPS    FRAMES\n");
	seq_printf(m, "                   -----------"
			"----------------------------------\n");
  }
  total = 0;
  return fr;
}

static void *l_start_info(struct seq_file *m, loff_t * pos)
{
  struct fr *fr = NULL;
  if(!list_empty(&list_of_frs) && !*pos) {
    fr = list_first_entry(&list_of_frs, struct fr, node);
	total = 0;
  }
  return fr;
}

static void *l_next(struct seq_file *m, void *p, loff_t * pos)
{
  struct fr *fr = (struct fr *)p;

  fr = list_first_entry(&fr->node, struct fr, node);
  if(&fr->node == &list_of_frs) return NULL;
  return fr;
}

static void l_stop(struct seq_file *m, void *p)
{
	if(total) seq_printf(m, "\nTotal: %d.%dM\n",
		total >> 20, ((total & 0xfffff) * 100) >> 20);
}

static inline void l_show_st(struct seq_file *m, struct fr_statistics *st)
{
	int i, j, count, avg, min, max;

	if(!st) {
		seq_printf(m, "\n");
		return ;
	}

	for(i = 0; i < FRTIME_ST_COUNT; i++) {
		count = avg = min = max = 0;
		for(j = 0; j < FRSTFRAMES; j++)
			if(st->ti[j][i] > 0) {
				avg += st->ti[j][i];
				min = min? MIN(min, st->ti[j][i]): st->ti[j][i];
				max = MAX(max, st->ti[j][i]);
				count++;
			}
		if(!count) count = 1;
		avg /= count;
		seq_printf(m, "%02d/%02d   ", avg, max);
	}

	/* which means not available yet */
	if(!avg) avg = 1;
	seq_printf(m, "%d.%d   %d/%d\n",
			1000 / avg, (1000 % avg) * 10 / avg,
			st->count, count);
}

static int l_show_info(struct seq_file *m, void *p)
{
  struct fr *fr = (struct fr *)p;
  struct fr_buf *buf;

  if(!fr) return 0;

  seq_printf(m, "%s: %d.%dM (%s", fr->name, fr->size / 0x100000,
          (fr->size & 0xfffff) * 100 / 0x100000,
		  FR_ISFLOAT(fr)? "float)\n": FR_ISVACANT(fr)?
		 "vacant: ": "ring: ");

  if(!FR_ISFLOAT(fr)) {
		buf = fr->ring;
		seq_printf(m, "%08x ", buf->phys_addr);
		if(!FR_ISVACANT(fr)) total += buf->size;
		list_for_each_entry(buf, &fr->ring->node, node) {
			seq_printf(m, "%08x ", buf->phys_addr);
			if(!FR_ISVACANT(fr)) total += buf->size;
		}
		seq_printf(m, ")\n");
  } else
	total += fr->size;

  return 0;
}

static int l_show(struct seq_file *m, void *p)
{
  struct fr *fr = (struct fr *)p;
  struct task_struct *task;
  int i;

  if(!fr) return 0;

  /* don't show producer info for single buffer */
  if(FR_GETCOUNT(fr) == 1 && fr->st_ref[FRTIME_ST_PUT].count <= 1) return 0;

  seq_printf(m, "[%c%s %d.%dMx%s%d]\n  |\n", fr->flag & FR_FLAG_NODROP?
			 '%': fr->flag & FR_FLAG_PROTECTED? '@': '*',
			 fr->name, fr->size / 0x100000,
          (fr->size & 0xfffff) * 10 / 0x100000,
		  FR_ISFLOAT(fr)? "F": "", FR_GETCOUNT(fr));

  task = find_task_by_vpid(fr->st_buf.pid);
  seq_printf(m, "   `-< %-12s", task? task->comm: "X");
  l_show_st(m, task? &fr->st_buf: NULL);

  for(i = 0; i < FRSTMAXREF; i++) {
    if(!fr->st_ref[i].pid)
      continue;

	task = find_task_by_vpid(fr->st_ref[i].pid);
	seq_printf(m, "   `-> %-12s", task? task->comm: "X");
	l_show_st(m, task? &fr->st_ref[i]: NULL);
  }

  seq_printf(m, "                   ----------------"
			 "-----------------------------\n");
  return 0;
}

static struct seq_operations fr_swap_seq_op = {
  .start = l_start,
  .next  = l_next,
  .stop  = l_stop,
  .show  = l_show
};

static struct seq_operations fr_info_seq_op = {
  .start = l_start_info,
  .next  = l_next,
  .stop  = l_stop,
  .show  = l_show_info
};

static int fr_seq_open(struct inode *inode, struct file *file)
{
  if(strcmp(file->f_path.dentry->d_name.name, "fr_info") == 0)
	  return seq_open(file, &fr_info_seq_op);
  return seq_open(file, &fr_swap_seq_op);
}

static struct file_operations fr_seq_fops = {
  .open = fr_seq_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = seq_release,
};

static int __init fr_seq_init(void)
{
  proc_create("fr_swap", 0, NULL, &fr_seq_fops);
  proc_create("fr_info", 0, NULL, &fr_seq_fops);
  return 0;
}

static void __exit fr_seq_exit(void)
{
  remove_proc_entry("fr_swap", NULL);
  remove_proc_entry("fr_info", NULL);
}

module_init(fr_seq_init);
module_exit(fr_seq_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("warits <warits.wang@infotm.com>");
MODULE_DESCRIPTION("fr manages ring-buffers of frames");
