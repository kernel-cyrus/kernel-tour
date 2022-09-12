#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>

#include "../include/test-modules.h"

static DEFINE_SEMAPHORE(test_sem_a);

static struct semaphore test_sem_b;

static int test_semaphore_init(void)
{
	pr_tour("Module init: test-semaphore");
	sema_init(&test_sem_b, 2);
	down(&test_sem_a);
	down(&test_sem_b);
	down(&test_sem_b);
	// dead lock, no error info
	// down(&test_sem_b);
	pr_tour("Critical Section");
	up(&test_sem_b);
	up(&test_sem_b);
	up(&test_sem_a);
	return 0;
}

static void test_semaphore_exit(void)
{
	pr_tour("Module exit: test-semaphore");
	return;
}

late_initcall(test_semaphore_init);
module_exit(test_semaphore_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");