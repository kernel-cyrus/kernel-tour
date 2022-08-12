#include <linux/init.h>
#include <linux/module.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "../include/test-modules.h"

struct test_data {
	struct list_head list;
	char *buf;
};

static struct test_data data = {
	.list = LIST_HEAD_INIT(data.list),
	.buf = "This is test data.\n"
};

static int single_show(struct seq_file *m, void *v)
{
	struct test_data *data = m->private;
	seq_printf(m, "Hello, procfs!\n");
	if (data)
		seq_printf(m, data->buf);
	return 0;
}

LIST_HEAD(test_list);

static DEFINE_SPINLOCK(test_list_lock);

static void *s_start(struct seq_file *m, loff_t *pos)
{
	spin_lock(&test_list_lock);
	seq_printf(m, "seq start.\n");
	return seq_list_start(&test_list, *pos);
}

static void *s_next(struct seq_file *m, void *p, loff_t *pos)
{
	
	return seq_list_next(p, &test_list, pos);
}

static void s_stop(struct seq_file *m, void *p)
{
	seq_printf(m, "seq stop.\n");
	spin_unlock(&test_list_lock);
}

static int s_show(struct seq_file *m, void *p)
{
	struct test_data *data;
	data = list_entry(p, struct test_data, list);
	if (data)
		seq_printf(m, data->buf);
	return 0;
}

static const struct seq_operations seq_ops = {
	.start	= s_start,
	.next	= s_next,
	.stop	= s_stop,
	.show	= s_show,
};

static char write_buf[32];

static DEFINE_SPINLOCK(test_write_lock);

/* pos is file pin */
static ssize_t mywrite(struct file *file, const char __user *ubuf, size_t ubuf_size, loff_t *ppos) 
{
	spin_lock(&test_write_lock);
	if(ubuf_size > 32)
		return -EFAULT;
	if(copy_from_user(write_buf, ubuf, sizeof(write_buf)))
		return -EFAULT;
	*ppos = strlen(write_buf);
	spin_unlock(&test_write_lock);
	return *ppos;
}

static ssize_t myread(struct file *file, char __user *ubuf, size_t ubuf_size, loff_t *ppos) 
{
	spin_lock(&test_write_lock);
	if(ubuf_size < 32)
		return -EFAULT;
	if(copy_to_user(ubuf, write_buf, sizeof(write_buf)))
		return -EFAULT;
	*ppos = strlen(write_buf);
	spin_unlock(&test_write_lock);
	return *ppos;
}

static struct proc_ops write_ops = 
{
	.proc_read = myread,
	.proc_write = mywrite,
};

static int test_procfs_init(void)
{
	struct proc_dir_entry *pde;
	struct test_data *p_data = &data;
	pr_tour("Module init: test-procfs");
	/* mkdir */
	pde = proc_mkdir("test-procfs", NULL);
	if (!pde)
		return 0;
	/* single */
	proc_create_single_data("test_single", S_IRUGO, pde, single_show, &data);
	/* seq */
	INIT_LIST_HEAD(&test_list);
	list_add(&p_data->list, &test_list);
	proc_create_seq("test_seq", S_IWUGO, pde, &seq_ops);
	/* node for input */
	proc_create("test_input", S_IALLUGO, pde, &write_ops);	// FIXME: not working.
	return 0;
}

static void test_procfs_exit(void)
{
	pr_tour("Module exit: test-procfs");
	return;
}

late_initcall(test_procfs_init);
module_exit(test_procfs_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");