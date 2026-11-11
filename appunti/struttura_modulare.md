# Struttura modulare

Il kernel Linux è monolitico in termini di spazio di indirizzamento ma è modulare in termini di codice sorgente e binario.

**L’intero codice è diviso in due parti**:

- Un core contenente tutte funzionalità imprescindibili
- Un insieme di moduli, contenente tutte le funzionalità non imprenscindibili

Ciascun modulo del kernel contiene un componente senza il quale il kernel può funzionare lo stesso, **su almeno una architettura hardware**

- tutto cio che non è imprescindibile per il kernel viene posto in dei moduli
- nel core risiede tutto quel codice tale che, se rimosso, non esisterebbe neanche un hardware sopra a cui il kernel può eseguire

I moduli, tra le altre cose, contengono:

- driver, per supportare l'hardware
- supporto di politiche di gestione delle risorse
    - scheduler
    - controller
    - file system
- supporto della virtualizzazione

At kernel compile time you will have to choose the features that will be built in the kernel image, the ones that will be built as loadable kernel modules and may be loaded once the kernel is executing, and the features that can be ignored and that will not be built at all.

- **this is done through the .config**

Modules are only a way to defer compilation (and/or execution) of a specific kernel feature. Once a kernel module is loaded, it is part of the monolithic kernel and shares the same address space of the code that was included at kernel compile time.

## Caricamento dinamico dei moduli 

I moduli possono essere caricati dinamicamente, ossia a tempo di esecuzione. Questo porta vantaggi evidenti:

- Occupazione di memoria solo se e quando necessario
- Riduzione delle dimensioni dell’immagine del kernel
- ma soprattutto: si può modificare il codice eseguibile del kernel che risiede in uno di tali moduli, **senza dover ricompilare, reinstallare e riavviare il kernel**. 
    - Basta infatti rimuovere, modificare, ricompilare e ricaricare solo il modulo

Il caricamento dinamico dei moduli è equivalente al caricamento dinamico di shared library in user-space (sembra proprio che venga utilizzato il dynamic linker senza modifiche)

**NB**: To load a module, the kernel must contain all the kernel symbols used in the module. If those symbols were not included in the kernel at compile time, the module will not be loaded due to missing dependencies. 

Dettagli caricamento moduli e linking:

- Collegamento da modulo a core prevede
    - caricamento del modulo in memoria (nello spazio di indirizzamento del kernel)
    - rilocazione del testo del modulo per aggiustare i riferimenti del modulo verso il codice del kernel
- Collegamento da core a modulo
    - il kernel core non può usare i simboli definiti all'interno di un modulo (non sa a priori come si chiamano)
    - il core invoca le funzionalità fornite dai moduli solo tramite strutture di callback (ossia di puntatori a funzione) popolate durante il caricamento del modulo
        - meccanismo tipo plugin
        - sono presenti delle strutture dati con un nome fisso che contengono dei puntatori che vengono popolati con gli indirizzi di memoria delle funzioni/simboli del modulo quando quest'ultimo viene caricato 
    - **NB**: Quando un modulo viene inizializzato (all’avvio del kernel se modulo built-in, al caricamento se modulo dinamico), il codice di init del modulo definirà le istanze di callback appropriate per quel modulo e le registrerà con le funzionalità già previste staticamente dal codice del kernel
- Collegamento da modulo a modulo
    - I moduli possono interagire tra loro:
        - Tramite registrazione di callback, come si fa per permettere al core di interagire con un modulo
        - Tramite collegamento diretto dei simboli
            - implementato mediante le macro EXPORT_SYMBOL
                - si crea una dipendenza diretta se un modulo A utilizza un simbolo esportato da un modulo B
                - sono proprio le dipendenze gestite automaticamente da modprobe
        - Con l’ausilio di funzionalità come symbol_get e symbol_put si possono gestire dipendenze opzionali

PROBLEMA:

- All’avvio del kernel, i moduli da caricare non sono già caricati
- Essi risiedono in un qualche filesystem
- Come fa il kernel, al suo avvio, a leggere il filesystem in cui sono contenuti proprio i moduli che deve caricare per poter leggere tale filesystem?

Semplicemente non tutti i moduli sono dinamici, una parte dei moduli è statica (moduli builtin)

- Moduli statici, cosiddetti built-in
    - Vengono linkati staticamente al core del kernel, e quindi fanno parte dell’immagine. 
    - Non c’è nulla da caricare a runtime
    - nel .config =y significa compilalo come builtin
- Moduli dinamici
    - Oggetti di tipo ELF, che vengono caricati e collegati dinamicamente attraverso dei programmi userspace (i.e dynamic linker)

