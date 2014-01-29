# slabbed-or-not

Detect if your container/VPS is running under a specific hypervisor.

Why would you want this?  Well... a lot of dishonest hosting providers will run their
OpenVZ/LXC containers inside another virtual machine.

This tool is meant to expose such subterfuge.  In a lot of cases, they will attempt to
hide it, by either obfuscating cpuinfo or other sloppyness.

## usage

Just download the source and go, basically.  You'll need GCC installed, of course.

```kaniini@localhost ~/slabbed-or-not> make
gcc -o slabbed-or-not slabbed-or-not.c
kaniini@localhost ~/slabbed-or-not> ./slabbed-or-not
Hypervisor: Xen PV
Version: 4.3
```

## what it can detect

* Xen
** PV or HVM mode
** Hypervisor version
* VMware
* Hyper-V
* KVM
* bhyve

## what it's not

This is not a substitute for `virt-what`.  `virt-what` is supposed to detect what
you've been sold, not what they are actually providing.  So, it should rightfully report
an OpenVZ or LXC container as such.  Even if it's running under a hypervisor.

