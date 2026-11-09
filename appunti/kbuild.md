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
    - pulisce artefatti di build (.o, .d, ...), ma non .config
- make mrproper
    - pulisce tutto, lascia solo i file sorgenti
    - pulisce anche header file generati, .config, script vari, e altre cose che non so

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
- **make oldconfig**
    - Update current config utilising a provided .config as base
    - a volte il kernel e la .config divergono di versione
        - git pull di una nuova versione del kernel
        - **quando modifichi te il kernel** aggiungendo nuovi sorgenti / nuovi moduli con relative nuove opzioni di configurazione dentro a nuovi KConfig
    - In questo contesto, **rebuildare il kernel con la vecchia config è problematico**
    - make oldconfig il vecchio file di configurazione come base e ti interroga solo sulle nuove opzioni introdotte nella versione corrente del kernel, lasciando inalterate le vecchie scelte.
        - questo target tenta di modificare il .config del vecchio kernel adattandolo per il nuovo kernel
- **make defconfig (default config)**
    - questo mi crea un .config con i valori di default per tutte le opzioni
    - questi default sono definiti nel database

Altri target interessant sono quelli che hai già usato

- all
- modules
- install
- modules_install
- bzImage
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
        - guarda dentro a /lib/modules/$(uname -r)/kernel/...path_src_tree_al_modulo.../<nome_modulo.ko>
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

Dato il kernel per cui si crea un modulo esterno, l’albero dei sorgenti di tale kernel può non essere presente affatto nel sistema in cui si crea un modulo esterno

- **Sono necessari solo gli header file di tale kernel**
    - ... e il .config, e il build system (makefile tree)
- tutto trovabile in /lib/modules/$(uname -r)/build (i.e. link alla root del mio build tree (vedo che punta alla root del mio src tree ma io non ho specificato un build tree separato))

NB: per il funzionamento corretto di un modulo esterno, il kernel running (quello in cui carichiamo il modulo esterno) deve corrispondere al kernel-tree che usiamo per compilare il modulo esterno

- Affinché tale modulo venga correttamente compilato, i sorgenti devono corrispondere esattamente a quelli che sono stati utilizzati quando abbiamo creato l’immagine del nostro kernel custom presente in /boot
- Il rischio che corriamo è di avere in esecuzione una versione del kernel custom che non corrisponde esattamente ai file sorgente
- questo è problematico a causa di problemi di ABI
    - I moduli del kernel comunicano con il resto del sistema usando strutture dati complesse (come file_operations, inode, net_device, ecc.).
    - La dimensione e l'offset dei campi all'interno di queste strutture dipendono interamente da come quel kernel è stato configurato (.config).
    - Se nel kernel in esecuzione hai disabilitato una funzionalità (es. il supporto ai firewall) e nel kernel usato per compilare il modulo quella funzionalità era attiva, le dimensioni delle strutture dati cambieranno.
- nel migliore dei casi, il sistema di caricamento dei moduli si accorge del mismatch è produce un errore: `modprobe: FATAL: Invalid module format`
- ma se questo non succede e l'ABI è rotta, il tuo modulo leggerà o scriverà in posizioni di memoria errate, causando un Kernel Panic immediato o corrompendo la memoria di sistema silenziosamente.

moduli statici chiamano le funzioni di init solamente una volta al boot e le funzioni di exit mai. Di conseguenza una volta eseguiti gli init al boot il kernel può liberare la memoria di queste funzioni. Le funzioni di exit non vengono neanche compilate

# kbuild continued

## Makefiles

The Makefiles have five parts:

- The top Makefile.
    - reads the .config file, which comes from the kernel configuration process.
    - is responsible for building two major products: vmlinux, and modules. It builds these goals by recursively descending into the subdirectories of the kernel source tree.
    - textually includes an arch Makefile with the name arch/$(SRCARCH)/Makefile.
- .config                     
    - the kernel configuration file.
- the arch Makefile -> arch/$(SRCARCH)/Makefile    
    - supplies architecture-specific information to the top Makefile.
- scripts/Makefile.*          
    - common definitions/rules etc. for all kbuild Makefiles.
    - una sorta di libreria comune per gli altri makefile
- subdirectory Makefile (anche detti kbuild makefiles (il top makefile viene considerato a parte))
    - Each subdirectory has a kbuild Makefile which carries out the commands passed down from above.
    - uses information from the .config file to construct various **file lists** (obj-y, obj-m) used by kbuild to build any built-in or modular targets.

