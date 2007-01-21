CC	= lcc -A
CFLAGS	= -g -I.
INCLUDEDIR=/cmnusr/local/include/cii
LIBDIR	= /usr/local/lib
OBJS	= ap.o arena.o arith.o array.o assert.o atom.o bit.o chan.o except.o \
	fmt.o list.o mem.o memmove.o mp.o ring.o seq.o set.o stack.o str.o \
	table.o text.o xp.o thread.o swtch.o
HEADERS	= ap.h arena.h arith.h array.h arrayrep.h assert.h atom.h chan.h \
	except.h fmt.h list.h mem.h mp.h seq.h set.h stack.h sem.h str.h \
	table.h text.h thread.h xp.h

libcii.a:	$(OBJS)
		ar r $@ $?
		ranlib $@

install:	installlib installheaders

installheaders:	$(HEADERS)
		if [ ! -d $(INCLUDEDIR) ]; then mkdir $(INCLUDEDIR); fi
		cp -p $(HEADERS) $(INCLUDEDIR)

installlib:	libcii.a memchk.o
		cp libcii.a $(LIBDIR)
		ranlib $(LIBDIR)/libcii.a

swtch.o:	swtch.s
		/lib/cpp -P swtch.s >swtch.i
		as -o $@ swtch.i && rm -f swtch.i

depend:
		makedepend -w1000 -I/usr/local/include/ansi *.c
		sed -e '/^# DO NOT/,$$s/\/usr[a-z\/.]*\.h//g' \
		    -e '/^# DO NOT/,$$s/  [ ]*/ /g' <makefile >tmp
		mv tmp makefile

clean:
		rm -f *.o

clobber:	clean
		rm -f libcii.a

# DO NOT DELETE THIS LINE -- make depend depends on it.

ap.o: assert.h except.h ap.h fmt.h xp.h mem.h 
arena.o: assert.h except.h arena.h 
arith.o: arith.h
array.o: assert.h except.h array.h arrayrep.h mem.h
assert.o: assert.h except.h 
atom.o: atom.h assert.h except.h mem.h 
bit.o: assert.h except.h bit.h mem.h
chan.o: assert.h except.h chan.h sem.h
except.o: except.h 
fmt.o: assert.h except.h fmt.h mem.h 
foo.o: mp.h except.h 
list.o: assert.h except.h mem.h list.h
mem.o: assert.h except.h mem.h 
memchk.o: assert.h except.h mem.h 
mp.o: assert.h except.h fmt.h mem.h xp.h mp.h
ring.o: assert.h except.h ring.h mem.h
seq.o: assert.h except.h seq.h array.h arrayrep.h mem.h
set.o: mem.h except.h assert.h arith.h set.h
stack.o: assert.h except.h mem.h stack.h
str.o: assert.h except.h str.h mem.h 
table.o: mem.h except.h assert.h table.h
text.o: assert.h except.h text.h mem.h 
thread.o: assert.h except.h thread.h sem.h
xp.o: assert.h except.h xp.h
