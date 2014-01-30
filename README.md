# slabbed-or-not

Detect if your container/VPS is running under a specific hypervisor.

Why would you want this?

Some hosting providers run their container hosting services (OpenVZ,
LXC VPS, Docker-as-a-Service, etc) under a parent hypervisor, without
disclosing that practice.  Worse yet, a few providers have been caught
doing this while explicitly claiming not to.

There are also of course, providers which do rightfully disclose their
usage of a parent hypervisor for their services, usually as part of a
high availability scheme.

In essence, the point of this tool is simply to disclose whether or not
your container is running under a hypervisor, and what information can be
collected about the hypervisor from inside the container.

It also detects common container types (i.e. OpenVZ), to clarify to the
user whether or not their VPS environment is inside a container, whether it's
in a hypervisor, or whether both aspects apply (container inside hypervisor).

## usage

Just download the source and go, basically.  You'll need GCC installed, of course.

```shell
kaniini@localhost ~/slabbed-or-not> make
gcc -o slabbed-or-not slabbed-or-not.c
kaniini@localhost ~/slabbed-or-not> ./slabbed-or-not
Container: OpenVZ
Hypervisor: Xen PV
Version: 4.3
```

## what it can detect

* Xen
  * PV or HVM mode
  * Hypervisor version
* VMware
  * Hypervisor type (Workstation, GSX/ESX)
  * Enumeratable virtual devices
  * VMware Tools backdoor protocol version (usually 6)
  * Virtualized hardware model version
* Hyper-V
* KVM
* bhyve
