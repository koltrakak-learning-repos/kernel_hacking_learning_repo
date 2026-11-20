#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <uapi/asm-generic/errno-base.h>

MODULE_DESCRIPTION("module that creates a sysfs kobject attribute which contains a null pointer dereference in the show method");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");

static ssize_t crash_show(struct kobject *kobj, const struct kobj_attribute *attr,
			char *buf)
{
	// bomba
	int *a = 0;
	*a = 100;

	return sysfs_emit(buf, "qua non ci arrivo");
}

static ssize_t crash_store(struct kobject *kobj, const struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	// dummy

	return count;
}

/* Sysfs attributes cannot be world-writable. */
static const struct kobj_attribute crash_attribute =
	__KOBJ_ATTR(null_ptr_deref_crash, 0664, crash_show, crash_store);

static struct kobject *crash_kobj;

static int __init example_init(void)
{
	// Create a kobject with the name of "crash_sysfs", located under /sys/kernel/
	crash_kobj = kobject_create_and_add("crash_sysfs", kernel_kobj);
	if (!crash_kobj)
		return -ENOMEM;

	// Create the attribute files associated with this kobject
	int retval = sysfs_create_file(crash_kobj, &crash_attribute.attr);
	if (retval)
		kobject_put(crash_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(crash_kobj);
}

module_init(example_init);
module_exit(example_exit);
