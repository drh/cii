# $Id$
MAJORVERSION=2
A=.a
O=.o
SO=.so
E=
CC=cc
I=include
CFLAGS=-g -I$I
LDFLAGS=-g
LD=cc
AS=as
AR=ar ruv
RANLIB=ranlib
DIFF=diff
RM=rm -f
CUSTOM=custom.mk
EXTRAS=$(BUILDDIR)memcmp$O $(BUILDDIR)memmove$O $(BUILDDIR)strncmp$O
THREADS=$(BUILDDIR)thread$O $(BUILDDIR)swtch$O $(BUILDDIR)chan$O
include $(CUSTOM)
B=$(BUILDDIR)

OBJS=	$Bap$O \
	$Barena$O \
	$Barith$O \
	$Barray$O \
	$Bassert$O \
	$Batom$O \
	$Bbit$O \
	$Bexcept$O \
	$Bfmt$O \
	$Blist$O \
	$Bmem$O \
	$Bmp$O \
	$Bring$O \
	$Bseq$O \
	$Bset$O \
	$Bstack$O \
	$Bstr$O \
	$Btable$O \
	$Btext$O \
	$Bxp$O \
	$(THREADS)

EXAMPLES=	$Bdouble$E \
		$Bcalc$E \
		$Bids$E \
		$Bmpcalc$E \
		$Bwf$E \
		$Bxref$E \
		$Bcref$E \
		$Biref$E \
		$Bkref$E \
		$Bidents$E \
		$Bwords$E \
		$Bbasename$E \
		$Bdirname$E \
		$Bsort$E \
		$Bspin$E \
		$Bsieve$E

all::		$Blibcii$A $(EXAMPLES) $Bmemchk$O

$Blibcii$A::	$(OBJS) $(EXTRAS)
		$(AR) $@ $(OBJS) $(EXTRAS); $(RANLIB) $@ || true

# Linux-specific rule for building a shared library
$Blibcii$(SO).$(MAJORVERSION): $(OBJS) $(EXTRAS)
		$(CC) -shared -Wl,-soname,libcii$(SO).$(MAJORVERSION) -o $@ $(OBJS) $(EXTRAS)

$Bap$O:		src/ap.c;	$(CC) $(CFLAGS) -c -o $@ src/ap.c
$Barena$O:	src/arena.c;	$(CC) $(CFLAGS) -c -o $@ src/arena.c
$Barith$O:	src/arith.c;	$(CC) $(CFLAGS) -c -o $@ src/arith.c
$Barray$O:	src/array.c;	$(CC) $(CFLAGS) -c -o $@ src/array.c
$Bassert$O:	src/assert.c;	$(CC) $(CFLAGS) -c -o $@ src/assert.c
$Batom$O:	src/atom.c;	$(CC) $(CFLAGS) -c -o $@ src/atom.c
$Bbit$O:	src/bit.c;	$(CC) $(CFLAGS) -c -o $@ src/bit.c
$Bchan$O:	src/chan.c;	$(CC) $(CFLAGS) -c -o $@ src/chan.c
$Bexcept$O:	src/except.c;	$(CC) $(CFLAGS) -c -o $@ src/except.c
$Bfmt$O:	src/fmt.c;	$(CC) $(CFLAGS) -c -o $@ src/fmt.c
$Blist$O:	src/list.c;	$(CC) $(CFLAGS) -c -o $@ src/list.c
$Bmem$O:	src/mem.c;	$(CC) $(CFLAGS) -c -o $@ src/mem.c
$Bmemchk$O:	src/memchk.c;	$(CC) $(CFLAGS) -c -o $@ src/memchk.c
$Bmp$O:		src/mp.c;	$(CC) $(CFLAGS) -c -o $@ src/mp.c
$Bring$O:	src/ring.c;	$(CC) $(CFLAGS) -c -o $@ src/ring.c
$Bseq$O:	src/seq.c;	$(CC) $(CFLAGS) -c -o $@ src/seq.c
$Bset$O:	src/set.c;	$(CC) $(CFLAGS) -c -o $@ src/set.c
$Bstack$O:	src/stack.c;	$(CC) $(CFLAGS) -c -o $@ src/stack.c
$Bstr$O:	src/str.c;	$(CC) $(CFLAGS) -c -o $@ src/str.c
$Btable$O:	src/table.c;	$(CC) $(CFLAGS) -c -o $@ src/table.c
$Btext$O:	src/text.c;	$(CC) $(CFLAGS) -c -o $@ src/text.c
$Bthread$O:	src/thread.c;	$(CC) $(CFLAGS) -c -o $@ src/thread.c
$Bthread-nt$O:	src/thread-nt.c;$(CC) $(CFLAGS) -c -o $@ src/thread-nt.c
$Bxp$O:		src/xp.c;	$(CC) $(CFLAGS) -c -o $@ src/xp.c
$Bswtch$O:	src/swtch.s;	$(AS) -o $@  src/swtch.s

