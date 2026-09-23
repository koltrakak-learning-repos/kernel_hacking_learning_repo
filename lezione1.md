la documentazione è sparsa, difficile da reperire, outdated, ecc.

# Versionamento

\<major.minor.bugfix_counter\>

- prima di avere una release ufficiale ci sono delle release candidates che incorporano fix e rendono mano a mano più stabile una nuova release
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

...

uname -a

```cosa significa immagine?

Un'immagine è praticamente la rappresentazione fisica sotto forma di blocco di byte di un qualcosa di più astratto

Ad esempio un file system

Oppure il kernel stesso, che solitamente è un processo in esecuzione
```

regola d'oro da seguire in questo corso: **non usare i diritti di root se non strettamente necessario!!**

- rompi cose

# bootstrap

- bios/uefi: sistema operativo base che sta in ROM che sceglie un drive da cui far partire il boot
- questo programma carica i byte che sono memorizzati dentro al MBR e imposta l'IP l'indirizzo del primo byte
- questo primo byte punta al bootloader che fa echo a quello che fa il bios caricando l'immagine del kernel
- il kernel fa partire l'userspace


...

il kernel ha una head

- codice, non compresso, eseguito all'inizio appena dopo che il bootloader lo ha caricato in memoria
    - l'head contiene il codice per decomprimere il resto del kernel (start())

# elixir

esploratore per il codice sorgente

## head_64.s

