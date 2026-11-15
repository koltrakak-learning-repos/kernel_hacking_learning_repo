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