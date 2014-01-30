static bool openvz_check(void)
{
	if (!access("/proc/vz/vzaquota", R_OK))
		return true;

	if (!access("/proc/user_beancounters", R_OK))
		return true;

	return false;
}

static struct test_impl openvz_impl = {
	.hv_name = "OpenVZ",
	.check = openvz_check,
};
