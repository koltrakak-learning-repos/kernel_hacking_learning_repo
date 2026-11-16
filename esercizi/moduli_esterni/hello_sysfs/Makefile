obj-m   := hello_sysfs.o 

# KDIR    := /lib/modules/$(shell uname -r)/build
KDIR    := /home/kevin/GitRepos/kernel_hacking_learning_repo/linux/build
# per passare KBUILD_EXTRA_SYMBOLS nel caso il tuo modulo dipendesse da simboli 
# esportati da un altro modulo esterno).
SYM=$(PWD) 
 
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

