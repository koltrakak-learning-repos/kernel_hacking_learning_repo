# KBuild

kbuild è il build system del kernel linux basato su make. Gestisce:

- Creazione dell’immagine del kernel
- Creazione dei moduli
- Creazione dell’initramfs
- Installazione del tutto (in /boot, /lib/modules, etc...)

Le operazioni eseguite make sono stabilite attraverso una combinazione di file:

- Un file di configurazione del kernel (.config)
    - costituito da una sequenza di righe rappresentanti coppie chiave-valore
    - ogni riga rappresenta una opzione di configurazione
    - **Il .config deve essere presente nell’albero di build del kernel** (di default, l’albero di build coincide con l’albero dei sorgenti)
- Un albero di makefile
    - Abbiamo un top Makefile, presente nella radice dell’albero dei sorgenti e un Makefile per ogni sottodirectory dell’albero dei sorgenti.
    - Quasi in ogni caso:
        - Le sottodirectory contengono file sorgenti dai quali si possono generare uno o più moduli
        - Il makefile di una sottodirectory è un kbuild Makefile per tali moduli, ossia contiene le istruzioni per generare tali moduli
- Un albero di file di configurazione
    - file di configurazione che definiscono l’insieme (**database**) di opzioni di configurazione
        - questo database definisce le opzioni utilizzabili nel .config
    - Il Kconfig nella radice dell’albero dei sorgenti si limita a includere tutti Kconfig contenuti in ciascuna delle subdirectory
    - I Kconfig contenuti nelle subdirectory sono quelli che effettivamente contengono le definizioni delle opzioni

Abbiamo quindi makefile gerarchici e file di configurazione gerarchici


## make targets

- make help
    - il build system è autodocumentato
- make clean
    - pulisce artefatti di build, ma non la .config
- make mrproper
    - pulisce tutto, lascia solo i file sorgenti


Le opzioni di configurazione sono interdipendenti tra di loro

- se setto questo devo settare anche quest'altro, ...
- nel database di configurazione sono definite queste dipendenze

Come modifico il config?

- lo posso fare a mano... super error prone (dipendenze), e tedioso 
- ci sono vari target per modificare la config in maniera più intelligente
- make config: ancora rudimentale
- **make menuconfig: il target più utilizzato**
    - xconfig e gconfig utilizzano grafica più spinta (QT)
    - '/' per cercare
    - l'albero di opzioni visualizzabile in questo menu è definito dalle entry nell'albero dei kconfig
- make oldconfig
    - a volte il kernel e la .config divergono di version (e.g. git pull di una nuova versione del kernel)
    - questo target tenta di modificare il .config del vecchio kernel adattandolo per il nuovo kernel
- **make defconfig (default config)**
    - questo mi crea un .config con i valori di default per tutte le opzioni
    - questi default sono definiti nel database

Altri target interessant sono quelli che hai già usato

- all
- modules
- bzImage
- install
- modules_install
- ...

## effetto di make install 

Il target make install crea/sovrascrive **quattro file**

- Immagine del kernel: vmlinux-<versione> o vmlinuz-<versione>
    - Copiato dall’albero di build
- Initramfs: initrd-<versione>
    - Generato dal build-system col comando mkinitramfs
- Config del kernel: config-<versione>
    - copiato dall’albero di build, il file sorgente è il .config
    - non strettamente necessario dato che il build è già stato effettuato, ma utile come log per il power-user
        - e.g. magari mi voglio copiare la config del kernel stock offerto dalla mia distro in maniera tale da buildare il mio kernel personale in maniera analoga e avere una configurazione che funziona di sicuro
- Tabella dei simboli: System.map
    - Non approfondiamo

Dopo aver creato/sovrascritto questi file, il make install aggiorna tipicamente il bootloader (eseguendo update-grub nel caso di grub)

# Gestione dei moduli

- lsmod 
    - lista i moduli dinamici caricati correntemente
    - Does not differentiate between statically and dynamically loaded modules.
    - Shows modules in chronological order, with the most recently loaded at the top.
    - interessante notare l'intero che rappresenta il numero di entità che stanno usando un dato modulo
        - posso rimuovere un modulo safely solamente se quel numero è 0
    - i nomi dopo quel modulo sono altri moduli dipendenti
        - gli utilizzatori non sono però solamente altri moduli
    - ... ci sono tanti valori interessanti: studia questo https://medium.com/@adilrk/kernel-modules-86f07c93362a

- insmod
    - carica un modulo dato il percorso di un .ko
    - non carica le dipendenze

- modprobe
    - comando simile a insmod ma più di alto livello
    - carica un modulo andando a cercare tra quelli installati
    - carica anche le dipenenze automaticamente
        - l'informazione sulle dipendenze è specificata a build-time dalle entry dei kconfig
        - make install_modules aggiunge un file che specifica le dipendenze (modules.dep) dentro a /lib/modules
    - **meccanismo consigliato per il caricamento dei moduli**

- rmmod
    - rimuove un modulo se non ci sono utilizzatori
    - -f per forzare

- modinfo
    - info utili su un modulo

## Moduli esterni

Altro asse di classificazione dei moduli (ortogonale rispetto a statico / dinamico)

I moduli esterni sono quelli out-of-tree

- possiamo sviluppare dei moduli in maniera indipendente rispetto allo sviluppo del kernel

### Creazione del nostro primo modulo esterno

moduli statici chiamano le funzioni di init solamente una volta al boot e le funzioni di exit mai. Di conseguenza una volta eseguiti gli init al boot il kernel può liberare la memoria di queste funzioni. Le funzioni di exit non vengono neanche compilate


# kbuild continued

per sintassi e semantica delle entry nei KConfig: https://docs.kernel.org/kbuild/kconfig-language.html


