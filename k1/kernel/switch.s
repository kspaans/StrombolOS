	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	stmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	sub	sp, sp, #60

	@mov	r5, #0x28
	@str	r1, [r5]	@ put the function pointer into interrupt handler

	swi	#0

	@ Return value should be properly gotten
	ldmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	mov	pc, lr
	.size	swtch, .-swtch
	.ident	"GCC: (GNU) 4.0.2"
