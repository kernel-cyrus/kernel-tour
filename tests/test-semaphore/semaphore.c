#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>

#include "../include/test-modules.h"

static DEFINE_SEMAPHORE(test_sem_a);

static struct semaphore test_sem_b;

static DECLARE_RWSEM(test_rwsem_a);

static struct rw_semaphore test_rwsem_b;

static int test_semaphore_init(void)
{
	pr_tour("Module init: test-semaphore");

	pr_tour("Test Semaphore Start");
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
	pr_tour("Test Semaphore Finish");

	pr_tour("Test RW-Semaphore Start");
	init_rwsem(&test_rwsem_b);
	down_read(&test_rwsem_a);
	down_read(&test_rwsem_a);
	down_read(&test_rwsem_b);
	down_read(&test_rwsem_b);
	pr_tour("Read Critical Section");
	up_read(&test_rwsem_b);
	up_read(&test_rwsem_b);
	up_read(&test_rwsem_a);
	up_read(&test_rwsem_a);
	down_write(&test_rwsem_a);
	pr_tour("Write Critical Section");
	up_write(&test_rwsem_a);

	down_read(&test_rwsem_a);
	// dead lock, no error info
	// down_write(&test_rwsem_a);
	// up_write(&test_rwsem_a);
	up_read(&test_rwsem_a);

	pr_tour("Test RW-Semaphore Finish");

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