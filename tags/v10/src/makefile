CC	= lcc -A
I	= ../include
CPPFLAGS=
CFLAGS	= -g -I$(I)
OBJS	= ap.o arena.o arith.o array.o assert.o atom.o bit.o chan.o except.o \
	fmt.o list.o mem.o mp.o ring.o seq.o set.o stack.o str.o \
	table.o text.o xp.o thread.o swtch.o \
	memcmp.o memmove.o strncmp.o strerror.o

libcii.a:	$(OBJS)
		ar r $@ $?
		ranlib $@

swtch.o:	swtch.s
		/lib/cpp -P $(CPPFLAGS) swtch.s >swtch.i
		as -o $@ swtch.i && rm -f swtch.i

depend:
		makedepend -w1000 -I$(I) *.c
		sed -e '/^# DO NOT/,$$s/\/usr[a-z\/.]*\.h//g' \
		    -e '/^# DO NOT/,$$s/  [ ]*/ /g' <makefile >tmp
		mv tmp makefile

clean:
		rm -f *.o

clobber:	clean
		rm -f libcii.a

# DO NOT DELETE THIS LINE -- make depend depends on it.

ap.o: $(I)/assert.h $(I)/except.h $(I)/ap.h $(I)/fmt.h $(I)/xp.h $(I)/mem.h
arena.o: $(I)/assert.h $(I)/except.h $(I)/arena.h
arith.o: $(I)/arith.h
array.o: $(I)/assert.h $(I)/except.h $(I)/array.h $(I)/arrayrep.h $(I)/mem.h
assert.o: $(I)/assert.h $(I)/except.h $(I)/
atom.o: $(I)/atom.h $(I)/assert.h $(I)/except.h $(I)/mem.h
bit.o: $(I)/assert.h $(I)/except.h $(I)/bit.h $(I)/mem.h
chan.o: $(I)/assert.h $(I)/except.h $(I)/mem.h $(I)/chan.h $(I)/sem.h
except.o: $(I)/assert.h $(I)/except.h $(I)/
fmt.o: $(I)/assert.h $(I)/except.h $(I)/fmt.h $(I)/mem.h
list.o: $(I)/assert.h $(I)/except.h $(I)/mem.h $(I)/list.h
mem.o: $(I)/assert.h $(I)/except.h $(I)/mem.h
memchk.o: $(I)/assert.h $(I)/except.h $(I)/mem.h
mp.o: $(I)/assert.h $(I)/except.h $(I)/fmt.h $(I)/mem.h $(I)/xp.h $(I)/mp.h $(I)/
ring.o: $(I)/assert.h $(I)/except.h $(I)/ring.h $(I)/mem.h
seq.o: $(I)/assert.h $(I)/except.h $(I)/seq.h $(I)/array.h $(I)/arrayrep.h $(I)/mem.h
set.o: $(I)/mem.h $(I)/except.h $(I)/assert.h $(I)/arith.h $(I)/set.h
stack.o: $(I)/assert.h $(I)/except.h $(I)/mem.h $(I)/stack.h
str.o: $(I)/assert.h $(I)/except.h $(I)/fmt.h $(I)/str.h $(I)/mem.h
table.o: $(I)/mem.h $(I)/except.h $(I)/assert.h $(I)/table.h
text.o: $(I)/assert.h $(I)/except.h $(I)/fmt.h $(I)/text.h $(I)/mem.h
thread.o: $(I)/assert.h $(I)/except.h $(I)/mem.h $(I)/thread.h $(I)/sem.h
xp.o: $(I)/assert.h $(I)/except.h $(I)/xp.h
