#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "except.h"
extern void assert(int e);
extern const Except_T Assert_Failed;
#define assert(e) ((void)((e) || (RAISE(Assert_Failed),0)))
#endif
/* $RCSfile: RCS/except.doc,v $ $Revision: 1.2 $ */
