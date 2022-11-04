#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_timer_init(void)
{
	pr_tour("Module init: test-timer");
	return 0;
}

static void test_timer_exit(void)
{
	pr_tour("Module exit: test-timer");
	return;
}

late_initcall(test_timer_init);
module_exit(test_timer_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");