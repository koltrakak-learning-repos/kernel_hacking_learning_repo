systems performance brendan gregg -> buon libro da leggere

- snello e sopravvive ai tempi

```
btw, avremo dei seminari da gente di Nvidia e AMD
```

...

kernel monolitico:

- l'aggettivo monolitico fa riferimento alla memoria del kernel
- monolitico significa che il kernel ha un unico spazio di indirizzamento
    - semplice
- interessante che un kernel monolitico **può comunque essere modulare**

la controparte dei kernel monolitici sono i microkernel

- qui il kernel contiene funzionalità minime, tipicamente riguardanti IPC

...

Come fa il kernel a sapere cosa eseguire quando arriva un interrupt

- ogni tipologia di interrupt ha un id associato
- il kernel ha tabella che contiene l'associazione: interrupt id - handler corretto
    - ISR -> interrupt service routine
- questa tabella di handler è inizializzata al boot

Le ISR è fondamentale che siano veloci, altrimenti il mio pc passa il tempo a gestire interrupt piuttosto che ad eseguire le mie applicazioni

- tuttavia, a volta un ISR richiede di fare parecchio lavoro
- in questo caso, si fa uso di un interrupt thread: un thread appartenente al kernel, ma comunque un thread preemptible
    - al boot, il kernel crea anche questi 'kernel thread' che usa per servire l'utente
    - l'interrupt thread è un classico worker thread che preleva workitems (puntatori a funzione) da una coda e li serve

...

tasklets / work queues

- c'è un ordine di grandezza di differenza nell'overhead di esecuzione di un ISR rispetto a questo bottom half
    - **interrupt latency**
        - vedi real time systems
    - ad esempio, pago dei cambi di contesto

...

le ISR generalmente causano la disabilitazione degli interrupt

- ma non necessariamente, è supportato anche interrupt nesting
    - di nuovo, vedi real-time systems


...


se il kernel non sta servendo system call/interrupt/trap il kernel schedula l'idle thread

- questo thread fa poco e viene schedulato ogni tanto per gestire operazioni riguardanti power saving e quanto è in sleep un core
- il kernel NON è un programma in esecuzione
- bisogna pensare al kernel come a una libreria/server
- l'unico momento in cui il kernel è in esecuzione è durante il boot, tutto il resto del tempo il kernel è idle

IRQ (interrupt service request)

- cio che arriva non è un interrupt ma più propriamente un IRQ

PREEMPT_RT ...

# Struttura modulare

il kernel è monolitico, ma il codice è il binario no

tutto cio che non è imprescindibile per il kernel viene posto in dei moduli

- nel core risiede tutto quel codice tale che, se rimosso, non esisterebbe neanche un hardware in cui il kernel può eseguire


## Caricamento dei moduli e initramfs/initrd

una parte dei moduli è statica (modulo builtin)

- nel .config =y significa compilalo come builtin

ma non posso renderli tutti statici per ogni possibile hardware, altrimenti ritorno al punto di partenza

il problema al boot quindi potrebbe rimanere

...

initramfs è un File system di tipo tmpfs

...

c'è un rootfs ma non è /

initramfs è un archivio compresso

il kernel scompatta l'archivio e lo mette in rootfs

...

c'è un init nascosto anche in questo initramfs che carica tutti moduli contenuti

- questo è il primo init che spuntava nel kernel log!