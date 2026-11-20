#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <uapi/asm-generic/errno-base.h>

MODULE_DESCRIPTION("hello sysfs");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");

static int integer = 0;

static ssize_t foo_show(struct kobject *kobj, const struct kobj_attribute *attr,
			char *buf)
{
	return sysfs_emit(buf, "hello world from sysfs. Variable: %d\n", integer);
}

static ssize_t foo_store(struct kobject *kobj, const struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int ret = kstrtoint(buf, 10, &integer);
	if (ret < 0)
		return ret;

    pr_alert("hello_sysfs: la variable ora vale: %d\n", integer);

	return count;
}

/* Sysfs attributes cannot be world-writable. */
static const struct kobj_attribute hello_attribute =
	__KOBJ_ATTR(hello, 0664, foo_show, foo_store);

static struct kobject *hello_sysfs_kobj;

static int __init example_init(void)
{
	// Create a kobject with the name of "hello_sysfs", located under /sys/kernel/
	hello_sysfs_kobj = kobject_create_and_add("hello_sysfs", kernel_kobj);
	if (!hello_sysfs_kobj)
		return -ENOMEM;

	// Create the attribute files associated with this kobject
	int retval = sysfs_create_file(hello_sysfs_kobj, &hello_attribute.attr);
	if (retval)
		kobject_put(hello_sysfs_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(hello_sysfs_kobj);
}

module_init(example_init);
module_exit(example_exit);
