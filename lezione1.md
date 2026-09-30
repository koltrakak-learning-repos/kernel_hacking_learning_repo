la documentazione è sparsa, difficile da reperire, outdated, ecc.

# Versionamento

https://docs.kernel.org/process/2.Process.html

\<major.minor.bugfix_counter\>

- prima di avere una release ufficiale ci sono delle **release candidates** che incorporano fix (e non nuove features) e rendono mano a mano più stabile una nuova release
- quando la frequenza di fix comincia ad abbassarsi e diventa stabile, linus torvarlds decide che abbiamo raggiunto uno stato sufficentemente stabile per avere una nuova minor version
- bugfix_counter comincia ad essere incrementato solo dopo una release (nuovo minor number)

versioni particolari

- EOL (end of line), non viene più mantenuta e non subirà più fix
- longterm invece subisce fix

NB: si cerca di essere sempre retrocompatibili (Linus ti ammazza)

# risorse

kernel.org

contiene:

- release
- source trees
- "documentazione" ufficiale autorevole
- mailing list
- bugzilla

# Albero dei sorgenti

varie top level dirs (linux kernel libs)


- firmware/ è il loro giro di parole per dire che è codice chiuso
- lib/ librerie generiche utilizzate dal resto del kernel
- samples/ esempi di uso del kernel
- script/ contiene script che fa funzionare il sistema di build

...

HAL (hardware abstraction layer)

- la roba hardware-specific risiede solamente dentro a arch/
- il resto è agnostico rispetto all'hardware

# /boot

le tre cose importanti che contiene sono

- initrd/initrams -> initial ram disk / initial ram file system
    - È un piccolo filesystem che il bootloader carica in RAM insieme al kernel.
    - Immagina che il tuo vero / sia su: /dev/nvme0n1p3; e che per poterlo leggere serva un driver per il disco
    - c'è un problema circolare: per caricare quei moduli devo leggere i moduli dal filesystem root, ma per leggere il filesystem root devo prima avere quei moduli.
    - L'initramfs risolve il problema.
- vmlinuz: immagine compressa del kernel


...

uname -a

```cosa significa immagine?

Un'immagine è praticamente la rappresentazione fisica sotto forma di blocco di byte di un qualcosa di più astratto

Ad esempio un file system

Oppure il kernel stesso, che solitamente è un processo in esecuzione

immagine significa semplicemente:

una rappresentazione binaria completa pronta per essere caricata in memoria.

Non è necessariamente un'immagine nel senso grafico.

È lo stesso concetto per cui puoi avere:

disk image
filesystem image
ISO image
kernel image

Sono rappresentazioni binarie che descrivono/contengono qualcosa che verrà caricato o interpretato.
```

...


regola d'oro da seguire in questo corso: **non usare i diritti di root se non strettamente necessario!!**

- rompi cose

# bootstrap

https://web.archive.org/web/20071011074709/http://www.ibm.com/developerworks/library/l-linuxboot/index.html

When a system is first booted, or is reset, the processor executes code at a well-known location. In a personal computer (PC), this location is in the basic input/output system (BIOS), which is stored in flash memory on the motherboard. 

In a PC, booting Linux begins in the BIOS at address 0xFFFF0. The first step of the BIOS is the power-on self test (POST). The job of the POST is to perform a check of the hardware. The second step of the BIOS is local device enumeration and initialization.

To boot an operating system, the BIOS runtime searches for devices that are both active and bootable in the order of preference defined. Commonly, Linux is booted from a hard disk, where the Master Boot Record (MBR) contains the primary boot loader. The MBR is a 512-byte sector, located in the first sector on the disk (sector 1 of cylinder 0, head 0). After the MBR is loaded into RAM, the BIOS yields control to it.

The secondary, or second-stage, boot loader could be more aptly called the kernel loader. The task at this stage is to load the Linux kernel and optional initial RAM disk.

The kernel image isn't so much an executable kernel, but a compressed kernel image. At the head of this kernel image is a routine that does some minimal amount of hardware setup and then decompresses the kernel contained within the kernel image and places it into high memory. 

