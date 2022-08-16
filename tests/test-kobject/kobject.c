#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "../include/test-modules.h"

static struct kset 	*test_kset;
static struct kobject 	*test_kobj;

static const char *kobj_uevent_name(struct kobject *kobj)
{
	return kobject_name(kobj);
}

static int kobj_uevent(struct kobject *kobj, struct kobj_uevent_env *env)
{
	return 0;
}

static const struct kset_uevent_ops uevent_ops = {
	.name =		kobj_uevent_name,
	.uevent =	kobj_uevent,
};

static ssize_t test_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sysfs_emit(buf, "hello kobject.\n");
}

static struct kobj_attribute test_attribute =
	__ATTR(test, 0664, test_show, NULL);

static struct attribute *attrs[] = {
	&test_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int test_kobject_init(void)
{
	int ret;
	
	pr_tour("Module init: test-kobject");

	// create kset under /sys/kernel/
	test_kset = kset_create_and_add("test_kset", &uevent_ops, kernel_kobj);
	if (!test_kset)
		goto kset_err;

	test_kobj = kobject_create_and_add("test_kobject", &test_kset->kobj);
	if (!test_kobj)
		goto kobj_err;

	ret = sysfs_create_group(test_kobj, &attr_group);
	if (ret)
		goto kobj_err;

	return 0;

 kobj_err:
	kobject_put(test_kobj);
 kset_err:
	kset_unregister(test_kset);

	return -ENOMEM;
}

static void test_kobject_exit(void)
{
	pr_tour("Module exit: test-kobject");
	kobject_put(test_kobj);
	kset_unregister(test_kset);
	return;
}

late_initcall(test_kobject_init);
module_exit(test_kobject_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Cyrus Huang");