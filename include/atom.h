#ifndef ATOM_INCLUDED
#define ATOM_INCLUDED
extern int   Atom_length(const char *str);
extern char *Atom_new   (const char *str, int len);
extern char *Atom_string(const char *str);
extern char *Atom_int   (long n);
#endif
/* $RCSfile: RCS/atom.doc,v $ $Revision: 1.6 $ */
