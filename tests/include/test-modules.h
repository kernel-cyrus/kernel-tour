#ifndef __TEST_MODULES_H__
#define __TEST_MODULES_H__

#include <linux/printk.h>

#define pr_tour(fmt, ...) \
	pr_info("kernel-tour: " fmt, ##__VA_ARGS__)

#endif