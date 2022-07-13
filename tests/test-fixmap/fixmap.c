#include <linux/init.h>
#include <linux/module.h>
#include <asm/fixmap.h>
#include <asm-generic/fixmap.h>

#include "../include/test-modules.h"

static int test_fixmap_init(void)
{
	unsigned long vaddr;
	unsigned long paddr = 0x40000000;

	pr_tour("Module init: test-fixmap");

	vaddr = set_fixmap_offset(FIX_EARLYCON_MEM_BASE, paddr);

	sprintf((char *)vaddr, "Hello, I hacked this buffer.");

	pr_tour("test-fixmap: fixmap-va: %lx, direct-va: %p, content: %s",
		vaddr, __va(paddr), (char*)__va(paddr));

	return 0;
}

static void test_fixmap_exit(void)
{
	pr_tour("Module exit: test-fixmap");
	return;
}

late_initcall(test_fixmap_init);
module_exit(test_fixmap_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");