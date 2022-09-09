#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#include "../include/test-modules.h"

static DEFINE_MUTEX(test_mutex_a);

static int test_mutex_init(void)
{
	static struct mutex test_mutex_b;
	pr_tour("Module init: test-mutex");
	mutex_init(&test_mutex_b);
	mutex_lock(&test_mutex_a);
	mutex_lock(&test_mutex_b);
	pr_tour("Critical Section.");
	pr_tour("current process: %p", current);
	// sleep in mutex
	msleep(1000);
	// dead lock, without any errors, even enable LOCK DEBUG?
	mutex_lock(&test_mutex_a);
	// break here to see lock->owner
	mutex_unlock(&test_mutex_b);
	mutex_unlock(&test_mutex_a);
	return 0;
}

static void test_mutex_exit(void)
{
	pr_tour("Module exit: test-mutex");
	return;
}

late_initcall(test_mutex_init);
module_exit(test_mutex_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");