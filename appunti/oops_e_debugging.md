esercizio impegnativo

- cyclictest misura il ritardo che passa tra il momento in cui si sveglia e il momento in cui doveva svegliarsi
    - dice sleep for 1ms e quando si risveglia controlla se è passato 1ms o di più
    - misura la latenza del kernel
        - se HZ è alto, il kernel passa il tempo a gestire interrupt piuttosto che a risvegliare i miei tasks

Cyclictest accurately and repeatedly measures the difference between a thread's intended wake-up time and the time at which it actually wakes up from clock_nanosleep in order to provide statistics about the system's latencies

---

# Approfondimento kernel log

i messaggi del kernel log sopra ad una certa soglia di criticità vengono anche mandati ad una console testuale di linux

Cos'è una console?

- sembra essere cio che emula un terminal emulator
- studia meglio...

Un meccanismo che si usa molto nei sistemi embedded è **reindirizzare i messaggi del kernel su una porta seriale** (piuttosto che su queste console)

- per farlo bisogna modificare la command-line di linux aggiungendo gli argomenti appropriati
- richiede modificare la configurazione di grub
- si può anche fornire input al kernel tramite la porta seriale

...

per vedere cosa esce da una porta seriale emulata da una macchina virtuale bisogna andare a guardare un file prodotto da vbox

`studia tty0/1/... e ttyS0/1/...`

# kernel oops

quando nel kernel avvien un fallimento di qualsiasi tipo questo fallimento viene informalmente chiamato OOPS

Quando succede un oops

- il kernel aggiunge un messaggio in fondo al kernel log
- kill del processo che ha causato l'oops (se è stato causato da un processo)
    - **NB**: l'esecuzione però da lì in poi non è più affidabile (?)
- se le risorse necessarie per andare avanti non sono presenti si ha un KERNEL_PANIC, e diventa necessario un reboot

dentro al messaggio di oops nel kernel log ci sono varie informazioni necessarie per il debugging

- causa del fallimento
- dove si è verificato il fallimento
- stack trace
- ...

...

alcune funzioni nello stack trace dell'oops sono marchiate con un ?

- a causa di ottimizzazioni
- ma anche a causa di interrupt che corrompono lo stack di una funzione

...

BUG_ON è una macro che svolge il ruolo di una assert ma con logica complementare -> scatta se la condizione è true

-  BUG_ON == assert del kernel

...

Problemi di visualizzazione di un oops:

- mandiamo l'output su una porta seriale di una macchina virtuale per questo motivo
- con gdb

...

# Kobject e sysfs

finora è stato solo il kernel a poter parlare con noi tramite il kernel log

sysfs è un meccanismo che permette anche a noi di interagire con il kernel (e viceversa) tramite Kobjects

kobject sono struct con

- un nome
- un ref count
- attributi
- e un padre
    - sono oggetti pensati per strutturati in maniera gerarchica

i kobject sono visibili sotto forma di directory all'interno di un syfs virtual filesystem (un ramfs)

- ogni volta che viene creato un kobject, viene creata una directory in questo sysfs
    - per questo i kobject hanno un padre (padre ~= la directory che lo contiene)
- gli attributi di un kbobject sono file contenuti all'interno della directory del kobject corrispondente

...

sysfs è tipicamente già montato e si trova in /sys

...

L'utente legge e scrive questi file per interagire con il kernel

- lettura/scrittura dei file attributo scatena un hook/callback all'interno del kernel

...

il file associati agli attributi sono file speciali/pseudofile

- leggere questi file ritorna una stringa
- scrivere questi file aggiorna questa stringa mandandola verso il kernel

un attributo va semplicemente pensato come una stringa