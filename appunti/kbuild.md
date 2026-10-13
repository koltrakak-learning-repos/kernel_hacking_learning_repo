kbuild è il build system del kernel linux basato su make. Gestisce:

- Creazione dell’immagine del kernel
- Creazione dei moduli
- Creazione dell’initramfs
- Installazione del tutto (in /boot, /lib/modules, etc...)

Le operazioni eseguite make sono stabilite attraverso una combinazione di file:

- Un file di configurazione del kernel (.config)
    - costituito da una sequenza di righe rappresentanti coppie chiave-valore
    - ogni riga rappresenta una opzione di configurazione
    - **Il .config deve essere presente nell’albero di build del kernel** (di default, l’albero di build coincide con l’albero dei sorgenti)
- Un albero di makefile
    - Abbiamo un top Makefile, presente nella radice dell’albero dei sorgenti e un Makefile per ogni sottodirectory dell’albero dei sorgenti.
    - Quasi in ogni caso:
        - Le sottodirectory contengono file sorgenti dai quali si possono generare uno o più moduli
        - Il makefile di una sottodirectory è un kbuild Makefile per tali moduli, ossia contiene le istruzioni per generare tali moduli
- Un albero di file di configurazione
    - Un albero di file di configurazione che definiscono l’insieme (database) di opzioni di configurazione
    - Il Kconfig nella radice dell’albero dei sorgenti si limita a includere tutti Kconfig contenuti in ciascuna delle subdirectory
    - I Kconfig contenuti nelle subdirectory sono quelli che effettivamente contengono le definizioni delle opzioni

Abbiamo quindi makefile gerarchici e file di configurazione gerarchici




kconfig