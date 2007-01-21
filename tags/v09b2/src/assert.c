#include "assert.h"
const Except_T Assert_Failed = { "Assertion failed" };
void (assert)(int e) {
	assert(e);
}
static char rcsid[] = "$RCSfile: RCS/except.doc,v $ $Revision: 1.2 $";
