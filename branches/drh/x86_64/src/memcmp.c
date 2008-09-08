#include <string.h>

/*lint -e613
memcmp is defined here because some vendors don't implement
it, strcmp, or strncmp correctly; they must treat the bytes
as unsigned chars.
*/
int memcmp(const void *s1, const void *s2, size_t n) {
	const unsigned char *cs1 = s1, *cs2 = s2;

	for ( ; n-- > 0; cs1++, cs2++)
		if (*cs1 != *cs2)
			return *cs1 - *cs2;
	return 0;
}
