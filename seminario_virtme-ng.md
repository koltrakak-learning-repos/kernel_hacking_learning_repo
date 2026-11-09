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