#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>

#include "../include/test-modules.h"

static int test_string_init(void)
{
	const char str1[] = "123456789";
	const char str2[] = "123456789012345";
	char buf[10];
	char *pstr;
	int count;

	pr_tour("Module init: test-string");
	
	pr_tour("%s\n", str1);
	pr_tour("%s\n", str2);

	count = strlen(str1);
	pr_tour("%d\n", count);

	pstr = strcpy(buf, str1);
	pr_tour("%s\n", pstr);

	memset(buf, 0, sizeof(buf));
	pstr = strncpy(buf, str2, sizeof(buf)-1);
	pr_tour("%s\n", pstr);

	count = strlcpy(buf, str2, sizeof(buf));
	pr_tour("%d\n", count);	// return 15

	count = strscpy(buf, str2, sizeof(buf));
	pr_tour("%d\n", count);	// return -7, means overflow

	count = strscpy(buf, str1, sizeof(buf));
	pr_tour("%d\n", count);	// return 9

	return 0;
}

static void test_string_exit(void)
{
	pr_tour("Module exit: test-string");
	return;
}

late_initcall(test_string_init);
module_exit(test_string_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");