- If an initial RAM disk image is present, this routine moves it into memory and notes it for later use. The routine then calls the kernel and the kernel boot begins.

... kernel initialization ...

- after this ends the kernel creates and runs the first init user process
    - and the idle_task for when there aren't any tasks left to schedule
    - this is done through a fork + exec combo
    - the father process executes the idle_task, the child process executes /sbin/init
- Al termine del proprio avvio, il kernel Linux avvierà un eseguibile di inizializzazione dello dello spazio utente
    - tipicamente /sbin/init, che è un link a /lib/systemd/sytemd

Tale eseguibile si occupa tipicamente di avviare l’intero sistema operativo, ossia: servizi, interfaccia utente

During the boot of the kernel, the initial-RAM disk (initrd) that was loaded into memory by the stage 2 boot loader is copied into RAM and mounted. This initrd serves as a temporary root file system in RAM and allows the kernel to fully boot without having to mount any physical disks.

Since the necessary modules needed to interface with peripherals can be part of the initrd, the kernel can be very small, but still support a large number of possible hardware configurations. After the kernel is booted, the root file system is pivoted (via pivot_root) where the initrd root file system is unmounted and the real root file system is mounted.

The initrd function allows you to create a small Linux kernel with drivers compiled as loadable modules. These loadable modules give the kernel the means to access disks and the file systems on those disks, as well as drivers for other hardware assets. Because the root file system is a file system on a disk, the initrd function provides a means of bootstrapping to gain access to the disk and mount the real root file system. In an embedded target without a hard disk, the initrd can be the final root file system.




- bios/uefi: sistema operativo base che sta in ROM che sceglie un drive da cui far partire il boot
- questo programma carica i byte che sono memorizzati dentro al MBR e imposta il PC all'indirizzo del primo byte del MBr
- questo primo byte punta al bootloader che fa echo a quello che fa il bios caricando l'immagine del kernel
- il kernel fa partire l'userspace


```With UEFI i don't have an MBR
Your system uses UEFI/GPT, not MBR: Your df -h output shows an active efivarfs mount at /sys/firmware/efi/efivars. This means your system boots using modern UEFI firmware rather than legacy BIOS/MBR. Modern systems use the GPT (GUID Partition Table) layout instead of a traditional MBR.
```

...

il kernel ha una head

- codice, non compresso, eseguito all'inizio appena dopo che il bootloader lo ha caricato in memoria
    - l'head contiene il codice per decomprimere il resto del kernel (start())

# elixir

esploratore per il codice sorgente

## head_64.s

...

## printing stuff

printk == printf del kernel

pr_notice == printk specializzata

- ci sono diversi livelli di criticità associati all stampe

modalità quiet

...

il kernel viene invocato come qualsiasi altro comando di una shell, **soltanto che stavolta l'ambiente è quello del bootloader**

il bootloader è colui che invoca il kernel e passo i cmdline parameters. Per cambiare i cmdline parameters del kernel bisogna modificare il file di configurazione di grub

### modalità menu di grub

ogni entry del menù è un insieme di righe di comando

- se modifico le righe di comando associate ad una entry al volo (tasto 'e') quelle modifiche sono effimere

# kernel log

il kernel non interagisce con l'utente, le printk vengono scritte nel kernel log -> un array circolare

interessante anche la strategia del bloccarsi quando sono pieno in questi sistemi produttore consumatore

...

**NB**: notiamo che c'è un altro /init nascosto oltre a systemd

- notiamo dei messaggi relativi ad un /init, e dopo un po' dei messaggi relativi a systemd

# Vari servizi che systemd fa partire

...

tyme sync

...

oom-deamon

- servizio che tramite euristiche sceglio quale processo uccidere quando la memoria finisce

...

```Differenze che noti quando fai il boot con il kernel che compili te
è tutto dovuto alla config minimale che creiamo per compilare il kernel

sostanzialmente stiamo disattivando cose che non dovremmo

è interessante anche sapere che c'è gente che di mestiere fa la configurazione di queste build del kernel per macchine/sistemi diversi
```