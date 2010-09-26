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
	@ lr needed for prologue
	stmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	sub	sp, sp, #60

	mov	r5, #0x28
	str	r1, [r5]	@ put the function pointer into interrupt handler

	swi	#0

	@ Return value should already be in r0
	ldmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	mov	pc, lr
	.size	swtch, .-swtch
	.ident	"GCC: (GNU) 4.0.2"
