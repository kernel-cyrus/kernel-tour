#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>

#include "../include/test-modules.h"

struct my_data_type {
	struct kobject kobj;
	char data[32];
};

struct my_attr_type {
	struct attribute attr;
	ssize_t (*show)(struct my_data_type *my_data, struct my_attr_type *my_attr,
		    char *buf);
	ssize_t (*store)(struct my_data_type *my_data, struct my_attr_type *my_attr,
		    const char *buf, size_t count);
};

/* Create kobject with show */

static struct my_data_type my_data = {
	.data = "This is my data."
};

static ssize_t test_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	ssize_t ret = -EIO;
	struct my_data_type* my_data = container_of(kobj, struct my_data_type, kobj);
	struct my_attr_type* my_attr = container_of(attr, struct my_attr_type, attr);
	ret = sysfs_emit(buf, "hello sysfs.\n");
	if (my_attr->show)
		ret += my_attr->show(my_data, my_attr, buf);
	return ret;
}

static struct sysfs_ops test_ops = {
	.show = test_show,
	.store = NULL
};

static struct kobj_type my_ktype = {
	.sysfs_ops = &test_ops
};

/* Create attrs, attr show should be dispatched from kobject show */

static ssize_t test_attr_show(struct my_data_type *my_data, struct my_attr_type *my_attr, char *buf)
{
	return sysfs_emit(buf, "%s\n", my_data->data);
}

static struct my_attr_type test_attr = __ATTR_RO(test_attr);

static int test_sysfs_init(void)
{
	pr_tour("Module init: test-sysfs");
	// Create my_policy directory, add show function for all attrs.
	kobject_init_and_add(&my_data.kobj, &my_ktype, kernel_kobj, "my_policy");
	// Add attr and dispatch show to attr_show
	sysfs_create_file(&my_data.kobj, &test_attr.attr);
	return 0;
}

static void test_sysfs_exit(void)
{
	pr_tour("Module exit: test-sysfs");
	return;
}

late_initcall(test_sysfs_init);
module_exit(test_sysfs_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");