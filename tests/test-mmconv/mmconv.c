#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_mmconv_init(void)
{
	pr_tour("Module init: test-mmconv");
	return 0;
}

static void test_mmconv_exit(void)
{
	pr_tour("Module exit: test-mmconv");
	return;
}

late_initcall(test_mmconv_init);
module_exit(test_mmconv_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");