## Makefiles

The Makefiles have five parts:

- The top Makefile.
    - reads the .config file, which comes from the kernel configuration process.
    - is responsible for building two major products: vmlinux, and modules. It builds these goals by recursively descending into the subdirectories of the kernel source tree.
    - textually includes an arch Makefile with the name arch/$(SRCARCH)/Makefile.
- .config                     the kernel configuration file.
- the arch Makefile -> arch/$(SRCARCH)/Makefile    
    - supplies architecture-specific information to the top Makefile.
- scripts/Makefile.*          
    - common definitions/rules etc. for all kbuild Makefiles.
    - una sorta di libreria comune per gli altri makefile
- subdirectory Makefile (i.e. kbuild makefiles (il top makefile viene considerato a parte))
    - Each subdirectory has a kbuild Makefile which carries out the commands passed down from above.
    - uses information from the .config file to construct various **file lists** used by kbuild to build any built-in or modular targets.

The preferred name for the kbuild files is 'Makefile', but 'Kbuild' can be used. 

### Goal definitions

These lines define the files to be built, any special compilation options, and any subdirectories to be entered recursively.

The most simple kbuild makefile contains one line:

```Example
obj-y += foo.o
```

This tells kbuild that there is one object in that directory, named foo.o. **foo.o will be built from foo.c or foo.S.**

- specifichiamo quindi il target e non le sorgenti!

If foo.o shall be built as a module, the variable obj-m is used. Therefore the following pattern is often used:

```Example
obj-$(CONFIG_FOO) += foo.o
```

$(CONFIG_FOO) evaluates to either y (for built-in) or m (for module).

- espansione delle variabili di configurazione per aggiungere i .o alle liste obj-y, obj-m etc

If CONFIG_FOO is neither y nor m, then the file will not be compiled nor linked.

### Modules

**A module may be built from one source file or several source files**

- In the case of one source file, the kbuild makefile simply adds the file to $(obj-m).
    - Example: obj-$(CONFIG_ISDN_PPP_BSDCOMP) += isdn_bsdcomp.o
If a kernel module is built from several source files, kbuild needs to know which object files you want to build your module from, so you have to tell it by setting a $(<module_name>-y) variable
    - Example:
        - isdn-y := isdn_net_lib.o isdn_v110.o isdn_common.o
        - obj-$(CONFIG_ISDN_I4L) += isdn.o
    - in this example, the module name will be isdn.o.

**NB**: questo significa che per kbuild un modulo è semplicemente quello che aggiungi alle righe obj-y/m nel makefile.

- un modulo può quindi tranquillamente essere un singolo file; quando un modulo è composto da più file, va specificato come sopra

### Descending down directories

A Makefile is only responsible for building objects in its own directory. Files in subdirectories should be taken care of by Makefiles in these subdirs. The build system will automatically invoke make recursively in subdirectories, provided you let it know of them.

To do so, obj-y and obj-m are used.

example:

- ext2 lives in a separate directory, and the Makefile present in fs/ tells kbuild to descend down using the following assignment.
- #fs/Makefile -> obj-$(CONFIG_EXT2_FS) += ext2/

Anche qui CONFIG_EXT2_FS can be set to either “y” or “m”

- When Kbuild descends into the directory with “y”, all built-in objects from that directory are combined into the built-in.a, which will be **eventually linked into vmlinux**.
- When Kbuild descends into the directory with “m”, in contrast, nothing from that directory will be linked into vmlinux. 
    - If the Makefile in that directory specifies obj-y, those objects will be left orphan. It is very likely a bug of the Makefile or of dependencies in Kconfig.

## Build di un modulo esterno

Nel caso in cui si sia certi che, dall’ultimo kbuild, si è modificato solo il codice sorgente di qualche modulo, ed in un’unica subdirectory, allora si può ricompilare solo i moduli toccati facendo: make M=<percorso-subdirectory>

Per compilare il modulo esterno abbiamo fatto: make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

- questa linea permette di agganciarmi al sistema di build del kernel e creare il mio modulo
- -C è mi fa usare il makefile in quel path (agganciamento al build system)
- la variabile M specifica la mia cartella
- sto dicendo:
    - compila i miei sorgenti creando un modulo
    - utilizzando il makefile che sto specificando
 
**NB**: /lib/modules/$(shell uname -r)/build, modules sembra contenere molte cose utili per buildare moduli

- e.g. header, link a kconfig, kbuild, makefile, etc

In particolare:

- -C specifica di spostarsi nella directory indicata e di cercare in tale directory il Makefile da eseguire. Lì si trova proprio il Makefile per la versione del kernel indicata da uname -r, nonché tutti gli header file di tale kernel
- l'assegnazione di $(PWD) alla variabile M, fa limitare il build ai soli moduli presenti nella directory $(PWD). Come target, esegue il target modules, per generare appunto i moduli (stesso risultato se non si aggiungeva modules affatto)

Un altro comando utile è: make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean, per fare pulizia degli artefatti di compilazione del proprio modulo

### Note su processo di build:

- make -M si rompe se ho modificato anche altra roba al di fuori della subdirectory specificata
- make block è una via intermedia tra make completo e make -M
    - è anche intermedio nel tempo di compilazione

Ora possiamo capire cosa abbiamo fatto in: "1-compilazione.md"!

- e possiamo studiarcelo e migliorare come abbiamo buildato
- il build me lo faccio in una cartella separata

È molto più comodo tenere il source-tree sull'host e non sul guest

- se crasho, non devo aspettare il reboot per incominciare a debuggare
- non rischio corruzioni dei dati del mio source tree che mi fanno perdere lavoro

# Moduli interni