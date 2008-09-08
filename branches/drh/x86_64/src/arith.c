static char rcsid[] = "$Id$";
#include "arith.h"
int Arith_max(int x, int y) {
	return x > y ? x : y;
}
int Arith_min(int x, int y) {
	return x > y ? y : x;
}
int Arith_div(int x, int y) {
	if (-13/5 == -2
	&&	(x < 0) != (y < 0) && x%y != 0)
		return x/y - 1;
	else
		return x/y;
}
int Arith_mod(int x, int y) {
	if (-13/5 == -2
	&&	(x < 0) != (y < 0) && x%y != 0)
		return x%y + y;
	else
		return x%y;
}
int Arith_floor(int x, int y) {
	return Arith_div(x, y);
}
int Arith_ceiling(int x, int y) {
	return Arith_div(x, y) + (x%y != 0);
}
