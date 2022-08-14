#include <linux/init.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>

#include "../include/test-modules.h"

static struct dentry *test_dir;

static char test_buf[32] = "This is a test line.\n";

static char *test_str = test_buf; // create str need pointer variable address

static unsigned long test_ulong;

static int test_single_show(struct seq_file *m, void *v)
{
	char *data = m->private;
	seq_printf(m, "Hello, debugfs!\n");
	if (data)
		seq_printf(m, data);
	return 0;
}
DEFINE_SHOW_ATTRIBUTE(test_single); // create file ops with seq file

static int test_debugfs_init(void)
{
	pr_tour("Module init: test-debugfs");
	test_dir = debugfs_create_dir("test-debugfs", NULL);
	debugfs_create_file("test-file", 0444, test_dir, test_buf, &test_single_fops);
	debugfs_create_ulong("test-ulong", 0644, test_dir, &test_ulong);
	debugfs_create_str("test-str", 0644, test_dir, &test_str);
	return 0;
}

static void test_debugfs_exit(void)
{
	pr_tour("Module exit: test-debugfs");
	return;
}

late_initcall(test_debugfs_init);
module_exit(test_debugfs_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");