	@ This is probably overkill for trying to shove the correct value into
	@ SWI's argument... Maybe just always call ``swi 0''?
	@ This is essentially a large switch statement.
	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #12
	@ lr needed for prologue
	str	r0, [sp, #8]
	ldr	r3, [sp, #8]
	str	r3, [sp, #0]
	ldr	r3, [sp, #0]

	cmp	r3, #0
	beq	RET0
	cmp	r3, #1
	beq	RET1
	cmp	r3, #2
	beq	RET2
	cmp	r3, #3
	beq	RET3
	cmp	r3, #4
	beq	RET4
	b	.L2

RET0:
	swi	#0
	b	EGRESS
RET1:
	swi	#1
	b	EGRESS
RET2:
	swi	#2
	b	EGRESS
RET3:
	swi	#3
	b	EGRESS
RET4:
	swi	#4
	b	EGRESS
.L2:
	swi	#42

	@ Return value should already be in r0
EGRESS:
	add	sp, sp, #12
	bx	lr
	.size	swtch, .-swtch
	.ident	"GCC: (GNU) 4.0.2"