The preferred name for the kbuild files is 'Makefile', but 'Kbuild' can be used. 

### Goal definitions

These lines define the files to be built, any special compilation options, and any subdirectories to be entered recursively.

The most simple kbuild makefile contains one line:

```Example
obj-y += foo.o
```

This tells kbuild that there is one object in that directory, named foo.o. **foo.o will be built from foo.c or foo.S.**

- specifichiamo quindi il target e non i sorgenti!

If foo.o shall be built as a (dynamic) module, the variable obj-m is used. Therefore the following pattern is often used:

```Example
obj-$(CONFIG_FOO) += foo.o
```

\$(CONFIG_FOO) evaluates to either y (for built-in) or m (for module).

- espansione delle variabili di configurazione (salvate dentro a .config) per aggiungere i .o alle liste obj-y, obj-m etc

If CONFIG_FOO is neither y nor m, then the file will not be compiled nor linked.

### Modules

**A module may be built from one source file or several source files**

- In the case of one source file, the kbuild makefile simply adds the file to \$(obj-m).
    - Example: obj-$(CONFIG_ISDN_PPP_BSDCOMP) += isdn_bsdcomp.o
- If a kernel module is built from several source files, kbuild needs to know which object files you want to build your module from, so you have to tell it by setting a \$(<module_name>-y) variable
    - Example:
        - isdn-y := isdn_net_lib.o isdn_v110.o isdn_common.o
        - obj-\$(CONFIG_ISDN_I4L) += isdn.o
    - in this example, **the module name will be isdn.o**

**NB**: questo significa che per kbuild un modulo è semplicemente quello che aggiungi alle righe obj-y/m nel makefile.

- un modulo può quindi tranquillamente essere un singolo file. Quando un modulo è composto da più file, va specificato come sopra

### Descending down directories

A Makefile is only responsible for building objects in its own directory. Files in subdirectories should be taken care of by Makefiles in these subdirs. The build system will automatically invoke make recursively in subdirectories, provided you let it know of them.

To do so, obj-y and obj-m are used.

example:

