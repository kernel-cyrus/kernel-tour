#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#include "../include/test-modules.h"

static const struct super_operations test_super_ops = {
	.statfs		= NULL,
	.drop_inode	= NULL,
	.show_options	= NULL,
};

static int test_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode = new_inode(sb);

	init_special_inode(inode, NULL, 0);

	sb->s_op = test_super_ops;

	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}


static void test_kill_sb(struct super_block *sb)
{
	pr_tour("Kill sb.");
}

static struct dentry *test_mount(struct file_system_type *fs_type,
                             int flags, const char *dev_name,
                             void *data)
{
	pr_tour("Mounting: %s", dev_name);
	return mount_nodev(fs_type, flags, dev_name, data, test_fill_super);
}

static struct file_system_type test_filesystem = {
	.name		= "testfs",
	.mount		= test_mount,
	.kill_sb	= test_kill_sb,
	.fs_flags	= 0
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