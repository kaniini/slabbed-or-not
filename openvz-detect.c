static bool openvz_check(void)
{
	if (!access("/proc/vz/aquota", R_OK))
		return true;

	return false;
}

static struct test_impl openvz_impl = {
	.hv_name = "OpenVZ",
	.check = openvz_check,
};
