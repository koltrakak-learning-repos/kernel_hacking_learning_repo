#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h> 

MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");
 
static int __init mem_info_init(void) { 
    printk("hello world\n"); 

    return 0;
} 
static void __exit mem_info_exit(void) { 
    printk("Goodbye world\n"); 
} 
 
module_init(mem_info_init); 
module_exit(mem_info_exit);