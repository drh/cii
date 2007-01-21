static char rcsid[] = "$Id: H:/drh/idioms/book/RCS/except.doc,v 1.10 1997/02/21 19:43:55 drh Exp $";
#include "assert.h"
const Except_T Assert_Failed = { "Assertion failed" };
void (assert)(int e) {
	assert(e);
}
