Questo documento di testo contiene istruzioni e dettagli tecnici per una delle attività del corso di kernel hacking. Per essere guidato nel corso, inizia dalla presentazione kernel-hacking-1-101.

---

# APPROFONDIMENTO KERNEL LOG

Come abbiamo visto, il kernel scrive i propri messaggi per l'utente all'interno di un array circolare di messaggi, chiamato kernel log.

Abbiamo visualizzato il contenuto del kernel log con il comando

`dmesg -H`

Ogni messaggio riporta anche l'istante in cui è stato generato, a partire dall'accensione del sistema.

I messaggi al di sopra di una certa soglia di criticità (che si può configurare con dmesg) non vengono solo inseriti nel kernel log, ma vengono anche stampati automaticamente su almeno una delle console testuali del sistema. Tipicamente si tratta della prima.

Infine, si può configurare il kernel per spedire tutti i suoi messaggi su console seriale, ossia sulla porta seriale. Per esempio, se si usa grub per il boot, editate il file
`/etc/default/grub`

e modificate il seguente parametro come segue:

`GRUB_CMDLINE_LINUX="<quello che già c'era> console=ttyS0 ignore_loglevel"`

quindi aggiornare grub con:

`sudo update-grub`

oppure, in distribuzioni tipo Fedora:
`sudo grub2-mkconfig -o /boot/grub2/grub.cfg`

e riavviare.

Questo è particolarmente utile se il kernel è eseguito in una macchina virtuale, perché la macchina virtuale presenta al guest delle interfacce seriali virtuali. Potrebbe essere necessario riconfigurare la macchina virtuale, nel caso in cui la configurazione attuale non preveda la presenza della porta seriale.

Per leggere l'output della porta seriale, le macchine virtuali forniscono di norma una delle seguenti modalità. La prima è che i byte che il kernel spedisce sulla porta venga scritto all'interno di un file. Ad esempio, con VirtualBox, basta andare su Porte, abilitare una delle porte seriali, scegliere la modalità raw per tale porta, e scrivere il percorso del file in cui si vuole che finisca il log del kernel. L'altra modalità è che la macchina spedisca a sua volta tali byte ad un device, dal quale poi li si può leggere.

NOTA: se si redireziona console sulla porta seriale, si perde la console a schermo. Quindi, ad esempio, se parte una shell di emergenza, non la si può usare. Per poter tornare ad usarla, dovete rimuovere il redirezionamento dalla riga di comando.


---

# KERNEL OOPS

