/* $Id: H:/drh/idioms/book/RCS/atom.doc,v 1.10 1997/02/21 19:42:46 drh Exp $ */
#ifndef ATOM_INCLUDED
#define ATOM_INCLUDED
extern       int   Atom_length(const char *str);
extern const char *Atom_new   (const char *str, int len);
extern const char *Atom_string(const char *str);
extern const char *Atom_int   (long n);
#endif
