static char rcsid[] = "$Id$";
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"
#include "ap.h"
#include "fmt.h"
Stack_T sp;
AP_T pop(void) {
	if (!Stack_empty(sp))
		return Stack_pop(sp);
	else {
		Fmt_fprint(stderr, "?stack underflow\n");
		return AP_new(0);
	}
}
int main(int argc, char *argv[]) {
	int c;
	sp = Stack_new();
	Fmt_register('D', AP_fmt);
	while ((c = getchar()) != EOF)
		switch (c) {
		case ' ': case '\t': case '\n': case '\f': case '\r':
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			char buf[512];
			{
				int i = 0;
				for ( ; c != EOF && isdigit(c); c = getchar(), i++)
					if (i < (int)sizeof (buf) - 1)
						buf[i] = c;
				if (i > (int)sizeof (buf) - 1) {
					i = (int)sizeof (buf) - 1;
					Fmt_fprint(stderr,
						"?integer constant exceeds %d digits\n", i);
				}
				buf[i] = 0;
				if (c != EOF)
					ungetc(c, stdin);
			}
			Stack_push(sp, AP_fromstr(buf, 10, NULL));
			break;
		}
		case '+': {
			AP_T y = pop(), x = pop();
			Stack_push(sp, AP_add(x, y));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case '-': {
			AP_T y = pop(), x = pop();
			Stack_push(sp, AP_sub(x, y));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case '*': {
			AP_T y = pop(), x = pop();
			Stack_push(sp, AP_mul(x, y));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case '/': {
			AP_T y = pop(), x = pop();
			if (AP_cmpi(y, 0) == 0) {
				Fmt_fprint(stderr, "?/ by 0\n");
				Stack_push(sp, AP_new(0));
			} else
				Stack_push(sp, AP_div(x, y));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case '%': {
			AP_T y = pop(), x = pop();
			if (AP_cmpi(y, 0) == 0) {
				Fmt_fprint(stderr, "?%% by 0\n");
				Stack_push(sp, AP_new(0));
			} else
				Stack_push(sp, AP_mod(x, y));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case '^': {
			AP_T y = pop(), x = pop();
			if (AP_cmpi(y, 0) <= 0) {
				Fmt_fprint(stderr, "?nonpositive power\n");
				Stack_push(sp, AP_new(0));
			} else
				Stack_push(sp, AP_pow(x, y, NULL));
			AP_free(&x);
			AP_free(&y);
			break;
		}
		case 'd': {
			AP_T x = pop();
			Stack_push(sp, x);
			Stack_push(sp, AP_addi(x, 0));
			break;
		}
		case 'p': {
			AP_T x = pop();
			Fmt_print("%D\n", x);
			Stack_push(sp, x);
			break;
		}
		case 'f':
			if (!Stack_empty(sp)) {
				Stack_T tmp = Stack_new();
				while (!Stack_empty(sp)) {
					AP_T x = pop();
					Fmt_print("%D\n", x);
					Stack_push(tmp, x);
				}
				while (!Stack_empty(tmp))
					Stack_push(sp, Stack_pop(tmp));
				Stack_free(&tmp);
			}
			break;
		case '~': {
			AP_T x = pop();
			Stack_push(sp, AP_neg(x));
			AP_free(&x);
			break;
		}
		case 'c': while (!Stack_empty(sp)) {
			  	AP_T x = Stack_pop(sp);
			  	AP_free(&x);
			  } break;
		case 'q': while (!Stack_empty(sp)) {
			  	AP_T x = Stack_pop(sp);
			  	AP_free(&x);
			  }
			  Stack_free(&sp);
			  return EXIT_SUCCESS;
		default:
			if (isprint(c))
				Fmt_fprint(stderr, "?'%c'", c);
			else
				Fmt_fprint(stderr, "?'\\%03o'", c);
			Fmt_fprint(stderr, " is unimplemented\n");
			break;
		}
	while (!Stack_empty(sp)) {
		AP_T x = Stack_pop(sp);
		AP_free(&x);
	}
	Stack_free(&sp);
	return EXIT_SUCCESS;
}
