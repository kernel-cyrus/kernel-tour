#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_memblock_init(void)
{
	pr_tour("Module init: test-memblock");
	return 0;
}

static void test_memblock_exit(void)
{
	pr_tour("Module exit: test-memblock");
	return;
}

late_initcall(test_memblock_init);
module_exit(test_memblock_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");