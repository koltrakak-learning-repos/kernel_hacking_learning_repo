#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_DESCRIPTION("module that creates a sysfs kobject with a bunch of attributes that cause an oops in various ways");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");

// -------------------------------------------------------------------------- //
// null pointer dereference oops
// -------------------------------------------------------------------------- //

static ssize_t crash_nullptr_show(struct kobject *kobj, const struct kobj_attribute *attr,
			char *buf) {
	// bomba
	int *a = 0;
	*a = 100;
	return sysfs_emit(buf, "qua non ci arrivo");
}

static const struct kobj_attribute crash_nullptr_attribute =
	__KOBJ_ATTR(null_ptr_deref_crash, 0664, crash_nullptr_show, NULL);


// -------------------------------------------------------------------------- //
// bug on oops
// -------------------------------------------------------------------------- //

static ssize_t crash_bug_on_show(struct kobject *kobj,
                                 const struct kobj_attribute *attr, char *buf) {
    // oppure direttamente BUG()
    BUG_ON(true);
    return sysfs_emit(buf, "qua non ci arrivo");
}

static const struct kobj_attribute crash_bug_on_attribute =
	__KOBJ_ATTR(bug_on_crash, 0664, crash_bug_on_show, NULL);


// -------------------------------------------------------------------------- //
// arm a timer that causes an oops after a while
// -------------------------------------------------------------------------- //

static struct timer_list boom_timer;

static void boom_timer_callback(struct timer_list *t) {
    pr_emerg("hello_oops: timer scaduto, provoco OOPS...\n");
    BUG();
}

static ssize_t crash_timer_show(struct kobject *kobj,
                                const struct kobj_attribute *attr, char *buf) {
    unsigned int delay_ms = 10000;
	boom_timer.function = boom_timer_callback;
	boom_timer.expires = jiffies + msecs_to_jiffies(delay_ms);
	add_timer(&boom_timer);

    return sysfs_emit(buf, "ho settato un timer lungo %u ms", delay_ms);
}


static const struct kobj_attribute crash_timer_attribute =
	__KOBJ_ATTR(timer_crash, 0664, crash_timer_show, NULL);


// -------------------------------------------------------------------------- //
// module stuff that creates and destroys the kobject
// -------------------------------------------------------------------------- //

static struct kobject *crash_kobj;

static int __init example_init(void)
{
	// Create a kobject with the name of "crash_sysfs", located under /sys/kernel/
	crash_kobj = kobject_create_and_add("crash_kernel", kernel_kobj);
	if (!crash_kobj)
		return -ENOMEM;

	// Create the attribute files associated with this kobject
	int retval = sysfs_create_file(crash_kobj, &crash_nullptr_attribute.attr);
	if (retval)
		kobject_put(crash_kobj);

	// Create the attribute files associated with this kobject
	retval = sysfs_create_file(crash_kobj, &crash_bug_on_attribute.attr);
	if (retval)
		kobject_put(crash_kobj);

	retval = sysfs_create_file(crash_kobj, &crash_timer_attribute.attr);
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
