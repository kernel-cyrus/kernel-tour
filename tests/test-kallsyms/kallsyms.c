#include <linux/init.h>
#include <linux/module.h>
#include <linux/kallsyms.h>

#include "../include/test-modules.h"

static int test_kallsyms_init(void)
{
	char name[32];
	unsigned long addr;

	pr_tour("Module init: test-kallsyms");

	addr = kallsyms_lookup_name("start_kernel");
	pr_tour("%px - %ps - %pS", (void*)addr, (void*)addr, (void*)addr);

	lookup_symbol_name(addr, name);
	pr_tour("%s", name);

	print_ip_sym("NOTE: ", addr);

	pr_tour("Dump current stack:");
	dump_stack();

	return 0;
}

static void test_kallsyms_exit(void)
{
	pr_tour("Module exit: test-kallsyms");
	return;
}

late_initcall(test_kallsyms_init);
module_exit(test_kallsyms_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");