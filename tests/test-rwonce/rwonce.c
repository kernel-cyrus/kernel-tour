#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_rwonce_init(void)
{
	pr_tour("Module init: test-rwonce");
	return 0;
}

static void test_rwonce_exit(void)
{
	pr_tour("Module exit: test-rwonce");
	return;
}

late_initcall(test_rwonce_init);
module_exit(test_rwonce_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");