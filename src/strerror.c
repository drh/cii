#include <string.h>
#include <stdio.h>

/*lint -e613
strerror is defined here because some vendors don't provide it.
*/
char *strerror(int errnum) {
	extern int sys_nerr;
	extern char *sys_errlist[];
	static char null[1] = { 0 };

	if (errnum <= 0)
		return null;
	else if (errnum >= sys_nerr) {
		static char errmsg[50];
		sprintf(errmsg, "error %d", errnum);
		return errmsg;
	} else
		return sys_errlist[errnum];
}
