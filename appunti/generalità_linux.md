la documentazione è sparsa, difficile da reperire, outdated, ecc.

# Versionamento

https://docs.kernel.org/process/2.Process.html

\<major.minor.bugfix_counter\>

- prima di avere una release ufficiale ci sono delle **release candidates** che incorporano fix (e non nuove features) e rendono mano a mano più stabile una nuova release
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