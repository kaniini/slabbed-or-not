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

static struct test_impl vmware_impl = {
	.hv_name = "VMware",
	.check = vmware_check,
};

static bool hyperv_check(void)
{
	return cpuid_scan("Microsoft Hv");
}

static struct test_impl hyperv_impl = {
	.hv_name = "Microsoft Hyper-V",
	.check = hyperv_check,
};

static bool kvm_check(void)
{
	return cpuid_scan("KVMKVMKVM");
}

static struct test_impl kvm_impl = {
	.hv_name = "KVM",
	.check = kvm_check,
};

static bool bhyve_check(void)
{
	return cpuid_scan("bhyve bhyve ");
}

static struct test_impl bhyve_impl = {
	.hv_name = "bhyve",
	.check = bhyve_check,
};
