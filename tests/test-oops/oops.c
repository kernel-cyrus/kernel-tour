#include <linux/init.h>
#include <linux/module.h>
#include <linux/panic.h>
#include <linux/bug.h>

#include "../include/test-modules.h"

/*
 *  To run this test
 *  Append "test_panic" to cmdline
 */

static int go_panic = 0;

static int test_oops_init(void)
{
	pr_tour("Module init: test-oops");
	WARN_ON(go_panic);
	BUG_ON(go_panic);
	panic("test-oops: haha, I made this panic.");
	return 0;
}

static void test_oops_exit(void)
{
	pr_tour("Module exit: test-oops");
	return;
}

static int parse_panic_param(char *str)
{
	go_panic = 1;
	return 0;
}

early_param("test_panic", parse_panic_param);

late_initcall(test_oops_init);
module_exit(test_oops_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");