$Bmemcmp$O:	src/memcmp.c;	$(CC) $(CFLAGS) -c -o $@ src/memcmp.c
$Bmemmove$O:	src/memmove.c;	$(CC) $(CFLAGS) -c -o $@ src/memmove.c
$Bstrncmp$O:	src/strncmp.c;	$(CC) $(CFLAGS) -c -o $@ src/strncmp.c

$Bdouble$O:	examples/double.c;	$(CC) $(CFLAGS) -c -o $@ examples/double.c
$Bcalc$O:	examples/calc.c;	$(CC) $(CFLAGS) -c -o $@ examples/calc.c
$Bids$O:	examples/ids.c;		$(CC) $(CFLAGS) -c -o $@ examples/ids.c
$Bmpcalc$O:	examples/mpcalc.c;	$(CC) $(CFLAGS) -c -o $@ examples/mpcalc.c
$Biref$O:	examples/iref.c;	$(CC) $(CFLAGS) -c -o $@ examples/iref.c
$Bkref$O:	examples/kref.c;	$(CC) $(CFLAGS) -c -o $@ examples/kref.c
$Bidents$O:	examples/idents.c;	$(CC) $(CFLAGS) -c -o $@ examples/idents.c
$Bwords$O:	examples/words.c;	$(CC) $(CFLAGS) -c -o $@ examples/words.c
$Bbasename$O:	examples/basename.c;	$(CC) $(CFLAGS) -c -o $@ examples/basename.c
$Bwf$O:		examples/wf.c;		$(CC) $(CFLAGS) -c -o $@ examples/wf.c
$Bgetword$O:	examples/getword.c;	$(CC) $(CFLAGS) -c -o $@ examples/getword.c
$Bxref$O:	examples/xref.c;	$(CC) $(CFLAGS) -c -o $@ examples/xref.c
$Binteger$O:	examples/integer.c;	$(CC) $(CFLAGS) -c -o $@ examples/integer.c
$Bcref$O:	examples/cref.c;	$(CC) $(CFLAGS) -c -o $@ examples/cref.c
$Bsort$O:	examples/sort.c;	$(CC) $(CFLAGS) -c -o $@ examples/sort.c
$Bspin$O:	examples/spin.c;	$(CC) $(CFLAGS) -c -o $@ examples/spin.c
$Bsieve$O:	examples/sieve.c;	$(CC) $(CFLAGS) -c -o $@ examples/sieve.c

$Bwf$O $Bxref$O $Bgetword$O:	examples/getword.h
$Bcref$O $Binteger$O:		examples/integer.h

