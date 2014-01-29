/*
 * Simple tests for detecting based on CPUID.  Not detailed information by any
 * means.
 */
static uint32_t cpuid_base = 0x40000000;

static bool cpuid_scan(char *sigbase)
{
	uint32_t eax, ebx, ecx, edx;
	char signature[13];

	for (cpuid_base = 0x40000000; cpuid_base < 0x40010000; cpuid_base += 0x100)
	{
		xen_cpuid(cpuid_base, &eax, &ebx, &ecx, &edx);
		*(uint32_t *)(signature + 0) = ebx;
		*(uint32_t *)(signature + 4) = ecx;
		*(uint32_t *)(signature + 8) = edx;
		signature[12] = '\0';

		if (!strcmp(sigbase, signature))
			return true;
	}

	return false;
}

static bool vmware_check(void)
{
	return cpuid_scan("VMwareVMware");
}

static void vmware_explain(void)
{
	printf("Hypervisor: VMware\n");
}

static struct test_impl vmware_impl = {
	.hv_name = "VMware",
	.check = vmware_check,
	.explain = vmware_explain,
};

static bool hyperv_check(void)
{
	return cpuid_scan("Microsoft Hv");
}

static void hyperv_explain(void)
{
	printf("Hypervisor: Microsoft Hyper-V\n");
}

static struct test_impl hyperv_impl = {
	.hv_name = "Hyper-V",
	.check = hyperv_check,
	.explain = hyperv_explain,
};

static bool kvm_check(void)
{
	return cpuid_scan("KVMKVMKVM");
}

static void kvm_explain(void)
{
	printf("Hypervisor: KVM\n");
}

static struct test_impl kvm_impl = {
	.hv_name = "KVM",
	.check = kvm_check,
	.explain = kvm_explain,
};

