# Struttura modulare

Il kernel Linux è monolitico in termini di spazio di indirizzamento ma è modulare in termini di codice sorgente e binario.

L’intero codice è diviso in due parti:

- Un core contenente tutte funzionalità imprescindibili
- Un insieme di moduli, contenente tutte le funzionalità non imprenscindibili

Ciascun modulo del kernel contiene un componente senza il quale il kernel può funzionare lo stesso, su almeno una architettura hardware

- tutto cio che non è imprescindibile per il kernel viene posto in dei moduli
- nel core risiede tutto quel codice tale che, se rimosso, non esisterebbe neanche un hardware sopra a cui il kernel può eseguire

I moduli, tra le altre cose, contengono:

- driver, per supportare l'hardware
- supporto di politiche di gestione delle risorse
    - scheduler
    - controller
    - file system
- supporto della virtualizzazione

## Caricamento dinamico dei moduli 

I moduli possono essere caricati dinamicamente, ossia a tempo di esecuzione.  Questo porta vantaggi evidenti:

- Occupazione di memoria solo se e quando necessario
- Riduzione delle dimensioni dell’immagine del kernel
- Si può modificare il codice eseguibile del kernel che risiede in uno di tali moduli, senza dover ricompilare, reinstallare e riavviare il kernel. Basta infatti rimuovere, modificare, ricompilare e ricaricare solo il modulo

Il caricamento dinamico dei moduli è equivalente al caricamento dinamico di shared library in user-space (sembra proprio che venga utilizzato il dynamic linker senza modifiche)

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

Il kernel Linux supporta un TIPO di **ram filesystem** chiamato tmpfs

- Tale supporto non è implementato come modulo, ma sta nel core quindi è sempre presente nell’immagine del kernel
- All’avvio, il kernel crea **in ram** un fs di tipo tmpfs
    - tale filesystem è denominato rootfs (**ma non è la stessa root che abbiamo dopo il boot: "/"**)
- E si aspetta che il bootloader gli fornisca un archivio compresso (in formato cpio)
    - **Tale archivio è chiamato initramfs** (con grande confusione di termini)
    - initramfs è quindi un semplice archivio compresso
- Il kernel quindi scompatta l'archivio initramfs dentro rootfs
    - Per semplicità, chiamiamo initramfs anche il filesystem che a questo punto si ritrova contenuto dentro al rootfs
- L’initramfs contiene:
    - Tutti i moduli necessari al kernel per montare il filesystem contenuto nel drive passato come root dalla riga di comando
    - Tutti i moduli necessari per supportare i dispositivi HID
    - Un programma di init (**distinto dall'init dell'userspace**, e.g. systemd), contenuto nella radice dell’albero
        - Carica tutti i moduli presenti nell’initramfs
            - questo è l'init segreto che spuntava nel kernel log!
        - Avvia l’init dello userspace (ossia /sbin/init, se non è stato modificato da riga di comando del kernel attraverso il l’opzione "init=")
        - Dettaglio: anche il percorso dell’init nel initramfs può essere modificato, attraverso l’opzione "initrd="

**NB**: initramfs si chiama così perché il suo contenuto è presente in ram e non su disco

- anche initrd (init ram disk) rende l'idea: disco contenuto in ram
- di conseguenza è volatile 
- ma è giusto così dato che fare modifiche permanenti farebbe più danni che altro
- dobbiamo solo caricare i moduli e avviare l'userspace init

### processo di boot e initramfs

È notevole il fatto che l'init dentro a initramfs sia un semplice script bash. Siamo già a tutti gli effetti in userspace!

Nel momento in cui il kernel Linux termina la sua fase di boot interna, monta l'initramfs in memoria (come rootfs temporaneo) e lancia il primo eseguibile con PID 1, avviene la transizione definitiva dal kernel mode allo user mode.

L'init di initramfs gira interamente nello spazio utente e, pur disponendo di un set molto limitato di binari statiche stipati nell'archivio CPIO compresso, effettua le operazioni di inizializzazione descritte in precedenza tramite comandi bash (caricamento dei moduli, mount di /, lancio del vero init).