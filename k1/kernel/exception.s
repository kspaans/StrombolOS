	.section	.rodata
	.align	2
.LC0:
	.ascii	"Me handler?\012\000"
	.text
	.align	2
	.global	handle
	.type	handle, %function
handle:
	@ Change to server mode || what about SPSR stuff?
	str	r4, [sp, #0]!

	mrs	r4, CPSR
	orr	r4, r4, #0x1F
	msr     CPSR_c, r4


	@ Save the user state, but r4 is dirty
	@                             V
	stmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	sub	sp, sp, #60
	mov	r5, sp

	@ Back to svc mode to restore kernel state, also kernel PSR?
	mrs	r4, CPSR
	bic	r4, r4, #0x1F
	orr	r4, r4, #0x13
	msr     CPSR_c, r4

	@ Restore the user's correct r4
	ldr	r4, [sp, #0]!
	str	r4, [r5, #16]

	@ SAVE USER STACK POINTER FOR UPDATING TD???

	@ User's state now successfully saved
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	@ Restore kernel state
	ldmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}

	@ grab arguments, make kernel do stuff!
	@@bl	fuck @ fun for a test?
	mov	pc, lr
.L4:
	.align	2
.L3:
	.word	.LC0
	.size	handle, .-handle
	.align	2
	.global	install_handler
	.type	install_handler, %function
install_handler:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #4
	@ lr needed for prologue
	str	r0, [sp, #0]
	MOV	r1, #0x28
	STR	r0, [r1]
	add	sp, sp, #4
	bx	lr
	.size	install_handler, .-install_handler
	.ident	"GCC: (GNU) 4.0.2"
