# Approfondimento kernel log

I messaggi del kernel log al di sopra di una certa soglia di criticità non vengono solo inseriti nel kernel log, ma vengono anche stampati automaticamente su almeno una delle **console testuali** del sistema (tipicamente la prima /dev/tty0)

- infatti li vediamo al boot (prima che parta la grafica) se togliamo splash e quiet

## Redirezione delle console

Si può configurare il kernel per redirezionare tutti i messaggi stampati sulla sua console (e.g. /dev/console), su una console (porta) seriale (e.g. /dev/ttyS0) 

- se si usa grub per il boot, editate il file `/etc/default/grub`
- e modificate il seguente parametro come segue: `GRUB_CMDLINE_LINUX="<quello che già c'era> console=ttyS0 ignore_loglevel"`
- quindi aggiornare grub con: `sudo update-grub`

Tramite questa redirezione è possibile anche fornire input al kernel, scrivendo dato su questa porta seriale

- quando il kernel leggerà /dev/console, leggerà in realtà i dati che abbiamo scritto sulla porta seriale

```Questo meccanismo si usa molto nei sistemi embedded dato che è un modo semplice per interagire con un sistema headless (niente terminale)```

Questo è particolarmente utile se il kernel è eseguito in una macchina virtuale, perché la macchina virtuale presenta al guest delle interfacce seriali virtuali.

Per leggere l'output della porta seriale, le macchine virtuali forniscono di norma una delle seguenti modalità: 
- i byte che il kernel spedisce sulla porta venga scritto all'interno di un file. 
    - con VirtualBox, basta andare su Porte, abilitare una delle porte seriali, scegliere la modalità raw per tale porta, e scrivere il percorso del file in cui si vuole che finisca il log del kernel. 
- l'altra modalità è che la macchina spedisca a sua volta tali byte ad un device, dal quale poi li si può leggere

# kernel oops

Quando nel kernel avviene un fallimento di qualsiasi tipo, tale fallimento viene informalmente chiamato OOPS.

- un kernel oops è un meccanismo del kernel per la gestione di bug rilevati

Al verificarsi di un oops vengono compiute le seguenti azioni:

- Il kernel aggiunge in fondo al kernel log un messaggio per informare dell'accaduto: il cosiddetto messaggio di (errore per il) kernel oops.
- kill del processo interessato (e del suo contesto nel kernel (e.g. kernel stack, ...)), se l'oops è stato causato da un processo utente (e.g. modprobe), per poter continuare l'esecuzione del sistema
    - **esecuzione che però non è più affidabile** (capiamo meglio dopo cosa significa)
    - The offending process that triggered this Oops gets killed **without releasing locks or cleaning up structures**, this is called an unstable state
- se le risorse necessarie per andare avanti non sono più disponibili, allora si ha un cosiddetto KERNEL PANIC e diventa necessario un reboot

Dentro al messaggio di oops nel kernel log ci sono varie informazioni necessarie per il debugging

- causa del fallimento
- dove si è verificato il fallimento
- stack trace
    - alcune funzioni nello stack trace dell'oops sono marchiate con un `?`
    - a causa di ottimizzazioni
    - ma anche a causa di interrupt che corrompono lo stack di una funzione (TODO: ?)

I messaggi di kernel oops iniziano con un qualcosa del genere

```
[  116.960673] BUG: kernel NULL pointer dereference, address: 00000000000001a8
[  116.962021] #PF: supervisor read access in kernel mode
[  116.962949] #PF: error_code(0x0000) - not-present page
[  116.963881] PGD 0 P4D 0 
[  116.964375] Oops: 0000 [#1] SMP KASAN PTI
    ...
```

Notiamo che 

- la prima riga inizia con la parola BUG
- la keywor Oops è qualche riga più sotto

Un altra riga importante dell'oops specifica il punto in cui è avvenuto con il seguente formato:

- funzione + offset / dimensione della funzione

```[  116.969044] RIP: 0010:bfq_bfqq_save_state+0x7d/0x890 [bfq]```

## Visualizzare gli oops

Purtroppo se un OOPS genera un kernel panic, allora non lo si può più visualizzare con dmesg in quanto il sistema si pianta.

Come già detto detto, i messaggi critici del kernel vengono stampati automaticamente su almeno una delle console di testo. Però, tali messaggi non sono visibili se ci si trova in ambiente grafico, e dopo il blocco del sistema, è impossibile commutare su una console di testo.

Alla luce di questi fatti, ecco alcune modalità per visualizzare un messaggio di OOPS dovuto ad un kernel panic

### Console di testo

Se si è scoperto come riprodurre l'OOPS, allora si può commutare in anticipo sulla prima console di testo ed eseguire i comandi che portano all'OOPS. 

- **NB**: Se state usando QEMU state di fatto già interagendo attraverso tale console e quindi non c'è neanche bisogno di commutare

Se state usando qemu da dentro un terminale virtuale, potete analizzare il messaggio senza problemi, perché rimane memorizzato nel buffer di righe del terminale virtuale (e potete scorrere il messaggio a piacimento).

