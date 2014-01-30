#define VMWARE_HYPERVISOR_MAGIC 0x564D5868
#define VMWARE_HYPERVISOR_PORT  0x5658

#define VMWARE_PORT_CMD_GETVERSION      10

#define VMWARE_PORT(cmd, eax, ebx, ecx, edx)                            \
        __asm__("inl (%%dx)" :                                          \
                        "=a"(eax), "=c"(ecx), "=d"(edx), "=b"(ebx) :    \
                        "0"(VMWARE_HYPERVISOR_MAGIC),                   \
                        "1"(VMWARE_PORT_CMD_##cmd),                     \
                        "2"(VMWARE_HYPERVISOR_PORT), "3"(UINT_MAX) :    \
                        "memory");

int hypervisor_port_check(void)
{
	uint32_t eax, ebx, ecx, edx;

	VMWARE_PORT(GETVERSION, eax, ebx, ecx, edx);
	if (ebx == VMWARE_HYPERVISOR_MAGIC)
		return true;

	return false;
}

static bool vmware_check(void)
{
	if (!cpuid_scan("VMwareVMware"))
		return false;

	return hypervisor_port_check();
}

static struct test_impl vmware_impl = {
	.hv_name = "VMware",
	.check = vmware_check,
};
