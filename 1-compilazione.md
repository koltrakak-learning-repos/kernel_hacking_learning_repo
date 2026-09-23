# Istruzioni e Dettagli Tecnici per la Compilazione del Kernel

> **Nota:** Questo documento contiene istruzioni e dettagli tecnici per una delle attività del corso di *kernel hacking*. Per essere guidato nel corso, inizia dalla presentazione `kernel-hacking-1-101`.

---

## Istruzioni Passo-Passo per la Compilazione del Kernel

### 1. Utilizzo di una Macchina Virtuale
Ti conviene usare una macchina virtuale, semplicemente perché, se fai casini, non distruggi il file system del tuo PC.

### 2. Scelta del Virtualizzatore
Se sei sotto Linux, hai due scelte come virtualizzatore: **VirtualBox** e **KVM/QEMU**. 
* VirtualBox è forse un po' più facile da configurare, ma può non rispettare la configurazione *"Non usare la cache dell'host"*, necessaria per test di performance da dentro una VM.
* **Attenzione:** Per entrambi i virtualizzatori, potrebbero non funzionare correttamente su un kernel custom (ossia fatto da te), perché quest'ultimo potrebbe non avere tutte le opzioni necessarie abilitate. Uno dei modi per assicurarsi di avere un kernel con tutte le opzioni necessarie abilitate è eseguire il **passo 6** avendo il virtualizzatore che volete utilizzare in funzione.

### 3. Installazione della Distribuzione
Installa nella macchina virtuale la distribuzione che ti piace di più. In quanto alle dimensioni del disco, ti consiglio di lasciare **30GB liberi**.

### 4. Installazione delle Dipendenze
Installati un po' di dipendenze. 

* **Sistemi APT-based (Debian, Ubuntu, ecc.):**
  ```bash
  sudo apt install git build-essential libncurses5-dev wget bzip2 libssl-dev libelf-dev flex bison dwarves
  ```

