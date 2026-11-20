hypervisor immaginiamocelo come uno strato intermedio tra vm e hardware

- astrae l'hardware per le vm


Dom0 è la macchina di management per tutte le altre

DomU sono tutte le altre vm

- su queste posso eseguire tutto quello che mi pare (nei limiti del supporto dell'hypervisor xen)

...

esempio reale automotive:

- 2 board distinte con architetture diverse (una arm e l'altra x86)
- xen hypervisor gestisce questo hardware eterogeneo in maniera trasparente per le vm sopra

...

freedom from interference tra vm viene garantito dall'hypervisor

...

irq latency

- quanto tempo faccio eseguire questi test per la misura della irq latency?
- NB: la latenza del caso peggiore è quello che ci interessa (latenza del caso medio non ci interessa niente)