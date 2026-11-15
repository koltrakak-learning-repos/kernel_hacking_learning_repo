# Che cos'è un terminale?

A physical terminal is **a combination** of input and output devices by which data can be entered into, or output from, a computer. Basically the terminal is the interface to the computer for the human.

- These days, input devices of computers include: keyboard, mouse, and microphone. Output devices includes monitor and speaker. They are commonly called peripheral devices. 
- **The opposite of terminal is the host**, which includes devices like CPU, RAM, Drive and so on.

Computer = Host + Terminal

Terminal = Input devices + Output devices

Ken Thompson and Dennis Ritchie wanted to make Unix become a multi-user operating system, which means each user would have a physical terminal to connect to a singl mainframe computer. Their idea was to use (cheap) teletypes as terminals

- **Teletype (TTY)** was mainly used to send and receive typed messages in telegraphy, but they can also be used to put information into computer and get information out from it. You use the keyboard to input information and print output on paper. This is why the acronym TTY is commonly used to represent the terminal in Unix/Linux.

Over the years, terminals became more and more advanced, soon video terminals were introduced. They had a monitor with a video screen for output.

We can distinguish between two kinds of terminals:

- a character terminal, also known as text terminal, can only output text-based information (e.g. teletypes)
- a graphics terminal can also show graphics/pictures on the screen
    - The most famous graphics terminal is X Terminal, which provided the basic support for the X window system.

Hosts without terminals are referred to as **headless systems**

## Terminal emulators

Abbiamo capito che originariamente un terminale era hardware (pensa a teletype)

Oggigiorno utilizziamo terminali software, i.e. terminal emulators, virtual terminals

- e.g. foot, alacritty, kitty, ghostty, ...
- they are not real terminals, but the OS thinks they are

Il compito principale di un terminal emulator è **fare da tramite tra te e la shell (Bash, Zsh, ...) o direttamente con un programma in esecuzione**:

- cattura l'input: 
    - prende i tasti che digiti sulla tastiera e li invia sotto forma di sequenze di byte alla shell o al programma in esecuzione.
- interpreta l'output e disegna il testo: 
    - riceve flussi di caratteri, li renderizza in una griglia di celle e gestisce i codici di controllo (ANSI escape codes) per colori, grassetto, posizione del cursore, pulizia dello schermo.

Quando apri una finestra di terminale stai avviando un programma che disegna una finestra grafica e vi fa girare dentro una shell

# Che cos'è una console?

In the 1970s and 1980s, terminals were often connected to the host by wire, but there was a special terminal that was attached to the host directly. This special terminal was called (physical) console.

There’s wasn't much difference in appearance between standard terminals and the console, but a console could do something that standard terminals could not. For example if the operating system failed to boot up, error messages would be displayed on the console but not on the terminals

## virtual consoles

Il kernel Linux supporta console virtuali: console logicamente separate che usano la stessa tastiera e lo stesso schermo. Una alla volta è attiva (in primo piano).

- Sono device /dev/tty1, /dev/tty2, ... (fino a 63). 
    - /dev/tty0 indica sempre la console virtuale attualmente attiva.
- Il sistema avvia di solito un login (getty) sulle prime 6 (tty1-tty6). Le altre si attivano al primo utilizzo.
- Una di esse è occupata dalla sessione grafica (tty1 sul mio portatile)
- Per cambiare console: Ctrl + Alt + F1, F2, ...
- You can run the `who` or `tty` command to check which virtual console you are using.

Spesso console e terminale sono usati come sinonimi, perché l'idea è la stessa: un programma cattura l'input da tastiera e mostra l'output sullo schermo. 

Distinzione utile:

- console virtuale
    - simula un **terminale fisico**, a livello di kernel
    - a virtual console is like a full-screen Linux terminal emulator opened by default **from the kernel**
    - on desktop systems, you'll see boot messages (\[ok\] messages) before the display manager starts.
- terminale virtuale 
    - user space program
    - finestra grafica aperta all'interno di un desktop environment che ti dà accesso alla shell

