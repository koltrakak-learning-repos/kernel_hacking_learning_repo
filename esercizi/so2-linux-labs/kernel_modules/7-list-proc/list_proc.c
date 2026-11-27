#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_DESCRIPTION("List current processes");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

static int my_proc_init(void)
{
	struct task_struct *p;
	pr_info("kk-init: current task:\t[%d] %s\n", current->pid, current->comm);
	pr_info("kk-init: -----------------------------------\n");

	pr_info("kk-init: all tasks\n");
	for_each_process(p) {
		pr_info("kk-init:\t[%d] %s\n", p->pid, p->comm);
	}
	pr_info("kk-init: -----------------------------------\n");

	return 0;
}

static void my_proc_exit(void)
{
	pr_info("kk-exit: current task:\t[%d] %s\n", current->pid, current->comm);
	pr_info("kk-exit: -----------------------------------\n");
}

module_init(my_proc_init);
module_exit(my_proc_exit);