Se invece siete su una console su un host fisico o dentro un guest, allora uno dei problemi di questa modalità di visualizzazione di un messaggio di OOPS è che il messaggio ha tipicamente più righe della console di testo. Quindi se ne vede solo la parte finale, ed a volte questa parte non basta per capire la causa del fallimento.

### Porta seriale di una macchina virtuale

Visualizzare un oops diventa estremamente facile se si sono rediretti i messaggi del kernel sulla porta seriale di una macchina virtuale, e se tale la macchina virtuale è configurata per scrivere su un file i caratteri che passano attraverso tale porta seriale.

### TODO: netconsole

Per vari motivi potreste non riuscire ad utilizzare una macchina virtuale per visualizzare il messaggio di OOPS (perché, ad esempio, il fallimento non si riproduce nella macchina virtuale, o perché non avete le risorse per creare ed eseguire una macchina virtuale).

In questo caso, per debuggare un kernel che va in kernel panic su una macchina fisica (che chiamo host d'ora in poi) può essere molto utile utilizzare netconsole. Attraverso netconsole potete fare spedire il kernel log via rete ad un altro PC (logger).

...

# BUG_ON

BUG_ON è una macro che svolge il ruolo di una assert ma con logica complementare 

- scatta se la condizione è true
-  BUG_ON == assert del kernel

**NB**: Le macro BUG_ON aiutano tantissimo nel controllo di condizioni anomale, perchè fanno generare al kernel un messaggio di OOPS che contiene il nome del file e la riga del sorgente che contenente il BUG_ON che è scattato

Fare defensive programming nel kernel mi sembra quasi necessario più che una buona pratica dato che in questo contesto il debuggin è molto più complicato rispetto al normale (difficile riprodurre, difficile visualizzare lo stato del kernel, ...). 

Mantenendo una alta percentuale di BUG_ON nel codice:

- si mantiene alta la probabilità che condizioni anomale, e quindi errori, vengano rilevati immediatamente
- inoltre, nel caso di attivazione di un BUG_ON, è altamente probabile che il fallimento accada in un punto del codice vicino all'errore che ne è la causa

Il problema è che i BUG_ON non sono tollerati in codice di produzione... (TODO: non ho capito il perchè, il prof non mi ha convinto molto con il suo discorso su torvalds che non vuole che il suo kernel crasci). Quindi, una soluzione pratica è mantenere una versione di sviluppo ed una di produzione del proprio codice.

Un aiuto simile ai BUG_ON è dato dai WARN_ON, che fanno solo generare un messaggio di OOPS, senza alcun kill di processi o fallimento del sistema.

# Debuggare un oops

Abbiamo visto che il punto in cui accade un OOPS è indicato dalla riga `RIP: nome_func + offset/size_func`

Per poter ritrovare tale punto nel codice sorgente possiamo usare GDB

- è necessario che il kernel sia compilato con debug info
- Ossia che nel .config ci sia: `CONFIG_DEBUG_INFO=y`

Trovate l'opzione attraverso il seguente percorso `Kernel Hacking -> compile-item checks and compiler options -> Compile the kernel with debug info`

- TODO: questo percorso non esiste più trova quello giusto

A questo punto, per ritrovare la riga in cui accade il fallimento, basta invocare gdb nel seguente modo:

- `gdb <nome_file_oggetto_o_modulo_contenente_tale_riga>`
- Nel caso si tratti di un modulo, può darsi che funzioni solo se passate a gdb il file contenente il modulo, ossia il file .ko.
- NB: non passate tutto l'eseguibile del kernel
    - funziona solo se quella riga è relativa ad un componente compilato con built-in
    - ma comunque non è necessario dato che basta il .o del file contenente la funzione colpevole e non l'intero binario enorme che rallenta tutto

esempio:

- se il problema è in una funzione definita dentro bfq-iosched.c, allora `gdb <percorso_build>/block/bfq-iosched.o`
- Poi, da dentro gdb: `list *(nome_funzione+offset)`

FIXME: come faccio a recuperare il nome del file in cui è definita la funzione colpevole?

FIXME: ci interessa il percorso che un kernel oops prende per essere generato? Nel mio caso se mettevo il mio null ptr dereference troppo presto, non riuscivo a recuperare il kernel oops; ho dovuto spostare tutto appena prima di rest_init()

# Kernel oops e unstable state

Linux attempts to avoid kernel crashes (i.e. kernel panics) by killing a faulty process' kernel context during a kernel oops

- Linux continues to run on a compromised reliability

Linux can remain reliable after kernel oops if errors are confined in a kernel context

- Shared kernel objects remain correct
- Non-faulty procs can continue running correctly without rebooting or any complex mechanisms

Possiamo quindi classificare gli oops in due categorie di errori

- process-local error
    - propagates only within the kernel context of a process
    - e.g., kernel stack, function-local data
    - errors can be removed by killing the faulty process
- kernel-global error
    - propagates to data shared among kernel contexts
    - e.g., kernel states, global data, heap data