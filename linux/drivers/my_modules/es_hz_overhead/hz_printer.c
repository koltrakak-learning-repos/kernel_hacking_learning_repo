#include <linux/module.h>       // Needed by all modules
#include <linux/kernel.h>       // Needed for KERN_INFO
#include <linux/init.h>         // Needed for the module macros
#include <asm-generic/param.h>  // per HZ

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Koltraka Kevin");
MODULE_DESCRIPTION("");

static int __init hz_init(void) {
    pr_alert("hz_printer: %d\n", HZ);
    return 0;
}

static void __exit hz_exit(void) {
    pr_alert("hz_printer: sono stato rimosso\n");
}

module_init(hz_init);
module_exit(hz_exit);