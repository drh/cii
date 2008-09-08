static const char *rcsid = "$Id$";
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "mem.h"
#include "seq.h"
#include "fmt.h"
#include "mp.h"
Seq_T sp;
int ibase = 10;
int obase = 10;
struct {
	const char *fmt;
	MP_T (*add)(MP_T, MP_T, MP_T);
	MP_T (*sub)(MP_T, MP_T, MP_T);
	MP_T (*mul)(MP_T, MP_T, MP_T);
	MP_T (*div)(MP_T, MP_T, MP_T);
	MP_T (*mod)(MP_T, MP_T, MP_T);
} s = { "%D\n",
	MP_add,  MP_sub,  MP_mul,  MP_div,  MP_mod  },
  u = { "%U\n",
	MP_addu, MP_subu, MP_mulu, MP_divu, MP_modu },
 *f = &s;
MP_T pop(void) {
	if (Seq_length(sp) > 0)
		return Seq_remhi(sp);
	else {
		Fmt_fprint(stderr, "?stack underflow\n");
		return MP_new(0);
	}
}
int main(int argc, char *argv[]) {
	int c;
	sp = Seq_new(0);
	Fmt_register('D', MP_fmt);
	Fmt_register('U', MP_fmtu);
	while ((c = getchar()) != EOF) {
		volatile MP_T x = NULL, y = NULL, z = NULL;
		TRY
 			switch (c) {
			default:
				if (isprint(c))
					Fmt_fprint(stderr, "?'%c'", c);
				else
					Fmt_fprint(stderr, "?'\\%03o'", c);
				Fmt_fprint(stderr, " is unimplemented\n");
				break;
			case ' ': case '\t': case '\n': case '\f': case '\r':
				break;
			case 'c': while (Seq_length(sp) > 0) {
				  	MP_T x = Seq_remhi(sp);
				  	FREE(x);
				  } break;
			case 'q': while (Seq_length(sp) > 0) {
				  	MP_T x = Seq_remhi(sp);
				  	FREE(x);
				  }
				  Seq_free(&sp);
				  return EXIT_SUCCESS;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': {
				char buf[512];
				z = MP_new(0);
				{
					int i = 0;
					for ( ;  strchr(&"zyxwvutsrqponmlkjihgfedcba9876543210"[36-ibase],
							tolower(c)); c = getchar(), i++)
						if (i < (int)sizeof (buf) - 1)
							buf[i] = c;
					if (i > (int)sizeof (buf) - 1) {
						i = (int)sizeof (buf) - 1;
						Fmt_fprint(stderr,
							"?integer constant exceeds %d digits\n", i);
					}
					buf[i] = '\0';
					if (c != EOF)
						ungetc(c, stdin);
				}
				MP_fromstr(z, buf, ibase, NULL);
				break;
			}
			case '+': y = pop(); x = pop();
				  z = MP_new(0); (*f->add)(z, x, y); break;
			case '-': y = pop(); x = pop();
				  z = MP_new(0); (*f->sub)(z, x, y); break;
			case '*': y = pop(); x = pop();
				  z = MP_new(0); (*f->mul)(z, x, y); break;
			case '/': y = pop(); x = pop();
				  z = MP_new(0); (*f->div)(z, x, y); break;
			case '%': y = pop(); x = pop();
				  z = MP_new(0); (*f->mod)(z, x, y); break;
			case '&': y = pop(); x = pop();
				  z = MP_new(0);    MP_and(z, x, y); break;
			case '|': y = pop(); x = pop();
				  z = MP_new(0);    MP_or (z, x, y); break;
			case '^': y = pop(); x = pop();
				  z = MP_new(0);    MP_xor(z, x, y); break;
			case '!': z = pop(); MP_not(z, z); break;
			case '~': z = pop(); MP_neg(z, z); break;
			case 'i': case 'o': {
				long n;
				x = pop();
				n = MP_toint(x);
				if (n < 2 || n > 36)
					Fmt_fprint(stderr, "?%d is an illegal base\n",n);
				else if (c == 'i')
					ibase = n;
				else
					obase = n;
				if (obase == 2 || obase == 8 || obase == 16)
					f = &u;
				else
					f = &s;
				break;
				}
			case 'p':
				Fmt_print(f->fmt, z = pop(), obase);
				break;
			case 'f': {
				int n = Seq_length(sp);
				while (--n >= 0)
					Fmt_print(f->fmt, Seq_get(sp, n), obase);
				break;
			}
			case '<': { long s;
				    y = pop();
				    z = pop();
				    s = MP_toint(y);
				    if (s < 0 || s > INT_MAX) {
				    	Fmt_fprint(stderr,
				    		"?%d is an illegal shift amount\n", s);
				    	break;
				    }; MP_lshift(z, z, s); break; }
			case '>': { long s;
				    y = pop();
				    z = pop();
				    s = MP_toint(y);
				    if (s < 0 || s > INT_MAX) {
				    	Fmt_fprint(stderr,
				    		"?%d is an illegal shift amount\n", s);
				    	break;
				    }; MP_rshift(z, z, s); break; }
			case 'k': {
				long n;
				x = pop();
				n = MP_toint(x);
				if (n < 2 || n > INT_MAX)
					Fmt_fprint(stderr,
						"?%d is an illegal precision\n", n);
				else if (Seq_length(sp) > 0)
					Fmt_fprint(stderr, "?nonempty stack\n");
				else
					MP_set(n);
				break;
				}
			case 'd': {
				MP_T x = pop();
				z = MP_new(0);
				Seq_addhi(sp, x);
				MP_addui(z, x, 0);
				break;
				}
			}
		EXCEPT(MP_Overflow)
			Fmt_fprint(stderr, "?overflow\n");
		EXCEPT(MP_Dividebyzero)
			Fmt_fprint(stderr, "?divide by 0\n");
		END_TRY;
		if (z)
			Seq_addhi(sp, z);
		FREE(x);
		FREE(y);
	}
	while (Seq_length(sp) > 0) {
		MP_T x = Seq_remhi(sp);
		FREE(x);
	}
	Seq_free(&sp);
	return EXIT_SUCCESS;
}
