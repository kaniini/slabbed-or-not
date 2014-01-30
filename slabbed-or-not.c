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

struct test_impl {
	const char *hv_name;
	bool (*check)(void);
	void (*explain)(void);
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include "xen-detect.c"
#include "cpuid-detect.c"

/*****************************************************************************************
 * testing framework                                                                     *
 *****************************************************************************************/
struct test_impl *test_impls[] = {
	&xen_impl,
	&vmware_impl,
	&hyperv_impl,
	&kvm_impl,
	&bhyve_impl,
};

static struct test_impl *do_checks(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(test_impls); i++)
	{
		if (test_impls[i]->check())
			return test_impls[i];
	}

	return NULL;
}

int main(int argc, const char *argv[])
{
	struct test_impl *impl;

	impl = do_checks();
	if (impl == NULL)
	{
		printf("Not running under any known hypervisor type\n");
		return EXIT_SUCCESS;
	}

	if (impl->explain != NULL)
		impl->explain();
	else
		printf("Hypervisor: %s\n", impl->hv_name);

	return EXIT_FAILURE;
}
