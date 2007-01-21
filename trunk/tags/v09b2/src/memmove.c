#include <string.h>

/* memmove is defined here because some vendors don't provide it at
   all and others do a terrible job (like calling malloc) */
void * memmove(void *dp, const void *sp, size_t n) {
	unsigned char *cdp;
	const unsigned char *csp;

	if (n<=0)
		return 0;
	cdp = dp;
	csp = sp;
	if (cdp < csp) {
		do {
			*cdp++ = *csp++;
		} while (--n);
	} else {
		cdp += n;
		csp += n;
		do {
			*--cdp = *--csp;
		} while (--n);
	}
	return 0;
}
