dentro a open64

   0x000000000041a730 <+256>:	mov    $0x101,%eax
   0x000000000041a735 <+261>:	syscall

# system call e tracing

ricordiamo che il kernel non è un processo ma una libreria di funzioni invocata tramite syscall e interrupt

...

Passare i parametri ad una syscall tramite stack è complicato e non molto performante

- bisogna accedere allo stack del processo utente da dentro il kernel
- bisogna quindi accedere alla tabella delle pagine di questo processo
- passando per la mmu
- magari la pagina non è nel tlb
- ...

di conseguenza se i parametri non sono troppi si usano i registri

- NB: uno di questi registri conterrà l'identificatore della syscall che voglio invocare

...

le syscall sono organizzate come gli interrupt in una tabella che associa id con un handler

le syscall si attivando con un'istruzione dedicata 

- questo nei processori moderni, in passato si attivavano con un interrupt generico. Data però l'importanza dell'esecuzione veloce delle syscall, nei processori moderni è stata aggiunta un'istruzione dedicata che permette una latenza minore
    - immagino si risparmi il codice all'interno dell'interrupt handler che riconosce che l'interrupt è in realtà un syscall

...

purtroppo list fopen non mostra niente dato che la libc installata nel sistema di default non ha le debug info

di conseguenza, piuttosto che guardare il disassemblate e impazzire, andiamo a guardare su elixir con il nostro bel browser

...

Guardare il disassemblato per capire che syscall sta invocando il mio programma è tedioso; **piuttosto posiamo usare strace**

- strace --syscall-number ./a.out
    - permette di listare le systemcall invocate dal binario e mostra anche il syscall number di ciascuna

Inoltre:

- dentro a `/linux/include/linux/syscalls.h` troviamo le firme di tutte le syscalls
- dentro a `/linux/arch/x86/entry/syscalls/syscall_64.tbl` troviamo di nuovo a che syscall è associata ad un determinato numero

## Definizione di una syscall

Non basta una normale definizione di funzione per definire una syscall, perché la syscall ha una modalità di passaggio dei parametri peculiare

- legata al cambio di spazio di indirizzamento
- variabile con l’architettura

La definizione avviene attraverso una macro: `SYSCALL_DEFINEn(<nome_funzione>, <par1>, <par2>, …, <parN>)`

- definisce una funzione con n parametri formali
- chiamata sys_<nome_funzione>
- e con un protocollo di passaggio dei parametri appropriato per l'architettura su cui è compilato il kernel

Andando a osservare come sono definite le syscall notiamo:

- che sono costituite da un po’ di istruzioni di contorno
- seguite da una funzione che contiene il corpo della syscall vero e proprio. 
    - Il nome di tale funzione ha il prefisso `do_sys_`

# ftrace

Una volta invocata una syscall siamo entrati nel kernel!

Problema: il flusso di istruzioni eseguite a partire dalla sua esecuzione può essere complesso da analizzare **staticamente**

- questo flusso attraversa pezzi del kernel con cui non abbiamo familiarità
- il kernel utilizza un'infinità di puntatori a funzione e (callback) che nascondono la vera funzione che sta eseguendo
- inoltre il kernel ha anche un bottom half eseguito a seguito di interrupt
- **ci si può fare dire dal kernel stesso cosa sta eseguendo?**

**ftrace** è un framework del kernel che consente di tracciare la sua stessa esecuzione **dinamicamente (runtime)**

- ftrace is an internal tracer designed to help find what is going on inside the kernel. 
- It can be used for: 
    - debugging 
    - analyzing latencies and performance issues that take place outside of user-space

Although ftrace is typically considered the function tracer, it is really a framework of several assorted tracing utilities; for example:

- there’s latency tracing to examine what occurs between interrupts disabled and enabled
- as well as for preemption
- and from a time a task is woken to the task is actually scheduled in.

One of the most common uses of ftrace is the event tracing

- throughout the kernel there are hundreds of static event points that can be enabled via the tracefs file system to see what is going on in certain parts of the kernel
- NB: evento in questo contesto può significare qualsiasi cosa
    - un evento è un oggetto custom definito per qualsiasi informazione che si ritiene rilevante
    - generalmente si tratta di chiamate a funzione; tuttavia, gli eventi distinguono, ad esempio, tra: funzioni normali, interrupt, syscalls, ...

ftrace uses the tracefs file system to hold the control files as well as the files to display output

When tracefs is configured into the kernel the directory `/sys/kernel/tracing` will be created. 

- to mount this directory `mount -t tracefs nodev /sys/kernel/tracing`
    - TODO: not sure if i actually need to mount it. It seems that the directory is empty, and the mounting activates and populated the tracefs filesystem

NB: tracciare ha un grosso overhead, le tempistiche del kernel vengono alterata pesantemente quando il tracing è attivo. Di conseguenza misurare con tracing attivo rende tutte le misure non affidabili

- montare la directory non significa attivare il tracing

## ftrace tracers

ftrace contiene vari tipi di tracer

- function
    - trace all kernel functions invocations
    - records the invocation timestamp
- **function_graph**
    - simile a function ma traccia anche l'exit delle funzioni e non solo l'invocazione
    - Probing on both ends of a function leads to special features such as:
        - measure of a function’s time execution
        - having a reliable call stack to draw function calls graph
- blk
    - the tracer used by the blktrace user application
    - traccia tutte le (funzioni del blk subsystem?)
- nop
    - this is the “trace nothing” tracer
    - default con cui il tracer è settato
    - to remove all tracers from tracing simply echo “nop” into current_tracer
    - settare a nop dopo un tracing libera la memoria allocata per il trace dentro al kernel

## trace-cmd frontend

è possibile interagire con ftrace direttamente tramite il tracefs montato in /sys/kernel/tracing scrivendo e leggendo i vari pseudofile che il framework espone. 

Considera il seguente esempio in cui si sta abilitando il function tracer per un secondo, per poi stampare il trace prodotto

```
echo function > current_tracer
echo 1 > tracing_on
sleep 1 # questo comando probabilmente non va bene
echo 0 > tracing_on
cat trace
```

Tuttavia, questa è una modalità a basso livello di interazione con ftrace. Esistono frontend di più alto livello come trace-cmd

Dalla manpage: The trace-cmd command interacts with the Ftrace tracer that is built inside the Linux kernel. It interfaces with the Ftrace specific files found in the debugfs file system under the tracing direc tory. 

- **A COMMAND must be specified to tell trace-cmd what to do**
    - record
    - report
    - non so come mai posso scrivere anche trace-cmd-record e trace-cmd-report con il trattino invece di specificare il comando come argomento
- nel man i tracer vengono chiamati plugin (???)

**trace-cmd-record**

- produce un file binario `trace.dat` con il trace
- -p tracer 
    - Specify a tracer. A tracer must be supported by the running kernel
    - To see a list of available tracers, see trace-cmd-list
- -e event
    - Specify an event to trace. 
    - Various static trace points have been added to the Linux kernel. 
        - They are grouped by subsystem where you can enable all events of a given subsystem 
        - or specify specific events to be enabled. 
    - The event is of the format "subsystem:event-name". 
        - You can also just specify the subsystem without the :event-name or the event-name without the "subsystem:"
        - e.g. using "-e sched_switch" will enable the "sched_switch" event whereas, "-e sched" will enable all events under the "sched" subsystem.
- -F
    - This will filter only the executable that is given on the command line lettin you trace only events  that are caused by the given command
- -g function-name
    - only for the function_graph tracer 
    - it will only trace the given function and all functions that it calls
    - you can have more than one -g on the command line