* **Sistemi RPM-based (Fedora, CentOS, RHEL):**
  ```bash
  sudo dnf groupinstall "Development Tools"
  sudo dnf install ncurses-devel wget bzip2 openssl openssl-devel elfutils-libelf-devel bc dwarves
  ```
  *Nota su Fedora:* Se manca `gcc`, eseguite:
  ```bash
  sudo dnf install gcc
  ```
  *Risorsa utile per Fedora e simili:* [Building a custom kernel - Fedora Project Wiki](https://fedoraproject.org/wiki/Building_a_custom_kernel)

### 5. Clonare il Repository Mainline
Da dentro la macchina virtuale, clona il repository mainline:
```bash
git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
```

### 6. Configurazione Minimale
Da dentro la cartella del repository che ti sei clonato, esegui:
```bash
make defconfig # crea un config iniziale, necessario per lo script successivo
./scripts/kconfig/streamline_config.pl > minimal_config
```

Questo comando crea un config minimale, con selezionati solo i moduli che il tuo sistema (nella macchina virtuale) sta usando nel momento in cui invochi lo script. 

> ⚠️ **Attenzione:** Stai attento che tutti i moduli che vuoi avere poi disponibili nel tuo nuovo kernel siano già in uso nel sistema quando invochi questo script. Se non sai bene come procedere, invoca tranquillamente questo script ora; poi, se mancheranno dei pezzi nel kernel, ripeterai questa operazione e la compilazione successiva.

*Nota:* Lo script potrebbe segnalare vari errori e ciononostante produrre lo stesso il file di config. Fondamentalmente, per ogni opzione che non riesce a configurare, segnala quello che non è andato come sperava.

*Verifica:* Per controllare se tutto è andato bene, aprite semplicemente il file `minimal_config` e controllate che contenga una sequenza (molto lunga) di opzioni di configurazione, con relativo valore tra `y`, `m` e non settata.

### 7. Backup del Config Esistente
Se per caso avevi già un file di config nella cartella del repository e non vuoi perderlo, mettitelo da parte. Il file di config ha sempre lo stesso nome, e si chiama `.config`.

### 8. Applicare la Configurazione Minimale
Esegui:
```bash
mv minimal_config .config
```
per avere il file di config minimale pronto all'uso per la compilazione del kernel.

---

> 💡 **Metodo Alternativo (Passi 6-7-8):** 
> Una volta diventati un po' più pratici di questi passi, potreste anche eseguire la procedura indicata nella sezione [Metodo per ottenere la configurazione del kernel in esecuzione](#metodo-per-ottenere-la-configurazione-del-kernel-in-esecuzione). Questa procedura vi è utile se volete che il nuovo kernel abbia una configurazione identica a quella del kernel in esecuzione.

---

### 9. Compilazione del Kernel e dei Moduli
> ⚠️ **Importante:** Cercate di non usare `sudo` per la compilazione!

Esegui:
```bash
make -j $(($(nproc) + 1))
make -j $(($(nproc) + 1)) modules
```

> **Nota:** In futuro impareremo che c'è un caso in cui non serve eseguire l'intero `make`. Siccome ora abbiamo eseguito l'intero `make`, in realtà il secondo comando per i moduli è ridondante, ma è bene prenderne nota.

* L'opzione `-j` fa sì che `make` esegua vari thread di compilazione in parallelo. Il numero ideale di thread per avere il massimo delle prestazioni è tipicamente uguale al numero di processori (virtuali) presenti nella macchina più uno: `-j $(($(nproc) + 1))`. Più processori dai alla macchina virtuale, più veloce sarà la compilazione.

#### Modifiche automatiche alla configurazione
All'inizio della compilazione, potrebbe avvenire un passo automatico di modifica della configurazione del kernel. Questo accade in particolare se il `.config` attuale non contiene opzioni presenti nel nuovo kernel. In questa fase vi potrebbe venire chiesto quale valore assegnare ad alcune opzioni: se non sapete cosa scegliere, **accettate il valore di default premendo semplicemente `Enter`**.

#### Risoluzione Errori di Compilazione Comuni
* **Errore OpenSSL:**
  ```text
  fatal error: openssl/opensslv.h: File o directory non esistente
  ```
  *Soluzione:* Installare il pacchetto di sviluppo OpenSSL.
  * Debian/Ubuntu: `sudo apt-get install libssl-dev`
  * Fedora/CentOS/RHEL: `sudo yum install openssl-devel`
  
  Una volta installato il pacchetto giusto, riprova a compilare.

* **Altri fallimenti sconosciuti:**
  Se la compilazione continua a fallire, potrebbe essersi verificata la corruzione di qualche file. Consultare la sezione [Risoluzione errori di compilazione dovuti a corruzione albero sorgenti](#risoluzione-errori-di-compilazione-dovuti-a-corruzione-albero-sorgenti) e ripartire dal punto 9.

### 10. Installazione

#### 10.1. Installazione dei Moduli
```bash
sudo make modules_install
```
Questo comando copia i moduli nella cartella `/lib/modules/<versione-kernel-compilato>/kernel`.

#### 10.2. Installazione dell'Immagine del Kernel

##### Distribuzioni Debian-like (Ubuntu, Mint, ecc.):
```bash
sudo make install
```
Copia l'immagine del kernel, il file di config e l'initramdisk nella cartella `/boot`, configurando anche il bootloader.

*In caso di errori generici (es. `dpkg-query: no packages found matching...`):*
```bash
sudo apt-get auto-remove && sudo apt-get clean && sudo apt-get update && sudo apt-get upgrade
```

##### Distribuzioni Arch-like (ArchLinux, Manjaro, ecc.):
```bash
# Segnatevi la versione del kernel stampata nell'ultima riga da modules_install (es. 4.18.0-bfq-mq-MANJARO+)
sudo cp -v arch/x86_64/boot/bzImage /boot/vmlinuz-<versione-kernel>
sudo mkinitcpio -k <versione-kernel> -g /boot/initramfs-<versione-kernel>.img
sudo cp System.map /boot/System.map-<versione-kernel>
sudo update-grub
```

##### Distribuzioni Fedora-like:
Se il target `install` non aggiorna il bootloader:
```bash
grub2-mkconfig -o /boot/grub2/grub.cfg
```
In caso di sistema EFI:
```bash
grub2-mkconfig -o /boot/efi/EFI/fedora/grub.cfg
```

#### 10.3. Secure Boot
Sarebbe meglio disattivare il **Secure Boot** dal BIOS/UEFI della VM se attivo, altrimenti dovrai gestire la firma digitale del kernel.

### 11. Riavvio
Riavvia la macchina. Al riavvio è molto probabile che il boot loader abbia scelto il kernel appena installato. Per controllare manualmente il kernel da caricare, assicurati che il boot loader si fermi e ti dia modo di scegliere (premendo ad esempio `Shift` o `Esc` all'avvio).

### 12. Verifica
Se la macchina si riavvia correttamente, controlla che il kernel in esecuzione sia il tuo tramite:
```bash
uname -a
```
Nell'output verifica la versione del kernel e la data/ora di compilazione.

---

## Informazioni Aggiuntive e Troubleshooting

### Soluzione Generale nel caso di Malfunzionamenti
Provare ad utilizzare il config di un kernel stock al posto del config minimale:
```bash
cp /boot/config-<kernel_stock> <percorso-albero-sorgenti>/.config
```

---

### Compilazione in una Cartella Separata (Opzionale)
Per mantenere l'albero dei sorgenti pulito, puoi compilare in una cartella esterna:

```bash
mkdir <percorso build directory>
cp <cartella sorgenti>/.config <percorso build directory>

cd <percorso sorgenti>
make mrproper # Da eseguire solo la prima volta!

make -j$(($(nproc) + 1)) O=<percorso build directory>
make -j$(($(nproc) + 1)) O=<percorso build directory> modules
sudo make -j$(($(nproc) + 1)) O=<percorso build directory> modules_install
sudo make -j$(($(nproc) + 1)) O=<percorso build directory> install
```

---

### Risoluzione Errori di Compilazione dovuti a Corruzione Albero Sorgenti
I passi che seguono sono in ordine dal **meno distruttivo al più distruttivo**. Tentateli uno alla volta riprovando la compilazione dopo ciascuno:

1. Se l'errore riguarda specifici file oggetto (`.o`), provare a cancellarli manualmente.
2. Eseguire il clean:
   ```bash
   make O=<build directory> clean
   ```
3. Reset della configurazione (cancella il `.config` nella build directory, fare prima un backup!):
   ```bash
   make O=<build directory> mrproper
   ```
4. Rigenerare il file `.config` (es. eseguendo di nuovo `streamline_config.pl`).
5. Resettare l'albero dei sorgenti Git (cancella tutte le modifiche non committate):
   ```bash
   git reset --hard
   ```

---

### Metodo per ottenere la Configurazione del Kernel in Esecuzione

Sui kernel in esecuzione abilitati, è possibile estrarre la configurazione con:
```bash
cat /proc/config.gz | gunzip > running.config
# oppure
zcat /proc/config.gz > running.config
```

Questa opzione richiede che il kernel corrente sia stato compilato con:
```text
General setup  --->
    [*] Kernel .config support
    [*] Enable access to .config through /proc/config.gz
```

Se il file `/proc/config.gz` non è visibile, provare a caricare il modulo:
```bash
sudo modprobe configs
```

#### Metodo Alternativo per Kernel Stock
Per i kernel stock delle distribuzioni, trovi quasi sempre il file di configurazione direttamente in `/boot`:
`/boot/config-<versione-del-kernel>`

---

### Kernel che si Bloccano al Boot senza Messaggi Utili
Spesso basta rimuovere le opzioni di "silenziamento" del boot loader:

1. Modifica da superuser il file `/etc/default/grub`.
2. Trova la riga `GRUB_CMDLINE_LINUX` e rimuovi le opzioni `quiet` e `splash`.
3. Aggiorna GRUB:
   ```bash
   sudo update-grub
   ```

---

### Mancanza Driver Disco in Initramfs
Se al boot il kernel non riesce ad accedere al disco di sistema, significa che il driver del disco non è presente nell'initramfs.
* **Soluzione:** Ricompilare il kernel abilitando il driver del proprio disco come **builtin** (`[Y]` anziché `[M]`) nel `.config`.
* Generalmente il driver generico **SCSI** risolve 9 volte su 10.
* Documentazione utile:
  * [Gentoo HDD Guide](https://wiki.gentoo.org/wiki/HDD)
  * [Linuxtopia - Kernel Configuration (IDE Disks & Serial ATA)](https://www.linuxtopia.org/online_books/linux_kernel/kernel_configuration/ch09.html)

---

### Mancanza Supporto Filesystem
Se manca il supporto al filesystem della partizione root:
* Consulta la guida: [Linuxtopia - Custom RootFS](https://www.linuxtopia.org/online_books/linux_kernel/kernel_configuration/ch08s02.html#LKN-custom_rootfs)

---

### Problemi con SELinux
In caso di problemi legati a SELinux, la soluzione rapida è disabilitarlo nel `menuconfig` deselezionando l'opzione **NSA SELinux Support**.

---

### Configurazione Supporto Rete
* Guida: [Linuxtopia - Network Device Support](https://www.linuxtopia.org/online_books/linux_kernel/kernel_configuration/ch09s04.html)
* Per identificare la scheda di rete e i driver in uso:
  ```bash
  sudo lspci -v
  # oppure
  sudo lshw
  ```
* *Regola generale:* Se avete dubbi su quale driver sia quello giusto tra vari candidati, abilitateli tutti.

---

### Problemi con Driver Audio
Se lo script `streamline_config.pl` non attiva l'audio, verificate di abilitare manualmente in `menuconfig`:
```text
<M> OSS Mixer API
<M> OSS PCM (digital audio) API
```

---

### Generazione Config da Kernel Stock Ubuntu
1. Visita: `https://kernel.ubuntu.com/~kernel-ppa/mainline/<versione-kernel-vostra>`
2. Applicate le patch (dovrebbero essere 5).
3. Eseguite:
   ```bash
   fakeroot ./debian/rules clean
   ./debian/rules build
   ```
4. Premi `CTRL+C` quando il processo arriva al punto di generare il file di configurazione.
5. Troverai il `.config` in: `debian/build/build/.config`.

---

### Eliminazione Kernel Installati

#### Kernel Stock
Utilizzare il gestore pacchetti della propria distribuzione (es. `apt remove ...` o `dnf remove ...`).

#### Kernel Custom
1. Rimuovere i file da `/boot`:
   ```bash
   sudo rm /boot/*<versione_kernel_da_eliminare>*
   ```
2. Rimuovere i moduli associati:
   ```bash
   sudo rm -rf /lib/modules/*<versione_kernel_da_eliminare>*
   ```
3. Aggiornare il bootloader:
   * **Debian/Ubuntu:**
     ```bash
     sudo update-grub
     ```
   * **Fedora/RPM:**
     ```bash
     grub2-mkconfig -o /boot/grub2/grub.cfg
     # o per EFI:
     grub2-mkconfig -o /boot/efi/EFI/fedora/grub.cfg
     ```

---

### Condivisione Albero Sorgenti Linux tra Host e VM (o Tra Due Macchine)

> **Pre-requisito:** Configurare l'accesso remoto al guest.

#### Gestione File System su Host non-Linux (es. macOS)
Il file system deve supportare la distinzione tra maiuscole e minuscole (*case-sensitive*). Su macOS, crea un'immagine disco formattata come **Mac OS esteso (case-sensitive, journaled)** e montala (es. chiamata `linux-dev.dmg`).

#### Configurazione Lato Host (esempio macOS)
1. Modifica `/etc/exports` aggiungendo la rete della VM:
   ```text
   /Volumes/linux-dev -mapall=501 -alldirs -network 192.168.46.0 -mask 255.255.255.0
   ```
2. Avvia/Riavvia il servizio NFS:
   ```bash
   sudo nfsd enable
   sudo nfsd start
   # Se già attivo:
   sudo nfsd restart
   ```
3. Controlla le esportazioni:
   ```bash
   showmount -e
   ```

#### Configurazione Lato Guest (Linux VM)
1. Crea il punto di mount:
   ```bash
   sudo mkdir -p /mnt/linux-dev/
   ```
2. Modifica `/etc/fstab`:
   ```text
   192.168.46.1:/Volumes/linux-dev /mnt/linux-dev/ nfs rw 0 0
   ```
3. Monta la condivisione:
   ```bash
   sudo mount -a
   ```

#### Indirizzi IP Dinamici tra Macchine Fisiche
In caso di IP dinamici assegnati dal router, puoi montare la directory remota al volo leggendo l'IP dalla sessione SSH:
```bash
addr=$(echo $SSH_CLIENT | awk '{print $1}') && sudo mount $addr:/Volumes/linux-dev /mnt/linux-dev/
```