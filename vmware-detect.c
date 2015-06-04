#define VMWARE_HYPERVISOR_MAGIC 0x564D5868
#define VMWARE_HYPERVISOR_PORT  0x5658

#define VMWARE_PORT_CMD_GETVERSION      10
#define VMWARE_PORT_CMD_GETDEVICE	11
#define VMWARE_PORT_CMD_GETHWMODELVER	17

#define VMWARE_MAX_DEVICES		50

#define VMWARE_PORT(cmd, eax, ebx, ecx, edx)                            \
        __asm__("inl %%dx, %%eax" :                                     \
                        "=a"(eax), "=c"(ecx), "=d"(edx), "=b"(ebx) :    \
                        "0"(VMWARE_HYPERVISOR_MAGIC),                   \
                        "1"(VMWARE_PORT_CMD_##cmd),                     \
                        "2"(VMWARE_HYPERVISOR_PORT), "3"(ebx) :    \
                        "memory");

static bool vmware_hypervisor_port_check(void)
{
	uint32_t eax, ebx, ecx, edx;

	ebx = UINT_MAX;
	VMWARE_PORT(GETVERSION, eax, ebx, ecx, edx);
	if (ebx == VMWARE_HYPERVISOR_MAGIC)
		return true;

	return false;
}

static bool vmware_check(void)
{
	if (!cpuid_scan("VMwareVMware"))
		return false;

	return vmware_hypervisor_port_check();
}

struct vmware_device {
	char devname[32];
	uint32_t devid;
	bool enabled;
	char pad[3];
};

static bool vmware_read_device_chunk(uint16_t id, uint16_t offset, uint32_t *data)
{
	uint32_t eax, ebx, ecx, edx;

	ebx = (id << 16) | offset;
	VMWARE_PORT(GETDEVICE, eax, ebx, ecx, edx);
	if (eax == 0)
		return false;

	*data = ebx;
	return true;
}

static bool vmware_read_device(uint16_t id, struct vmware_device *dev)
{
	uint16_t step;
	uint32_t *p;

	for (step = 0, p = (uint32_t *) dev; step < sizeof(struct vmware_device); step += sizeof(uint32_t), p++)
		if (!vmware_read_device_chunk(id, step, p))
			return false;

	return true;
}

static void vmware_enum_devices(void)
{
	uint16_t i;

	for (i = 0; i < VMWARE_MAX_DEVICES; i++)
	{
		struct vmware_device dev = { };

		if (!vmware_read_device(i, &dev))
			continue;

		if (*dev.devname == '\0')
			continue;

		printf("Device %d: %s (%s)\n", i, dev.devname, dev.enabled ? "enabled" : "disabled");
	}
}

static const char *vmware_types[] = {
	"Unknown / Not Set",
	"???",
	"ESX/ESXi Server",
	"???",
	"Workstation",
};

static void vmware_explain(void)
{
	uint32_t eax, ebx, ecx, edx;

	VMWARE_PORT(GETVERSION, eax, ebx, ecx, edx);
	if (ebx != VMWARE_HYPERVISOR_MAGIC)
		abort();

	printf("Hypervisor: VMware (protocol %d)\n", eax);
	if (ecx != 0)
		printf("Product: %s\n", vmware_types[ecx]);

	vmware_enum_devices();

	VMWARE_PORT(GETHWMODELVER, eax, ebx, ecx, edx);
	if (ebx != VMWARE_HYPERVISOR_MAGIC)
		return;

	if (eax != -1)
		printf("Hardware Model: %d\n", eax);
}

static struct test_impl vmware_impl = {
	.hv_name = "VMware",
	.check = vmware_check,
	.explain = vmware_explain,
};
