static char rcsid[] = "$Id$";
#include "assert.h"
const Assert_Failed = { "Assertion failed" };
void (assert)(int e) {
	assert(e);
}
