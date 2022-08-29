#include <linux/init.h>
#include <linux/module.h>
#include <linux/percpu.h>
#include <linux/delay.h>
#include <linux/smp.h>

#include "../include/test-modules.h"

DEFINE_PER_CPU(int, count);

static int test_percpu_init(void)
{
	int cpu;
	int *cnt;
	int __percpu *tst;

	pr_tour("Module init: test-percpu");

	/* static percpu variable */
	for (int i = 0; i <=100; i++) {
		cpu = get_cpu();
		cnt = &get_cpu_var(count);
		(*cnt)++;
		put_cpu_var(count);
		pr_tour("Static, CPU: %d, COUNT: %d", cpu, *cnt);
		put_cpu();
		msleep(100);
	}

	/* alloc percpu variable */
	tst = alloc_percpu(int);
	for_each_possible_cpu(cpu) {
		cnt = per_cpu_ptr(tst, cpu);
		pr_tour("Dynamic, CPU: %d, COUNT: %d", cpu, *cnt);
	}

	/* current percpu data */
	cpu = get_cpu();
	cnt = raw_cpu_ptr(tst);
	pr_tour("Current, CPU: %d, COUNT: %d", cpu, *cnt);
	put_cpu();
	
	return 0;
}

static void test_percpu_exit(void)
{
	pr_tour("Module exit: test-percpu");
	return;
}

late_initcall(test_percpu_init);
module_exit(test_percpu_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");