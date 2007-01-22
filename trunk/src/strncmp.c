#include <string.h>

/*lint -e613
strncmp is defined here because some vendors don't implement
it, strcmp, or memcmp correctly; they must treat the bytes
as unsigned chars.
*/
int strncmp(const char *s1, const char *s2, size_t n) {
	for ( ; n-- > 0; s1++, s2++)
		if (*s1 != *s2)
			return (unsigned char)*s1 - (unsigned char)*s2;
		else if (*s1 == 0)
			return 0;
	return 0;
}