Quando nel kernel avviene un fallimento di qualsiasi tipo (system fault), tale fallimento viene informalmente chiamato OOPS (nome tratto proprio da ciò che si esclama quando accade qualche guaio). Vengono compiute le seguenti azioni:
- Il kernel aggiunge in fondo al kernel log un messaggio per informare dell'accaduto: il cosiddetto messaggio di (errore per il) kernel oops.
- kill del processo interessato, per poter continuare l'esecuzione del sistema, esecuzione che però non è più affidabile
- se le risorse necessarie per andare avanti non sono più disponibili, a causa del kill del processo, allora si ha un cosiddetto KERNEL PANIC; diventa necessario un reboot (automatico o con comando dell'utente).

Il messaggio di oops contiene informazioni fondamentali per cercare l'errore che ha causato il fallimento:
- Che fallimento si è verificato
- Dove si è verificato il fallimento
- Stack trace che ha portato fino all'esecuzione dell'istruzione che ha causato il fallimento
Ad esempio, ecco un OOPS causato dalla dereferenziazione di un puntatore nullo (ma non uguale a zero).

```text
[  116.960673] BUG: kernel NULL pointer dereference, address: 00000000000001a8
[  116.962021] #PF: supervisor read access in kernel mode
[  116.962949] #PF: error_code(0x0000) - not-present page
[  116.963881] PGD 0 P4D 0 
[  116.964375] Oops: 0000 [#1] SMP KASAN PTI
[  116.965208] CPU: 1 PID: 1432 Comm: sh Tainted: G    B             5.6.0-bfq+ #5
[  116.966655] Hardware name: Parallels Software International Inc. Parallels Virtual Platform/Parallels Virtual Platform, BIOS 1.5.0 (20116) 01/27/2020
[  116.969044] RIP: 0010:bfq_bfqq_save_state+0x7d/0x890 [bfq]
[  116.970105] Code: f1 f1 f1 f1 c7 40 24 f3 f3 f3 f3 c7 40 28 f3 f3 f3 f3 65 48 8b 04 25 28 00 00 00 48 89 84 24 90 01 00 00 31 c0 e8 c3 d4 e5 ce <48> 8b ab a8 01 00 00 48 85 ed 0f 84 b1 03 00 00 48 8d 7b 18 e8 aa
[  116.973829] RSP: 0018:ffff8881b1fe6970 EFLAGS: 00010046
[  116.974889] RAX: 0000000000000000 RBX: 0000000000000000 RCX: ffffffffc063eecd
[  116.976317] RDX: 0000000000000000 RSI: 0000000000000008 RDI: 00000000000001a8
[  116.977502] RBP: 0000000000000000 R08: ffffffff8f1d4b34 R09: fffffbfff22d595b
[  116.978686] R10: fffffbfff22d595a R11: ffffffff916acad7 R12: 1ffff110363fcd31
[  116.979878] R13: 0000000000000000 R14: ffff88815e963470 R15: 0000000000000000
[  116.981094] FS:  00007fa99982a740(0000) GS:ffff8881cf480000(0000) knlGS:0000000000000000
[  116.982421] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[  116.983373] CR2: 00000000000001a8 CR3: 000000015bf38005 CR4: 00000000003606e0
[  116.984585] Call Trace:
[  116.985016]  ? deref_stack_reg+0x8b/0xd0
[  116.985687]  ? bfq_check_fifo+0x4e0/0x4e0 [bfq]
[  116.986513]  ? __lock_acquire+0xa6/0x2620
[  116.987196]  ? __lock_acquire+0xa6/0x2620
[  116.987879]  ? __lock_acquire+0xa6/0x2620
[  116.988563]  ? lockdep_hardirqs_on+0x260/0x260
[  116.989315]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  116.990122]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  116.991029]  ? lock_downgrade+0x360/0x360
[  116.991706]  ? lock_acquire+0xe5/0x1f0
[  116.992348]  ? bfq_merge_bfqqs+0x5/0x1180 [bfq]
[  116.993117]  bfq_merge_bfqqs+0x192/0x1180 [bfq]
[  116.993887]  ? bfq_init_entity+0x27a/0x630 [bfq]
[  116.994668]  ? bfq_release_process_ref+0x90/0x90 [bfq]
[  116.995524]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  116.996333]  ? lockdep_hardirqs_on+0x260/0x260
[  116.997077]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  116.997878]  ? lockdep_hardirqs_on+0x260/0x260
[  116.998622]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  116.999436]  ? lock_downgrade+0x360/0x360
[  117.000189]  ? lock_acquire+0xe5/0x1f0
[  117.000838]  ? bfq_bio_merge+0x250/0x250 [bfq]
[  117.001602]  bfq_get_queue+0x784/0x1390 [bfq]
[  117.002351]  ? bfq_merge_bfqqs+0x1180/0x1180 [bfq]
[  117.003160]  ? deref_stack_reg+0x8b/0xd0
[  117.003828]  ? __read_once_size_nocheck.constprop.0+0x10/0x10
[  117.004843]  ? stack_trace_consume_entry+0x80/0x80
[  117.005645]  ? __orc_find+0x68/0xc0
[  117.006237]  ? ftrace_graph_ret_addr+0x1f/0x90
[  117.006983]  ? stack_access_ok+0x3a/0x90
[  117.007644]  ? rcu_read_lock_held_common+0x3e/0x90
[  117.008443]  ? rcu_read_lock_held+0x5f/0xb0
[  117.009145]  ? rcu_read_lock_sched_held+0xc0/0xc0
[  117.010048]  ? __read_once_size_nocheck.constprop.0+0x10/0x10
[  117.011093]  ? deref_stack_reg+0xd0/0xd0
[  117.011755]  ? blk_mq_get_request+0x6eb/0x9a0
[  117.012491]  ? __orc_find+0x68/0xc0
[  117.013082]  ? ftrace_graph_ret_addr+0x1f/0x90
[  117.013836]  ? bfq_find_set_group+0x19c/0x230 [bfq]
[  117.014729]  bfq_get_bfqq_handle_split+0x12b/0x7c0 [bfq]
[  117.015818]  ? init_object+0x4e/0x80
[  117.016573]  ? __lock_acquire+0xa6/0x2620
[  117.017428]  ? bfq_finish_requeue_request+0x790/0x790 [bfq]
[  117.018655]  ? lockdep_hardirqs_on+0x260/0x260
[  117.019611]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.020671]  ? lock_downgrade+0x360/0x360
[  117.021515]  ? kernfs_name_hash+0x90/0x90
[  117.022353]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.023349]  ? lock_downgrade+0x360/0x360
[  117.024188]  ? lockdep_hardirqs_off+0x1a/0x100
[  117.025143]  ? kernfs_path_from_node+0x4c/0x60
[  117.026068]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.026883]  bfq_init_rq+0x205/0x12a0 [bfq]
[  117.027590]  ? lock_downgrade+0x360/0x360
[  117.028278]  ? bfq_get_bfqq_handle_split+0x7c0/0x7c0 [bfq]
[  117.029194]  ? __lock_acquire+0xa6/0x2620
[  117.029914]  ? blk_account_io_start+0x241/0x5a0
[  117.030671]  ? blk_account_io_done+0x660/0x660
[  117.031411]  ? blk_mq_get_request+0x73a/0x9a0
[  117.032138]  ? lockdep_hardirqs_on+0x260/0x260
[  117.032886]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.033684]  ? do_raw_spin_lock+0x119/0x1d0
[  117.034383]  ? rwlock_bug.part.0+0x60/0x60
[  117.035129]  bfq_insert_request+0xff/0xb20 [bfq]
[  117.035906]  ? lock_downgrade+0x360/0x360
[  117.036593]  ? __bfq_insert_request+0x16e0/0x16e0 [bfq]
[  117.037465]  ? generic_make_request+0x14d/0x630
[  117.038228]  ? __lock_acquire+0xa6/0x2620
[  117.038907]  ? blk_queue_enter+0x730/0x730
[  117.039601]  ? lockdep_hardirqs_on+0x260/0x260
[  117.040409]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.041205]  ? lock_downgrade+0x360/0x360
[  117.041886]  bfq_insert_requests+0xb2/0xe0 [bfq]
[  117.042845]  blk_mq_sched_insert_requests+0x119/0x330
[  117.043687]  blk_mq_flush_plug_list+0x246/0x3e0
[  117.044463]  ? blk_mq_insert_requests+0x290/0x290
[  117.045290]  blk_flush_plug_list+0x20f/0x250
[  117.046004]  ? blk_insert_cloned_request+0x160/0x160
[  117.046829]  ? lock_downgrade+0x360/0x360
[  117.047502]  blk_finish_plug+0x3c/0x54
[  117.048132]  read_pages+0x104/0x340
[  117.048721]  ? debug_lockdep_rcu_enabled+0x23/0x60
[  117.049525]  ? read_cache_pages+0x350/0x350
[  117.050255]  ? __alloc_pages_slowpath+0x1480/0x1480
[  117.051068]  ? policy_nodemask+0x1a/0xa0
[  117.051724]  ? policy_node+0x56/0x60
[  117.052328]  __do_page_cache_readahead+0x28b/0x2b0
[  117.053126]  ? read_pages+0x340/0x340
[  117.053743]  ? lock_acquire+0xe5/0x1f0
[  117.054378]  ? page_cache_sync_readahead+0x43/0x290
[  117.055287]  ondemand_readahead+0x2c7/0x610
[  117.055991]  generic_file_read_iter+0x746/0x16b0
[  117.056765]  ? lock_downgrade+0x360/0x360
[  117.057442]  ? filemap_write_and_wait_range+0xd0/0xd0
[  117.058281]  ? lockdep_hardirqs_on+0x11/0x260
[  117.059009]  ? _raw_spin_unlock_irq+0x24/0x30
[  117.059847]  ? finish_task_switch+0xc3/0x380
[  117.060703]  ? finish_task_switch+0x8c/0x380
[  117.061561]  ? __switch_to+0x2ee/0x5f0
[  117.062328]  ? __switch_to_asm+0x34/0x70
[  117.063137]  new_sync_read+0x27c/0x3a0
[  117.063917]  ? __ia32_sys_llseek+0x1d0/0x1d0
[  117.064915]  ? security_file_permission+0xe7/0x190
[  117.065777]  vfs_read+0xc1/0x1b0
[  117.066333]  kernel_read+0x78/0xc0
[  117.066945]  prepare_binprm+0x16f/0x350
[  117.067745]  ? install_exec_creds+0xb0/0xb0
[  117.068614]  ? get_user_arg_ptr.isra.0+0x2c/0x70
[  117.069554]  __do_execve_file.isra.0+0x87e/0x1030
[  117.070572]  ? copy_strings_kernel+0xa0/0xa0
[  117.071452]  ? strncpy_from_user+0xbe/0x1b0
[  117.072323]  __x64_sys_execve+0x54/0x60
[  117.073125]  do_syscall_64+0x74/0xd0
[  117.073875]  entry_SYSCALL_64_after_hwframe+0x49/0xbe
[  117.074908] RIP: 0033:0x7fa9998fb49b
[  117.075510] Code: 41 89 01 eb da 66 2e 0f 1f 84 00 00 00 00 00 f7 d8 64 41 89 01 eb d6 0f 1f 84 00 00 00 00 00 f3 0f 1e fa b8 3b 00 00 00 0f 05 <48> 3d 01 f0 ff ff 73 01 c3 48 8b 0d bd 39 0f 00 f7 d8 64 89 01 48
[  117.078565] RSP: 002b:00007ffe1f3394b8 EFLAGS: 00000246 ORIG_RAX: 000000000000003b
[  117.079853] RAX: ffffffffffffffda RBX: 0000562d9b10ff60 RCX: 00007fa9998fb49b
[  117.081020] RDX: 0000562d9b10ddb0 RSI: 0000562d9b110260 RDI: 0000562d9b110160
[  117.082185] RBP: 0000562d9b110160 R08: 0000562d9b110260 R09: 0000000000000000
[  117.083351] R10: 0000000000000030 R11: 0000000000000246 R12: 00000000ffffffff
[  117.084516] R13: 0000562d9b110260 R14: 0000562d9b10ddb0 R15: 0000000000000001
[  117.085731] Modules linked in: x86_pkg_temp_thermal coretemp crct10dif_pclmul crc32_pclmul ghash_clmulni_intel intel_rapl_perf bfq sbs sbshc pvpanic snd_intel8x0 snd_ac97_codec ac97_bus snd_seq snd_seq_device snd_pcm snd_timer snd virtio_balloon soundcore i2c_i801 lpc_ich virtio_net net_failover failover crc32c_intel serio_raw ata_generic virtio_pci virtio_ring pata_acpi virtio
[  117.091505] CR2: 00000000000001a8
[  117.092208] ---[ end trace 363e6edcaf323932 ]---
[  117.093156] RIP: 0010:bfq_bfqq_save_state+0x7d/0x890 [bfq]
[  117.094235] Code: f1 f1 f1 f1 c7 40 24 f3 f3 f3 f3 c7 40 28 f3 f3 f3 f3 65 48 8b 04 25 28 00 00 00 48 89 84 24 90 01 00 00 31 c0 e8 c3 d4 e5 ce <48> 8b ab a8 01 00 00 48 85 ed 0f 84 b1 03 00 00 48 8d 7b 18 e8 aa
[  117.097426] RSP: 0018:ffff8881b1fe6970 EFLAGS: 00010046
[  117.098299] RAX: 0000000000000000 RBX: 0000000000000000 RCX: ffffffffc063eecd
[  117.099476] RDX: 0000000000000000 RSI: 0000000000000008 RDI: 00000000000001a8
[  117.100711] RBP: 0000000000000000 R08: ffffffff8f1d4b34 R09: fffffbfff22d595b
[  117.101887] R10: fffffbfff22d595a R11: ffffffff916acad7 R12: 1ffff110363fcd31
[  117.103051] R13: 0000000000000000 R14: ffff88815e963470 R15: 0000000000000000
[  117.104217] FS:  00007fa99982a740(0000) GS:ffff8881cf480000(0000) knlGS:0000000000000000
[  117.105579] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[  117.106533] CR2: 00000000000001a8 CR3: 000000015bf38005 CR4: 00000000003606e0:
```

Dal contenuto dei registri si può capire nel dettaglio quello che è successo. Si vede che nei registri RDI e CR2 c'è il valore 1a8, che è proprio l'indirizzo di fatto NULL a cui si è tentato di accedere. In particolare CR2 ci dice l'ultimo indirizzo di memoria acceduta prima del crash.

Se volete approfondire l'argomento (non richiesto per il corso), in fondo a questo documento trovate molti dettagli e link, forniti da Chiara Bruschi.


---

# DEBUGGING PROATTIVO CON I BUG_ON

In merito agli OOPS, le macro BUG_ON aiutano tantissimo nel controllo di condizioni anomale, perché forzano uno speciale tipo di fallimento, che fa generare al kernel un messaggio di OOPS che contiene il nome del file ed il numero di riga del sorgente in cui si trova il BUG_ON che ha causato il fallimento. Ad esempio:

```text
[  256.442395] kernel BUG at block/bfq-iosched.c:5209!
[  256.443333] invalid opcode: 0000 [#1] SMP KASAN PTI
[  256.444131] CPU: 1 PID: 2240 Comm: upowerd Not tainted 5.6.0-rc6-bfq+ #2
[  256.445369] Hardware name: Parallels Software International Inc. Parallels Virtual Platform/Parallels Virtual Platform, BIOS 1.5.0 (20116) 01/27/2020
[  256.447826] RIP: 0010:bfq_has_work+0x299/0x390 [bfq]
[  256.448658] Code: c0 e8 2b 90 d4 d3 48 89 ef e8 d3 54 02 00 85 c0 0f 85 81 fe ff ff 48 8d 7d 44 e8 e2 06 01 d4 8b 45 44 85 c0 0f 8e 6d fe ff ff <0f> 0b e8 50 d4 d7 d3 84 c0 75 be 48 c7 c2 40 d0 4b c0 be 8b 02 00
[  256.451913] RSP: 0018:ffff888198489cc8 EFLAGS: 00010202
[  256.452936] RAX: 0000000000000001 RBX: 0000000000000000 RCX: dffffc0000000000
[  256.454139] RDX: 0000000000000007 RSI: 0000000000000003 RDI: ffff888192533844
[  256.455318] RBP: ffff888192533800 R08: ffffffffc048b92e R09: fffffbfff2cd58bb
[  256.456520] R10: fffffbfff2cd58ba R11: ffffffff966ac5d7 R12: 0000000000000000
[  256.457890] R13: ffffffffc04d0000 R14: ffff88817fe9a838 R15: ffff88817fe9a938
[  256.459174] FS:  00007f852b324840(0000) GS:ffff888198480000(0000) knlGS:0000000000000000
[  256.460613] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[  256.461748] CR2: 00005644a7dc1338 CR3: 000000008bde6005 CR4: 00000000003606e0
[  256.462949] Call Trace:
[  256.463382]  <IRQ>
[  256.463752]  blk_mq_run_hw_queue+0x18e/0x1a0
[  256.464483]  ? blk_mq_delay_run_hw_queue+0x20/0x20
[  256.465299]  ? mq_flush_data_end_io+0x108/0x150
[  256.466070]  scsi_end_request+0x17b/0x3a0
[  256.466866]  scsi_io_completion+0x116/0x7d0
....
```

I BUG_ON sono fondamentalmente l'equivalente delle assert in C. Possono essere utilizzati come strumento di defensive programming, o più precisamente di offensive programming. Mantenendo una alta percentuale di BUG_ON in un programma, si mantiene alta la probabilità che condizioni anomale, e quindi errori, vengano rilevati immediatamente. Inoltre, nel caso di attivazione di un BUG_ON, si tiene alta la probabilità che il fallimento accada in un punto del codice vicino all'errore che ne è la causa. Questo rende molto più facile trovare l'errore.

C'è ovviamente il problema che è poco probabile che dei BUG_ON siano tollerati in codice di produzione. Queste le opinioni di Torvalds in merito:
https://yarchive.net/comp/linux/BUG.html

Quindi, una soluzione pratica è mantenere una versione di sviluppo ed una di produzione del proprio codice. Ecco ad esempio le versioni di sviluppo di BFQ che mantenevo quando sviluppavo lo scheduler:
https://github.com/Algodev-github/bfq-mq

Ed eccovi un esempio molto pratico dei vantaggi. Il 31 ottobre arriva una segnalazione di fallimento del mio I/O scheduler BFQ. Poi altre due il 6 Novembre:
https://bugzilla.kernel.org/show_bug.cgi?id=205447
Nelle varie segnalazioni, il crash avviene in punti sempre diversi di BFQ. Capire la causa dell'errore da questi OOPS sarebbe stato difficilissimo. Allora ho semplicemente condiviso la versione di sviluppo di BFQ, piena zeppa di BUG_ON. Ed è arrivato un OOPS molto più informativo (commento 8):
https://bugzilla.kernel.org/show_bug.cgi?id=205447#c8
Ho potuto controllare questo nuovo OOPS il giorno dopo. Dall'OOPS ho capito il problema in 5 minuti, ed ho potuto scrivere e condividere la soluzione dopo un'ora (commento 10). Nello stesso giorno l'utente ha risposto che il problema era risolto. Senza quei BUG_ON, questo sarebbe stato semplicemente impossibile.

Un aiuto simile è dato dai WARN_ON, che fanno solo generare un messaggio di OOPS, senza alcun kill di processi o fallimento del sistema.


---

# PROBLEMI DI VISUALIZZAZIONE DI UN OOPS

Purtroppo se un OOPS genera un kernel panic, allora non lo si può più visualizzare con dmesg, semplicemente perché il sistema si pianta. Esiste un meccanismo del kernel per preservare un minimo di controllo in situazioni non troppo gravi (magic SysRq key), ma non lo trattiamo in queste note.

Come già detto detto, i messaggi critici del kernel vengono stampati automaticamente su almeno una delle console di testo. Però, non si vedono ovviamente tali messaggi se ci si trova in ambiente grafico, e, dopo il blocco del sistema, è impossibile commutare su una console di testo.

Alla luce di questi fatti, ecco alcuni sistemi per visualizzare un messaggio di OOPS dovuto ad un kernel panic.

## CONSOLE DI TESTO

Se si è scoperto come riprodurre l'OOPS, allora commutare sulla prima console di testo ed eseguire i comandi che portano all'OOPS. Se state usando QEMU state di fatto già interagendo attraverso tale console.

Sulla prima console si dovrebbe vedere il messaggio di OOPS.

Se state usando qemu da dentro un terminale virtuale, potete analizzare il messaggio senza problemi, perché rimane memorizzato nel buffer di righe del terminale virtuale (e potete scorrere il messaggio a piacimento).

Se invece siete su una console su un host fisico o dentro un guest, allora uno dei problemi di questa modalità di visualizzazione di un messaggio di OOPS è che il messaggio ha tipicamente più righe della console di testo. Quindi se ne vede solo la parte finale, ed a volte questa parte non basta per capire la causa del fallimento.

## PORTA SERIALE DI UNA MACCHINA VIRTUALE

Come spiegato all'inizio di queste note, diventa estremamente facile visualizzare un OOPS se si sono rediretti i messaggi del kernel sulla porta seriale di una macchina virtuale, e se tale la macchina virtuale è configurata per scrivere su un file i caratteri che passano attraverso tale porta seriale.

## NETCONSOLE

Per vari motivi potreste non riuscire ad utilizzare una macchina virtuale per visualizzare il massaggio di OOPS (perché, ad esempio, il fallimento non si riproduce nella macchina virtuale, o perché non avete le risorse per creare ed eseguire una macchina virtuale).

In questo caso, per debuggare un kernel che va in kernel panic su una macchina fisica (che chiamo host d'ora in poi) può essere molto utile utilizzare netconsole. Attraverso netconsole potete fare spedire il kernel log via rete ad un altro PC (logger).

NOTA: per la mia esperienza, netconsole funziona SOLO con interfacce di rete fissa. Quindi, se ad esempio avete un router a cui il logger è collegato via wifi, dovete collegare l'host al router attraverso la rete fissa.

Trovate documentazione completa ad esempio qui:
https://www.kernel.org/doc/Documentation/networking/netconsole.txt

Qui vi riporto invece un esempio di uso molto semplice.

1. Abilitare netconsole come modulo del kernel
`Device Drivers -> Network device support -> Network core driver support -> Network console logging support`
poi ricompilare, reinstallare il kernel e riavviare (non basta reinstallare solo i moduli quando si aggiunge un nuovo modulo)

2. Attivare la ricezione sul logger dalla porta 6666 (quella su cui netconsole spedirà):
`nc -u -l 6666` # su sistemi stile BSD (tipo OS X), controllare la sintassi per altri sistemi

3. Attivare netconsole sull'host. Ad esempio, per mandare tutti i messaggi di kernel log da un host ad logger
- calcolando automaticamente l'indirizzo IP del logger nel caso in cui si sia collegati via ssh all'host proprio dal logger, oppure usando 192.168.0.100 come indirizzo IP del logger
- spedendo sulla porta 6666 del logger
- passando dall'interfaccia di rete enp0s25 dell'host (cercate il nome della vostra interfaccia di rete con comandi come ip o ifconfig)
- utilizzando la porta 6665 dell'host (valore di default):
```bash
if [[ "$SSH_CLIENT" != "" ]]; then
        addr=$(echo $SSH_CLIENT | awk '{print $1}')
else
        addr=192.168.0.100
fi
sudo dmesg -n debug # mandare qualsiasi messaggio sulle console (quindi poi su netconsole)
sudo modprobe netconsole netconsole="@/enp0s25,@$addr/"
```

Se tutto funziona, si riceve immediatamente sul logger una serie di righe tipo questa:
```text
[  141.380539] netpoll: netconsole: local port 6665
[  141.380545] netpoll: netconsole: local IPv4 address 0.0.0.0
[  141.380548] netpoll: netconsole: interface 'enp0s25'
[  141.380550] netpoll: netconsole: remote port 6666
[  141.380553] netpoll: netconsole: remote IPv4 address 192.168.0.101
[  141.380555] netpoll: netconsole: remote ethernet address ff:ff:ff:ff:ff:ff
[  141.380560] netpoll: netconsole: local IP 192.168.0.107
[  141.380638] console [netcon0] enabled
[  141.380649] netconsole: network logging started
```

Se non si riceve subito queste righe, allora qualcosa non ha funzionato. Controllare con dmesg come è andata dal lato dell'host (confrontate il log di netconsole con quello che vi ho riportato sopra). Se tutto è andato bene dal lato host, allora un possibile errore potrebbe essere per esempio che l'indirizzo IP di destinazione non è corretto.

Se non riuscite a cavarne le gambe, una semplice prova che è trasmettere manualmente con netcat (tipicamente il comando nc) dall'host, e vedere se quello che si trasmette dall'host arriva sul logger. La cosa importante è che, per questa prova:
- facciate usare ai netcat il protocollo UDP per spedire e ricevere, perché poi netconsole utilizza appunto UDP
- facciate spedire i byte dall'host attraverso stessa porta da cui li spedisce netconsole (6665 di default), e verso la stessa porta del logger a cui le spedisce netconsole (6666 se usate la stessa riga di comando riportata in queste note)
Partendo da questa prova, di norma si riesce a trovare cosa non va.


---

# DEBUGGING OOPS

Il punto in cui accade un OOPS è indicato dalla riga
`RIP: indirizzo`

C'è spesso due righe, che riporto il contenuto del registro RIP. Nella prima riga, l'indirizzo è riportato con notazione simbolica: `nome_funzione + offset/lunghezza_funzione`.

Questa notazione è molto utile per ritrovare il punto nel codice in cui si è verificato il problema. Per poter ritrovare tale punto, è necessario che il kernel sia compilato con le cosiddette informazioni di debug. Ossia che nel .config ci sia:
`DEBUG_INFO=yes`

Potete abilitare tale opzione anche attraverso il comando
`make menuconfig`
Trovate l'opzione attraverso il seguente percorso
`Kernel Hacking -> compile-item checks and compiler options -> Compile the kernel with debug info`

A questo punto, per ritrovare la riga di codice in cui accade il fallimento, si può utilizzare gdb, invocandolo nel modo seguente:

`gdb <nome_file_oggetto_o_modulo_contenente_tale_riga>`

Nel caso si tratti di un modulo, può darsi che funzioni solo se passate a gdb il file contenente il modulo, ossia il file .ko.

-> NON PASSATE TUTTO L'ESEGUIBILE DEL KERNEL, perché altrimenti funziona solo se quella riga è relativa ad un componente compilato con built-in.

Per esempio, se il problema è in una funzione definita dentro bfq-iosched.c, allora
`gdb <percorso_build>/block/bfq-iosched.o`

Poi, da dentro gdb:

`list *(nome_funzione+offset)`


---

# ESERCIZIO DA SVOLGERE DOPO AVER STUDIATO IL SYSFS

1. Inserire una eccezione in un punto del nostro modulo hello-world con sysfs. Deve essere un punto tale che si possa controllare da userspace l'istante in cui viene eseguita l'eccezione. Una possibilità è mettere l'eccezione nell'hook di show del file speciale.
2. Scatenare bug e leggere OOPS
3. Scrivere programma in C che apre un file e scatena l'OOPS. Controllare se il file rimane aperto o chiuso. Lo stato in cui va il sistema dovrebbe essere diverso da quello in cui va se si termina il programma violentemente, ma da userspace. Nel secondo caso dovrebbe avvenire una exit_files() e/o altro cleanup.
4. Ripetere, ma con OOPS scatenato dalla macro BUG()
5. Ripetere, ma con OOPS scatenato da handler di timer

Nota sul punto 1
- Se si inserisce l'eccezione nella funzione di init del modulo, allora l'OOPS è di fatto un panic.

Nota sul punto 2
Se provate poi a fare la rmmod del modulo dopo l'OOPS, il kernel si comporta in modo anomalo. La reazione più probabile è che il processo si blocchi indefinitamente. Se poi provate da un'altra shell ad eseguire lsmod, dovrebbe dare -1 come numero di utilizzatori del modulo. A quel punto un ulteriore tentativo di rimozione del modulo fallisce perché il modulo è considerato in uso (perché il contatore non è uguale è zero). Un'altra possibilità è che la prima rmmod funzioni, ma poi un nuovo caricamento del modulo fallisca. In sostanza, il kernel è entrato in uno stato incosistente.

Note su punto 3
- Se il programma scatena l'OOPS tramite una system("qualche comando") allora il programma NON viene ucciso.
- Per verificare quali file sono aperti si può usare il comando lsof
- Per contare quanti file sono aperti si può leggere il contenuto del file speciale /proc/sys/fs/file-nr
- Dovreste scoprire che in ogni caso non rimane il file aperto. Quindi nessuna inconsistenza da questo punto di vista. 

Nota sul punto 5
- Nonostante l'eccezione venga causata nell'handler di un interrupt, il kernel quasi sicuramente sopravvive. A morire è solo il processo che ha la sfortuna di essere in esecuzione sul core su cui viene eseguito l'handler dell'interrupt.

Potete trovare un modulo kernel che causa vari tipi di crash qui:
https://github.com/arighi/crashtest


---

# DETTAGLI E LINK SU KERNEL OOPS (forniti da Chiara Bruschi)

- http://opensourceforu.com/2011/01/understanding-a-kernel-oops/
	-> "depending on the type of error detected by the kernel"
	-> "an "Oops" is what the kernel throws at us when it finds something faulty, or an exception"
	-> "the system may not even resume its normal operations sometimes; this is called unstable state"

- per una spiegazione in intesi, anche se poco dettagliata https://en.wikipedia.org/wiki/Linux_kernel_oops
- differenza tra kernel oops e kernel panic, https://en.wikipedia.org/wiki/Kernel_panic
	->	di solito il processo che ha causato l'errore (seguito dal messaggio di kernel oops) viene killato, ma se le
		risorse necessarie per continuare l'esecuzione (seppur inaffidabile) non sono più disponibili, allora il kernel
		non può più eseguire ed entra in kernel panic
		----------
	->	nel mio caso, la divisione per 0 impedisce l'utilizzo del valor di rate calcolato e l'esecuzione
		non continua, la conseguenza è il freeze della macchina virtuale (dunque accade un kernel panic, non è
		solamente un kernel oops; infatti nel log l'errore è identificato come "divide error:" e non come
		"oops"-come indicato nell'esempio del primo link di questo file- e gli ultimi messaggi del log sono
		proprio "Kernel panic"
		----------
	->	con kernel oops, il sistema continua ad eseguire, ma non è più affidabile; con kernel panic il sistema ha un
		"freeze" e aspetta un comando di reboot
		----------
	->  (dal secondo link di Wikipedia) "kernel panics can also generate another kind of error condition, known as a
		kernel oops. In this case, the kernel normally continues to run after killing the offending process. As an oops
		could cause some subsystems or resources to become unavailable, they can later lead to a full kernel panic."
		
- https://www.safaribooksonline.com/library/view/understanding-the-linux/0596005652/ch04s05.html
	->	"Most exceptions issued by the CPU are interpreted by Linux as error conditions. When one of them occurs, the kernel
		sends a signal to the process that caused the exception to notify it of an anomalous condition. If, for instance,
		a process performs a division by zero, the CPU raises a "Divide error " exception"
	->	"In order to avoid data corruption on the hard disks, the handler invokes the die( ) function, which prints the
		contents of all CPU registers on the console (this dump is called kernel oops) and terminates the current process
		by calling do_exit( )"
		
- https://www.usenix.org/conference/hotdep12/workshop-program/presentation/yoshimura

- https://wiki.ubuntu.com/DebuggingKernelOops

- dalla documentazione ufficiale, http://elixir.free-electrons.com/linux/v4.1/source/Documentation/oops-tracing.txt
G