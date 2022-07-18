#include <linux/init.h>
#include <linux/module.h>

#include "../include/test-modules.h"

static int test_mmu_init(void)
{
	pr_tour("Module init: test-mmu");
	return 0;
}

static void test_mmu_exit(void)
{
	pr_tour("Module exit: test-mmu");
	return;
}

late_initcall(test_mmu_init);
module_exit(test_mmu_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");