necessario package rt-tests

**1000 Hz**

top - 20:20:50 up 0 min,  0 users,  load average: 0.00, 0.00, 0.00
Tasks: 131 total, 1 running, 130 sleep, 0 d-sleep, 0 stopped, 0 zombie
top - 20:21:35 up 1 min,  0 users,  load average: 0.00, 0.00, 0.00
Tasks: 131 total, 1 running, 130 sleep, 0 d-sleep, 0 stopped, 0 zombie
%Cpu(s):  0.0 us,  0.0 sy,  0.0 ni,100.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st 
MiB Mem :    970.5 total,    708.2 free,     82.8 used,    199.3 buff/cache     
MiB Swap:      0.0 total,      0.0 free,      0.0 used.    887.7 avail Mem 


T: 0 (  701) P: 0 I:1000 C: 138136 Min:      6 Act:   54 Avg:   55 Max:    2075

- T	    Thread	    Thread index and thread ID
- P	    Priority	RT thread priority
- I	    Interval	Intended wake up period for the latency measuring threads (in us)
- C	    Count	    Number of times the latency was measured i.e. iteration count
- Min	Minimum	    Minimum latency that was measured (in us)
- Act	Actual	    Latency measured during the latest completed iteration (in us)
- Max	Maximum	    Maximum latency that was measured (in us)


**100 Hz**

top - 21:03:10 up 2 min,  0 users,  load average: 0.00, 0.00, 0.00
Tasks: 131 total, 1 running, 130 sleep, 0 d-sleep, 0 stopped, 0 zombie
%Cpu(s):  0.0 us,  0.0 sy,  0.0 ni,100.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st 
MiB Mem :    970.5 total,    657.4 free,     88.6 used,    244.3 buff/cache     
MiB Swap:      0.0 total,      0.0 free,      0.0 used.    881.9 avail Mem 

T: 0 (  332) P: 0 I:1000 C:  53114 Min:     51 Act:   54 Avg:   85 Max:     928


# conclusione:

- se abbasso HZ la latenza aumenta
- non ho notato particolari differenze di uso della cpu
    - 100% idle in entrambi i casi
    - si vede che 1000Hz è comunque troppo poco per notare una differenza
- misurare le latenze è difficile