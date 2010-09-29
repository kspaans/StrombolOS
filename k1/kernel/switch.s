	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	str	lr, [sp, #-4]!	@ writes-back decr'd value of sp
	swi	#0
	ldr	lr, [sp], #4	@ also performs a write-back to sp
	mov	pc, lr
	.size	swtch, .-swtch
	.text
	.align  2
	.global print_mode
	.type   print_mode, %function
print_mode:
	str	lr, [sp, #-4]!;
	mov	r0, #1
	mrs	r1, CPSR
	bic	r1, r1, #0xFFFFFFE0 @ lol, turned into AND
	bl	bwputr(PLT)
	ldr	pc, [sp], #4
	.size	print_mode, .-print_mode
	.text
	.align  2
	.global activate_lower
	.type   activate_lower, %function
activate_lower:
	@@
	@@ r0 is pointer to task descriptor
	@@
	@ Save kernel state
	stmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	sub	sp, sp, #60
	
	@ restore user state, we get the CORRECT stack pointer from the TD
        ldr	r2, [r0, #4] @ pull sp out of td

        @ Change to user mode
	mrs	r3, CPSR
	bic	r3, r3, #0x1F
	orr	r3, r3, #0x10
        msr     CPSR_c, r3

	@ Restore the sp - ugliness because of register banking
	mov	sp, r2

	@ Now restore user state...
	ldmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}

	@ Jump into the user
	mov	pc, lr
.HANDLE:
	@ We return to the kernel  H E R E
	@ Change to server mode || what about SPSR stuff?
	stmfd	sp!, {r4}

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
	ldmfd	sp!, {r4}
	str	r4, [r5, #16]
	@ User's state now successfully saved

	@ Restore kernel state
	ldmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}

	@ Save r4 for the purpose of getting the user's SP
	stmfd	sp!, {r4}

	@ Go back to server mode to save the user's stack pointer
	mrs	r4, CPSR
	orr	r4, r4, #0x1F
	msr     CPSR_c, r4

	@ r0 has the pointer to the TD
	@str	sp, [r0, #4]
	mov	r0, sp

	@ Back to supervisor mode
	mrs	r4, CPSR
	bic	r4, r4, #0x1F
	orr	r4, r4, #0x13
	msr     CPSR_c, r4

	@ Restore the kernel's real r4
	ldmfd	sp!, {r4}

	@ grab user arguments
	mov	pc, lr
	.size	activate_lower, .-activate_lower
	.text
	.align	2
	.global	install_handler
	.type	install_handler, %function
install_handler:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	stmfd	sp!, {r0, r1}
	MOV	r1, #0x28
	ldr	r0, .L1
	STR	r0, [r1]
	ldmfd	sp!, {r0, r1}
	mov	pc, lr
	.align 2
.L1:
	.word	.HANDLE
	.size	install_handler, .-install_handler
	.ident	"GCC: (GNU) 4.0.2"
