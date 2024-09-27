# Report for assignment 1
## Part 1. Login into Dandelion
This part gave only one issue, which was the permissions. By looking online, we were able to find out that we needed to change key file permissions so only the user had permissions for reading and writing, which was done via `chmod 600`.
```bash
Linux dandelion 5.10.0-28-amd64 #1 SMP Debian 5.10.209-2 (2024-01-31) x86_64

WELCOME TO COMPUTER SCIENCE

This system is for the use of authorized users only. Individuals using

this computer system without authority, or in excess of their

authority, are subject to having all of their activities on this system

monitored and recorded by system personnel.

In the course of monitoring individuals improperly using this system,

or in the course of system maintenance, the activities of authorized

users may also be monitored.

Anyone using this system expressly consents to such monitoring and is

advised that if such monitoring reveals possible evidence of criminal

activity, system personnel may provide the evidence of such monitoring

to law enforcement officials.

Check http://utep.edu/cs for information on the CS computer labs.

The programs included with the Debian GNU/Linux system are free software;

the exact distribution terms for each program are described in the

individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent

permitted by applicable law.

Last login: Mon Sep 16 10:45:56 2024 from 172.20.41.238
```

## Part 2. Getting to know Bash
We had to search for some commands, but no issues where encountered. Here are our results:
• **how many users are logged in concurrently with you,**
```bash
rgarcia117@dandelion:~$ who | wc -l
3
```
• **how long the system is already running since last rebooted,**
```bash
rgarcia117@dandelion:~$ uptime
10:59:45 up 143 days, 23:48, 3 users, load average: 0.00, 0.00, 0.00
```

