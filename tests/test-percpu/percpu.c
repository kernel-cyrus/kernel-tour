#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_percpu_init(void)
{
	pr_tour("Module init: test-percpu");
	return 0;
}

static void test_percpu_exit(void)
{
	pr_tour("Module exit: test-percpu");
	return;
}

late_initcall(test_percpu_init);
module_exit(test_percpu_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");