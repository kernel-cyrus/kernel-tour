#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include "../include/test-modules.h"

DEFINE_SPINLOCK(a_lock);

spinlock_t b_lock;

static int test_spinlock_init(void)
{
	unsigned long flags;

	pr_tour("Module init: test-spinlock");

	spin_lock_init(&b_lock);

	spin_lock_irqsave(&a_lock, flags);
	spin_lock(&b_lock);

	/*
	 * BUG: scheduling while atomic:
	 * 1 lock held by swapper/0/1
	 * #0: ffff80000b308850 (&b_lock){+.+.}-{2:2}, at: test_spinlock_init+0x44/0x68
	 */
	// ssleep(1);

	/*
	 * WARNING: possible recursive locking detected
	 * swapper/0/1 is trying to acquire lock:
	 * ffff80000b308850 (&b_lock){+.+.}-{2:2}, at: test_spinlock_init+0x4c/0x68
	 * but task is already holding lock:
	 * ffff80000b308850 (&b_lock){+.+.}-{2:2}, at: test_spinlock_init+0x44/0x68
	 */
	//spin_lock(&b_lock);

	spin_unlock_irqrestore(&a_lock, flags);
	spin_unlock(&b_lock);
	
	return 0;
}

static void test_spinlock_exit(void)
{
	pr_tour("Module exit: test-spinlock");
	return;
}

late_initcall(test_spinlock_init);
module_exit(test_spinlock_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");