$Bdouble$E:	$Bdouble$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bdouble$O $Blibcii$A
$Bcalc$E:	$Bcalc$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bcalc$O $Blibcii$A
$Bids$E:	$Bids$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bids$O $Blibcii$A
$Bmpcalc$E:	$Bmpcalc$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bmpcalc$O $Blibcii$A
$Biref$E:	$Biref$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Biref$O $Blibcii$A
$Bkref$E:	$Bkref$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bkref$O $Blibcii$A
$Bidents$E:	$Bidents$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bidents$O $Blibcii$A
$Bwords$E:	$Bwords$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bwords$O $Blibcii$A
$Bbasename$E:	$Bbasename$O $Blibcii$A;	$(LD) $(LDFLAGS) -o $@ $Bbasename$O $Blibcii$A
$Bdirname$E:	$Bbasename$O $Blibcii$A;	$(LD) $(LDFLAGS) -o $@ $Bbasename$O $Blibcii$A
$Bwf$E:		$Bwf$O $Bgetword$O $Blibcii$A;	$(LD) $(LDFLAGS) -o $@ $Bwf$O $Bgetword$O $Blibcii$A
$Bxref$E:	$Bxref$O $Bgetword$O $Blibcii$A;$(LD) $(LDFLAGS) -o $@ $Bxref$O $Bgetword$O $Blibcii$A
$Bcref$E:	$Bcref$O $Binteger$O $Blibcii$A;$(LD) $(LDFLAGS) -o $@ $Bcref$O $Binteger$O $Blibcii$A
$Bsort$E:	$Bsort$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bsort$O $Blibcii$A
$Bspin$E:	$Bspin$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bspin$O $Blibcii$A
$Bsieve$E:	$Bsieve$O $Blibcii$A;		$(LD) $(LDFLAGS) -o $@ $Bsieve$O $Blibcii$A

maxalign:	$Bmaxalign$E
		$Bmaxalign$E

$Bmaxalign$E:	misc/maxalign.c
		$(CC) -o $@ misc/maxalign.c

clean::
		$(RM) $B*$O
		$(RM) $(EXAMPLES)

clobber::	clean
		$(RM) $Blibcii$A



# DO NOT DELETE THIS LINE -- make depend depends on it.

$Bap$O:		$I/assert.h $I/except.h $I/ap.h $I/fmt.h $I/xp.h $I/mem.h
$Barena$O:	$I/assert.h $I/except.h $I/arena.h
$Barith$O:	$I/arith.h
$Barray$O:	$I/assert.h $I/except.h $I/array.h $I/arrayrep.h $I/mem.h
$Bassert$O:	$I/assert.h $I/except.h
$Batom$O:	$I/atom.h $I/assert.h $I/except.h $I/mem.h
$Bbit$O:	$I/assert.h $I/except.h $I/bit.h $I/mem.h
$Bchan$O:	$I/assert.h $I/except.h $I/mem.h $I/chan.h $I/sem.h
$Bexcept$O:	$I/assert.h $I/except.h
$Bfmt$O:	$I/assert.h $I/except.h $I/fmt.h $I/mem.h
$Blist$O:	$I/assert.h $I/except.h $I/mem.h $I/list.h
$Bmem$O:	$I/assert.h $I/except.h $I/mem.h
$Bmemchk$O:	$I/assert.h $I/except.h $I/mem.h
$Bmp$O:		$I/assert.h $I/except.h $I/fmt.h $I/mem.h $I/xp.h $I/mp.h
$Bring$O:	$I/assert.h $I/except.h $I/ring.h $I/mem.h
$Bseq$O:	$I/assert.h $I/except.h $I/seq.h $I/array.h $I/arrayrep.h $I/mem.h
$Bset$O:	$I/mem.h $I/except.h $I/assert.h $I/arith.h $I/set.h
$Bstack$O:	$I/assert.h $I/except.h $I/mem.h $I/stack.h
$Bstr$O:	$I/assert.h $I/except.h $I/fmt.h $I/str.h $I/mem.h
$Btable$O:	$I/mem.h $I/except.h $I/assert.h $I/table.h
$Btext$O:	$I/assert.h $I/except.h $I/fmt.h $I/text.h $I/mem.h
$Bthread$O:	$I/assert.h $I/except.h $I/mem.h $I/thread.h $I/sem.h
$Bthread-nt$O:	$I/assert.h $I/except.h $I/mem.h $I/thread.h $I/sem.h
$Bxp$O:		$I/assert.h $I/except.h $I/xp.h