The main difference between the Linux console and graphical terminal emulators is the shells in the Linux console are attached directly to TTY devices (/dev/tty*), whereas the shells in a graphical terminal emulator are attached to pseudo-TTYs (/dev/pts/*).

Quando serve una virtual console?

- per il lavoro quotidiano, non serve.
- La grafica è rotta (driver, compositor che non parte, display manager in crash).
- Un processo ha bloccato la sessione grafica e vuoi ucciderlo (pkill, systemctl restart).
- Rescue/emergency mode, manutenzione senza interfaccia grafica.
- Debug del boot e dei driver.

## tty and pts

I tty sono device file tramite cui il kernel da un nome ai terminali del sistema. È l'interfaccia che permette di accedere a un terminale.

- These ttys may be virtual terminals **or directly connected to a serial port**
    - nel caso di porte seriali, quest'ultime possono essere collegate a dell'hardware reale (e.g. spi peripheral), oppure possono essere virtuali (e.g. file di dump di virtualbox)
- they are an abstraction for interfaces that send/receive characters to/from the computer

Sotto /dev si trovano molti file tty:

| Device                    | Significato |
|---------------------------|-------------|
| /dev/tty                  | Terminale di controllo del processo corrente |
| /dev/tty1 ... /dev/ttyN   | Console virtuali |
| /dev/tty0                 | Console virtuale attiva in questo momento |
| /dev/console              | Console di sistema (destinazione dei messaggi del kernel) |
| /dev/ttyS0 ...            | Porte seriali |
| /dev/pts/N                | Pseudo-terminali (emulatori, SSH, tmux) |

Abbiamo quindi che le console sono rappresentate da /dev/tty1..6

Le istanze di terminal emulator / remote ssh sessions sono rappresentate da /dev/pts1..n

- pts = pseudo terminal slave

### scrivere e leggere tty

Ricorda che terminali e console in passato erano sostanzialmente composti da monitor + tastiera

Oggi però sia terminali che console sono virtuali, e i tty device file fungono da interfaccia per le molteplici istanze virtuali di quest'utimi:

L'azione di scrivere un tty device file corrisponde logicamente a un computer che legge l'input di una tastiera da un terminale immaginario. Analogamente, leggere un tty device file corrisponde logicamente a un computer che mostra un messaggio sul monitor di un terminale immaginario. 

Andando a vedere a cosa corrisponde il tty device file che stiamo leggendo/scrivendo è facile intuire l'effetto di queste letture/scritture. Ad esempio se leggo un tty device file associato alla finestra di un terminal emulator, le scritture scrivono all'interno di questa finestra, mentre le letture intercettano i caratteri scritti su questa finestra

- scrittura: `echo "ciao"> /dev/pts/1`
- lettura: `cat /dev/pts/1`

### printing on a console and console redirection

I messaggi del kernel nel kernel log vanno sulla console di sistema, /dev/console

- scelta col parametro di boot console=. Su un portatile di default è lo schermo (tty0);
- infatti se togliamo quiet vediamo queste stampe al boot sul nostro monitor, i.e. console (prima che la grafica parta)

Similmente, i messaggi [ OK ] che vedi durante l'avvio, prima del display manager, sono output della console (spesso nascosti da uno splash screen come Plymouth);

Come mai?

- Un programma normale stampa con printf su stdout, cioè su un terminale. 
- Il kernel però parte prima di tutto: non c’è ancora una shell, né un filesystem, né un terminale
- Eppure deve poter dirti «ho trovato questo disco» o «driver X ha fallito». 
- Gli serve quindi un posto dove scrivere che non dipenda da nulla di userspace.

Quando il kernel chiama printk("...") per aggiungere una entry al kernel log succedono due cose:

- Il messaggio finisce in un buffer circolare in memoria
    - quello che leggi con dmesg
- Il messaggio viene mandato anche ai driver di console registrati, cioè dispositivi di output che il kernel sa pilotare da solo: 
    - lo schermo (console virtuale, tramite framebuffer)
    - oppure una porta seriale
    - o magari anche la rete

Il kernel quindi prende i caratteri e li scrive direttamente su un dispositivo di output che sa gestire da solo, senza passare da shell o emulatori.

Perché serve proprio la console

Mostra i messaggi in tempo reale anche se il sistema si pianta a metà boot, quando dmesg non puoi lanciarlo perché non hai una shell.

- Su un portatile i messaggi del boot che vedi a schermo (se non c’è lo splash) arrivano da qui.
- Su un server senza schermo puoi dirgli console=ttyS0 e i messaggi escono dalla seriale, dove li legge un altro computer.
- su server e dispositivi embedded la console può essere una porta seriale (console=ttyS0,115200), utilissima per il debug.

# Cos'è una shell?

Il terminale è solo un meccanismo per trasferire informazioni da/verso l'host. Affinche il sistema operativo capisca cosa vuoi fare serve una shell: un programma che interpreta i comandi che scrivi e lancia altri programmi.

- shell == interprete dei comandi
- All'apertura di un terminale viene avviata automaticamente una shell.
- Esistono molte shell: Bash, Zsh, Fish, Ksh, Csh, Dash, ...
- Quale shell sto usando adesso? 
    - echo $0. 
    - echo $SHELL mostra invece la shell di login predefinita
- Cambiare la shell predefinita: `chsh -s /bin/zsh`
- La shell non è il terminale, puoi lanciare una shell diversa dentro lo stesso emulatore, e lo stesso emulatore può funzionare con shell diverse.

# Comandi utili

| Comando                           | A cosa serve |
|-----------------------------------|--------------|
| tty                               | Stampa il device del terminale in uso (/dev/tty3 o /dev/pts/0) |
| who / w                           | Utenti collegati e relativo tty |
| echo $0                           | Shell in uso |
| fgconsole                         | Numero della console virtuale attiva |
| loginctl                          | Sessioni e relative VT |
| cat /sys/class/tty/console/active | Console di sistema attiva |
| cat /proc/cmdline                 | Parametri di boot, incluso console= |