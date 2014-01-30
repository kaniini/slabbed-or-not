/*
 * slabbed-or-not -- find out if you're being slabbed or not
 *
 * Copyright (c) 2014 William Pitcock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

struct test_impl {
	const char *hv_name;
	bool (*check)(void);
	void (*explain)(void);
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*****************************************************************************************
 * hypervisor checker engines                                                            *
 *****************************************************************************************/
#include "xen-detect.c"
#include "cpuid-detect.c"
#include "vmware-detect.c"

/*****************************************************************************************
 * container checker engines                                                             *
 *****************************************************************************************/
#include "openvz-detect.c"

/*****************************************************************************************
 * testing framework                                                                     *
 *****************************************************************************************/
static struct test_impl *do_tests(struct test_impl **impls, size_t impl_count)
{
	size_t i;

	for (i = 0; i < impl_count; i++)
	{
		if (impls[i]->check())
			return impls[i];
	}

	return NULL;
}

struct test_impl *hv_test_impls[] = {
	&xen_impl,
	&vmware_impl,
	&hyperv_impl,
	&kvm_impl,
	&bhyve_impl,
};

struct test_impl *ctr_test_impls[] = {
	&openvz_impl,
};

int main(int argc, const char *argv[])
{
	struct test_impl *hv, *ctr;

	ctr = do_tests(ctr_test_impls, ARRAY_SIZE(ctr_test_impls));
	if (ctr != NULL)
	{
		if (ctr->explain != NULL)
			ctr->explain();
		else
			printf("Container: %s\n", ctr->hv_name);
	}
	else
		printf("Not running under any known container type\n");

	hv = do_tests(hv_test_impls, ARRAY_SIZE(hv_test_impls));
	if (hv == NULL)
	{
		printf("Not running under any known hypervisor type\n");
		return EXIT_SUCCESS;
	}

	if (hv->explain != NULL)
		hv->explain();
	else
		printf("Hypervisor: %s\n", hv->hv_name);

	return EXIT_FAILURE;
}