- ext2 lives in a separate directory, and **the Makefile present in fs/** tells kbuild to descend down using the following assignment: `obj-$(CONFIG_EXT2_FS) += ext2/`

**NB**: Anche qui CONFIG_EXT2_FS can be set to either “y” or “m”

- in entrambi i casi questo dice a kbuild di visitare la sottodirectory
- the value of the config variable decides whether or not to link objects from the directory into the vmlinux binary
- When Kbuild descends into the directory with “y”, all built-in objects from that directory are combined into the subirs built-in.a, which will be **eventually linked into vmlinux**.
- When Kbuild descends into the directory with “m”, in contrast, nothing from that directory will be linked into vmlinux. 
    - If the Makefile in that directory specifies obj-y, those objects will be left orphan. 
    - It is **very likely a bug** of the Makefile or of dependencies in Kconfig.

| Padre | Figlio | Risultato |
|:------|:-------|:----------|
|obj-y += dir/ | obj-y += foo.o | viene compilato un foo.o che finisce in dir/built-in.a, che a sua volta viene linkato in vmlinux. Il caso classico “built-in” |
| obj-y+=dir/ | obj-m += foo.o | foo.o viene compilato come modulo (foo.ko). Entrando con y, i built-in verrebbero linkati in vmlinux, ma qui non ce ne sono: il modulo resta un modulo da caricare dinamicamente in un secondo momento | 
| obj-m += dir/ | obj-m += foo.o | foo.o compilato come modulo (foo.ko). Nulla va in vmlinux. Il caso classico “modulo” |
| obj-m += dir/ | obj-y += foo.o | **Caso problematico** Kbuild entra nella directory, compila foo.o, **ma il built-in.a di quella directory non viene collegato in vmlinux dato che il padre specifica (obj-m)**. foo.o è orfano |

Riassumendo:

- Il Makefile figlio decide cosa viene compilato dentro la directory e in che forma 
    - .o oppure .ko
- Il Makefile padre decide se e come Kbuild entra nella sottodirectory 
    - linko in vmlinux oppure no

**NB**: non ha senso includere una subdir com 'obj-m +=' dato che rischio di fare solo danni. Usa sempre obj-y, oppure aggiungi un'opzione di configurazione

## KConfig

Anche in kconfig sono gerarchici

- esempio: https://elixir.bootlin.com/linux/v6.16.7/source/drivers/soc/microchip

I KConfig foglia definiscono le opzioni di configurazione da settare nel .config

```drivers/soc/microchip/KConfig
config POLARFIRE_SOC_SYS_CTRL
	tristate "Microchip PolarFire SoC (MPFS) system controller support"
    default y
	depends on POLARFIRE_SOC_MAILBOX
	depends on MTD
	help
	  This driver adds support for the PolarFire SoC (MPFS) system controller.

	  To compile this driver as a module, choose M here. the
	  module will be called mpfs_system_controller.

	  If unsure, say N.
```

I KConfig padre inclusono semplicemente i KConfig foglia

```drivers/soc/KConfig
menu "SOC (System On Chip) specific Drivers"

source "drivers/soc/amlogic/Kconfig"
...
source "drivers/soc/microchip/Kconfig"
...
source "drivers/soc/xilinx/Kconfig"

endmenu
```

Two kinds of values are used for conditional compilation: 

- Boolean symbols can take one of two values: true or false
- Tristate symbols, on the other hand, can take three different values: yes, no or module.

**Not everything in the kernel can be compiled as a module**. Many features are so intrusive that you have to decide at compilation time whether the kernel will support them. For example, you can't add Symmetric Multi-Processing (SMP) or kernel preemption support to a running kernel. So, using a boolean config symbol makes sense for those kinds of features. 

Boolean config options can also be used for subfeatures of a module, for example

- most drivers can be built in the kernel, included as a module or not included at all. So, the first config option for them should of type tristate (y/n/m).
- but a driver could also have a second option that, for example, is used to decide whether you want to expose some statistics. This is a binary choice.

Also, it doesn't make sense to add the statistics to the kernel if you choose not to include the driver itself

- spesso ci sono delle dipendenze tra le opzioni di configurazione; queste vengono modellate nei Kconfig

per sintassi e semantica delle entry nei KConfig: https://docs.kernel.org/kbuild/kconfig-language.html

NB: nel .config viene aggiunto automaticamente il prefisso 'CONFIG_' a tutte le opzioni. Nell'esempio sotto l'opzione spunta di conseguenza con nome CONFIG_MY_MODULE_CONFIG

```
config MY_MODULE_CONFIG
	tristate "my internal module"
	default m
```

## Build di un modulo esterno

```Makefile modulo esterno
obj-m += my_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Per compilare il modulo esterno abbiamo fatto: make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

- questa linea permette di agganciarmi al sistema di build del kernel e creare il mio modulo
- -C è mi fa usare il (root) makefile in quel path (agganciamento al build system del kernel)
- la variabile M specifica la mia cartella
- Come target, esegue il target modules, per generare appunto i moduli (stesso risultato se non si aggiungeva modules affatto)
- sto dicendo:
    - compila i miei sorgenti creando un modulo dinamico (obj-m)
    - utilizzando il makefile che sto specificando
 
**NB**: /lib/modules/$(shell uname -r)/build, modules sembra contenere molte cose utili per buildare moduli

- e.g. header, link a kconfig, kbuild, makefile, etc

In particolare:

- -C specifica di spostarsi nella directory indicata e di cercare in tale directory il Makefile da eseguire. Lì si trova proprio il Makefile per la versione del kernel indicata da uname -r, nonché tutti gli header file di tale kernel
- l'assegnazione di $(PWD) alla variabile M, fa limitare il build ai soli moduli presenti nella directory $(PWD).

### ricompilare solo una subdirectory

Nel caso in cui si sia certi che, dall’ultimo kbuild, si è modificato solo il codice sorgente di qualche modulo dinamico, ed in un’unica subdirectory, allora si può ricompilare solo i moduli toccati facendo: make M=<percorso-subdirectory>

- make -M si rompe se ho modificato la configurazione nel frattempo

Nel caso in cui si sia modificata anche la configurazione, oppure nel caso in cui ci siano anche moduli built-in da compilare in una subdirectory, si può utilizzare la seguente alternativa

- make <nome subdir in cui ci sono i file di cui volete effettuare la ricompilazione> 
    - è una via intermedia tra make completo e make -M
    - è anche intermedio nel tempo di compilazione

Ora possiamo capire cosa abbiamo fatto in: "1-compilazione.md"!

- e possiamo studiarcelo e migliorare come abbiamo buildato
- il build me lo faccio in una cartella separata

btw: è molto più comodo tenere il source-tree sull'host e non sul guest

- se crasho, non devo aspettare il reboot per incominciare a debuggare
- non rischio corruzioni dei dati del mio source tree che mi fanno perdere lavoro

# Moduli interni

un modulo è interno se i suoi sorgenti appartengono all'albero dei sorgenti del kernel, e la sua compilazione è gestita dal build system del kernel

- detta in altra maniera, l'albero dei sorgenti del kernel contiene il core del kernel e tutti i moduli interni
- i moduli esterni possono essere solamente dinamici (vengono caricati solo con insmod)
- i moduli interni invece possono essere sia builtin che dinamici

...

nella subdirectory del mio modulo interno ci metto

- i miei sorgenti
- il Makefile
- il KConfig

...

...

---

### Domanda slide 67

il meccanismo di caricamento dei moduli dinamici è interessante e richiede la ricompilazione e il riavvio del kernel al contrario di quello che si potrebbe pensare

- ci sono strutture dati che vengono incluse tramite #ifdef quando un modulo è incluso con m e vengono elise quando il modulo non è incluso

è interessante anche notare che i moduli esterni sono più limitati rispetto ai moduli interni in questo contesto (la configurazione del kernel deve essere affine al mio modulo esterno)

- capisci meglio


Ricorda che to load a module, the kernel must contain all the kernel symbols used in the module. If those symbols were not included in the kernel at compile time, the module will not be loaded due to missing dependencies. 






---

...

riassumendo, ogni volta che cambio il .config devo ricompilare tutto quanto

### esercizio modulo interno

mettiamolo dentro drivers/ dato che è spesso usato come contenitore generico

## Collegamento da modulo a core 

rilocazione del testo del modulo per far aggiustare i riferimenti all'interno del modulo verso il codice del kernel

## Collegamento da core a modulo

il kernel non può usare i simboli definiti all'interno di un modulo

il collegamento avviene tramite un meccanismo tipo plugin

- sono presenti delle strutture dati con un nome fisso che contengono dei puntatori che vengono popolati con gli indirizzi di memoria delle funzioni/simboli del modulo quando quest'ultimo viene caricato 

## caricamento automatico dei moduli on-demand

il kernel è in grado di caricare da solo i moduli di cui ha bisogno

- come? quando? ... lo scopriremo

guarda esempio i/o sched in slide 99

---

## How to compile source code segments conditionally

Abbiamo visto che configurando un'opzione nel .config, possiamo aggiungere o meno un modulo alle list obj-y/obj-m, e quindi decidere decidere se includere o meno quel modulo nella nostra build del kernel 

Può capitare però che a volte abbiamo bisogno di abilitare/disabilitare alcune feature all'interno di uno stesso modulo

- pensa all'esempio delle statistiche di un driver; se non vogliamo le statistiche non c'è bisogno di compilare il codice all'interno del modulo che le produce

**This can be done easily by using macros defined in _include/generated/autoconf.h_**

```
// other module code ...

#ifdef CONFIG_COIN_STAT // name of the config option enabled/disabled in .config
    dir = debugfs_create_dir("coin", NULL);
    file = debugfs_create_file("stats", 0644, dir, &file_value, &fstat);
#endif

// other module code ...
```

### Where do these macros come from?

When you execute a compile target to compile the kernel or the modules, it first executes a binary that reads all the Kconfig files and .config: `scripts/kconfig/conf Kconfig`

This binary updates (or creates) a C header file with the values you chose for all the configuration symbols. 

- This file is include/generated/autoconf.h, and every gcc compile instruction includes it, so **the symbols can be used in any source file in the kernel**

The file is composed of thousands of #define macros that describe the state for each symbol. For example, the bool CONFIG_COIN_STAT symbol with the value true, and the tristate CONFIG_COIN symbol with the value yes will generate the following:

```
#define __enabled_CONFIG_COIN_STAT 1
#define __enabled_CONFIG_COIN_STAT_MODULE 0
#define CONFIG_COIN_STAT 1

#define __enabled_CONFIG_COIN 1
#define __enabled_CONFIG_COIN_MODULE 0
#define CONFIG_COIN 1
```