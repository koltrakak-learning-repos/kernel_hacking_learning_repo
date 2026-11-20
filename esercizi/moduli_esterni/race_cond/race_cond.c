#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <uapi/asm-generic/errno-base.h>

MODULE_DESCRIPTION("module that demonstrates a race condition");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");

static int integer = 0;

static ssize_t foo_show(struct kobject *kobj, const struct kobj_attribute *attr,
			char *buf)
{
	int buffer_position = sysfs_emit_at(buf, 0, "race_cond: variabile\t= %d\n", integer);
	integer++;
	buffer_position += sysfs_emit_at(buf, buffer_position, "race_cond: variabile+1\t= %d\n", integer);
	buffer_position += sysfs_emit_at(buf, buffer_position, "--------------------------\n");
	return buffer_position;
}

static ssize_t foo_store(struct kobject *kobj, const struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int ret = kstrtoint(buf, 10, &integer);
	if (ret < 0)
		return ret;

    pr_alert("race_cond: la variable ora vale: %d\n", integer);

	return count;
}

// NB: i file attributo hanno come owner root; inoltre, Sysfs attributes cannot 
// be world-writable. Questo significa che per scrivere devo fare cose come:
// 	$ echo "14" | sudo tee /sys/kernel/race_cond/race_cond
static const struct kobj_attribute race_attribute =
	__KOBJ_ATTR(race_cond, 0664, foo_show, foo_store);

static struct kobject *race_cond_kobj;

static int __init example_init(void)
{
	// Create a kobject with the name of "race_cond", located under /sys/kernel/
	race_cond_kobj = kobject_create_and_add("race_cond", kernel_kobj);
	if (!race_cond_kobj)
		return -ENOMEM;

	// Create the attribute files associated with this kobject
	int retval = sysfs_create_file(race_cond_kobj, &race_attribute.attr);
	if (retval)
		kobject_put(race_cond_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(race_cond_kobj);
}

module_init(example_init);
module_exit(example_exit);
