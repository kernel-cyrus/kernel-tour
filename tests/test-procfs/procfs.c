#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_procfs_init(void)
{
	pr_tour("Module init: test-procfs");
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