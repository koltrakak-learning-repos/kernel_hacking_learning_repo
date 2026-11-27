ccflags-y = -Wno-unused-function -Wno-unused-label -Wno-unused-variable
obj-m   := list_proc.o

# KDIR    := /lib/modules/$(shell uname -r)/build
KDIR := /home/kevin/GitRepos/kernel_hacking_learning_repo/linux/build
# per passare KBUILD_EXTRA_SYMBOLS nel caso il tuo modulo dipendesse da simboli 
# esportati da un altro modulo esterno).
SYM  := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

modules_install:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules_install

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean