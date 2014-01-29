/******************************************************************************
 * xen-detect.c
 *
 * Simple GNU C / POSIX application to detect execution on Xen VMM platform.
 *
 * Copyright (c) 2007, XenSource Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

static int xen_pv_context;
static uint32_t xen_base = 0x40000000;

static jmp_buf xen_sigill_jmp;
static void xen_sigill_handler(int sig)
{
        longjmp(xen_sigill_jmp, 1);
}

static void xen_cpuid(uint32_t idx,
                  uint32_t *eax,
                  uint32_t *ebx,
                  uint32_t *ecx,
                  uint32_t *edx)
{
	asm volatile (
        	"test %1,%1 ; jz 1f ; ud2a ; .ascii \"xen\" ; 1: cpuid"
	        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        	: "0" (idx), "1" (xen_pv_context) );
}

static bool xen_check_inner(void)
{
	uint32_t eax, ebx, ecx, edx;
	char signature[13];

	for (xen_base = 0x40000000; xen_base < 0x40010000; xen_base += 0x100)
	{
		xen_cpuid(xen_base, &eax, &ebx, &ecx, &edx);
		*(uint32_t *)(signature + 0) = ebx;
		*(uint32_t *)(signature + 4) = ecx;
		*(uint32_t *)(signature + 8) = edx;
		signature[12] = '\0';

		if (!strcmp("XenVMMXenVMM", signature) && eax >= (xen_base + 2))
			return true;
	}

	return false;
}

static void xen_explain(void)
{
	uint32_t eax, ebx, ecx, edx;

	xen_cpuid(xen_base + 1, &eax, &ebx, &ecx, &edx);
	printf("Hypervisor: Xen %s\n", xen_pv_context ? "PV" : "HVM");
	printf("Version: %d.%d\n", (uint16_t)(eax >> 16), (uint16_t)eax);
}

static bool xen_check_outer(void)
{
	struct sigaction act, oldact;

	/* first check for HVM... */
	xen_pv_context = 0;
	if (xen_check_inner())
		return true;

	/* not HVM... catch SIGILL in case we're running outside Xen */
	if (setjmp(xen_sigill_jmp))
		return false;

	memset(&act, 0, sizeof act);
	act.sa_handler = xen_sigill_handler;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGILL, &act, &oldact))
		return false;

	/* actually do the check now */
	xen_pv_context = 1;
	if (xen_check_inner())
		return true;

	/* restore old SIGILL handler */
	sigaction(SIGILL, &oldact, NULL);
}

static struct test_impl xen_impl = {
	.hv_name = "Xen",
	.check = xen_check_outer,
	.explain = xen_explain,
};
