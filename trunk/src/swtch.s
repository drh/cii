#if alpha
.globl	_swtch
.ent	_swtch
_swtch:	lda	$sp,-112($sp)	# allocate _swtch's frame
	.frame	$sp,112,$26
	.fmask	0x3f0000,-112
	stt	$f21,0($sp)	# save from's registers
	stt	$f20,8($sp)
	stt	$f19,16($sp)
	stt	$f18,24($sp)
	stt	$f17,32($sp)
	stt	$f16,40($sp)
	.mask	0x400fe00,-64
	stq	$26,48+0($sp)
	stq	$15,48+8($sp)
	stq	$14,48+16($sp)
	stq	$13,48+24($sp)
	stq	$12,48+32($sp)
	stq	$11,48+40($sp)
	stq	$10,48+48($sp)
	stq	$9,48+56($sp)
	.prologue 0
	stq	$sp,0($16)	# save from's stack pointer
	ldq	$sp,0($17)	# restore to's stack pointer
	ldt	$f21,0($sp)	# restore to's registers
	ldt	$f20,8($sp)
	ldt	$f19,16($sp)
	ldt	$f18,24($sp)
	ldt	$f17,32($sp)
	ldt	$f16,40($sp)
	ldq	$26,48+0($sp)
	ldq	$15,48+8($sp)
	ldq	$14,48+16($sp)
	ldq	$13,48+24($sp)
	ldq	$12,48+32($sp)
	ldq	$11,48+40($sp)
	ldq	$10,48+48($sp)
	ldq	$9,48+56($sp)
	lda	$sp,112($sp)	# deallocate frame
	ret	$31,($26)
.end	_swtch
.globl	_start
.ent	_start
_start:	.frame	$sp,0,$26
	.mask	0x0,0
	.prologue 0
	mov	$14,$16	# register 14 holds args
	mov	$15,$27	# register 15 holds apply
	jsr	$26,($27)	# call apply
	ldgp	$26,0($26)	# reload the global pointer
	mov	$0,$16	# Thread_exit(apply(args))
	mov	$13,$27	# register 13 has Thread_exit
	jsr	$26,($27)
	call_pal	0
.end	_start
.globl	_ENDMONITOR
_ENDMONITOR:
#elif sparc
	.global	__swtch
	.align	4
	.proc	4
	__swtch:	save	%sp,-(8+64),%sp
	st	%fp,[%sp+64+0]	! save from's frame pointer
	st	%i7,[%sp+64+4]	! save from's return address
	ta	3	! flush from's registers
	st	%sp,[%i0]	! save from's stack pointer
	ld	[%i1],%sp	! load to's stack pointer
	ld	[%sp+64+0],%fp	! restore to's frame pointer
	ld	[%sp+64+4],%i7	! restore to's return address
	ret	! continue execution of to
	restore
	.global	__start
	.align	4
	.proc	4
	__start:	ld	[%sp+64+4],%o0
	ld	[%sp+64],%o1
	call	%o1; nop
	call	_Thread_exit; nop
	unimp	0
	.global __ENDMONITOR
	__ENDMONITOR:
#elif mips
.text
.globl	_swtch
.align	2
.ent	_swtch
.set	reorder
_swtch:	.frame	$sp,88,$31
	subu	$sp,88
	.fmask	0xfff00000,-48
	s.d	$f20,0($sp)
	s.d	$f22,8($sp)
	s.d	$f24,16($sp)
	s.d	$f26,24($sp)
	s.d	$f28,32($sp)
	s.d	$f30,40($sp)
	.mask	0xc0ff0000,-4
	sw	$16,48+0($sp)
	sw	$17,48+4($sp)
	sw	$18,48+8($sp)
	sw	$19,48+12($sp)
	sw	$20,48+16($sp)
	sw	$21,48+20($sp)
	sw	$22,48+24($sp)
	sw	$23,48+28($sp)
	sw	$30,48+32($sp)
	sw	$31,48+36($sp)
	sw	$sp,0($4)
	lw	$sp,0($5)
	l.d	$f20,0($sp)
	l.d	$f22,8($sp)
	l.d	$f24,16($sp)
	l.d	$f26,24($sp)
	l.d	$f28,32($sp)
	l.d	$f30,40($sp)
	lw	$16,48+0($sp)
	lw	$17,48+4($sp)
	lw	$18,48+8($sp)
	lw	$19,48+12($sp)
	lw	$20,48+16($sp)
	lw	$21,48+20($sp)
	lw	$22,48+24($sp)
	lw	$23,48+28($sp)
	lw	$30,48+32($sp)
	lw	$31,48+36($sp)
	addu	$sp,88
	j	$31
.globl	_start
_start:	move	$4,$23	# register 23 holds args
	move	$25,$30	# register 30 holds apply
	jal	$25
	move	$4,$2	# Thread_exit(apply(p))
	move	$25,$21	# register 21 holds Thread_exit
	jal	$25
	syscall
.end	_swtch
.globl	_ENDMONITOR
_ENDMONITOR:
#elif (linux || __APPLE__) && i386
.align	4
#if __APPLE__
.globl	__swtch
__swtch:
#else
.globl	_swtch
_swtch:
#endif
	subl	$16,%esp
	movl	%ebx,0(%esp)
	movl	%esi,4(%esp)
	movl	%edi,8(%esp)
	movl	%ebp,12(%esp)
	movl	20(%esp),%eax
	movl	%esp,0(%eax)
	movl	24(%esp),%eax
	movl	0(%eax),%esp
	movl	0(%esp),%ebx
	movl	4(%esp),%esi
	movl	8(%esp),%edi
	movl	12(%esp),%ebp
	addl	$16,%esp
	ret
.align	4
.globl	__thrstart
.globl	_thrstart
__thrstart:
_thrstart:
	pushl	%edi		# stack is 16-byte aligned after this push
	call	*%esi
	subl	$12,%esp	# ensure stack is 16-byte aligned before the call
	pushl	%eax
#if __APPLE__
	call	_Thread_exit
#else
	call	Thread_exit
#endif
.globl	__ENDMONITOR
.globl	_ENDMONITOR
__ENDMONITOR:
_ENDMONITOR:
#else
Unsupported platform
#endif
