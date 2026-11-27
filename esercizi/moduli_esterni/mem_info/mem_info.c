#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/mm.h>
#include <linux/mm_types.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kevin Koltraka");
MODULE_DESCRIPTION("Display virtual memory areas of the current process");

// Investigate the structures 
//
// - struct task_struct 
// - struct mm_struct (memory management struct)
// - struct vm_area_struct
// 
// A memory area is indicated by a structure of type struct vm_area_struct.


static int __init mem_info_init(void) { 
    struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;

    pr_info("kk - mem_info: VMAs for %s (pid %d)\n", current->comm, current->pid);

	VMA_ITERATOR(vmi, mm, 0);
	for_each_vma(vmi, vma) {
        // TODO: possiamo anche provare a esplorare i campi di vm_area_struct
        // per stampare informazioni aggiuntive
        // - l'area ha un nome? (e.g. heap?)
        // - flag associati all'area (r, w, x, ...)
        // - refcount di un area?
		pr_info("  0x%lx - 0x%lx\n", vma->vm_start, vma->vm_end);
	}

    return 0;
} 
static void __exit mem_info_exit(void) { 
    pr_info("kk - mem_info: Goodbye world\n"); 
} 
 
module_init(mem_info_init); 
module_exit(mem_info_exit);