Questo documento di testo contiene istruzioni e dettagli tecnici per una delle attività del corso di kernel hacking. Per essere guidato nel corso, inizia dalla presentazione kernel-hacking-1-101.

---

## Soluzione 1. Nuova subdirectory dedicata

Nel Kconfig padre:
```kconfig
+source "drivers/modulo_custom/Kconfig"
```

Nel Makefile padre:
```make
+obj-$(CONFIG_HELLO_MODULE) += modulo_custom/
```

Kconfig nella cartella:
```kconfig
menu "Menu per il modulo custom"
config HELLO_MODULE
	tristate "Modulo custom"
	help
	  Modulo custom che stampa delle frasi nel log di sistema.
endmenu
```

Makefile nella cartella:
```make
obj-$(CONFIG_HELLO_MODULE) += hello_module.o
```

Sorgente modulo nella cartella:
```c
#define pr_fmt(fmt) 	KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>

static __init int hello_init(void) # la direttiva __init serve per far sì che
										# la memoria sia liberata dopo l'esecuzione
										# della funzione (non serve più tenerla in memoria)
										# per il caso built-in
{
	pr_info("Modulo custom inserito\n");
	return 0;
}

static __exit void hello_exit(void) # la direttiva __exit serve per far sì che
										# la funzione non sia neanche compilata
										# per il caso built-in (perché non sarà mai eseguita)
{
	pr_info("Modulo custom rimosso\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Il mio modulo custom");
MODULE_VERSION("0.1");
```

---

## Soluzione 2. Integrazione in subdirectory esistente

Nel Kconfig, al posto del source di sopra, modificare così:
```kconfig
+config HELLO_MODULE
+	tristate "Modulo custom"
+	help
+	  Modulo custom che stampa delle frasi nel log di sistema.
```

Nel Makefile:
```make
+obj-$(CONFIG_HELLO_MODULE) += hello_module.o
```