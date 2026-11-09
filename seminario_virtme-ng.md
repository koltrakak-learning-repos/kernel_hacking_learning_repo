vmg

- condivide con l'host tutto
    - file system
    - os
- cambia solo il kernel

NB: le modifiche non sono permanenti

```dipendenze:
sudo pacman -S --needed qemu-base virtiofsd busybox base-devel bc cpio pahole libelf
```

...

incredibile carico il nuovo kernel in un secondo

- tolgo un sacco di step intermedi
- posso compilare da host
- eseguo su una vm qemu che mi fa da sandbox

...

ci sono degli scripts per generare il config

# Comandi utili

tutti da dentro linux/

- `vng -b` o `vng --build`
    - builda il kernel
    - if a .config is not available virtme-ng will automatically create a minimum .config with all the required feature to boot the instance

- `export KBUILD_OUTPUT=.virtme/build && vng --build`
    - Build the kernel using a separate build directory
        - **NB**: .virtme/build è il nome della cartella di build (a quanto pare la convenzione è lasciarla nascosta)
    - puoi usare O= se non hai voglia di ricordarti la env-var: `vng --build -- O=.virtme/build`
        - ... però per eseguire: `vng -- O=.virtme/build`

- `vng --kconfig`
    - Generate a minimal kernel .config in the current kernel build directory:
    - `vng --configitem CONFIG_EXT4_FS=y`
        - adds a CONFIG_ITEM=val overriding previous .config setting

- `vng`
    - esegue il kernel buildato nella directory corrente
    - `vng -- uname -r | cowsay`
        - specifying uname -r after **--** executes the command **inside a kernel recompiled in the current directory**
        - piping sends the output to cowsay **on the host**:

`PATH="/usr/lib/ccache:${PATH}" KBUILD_BUILD_TIMESTAMP=0 vng --build`
    - Accelerate the kernel rebuilds using CCache (if installed):

TODO:

- cpu configuration
- guest networking
- ssh
- debugging, gdb and memory dumps