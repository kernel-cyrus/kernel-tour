#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/stat.h>

#include "../include/test-modules.h"

/*
 *  To run this test
 *  Append "test_bootargs=hello params.test_int=1 params.test_string=hello    \
 *  params.test_array=1,2,3,4 test_core=100" to cmdline
 */

static int test_core;
static int test_int;
static int test_array[4];
static char *test_string;
static char test_bootargs[16];
static int num;

static int test_params_init(void)
{
	int i;

	pr_tour("Module init: test-params");
	pr_tour("test-params: bootargs: %s", test_bootargs);
	pr_tour("test-params: int: %d, string: %s", test_int, test_string);
	for (i = 0; i < 3 && i < num; i++)
		pr_tour("test-params: array[%d]: %d", i, test_array[i]);
	pr_tour("test-params: core: %d", test_core);
	return 0;
}

static void test_params_exit(void)
{
	pr_tour("Module exit: test-params");
	return;
}

static int parse_test_bootargs_param(char *str)
{
	strncpy(test_bootargs, str, 16);
	return 0;
}

early_param("test_bootargs", parse_test_bootargs_param);

core_param(test_core, test_core, int, S_IRUSR | S_IWUSR);

module_param(test_int, int, S_IRUSR | S_IWUSR);
module_param(test_string, charp, S_IRUSR | S_IWUSR);
module_param_array(test_array, int, &num, S_IRUSR | S_IWUSR);

late_initcall(test_params_init);
module_exit(test_params_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");