il programma apre un file qualsiasi

e poi scatena l'oops leggendo il file speciale (se la bomba è dentro a show())

- il fallimento in realtà viene scatenato in vari modi

...

far fallire un modulo durante il suo init comincia a far soffrire il kernel dato che è in esecuzione tutto il meccanismo di caricamento dei moduli dinamici

- dovrebbe diventare evidente quello che dicevamo riguardo a kernel instabile dopo un oops

take home: anche se il kernel sembra funzionare dopo l'oops, in realtà non ci possiamo più fidare

...


crashtest ci da un menu che ci da tanti modi con cui far crashare il kernel come ci pare

---

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

le syscall si attivando con un'istruzione dedicata (nei processori moderni, in passato scatenando un interrupt)

...

purtroppo list fopen non mostra niente dato che la libc installata nel sistema di default non ha le debug info

di conseguenza, piuttosto che guardare il disassemblate e impazzire, andiamo a guardare su elixir con il nostro bel browser

...

andare a vedere il disassemblato per capire che syscall sta invocando il mio programma è tedioso

piuttosto posiamo usare strace

...

una volta invocata una syscall il flusso di codice eseguito può essere complicato a piacere (siamo entrati dentro al kernel!)

inoltre questo flusso attraversa pezzi del kernel con cui non abbiamo familiarità

per questo motivo anche dentro al kernel abbiamo un tracer: ftrace

NB: tracciare ha un grosso overhead, le tempistiche del kernel vengono alterata pesantemente quando il tracing è attivo. Di conseguenza misurare con tracing attivo rende tutte le misure non affidabili

vari tipi di tracer

- function
    - traccia tutte le invocazioni di funzioni
- blk
    - traccia tutte le 
- nop
    - default con cui il tracer è settato
    - non traccia niente
    - settare a nop dopo un tracing libera la memoria allocata per il trace dentro al kernel

...

nel man i tracer vengono chiamati plugin (???)

...

-F filtra gli eventi riguardanti al comando che abbiamo invocato

...

con eventi