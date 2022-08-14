#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_hello_init(void)
{
	pr_tour("Module init: test-hello");
	return 0;
}

static void test_hello_exit(void)
{
	pr_tour("Module exit: test-hello");
	return;
}

late_initcall(test_hello_init);
module_exit(test_hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");