Quasi sempre solo i moduli dinamici sono denominati moduli, mentre i moduli built-in sono chiamati semplicemente built-in. A volte i moduli dinamici sono anche chiamati moduli caricabili (loadable)

## initramfs/initrd

**NB**: Non posso però rendere tutti i moduli per ogni possibile hardware statici, altrimenti ritorna ad essere monolitico anche in termini del binario prodotto

Tipicamente solo i moduli necessari per l’avvio ed il funzionamento di base del kernel sono compilati built-in:

- Tutti gli altri moduli disponibili nel kernel sono compilati come moduli dinamici
- Questo vale anche per
    - I driver necessari per il supporto dei dispositivi di storage, e per i componenti di supporto dei filesystem
    - I driver Human Interface Device (HID): tastiera, mouse, ... 
- dato che i possibili dispositivi di storage ed HID sono tantissimi, ed i filesystem sono tanti

il problema della mancanza dei moduli per caricare moduli al boot potrebbe quindi rimanere

```aside: ricorda che cos'è un file system
File systems are an ORGANIZATION OF DATA AS FILES AND DIRECTORIES. 

The operating system provides a global file namespace, organized as a top-down tree topology starting with the root level (“/”). File systems join the tree by mounting, attaching their own tree to a directory (the mount point). This allows the end user to navigate the file namespace transparently, regardless of the underlying file system type

Most file system types use storage devices (disks) to store their contents. Some file system types are dynamically created by the kernel, such as /proc and /dev.
 ```

Il kernel Linux supporta un TIPO di **ram filesystem** chiamato tmpfs

- Tale supporto non è implementato come modulo, ma sta nel core quindi è sempre presente nell’immagine del kernel
- All’avvio, il kernel crea **in ram** un fs di tipo tmpfs
    - tale filesystem è denominato rootfs (**ma non è la stessa root che abbiamo dopo il boot: "/"**)
- E si aspetta che il bootloader gli fornisca un archivio compresso (in formato **cpio**)
    - **Tale archivio è chiamato initramfs** (con grande confusione di termini)
    - initramfs è quindi un semplice archivio compresso (ricorda busybox)
- Il kernel quindi scompatta l'archivio initramfs dentro rootfs
    - Per semplicità, chiamiamo initramfs anche il filesystem che a questo punto si ritrova contenuto dentro al rootfs
- L’initramfs contiene:
    - Tutti i moduli necessari al kernel per montare il filesystem contenuto nel drive passato come root dalla riga di comando
    - Tutti i moduli necessari per supportare i dispositivi HID
        - in modo che l'utente possa interagire con il sistema tramite monitor, mouse, tastiera e quant'altro
    - Un programma di init (**distinto dall'init dell'userspace**, e.g. systemd), contenuto nella radice dell’albero che:
        - Carica tutti i moduli presenti nell’initramfs
        - Avvia l’init dell'userspace vero e proprio (ossia /sbin/init, se non è stato modificato da riga di comando del kernel attraverso il l’opzione "init=")
    - questo è l'init segreto che spuntava nel kernel log!
        - Dettaglio: anche il percorso dell’init nel initramfs può essere modificato, attraverso l’opzione "initrd="

**NB**: initramfs si chiama così perché il suo contenuto è presente in ram e non su disco

- anche initrd (init ram disk) rende l'idea: disco contenuto in ram
- di conseguenza è volatile 
- ma è giusto così dato che fare modifiche permanenti farebbe più danni che altro
- dobbiamo solo caricare i moduli e avviare l'userspace init

### processo di boot e initramfs

È notevole il fatto che l'init dentro a initramfs sia un semplice script bash. Siamo già a tutti gli effetti in userspace!

Nel momento in cui il kernel Linux termina la sua fase di boot interna (decompressione, e varie init), come ultimo passaggio monta l'initramfs in memoria e lancia l'init al suo interno come primo eseguibile con PID 1 

- avviene la transizione definitiva da kernel mode a user mode

L'init di initramfs gira interamente nello spazio utente e, pur disponendo di un set molto limitato di binari statici stipati nell'archivio CPIO compresso (e.g. busybox), effettua le operazioni di inizializzazione descritte in precedenza tramite comandi bash (caricamento dei moduli, mount di /, lancio del vero init).

- il secondo init ruba il PID 1 al primo initramfs init dato che viene utilizzata una exec

### Creazione initramfs

Avviene tramite comando mkinitramfs

Crea un initramfs per il kernel running, seguendo le istruzioni contenute in un file di configurazione

- Popola l’initramfs con un albero di moduli copiandoli da quelli installati per il kernel running
    - Selezionando i moduli indicati nel file di configurazione
- Copia alla radice dell’initramfs il file di init (tipicamente copiato da /usr/share/initramfs-tools/init)