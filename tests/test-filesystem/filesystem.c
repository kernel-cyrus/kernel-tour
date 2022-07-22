#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#include "../include/test-modules.h"

static void test_kill_sb(struct super_block *sb)
{
	pr_tour("Kill sb.");
}

static struct dentry *test_mount(struct file_system_type *fs_type,
                             int flags, const char *dev_name,
                             void *data)
{
	pr_tour("Mounting: %s", dev_name);
	return 0
}

static struct file_system_type test_filesystem = {
	.name		= "testfs",
	.mount		= test_mount,
	.kill_sb	= test_kill_sb
};

static int test_filesystem_init(void)
{
	pr_tour("Module init: test-filesystem");
	register_filesystem(&test_filesystem);
	return 0;
}

static void test_filesystem_exit(void)
{
	pr_tour("Module exit: test-filesystem");
	return;
}

late_initcall(test_filesystem_init);
module_exit(test_filesystem_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");