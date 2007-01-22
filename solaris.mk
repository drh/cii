CC=cc -DMAXALIGN=8
BUILDDIR=../sparc-solaris
THREADS=

all::

clean::
	(cd $(BUILDDIR); rm -fr ap arith array bit list memchk mp ring set str text xp)
