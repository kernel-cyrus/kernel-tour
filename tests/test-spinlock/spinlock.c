#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_spinlock_init(void)
{
	pr_tour("Module init: test-spinlock");
	return 0;
}

static void test_spinlock_exit(void)
{
	pr_tour("Module exit: test-spinlock");
	return;
}

late_initcall(test_spinlock_init);
module_exit(test_spinlock_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");