• **what processors it has,**
```bash
Architecture: x86_64
CPU op-mode(s): 32-bit, 64-bit
Byte Order: Little Endian
Address sizes: 43 bits physical, 48 bits virtual
CPU(s): 8
On-line CPU(s) list: 0-7
Thread(s) per core: 1
Core(s) per socket: 8
Socket(s): 1
NUMA node(s): 4
Vendor ID: AuthenticAMD
CPU family: 23
Model: 49
Model name: AMD EPYC 7232P 8-Core Processor
Stepping: 0
CPU MHz: 3193.420
BogoMIPS: 6188.37
Virtualization: AMD-V
L1d cache: 256 KiB
L1i cache: 256 KiB
L2 cache: 4 MiB
L3 cache: 32 MiB
NUMA node0 CPU(s): 0,1
NUMA node1 CPU(s): 2,3
NUMA node2 CPU(s): 4,5
NUMA node3 CPU(s): 6,7
Vulnerability Gather data sampling: Not affected
Vulnerability Itlb multihit: Not affected
Vulnerability L1tf: Not affected
Vulnerability Mds: Not affected
Vulnerability Meltdown: Not affected
Vulnerability Mmio stale data: Not affected
Vulnerability Retbleed: Mitigation; untrained return thunk; SMT disa
bled
Vulnerability Spec rstack overflow: Mitigation; SMT disabled
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disable
d via prctl and seccomp
Vulnerability Spectre v1: Mitigation; usercopy/swapgs barriers and __u
ser pointer sanitization
Vulnerability Spectre v2: Mitigation; Retpolines, IBPB conditional, ST
IBP disabled, RSB filling, PBRSB-eIBRS Not a
ffected
Vulnerability Srbds: Not affected
Vulnerability Tsx async abort: Not affected
Flags: fpu vme de pse tsc msr pae mce cx8 apic sep
mtrr pge mca cmov pat pse36 clflush mmx fxsr
sse sse2 ht syscall nx mmxext fxsr_opt pdpe
1gb rdtscp lm constant_tsc rep_good nopl non
stop_tsc cpuid extd_apicid aperfmperf pni pc
lmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2
x2apic movbe popcnt aes xsave avx f16c rdra
nd lahf_lm cmp_legacy svm extapic cr8_legacy
abm sse4a misalignsse 3dnowprefetch osvw ib
s skinit wdt tce topoext perfctr_core perfct
r_nb bpext perfctr_llc mwaitx cpb cat_l3 cdp
_l3 hw_pstate ssbd mba ibrs ibpb stibp vmmca
ll fsgsbase bmi1 avx2 smep bmi2 cqm rdt_a rd
seed adx smap clflushopt clwb sha_ni xsaveop
t xsavec xgetbv1 cqm_llc cqm_occup_llc cqm_m
bm_total cqm_mbm_local clzero irperf xsaveer
ptr rdpru wbnoinvd amd_ppin arat npt lbrv sv
m_lock nrip_save tsc_scale vmcb_clean flushb
yasid decodeassists pausefilter pfthreshold
avic v_vmsave_vmload vgif umip rdpid overflo
w_recov succor smca sme sev sev_es
```
• **how much RAM is available.**
```bash
rgarcia117@dandelion:~$ free -h
total used free shared buff/cache available

Mem: 31Gi 6.2Gi 3.6Gi 3.0Mi 21Gi 24Gi
Swap: 555Gi 1.6Gi 553Gi
```
- **We used less to explore the nanpa file**
We explored the nanpa file using less while finding some entries:
![[OS Report 1_P2_1.png]]
![[OS Report 1_P2_2.png]]
![[OS Report 1_P2_3.png]]
![[OS Report 1_P2_4.png]]
![[OS Report 1_P2_5.png]]![[OS Report 1_P2_6.png]]
![[OS Report 1_P2_7.png]]
- **Find out how many lines connecting prefixes to locations are contained in the file nanpa. Which Linux command line tool do you use to count lines?**
```bash
(base) pills@rpc ~ $ curl https://www.christoph-lauter.org/utep-os/nanpa | wc -l
% Total % Received % Xferd Average Speed Time Time Time Current
Dload Upload Total Spent Left Speed
100 5202k 100 5202k 0 0 3521k 0 0:00:01 0:00:01 --:--:-- 3522k
166482
```
- ** List the first 17 lines of the nanpa file on command line. Also list the last 42 lines of the file. You can use the Linux tools head and tail for this task.**
```bash
(base) pills@rpc ~ $ curl -s https://www.christoph-lauter.org/utep-os/nanpa | head -n 17
201200Jersey City NJ
201202Hackensack NJ
201203Hackensack NJ
201204Jersey City NJ
201205Jersey City NJ
201206Hackensack NJ
201207Newark NJ
201208Jersey City NJ
201209Jersey City NJ
201210Union City NJ
201212Hackensack NJ
201213Morristown NJ
201214Hackensack NJ
201215Bayonne NJ
201216Jersey City NJ
201217Jersey City NJ
201218Hackensack NJ

(base) pills@rpc ~ $ curl -s https://www.christoph-lauter.org/utep-os/nanpa | tail -n 42
989921Saginaw MI
989922Bay City MI
989923Midland MI
989924Alma MI
989925Manistee Ri MI
989926Midland MI
989928Saginaw MI
989929Bay City MI
989932Durand MI
989934Rose City MI
989935Clare MI
989936Owosso MI
989938Grace Harbo MI
989939Chester MI
989941Midland MI
989942West Branch MI
989943Middleton MI
989944Mount Pleas MI
989945Ovid MI
989946Standish MI
989948Midland MI
989949McBrides MI
989953Mount Pleas MI
989954Mount Pleas MI
989956Mount Pleas MI
989962Minden City MI
989963Elkton MI
989964Saginaw MI
989965Gladwin MI
989966Vanderbilt MI
989967Remus MI
989968Alma MI
989971Saginaw MI
989975Bad Axe MI
989977Sebewaing MI
989979St Johns MI
989980Saginaw MI
989981Hubbardston MI
989983Vanderbilt MI
989984East Tawas MI
989992Saginaw MI
989996Saginaw MI
```