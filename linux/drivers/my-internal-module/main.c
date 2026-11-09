#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sono io!");      /* Who wrote this module? */
MODULE_DESCRIPTION("hello world");      /* What does this module do */


static int __init hello_2_init(void)
{
        printk(KERN_ALERT "Hello, world modificato tantissimo!\n");
        return 0;
}

static void __exit hello_2_exit(void)
{
        printk(KERN_ALERT "Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);